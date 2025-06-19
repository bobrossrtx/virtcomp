#include "inw.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

void handle_inw(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    
    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];
        
        Logger::instance().debug() << fmt::format(
            "[PC=0x{:04X}]{:>6}[INW] │ PC={} R{} <- port {} (word)", 
            pc, "", pc, reg, port
        ) << std::endl;
        
        if (reg < cpu.get_registers().size()) {
            uint16_t value = cpu.read_port_word(port);
            // Store lower 8 bits in reg, upper 8 bits in next reg (if exists)
            cpu.get_registers()[reg] = static_cast<uint8_t>(value & 0xFF);
            if (reg + 1 < cpu.get_registers().size()) {
                cpu.get_registers()[reg + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
            }
            
            Logger::instance().debug() << fmt::format(
                "[PC=0x{:04X}]{:>6}[INW] │ R{} = {}, R{} = {}", 
                pc, "", reg, cpu.get_registers()[reg], reg + 1, 
                (reg + 1 < cpu.get_registers().size() ? cpu.get_registers()[reg + 1] : 0)
            ) << std::endl;
        }
        
        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }
    
    cpu.print_state("INW");
}
