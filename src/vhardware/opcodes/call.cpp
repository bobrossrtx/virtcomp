#include "call.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

void handle_call(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
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
