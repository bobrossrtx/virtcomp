#include "outl.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

void handle_outl(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    
    if (pc + 2 < program.size()) {
        uint8_t reg = program[pc + 1];
        uint8_t port = program[pc + 2];
        
        Logger::instance().debug() << fmt::format(
            "[PC=0x{:04X}]{:>5}[OUTL] │ PC={} port {} <- R{} (dword)", 
            pc, "", pc, port, reg
        ) << std::endl;
        
        if (reg < cpu.get_registers().size()) {
            uint32_t value = 0;
            for (int i = 0; i < 4 && (reg + i) < cpu.get_registers().size(); ++i) {
                value |= (static_cast<uint32_t>(cpu.get_registers()[reg + i]) << (8 * i));
            }
            cpu.write_port_dword(port, value);
        }
        
        cpu.set_pc(pc + 3);
    } else {
        running = false;
    }
    
    cpu.print_state("OUTL");
}
