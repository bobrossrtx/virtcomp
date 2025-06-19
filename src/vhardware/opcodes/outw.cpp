#include "outw.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

void handle_outw(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    
    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];
        
        Logger::instance().debug() << fmt::format(
            "[PC=0x{:04X}]{:>5}[OUTW] │ PC={} port {} <- R{} (word)", 
            pc, "", pc, port, reg
        ) << std::endl;
        
        if (reg < cpu.get_registers().size()) {
            uint16_t value = cpu.get_registers()[reg];
            if (reg + 1 < cpu.get_registers().size()) {
                value |= (static_cast<uint16_t>(cpu.get_registers()[reg + 1]) << 8);
            }
            cpu.write_port_word(port, value);
        }
        
        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }
    
    cpu.print_state("OUTW");
}
