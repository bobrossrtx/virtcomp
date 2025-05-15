#include <iomanip>
#include <chrono>
#include <thread>
#include <ctime>
#include <sstream>
#include <unordered_set>
#include <fmt/core.h>

#include "cpu.hpp"
#include "../config.hpp"
#include "../debug/logger.hpp"


// Flag definitions
constexpr uint32_t FLAG_ZERO = 1 << 0;
constexpr uint32_t FLAG_SIGN = 1 << 1;

// CPU constructor
CPU::CPU()
    : registers(4, 0), memory(256, 0), pc(0), sp(256), flags(0), last_accessed_addr(static_cast<uint32_t>(-1)), last_modified_addr(static_cast<uint32_t>(-1)) {} // Stack grows down from top of memory

CPU::~CPU() = default;

// Reset the CPU state
void CPU::reset() {
    std::fill(registers.begin(), registers.end(), 0);
    std::fill(memory.begin(), memory.end(), 0); // Clear memory
    pc = 0;
    sp = memory.size();
    fp = sp;
    flags = 0;
    last_accessed_addr = static_cast<uint32_t>(-1); // Clear highlight
    last_modified_addr = static_cast<uint32_t>(-1); // Clear highlight
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
    oss << std::dec;
    Logger::instance().debug() << oss.str() << std::endl;

    // Optionally print a small memory window around SP, with date and time
    if (sp < memory.size()) {
        std::ostringstream stack_oss;
        stack_oss << std::right << std::setw(45) << std::setfill(' ') << "(Stack top) │ memory[SP..SP+3]: ";
        for (size_t i = sp; i < std::min(static_cast<size_t>(sp + 4), memory.size()); ++i) {
            stack_oss << "[" << std::setw(3) << std::setfill(' ') << i << "]="
                    << std::left << std::setw(3) << static_cast<int>(memory[i]) << std::setfill(' ');
        }
        Logger::instance().debug() << stack_oss.str() << std::endl;
    }
}

void CPU::print_stack_frame(const std::string& label) const {
    std::ostringstream oss;
    oss << "[Stack frame]" << std::right << std::setw(9) << std::string("(" + label + ")") << " │ ";
    for (size_t i = 0; i < registers.size(); ++i) {
        oss << "R" << i << ": " << registers[i] << " ";
    }
    Logger::instance().debug() << oss.str() << std::endl;
}

std::unordered_set<uint8_t> compute_valid_instruction_starts(const std::vector<uint8_t>& program) {
    std::unordered_set<uint8_t> starts;
    size_t pc = 0;
    while (pc < program.size()) {
        starts.insert(static_cast<uint8_t>(pc));
        Opcode opcode = static_cast<Opcode>(program[pc]);
        switch (opcode) {
            case Opcode::LOAD_IMM:
            case Opcode::ADD:
            case Opcode::SUB:
            case Opcode::MOV:
            case Opcode::LOAD:
            case Opcode::STORE:
            case Opcode::CMP:
                pc += 3;
                break;
            case Opcode::JMP:
            case Opcode::JZ:
            case Opcode::JNZ:
            case Opcode::JS:
            case Opcode::JNS:
            case Opcode::PUSH:
            case Opcode::POP:
                pc += 2;
                break;
            case Opcode::HALT:
            case Opcode::NOP:
                pc += 1;
                break;
            default:
                pc += 1;
                break;
        }
    }
    return starts;
}

// Fetches the next byte as an operand and advances PC
uint8_t CPU::fetch_operand() {
    uint8_t operand = memory[pc + 1];
    Logger::instance().debug() << fmt::format("[FETCH_OPERAND]{:>8}│ PC={} operand={}", "", pc, static_cast<int>(operand)) << std::endl;
    pc++;
    return operand;
}

// Reads a 32-bit value from memory at the given address (little-endian)
uint32_t CPU::read_mem32(uint32_t addr) const {
    last_accessed_addr = addr;
    if (addr + 3 < memory.size()) {
        return (static_cast<uint32_t>(memory[addr])      ) |
               (static_cast<uint32_t>(memory[addr + 1]) << 8 ) |
               (static_cast<uint32_t>(memory[addr + 2]) << 16) |
               (static_cast<uint32_t>(memory[addr + 3]) << 24);
    } else {
        // Out of bounds, return 0 (or handle error)
        return 0;
    }
}

// Writes a 32-bit value to memory at the given address (little-endian)
void CPU::write_mem32(uint32_t addr, uint32_t value) {
    last_modified_addr = addr;
    if (addr + 3 < memory.size()) {
        memory[addr    ] = static_cast<uint8_t>(value      );
        memory[addr + 1] = static_cast<uint8_t>(value >> 8 );
        memory[addr + 2] = static_cast<uint8_t>(value >> 16);
        memory[addr + 3] = static_cast<uint8_t>(value >> 24);
    }
}

void CPU::execute(const std::vector<uint8_t>& program) {
    // Copy program into memory
    std::copy(program.begin(), program.end(), memory.begin());
    pc = 0;
    sp = memory.size() - 4; // Stack pointer starts at the end of memory
    fp = sp;
    bool running = true;
    auto valid_instr_starts = compute_valid_instruction_starts(program);
    while (pc < program.size() && running) {
        Opcode opcode = static_cast<Opcode>(program[pc]);
        // // Slow down execution for debugging
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        switch (opcode) {
            case Opcode::NOP:
                Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[NOP] │ PC={}", pc, "", pc) << std::endl;
                ++pc;
                print_state("NOP");
                break;
            case Opcode::LOAD_IMM: {
                if (pc + 2 < program.size()) {
                    uint8_t reg = program[pc + 1];
                    uint8_t imm = program[pc + 2];
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>1}[LOAD_IMM] │ PC={} reg={} imm={}", pc, "", pc, reg, imm) << std::endl;
                    if (reg < registers.size()) {
                        registers[reg] = imm;
                        Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>1}[LOAD_IMM] │ Set R{} = {}", pc, "", reg, imm) << std::endl;
                    }
                    pc += 3;
                } else {
                    running = false;
                }
                print_state("LOAD_IMM");
                break;
            }
            case Opcode::ADD: {
                if (pc + 2 < program.size()) {
                    uint8_t reg1 = program[pc + 1];
                    uint8_t reg2 = program[pc + 2];
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[ADD] │ PC={} R{} += R{}", pc, "", pc, reg1, reg2) << std::endl;
                    uint8_t before = registers[reg1];
                    registers[reg1] += registers[reg2];
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[ADD] │ R{}: {} + {} = {}", pc, "", reg1, before, registers[reg2], registers[reg1]) << std::endl;
                }
                pc += 3;
                print_state("ADD");
                break;
            }
            case Opcode::SUB: {
                if (pc + 2 < program.size()) {
                    uint8_t reg1 = program[pc + 1];
                    uint8_t reg2 = program[pc + 2];
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[SUB] │ PC={} R{} -= R{}", pc, "", pc, reg1, reg2) << std::endl;
                    if (reg1 < registers.size() && reg2 < registers.size()) {
                        uint8_t before = registers[reg1];
                        registers[reg1] -= registers[reg2];
                        Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[SUB] │ R{}: {} - {} = {}", pc, "", reg1, before, registers[reg2], registers[reg1]) << std::endl;
                    }
                    pc += 3;
                } else {
                    running = false;
                }
                print_state("SUB");
                break;
            }
            case Opcode::MOV: {
                if (pc + 2 < program.size()) {
                    uint8_t reg1 = program[pc + 1];
                    uint8_t reg2 = program[pc + 2];
                    if (reg1 < registers.size() && reg2 < registers.size()) {
                        registers[reg1] = registers[reg2];
                    }
                    pc += 3;
                } else {
                    running = false;
                }
                print_state("MOV");
                break;
            }
            case Opcode::JMP: {
                if (pc + 1 < program.size()) {
                    uint8_t addr = program[pc + 1];
                    if (valid_instr_starts.find(addr) == valid_instr_starts.end()) {
                        Logger::instance().error() << std::right << std::setw(23) << std::setfill(' ')
                        << "Invalid jump address " << "│ (JMP): "
                        << static_cast<int>(addr) << " at PC=" << pc << std::endl;
                        running = false;
                        break;
                    }
                    pc = addr;
                } else {
                    running = false;
                }
                print_state("JMP");
                break;
            }
            case Opcode::LOAD: {
                // LOAD reg, addr
                if (pc + 2 < program.size()) {
                    uint8_t reg = program[pc + 1];
                    uint8_t addr = program[pc + 2];
                    if (reg < registers.size() && addr < memory.size()) {
                        registers[reg] = memory[addr];
                    }
                    pc += 3;
                } else {
                    running = false;
                }
                print_state("LOAD");
                break;
            }
            case Opcode::STORE: {
                // STORE reg, addr
                if (pc + 2 < program.size()) {
                    uint8_t reg = program[pc + 1];
                    uint8_t addr = program[pc + 2];
                    if (reg < registers.size() && addr < memory.size()) {
                        memory[addr] = registers[reg];
                    }
                    pc += 3;
                } else {
                    running = false;
                }
                print_state("STORE");
                break;
            }
            case Opcode::PUSH: {
                if (pc + 1 < program.size()) {
                    uint8_t reg = program[pc + 1];
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>5}[PUSH] │ PC={} Pushing R{}={}", pc, "", pc, static_cast<int>(reg), registers[reg]) << std::endl;
                    sp -= 4;
                    write_mem32(sp, registers[reg]);
                    pc += 2;
                } else {
                    running = false;
                }
                print_state("PUSH");
                break;
            }
            case Opcode::POP: {
                if (pc + 1 < program.size()) {
                    uint8_t reg = program[pc + 1];
                    registers[reg] = read_mem32(sp);
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[POP] │ PC={} Popping to R{}={}", pc, "", pc, static_cast<int>(reg), registers[reg]) << std::endl;
                    sp += 4;
                    pc += 2;
                } else {
                    running = false;
                }
                print_state("POP");
                break;
            }
            case Opcode::CMP: {
                if (pc + 2 < program.size()) {
                    uint8_t reg1 = program[pc + 1];
                    uint8_t reg2 = program[pc + 2];
                    if (reg1 < registers.size() && reg2 < registers.size()) {
                        int32_t result = static_cast<int32_t>(registers[reg1]) - static_cast<int32_t>(registers[reg2]);
                        flags = 0;
                        if (result == 0) flags |= FLAG_ZERO;
                        if (result < 0)  flags |= FLAG_SIGN;
                    }
                    pc += 3;
                } else {
                    running = false;
                }
                print_state("CMP");
                break;
            }
            case Opcode::JZ: {
                if (pc + 1 < program.size()) {
                    uint8_t addr = program[pc + 1];
                    if (flags & FLAG_ZERO) {
                        if (valid_instr_starts.find(addr) == valid_instr_starts.end()) {
                            Logger::instance().error() << std::right << std::setw(23) << std::setfill(' ')
                                << "Invalid jump address " << "│ (JZ): "
                                << static_cast<int>(addr) << " at PC=" << pc << std::endl;
                            running = false;
                            break;
                        }
                        pc = addr;
                    } else {
                        pc += 2;
                    }
                } else {
                    running = false;
                }
                print_state("JZ");
                break;
            }
            case Opcode::JNZ: {
                if (pc + 1 < program.size()) {
                    uint8_t addr = program[pc + 1];
                    if (!(flags & FLAG_ZERO)) {
                        if (valid_instr_starts.find(addr) == valid_instr_starts.end()) {
                            Logger::instance().error() << std::right << std::setw(23) << std::setfill(' ')
                                << "Invalid jump address " << "│ (JNZ): "
                                << static_cast<int>(addr) << " at PC=" << pc << std::endl;
                            running = false;
                            break;
                        }
                        pc = addr;
                    } else {
                        pc += 2;
                    }
                } else {
                    running = false;
                }
                print_state("JNZ");
                break;
            }
            case Opcode::JS: {
                if (pc + 1 < program.size()) {
                    uint8_t addr = program[pc + 1];
                    if (flags & FLAG_SIGN) {
                        if (valid_instr_starts.find(addr) == valid_instr_starts.end()) {
                            Logger::instance().error() << std::right << std::setw(23) << std::setfill(' ')
                                << "Invalid jump address " << "│ (JS): "
                                << static_cast<int>(addr) << " at PC=" << pc << std::endl;
                            running = false;
                            break;
                        }
                        pc = addr;
                    } else {
                        pc += 2;
                    }
                } else {
                    running = false;
                }
                print_state("JS");
                break;
            }
            case Opcode::JNS: {
                if (pc + 1 < program.size()) {
                    uint8_t addr = program[pc + 1];
                    if (!(flags & FLAG_SIGN)) {
                        if (valid_instr_starts.find(addr) == valid_instr_starts.end()) {
                            Logger::instance().error() << std::right << std::setw(23) << std::setfill(' ')
                                << "Invalid jump address " << "│ (JNS): "
                                << static_cast<int>(addr) << " at PC=" << pc << std::endl;
                            running = false;
                            break;
                        }
                        pc = addr;
                    } else {
                        pc += 2;
                    }
                } else {
                    running = false;
                }
                print_state("JNS");
                break;
            }
            case Opcode::MUL: {
                if (pc + 2 < program.size()) {
                    uint8_t reg1 = program[pc + 1];
                    uint8_t reg2 = program[pc + 2];
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[MUL] │ PC={} R{} *= R{}", pc, "", pc, reg1, reg2) << std::endl;
                    if (reg1 < registers.size() && reg2 < registers.size()) {
                        uint8_t before = registers[reg1];
                        registers[reg1] *= registers[reg2];
                        Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[MUL] │ R{}: {} * {} = {}", pc, "", reg1, before, registers[reg2], registers[reg1]) << std::endl;
                    }
                    pc += 3;
                } else {
                    running = false;
                }
                print_state("MUL");
                break;
            }
            case Opcode::DIV: {
                if (pc + 2 < program.size()) {
                    uint8_t reg1 = program[pc + 1];
                    uint8_t reg2 = program[pc + 2];
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[DIV] │ PC={} R{} /= R{}", pc, "", pc, reg1, reg2) << std::endl;
                    if (reg1 < registers.size() && reg2 < registers.size()) {
                        if (registers[reg2] == 0) {
                            Logger::instance().error() << fmt::format("{:>6}Invalid Division │ Division by zero at PC={}", "", pc) << std::endl;
                            running = false;
                            break;
                        }
                        uint8_t before = registers[reg1];
                        registers[reg1] /= registers[reg2];
                        Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[DIV] │ R{}: {} / {} = {}", pc, "", reg1, before, registers[reg2], registers[reg1]) << std::endl;
                    }
                    pc += 3;
                } else {
                    running = false;
                }
                print_state("DIV");
                break;
            }
            case Opcode::INC: {
                if (pc + 1 < program.size()) {
                    uint8_t reg = program[pc + 1];
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[INC] │ PC={} R{}", pc, "", pc, static_cast<int>(reg)) << std::endl;
                    if (reg < registers.size()) {
                        uint8_t before = registers[reg];
                        ++registers[reg];
                        Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[INC] │ R{}: {} + 1 = {}", pc, "", static_cast<int>(reg), before, registers[reg]) << std::endl;
                    }
                    pc += 2;
                } else {
                    running = false;
                }
                print_state("INC");
                break;
            }
            case Opcode::DEC: {
                if (pc + 1 < program.size()) {
                    uint8_t reg = program[pc + 1];
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>5}[DEC] │ PC={} R{}", pc, "", pc, static_cast<int>(reg)) << std::endl;
                    if (reg < registers.size()) {
                        uint8_t before = registers[reg];
                        --registers[reg];
                        Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>5}[DEC] │ R{}: {} - 1 = {}", pc, "", static_cast<int>(reg), before, registers[reg]) << std::endl;
                    }
                    pc += 2;
                } else {
                    running = false;
                }
                print_state("DEC");
                break;
            }
            case Opcode::AND: {
                if (pc + 2 < program.size()) {
                    uint8_t reg1 = program[pc + 1];
                    uint8_t reg2 = program[pc + 2];
                    if (reg1 < registers.size() && reg2 < registers.size()) {
                        registers[reg1] &= registers[reg2];
                    }
                    pc += 3;
                } else {
                    running = false;
                }
                print_state("AND");
                break;
            }
            case Opcode::OR: {
                if (pc + 2 < program.size()) {
                    uint8_t reg1 = program[pc + 1];
                    uint8_t reg2 = program[pc + 2];
                    if (reg1 < registers.size() && reg2 < registers.size()) {
                        registers[reg1] |= registers[reg2];
                    }
                    pc += 3;
                } else {
                    running = false;
                }
                print_state("OR");
                break;
            }
            case Opcode::XOR: {
                if (pc + 2 < program.size()) {
                    uint8_t reg1 = program[pc + 1];
                    uint8_t reg2 = program[pc + 2];
                    if (reg1 < registers.size() && reg2 < registers.size()) {
                        registers[reg1] ^= registers[reg2];
                    }
                    pc += 3;
                } else {
                    running = false;
                }
                print_state("XOR");
                break;
            }
            case Opcode::NOT: {
                if (pc + 1 < program.size()) {
                    uint8_t reg = program[pc + 1];
                    if (reg < registers.size()) {
                        registers[reg] = ~registers[reg];
                    }
                    pc += 2;
                } else {
                    running = false;
                }
                print_state("NOT");
                break;
            }
            case Opcode::SHL: {
                if (pc + 2 < program.size()) {
                    uint8_t reg = program[pc + 1];
                    uint8_t imm = program[pc + 2];
                    if (reg < registers.size()) {
                        registers[reg] <<= imm;
                    }
                    pc += 3;
                } else {
                    running = false;
                }
                print_state("SHL");
                break;
            }
            case Opcode::SHR: {
                if (pc + 2 < program.size()) {
                    uint8_t reg = program[pc + 1];
                    uint8_t imm = program[pc + 2];
                    if (reg < registers.size()) {
                        registers[reg] >>= imm;
                    }
                    pc += 3;
                } else {
                    running = false;
                }
                print_state("SHR");
                break;
            }
            case Opcode::CALL: {
                arg_offset = 8; // Reset offset at each call
                uint32_t addr = fetch_operand();
                Logger::instance().debug() << fmt::format(
                    "[PC=0x{:04X}]{:>5}[Call] │ PC=0x{:X}->addr=0x{:X} ret 0x{:X} and FP 0x{:X} to stack at SP=0x{:X}",
                    pc, "", pc, addr, (pc + 1), fp, sp
                ) << std::endl;
                // Push old FP
                sp -= 4;
                write_mem32(sp, fp);
                // Push return address (pc + 1)
                sp -= 4;
                write_mem32(sp, pc + 1);
                // Set new FP
                fp = sp;
                print_stack_frame("CALL");
                pc = addr;
                Logger::instance().debug() << fmt::format(
                    "[PC=0x{:04X}]{:>5}[CALL] │ After jump PC=0x{:X}",
                    pc, "", pc
                ) << std::endl;
                print_state("CALL");
                break;
            }
            case Opcode::RET: {
                Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[RET] │ SP={} Restoring FP and popping return address", pc, "", sp) << std::endl;
                // Save return value before unwinding stack
                uint32_t ret_val = read_mem32(sp); // return value is at sp
                uint32_t ret_addr = read_mem32(sp + 4);
                uint32_t old_fp = read_mem32(sp + 8);

                // Unwind stack
                sp += 12;
                fp = old_fp;

                // Write return value to caller's stack frame (at fp + 8)
                write_mem32(fp + 0, ret_val);

                print_stack_frame("RET");
                pc = ret_addr;
                arg_offset = 0; // Reset offset at each return
                print_state("RET");
                break;
            }
            case Opcode::PUSH_ARG: {
                uint8_t reg = fetch_operand();
                Logger::instance().debug() << fmt::format(
                    "[PC=0x{:04X}]{:>1}[PUSH_ARG] │ SP={} Pushing R{}={}",
                    pc, "", sp, static_cast<int>(reg), registers[reg]
                ) << std::endl;
                sp -= 4;
                write_mem32(sp, registers[reg]);
                pc++;
                print_state("PUSH_ARG");
                break;
            }
            case Opcode::POP_ARG: {
                uint8_t reg = fetch_operand();
                registers[reg] = read_mem32(fp + arg_offset);
                Logger::instance().debug() << fmt::format(
                    "[PC=0x{:04X}]{:>2}[POP_ARG] │ FP={} arg_offset={} addr={} value={}",
                    pc, "", fp, arg_offset, (fp + arg_offset), registers[reg]
                ) << std::endl;
                arg_offset += 4;
                pc++;
                print_state("POP_ARG");
                break;
            }
            case Opcode::IN: {
                if (pc + 2 < program.size()) {
                    uint8_t reg = program[pc + 1];
                    uint8_t port = program[pc + 2];
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>7}[IN] │ PC={} R{} <- port {}", pc, "", pc, reg, port) << std::endl;
                    if (reg < registers.size()) {
                        uint8_t value = readPort(port);
                        registers[reg] = value;
                        Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>7}[IN] │ R{} = {}", pc, "", reg, value) << std::endl;
                    }
                    pc += 3;
                } else {
                    running = false;
                }
                print_state("IN");
                break;
            }
            case Opcode::OUT: {
                if (pc + 2 < program.size()) {
                    uint8_t reg = program[pc + 1];
                    uint8_t port = program[pc + 2];
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[OUT] │ PC={} port {} <- R{}={}", pc, "", pc, port, reg, registers[reg]) << std::endl;
                    if (reg < registers.size()) {
                        writePort(port, registers[reg]);
                    }
                    pc += 3;
                } else {
                    running = false;
                }
                print_state("OUT");
                break;
            }
            case Opcode::HALT:
                Logger::instance().debug() << fmt::format(
                    "[PC=0x{:04X}]{:>5}[HALT] │ PC={}",
                    pc, "", pc
                ) << std::endl;
                running = false;
                ++pc;
                print_state("HALT");
                break;
            default: {
                // Print opcode name if possible
                std::string opcode_name;
                switch (opcode) {
                    case Opcode::NOP: opcode_name = "NOP"; break;
                    case Opcode::LOAD_IMM: opcode_name = "LOAD_IMM"; break;
                    case Opcode::ADD: opcode_name = "ADD"; break;
                    case Opcode::SUB: opcode_name = "SUB"; break;
                    case Opcode::MOV: opcode_name = "MOV"; break;
                    case Opcode::JMP: opcode_name = "JMP"; break;
                    case Opcode::LOAD: opcode_name = "LOAD"; break;
                    case Opcode::STORE: opcode_name = "STORE"; break;
                    case Opcode::PUSH: opcode_name = "PUSH"; break;
                    case Opcode::POP: opcode_name = "POP"; break;
                    case Opcode::CMP: opcode_name = "CMP"; break;
                    case Opcode::JZ: opcode_name = "JZ"; break;
                    case Opcode::JNZ: opcode_name = "JNZ"; break;
                    case Opcode::JS: opcode_name = "JS"; break;
                    case Opcode::JNS: opcode_name = "JNS"; break;
                    case Opcode::MUL: opcode_name = "MUL"; break;
                    case Opcode::DIV: opcode_name = "DIV"; break;
                    case Opcode::INC: opcode_name = "INC"; break;
                    case Opcode::DEC: opcode_name = "DEC"; break;
                    case Opcode::AND: opcode_name = "AND"; break;
                    case Opcode::OR: opcode_name = "OR"; break;
                    case Opcode::XOR: opcode_name = "XOR"; break;
                    case Opcode::NOT: opcode_name = "NOT"; break;
                    case Opcode::SHL: opcode_name = "SHL"; break;
                    case Opcode::SHR: opcode_name = "SHR"; break;
                    case Opcode::CALL: opcode_name = "CALL"; break;
                    case Opcode::RET: opcode_name = "RET"; break;
                    case Opcode::PUSH_ARG: opcode_name = "PUSH_ARG"; break;
                    case Opcode::POP_ARG: opcode_name = "POP_ARG"; break;
                    case Opcode::HALT: opcode_name = "HALT"; break;
                    default: opcode_name = "UNKNOWN"; break;
                }
                Logger::instance().error()
                    << std::right << std::setw(23) << std::setfill(' ') << "Invalid opcode " << "│ "
                    << "opcode: 0x"
                    << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(opcode)
                    << " │ is not defined" << std::dec << std::endl;
                std::ostringstream buf;
                buf << std::right << std::setw(22) << std::setfill(' ') << "Stack top" << " │ memory[SP..SP+3]: ";
                for (size_t i = sp; i < std::min(static_cast<size_t>(sp + 4), memory.size()); ++ i) {
                    buf << "[" << std::setw(3) << std::setfill(' ') << i << "]="
                            << std::left << std::setw(3) << static_cast<int>(memory[i]) << std::setfill(' ');
                }
                Logger::instance().error() << buf.str() << std::endl;
                running = false;
                print_state(opcode_name);
                break;
            }
        }
    }
}

void CPU::print_registers() const {
    std::ostringstream oss;
    oss << std::right << std::setw(24) << "Registers " << "│ ";
    for (size_t i = 0; i < registers.size(); ++i) {
        oss << "R" << i << ": " << registers[i] << " ";
    } oss << std::endl;

    oss << std::right << std::setw(55) << "Memory " << "│ ";
    oss << "PC: " << pc << ", SP: " << sp << ", Flags: " << flags;
    Logger::instance().info() << oss.str() << std::endl;
}

void CPU::print_memory(std::size_t start, std::size_t end) const {
    if (end > memory.size()) end = memory.size();
    std::ostringstream oss;
    oss << std::right << std::setw(16) << "Memory dump [" << start << "..." << end-1 << "] │" << std::endl;
    for (std::size_t i = start; i < end; ++i) {
        oss << "[" << "0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << i << "]="
            << "0x" << std::setw(2) << std::setfill('0') << static_cast<int>(memory[i]) << " ";
        if ((i - start + 1) % 8 == 0) oss << std::endl;
    }
    oss << std::endl;
    Logger::instance().info() << oss.str() << std::endl;
}

// Run the whole program (reset and step until done)
void CPU::run(const std::vector<uint8_t>& program) {
    reset();
    std::copy(program.begin(), program.end(), memory.begin());
    pc = 0;
    sp = memory.size() - 4;
    fp = sp;
    bool running = true;
    auto valid_instr_starts = compute_valid_instruction_starts(program);
    while (running && pc < program.size()) {
        running = step(program);
    }
}

// Step a single instruction. Returns false if halted or error, true if can continue
bool CPU::step(const std::vector<uint8_t>& program) {
    if (pc >= program.size()) return false;
    Opcode opcode = static_cast<Opcode>(program[pc]);
    bool running = true;
    auto valid_instr_starts = compute_valid_instruction_starts(program);

    // Copy of the main switch from execute()
    switch (opcode) {
        case Opcode::NOP:
            Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[NOP] │ PC={}", pc, "", pc) << std::endl;
            ++pc;
            print_state("NOP");
            break;
        case Opcode::LOAD_IMM: {
            if (pc + 2 < program.size()) {
                uint8_t reg = program[pc + 1];
                uint8_t imm = program[pc + 2];
                Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>1}[LOAD_IMM]│ PC={} reg={} imm={}", pc, "", pc, reg, imm) << std::endl;
                if (reg < registers.size()) {
                    registers[reg] = imm;
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>1}[LOAD_IMM]│ Set R{} = {}", pc, "", reg, imm) << std::endl;
                }
                pc += 3;
            } else {
                running = false;
            }
            print_state("LOAD_IMM");
            break;
        }
        case Opcode::ADD: {
            if (pc + 2 < program.size()) {
                uint8_t reg1 = program[pc + 1];
                uint8_t reg2 = program[pc + 2];
                Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[ADD] │ PC={} R{} += R{}", pc, "", pc, reg1, reg2) << std::endl;
                uint8_t before = registers[reg1];
                registers[reg1] += registers[reg2];
                Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[ADD] │ R{}: {} + {} = {}", pc, "", reg1, before, registers[reg2], registers[reg1]) << std::endl;
            }
            pc += 3;
            print_state("ADD");
            break;
        }
        case Opcode::SUB: {
            if (pc + 2 < program.size()) {
                uint8_t reg1 = program[pc + 1];
                uint8_t reg2 = program[pc + 2];
                Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[SUB] │ PC={} R{} -= R{}", pc, "", pc, reg1, reg2) << std::endl;
                if (reg1 < registers.size() && reg2 < registers.size()) {
                    uint8_t before = registers[reg1];
                    registers[reg1] -= registers[reg2];
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[SUB] │ R{}: {} - {} = {}", pc, "", reg1, before, registers[reg2], registers[reg1]) << std::endl;
                }
                pc += 3;
            } else {
                running = false;
            }
            print_state("SUB");
            break;
        }
        case Opcode::MOV: {
            if (pc + 2 < program.size()) {
                uint8_t reg1 = program[pc + 1];
                uint8_t reg2 = program[pc + 2];
                if (reg1 < registers.size() && reg2 < registers.size()) {
                    registers[reg1] = registers[reg2];
                }
                pc += 3;
            } else {
                running = false;
            }
            print_state("MOV");
            break;
        }
        case Opcode::JMP: {
            if (pc + 1 < program.size()) {
                uint8_t addr = program[pc + 1];
                if (valid_instr_starts.find(addr) == valid_instr_starts.end()) {
                    Logger::instance().error() << std::right << std::setw(23) << std::setfill(' ')
                    << "Invalid jump address " << "│ (JMP): "
                    << static_cast<int>(addr) << " at PC=" << pc << std::endl;
                    running = false;
                    break;
                }
                pc = addr;
            } else {
                running = false;
            }
            print_state("JMP");
            break;
        }
        case Opcode::LOAD: {
            // LOAD reg, addr
            if (pc + 2 < program.size()) {
                uint8_t reg = program[pc + 1];
                uint8_t addr = program[pc + 2];
                if (reg < registers.size() && addr < memory.size()) {
                    registers[reg] = memory[addr];
                }
                pc += 3;
            } else {
                running = false;
            }
            print_state("LOAD");
            break;
        }
        case Opcode::STORE: {
            // STORE reg, addr
            if (pc + 2 < program.size()) {
                uint8_t reg = program[pc + 1];
                uint8_t addr = program[pc + 2];
                if (reg < registers.size() && addr < memory.size()) {
                    memory[addr] = registers[reg];
                }
                pc += 3;
            } else {
                running = false;
            }
            print_state("STORE");
            break;
        }
        case Opcode::PUSH: {
            if (pc + 1 < program.size()) {
                uint8_t reg = program[pc + 1];
                Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>5}[PUSH] │ PC={} Pushing R{}={}", pc, "", pc, static_cast<int>(reg), registers[reg]) << std::endl;
                sp -= 4;
                write_mem32(sp, registers[reg]);
                pc += 2;
            } else {
                running = false;
            }
            print_state("PUSH");
            break;
        }
        case Opcode::POP: {
            if (pc + 1 < program.size()) {
                uint8_t reg = program[pc + 1];
                registers[reg] = read_mem32(sp);
                Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[POP] │ PC={} Popping to R{}={}", pc, "", pc, static_cast<int>(reg), registers[reg]) << std::endl;
                sp += 4;
                pc += 2;
            } else {
                running = false;
            }
            print_state("POP");
            break;
        }
        case Opcode::CMP: {
            if (pc + 2 < program.size()) {
                uint8_t reg1 = program[pc + 1];
                uint8_t reg2 = program[pc + 2];
                if (reg1 < registers.size() && reg2 < registers.size()) {
                    int32_t result = static_cast<int32_t>(registers[reg1]) - static_cast<int32_t>(registers[reg2]);
                    flags = 0;
                    if (result == 0) flags |= FLAG_ZERO;
                    if (result < 0)  flags |= FLAG_SIGN;
                }
                pc += 3;
            } else {
                running = false;
            }
            print_state("CMP");
            break;
        }
        case Opcode::JZ: {
            if (pc + 1 < program.size()) {
                uint8_t addr = program[pc + 1];
                if (flags & FLAG_ZERO) {
                    if (valid_instr_starts.find(addr) == valid_instr_starts.end()) {
                        Logger::instance().error() << std::right << std::setw(23) << std::setfill(' ')
                            << "Invalid jump address " << "│ (JZ): "
                            << static_cast<int>(addr) << " at PC=" << pc << std::endl;
                        running = false;
                        break;
                    }
                    pc = addr;
                } else {
                    pc += 2;
                }
            } else {
                running = false;
            }
            print_state("JZ");
            break;
        }
        case Opcode::JNZ: {
            if (pc + 1 < program.size()) {
                uint8_t addr = program[pc + 1];
                if (!(flags & FLAG_ZERO)) {
                    if (valid_instr_starts.find(addr) == valid_instr_starts.end()) {
                        Logger::instance().error() << std::right << std::setw(23) << std::setfill(' ')
                            << "Invalid jump address " << "│ (JNZ): "
                            << static_cast<int>(addr) << " at PC=" << pc << std::endl;
                        running = false;
                        break;
                    }
                    pc = addr;
                } else {
                    pc += 2;
                }
            } else {
                running = false;
            }
            print_state("JNZ");
            break;
        }
        case Opcode::JS: {
            if (pc + 1 < program.size()) {
                uint8_t addr = program[pc + 1];
                if (flags & FLAG_SIGN) {
                    if (valid_instr_starts.find(addr) == valid_instr_starts.end()) {
                        Logger::instance().error() << std::right << std::setw(23) << std::setfill(' ')
                            << "Invalid jump address " << "│ (JS): "
                            << static_cast<int>(addr) << " at PC=" << pc << std::endl;
                        running = false;
                        break;
                    }
                    pc = addr;
                } else {
                    pc += 2;
                }
            } else {
                running = false;
            }
            print_state("JS");
            break;
        }
        case Opcode::JNS: {
            if (pc + 1 < program.size()) {
                uint8_t addr = program[pc + 1];
                if (!(flags & FLAG_SIGN)) {
                    if (valid_instr_starts.find(addr) == valid_instr_starts.end()) {
                        Logger::instance().error() << std::right << std::setw(23) << std::setfill(' ')
                            << "Invalid jump address " << "│ (JNS): "
                            << static_cast<int>(addr) << " at PC=" << pc << std::endl;
                        running = false;
                        break;
                    }
                    pc = addr;
                } else {
                    pc += 2;
                }
            } else {
                running = false;
            }
            print_state("JNS");
            break;
        }
        case Opcode::MUL: {
            if (pc + 2 < program.size()) {
                uint8_t reg1 = program[pc + 1];
                uint8_t reg2 = program[pc + 2];
                Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[MUL] │ PC={} R{} *= R{}", pc, "", pc, reg1, reg2) << std::endl;
                if (reg1 < registers.size() && reg2 < registers.size()) {
                    uint8_t before = registers[reg1];
                    registers[reg1] *= registers[reg2];
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[MUL] │ R{}: {} * {} = {}", pc, "", reg1, before, registers[reg2], registers[reg1]) << std::endl;
                }
                pc += 3;
            } else {
                running = false;
            }
            print_state("MUL");
            break;
        }
        case Opcode::DIV: {
            if (pc + 2 < program.size()) {
                uint8_t reg1 = program[pc + 1];
                uint8_t reg2 = program[pc + 2];
                Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[DIV] │ PC={} R{} /= R{}", pc, "", pc, reg1, reg2) << std::endl;
                if (reg1 < registers.size() && reg2 < registers.size()) {
                    if (registers[reg2] == 0) {
                        Logger::instance().error() << fmt::format("{:>6}Invalid Division │ Division by zero at PC={}", "", pc) << std::endl;
                        running = false;
                        break;
                    }
                    uint8_t before = registers[reg1];
                    registers[reg1] /= registers[reg2];
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[DIV] │ R{}: {} / {} = {}", pc, "", reg1, before, registers[reg2], registers[reg1]) << std::endl;
                }
                pc += 3;
            } else {
                running = false;
            }
            print_state("DIV");
            break;
        }
        case Opcode::INC: {
            if (pc + 1 < program.size()) {
                uint8_t reg = program[pc + 1];
                Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>5}[INC] │ PC={} R{}", pc, "", pc, static_cast<int>(reg)) << std::endl;
                if (reg < registers.size()) {
                    uint8_t before = registers[reg];
                    ++registers[reg];
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>5}[INC] │ R{}: {} + 1 = {}", pc, "", static_cast<int>(reg), before, registers[reg]) << std::endl;
                }
                pc += 2;
            } else {
                running = false;
            }
            print_state("INC");
            break;
        }
        case Opcode::DEC: {
            if (pc + 1 < program.size()) {
                uint8_t reg = program[pc + 1];
                Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>5}[DEC] │ PC={} R{}", pc, "", pc, static_cast<int>(reg)) << std::endl;
                if (reg < registers.size()) {
                    uint8_t before = registers[reg];
                    --registers[reg];
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>5}[DEC] │ R{}: {} - 1 = {}", pc, "", static_cast<int>(reg), before, registers[reg]) << std::endl;
                }
                pc += 2;
            } else {
                running = false;
            }
            print_state("DEC");
            break;
        }
        case Opcode::AND: {
            if (pc + 2 < program.size()) {
                uint8_t reg1 = program[pc + 1];
                uint8_t reg2 = program[pc + 2];
                if (reg1 < registers.size() && reg2 < registers.size()) {
                    registers[reg1] &= registers[reg2];
                }
                pc += 3;
            } else {
                running = false;
            }
            print_state("AND");
            break;
        }
        case Opcode::OR: {
            if (pc + 2 < program.size()) {
                uint8_t reg1 = program[pc + 1];
                uint8_t reg2 = program[pc + 2];
                if (reg1 < registers.size() && reg2 < registers.size()) {
                    registers[reg1] |= registers[reg2];
                }
                pc += 3;
            } else {
                running = false;
            }
            print_state("OR");
            break;
        }
        case Opcode::XOR: {
            if (pc + 2 < program.size()) {
                uint8_t reg1 = program[pc + 1];
                uint8_t reg2 = program[pc + 2];
                if (reg1 < registers.size() && reg2 < registers.size()) {
                    registers[reg1] ^= registers[reg2];
                }
                pc += 3;
            } else {
                running = false;
            }
            print_state("XOR");
            break;
        }
        case Opcode::NOT: {
            if (pc + 1 < program.size()) {
                uint8_t reg = program[pc + 1];
                if (reg < registers.size()) {
                    registers[reg] = ~registers[reg];
                }
                pc += 2;
            } else {
                running = false;
            }
            print_state("NOT");
            break;
        }
        case Opcode::SHL: {
            if (pc + 2 < program.size()) {
                uint8_t reg = program[pc + 1];
                uint8_t imm = program[pc + 2];
                if (reg < registers.size()) {
                    registers[reg] <<= imm;
                }
                pc += 3;
            } else {
                running = false;
            }
            print_state("SHL");
            break;
        }
        case Opcode::SHR: {
            if (pc + 2 < program.size()) {
                uint8_t reg = program[pc + 1];
                uint8_t imm = program[pc + 2];
                if (reg < registers.size()) {
                    registers[reg] >>= imm;
                }
                pc += 3;
            } else {
                running = false;
            }
            print_state("SHR");
            break;
        }
        case Opcode::CALL: {
            arg_offset = 8; // Reset offset at each call
            uint32_t addr = fetch_operand();
            Logger::instance().debug() << fmt::format(
                "[PC=0x{:04X}]{:>5}[Call] │ PC=0x{:X}->addr=0x{:X} ret 0x{:X} and FP 0x{:X} to stack at SP=0x{:X}",
                pc, "", pc, addr, (pc + 2), fp, sp
            ) << std::endl;
            // Push old FP
            sp -= 4;
            write_mem32(sp, fp);
            // Push return address (pc + 1)
            sp -= 4;
            write_mem32(sp, pc + 2);
            // Set new FP
            fp = sp;
            print_stack_frame("CALL");
            pc = addr;
            Logger::instance().debug() << fmt::format(
                "[PC=0x{:04X}]{:>5}[CALL] │ After jump PC=0x{:X}",
                pc, "", pc
            ) << std::endl;
            print_state("CALL");
            break;
        }
        case Opcode::RET: {
            Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[RET] │ SP={} Restoring FP and popping return address", pc, "", sp) << std::endl;
            // Save return value before unwinding stack
            uint32_t ret_val = read_mem32(sp); // return value is at sp
            uint32_t ret_addr = read_mem32(sp + 4);
            uint32_t old_fp = read_mem32(sp + 8);

            // Unwind stack
            sp += 12;
            fp = old_fp;

            // Write return value to caller's stack frame (at fp + 8)
            write_mem32(fp + 0, ret_val);

            print_stack_frame("RET");
            pc = ret_addr;
            arg_offset = 0; // Reset offset at each return
            print_state("RET");
            break;
        }
        case Opcode::PUSH_ARG: {
            uint8_t reg = fetch_operand();
            Logger::instance().debug() << fmt::format(
                "[PC=0x{:04X}]{:>1}[PUSH_ARG] │ SP={} Pushing R{}={}",
                pc, "", sp, static_cast<int>(reg), registers[reg]
            ) << std::endl;
            sp -= 4;
            write_mem32(sp, registers[reg]);
            pc++;
            print_state("PUSH_ARG");
            break;
        }
        case Opcode::POP_ARG: {
            uint8_t reg = fetch_operand();
            registers[reg] = read_mem32(fp + arg_offset);
            Logger::instance().debug() << fmt::format(
                "[PC=0x{:04X}]{:>2}[POP_ARG] │ FP={} arg_offset={} addr={} value={}",
                pc, "", fp, arg_offset, (fp + arg_offset), registers[reg]
            ) << std::endl;
            arg_offset += 4;
            pc++;
            print_state("POP_ARG");
            break;
        }
        case Opcode::IN: {
            if (pc + 2 < program.size()) {
                uint8_t reg = program[pc + 1];
                uint8_t port = program[pc + 2];
                Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>7}[IN] │ PC={} R{} <- port {}", pc, "", pc, reg, port) << std::endl;
                if (reg < registers.size()) {
                    uint8_t value = readPort(port);
                    registers[reg] = value;
                    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>7}[IN] │ R{} = {}", pc, "", reg, value) << std::endl;
                }
                pc += 3;
            } else {
                running = false;
            }
            print_state("IN");
            break;
        }
        case Opcode::OUT: {
            if (pc + 2 < program.size()) {
                uint8_t reg = program[pc + 1];
                uint8_t port = program[pc + 2];
                Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[OUT] │ PC={} port {} <- R{}={}", pc, "", pc, port, reg, registers[reg]) << std::endl;
                if (reg < registers.size()) {
                    writePort(port, registers[reg]);
                }
                pc += 3;
            } else {
                running = false;
            }
            print_state("OUT");
            break;
        }
        case Opcode::HALT:
            Logger::instance().debug() << fmt::format(
                "[PC=0x{:04X}]{:>5}[HALT] │ PC={}",
                pc, "", pc
            ) << std::endl;
            running = false;
            ++pc;
            print_state("HALT");
            break;
        default: {
            // Print opcode name if possible
            std::string opcode_name;
            switch (opcode) {
                case Opcode::NOP: opcode_name = "NOP"; break;
                case Opcode::LOAD_IMM: opcode_name = "LOAD_IMM"; break;
                case Opcode::ADD: opcode_name = "ADD"; break;
                case Opcode::SUB: opcode_name = "SUB"; break;
                case Opcode::MOV: opcode_name = "MOV"; break;
                case Opcode::JMP: opcode_name = "JMP"; break;
                case Opcode::LOAD: opcode_name = "LOAD"; break;
                case Opcode::STORE: opcode_name = "STORE"; break;
                case Opcode::PUSH: opcode_name = "PUSH"; break;
                case Opcode::POP: opcode_name = "POP"; break;
                case Opcode::CMP: opcode_name = "CMP"; break;
                case Opcode::JZ: opcode_name = "JZ"; break;
                case Opcode::JNZ: opcode_name = "JNZ"; break;
                case Opcode::JS: opcode_name = "JS"; break;
                case Opcode::JNS: opcode_name = "JNS"; break;
                case Opcode::MUL: opcode_name = "MUL"; break;
                case Opcode::DIV: opcode_name = "DIV"; break;
                case Opcode::INC: opcode_name = "INC"; break;
                case Opcode::DEC: opcode_name = "DEC"; break;
                case Opcode::AND: opcode_name = "AND"; break;
                case Opcode::OR: opcode_name = "OR"; break;
                case Opcode::XOR: opcode_name = "XOR"; break;
                case Opcode::NOT: opcode_name = "NOT"; break;
                case Opcode::SHL: opcode_name = "SHL"; break;
                case Opcode::SHR: opcode_name = "SHR"; break;
                case Opcode::CALL: opcode_name = "CALL"; break;
                case Opcode::RET: opcode_name = "RET"; break;
                case Opcode::PUSH_ARG: opcode_name = "PUSH_ARG"; break;
                case Opcode::POP_ARG: opcode_name = "POP_ARG"; break;
                case Opcode::HALT: opcode_name = "HALT"; break;
                default: opcode_name = "UNKNOWN"; break;
            }
            Logger::instance().error()
                << std::right << std::setw(23) << std::setfill(' ') << "Invalid opcode " << "│ "
                << "opcode: 0x"
                << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(opcode)
                << " │ is not defined" << std::dec << std::endl;
            std::ostringstream buf;
            buf << std::right << std::setw(22) << std::setfill(' ') << "Stack top" << " │ memory[SP..SP+3]: ";
            for (size_t i = sp; i < std::min(static_cast<size_t>(sp + 4), memory.size()); ++i) {
                buf << "[" << std::setw(3) << std::setfill(' ') << i << "]="
                        << std::left << std::setw(3) << static_cast<int>(memory[i]) << std::setfill(' ');
            }
            Logger::instance().error() << buf.str() << std::endl;
            running = false;
            print_state(opcode_name);
            break;
        }
    }
    return running && pc < program.size();
}

uint8_t CPU::readPort(uint8_t port) {
    return vhw::DeviceManager::instance().readPort(port);
}

void CPU::writePort(uint8_t port, uint8_t value) {
    vhw::DeviceManager::instance().writePort(port, value);
}