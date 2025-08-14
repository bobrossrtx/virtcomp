#include "movex.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>
#include <iomanip>

using Logging::Logger;

void handle_movex(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg1 = program[pc + 1];
        uint8_t reg2 = program[pc + 2];

        // Debug output for operation
        Logger::instance().debug() << fmt::format(
            "[PC=0x{:04X}] [MOVEX] Moving value between extended registers {} and {}",
            pc, reg1, reg2) << std::endl;

        // Get the source register value in 64-bit mode
        uint64_t src_value = cpu.get_register_64(static_cast<Register>(reg2));
        
        // Move to destination register in 64-bit mode
        cpu.set_register_64(static_cast<Register>(reg1), src_value);

        // Debug output for completed operation
        Logger::instance().debug() << fmt::format(
            "[PC=0x{:04X}] [MOVEX] Moved value 0x{:016X} from R{} to R{}",
            pc, src_value, reg2, reg1) << std::endl;

        cpu.set_pc(pc + 3); // Advance past opcode and two register operands
    } else {
        running = false;
    }

    cpu.print_state("MOVEX");
}
