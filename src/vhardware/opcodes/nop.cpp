#include "nop.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

void handle_nop(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>6}[NOP] │ PC={}", cpu.get_pc(), "", cpu.get_pc()) << std::endl;
    cpu.set_pc(cpu.get_pc() + 1);
    cpu.print_state("NOP");
}
