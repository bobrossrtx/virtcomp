#pragma once
#include <cstdint>

namespace Assembler {

// VirtComp instruction opcodes (standalone version for assembler)
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
    LOAD_IMM64 = 0x53,  // Load 64-bit immediate
    MUL64 = 0x54,       // 64-bit Multiply reg1, reg2
    DIV64 = 0x55,       // 64-bit Divide reg1, reg2
    AND64 = 0x56,       // 64-bit Bitwise AND reg1, reg2
    OR64 = 0x57,        // 64-bit Bitwise OR reg1, reg2
    XOR64 = 0x58,       // 64-bit Bitwise XOR reg1, reg2
    SHL64 = 0x59,       // 64-bit Shift Left reg, imm
    SHR64 = 0x5A,       // 64-bit Shift Right reg, imm
    CMP64 = 0x5B,       // 64-bit Compare reg1, reg2
    NOT64 = 0x5C,       // 64-bit Bitwise NOT reg
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

    // Mode Control Operations (0x70-0x7F range)
    MODE32 = 0x70,      // Switch to 32-bit mode
    MODE64 = 0x71,      // Switch to 64-bit mode
    MODECMP = 0x72,     // Compare current CPU mode

    HALT = 0xFF         // Halt execution
};

} // namespace Assembler
