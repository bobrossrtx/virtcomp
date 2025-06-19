#include "outstr.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

void handle_outstr(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    
    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];
        
        Logger::instance().debug() << fmt::format(
            "[PC=0x{:04X}]{:>4}[OUTSTR] │ PC={} port {} <- string from memory", 
            pc, "", pc, port
        ) << std::endl;
        
        if (reg < cpu.get_registers().size()) {
            // Build string from memory starting at address in register
            uint8_t addr = cpu.get_registers()[reg];
            std::string str;
            for (size_t i = addr; i < cpu.get_memory().size() && cpu.get_memory()[i] != 0; ++i) {
                str += static_cast<char>(cpu.get_memory()[i]);
            }
            
            Logger::instance().debug() << fmt::format(
                "[PC=0x{:04X}]{:>4}[OUTSTR] │ Writing string: '{}'", 
                pc, "", str
            ) << std::endl;
            
            cpu.write_port_string(port, str);
        }
        
        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }
    
    cpu.print_state("OUTSTR");
}
