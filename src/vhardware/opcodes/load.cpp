#include "load.hpp"
#include "../cpu.hpp"

void handle_load(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // LOAD reg, addr
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t reg = program[cpu.get_pc() + 1];
        uint8_t addr = program[cpu.get_pc() + 2];
        if (reg < cpu.get_registers().size() && addr < cpu.get_memory().size()) {
            cpu.get_registers()[reg] = cpu.get_memory()[addr];
        }
        cpu.set_pc(cpu.get_pc() + 3);
    } else {
        running = false;
    }
    cpu.print_state("LOAD");
}
