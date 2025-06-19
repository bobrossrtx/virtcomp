#include "cmp.hpp"
#include "../cpu.hpp"
#include "../cpu_flags.hpp"

void handle_cmp(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 2 < program.size()) {
        uint8_t reg1 = program[cpu.get_pc() + 1];
        uint8_t reg2 = program[cpu.get_pc() + 2];
        if (reg1 < cpu.get_registers().size() && reg2 < cpu.get_registers().size()) {
            int32_t result = static_cast<int32_t>(cpu.get_registers()[reg1]) - static_cast<int32_t>(cpu.get_registers()[reg2]);
            uint32_t flags = 0;
            if (result == 0) flags |= FLAG_ZERO;
            if (result < 0)  flags |= FLAG_SIGN;
            cpu.set_flags(flags);
        }
        cpu.set_pc(cpu.get_pc() + 3);
    } else {
        running = false;
    }
    cpu.print_state("CMP");
}
