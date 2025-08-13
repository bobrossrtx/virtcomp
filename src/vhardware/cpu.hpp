#pragma once
#include <vector>
#include <cstdint>
#include <string>

#include "../config.hpp"
#include "../debug/logger.hpp"
#include "cpu_registers.hpp"  // New extended register architecture
#include "device_manager.hpp"

using Logging::Logger;
using VirtComp_Registers::Register;
using VirtComp_Registers::RegisterNames;
using VirtComp_Registers::TOTAL_REGISTERS;

// CPU Operation Modes
enum class CPUMode : uint8_t {
    MODE_32BIT = 0,     // 32-bit mode (legacy compatibility)
    MODE_64BIT = 1      // 64-bit mode (extended operations)
};

enum class Opcode : uint8_t {
    NOP = 0x00,         // No operation
    LOAD_IMM = 0x01,    // Load immediate value into reg
    ADD = 0x02,         // Add reg1, reg2
    SUB = 0x03,         // Subtract reg1, reg2
    MOV = 0x04,         // Move reg1, reg2 (reg1 = reg2)
    JMP = 0x05,         // Jump to address
    LOAD = 0x06,        // Load value from memory to reg
    STORE = 0x07,       // Store value from reg to memory

    PUSH = 0x08,        // Push reg onto stack
    POP = 0x09,         // Pop value from stack to reg
    CMP = 0x0A,         // Compare reg1, reg2

    JZ  = 0x0B,         // Jump if zero flag set
    JNZ = 0x0C,         // Jump if zero flag not set
    JS  = 0x0D,         // Jump if sign flag set
    JNS = 0x0E,         // Jump if sign flag not set
    JC  = 0x0F,         // Jump if carry flag set
    JNC = 0x22,         // Jump if carry flag not set
    JO  = 0x23,         // Jump if overflow flag set
    JNO = 0x24,         // Jump if overflow flag not set
    JG  = 0x25,         // Jump if greater (signed)
    JL  = 0x26,         // Jump if less (signed)
    JGE = 0x27,         // Jump if greater or equal (signed)
    JLE = 0x28,         // Jump if less or equal (signed)

    MUL = 0x10,         // Multiply reg1, reg2
    DIV = 0x11,         // Divide reg1, reg2
    INC = 0x12,         // Increment reg
    DEC = 0x13,         // Decrement reg
    AND = 0x14,         // Bitwise AND reg1, reg2
    OR  = 0x15,         // Bitwise OR reg1, reg2
    XOR = 0x16,         // Bitwise XOR reg1, reg2
    NOT = 0x17,         // Bitwise NOT reg
    SHL = 0x18,         // Shift Left reg, imm
    SHR = 0x19,         // Shift Right reg, imm
    CALL = 0x1A,        // Call subroutine
    RET  = 0x1B,        // Return from subroutine
    PUSH_ARG = 0x1C,    // Push argument onto stack
    POP_ARG  = 0x1D,    // Pop argument from stack
    PUSH_FLAG = 0x1E,   // Push flags onto stack
    POP_FLAG  = 0x1F,   // Pop flags from stack

    LEA = 0x20,         // Load Effective Address - load address into register
    SWAP = 0x21,        // Swap - swap values between register and memory

    IN = 0x30,          // Input from port/device to register
    OUT = 0x31,         // Output from register to port/device
    INB = 0x32,         // Input byte from port/device to register
    OUTB = 0x33,        // Output byte from register to port/device
    INW = 0x34,         // Input word from port/device to register
    OUTW = 0x35,        // Output word from register to port/device
    INL = 0x36,         // Input long from port/device to register
    OUTL = 0x37,        // Output long from register to port/device
    INSTR = 0x38,       // Input instruction from port/device to register
    OUTSTR = 0x39,      // Output string from register to port/device

    DB = 0x40,          // Define byte

    // Extended 64-bit Register Operations (0x50-0x6F range)
    ADD64 = 0x50,       // 64-bit Add reg1, reg2
    SUB64 = 0x51,       // 64-bit Subtract reg1, reg2
    MOV64 = 0x52,       // 64-bit Move reg1, reg2 (reg1 = reg2)
    LOAD_IMM64 = 0x53,  // Load 64-bit immediate value into reg
    MUL64 = 0x54,       // 64-bit Multiply reg1, reg2
    DIV64 = 0x55,       // 64-bit Divide reg1, reg2
    AND64 = 0x56,       // 64-bit Bitwise AND reg1, reg2
    OR64 = 0x57,        // 64-bit Bitwise OR reg1, reg2
    XOR64 = 0x58,       // 64-bit Bitwise XOR reg1, reg2
    NOT64 = 0x59,       // 64-bit Bitwise NOT reg
    SHL64 = 0x5A,       // 64-bit Shift Left reg, imm
    SHR64 = 0x5B,       // 64-bit Shift Right reg, imm
    CMP64 = 0x5C,       // 64-bit Compare reg1, reg2
    INC64 = 0x5D,       // 64-bit Increment reg
    DEC64 = 0x5E,       // 64-bit Decrement reg

    // Extended Register Set Operations (0x60-0x6F range)
    MOVEX = 0x60,       // Move between extended registers (R8-R15)
    ADDEX = 0x61,       // Add with extended registers
    SUBEX = 0x62,       // Subtract with extended registers
    MULEX = 0x63,       // Multiply with extended registers
    DIVEX = 0x64,       // Divide with extended registers
    CMPEX = 0x65,       // Compare with extended registers
    LOADEX = 0x66,      // Load from memory to extended register
    STOREX = 0x67,      // Store from extended register to memory
    PUSHEX = 0x68,      // Push extended register onto stack
    POPEX = 0x69,       // Pop from stack to extended register

    // CPU Mode Control Operations (0x70-0x7F range)
    MODE32 = 0x70,      // Switch to 32-bit mode
    MODE64 = 0x71,      // Switch to 64-bit mode
    MODECMP = 0x72,     // Compare current mode with operand
    MODEFLAG = 0x73,    // Set mode flag in RFLAGS register

    // SIMD Operations (0x80-0x9F range)
    MOVAPS = 0x80,      // Move Aligned Packed Single
    MOVUPS = 0x81,      // Move Unaligned Packed Single
    ADDPS = 0x82,       // Add Packed Single
    SUBPS = 0x83,       // Subtract Packed Single
    MULPS = 0x84,       // Multiply Packed Single
    DIVPS = 0x85,       // Divide Packed Single
    SQRTPS = 0x86,      // Square Root Packed Single
    MAXPS = 0x87,       // Maximum Packed Single
    MINPS = 0x88,       // Minimum Packed Single
    ANDPS = 0x89,       // Bitwise AND Packed Single
    ORPS = 0x8A,        // Bitwise OR Packed Single
    XORPS = 0x8B,       // Bitwise XOR Packed Single
    CMPPS = 0x8C,       // Compare Packed Single

    // Packed Double Operations
    MOVAPD = 0x8D,      // Move Aligned Packed Double
    MOVUPD = 0x8E,      // Move Unaligned Packed Double
    ADDPD = 0x8F,       // Add Packed Double
    SUBPD = 0x90,       // Subtract Packed Double
    MULPD = 0x91,       // Multiply Packed Double
    DIVPD = 0x92,       // Divide Packed Double
    SQRTPD = 0x93,      // Square Root Packed Double
    MAXPD = 0x94,       // Maximum Packed Double
    MINPD = 0x95,       // Minimum Packed Double
    ANDPD = 0x96,       // Bitwise AND Packed Double
    ORPD = 0x97,        // Bitwise OR Packed Double
    XORPD = 0x98,       // Bitwise XOR Packed Double
    CMPPD = 0x99,       // Compare Packed Double

    // FPU Operations (0xA0-0xBF range)
    FLD = 0xA0,         // Load floating point value
    FST = 0xA1,         // Store floating point value
    FSTP = 0xA2,        // Store floating point value and pop
    FILD = 0xA3,        // Load integer as floating point
    FIST = 0xA4,        // Store floating point as integer
    FISTP = 0xA5,       // Store floating point as integer and pop
    FADD = 0xA6,        // Floating point add
    FSUB = 0xA7,        // Floating point subtract
    FMUL = 0xA8,        // Floating point multiply
    FDIV = 0xA9,        // Floating point divide
    FSIN = 0xAA,        // Floating point sine
    FCOS = 0xAB,        // Floating point cosine
    FTAN = 0xAC,        // Floating point tangent
    FSQRT = 0xAD,       // Floating point square root
    FABS = 0xAE,        // Floating point absolute value
    FCHS = 0xAF,        // Floating point change sign

    // FPU Control Operations
    FINIT = 0xB0,       // Initialize FPU
    FCLEX = 0xB1,       // Clear exceptions
    FSTCW = 0xB2,       // Store control word
    FLDCW = 0xB3,       // Load control word
    FSTSW = 0xB4,       // Store status word
    FCOMPP = 0xB5,      // Compare and pop twice
    FUCOMPP = 0xB6,     // Unordered compare and pop twice

    // AVX Operations (0xC0-0xDF range)
    VADDPS = 0xC0,      // AVX Add Packed Single
    VSUBPS = 0xC1,      // AVX Subtract Packed Single
    VMULPS = 0xC2,      // AVX Multiply Packed Single
    VDIVPS = 0xC3,      // AVX Divide Packed Single
    VSQRTPS = 0xC4,     // AVX Square Root Packed Single
    VMAXPS = 0xC5,      // AVX Maximum Packed Single
    VMINPS = 0xC6,      // AVX Minimum Packed Single
    VANDPS = 0xC7,      // AVX Bitwise AND Packed Single
    VORPS = 0xC8,       // AVX Bitwise OR Packed Single
    VXORPS = 0xC9,      // AVX Bitwise XOR Packed Single

    // AVX Packed Double Operations
    VADDPD = 0xCA,      // AVX Add Packed Double
    VSUBPD = 0xCB,      // AVX Subtract Packed Double
    VMULPD = 0xCC,      // AVX Multiply Packed Double
    VDIVPD = 0xCD,      // AVX Divide Packed Double
    VSQRTPD = 0xCE,     // AVX Square Root Packed Double
    VMAXPD = 0xCF,      // AVX Maximum Packed Double
    VMINPD = 0xD0,      // AVX Minimum Packed Double
    VANDPD = 0xD1,      // AVX Bitwise AND Packed Double
    VORPD = 0xD2,       // AVX Bitwise OR Packed Double
    VXORPD = 0xD3,      // AVX Bitwise XOR Packed Double

    // MMX Operations (0xE0-0xEF range)
    MOVQ = 0xE0,        // Move Quadword
    PADDB = 0xE1,       // Add Packed Bytes
    PADDW = 0xE2,       // Add Packed Words
    PADDD = 0xE3,       // Add Packed Doublewords
    PSUBB = 0xE4,       // Subtract Packed Bytes
    PSUBW = 0xE5,       // Subtract Packed Words
    PSUBD = 0xE6,       // Subtract Packed Doublewords
    PCMPEQB = 0xE7,     // Compare Packed Bytes for Equality
    PCMPEQW = 0xE8,     // Compare Packed Words for Equality
    PCMPEQD = 0xE9,     // Compare Packed Doublewords for Equality
    EMMS = 0xEA,        // Empty MMX State

    HALT = 0xFF         // Halt execution
};

class CPU {
public:
    CPU(size_t memory_size = 0); // 0 means use default size
    static CPU create_test_cpu(); // Factory method for test compatibility
    ~CPU();

    void reset();
    void execute(const std::vector<uint8_t>& program);
    void run(const std::vector<uint8_t>& program); // resets and runs whole program
    bool step(const std::vector<uint8_t>& program); // executes one instruction, returns false if halted or error
    void print_state(const std::string& info) const;
    void print_registers() const;
    void print_extended_registers() const; // Show all 50 registers
    void print_register_update(Register reg, uint64_t old_value, uint64_t new_value) const; // Print register change message
    void print_memory(std::size_t start = 0, std::size_t end = 0x20) const; // Print first 32 bytes by default

    // Memory management
    size_t get_memory_size() const { return memory.size(); }
    void resize_memory(size_t new_size); // Dynamic memory resizing

    // CPU Mode Management (x32/x64 support)
    CPUMode get_cpu_mode() const { return cpu_mode; }
    void set_cpu_mode(CPUMode mode) {
        cpu_mode = mode;
        Logger::instance().info() << fmt::format(
            "CPU mode switched to {}-bit",
            (mode == CPUMode::MODE_64BIT) ? 64 : 32) << std::endl;
    }
    bool is_64bit_mode() const { return cpu_mode == CPUMode::MODE_64BIT; }
    bool is_32bit_mode() const { return cpu_mode == CPUMode::MODE_32BIT; }

    // Mode-aware register operations
    uint64_t get_register_mode_aware(Register reg) const {
        if (is_32bit_mode()) {
            return get_register_32(reg); // Return only lower 32 bits in 32-bit mode
        }
        return get_register_64(reg); // Return full 64 bits in 64-bit mode
    }

    void set_register_mode_aware(Register reg, uint64_t value) {
        if (is_32bit_mode()) {
            set_register_32(reg, static_cast<uint32_t>(value)); // Only set lower 32 bits
        } else {
            set_register_64(reg, value); // Set full 64 bits
        }
    }

    // Get effective register size based on current mode
    size_t get_register_size() const {
        return is_64bit_mode() ? 8 : 4; // 8 bytes for 64-bit, 4 bytes for 32-bit
    }

    // Extended register access (64-bit registers)
    uint64_t get_register(Register reg) const;
    void set_register(Register reg, uint64_t value);

    // Enhanced register operations with size specification
    uint64_t get_register_64(Register reg) const { return get_register(reg); }
    uint32_t get_register_32(Register reg) const { return static_cast<uint32_t>(get_register(reg)); }
    uint16_t get_register_16(Register reg) const { return static_cast<uint16_t>(get_register(reg)); }
    uint8_t get_register_8(Register reg) const { return static_cast<uint8_t>(get_register(reg)); }

    void set_register_64(Register reg, uint64_t value) { set_register(reg, value); }
    void set_register_32(Register reg, uint32_t value) {
        // Preserve upper 32 bits when setting lower 32 bits
        uint64_t current = get_register(reg);
        set_register(reg, (current & 0xFFFFFFFF00000000ULL) | value);
    }
    void set_register_16(Register reg, uint16_t value) {
        // Preserve upper 48 bits when setting lower 16 bits
        uint64_t current = get_register(reg);
        set_register(reg, (current & 0xFFFFFFFFFFFF0000ULL) | value);
    }
    void set_register_8(Register reg, uint8_t value) {
        // Preserve upper 56 bits when setting lower 8 bits
        uint64_t current = get_register(reg);
        set_register(reg, (current & 0xFFFFFFFFFFFFFF00ULL) | value);
    }

    // Extended register validation
    bool is_valid_register(Register reg) const {
        return static_cast<size_t>(reg) < TOTAL_REGISTERS;
    }
    bool is_extended_register(Register reg) const {
        auto index = static_cast<size_t>(reg);
        return index >= 8 && index < 16; // R8-R15
    }

    // SIMD register access (128-bit XMM registers)
    void get_xmm_register(Register xmm_reg, uint64_t& low, uint64_t& high) const {
        if (RegisterNames::is_simd(xmm_reg)) {
            low = get_register(xmm_reg);
            // Get corresponding high part
            auto high_reg = static_cast<Register>(static_cast<size_t>(xmm_reg) + 1);
            high = get_register(high_reg);
        }
    }

    void set_xmm_register(Register xmm_reg, uint64_t low, uint64_t high) {
        if (RegisterNames::is_simd(xmm_reg)) {
            set_register(xmm_reg, low);
            // Set corresponding high part
            auto high_reg = static_cast<Register>(static_cast<size_t>(xmm_reg) + 1);
            set_register(high_reg, high);
        }
    }

    // FPU register access (80-bit floating point)
    void get_fpu_register(Register st_reg, uint64_t& mantissa, uint64_t& exponent_sign) const {
        if (RegisterNames::is_fpu(st_reg)) {
            mantissa = get_register(st_reg);
            // Get corresponding metadata part
            auto meta_reg = static_cast<Register>(static_cast<size_t>(st_reg) + 1);
            exponent_sign = get_register(meta_reg);
        }
    }

    void set_fpu_register(Register st_reg, uint64_t mantissa, uint64_t exponent_sign) {
        if (RegisterNames::is_fpu(st_reg)) {
            set_register(st_reg, mantissa);
            // Set corresponding metadata part
            auto meta_reg = static_cast<Register>(static_cast<size_t>(st_reg) + 1);
            set_register(meta_reg, exponent_sign);
        }
    }

    // AVX register access (256-bit YMM registers)
    void get_ymm_register(Register ymm_reg, uint64_t parts[4]) const {
        if (RegisterNames::is_simd(ymm_reg)) {
            // Lower 128 bits from XMM
            get_xmm_register(ymm_reg, parts[0], parts[1]);

            // Upper 128 bits from YMM high parts
            auto base_index = static_cast<size_t>(ymm_reg) - static_cast<size_t>(Register::XMM0);
            auto high2_reg = static_cast<Register>(static_cast<size_t>(Register::YMM0_HIGH2) + base_index * 2);
            auto high3_reg = static_cast<Register>(static_cast<size_t>(Register::YMM0_HIGH3) + base_index * 2);
            parts[2] = get_register(high2_reg);
            parts[3] = get_register(high3_reg);
        }
    }

    void set_ymm_register(Register ymm_reg, const uint64_t parts[4]) {
        if (RegisterNames::is_simd(ymm_reg)) {
            // Lower 128 bits to XMM
            set_xmm_register(ymm_reg, parts[0], parts[1]);

            // Upper 128 bits to YMM high parts
            auto base_index = static_cast<size_t>(ymm_reg) - static_cast<size_t>(Register::XMM0);
            auto high2_reg = static_cast<Register>(static_cast<size_t>(Register::YMM0_HIGH2) + base_index * 2);
            auto high3_reg = static_cast<Register>(static_cast<size_t>(Register::YMM0_HIGH3) + base_index * 2);
            set_register(high2_reg, parts[2]);
            set_register(high3_reg, parts[3]);
        }
    }

    // SIMD and FPU control register access
    uint32_t get_mxcsr() const { return static_cast<uint32_t>(get_register(Register::MXCSR)); }
    void set_mxcsr(uint32_t value) { set_register(Register::MXCSR, value); }

    uint16_t get_fpu_control() const { return static_cast<uint16_t>(get_register(Register::FPU_CONTROL)); }
    void set_fpu_control(uint16_t value) { set_register(Register::FPU_CONTROL, value); }

    uint16_t get_fpu_status() const { return static_cast<uint16_t>(get_register(Register::FPU_STATUS)); }
    void set_fpu_status(uint16_t value) { set_register(Register::FPU_STATUS, value); }

    uint16_t get_fpu_tag() const { return static_cast<uint16_t>(get_register(Register::FPU_TAG)); }
    void set_fpu_tag(uint16_t value) { set_register(Register::FPU_TAG, value); }

    // Register name support for debugging
    std::string get_register_name(Register reg) const;

    // Legacy register access (for backward compatibility)
    const std::vector<uint32_t>& get_registers() const { return legacy_registers; }
    std::vector<uint32_t>& get_registers() { return legacy_registers; } // Non-const version for opcodes

    std::vector<uint8_t>& get_memory() { return memory; }
    uint32_t get_flags() const { return static_cast<uint32_t>(registers[static_cast<size_t>(Register::RFLAGS)]); }
    void set_flags(uint32_t value) { registers[static_cast<size_t>(Register::RFLAGS)] = value; }
    uint32_t get_pc() const { return static_cast<uint32_t>(registers[static_cast<size_t>(Register::RIP)]); }
    uint32_t get_sp() const { return static_cast<uint32_t>(registers[static_cast<size_t>(Register::RSP)]); }
    uint32_t get_fp() const { return static_cast<uint32_t>(registers[static_cast<size_t>(Register::RBP)]); }
    int get_arg_offset() const { return arg_offset; }
    void set_arg_offset(int value) { arg_offset = value; }

    void set_pc(uint32_t value) { registers[static_cast<size_t>(Register::RIP)] = value; }
    void set_sp(uint32_t value) { registers[static_cast<size_t>(Register::RSP)] = value; }
    void set_fp(uint32_t value) { registers[static_cast<size_t>(Register::RBP)] = value; }

    uint8_t fetch_operand();
    void write_mem32(uint32_t addr, uint32_t value);
    uint32_t read_mem32(uint32_t addr) const;

    void print_stack_frame(const std::string& label) const;

    uint32_t get_last_accessed_addr() const { return last_accessed_addr; }
    uint32_t get_last_modified_addr() const { return last_modified_addr; }

    // I/O operations for opcode handlers
    uint8_t read_port(uint8_t port) { return readPort(port); }
    void write_port(uint8_t port, uint8_t value) { writePort(port, value); }
    std::string read_port_string(uint8_t port, uint8_t maxLength = 255) { return readPortString(port, maxLength); }
    void write_port_string(uint8_t port, const std::string& str) { writePortString(port, str); }

    // Additional I/O methods for word and dword operations
    uint16_t read_port_word(uint8_t port) { return vhw::DeviceManager::instance().readPortWord(port); }
    void write_port_word(uint8_t port, uint16_t value) { vhw::DeviceManager::instance().writePortWord(port, value); }
    uint32_t read_port_dword(uint8_t port) { return vhw::DeviceManager::instance().readPortDWord(port); }
    void write_port_dword(uint8_t port, uint32_t value) { vhw::DeviceManager::instance().writePortDWord(port, value); }

private:
    // CPU operation mode (32-bit or 64-bit)
    CPUMode cpu_mode;

    // Extended 64-bit register array (50 registers total)
    std::vector<uint64_t> registers;

    // Legacy 32-bit register compatibility layer
    std::vector<uint32_t> legacy_registers;

    std::vector<uint8_t> memory;
    int arg_offset; // Offset for arguments
    mutable uint32_t last_accessed_addr = static_cast<uint32_t>(-1);
    uint32_t last_modified_addr = static_cast<uint32_t>(-1);

    // Internal register synchronization
    void sync_legacy_registers();
    void sync_from_legacy_registers();

    uint8_t readPort(uint8_t port);
    void writePort(uint8_t port, uint8_t value);
    std::string readPortString(uint8_t port, uint8_t maxLength = 255);
    void writePortString(uint8_t port, const std::string& str);
};