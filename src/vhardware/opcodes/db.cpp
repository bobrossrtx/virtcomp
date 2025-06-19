#include "db.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

void handle_db(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    
    // DB opcode: Define bytes - copy data to specified address
    if (pc + 2 < program.size()) {
        uint8_t target_addr = program[pc + 1]; // Target memory address
        uint8_t length = program[pc + 2]; // Number of data bytes
        
        Logger::instance().debug() << fmt::format(
            "[PC=0x{:04X}]{:>7}[DB] │ Copying {} data bytes to address 0x{:02X}",
            pc, "", length, target_addr
        ) << std::endl;

        // Copy data bytes to memory starting at target_addr
        for (uint8_t i = 0; i < length && (pc + 3 + i) < program.size() && (target_addr + i) < cpu.get_memory().size(); ++i) {
            cpu.get_memory()[target_addr + i] = program[pc + 3 + i];
            
            Logger::instance().debug() << fmt::format(
                "[PC=0x{:04X}]{:>7}[DB] │ memory[0x{:02X}] = 0x{:02X} ('{}')",
                pc, "", target_addr + i, program[pc + 3 + i],
                (program[pc + 3 + i] >= 32 && program[pc + 3 + i] <= 126) ? static_cast<char>(program[pc + 3 + i]) : '.'
            ) << std::endl;
        }

        cpu.set_pc(pc + 3 + length); // Skip opcode + target_addr + length + data bytes
    } else {
        running = false;
    }
    
    cpu.print_state("DB");
}
