// Consolidated opcode implementations for standalone builds
// This file includes all opcode implementations in a single compilation unit
// to reduce compilation time for standalone executables

#include "opcode_dispatcher.hpp"
#include "../cpu.hpp"
#include "../cpu_flags.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>
#include <iomanip>

using Logging::Logger;

// Include all opcode header files
#include "add.hpp"
#include "and.hpp"
#include "call.hpp"
#include "cmp.hpp"
#include "db.hpp"
#include "dec.hpp"
#include "div.hpp"
#include "halt.hpp"
#include "inb.hpp"
#include "inc.hpp"
#include "in.hpp"
#include "inl.hpp"
#include "instr.hpp"
#include "inw.hpp"
#include "jmp.hpp"
#include "jns.hpp"
#include "jnz.hpp"
#include "js.hpp"
#include "jz.hpp"
#include "lea.hpp"
#include "load.hpp"
#include "load_imm.hpp"
#include "mov.hpp"
#include "mul.hpp"
#include "nop.hpp"
#include "not.hpp"
#include "opcode_handler.hpp"
#include "or.hpp"
#include "outb.hpp"
#include "out.hpp"
#include "outl.hpp"
#include "outstr.hpp"
#include "outw.hpp"
#include "pop_arg.hpp"
#include "pop_flag.hpp"
#include "pop.hpp"
#include "push_arg.hpp"
#include "push_flag.hpp"
#include "push.hpp"
#include "ret.hpp"
#include "shl.hpp"
#include "shr.hpp"
#include "store.hpp"
#include "sub.hpp"
#include "swap.hpp"
#include "xor.hpp"

// Consolidated implementations of all opcodes

// Implementation from add.cpp
void handle_add(CPU& cpu, const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t reg1 = program[cpu.get_pc() + 1];
        uint8_t reg2 = program[cpu.get_pc() + 2];
        Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[ADD] │ PC={} R{} += R{}", cpu.get_pc(), "", cpu.get_pc(), reg1, reg2) << std::endl;
        uint8_t before = cpu.get_registers()[reg1];
        cpu.get_registers()[reg1] += cpu.get_registers()[reg2];
        Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[ADD] │ R{}: {} + {} = {}", cpu.get_pc(), "", reg1, before, cpu.get_registers()[reg2], cpu.get_registers()[reg1]) << std::endl;
    }
    cpu.set_pc(cpu.get_pc() + 3);
    cpu.print_state("ADD");
}

// Implementation from and.cpp
void handle_and(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg1 = program[pc + 1];
        uint8_t reg2 = program[pc + 2];

        if (reg1 < cpu.get_registers().size() && reg2 < cpu.get_registers().size()) {
            cpu.get_registers()[reg1] &= cpu.get_registers()[reg2];
        }

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("AND");
}

// Implementation from call.cpp
void handle_call(CPU& cpu,[[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    uint32_t pc = cpu.get_pc();

    // Reset offset at each call
    cpu.set_arg_offset(8);

    uint8_t addr = cpu.fetch_operand();

    Logger::instance().debug() << fmt::format(
        "[PC=0x{:04X}]{:>5}[Call] │ PC=0x{:X}->addr=0x{:X} ret 0x{:X} and FP 0x{:X} to stack at SP=0x{:X}",
        pc, "", pc, addr, (pc + 1), cpu.get_fp(), cpu.get_sp()
    ) << std::endl;

    // Push old FP
    uint32_t sp = cpu.get_sp() - 4;
    cpu.set_sp(sp);
    cpu.write_mem32(sp, cpu.get_fp());

    // Push return address (pc + 1)
    sp -= 4;
    cpu.set_sp(sp);
    cpu.write_mem32(sp, pc + 1);

    // Set new FP
    cpu.set_fp(sp);
    cpu.print_stack_frame("CALL");
    cpu.set_pc(addr);

    Logger::instance().debug() << fmt::format(
        "[PC=0x{:04X}]{:>5}[CALL] │ After jump PC=0x{:X}",
        pc, "", pc
    ) << std::endl;

    cpu.print_state("CALL");
}

// Implementation from cmp.cpp
void handle_cmp(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t reg1 = program[cpu.get_pc() + 1];
        uint8_t reg2 = program[cpu.get_pc() + 2];
        if (reg1 < cpu.get_registers().size() && reg2 < cpu.get_registers().size()) {
            int32_t result = static_cast<int32_t>(cpu.get_registers()[reg1]) - static_cast<int32_t>(cpu.get_registers()[reg2]);
            uint32_t flags = 0;
            if (result == 0) flags |= FLAG_ZERO;
            if (result < 0)  flags |= FLAG_SIGN;
            cpu.set_flags(flags);
        }
        cpu.set_pc(cpu.get_pc() + 3);
    } else {
        running = false;
    }
    cpu.print_state("CMP");
}

// Implementation from db.cpp
void handle_db(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    // DB opcode: Define bytes - copy data to specified address
    if (pc + 2 < program.size()) {
        uint8_t target_addr = program[pc + 1]; // Target memory address
        uint8_t length = program[pc + 2]; // Number of data bytes

        Logger::instance().debug() << fmt::format(
            "[PC=0x{:04X}]{:>7}[DB] │ Copying {} data bytes to address 0x{:02X}",
            pc, "", length, target_addr
        ) << std::endl;

        // Copy data bytes to memory starting at target_addr
        for (uint8_t i = 0; i < length && (pc + 3 + i) < program.size() && (target_addr + i) < cpu.get_memory().size(); ++i) {
            cpu.get_memory()[target_addr + i] = program[pc + 3 + i];

            Logger::instance().debug() << fmt::format(
                "[PC=0x{:04X}]{:>7}[DB] │ memory[0x{:02X}] = 0x{:02X} ('{}')",
                pc, "", target_addr + i, program[pc + 3 + i],
                (program[pc + 3 + i] >= 32 && program[pc + 3 + i] <= 126) ? static_cast<char>(program[pc + 3 + i]) : '.'
            ) << std::endl;
        }

        cpu.set_pc(pc + 3 + length); // Skip opcode + target_addr + length + data bytes
    } else {
        running = false;
    }

    cpu.print_state("DB");
}

// Implementation from dec.cpp
void handle_dec(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 1 < program.size()) {
        uint8_t reg = program[cpu.get_pc() + 1];
        Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>5}[DEC] │ PC={} R{}", cpu.get_pc(), "", cpu.get_pc(), static_cast<int>(reg)) << std::endl;
        if (reg < cpu.get_registers().size()) {
            uint8_t before = cpu.get_registers()[reg];
            --cpu.get_registers()[reg];
            Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>5}[DEC] │ R{}: {} - 1 = {}", cpu.get_pc(), "", static_cast<int>(reg), before, cpu.get_registers()[reg]) << std::endl;
        }
        cpu.set_pc(cpu.get_pc() + 2);
    } else {
        running = false;
    }
    cpu.print_state("DEC");
}

// Implementation from div.cpp
void handle_div(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t reg1 = program[cpu.get_pc() + 1];
        uint8_t reg2 = program[cpu.get_pc() + 2];
        Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[DIV] │ PC={} R{} /= R{}", cpu.get_pc(), "", cpu.get_pc(), reg1, reg2) << std::endl;
        if (reg1 < cpu.get_registers().size() && reg2 < cpu.get_registers().size()) {
            if (cpu.get_registers()[reg2] == 0) {
                Logger::instance().error() << fmt::format("{:>6}Invalid Division │ Division by zero at PC={}", "", cpu.get_pc()) << std::endl;
                running = false;
                return;
            }
            uint8_t before = cpu.get_registers()[reg1];
            cpu.get_registers()[reg1] /= cpu.get_registers()[reg2];
            Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[DIV] │ R{}: {} / {} = {}", cpu.get_pc(), "", reg1, before, cpu.get_registers()[reg2], cpu.get_registers()[reg1]) << std::endl;
        }
        cpu.set_pc(cpu.get_pc() + 3);
    } else {
        running = false;
    }
    cpu.print_state("DIV");
}

// Implementation from halt.cpp
void handle_halt(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    Logger::instance().debug() << fmt::format(
        "[PC=0x{:04X}]{:>5}[HALT] │ PC={}",
        cpu.get_pc(), "", cpu.get_pc()
    ) << std::endl;
    running = false;
    cpu.set_pc(cpu.get_pc() + 1);
    cpu.print_state("HALT");
}

// Implementation from inb.cpp
void handle_inb(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];

        Logger::instance().debug() << fmt::format(
            "[PC=0x{:04X}]{:>6}[INB] │ PC={} R{} <- port {}",
            pc, "", pc, reg, port
        ) << std::endl;

        if (reg < cpu.get_registers().size()) {
            uint8_t value = cpu.read_port(port);
            cpu.get_registers()[reg] = value;

            Logger::instance().debug() << fmt::format(
                "[PC=0x{:04X}]{:>6}[INB] │ R{} = {}",
                pc, "", reg, value
            ) << std::endl;
        }

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("INB");
}

// Implementation from inc.cpp
void handle_inc(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 1 < program.size()) {
        uint8_t reg = program[cpu.get_pc() + 1];
        Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[INC] │ PC={} R{}", cpu.get_pc(), "", cpu.get_pc(), static_cast<int>(reg)) << std::endl;
        if (reg < cpu.get_registers().size()) {
            uint8_t before = cpu.get_registers()[reg];
            ++cpu.get_registers()[reg];
            Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[INC] │ R{}: {} + 1 = {}", cpu.get_pc(), "", static_cast<int>(reg), before, cpu.get_registers()[reg]) << std::endl;
        }
        cpu.set_pc(cpu.get_pc() + 2);
    } else {
        running = false;
    }
    cpu.print_state("INC");
}

// Implementation from in.cpp
void handle_in(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];

        Logger::instance().debug() << fmt::format(
            "[PC=0x{:04X}]{:>7}[IN] │ PC={} R{} <- port {}",
            pc, "", pc, reg, port
        ) << std::endl;

        if (reg < cpu.get_registers().size()) {
            uint8_t value = cpu.read_port(port);
            cpu.get_registers()[reg] = value;

            Logger::instance().debug() << fmt::format(
                "[PC=0x{:04X}]{:>7}[IN] │ R{} = {}",
                pc, "", reg, value
            ) << std::endl;
        }

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("IN");
}

// Implementation from inl.cpp
void handle_inl(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];

        Logger::instance().debug() << fmt::format(
            "[PC=0x{:04X}]{:>7}[INL] │ PC={} R{} <- port {} (dword)",
            pc, "", pc, reg, port
        ) << std::endl;

        if (reg < cpu.get_registers().size()) {
            uint32_t value = cpu.read_port_dword(port);
            // Store bytes in reg, reg+1, reg+2, reg+3 if available
            for (size_t i = 0; i < 4 && (reg + i) < cpu.get_registers().size(); ++i) {
                cpu.get_registers()[reg + i] = static_cast<uint8_t>((value >> (8 * i)) & 0xFF);
            }
        }

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("INL");
}

// Implementation from instr.cpp
void handle_instr(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];

        Logger::instance().debug() << fmt::format(
            "[PC=0x{:04X}]{:>5}[INSTR] │ PC={} R{} <- port {} (string)",
            pc, "", pc, reg, port
        ) << std::endl;

        if (reg < cpu.get_registers().size()) {
            uint8_t maxLength = cpu.get_registers()[reg]; // Use register value as max length
            std::string value = cpu.read_port_string(port, maxLength);

            Logger::instance().debug() << fmt::format(
                "[PC=0x{:04X}]{:>5}[INSTR] │ Read string: '{}'",
                pc, "", value
            ) << std::endl;

            // Store string length in register
            cpu.get_registers()[reg] = static_cast<uint8_t>(value.length());
        }

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("INSTR");
}

// Implementation from inw.cpp
void handle_inw(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];

        Logger::instance().debug() << fmt::format(
            "[PC=0x{:04X}]{:>6}[INW] │ PC={} R{} <- port {} (word)",
            pc, "", pc, reg, port
        ) << std::endl;

        if (reg < cpu.get_registers().size()) {
            uint16_t value = cpu.read_port_word(port);
            // Store lower 8 bits in reg, upper 8 bits in next reg (if exists)
            cpu.get_registers()[reg] = static_cast<uint8_t>(value & 0xFF);
            if (static_cast<size_t>(reg + 1) < cpu.get_registers().size()) {
                cpu.get_registers()[reg + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
            }

            Logger::instance().debug() << fmt::format(
                "[PC=0x{:04X}]{:>6}[INW] │ R{} = {}, R{} = {}",
                pc, "", reg, cpu.get_registers()[reg], reg + 1,
                (static_cast<size_t>(reg + 1) < cpu.get_registers().size() ? cpu.get_registers()[reg + 1] : 0)
            ) << std::endl;
        }

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("INW");
}

// Implementation from jmp.cpp
void handle_jmp(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 1 < program.size()) {
        uint8_t addr = program[cpu.get_pc() + 1];
        // Simple validation - check if address is within program bounds
        if (addr >= program.size()) {
            Logger::instance().error() << std::right << std::setw(23) << std::setfill(' ')
                << "Invalid jump address " << "│ (JMP): "
                << static_cast<int>(addr) << " at PC=" << cpu.get_pc() << std::endl;
            running = false;
            return;
        }
        cpu.set_pc(addr);
    } else {
        running = false;
    }
    cpu.print_state("JMP");
}

// Implementation from jns.cpp
void handle_jns(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 1 < program.size()) {
        uint8_t addr = program[pc + 1];

        if (!(cpu.get_flags() & FLAG_SIGN)) {
            // TODO: Add valid instruction start validation
            cpu.set_pc(addr);
        } else {
            cpu.set_pc(pc + 2);
        }
    } else {
        running = false;
    }

    cpu.print_state("JNS");
}

// Implementation from jnz.cpp
void handle_jnz(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 1 < program.size()) {
        uint8_t addr = program[pc + 1];

        if (!(cpu.get_flags() & FLAG_ZERO)) {
            // TODO: Add valid instruction start validation
            cpu.set_pc(addr);
        } else {
            cpu.set_pc(pc + 2);
        }
    } else {
        running = false;
    }

    cpu.print_state("JNZ");
}

// Implementation from js.cpp
void handle_js(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 1 < program.size()) {
        uint8_t addr = program[pc + 1];

        if (cpu.get_flags() & FLAG_SIGN) {
            // TODO: Add valid instruction start validation
            cpu.set_pc(addr);
        } else {
            cpu.set_pc(pc + 2);
        }
    } else {
        running = false;
    }

    cpu.print_state("JS");
}

// Implementation from jz.cpp
void handle_jz(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 1 < program.size()) {
        uint8_t addr = program[cpu.get_pc() + 1];
        if (cpu.get_flags() & FLAG_ZERO) {
            // Simple validation - check if address is within program bounds
            if (addr >= program.size()) {
                Logger::instance().error() << std::right << std::setw(23) << std::setfill(' ')
                    << "Invalid jump address " << "│ (JZ): "
                    << static_cast<int>(addr) << " at PC=" << cpu.get_pc() << std::endl;
                running = false;
                return;
            }
            cpu.set_pc(addr);
        } else {
            cpu.set_pc(cpu.get_pc() + 2);
        }
    } else {
        running = false;
    }
    cpu.print_state("JZ");
}

// Implementation from load.cpp
void handle_load(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // LOAD reg, addr
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t reg = program[cpu.get_pc() + 1];
        uint8_t addr = program[cpu.get_pc() + 2];
        if (reg < cpu.get_registers().size() && addr < cpu.get_memory().size()) {
            cpu.get_registers()[reg] = cpu.get_memory()[addr];
        }
        cpu.set_pc(cpu.get_pc() + 3);
    } else {
        running = false;
    }
    cpu.print_state("LOAD");
}

// Implementation from load_imm.cpp
void handle_load_imm(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t reg = program[cpu.get_pc() + 1];
        uint8_t imm = program[cpu.get_pc() + 2];
        Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>1}[LOAD_IMM] │ PC=0x{:02X} reg={} imm={}", cpu.get_pc(), "", cpu.get_pc(), reg, imm) << std::endl;
        if (reg < cpu.get_registers().size()) {
            cpu.get_registers()[reg] = imm;
            Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>1}[LOAD_IMM] │ Set R{} = {}", cpu.get_pc(), "", reg, imm) << std::endl;
        }
        cpu.set_pc(cpu.get_pc() + 3);
    } else {
        running = false;
    }
    cpu.print_state("LOAD_IMM");
}

// Implementation for LEA (Load Effective Address)
void handle_lea(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t reg = program[cpu.get_pc() + 1];
        uint8_t addr = program[cpu.get_pc() + 2];
        Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[LEA] │ PC={} Loading address {} into R{}", cpu.get_pc(), "", cpu.get_pc(), addr, reg) << std::endl;
        if (reg < cpu.get_registers().size()) {
            cpu.get_registers()[reg] = addr;  // Load the address itself, not the value at the address
            Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[LEA] │ R{} = 0x{:02X} (address)", cpu.get_pc(), "", reg, addr) << std::endl;
        }
        cpu.set_pc(cpu.get_pc() + 3);
    } else {
        running = false;
    }
    cpu.print_state("LEA");
}

// Implementation from mov.cpp
void handle_mov(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t reg1 = program[cpu.get_pc() + 1];
        uint8_t reg2 = program[cpu.get_pc() + 2];
        if (reg1 < cpu.get_registers().size() && reg2 < cpu.get_registers().size()) {
            cpu.get_registers()[reg1] = cpu.get_registers()[reg2];
        }
        cpu.set_pc(cpu.get_pc() + 3);
    } else {
        running = false;
    }
    cpu.print_state("MOV");
}

// Implementation from mul.cpp
void handle_mul(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t reg1 = program[cpu.get_pc() + 1];
        uint8_t reg2 = program[cpu.get_pc() + 2];
        Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[MUL] │ PC={} R{} *= R{}", cpu.get_pc(), "", cpu.get_pc(), reg1, reg2) << std::endl;
        if (reg1 < cpu.get_registers().size() && reg2 < cpu.get_registers().size()) {
            uint8_t before = cpu.get_registers()[reg1];
            cpu.get_registers()[reg1] *= cpu.get_registers()[reg2];
            Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[MUL] │ R{}: {} * {} = {}", cpu.get_pc(), "", reg1, before, cpu.get_registers()[reg2], cpu.get_registers()[reg1]) << std::endl;
        }
        cpu.set_pc(cpu.get_pc() + 3);
    } else {
        running = false;
    }
    cpu.print_state("MUL");
}

// Implementation from nop.cpp
void handle_nop(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[NOP] │ PC={}", cpu.get_pc(), "", cpu.get_pc()) << std::endl;
    cpu.set_pc(cpu.get_pc() + 1);
    cpu.print_state("NOP");
}

// Implementation from not.cpp
void handle_not(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 1 < program.size()) {
        uint8_t reg = program[pc + 1];

        if (reg < cpu.get_registers().size()) {
            cpu.get_registers()[reg] = ~cpu.get_registers()[reg];
        }

        cpu.set_pc(pc + 2);
    } else {
        running = false;
    }

    cpu.print_state("NOT");
}

// Implementation from or.cpp
void handle_or(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg1 = program[pc + 1];
        uint8_t reg2 = program[pc + 2];

        if (reg1 < cpu.get_registers().size() && reg2 < cpu.get_registers().size()) {
            cpu.get_registers()[reg1] |= cpu.get_registers()[reg2];
        }

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("OR");
}

// Implementation from outb.cpp
void handle_outb(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];

        Logger::instance().debug() << fmt::format(
            "[PC=0x{:04X}]{:>5}[OUTB] │ PC={} port {} <- R{}={}",
            pc, "", pc, port, reg, cpu.get_registers()[reg]
        ) << std::endl;

        if (reg < cpu.get_registers().size()) {
            cpu.write_port(port, cpu.get_registers()[reg]);
        }

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("OUTB");
}

// Implementation from out.cpp
void handle_out(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];

        Logger::instance().debug() << fmt::format(
            "[PC=0x{:04X}]{:>6}[OUT] │ PC={} port {} <- R{}={}",
            pc, "", pc, port, reg, cpu.get_registers()[reg]
        ) << std::endl;

        if (reg < cpu.get_registers().size()) {
            cpu.write_port(port, cpu.get_registers()[reg]);
        }

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("OUT");
}

// Implementation from outl.cpp
void handle_outl(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];

        Logger::instance().debug() << fmt::format(
            "[PC=0x{:04X}]{:>5}[OUTL] │ PC={} port {} <- R{} (dword)",
            pc, "", pc, port, reg
        ) << std::endl;

        if (reg < cpu.get_registers().size()) {
            uint32_t value = 0;
            for (size_t i = 0; i < 4 && (reg + i) < cpu.get_registers().size(); ++i) {
                value |= (static_cast<uint32_t>(cpu.get_registers()[reg + i]) << (8 * i));
            }
            cpu.write_port_dword(port, value);
        }

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("OUTL");
}

// Implementation from outstr.cpp
void handle_outstr(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];

        Logger::instance().debug() << fmt::format(
            "[PC=0x{:04X}]{:>4}[OUTSTR] │ PC={} port {} <- string from memory",
            pc, "", pc, port
        ) << std::endl;

        if (reg < cpu.get_registers().size()) {
            // Build string from memory starting at address in register
            uint8_t addr = cpu.get_registers()[reg];
            std::string str;
            for (size_t i = addr; i < cpu.get_memory().size() && cpu.get_memory()[i] != 0; ++i) {
                str += static_cast<char>(cpu.get_memory()[i]);
            }

            Logger::instance().debug() << fmt::format(
                "[PC=0x{:04X}]{:>4}[OUTSTR] │ Writing string: '{}'",
                pc, "", str
            ) << std::endl;

            cpu.write_port_string(port, str);
        }

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("OUTSTR");
}

// Implementation from outw.cpp
void handle_outw(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];

        Logger::instance().debug() << fmt::format(
            "[PC=0x{:04X}]{:>5}[OUTW] │ PC={} port {} <- R{} (word)",
            pc, "", pc, port, reg
        ) << std::endl;

        if (reg < cpu.get_registers().size()) {
            uint16_t value = cpu.get_registers()[reg];
            if (static_cast<size_t>(reg + 1) < cpu.get_registers().size()) {
                value |= (static_cast<uint16_t>(cpu.get_registers()[reg + 1]) << 8);
            }
            cpu.write_port_word(port, value);
        }

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("OUTW");
}

// Implementation from pop_arg.cpp
void handle_pop_arg(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    uint32_t pc = cpu.get_pc();
    uint8_t reg = cpu.fetch_operand();

    cpu.get_registers()[reg] = cpu.read_mem32(cpu.get_fp() + cpu.get_arg_offset());

    Logger::instance().debug() << fmt::format(
        "[PC=0x{:04X}]{:>2}[POP_ARG] │ FP={} arg_offset={} addr={} value={}",
        pc, "", cpu.get_fp(), cpu.get_arg_offset(),
        (cpu.get_fp() + cpu.get_arg_offset()), cpu.get_registers()[reg]
    ) << std::endl;

    cpu.set_arg_offset(cpu.get_arg_offset() + 4);
    cpu.set_pc(pc + 1);
    cpu.print_state("POP_ARG");
}

// Implementation from pop.cpp
void handle_pop(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 1 < program.size()) {
        uint8_t reg = program[cpu.get_pc() + 1];
        cpu.get_registers()[reg] = cpu.read_mem32(cpu.get_sp());
        Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[POP] │ PC={} Popping to R{}={}", cpu.get_pc(), "", cpu.get_pc(), static_cast<int>(reg), cpu.get_registers()[reg]) << std::endl;
        cpu.set_sp(cpu.get_sp() + 4);
        cpu.set_pc(cpu.get_pc() + 2);
    } else {
        running = false;
    }
    cpu.print_state("POP");
}

// Implementation from pop_flag.cpp
void handle_pop_flag(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>5}[POPF] │ PC={} Popping FLAGS={:08X}", cpu.get_pc(), "", cpu.get_pc(), cpu.get_flags()) << std::endl;
    cpu.set_flags(cpu.read_mem32(cpu.get_sp()));
    cpu.set_sp(cpu.get_sp() + 4);
    cpu.set_pc(cpu.get_pc() + 1);  // POP_FLAG is a single-byte instruction
    cpu.print_state("POPF");
}

// Implementation from push_arg.cpp
void handle_push_arg(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    uint32_t pc = cpu.get_pc();
    uint8_t reg = cpu.fetch_operand();

    Logger::instance().debug() << fmt::format(
        "[PC=0x{:04X}]{:>1}[PUSH_ARG] │ SP={} Pushing R{}={}",
        pc, "", cpu.get_sp(), static_cast<int>(reg), cpu.get_registers()[reg]
    ) << std::endl;

    uint32_t sp = cpu.get_sp() - 4;
    cpu.set_sp(sp);
    cpu.write_mem32(sp, cpu.get_registers()[reg]);

    cpu.set_pc(pc + 1);
    cpu.print_state("PUSH_ARG");
}

// Implementation from push.cpp
void handle_push(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 1 < program.size()) {
        uint8_t reg = program[cpu.get_pc() + 1];
        Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>5}[PUSH] │ PC={} Pushing R{}={}", cpu.get_pc(), "", cpu.get_pc(), static_cast<int>(reg), cpu.get_registers()[reg]) << std::endl;
        cpu.set_sp(cpu.get_sp() - 4);
        cpu.write_mem32(cpu.get_sp(), cpu.get_registers()[reg]);
        cpu.set_pc(cpu.get_pc() + 2);
    } else {
        running = false;
    }
    cpu.print_state("PUSH");
}

// Implementation from push_flag.cpp
void handle_push_flag(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>4}[PUSHF] │ PC={} Pushing FLAGS={:08X}", cpu.get_pc(), "", cpu.get_pc(), cpu.get_flags()) << std::endl;
    cpu.set_sp(cpu.get_sp() - 4);
    cpu.write_mem32(cpu.get_sp(), cpu.get_flags());
    cpu.set_pc(cpu.get_pc() + 1);  // PUSH_FLAG is a single-byte instruction
    cpu.print_state("PUSHF");
}

// Implementation from ret.cpp
void handle_ret(CPU& cpu, [[maybe_unused]] const std::vector<uint8_t>& program, [[maybe_unused]] bool& running) {
    uint32_t pc = cpu.get_pc();
    uint32_t sp = cpu.get_sp();

    Logger::instance().debug() << fmt::format(
        "[PC=0x{:04X}]{:>6}[RET] │ SP={} Restoring FP and popping return address",
        pc, "", sp
    ) << std::endl;

    // Save return value before unwinding stack
    uint32_t ret_val = cpu.read_mem32(sp);         // return value is at sp
    uint32_t ret_addr = cpu.read_mem32(sp + 4);    // return address
    uint32_t old_fp = cpu.read_mem32(sp + 8);      // old frame pointer

    // Unwind stack
    sp += 12;
    cpu.set_sp(sp);
    cpu.set_fp(old_fp);

    // Write return value to caller's stack frame (at fp + 0)
    cpu.write_mem32(cpu.get_fp(), ret_val);

    cpu.print_stack_frame("RET");
    cpu.set_pc(ret_addr);

    // Reset offset at each return
    cpu.set_arg_offset(0);

    cpu.print_state("RET");
}

// Implementation from shl.cpp
void handle_shl(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t imm = program[pc + 2];

        if (reg < cpu.get_registers().size()) {
            cpu.get_registers()[reg] <<= imm;
        }

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("SHL");
}

// Implementation from shr.cpp
void handle_shr(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t imm = program[pc + 2];

        if (reg < cpu.get_registers().size()) {
            cpu.get_registers()[reg] >>= imm;
        }

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("SHR");
}

// Implementation from store.cpp
void handle_store(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // STORE reg, addr
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t reg = program[cpu.get_pc() + 1];
        uint8_t addr = program[cpu.get_pc() + 2];
        if (reg < cpu.get_registers().size() && addr < cpu.get_memory().size()) {
            cpu.get_memory()[addr] = cpu.get_registers()[reg];
        }
        cpu.set_pc(cpu.get_pc() + 3);
    } else {
        running = false;
    }
    cpu.print_state("STORE");
}

// Implementation from sub.cpp
void handle_sub(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t reg1 = program[cpu.get_pc() + 1];
        uint8_t reg2 = program[cpu.get_pc() + 2];
        Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[SUB] │ PC={} R{} -= R{}", cpu.get_pc(), "", cpu.get_pc(), reg1, reg2) << std::endl;
        if (reg1 < cpu.get_registers().size() && reg2 < cpu.get_registers().size()) {
            uint8_t before = cpu.get_registers()[reg1];
            cpu.get_registers()[reg1] -= cpu.get_registers()[reg2];
            Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[SUB] │ R{}: {} - {} = {}", cpu.get_pc(), "", reg1, before, cpu.get_registers()[reg2], cpu.get_registers()[reg1]) << std::endl;
        }
        cpu.set_pc(cpu.get_pc() + 3);
    } else {
        running = false;
    }
    cpu.print_state("SUB");
}

// Implementation for SWAP
void handle_swap(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t reg = program[cpu.get_pc() + 1];
        uint8_t addr = program[cpu.get_pc() + 2];
        Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>5}[SWAP] │ PC={} Swapping R{} with memory[{}]", cpu.get_pc(), "", cpu.get_pc(), reg, addr) << std::endl;

        if (reg < cpu.get_registers().size() && addr < cpu.get_memory().size()) {
            uint32_t temp = cpu.get_registers()[reg];
            cpu.get_registers()[reg] = cpu.get_memory()[addr];
            cpu.get_memory()[addr] = temp;
            Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>5}[SWAP] │ R{} = {}, memory[{}] = {}", cpu.get_pc(), "", reg, cpu.get_registers()[reg], addr, cpu.get_memory()[addr]) << std::endl;
        }
        cpu.set_pc(cpu.get_pc() + 3);
    } else {
        running = false;
    }
    cpu.print_state("SWAP");
}

// Implementation from xor.cpp
void handle_xor(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg1 = program[pc + 1];
        uint8_t reg2 = program[pc + 2];

        if (reg1 < cpu.get_registers().size() && reg2 < cpu.get_registers().size()) {
            cpu.get_registers()[reg1] ^= cpu.get_registers()[reg2];
        }

        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }

    cpu.print_state("XOR");
}

// Dispatcher function (copied from opcode_dispatcher.cpp)
void dispatch_opcode(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() >= program.size()) {
        running = false;
        return;
    }

    Opcode opcode = static_cast<Opcode>(program[cpu.get_pc()]);

    switch (opcode) {
        case Opcode::NOP:
            handle_nop(cpu, program, running);
            break;
        case Opcode::LOAD_IMM:
            handle_load_imm(cpu, program, running);
            break;
        case Opcode::ADD:
            handle_add(cpu, program, running);
            break;
        case Opcode::SUB:
            handle_sub(cpu, program, running);
            break;
        case Opcode::MUL:
            handle_mul(cpu, program, running);
            break;
        case Opcode::DIV:
            handle_div(cpu, program, running);
            break;
        case Opcode::INC:
            handle_inc(cpu, program, running);
            break;
        case Opcode::DEC:
            handle_dec(cpu, program, running);
            break;
        case Opcode::MOV:
            handle_mov(cpu, program, running);
            break;
        case Opcode::JMP:
            handle_jmp(cpu, program, running);
            break;
        case Opcode::JZ:
            handle_jz(cpu, program, running);
            break;
        case Opcode::JNZ:
            handle_jnz(cpu, program, running);
            break;
        case Opcode::JS:
            handle_js(cpu, program, running);
            break;
        case Opcode::JNS:
            handle_jns(cpu, program, running);
            break;
        case Opcode::LOAD:
            handle_load(cpu, program, running);
            break;
        case Opcode::LEA:
            handle_lea(cpu, program, running);
            break;
        case Opcode::STORE:
            handle_store(cpu, program, running);
            break;
        case Opcode::SWAP:
            handle_swap(cpu, program, running);
            break;
        case Opcode::PUSH:
            handle_push(cpu, program, running);
            break;
        case Opcode::POP:
            handle_pop(cpu, program, running);
            break;
        case Opcode::CMP:
            handle_cmp(cpu, program, running);
            break;
        case Opcode::PUSH_FLAG:
            handle_push_flag(cpu, program, running);
            break;
        case Opcode::POP_FLAG:
            handle_pop_flag(cpu, program, running);
            break;
        case Opcode::HALT:
            handle_halt(cpu, program, running);
            break;
        case Opcode::AND:
            handle_and(cpu, program, running);
            break;
        case Opcode::OR:
            handle_or(cpu, program, running);
            break;
        case Opcode::XOR:
            handle_xor(cpu, program, running);
            break;
        case Opcode::NOT:
            handle_not(cpu, program, running);
            break;
        case Opcode::SHL:
            handle_shl(cpu, program, running);
            break;
        case Opcode::SHR:
            handle_shr(cpu, program, running);
            break;
        case Opcode::CALL:
            handle_call(cpu, program, running);
            break;
        case Opcode::RET:
            handle_ret(cpu, program, running);
            break;
        case Opcode::PUSH_ARG:
            handle_push_arg(cpu, program, running);
            break;
        case Opcode::POP_ARG:
            handle_pop_arg(cpu, program, running);
            break;
        case Opcode::IN:
            handle_in(cpu, program, running);
            break;
        case Opcode::OUT:
            handle_out(cpu, program, running);
            break;
        case Opcode::INB:
            handle_inb(cpu, program, running);
            break;
        case Opcode::OUTB:
            handle_outb(cpu, program, running);
            break;
        case Opcode::INW:
            handle_inw(cpu, program, running);
            break;
        case Opcode::OUTW:
            handle_outw(cpu, program, running);
            break;
        case Opcode::INL:
            handle_inl(cpu, program, running);
            break;
        case Opcode::OUTL:
            handle_outl(cpu, program, running);
            break;
        case Opcode::INSTR:
            handle_instr(cpu, program, running);
            break;
        case Opcode::OUTSTR:
            handle_outstr(cpu, program, running);
            break;
        case Opcode::DB:
            handle_db(cpu, program, running);
            break;
        default:
            Logger::instance().error()
                << std::right << std::setw(23) << std::setfill(' ') << "Invalid opcode " << "│ "
                << "Unknown opcode 0x"
                << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(opcode)
                << " at PC=" << std::dec << cpu.get_pc() << std::endl;
            running = false;
            break;
    }
}
