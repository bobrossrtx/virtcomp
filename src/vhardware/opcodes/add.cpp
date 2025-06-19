#include "add.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

void handle_add(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
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
