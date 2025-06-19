#include "js.hpp"
#include "../cpu.hpp"
#include "../cpu_flags.hpp"

void handle_js(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    
    if (pc + 1 < program.size()) {
        uint8_t addr = program[pc + 1];
        
        if (cpu.get_flags() & FLAG_SIGN) {
            // TODO: Add valid instruction start validation
            cpu.set_pc(addr);
        } else {
            cpu.set_pc(pc + 2);
        }
    } else {
        running = false;
    }
    
    cpu.print_state("JS");
}
