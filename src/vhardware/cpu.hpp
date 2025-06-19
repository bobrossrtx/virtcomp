#pragma once
#include <vector>
#include <cstdint>
#include <string>

#include "device_manager.hpp"

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

    HALT = 0xFF         // Halt execution
};

class CPU {
public:
    CPU();
    ~CPU();

    void reset();
    void execute(const std::vector<uint8_t>& program);
    void run(const std::vector<uint8_t>& program); // resets and runs whole program
    bool step(const std::vector<uint8_t>& program); // executes one instruction, returns false if halted or error
    void print_state(const std::string& info) const;
    void print_registers() const;
    void print_memory(std::size_t start = 0, std::size_t end = 0x20) const; // Print first 32 bytes by default

    // Add these getters for testing
    const std::vector<uint32_t>& get_registers() const { return registers; }
    std::vector<uint32_t>& get_registers() { return registers; } // Non-const version for opcodes
    std::vector<uint8_t>& get_memory() { return memory; }
    uint32_t get_flags() const { return flags; }
    void set_flags(uint32_t value) { flags = value; }
    uint32_t get_pc() const { return pc; }
    uint32_t get_sp() const { return sp; }
    uint32_t get_fp() const { return fp; }
    int get_arg_offset() const { return arg_offset; }
    void set_arg_offset(int value) { arg_offset = value; }

    void set_pc(uint32_t value) { pc = value; }
    void set_sp(uint32_t value) { sp = value; }
    void set_fp(uint32_t value) { fp = value; }

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
    std::vector<uint32_t> registers;
    std::vector<uint8_t> memory;
    uint32_t pc; // Program Counter
    uint32_t sp; // Stack Pointer
    uint32_t fp; // Frame Pointer
    uint32_t flags; // Status Flags
    int arg_offset; // Offset for arguments    // Add these:
    mutable uint32_t last_accessed_addr = static_cast<uint32_t>(-1);
    uint32_t last_modified_addr = static_cast<uint32_t>(-1);
    
    uint8_t readPort(uint8_t port);
    void writePort(uint8_t port, uint8_t value);
    std::string readPortString(uint8_t port, uint8_t maxLength = 255);
    void writePortString(uint8_t port, const std::string& str);
};