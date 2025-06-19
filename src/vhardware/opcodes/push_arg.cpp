#include "push_arg.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

void handle_push_arg(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
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
