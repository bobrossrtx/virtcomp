#include "inb.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

void handle_inb(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    
    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];
        
        Logger::instance().debug() << fmt::format(
            "[PC=0x{:04X}]{:>6}[INB] │ PC={} R{} <- port {}", 
            pc, "", pc, reg, port
        ) << std::endl;
        
        if (reg < cpu.get_registers().size()) {
            uint8_t value = cpu.read_port(port);
            cpu.get_registers()[reg] = value;
            
            Logger::instance().debug() << fmt::format(
                "[PC=0x{:04X}]{:>6}[INB] │ R{} = {}", 
                pc, "", reg, value
            ) << std::endl;
        }
        
        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }
    
    cpu.print_state("INB");
}
