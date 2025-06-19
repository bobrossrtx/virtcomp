#include "instr.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

void handle_instr(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    
    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];
        
        Logger::instance().debug() << fmt::format(
            "[PC=0x{:04X}]{:>5}[INSTR] │ PC={} R{} <- port {} (string)", 
            pc, "", pc, reg, port
        ) << std::endl;
        
        if (reg < cpu.get_registers().size()) {
            uint8_t maxLength = cpu.get_registers()[reg]; // Use register value as max length
            std::string value = cpu.read_port_string(port, maxLength);
            
            Logger::instance().debug() << fmt::format(
                "[PC=0x{:04X}]{:>5}[INSTR] │ Read string: '{}'", 
                pc, "", value
            ) << std::endl;
            
            // Store string length in register
            cpu.get_registers()[reg] = static_cast<uint8_t>(value.length());
        }
        
        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }
    
    cpu.print_state("INSTR");
}
