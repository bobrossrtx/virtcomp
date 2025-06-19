#include "shl.hpp"
#include "../cpu.hpp"

void handle_shl(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    
    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t imm = program[pc + 2];
        
        if (reg < cpu.get_registers().size()) {
            cpu.get_registers()[reg] <<= imm;
        }
        
        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }
    
    cpu.print_state("SHL");
}
