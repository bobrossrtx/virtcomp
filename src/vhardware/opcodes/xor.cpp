#include "xor.hpp"
#include "../cpu.hpp"

void handle_xor(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    
    if (pc + 2 < program.size()) {
        uint8_t reg1 = program[pc + 1];
        uint8_t reg2 = program[pc + 2];
        
        if (reg1 < cpu.get_registers().size() && reg2 < cpu.get_registers().size()) {
            cpu.get_registers()[reg1] ^= cpu.get_registers()[reg2];
        }
        
        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }
    
    cpu.print_state("XOR");
}
