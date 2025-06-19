#include "load_imm.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

void handle_load_imm(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t reg = program[cpu.get_pc() + 1];
        uint8_t imm = program[cpu.get_pc() + 2];
        Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>1}[LOAD_IMM]│ PC={} reg={} imm={}", cpu.get_pc(), "", cpu.get_pc(), reg, imm) << std::endl;
        if (reg < cpu.get_registers().size()) {
            cpu.get_registers()[reg] = imm;
            Logger::instance().debug() << fmt::format("[PC=0x{:04X}]{:>1}[LOAD_IMM]│ Set R{} = {}", cpu.get_pc(), "", reg, imm) << std::endl;
        }
        cpu.set_pc(cpu.get_pc() + 3);
    } else {
        running = false;
    }
    cpu.print_state("LOAD_IMM");
}
