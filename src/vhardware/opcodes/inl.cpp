#include "inl.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

void handle_inl(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    
    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];
        
        Logger::instance().debug() << fmt::format(
            "[PC=0x{:04X}]{:>7}[INL] │ PC={} R{} <- port {} (dword)", 
            pc, "", pc, reg, port
        ) << std::endl;
        
        if (reg < cpu.get_registers().size()) {
            uint32_t value = cpu.read_port_dword(port);
            // Store bytes in reg, reg+1, reg+2, reg+3 if available
            for (int i = 0; i < 4 && (reg + i) < cpu.get_registers().size(); ++i) {
                cpu.get_registers()[reg + i] = static_cast<uint8_t>((value >> (8 * i)) & 0xFF);
            }
        }
        
        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }
    
    cpu.print_state("INL");
}
