#include "pop_arg.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

void handle_pop_arg(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
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
