#include "ret.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

void handle_ret(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
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
