#include "pop_flag.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

void handle_pop_flag(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>5}[POPF] │ PC={} Popping FLAGS={:08X}", cpu.get_pc(), "", cpu.get_pc(), cpu.get_flags()) << std::endl;
    cpu.set_flags(cpu.read_mem32(cpu.get_sp()));
    cpu.set_sp(cpu.get_sp() + 4);
    cpu.set_pc(cpu.get_pc() + 1);  // POP_FLAG is a single-byte instruction
    cpu.print_state("POPF");
}
