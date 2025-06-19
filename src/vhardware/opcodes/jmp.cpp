#include "jmp.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

void handle_jmp(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 1 < program.size()) {
        uint8_t addr = program[cpu.get_pc() + 1];
        // Simple validation - check if address is within program bounds
        if (addr >= program.size()) {
            Logger::instance().error() << std::right << std::setw(23) << std::setfill(' ')
                << "Invalid jump address " << "│ (JMP): "
                << static_cast<int>(addr) << " at PC=" << cpu.get_pc() << std::endl;
            running = false;
            return;
        }
        cpu.set_pc(addr);
    } else {
        running = false;
    }
    cpu.print_state("JMP");
}
