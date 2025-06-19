#include "push_flag.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

void handle_push_flag(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>4}[PUSHF] │ PC={} Pushing FLAGS={:08X}", cpu.get_pc(), "", cpu.get_pc(), cpu.get_flags()) << std::endl;
    cpu.set_sp(cpu.get_sp() - 4);
    cpu.write_mem32(cpu.get_sp(), cpu.get_flags());
    cpu.set_pc(cpu.get_pc() + 1);  // PUSH_FLAG is a single-byte instruction
    cpu.print_state("PUSHF");
}
