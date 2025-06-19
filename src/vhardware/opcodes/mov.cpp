#include "mov.hpp"
#include "../cpu.hpp"

void handle_mov(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t reg1 = program[cpu.get_pc() + 1];
        uint8_t reg2 = program[cpu.get_pc() + 2];
        if (reg1 < cpu.get_registers().size() && reg2 < cpu.get_registers().size()) {
            cpu.get_registers()[reg1] = cpu.get_registers()[reg2];
        }
        cpu.set_pc(cpu.get_pc() + 3);
    } else {
        running = false;
    }
    cpu.print_state("MOV");
}
