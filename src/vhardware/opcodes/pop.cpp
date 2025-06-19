#include "pop.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

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
