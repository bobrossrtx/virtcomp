#include "opcode_dispatcher.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>
#include <iomanip>

// Include all opcode handlers
#include "nop.hpp"
#include "load_imm.hpp"
#include "add.hpp"
#include "sub.hpp"
#include "mul.hpp"
#include "div.hpp"
#include "inc.hpp"
#include "dec.hpp"
#include "mov.hpp"
#include "jmp.hpp"
#include "jz.hpp"
#include "jnz.hpp"
#include "js.hpp"
#include "jns.hpp"
#include "load.hpp"
#include "store.hpp"
#include "push.hpp"
#include "pop.hpp"
#include "cmp.hpp"
#include "push_flag.hpp"
#include "pop_flag.hpp"
#include "and.hpp"
#include "or.hpp"
#include "xor.hpp"
#include "not.hpp"
#include "shl.hpp"
#include "shr.hpp"
#include "call.hpp"
#include "ret.hpp"
#include "push_arg.hpp"
#include "pop_arg.hpp"
#include "in.hpp"
#include "out.hpp"
#include "inb.hpp"
#include "outb.hpp"
#include "inw.hpp"
#include "outw.hpp"
#include "inl.hpp"
#include "outl.hpp"
#include "instr.hpp"
#include "outstr.hpp"
#include "db.hpp"
#include "halt.hpp"

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
        
        // Bitwise operations
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
        
        // Function call operations
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
        
        // I/O operations
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
        
        // Data definition
        case Opcode::DB:
            handle_db(cpu, program, running);
            break;

        // TODO: Add remaining opcodes as we create their handlers
        default: {
            // Handle unknown opcode
            Logger::instance().error()
                << std::right << std::setw(23) << std::setfill(' ') << "Invalid opcode " << "│ "
                << "opcode: 0x"
                << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(opcode)
                << " │ is not defined" << std::dec << std::endl;
            running = false;
            cpu.print_state("UNKNOWN");
            break;
        }
    }
}
