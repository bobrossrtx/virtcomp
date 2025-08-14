#include <iomanip>
#include <chrono>
#include <thread>
#include <ctime>
#include <sstream>
#include <unordered_set>
#include <fmt/core.h>

#include "cpu.hpp"
#include "cpu_flags.hpp"
#include "cpu_registers.hpp"  // Include the new register system
#include "opcodes/opcode_dispatcher.hpp"

using namespace DemiEngine_Registers;

// Constants for CPU configuration
constexpr size_t CPU_LEGACY_REGISTER_COUNT = 8;  // For backward compatibility
constexpr size_t CPU_DEFAULT_MEMORY_SIZE = 1024 * 1024; // 1MB default (massive increase from 256 bytes)
constexpr size_t CPU_TEST_MEMORY_SIZE = 256; // Maintain old size for test compatibility
constexpr size_t CPU_MIN_MEMORY_SIZE = 256; // Allow 256 bytes minimum for test compatibility
constexpr size_t CPU_MAX_MEMORY_SIZE = 64 * 1024 * 1024; // 64MB maximum for performance
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
            case Opcode::ADD64:
            case Opcode::SUB64:
            case Opcode::MOV64:
            case Opcode::MUL64:
            case Opcode::DIV64:
            case Opcode::AND64:
            case Opcode::OR64:
            case Opcode::XOR64:
            case Opcode::SHL64:
            case Opcode::SHR64:
            case Opcode::CMP64:
            case Opcode::MOVEX:
            case Opcode::ADDEX:
            case Opcode::SUBEX:
            case Opcode::MULEX:
            case Opcode::DIVEX:
            case Opcode::CMPEX:
            case Opcode::LOADEX:
            case Opcode::STOREX:
            case Opcode::PUSHEX:
            case Opcode::POPEX:
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
            case Opcode::MODE32:
            case Opcode::MODE64:
                pc += 1;
                break;
            case Opcode::LOAD_IMM64:
                pc += 10; // opcode + reg + 8-byte immediate
                break;
            case Opcode::MODECMP:
                pc += 2; // opcode + mode value
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
CPU::CPU(size_t memory_size)
    : cpu_mode(CPUMode::MODE_32BIT), // Default to 32-bit mode for backward compatibility
      registers(TOTAL_REGISTERS, 0), legacy_registers(CPU_LEGACY_REGISTER_COUNT, 0) {

    // Determine actual memory size to use
    if (memory_size == 0) {
        memory_size = CPU_DEFAULT_MEMORY_SIZE; // Use default 1MB
    }

    // Validate memory size bounds
    if (memory_size < CPU_MIN_MEMORY_SIZE) {
        Logger::instance().warn() << fmt::format(
            "Memory size {} bytes is below minimum {}, using minimum",
            memory_size, CPU_MIN_MEMORY_SIZE) << std::endl;
        memory_size = CPU_MIN_MEMORY_SIZE;
    }
    if (memory_size > CPU_MAX_MEMORY_SIZE) {
        Logger::instance().warn() << fmt::format(
            "Memory size {} bytes exceeds maximum {}, using maximum",
            memory_size, CPU_MAX_MEMORY_SIZE) << std::endl;
        memory_size = CPU_MAX_MEMORY_SIZE;
    }

    // Initialize memory
    memory.resize(memory_size, 0);

    // Initialize special registers
    registers[static_cast<size_t>(Register::RIP)] = 0;  // Program counter
    registers[static_cast<size_t>(Register::RSP)] = memory.size();  // Stack pointer
    registers[static_cast<size_t>(Register::RBP)] = memory.size();  // Frame pointer
    registers[static_cast<size_t>(Register::RFLAGS)] = 0;  // Flags

    arg_offset = 0;
    last_accessed_addr = INVALID_ADDR;
    last_modified_addr = INVALID_ADDR;

    // Sync legacy registers for backward compatibility
    sync_legacy_registers();

    Logger::instance().info() << fmt::format(
        "Virtual CPU initialized with {} bytes ({:.1f}MB) of memory and {} total registers",
        memory.size(), memory.size() / (1024.0 * 1024.0), TOTAL_REGISTERS) << std::endl;
}

CPU::~CPU() = default;

// Extended register access methods
uint64_t CPU::get_register(Register reg) const {
    auto index = static_cast<size_t>(reg);
    if (index < TOTAL_REGISTERS) {
        return registers[index];
    }
    return 0;
}

void CPU::set_register(Register reg, uint64_t value) {
    auto index = static_cast<size_t>(reg);
    if (index < TOTAL_REGISTERS) {
        uint64_t old_value = registers[index];
        registers[index] = value;
        sync_legacy_registers(); // Keep legacy registers in sync

        // Print register update if value changed
        if (old_value != value) {
            print_register_update(reg, old_value, value);
        }
    }
}

// Get register name for debugging
std::string CPU::get_register_name(Register reg) const {
    return RegisterNames::get_name(reg);
}

// Synchronize legacy 32-bit registers with new 64-bit registers (for backward compatibility)
void CPU::sync_legacy_registers() {
    for (size_t i = 0; i < CPU_LEGACY_REGISTER_COUNT && i < TOTAL_REGISTERS; ++i) {
        legacy_registers[i] = static_cast<uint32_t>(registers[i]);
    }
}

// Synchronize from legacy registers to new registers (when legacy code modifies registers)
void CPU::sync_from_legacy_registers() {
    for (size_t i = 0; i < CPU_LEGACY_REGISTER_COUNT && i < TOTAL_REGISTERS; ++i) {
        // Only update the lower 32 bits, preserve upper 32 bits
        registers[i] = (registers[i] & 0xFFFFFFFF00000000ULL) | legacy_registers[i];
    }
}

// Factory method for test compatibility - creates CPU with old memory size
CPU CPU::create_test_cpu() {
    return CPU(CPU_TEST_MEMORY_SIZE);
}

// Dynamic memory resizing
void CPU::resize_memory(size_t new_size) {
    // Validate new size bounds
    if (new_size < CPU_MIN_MEMORY_SIZE) {
        Logger::instance().warn() << fmt::format(
            "Cannot resize memory to {} bytes (below minimum {})",
            new_size, CPU_MIN_MEMORY_SIZE) << std::endl;
        return;
    }
    if (new_size > CPU_MAX_MEMORY_SIZE) {
        Logger::instance().warn() << fmt::format(
            "Cannot resize memory to {} bytes (exceeds maximum {})",
            new_size, CPU_MAX_MEMORY_SIZE) << std::endl;
        return;
    }

    size_t old_size = memory.size();
    memory.resize(new_size, 0); // Initialize new memory to zero

    // Adjust stack pointer if it's now out of bounds
    auto current_sp = static_cast<size_t>(registers[static_cast<size_t>(Register::RSP)]);
    if (current_sp >= new_size) {
        registers[static_cast<size_t>(Register::RSP)] = new_size - 4; // Move stack pointer to valid location
        registers[static_cast<size_t>(Register::RBP)] = registers[static_cast<size_t>(Register::RSP)]; // Reset frame pointer too
    }

    Logger::instance().info() << fmt::format(
        "Memory resized from {} bytes ({:.1f}MB) to {} bytes ({:.1f}MB)",
        old_size, old_size / (1024.0 * 1024.0),
        new_size, new_size / (1024.0 * 1024.0)) << std::endl;
}

// Reset the CPU state
void CPU::reset() {
    std::fill(registers.begin(), registers.end(), 0);
    std::fill(legacy_registers.begin(), legacy_registers.end(), 0);
    std::fill(memory.begin(), memory.end(), 0); // Clear memory

    // Reset CPU mode to 32-bit for backward compatibility
    cpu_mode = CPUMode::MODE_32BIT;

    // Reset special registers
    registers[static_cast<size_t>(Register::RIP)] = 0;
    registers[static_cast<size_t>(Register::RSP)] = memory.size();
    registers[static_cast<size_t>(Register::RBP)] = memory.size();
    registers[static_cast<size_t>(Register::RFLAGS)] = 0;

    arg_offset = 0; // Initialize arg_offset for PUSH_ARG/POP_ARG operations
    last_accessed_addr = INVALID_ADDR; // Clear highlight
    last_modified_addr = INVALID_ADDR; // Clear highlight

    // Sync legacy registers
    sync_legacy_registers();
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
    pc_ss << "PC=0x" << std::setw(4) << std::setfill('0') << std::hex << std::uppercase << get_pc();
    oss << "[" << pc_ss.str() << "] "<< std::string("(" + info + ")") << " ";

    // Add CPU mode indicator
    oss << "MODE=" << (is_64bit_mode() ? "x64" : "x32") << " ";

    for (size_t i = 0; i < legacy_registers.size(); ++i) {
        oss << "R" << i << "=0x" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(legacy_registers[i]) << " ";
    }
    oss << "SP=0x" << std::setw(3) << std::setfill('0') << std::hex << std::uppercase << get_sp() << " ";
    oss << "FLAGS=0x" << std::setw(8) << std::setfill('0') << std::hex << std::uppercase << get_flags();

    Logger::instance().debug() << oss.str() << std::endl;
}

void CPU::print_stack_frame(const std::string& label) const {
    // If debug is not enabled, do not print the state
    if (!Config::debug) return;

    Logger::instance().debug() << fmt::format(
        "[{:>12}] FP=0x{:X} SP=0x{:X} arg_offset={}",
        label, get_fp(), get_sp(), arg_offset
    ) << std::endl;
}

// Fetches the next byte as an operand and advances PC
uint8_t CPU::fetch_operand() {
    if (get_pc() + 1 >= memory.size()) {
        Logger::instance().debug() << "[FETCH_OPERAND] Out of bounds access at PC=" << get_pc() << std::endl;
        return 0;
    }
    uint8_t operand = memory[get_pc() + 1];
    Logger::instance().debug() << fmt::format("[FETCH_OPERAND]{:>8} PC={} operand={}", "", get_pc(), static_cast<int>(operand)) << std::endl;
    set_pc(get_pc() + 1);
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
    set_pc(0);
    registers[static_cast<size_t>(Register::RSP)] = memory.size() - 4; // Stack pointer starts at the end of memory
    registers[static_cast<size_t>(Register::RBP)] = get_sp();
    bool running = true;

    while (get_pc() < program.size() && running) {
        // Use the new opcode dispatcher
        dispatch_opcode(*this, program, running);
    }
}

void CPU::print_registers() const {
    std::ostringstream oss;
    oss << "Registers:" << std::endl;
    for (size_t i = 0; i < legacy_registers.size(); ++i) {
        oss << "R" << i << "=0x" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(legacy_registers[i]) << " ";
    }
    oss << "SP=0x" << std::setw(3) << std::setfill('0') << std::hex << std::uppercase << get_sp() << " ";
    oss << "FLAGS=0x" << std::setw(8) << std::setfill('0') << std::hex << std::uppercase << get_flags();

    Logger::instance().info() << oss.str() << std::endl;
}

void CPU::print_register_update(Register reg, uint64_t old_value, uint64_t new_value) const {
    // Only print if debug mode and extended registers are both enabled
    if (!Config::debug || !Config::extended_registers) return;

    std::string reg_name = get_register_name(reg);
    Logger::instance().debug() << fmt::format(
        "[REG_UPDATE] {} changed: 0x{:016X} -> 0x{:016X}",
        reg_name, old_value, new_value
    ) << std::endl;
}

void CPU::print_extended_registers() const {
    std::ostringstream oss;
    oss << "Extended Registers:" << std::endl;

    // General purpose registers (RAX-R15) - 4 per line
    for (size_t i = 0; i < GENERAL_PURPOSE_COUNT; ++i) {
        if (i % 4 == 0) {
            if (i > 0) oss << std::endl;
            oss << "GP" << (i/4 + 1) << ": ";
        }
        Register reg = static_cast<Register>(i);
        std::string reg_name = get_register_name(reg);

        // Reset all formatting before register name
        oss << std::left << std::setw(3) << std::setfill(' ') << reg_name;

        // Format the hex value with proper settings
        oss << "=0x" << std::right << std::setw(16) << std::setfill('0')
            << std::hex << std::uppercase << get_register_64(reg) << " ";
    }
    oss << std::endl;

    // Special registers
    oss << "Special: ";
    oss << "RIP=0x" << std::setw(16) << std::setfill('0') << std::hex << std::uppercase << get_register_64(Register::RIP) << " ";
    oss << "RSP=0x" << std::setw(16) << std::setfill('0') << std::hex << std::uppercase << get_register_64(Register::RSP) << " ";
    oss << "RBP=0x" << std::setw(16) << std::setfill('0') << std::hex << std::uppercase << get_register_64(Register::RBP) << " ";
    oss << "RFLAGS=0x" << std::setw(16) << std::setfill('0') << std::hex << std::uppercase << get_register_64(Register::RFLAGS);

    Logger::instance().info() << oss.str() << std::endl;
}

void CPU::print_memory(std::size_t start, std::size_t end) const {
    std::ostringstream oss;
    oss << "Memory:" << std::endl;
    for (std::size_t i = start; i < end && i < memory.size(); ++i) {
        // Print address at the start of each row
        if (i % 16 == 0) {
            if (i > start) oss << std::endl;
            oss << "0x" << std::setw(8) << std::setfill('0') << std::hex << std::uppercase << i << ": ";
        }

        // Print memory value with appropriate highlight
        if (i == last_accessed_addr)
            oss << "[A:" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(memory[i]) << "] ";
        else if (i == last_modified_addr)
            oss << "[M:" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(memory[i]) << "] ";
        else
            oss << "[" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(memory[i]) << "] ";
    }

    Logger::instance().info() << oss.str() << std::endl;
}

void CPU::run(const std::vector<uint8_t>& program) {
    reset();
    execute(program);
}

bool CPU::step(const std::vector<uint8_t>& program) {
    // Copy program into memory if first step and memory is empty
    if (get_pc() == 0 && memory[0] == 0 && !program.empty()) {
        std::copy(program.begin(), program.end(), memory.begin());
        registers[static_cast<size_t>(Register::RSP)] = memory.size() - 4;
        registers[static_cast<size_t>(Register::RBP)] = get_sp();
    }

    if (get_pc() >= program.size()) {
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
