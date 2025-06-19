#include "not.hpp"
#include "../cpu.hpp"

void handle_not(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    
    if (pc + 1 < program.size()) {
        uint8_t reg = program[pc + 1];
        
        if (reg < cpu.get_registers().size()) {
            cpu.get_registers()[reg] = ~cpu.get_registers()[reg];
        }
        
        cpu.set_pc(pc + 2);
    } else {
        running = false;
    }
    
    cpu.print_state("NOT");
}
