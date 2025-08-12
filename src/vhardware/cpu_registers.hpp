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

    // SIMD Registers (128-bit XMM registers - stored as 2x64-bit)
    XMM0 = 50,  XMM0_HIGH = 51,   // XMM0 (128-bit SIMD register)
    XMM1 = 52,  XMM1_HIGH = 53,   // XMM1 (128-bit SIMD register)
    XMM2 = 54,  XMM2_HIGH = 55,   // XMM2 (128-bit SIMD register)
    XMM3 = 56,  XMM3_HIGH = 57,   // XMM3 (128-bit SIMD register)
    XMM4 = 58,  XMM4_HIGH = 59,   // XMM4 (128-bit SIMD register)
    XMM5 = 60,  XMM5_HIGH = 61,   // XMM5 (128-bit SIMD register)
    XMM6 = 62,  XMM6_HIGH = 63,   // XMM6 (128-bit SIMD register)
    XMM7 = 64,  XMM7_HIGH = 65,   // XMM7 (128-bit SIMD register)
    XMM8 = 66,  XMM8_HIGH = 67,   // XMM8 (128-bit SIMD register)
    XMM9 = 68,  XMM9_HIGH = 69,   // XMM9 (128-bit SIMD register)
    XMM10 = 70, XMM10_HIGH = 71,  // XMM10 (128-bit SIMD register)
    XMM11 = 72, XMM11_HIGH = 73,  // XMM11 (128-bit SIMD register)
    XMM12 = 74, XMM12_HIGH = 75,  // XMM12 (128-bit SIMD register)
    XMM13 = 76, XMM13_HIGH = 77,  // XMM13 (128-bit SIMD register)
    XMM14 = 78, XMM14_HIGH = 79,  // XMM14 (128-bit SIMD register)
    XMM15 = 80, XMM15_HIGH = 81,  // XMM15 (128-bit SIMD register)

    // FPU Stack Registers (80-bit floating point - stored as 2x64-bit with metadata)
    ST0 = 82,   ST0_META = 83,    // FPU Stack register 0
    ST1 = 84,   ST1_META = 85,    // FPU Stack register 1
    ST2 = 86,   ST2_META = 87,    // FPU Stack register 2
    ST3 = 88,   ST3_META = 89,    // FPU Stack register 3
    ST4 = 90,   ST4_META = 91,    // FPU Stack register 4
    ST5 = 92,   ST5_META = 93,    // FPU Stack register 5
    ST6 = 94,   ST6_META = 95,    // FPU Stack register 6
    ST7 = 96,   ST7_META = 97,    // FPU Stack register 7

    // MMX Registers (64-bit multimedia - aliased to FPU registers)
    MM0 = 82,   // Alias to ST0 (MMX register 0)
    MM1 = 84,   // Alias to ST1 (MMX register 1)
    MM2 = 86,   // Alias to ST2 (MMX register 2)
    MM3 = 88,   // Alias to ST3 (MMX register 3)
    MM4 = 90,   // Alias to ST4 (MMX register 4)
    MM5 = 92,   // Alias to ST5 (MMX register 5)
    MM6 = 94,   // Alias to ST6 (MMX register 6)
    MM7 = 96,   // Alias to ST7 (MMX register 7)

    // SIMD Control and Status Registers
    MXCSR = 98,     // SIMD Control and Status Register
    FPU_CONTROL = 99, // FPU Control Word
    FPU_STATUS = 100, // FPU Status Word
    FPU_TAG = 101,    // FPU Tag Word

    // Extended SIMD Registers (AVX support - 256-bit YMM registers)
    // YMM registers use XMM as lower 128 bits + additional 128 bits
    YMM0_HIGH2 = 102,  YMM0_HIGH3 = 103,   // YMM0 upper 128 bits
    YMM1_HIGH2 = 104,  YMM1_HIGH3 = 105,   // YMM1 upper 128 bits
    YMM2_HIGH2 = 106,  YMM2_HIGH3 = 107,   // YMM2 upper 128 bits
    YMM3_HIGH2 = 108,  YMM3_HIGH3 = 109,   // YMM3 upper 128 bits
    YMM4_HIGH2 = 110,  YMM4_HIGH3 = 111,   // YMM4 upper 128 bits
    YMM5_HIGH2 = 112,  YMM5_HIGH3 = 113,   // YMM5 upper 128 bits
    YMM6_HIGH2 = 114,  YMM6_HIGH3 = 115,   // YMM6 upper 128 bits
    YMM7_HIGH2 = 116,  YMM7_HIGH3 = 117,   // YMM7 upper 128 bits
    YMM8_HIGH2 = 118,  YMM8_HIGH3 = 119,   // YMM8 upper 128 bits
    YMM9_HIGH2 = 120,  YMM9_HIGH3 = 121,   // YMM9 upper 128 bits
    YMM10_HIGH2 = 122, YMM10_HIGH3 = 123,  // YMM10 upper 128 bits
    YMM11_HIGH2 = 124, YMM11_HIGH3 = 125,  // YMM11 upper 128 bits
    YMM12_HIGH2 = 126, YMM12_HIGH3 = 127,  // YMM12 upper 128 bits
    YMM13_HIGH2 = 128, YMM13_HIGH3 = 129,  // YMM13 upper 128 bits
    YMM14_HIGH2 = 130, YMM14_HIGH3 = 131,  // YMM14 upper 128 bits
    YMM15_HIGH2 = 132, YMM15_HIGH3 = 133,  // YMM15 upper 128 bits

    // Total register count (expanded from 50 to 134)
    REGISTER_COUNT = 134
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
constexpr size_t SIMD_REGISTER_COUNT = 16;      // XMM0-XMM15
constexpr size_t FPU_REGISTER_COUNT = 8;       // ST0-ST7
constexpr size_t MMX_REGISTER_COUNT = 8;       // MM0-MM7
constexpr size_t AVX_REGISTER_COUNT = 16;      // YMM0-YMM15

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
    static bool is_simd(Register reg);         // XMM registers
    static bool is_fpu(Register reg);          // ST registers
    static bool is_mmx(Register reg);          // MM registers
    static bool is_avx(Register reg);          // YMM registers (AVX)
    static bool is_simd_control(Register reg); // MXCSR, FPU control registers
};

/**
 * Register groups for efficient access patterns
 */
struct RegisterInfo {
    Register reg;
    std::string name;
    std::string description;
    bool is_64bit;
    bool is_128bit;      // For SIMD registers
    bool is_256bit;      // For AVX registers
    bool is_80bit;       // For FPU registers
    bool is_system;      // Requires privileged access
    bool is_vector;      // SIMD/vector register
    bool is_floating;    // Floating-point register
};

// Register type enumeration for classification
enum class RegisterType {
    GENERAL_PURPOSE,
    SEGMENT,
    CONTROL,
    DEBUG,
    SPECIAL,
    SIMD_XMM,
    FPU_ST,
    MMX,
    AVX_YMM,
    SIMD_CONTROL
};

// Predefined register information
extern const RegisterInfo REGISTER_INFO[];

} // namespace VirtComp_Registers
