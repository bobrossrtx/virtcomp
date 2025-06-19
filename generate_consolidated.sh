#!/bin/bash

# Generate consolidated opcodes file
OUTFILE="src/vhardware/opcodes/opcodes_consolidated.cpp"

cat > "$OUTFILE" << 'HEADER'
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
HEADER

# Add all header includes
for file in src/vhardware/opcodes/*.hpp; do
    if [[ "$file" != *"opcode_dispatcher.hpp"* ]]; then
        echo "#include \"$(basename "$file")\"" >> "$OUTFILE"
    fi
done

cat >> "$OUTFILE" << 'MIDDLE'

// Consolidated implementations of all opcodes
MIDDLE

# Add all implementations by copying from the .cpp files
for file in src/vhardware/opcodes/*.cpp; do
    if [[ "$file" != *"opcode_dispatcher.cpp"* ]] && [[ "$file" != *"opcodes_consolidated.cpp"* ]]; then
        echo "" >> "$OUTFILE"
        echo "// Implementation from $(basename "$file")" >> "$OUTFILE"
        # Extract just the function implementation, skip includes and using declarations
        sed -n '/^void handle_/,/^}$/p' "$file" >> "$OUTFILE"
    fi
done

# Add the dispatcher function
cat >> "$OUTFILE" << 'FOOTER'

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
        case Opcode::STORE:
            handle_store(cpu, program, running);
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
FOOTER

echo "Generated consolidated opcodes file: $OUTFILE"
