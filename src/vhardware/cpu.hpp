#pragma once
#include <vector>
#include <cstdint>
#include <string>

enum class Opcode : uint8_t {
    NOP = 0x00,
    LOAD_IMM = 0x01,
    ADD = 0x02,
    SUB = 0x03,
    MUL = 0x10,     // Multiply reg1, reg2
    DIV = 0x11,     // Divide reg1, reg2
    INC = 0x12,     // Increment reg
    DEC = 0x13,     // Decrement reg
    AND = 0x14,     // Bitwise AND reg1, reg2
    OR  = 0x15,     // Bitwise OR reg1, reg2
    XOR = 0x16,     // Bitwise XOR reg1, reg2
    NOT = 0x17,     // Bitwise NOT reg
    SHL = 0x18,     // Shift Left reg, imm
    SHR = 0x19,     // Shift Right reg, imm
    MOV = 0x04,
    JMP = 0x05,
    LOAD = 0x06,
    STORE = 0x07,
    PUSH = 0x08,
    POP = 0x09,
    CMP = 0x0A,     // Compare reg1, reg2
    JZ  = 0x0B,     // Jump if zero flag set
    JNZ = 0x0C,     // Jump if zero flag not set
    JS  = 0x0D,     // Jump if sign flag set
    JNS = 0x0E,     // Jump if sign flag not set
    HALT = 0xFF
};

class CPU {
public:
    CPU();
    ~CPU();

    void reset();
    void execute(const std::vector<uint8_t>& program);
    void print_state(const std::string& info) const;
    void print_registers() const;
    void print_memory(std::size_t start = 0, std::size_t end = 0x20) const; // Print first 32 bytes by default

    // Add these getters for testing
    const std::vector<uint32_t>& get_registers() const { return registers; }
    const std::vector<uint8_t>& get_memory() const { return memory; }
    uint32_t get_flags() const { return flags; }
    uint32_t get_pc() const { return pc; }
    uint32_t get_sp() const { return sp; }

private:
    std::vector<uint32_t> registers;
    std::vector<uint8_t> memory;
    uint32_t pc; // Program Counter
    uint32_t sp; // Stack Pointer
    uint32_t flags; // Status Flags
};