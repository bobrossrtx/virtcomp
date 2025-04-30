#include <iomanip>
#include <chrono>
#include <ctime>
#include <sstream>
#include <unordered_set>

#include "cpu.hpp"
#include "../config.hpp"
#include "../debug/logger.hpp"


// Flag definitions
constexpr uint32_t FLAG_ZERO = 1 << 0;
constexpr uint32_t FLAG_SIGN = 1 << 1;

// CPU constructor
CPU::CPU()
    : registers(4, 0), memory(256, 0), pc(0), sp(256), flags(0) {} // Stack grows down from top of memory

CPU::~CPU() = default;

// Reset the CPU state
void CPU::reset() {
    std::fill(registers.begin(), registers.end(), 0);
    pc = 0;
    sp = memory.size();
    flags = 0;
}

// Print the CPU state (debugging information)
void CPU::print_state(const std::string& info) const {

    // If debug is not enabled, do not print the state
    if (!debug) return;

    // Get current time
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    std::ostringstream pc_ss;
    pc_ss << "PC=0x" << std::setw(4) << std::setfill('0') << std::hex << std::uppercase << pc;
    oss << "[" << pc_ss.str() << "] "
        << std::right << std::setw(10) << std::setfill(' ') << info << " | ";
    for (size_t i = 0; i < registers.size(); ++i) {
        oss << "R" << i << "=0x" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(registers[i]) << " ";
    }
    oss << "SP=0x" << std::setw(3) << std::setfill('0') << std::hex << std::uppercase << sp << " ";
    oss << std::dec;
    Logger::instance().debug() << oss.str() << std::endl;

    // Optionally print a small memory window around SP, with date and time
    if (sp < memory.size()) {
        std::ostringstream stack_oss;
        stack_oss << std::right << std::setw(22) << std::setfill(' ') << "Stack top" << " | memory[SP..SP+3]: ";
        for (size_t i = sp; i < std::min(static_cast<size_t>(sp + 4), memory.size()); ++i) {
            stack_oss << "[" << std::setw(3) << std::setfill(' ') << i << "]="
                      << std::setw(3) << std::setfill(' ') << static_cast<int>(memory[i]) << " ";
        }
        Logger::instance().debug() << stack_oss.str() << std::endl;
    }
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

void CPU::execute(const std::vector<uint8_t>& program) {
    pc = 0;
    bool running = true;
    auto valid_instr_starts = compute_valid_instruction_starts(program);
    while (pc < program.size() && running) {
        Opcode opcode = static_cast<Opcode>(program[pc]);
        switch (opcode) {
            case Opcode::NOP:
                ++pc;
                print_state("NOP");
                break;
            case Opcode::LOAD_IMM: {
                // LOAD_IMM reg, imm
                if (pc + 2 < program.size()) {
                    uint8_t reg = program[pc + 1];
                    uint8_t imm = program[pc + 2];
                    if (reg < registers.size()) {
                        registers[reg] = imm;
                    }
                    pc += 3;
                } else {
                    running = false;
                }
                print_state("LOAD_IMM");
                break;
            }
            case Opcode::ADD: {
                // ADD reg1, reg2 (reg1 = reg1 + reg2)
                if (pc + 2 < program.size()) {
                    uint8_t reg1 = program[pc + 1];
                    uint8_t reg2 = program[pc + 2];
                    if (reg1 < registers.size() && reg2 < registers.size()) {
                        registers[reg1] += registers[reg2];
                    }
                    pc += 3;
                } else {
                    running = false;
                }
                print_state("ADD");
                break;
            }
            case Opcode::SUB: {
                // SUB reg1, reg2 (reg1 = reg1 - reg2)
                if (pc + 2 < program.size()) {
                    uint8_t reg1 = program[pc + 1];
                    uint8_t reg2 = program[pc + 2];
                    if (reg1 < registers.size() && reg2 < registers.size()) {
                        registers[reg1] -= registers[reg2];
                    }
                    pc += 3;
                } else {
                    running = false;
                }
                print_state("SUB");
                break;
            }
            case Opcode::MUL: {
                // MUL reg1, reg2 (reg1 = reg1 * reg2)
                if (pc + 2 < program.size()) {
                    uint8_t reg1 = program[pc + 1];
                    uint8_t reg2 = program[pc + 2];
                    if (reg1 < registers.size() && reg2 < registers.size()) {
                        registers[reg1] *= registers[reg2];
                    }
                    pc += 3;
                } else {
                    running = false;
                }
                print_state("MUL");
                break;
            }
            case Opcode::DIV: {
                // DIV reg1, reg2 (reg1 = reg1 / reg2)
                if (pc + 2 < program.size()) {
                    uint8_t reg1 = program[pc + 1];
                    uint8_t reg2 = program[pc + 2];
                    if (reg1 < registers.size() && reg2 < registers.size()) {
                        if (registers[reg2] == 0) {
                            Logger::instance().error() << std::right << std::setw(24) << std::setfill(' ') << "Division by zero |" << std::endl;
                            running = false;
                            break;
                        }
                        registers[reg1] /= registers[reg2];
                    }
                    pc += 3;
                } else {
                    running = false;
                }
                print_state("DIV");
                break;
            }
            case Opcode::INC: {
                // INC reg (reg = reg + 1)
                if (pc + 1 < program.size()) {
                    uint8_t reg = program[pc + 1];
                    if (reg < registers.size()) {
                        ++registers[reg];
                    }
                    pc += 2;
                } else {
                    running = false;
                }
                print_state("INC");
                break;
            }
            case Opcode::DEC: {
                // DEC reg (reg = reg - 1)
                if (pc + 1 < program.size()) {
                    uint8_t reg = program[pc + 1];
                    if (reg < registers.size()) {
                        --registers[reg];
                    }
                    pc += 2;
                } else {
                    running = false;
                }
                print_state("DEC");
                break;
            }
            case Opcode::AND: {
                // AND reg1, reg2 (reg1 = reg1 & reg2)
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
                // OR reg1, reg2 (reg1 = reg1 | reg2)
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
                // XOR reg1, reg2 (reg1 = reg1 ^ reg2)
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
                // NOT reg (reg = ~reg)
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
                // SHL reg, imm (reg = reg << imm)
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
                // SHR reg, imm (reg = reg >> imm)
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
            case Opcode::MOV: {
                // MOV reg1, reg2 (reg1 = reg2)
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
                        << "Invalid jump address " << "| (JMP): "
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
                // LOAD reg, mem_addr
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
                // STORE reg, mem_addr
                if (pc + 2 < program.size()) {
                    uint8_t reg = program[pc + 1];
                    uint8_t addr = program[pc + 2];
                    if (reg < registers.size() && addr < memory.size()) {
                        memory[addr] = static_cast<uint8_t>(registers[reg]);
                    }
                    pc += 3;
                } else {
                    running = false;
                }
                print_state("STORE");
                break;
            }
            case Opcode::PUSH: {
                // PUSH reg
                if (pc + 1 < program.size()) {
                    uint8_t reg = program[pc + 1];
                    if (reg < registers.size()) {
                        if (sp == 0) {
                            Logger::instance().error() << "Stack overflow on PUSH!" << std::endl;
                            running = false;
                            break;
                        }
                        --sp;
                        memory[sp] = static_cast<uint8_t>(registers[reg]);
                    }
                    pc += 2;
                } else {
                    running = false;
                }
                print_state("PUSH");
                break;
            }
            case Opcode::POP: {
                // POP reg
                if (pc + 1 < program.size()) {
                    uint8_t reg = program[pc + 1];
                    if (reg < registers.size()) {
                        if (sp == memory.size()) {
                            Logger::instance().error() << "Stack underflow on POP!" << std::endl;
                            running = false;
                            break;
                        }
                        registers[reg] = memory[sp];
                        ++sp;
                    }
                    pc += 2;
                } else {
                    running = false;
                }
                print_state("POP");
                break;
            }
            case Opcode::HALT:
                running = false;
                ++pc;
                print_state("HALT");
                break;
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
                                << "Invalid jump address " << "| (JZ): "
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
                                << "Invalid jump address " << "| (JNZ): "
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
                                << "Invalid jump address " << "| (JS): "
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
                                << "Invalid jump address " << "| (JNS): "
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
                    case Opcode::HALT: opcode_name = "HALT"; break;
                    default: opcode_name = "UNKNOWN"; break;
                }
                
                Logger::instance().error()
                    << std::right << std::setw(23) << std::setfill(' ') << "Invalid opcode " << "| "
                    << "opcode: 0x"
                    << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(opcode)
                    << " | is not defined" << std::dec << std::endl;

                // Print a small memory window around PC for debugging
                std::ostringstream buf;
                buf << std::right << std::setw(23) << std::setfill(' ') << "Stack top " << "| Memory[PC..PC+3] : ";
                for (size_t i = pc; i < std::min(static_cast<size_t>(pc + 4), program.size()); ++i) {
                    buf << "[" << i << "]=" << static_cast<int>(program[i]) << " ";
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
    oss << std::right << std::setw(24) << "Registers " << "| ";
    for (size_t i = 0; i < registers.size(); ++i) {
        oss << "R" << i << ": " << registers[i] << " ";
    } oss << std::endl;
    
    oss << std::right << std::setw(57) << "Memory " << "| ";
    oss << "PC: " << pc << ", SP: " << sp << ", Flags: " << flags;
    Logger::instance().info() << oss.str() << std::endl;
}

void CPU::print_memory(std::size_t start, std::size_t end) const {
    if (end > memory.size()) end = memory.size();
    std::ostringstream oss;
    oss << std::right << std::setw(16) << "Memory dump [" << start << "..." << end-1 << "] |" << std::endl;
    for (std::size_t i = start; i < end; ++i) {
        oss << "[" << "0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << i << "]="
            << std::dec << static_cast<int>(memory[i]) << " ";
        if ((i - start + 1) % 8 == 0) oss << std::endl;
    }
    oss << std::endl;
    Logger::instance().info() << oss.str() << std::endl;
}