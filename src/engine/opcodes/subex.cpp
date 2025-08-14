#include "subex.hpp"
#include "../cpu.hpp"
#include "../cpu_flags.hpp"
#include "../../debug/logger.hpp"
#include <fmt/core.h>

using Logging::Logger;

void handle_subex(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();

    if (pc + 2 < program.size()) {
        uint8_t reg1 = program[pc + 1];
        uint8_t reg2 = program[pc + 2];

        // Debug output for operation
        Logger::instance().debug() << fmt::format(
            "[PC=0x{:04X}] [SUBEX] Subtracting extended registers {} and {}",
            pc, reg1, reg2) << std::endl;

        // Get register values in 64-bit mode
        uint64_t value1 = cpu.get_register_64(static_cast<Register>(reg1));
        uint64_t value2 = cpu.get_register_64(static_cast<Register>(reg2));
        uint64_t result = value1 - value2;

        // Update flags for 64-bit arithmetic
        uint32_t flags = cpu.get_flags();

        // Check for borrow (carry in subtraction)
        if (value1 < value2) { // Underflow in unsigned subtraction
            flags |= FLAG_CARRY;
        } else {
            flags &= ~FLAG_CARRY;
        }

        // Check for signed overflow
        bool sign1 = (value1 >> 63) & 1;
        bool sign2 = (value2 >> 63) & 1;
        bool signr = (result >> 63) & 1;
        
        if ((sign1 != sign2) && (sign1 != signr)) {
            flags |= FLAG_OVERFLOW;
        } else {
            flags &= ~FLAG_OVERFLOW;
        }

        // Check for zero
        if (result == 0) {
            flags |= FLAG_ZERO;
        } else {
            flags &= ~FLAG_ZERO;
        }

        // Check for negative (using sign bit)
        if (signr) {
            flags |= FLAG_SIGN;
        } else {
            flags &= ~FLAG_SIGN;
        }

        // Update flags and register
        cpu.set_flags(flags);
        cpu.set_register_64(static_cast<Register>(reg1), result);

        // Debug output for completed operation with flags
        Logger::instance().debug() << fmt::format(
            "[PC=0x{:04X}] [SUBEX] Result: 0x{:016X} = 0x{:016X} - 0x{:016X} (Flags: C={} O={} S={} Z={})",
            pc, result, value1, value2,
            (flags & FLAG_CARRY) ? 1 : 0,
            (flags & FLAG_OVERFLOW) ? 1 : 0,
            (flags & FLAG_SIGN) ? 1 : 0,
            (flags & FLAG_ZERO) ? 1 : 0) << std::endl;

        cpu.set_pc(pc + 3); // Advance past opcode and two register operands
    } else {
        running = false;
    }

    cpu.print_state("SUBEX");
}
