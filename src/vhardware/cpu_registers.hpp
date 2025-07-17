#pragma once
#include <cstdint>
#include <string>

/**
 * VirtComp Extended Register Architecture
 *
 * Implements a comprehensive x64-style register set for the VirtComp virtual machine.
 * This provides the foundation for advanced programming language compilation targets.
 */

namespace VirtComp_Registers {

// Register enumeration for 64-bit x64-style architecture
enum class Register : uint8_t {
    // General Purpose Registers (64-bit)
    RAX = 0,    // Accumulator register
    RCX = 1,    // Counter register
    RDX = 2,    // Data register
    RBX = 3,    // Base register
    RSP = 4,    // Stack pointer
    RBP = 5,    // Base pointer (frame pointer)
    RSI = 6,    // Source index
    RDI = 7,    // Destination index
    R8  = 8,    // Extended general purpose
    R9  = 9,    // Extended general purpose
    R10 = 10,   // Extended general purpose
    R11 = 11,   // Extended general purpose
    R12 = 12,   // Extended general purpose
    R13 = 13,   // Extended general purpose
    R14 = 14,   // Extended general purpose
    R15 = 15,   // Extended general purpose

    // Segment Registers (16-bit, but stored as 64-bit for consistency)
    CS = 16,    // Code segment
    DS = 17,    // Data segment
    ES = 18,    // Extra segment
    FS = 19,    // General segment F
    GS = 20,    // General segment G
    SS = 21,    // Stack segment

    // Control Registers (64-bit)
    CR0 = 22,   // Control register 0
    CR1 = 23,   // Control register 1 (reserved)
    CR2 = 24,   // Control register 2 (page fault linear address)
    CR3 = 25,   // Control register 3 (page directory base)
    CR4 = 26,   // Control register 4 (extensions)
    CR5 = 27,   // Control register 5 (reserved)
    CR6 = 28,   // Control register 6 (reserved)
    CR7 = 29,   // Control register 7 (reserved)
    CR8 = 30,   // Control register 8 (task priority)

    // Debug Registers (64-bit)
    DR0 = 31,   // Debug register 0 (breakpoint)
    DR1 = 32,   // Debug register 1 (breakpoint)
    DR2 = 33,   // Debug register 2 (breakpoint)
    DR3 = 34,   // Debug register 3 (breakpoint)
    DR4 = 35,   // Debug register 4 (reserved)
    DR5 = 36,   // Debug register 5 (reserved)
    DR6 = 37,   // Debug register 6 (debug status)
    DR7 = 38,   // Debug register 7 (debug control)
    DR8 = 39,   // Debug register 8 (extended)
    DR9 = 40,   // Debug register 9 (extended)
    DR10 = 41,  // Debug register 10 (extended)
    DR11 = 42,  // Debug register 11 (extended)
    DR12 = 43,  // Debug register 12 (extended)
    DR13 = 44,  // Debug register 13 (extended)
    DR14 = 45,  // Debug register 14 (extended)
    DR15 = 46,  // Debug register 15 (extended)

    // Special Purpose Registers
    RIP = 47,   // Instruction pointer (64-bit PC equivalent)
    RFLAGS = 48,// 64-bit flags register
    MSW = 49,   // Machine status word

    // Total register count
    REGISTER_COUNT = 50
};

// Legacy register aliases for backward compatibility
constexpr Register R0 = Register::RAX;
constexpr Register R1 = Register::RCX;
constexpr Register R2 = Register::RDX;
constexpr Register R3 = Register::RBX;
constexpr Register R4 = Register::RSP;
constexpr Register R5 = Register::RBP;
constexpr Register R6 = Register::RSI;
constexpr Register R7 = Register::RDI;

// Constants
constexpr size_t TOTAL_REGISTERS = static_cast<size_t>(Register::REGISTER_COUNT);
constexpr size_t GENERAL_PURPOSE_COUNT = 16;
constexpr size_t SEGMENT_REGISTER_COUNT = 6;
constexpr size_t CONTROL_REGISTER_COUNT = 9;
constexpr size_t DEBUG_REGISTER_COUNT = 16;

/**
 * Register name mapping for assembly language support
 */
class RegisterNames {
public:
    static std::string get_name(Register reg);
    static Register from_name(const std::string& name);
    static bool is_general_purpose(Register reg);
    static bool is_segment(Register reg);
    static bool is_control(Register reg);
    static bool is_debug(Register reg);
    static bool is_special(Register reg);
};

/**
 * Register groups for efficient access patterns
 */
struct RegisterInfo {
    Register reg;
    std::string name;
    std::string description;
    bool is_64bit;
    bool is_system;  // Requires privileged access
};

// Predefined register information
extern const RegisterInfo REGISTER_INFO[];

} // namespace VirtComp_Registers
