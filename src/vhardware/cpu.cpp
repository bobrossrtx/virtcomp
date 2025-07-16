#include <iomanip>
#include <chrono>
#include <thread>
#include <ctime>
#include <sstream>
#include <unordered_set>
#include <fmt/core.h>

#include "cpu.hpp"
#include "cpu_flags.hpp"
#include "opcodes/opcode_dispatcher.hpp"

// Constants for CPU configuration
constexpr size_t CPU_REGISTER_COUNT = 8;
constexpr size_t CPU_MEMORY_SIZE = 256;
const uint32_t INVALID_ADDR = static_cast<uint32_t>(-1);

// Standalone function to compute valid instruction starts
std::unordered_set<size_t> compute_valid_instruction_starts(const std::vector<uint8_t>& program) {
    std::unordered_set<size_t> starts;
    size_t pc = 0;
    while (pc < program.size()) {
        starts.insert(static_cast<size_t>(pc));
        Opcode opcode = static_cast<Opcode>(program[pc]);
        switch (opcode) {
            case Opcode::LOAD_IMM:
            case Opcode::ADD:
            case Opcode::SUB:
            case Opcode::MOV:
            case Opcode::LOAD:
            case Opcode::STORE:
            case Opcode::CMP:
            case Opcode::MUL:
            case Opcode::DIV:
            case Opcode::AND:
            case Opcode::OR:
            case Opcode::XOR:
            case Opcode::SHL:
            case Opcode::SHR:
            case Opcode::IN:
            case Opcode::OUT:
            case Opcode::INB:
            case Opcode::OUTB:
            case Opcode::INW:
            case Opcode::OUTW:
            case Opcode::INL:
            case Opcode::OUTL:
            case Opcode::INSTR:
            case Opcode::OUTSTR:
                pc += 3;
                break;
            case Opcode::JMP:
            case Opcode::JZ:
            case Opcode::JNZ:
            case Opcode::JS:
            case Opcode::JNS:
            case Opcode::JC:
            case Opcode::JNC:
            case Opcode::JO:
            case Opcode::JNO:
            case Opcode::PUSH:
            case Opcode::POP:
            case Opcode::INC:
            case Opcode::DEC:
            case Opcode::NOT:
            case Opcode::CALL:
            case Opcode::PUSH_ARG:
            case Opcode::POP_ARG:
                pc += 2;
                break;
            case Opcode::HALT:
            case Opcode::NOP:
            case Opcode::RET:
            case Opcode::PUSH_FLAG:
            case Opcode::POP_FLAG:
                pc += 1;
                break;
            case Opcode::DB:
                if (pc + 2 < program.size()) {
                    uint8_t length = program[pc + 2];
                    pc += 3 + length;
                } else {
                    pc += 1;
                }
                break;
            default:
                pc += 1;
                break;
        }
    }
    return starts;
}

// CPU constructor
CPU::CPU()
    : registers(CPU_REGISTER_COUNT, 0), memory(CPU_MEMORY_SIZE, 0), pc(0), sp(CPU_MEMORY_SIZE), flags(0), last_accessed_addr(INVALID_ADDR), last_modified_addr(INVALID_ADDR) {} // Stack grows down from top of memory

CPU::~CPU() = default;

// Reset the CPU state
void CPU::reset() {
    std::fill(registers.begin(), registers.end(), 0);
    std::fill(memory.begin(), memory.end(), 0); // Clear memory
    pc = 0;
    sp = memory.size();
    fp = sp;
    flags = 0;
    arg_offset = 0; // Initialize arg_offset for PUSH_ARG/POP_ARG operations
    last_accessed_addr = INVALID_ADDR; // Clear highlight
    last_modified_addr = INVALID_ADDR; // Clear highlight
}

// Print the CPU state (debugging information)
void CPU::print_state(const std::string& info) const {
    // If debug is not enabled, do not print the state
    if (!Config::debug) return;

    // Get current time
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    std::ostringstream pc_ss;
    pc_ss << "PC=0x" << std::setw(4) << std::setfill('0') << std::hex << std::uppercase << pc;
    oss << "[" << pc_ss.str() << "] "
        << std::right << std::setw(10) << std::setfill(' ') << std::string("(" + info + ")") << " │ ";
    for (size_t i = 0; i < registers.size(); ++i) {
        oss << "R" << i << "=0x" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(registers[i]) << " ";
    }
    oss << "SP=0x" << std::setw(3) << std::setfill('0') << std::hex << std::uppercase << sp << " ";
    oss << "FLAGS=0x" << std::setw(8) << std::setfill('0') << std::hex << std::uppercase << flags;

    Logger::instance().debug() << oss.str() << std::endl;
}

void CPU::print_stack_frame(const std::string& label) const {
    // If debug is not enabled, do not print the state
    if (!Config::debug) return;

    Logger::instance().debug() << fmt::format(
        "[{:>12}] │ FP=0x{:X} SP=0x{:X} arg_offset={}",
        label, fp, sp, arg_offset
    ) << std::endl;
}

// Fetches the next byte as an operand and advances PC
uint8_t CPU::fetch_operand() {
    if (pc + 1 >= memory.size()) {
        Logger::instance().debug() << "[FETCH_OPERAND] Out of bounds access at PC=" << pc << std::endl;
        return 0;
    }
    uint8_t operand = memory[pc + 1];
    Logger::instance().debug() << fmt::format("[FETCH_OPERAND]{:>8}│ PC={} operand={}", "", pc, static_cast<int>(operand)) << std::endl;
    pc++;
    return operand;
}

// Reads a 32-bit value from memory at the given address (little-endian)
uint32_t CPU::read_mem32(uint32_t addr) const {
    if (addr + 3 >= memory.size()) {
        Logger::instance().debug() << "[READ_MEM32] Out of bounds access at addr=" << addr << std::endl;
        return 0;
    }
    last_accessed_addr = addr;
    return (static_cast<uint32_t>(memory[addr])      ) |
           (static_cast<uint32_t>(memory[addr + 1]) << 8 ) |
           (static_cast<uint32_t>(memory[addr + 2]) << 16) |
           (static_cast<uint32_t>(memory[addr + 3]) << 24);
}

// Writes a 32-bit value to memory at the given address (little-endian)
void CPU::write_mem32(uint32_t addr, uint32_t value) {
    if (addr + 3 >= memory.size()) {
        Logger::instance().debug() << "[WRITE_MEM32] Out of bounds access at addr=" << addr << std::endl;
        return;
    }
    last_modified_addr = addr;
    memory[addr    ] = static_cast<uint8_t>(value      );
    memory[addr + 1] = static_cast<uint8_t>(value >> 8 );
    memory[addr + 2] = static_cast<uint8_t>(value >> 16);
    memory[addr + 3] = static_cast<uint8_t>(value >> 24);
}

void CPU::execute(const std::vector<uint8_t>& program) {
    // Copy program into memory
    std::copy(program.begin(), program.end(), memory.begin());
    pc = 0;
    sp = memory.size() - 4; // Stack pointer starts at the end of memory
    fp = sp;
    bool running = true;

    while (pc < program.size() && running) {
        // Use the new opcode dispatcher
        dispatch_opcode(*this, program, running);
    }
}

void CPU::print_registers() const {
    std::ostringstream oss;
    oss << std::right << std::setw(24) << "Registers " << "│ ";
    for (size_t i = 0; i < registers.size(); ++i) {
        oss << "R" << i << "=0x" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(registers[i]) << " ";
    }
    oss << "SP=0x" << std::setw(3) << std::setfill('0') << std::hex << std::uppercase << sp << " ";
    oss << "FLAGS=0x" << std::setw(8) << std::setfill('0') << std::hex << std::uppercase << flags;

    Logger::instance().info() << oss.str() << std::endl;
}

void CPU::print_memory(std::size_t start, std::size_t end) const {
    std::ostringstream oss;
    oss << std::right << std::setw(24) << "Memory " << "│ ";
    for (std::size_t i = start; i < end && i < memory.size(); ++i) {
        if (i == last_accessed_addr) oss << "[A:" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(memory[i]) << "] ";
        else if (i == last_modified_addr) oss << "[M:" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(memory[i]) << "] ";
        else oss << "[" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(memory[i]) << "] ";
    }

    Logger::instance().info() << oss.str() << std::endl;
}

void CPU::run(const std::vector<uint8_t>& program) {
    reset();
    execute(program);
}

bool CPU::step(const std::vector<uint8_t>& program) {
    // Copy program into memory if first step and memory is empty
    if (pc == 0 && memory[0] == 0 && !program.empty()) {
        std::copy(program.begin(), program.end(), memory.begin());
        sp = memory.size() - 4;
        fp = sp;
    }

    if (pc >= program.size()) {
        return false; // Program ended
    }

    bool running = true;
    dispatch_opcode(*this, program, running);

    return running;
}

uint8_t CPU::readPort(uint8_t port) {
    return vhw::DeviceManager::instance().readPort(port);
}

void CPU::writePort(uint8_t port, uint8_t value) {
    vhw::DeviceManager::instance().writePort(port, value);
}

std::string CPU::readPortString(uint8_t port, uint8_t maxLength) {
    return vhw::DeviceManager::instance().readPortString(port, maxLength);
}

void CPU::writePortString(uint8_t port, const std::string& str) {
    vhw::DeviceManager::instance().writePortString(port, str);
}
