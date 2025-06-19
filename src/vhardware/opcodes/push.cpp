#include "push.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

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
