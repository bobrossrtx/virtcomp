#include "inc.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

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
