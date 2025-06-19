#include "store.hpp"
#include "../cpu.hpp"

void handle_store(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    // STORE reg, addr
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t reg = program[cpu.get_pc() + 1];
        uint8_t addr = program[cpu.get_pc() + 2];
        if (reg < cpu.get_registers().size() && addr < cpu.get_memory().size()) {
            cpu.get_memory()[addr] = cpu.get_registers()[reg];
        }
        cpu.set_pc(cpu.get_pc() + 3);
    } else {
        running = false;
    }
    cpu.print_state("STORE");
}
