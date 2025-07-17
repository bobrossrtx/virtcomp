# CPU Core System

**File:** `src/vhardware/cpu.hpp` and `src/vhardware/cpu.cpp`  
**Purpose:** Virtual CPU implementation with full instruction set architecture

## Overview

The CPU system provides a complete virtual processor with support for both 32-bit and 64-bit operations, comprehensive register architecture, and advanced debugging capabilities.

## Architecture

### CPU Modes

```cpp
enum class CPUMode : uint8_t {
    MODE_32BIT = 0,     // Legacy 32-bit compatibility mode
    MODE_64BIT = 1      // Extended 64-bit operations mode
};
```

### Register System

- **Standard Registers:** R0-R7 (32-bit general purpose)
- **Extended Registers:** R8-R49 (additional computational registers)
- **Special Registers:**
  - Stack Pointer (SP)
  - Program Counter (PC)
  - Flags Register (FLAGS)

### Flags Architecture

```cpp
struct CPUFlags {
    bool zero = false;        // Z flag - result was zero
    bool carry = false;       // C flag - arithmetic carry occurred
    bool overflow = false;    // O flag - arithmetic overflow
    bool sign = false;        // S flag - result was negative
    bool parity = false;      // P flag - result has even parity
    bool auxiliary = false;   // A flag - auxiliary carry for BCD
    bool interrupt = false;   // I flag - interrupt enable
    bool direction = false;   // D flag - string operation direction
};
```

## Instruction Set

### Arithmetic Operations
- `ADD`, `SUB`, `MUL`, `DIV` - Basic arithmetic
- `INC`, `DEC` - Increment/decrement operations
- `ADD64`, `SUB64`, `MUL64`, `DIV64` - 64-bit arithmetic

### Logic Operations
- `AND`, `OR`, `XOR`, `NOT` - Bitwise logic
- `SHL`, `SHR` - Bit shifting operations

### Memory Operations
- `LOAD_IMM` - Load immediate values
- `MOV` - Register-to-register transfers
- `LOAD`, `STORE` - Memory access operations
- `LEA` - Load effective address

### Control Flow
- `JMP` - Unconditional jump
- `JZ`, `JNZ` - Zero flag conditional jumps
- `JG`, `JL`, `JGE`, `JLE` - Comparison-based jumps
- `JS`, `JNS` - Sign flag jumps
- `JC`, `JNC` - Carry flag jumps
- `JO`, `JNO` - Overflow flag jumps

### Stack Operations
- `PUSH`, `POP` - Stack manipulation
- `PUSHF`, `POPF` - Flags register stack operations

### I/O Operations
- `IN`, `OUT` - Device communication
- Supports multiple device types via device manager

### System Operations
- `CMP` - Compare operations (sets flags)
- `NOP` - No operation
- `HALT` - Terminate execution
- `SWAP` - Register exchange operations

## Key Classes

### CPU Class

```cpp
class CPU {
public:
    CPU();
    ~CPU();
    
    // Core execution
    void run(const std::vector<uint8_t>& program);
    void step();
    bool is_running() const;
    void halt();
    
    // Register access
    uint32_t get_register(uint8_t reg) const;
    void set_register(uint8_t reg, uint32_t value);
    
    // Memory management
    uint8_t read_memory(uint32_t address) const;
    void write_memory(uint32_t address, uint8_t value);
    
    // Debugging support
    std::string get_state_string() const;
    void dump_registers() const;
    void dump_memory(uint32_t start, uint32_t length) const;
    
    // Device integration
    void attach_device_manager(DeviceManager* dm);
    
private:
    std::unique_ptr<Register[]> registers;
    std::vector<uint8_t> memory;
    CPUFlags flags;
    uint32_t pc;           // Program counter
    uint32_t sp;           // Stack pointer
    bool running;
    DeviceManager* device_manager;
};
```

## Memory Architecture

- **Total Memory:** 1MB (0x00000 - 0xFFFFF)
- **Stack:** Grows downward from high memory
- **Program:** Loaded starting at address 0x00000
- **Device I/O:** Memory-mapped device access

## Instruction Encoding

### Standard Format
```
[OPCODE][REG1][REG2][IMMEDIATE/ADDRESS]
```

### Size Variants
- **1-byte immediate:** `LOAD_IMM R0, 42`
- **4-byte address:** `JMP 0x1000`
- **Register-only:** `ADD R0, R1`

## Error Handling

### Exception Types
- **Invalid Opcode:** Unknown instruction encountered
- **Register Out of Bounds:** Invalid register access
- **Memory Violation:** Address beyond memory limits
- **Division by Zero:** Arithmetic exceptions
- **Stack Overflow/Underflow:** Stack boundary violations

### Recovery Mechanisms
- Graceful halt on critical errors
- Detailed error logging with context
- State preservation for debugging

## Device Integration

The CPU integrates with the device manager to provide:

- **Console I/O:** Device ID 1 for text output
- **Future Expansion:** Graphics, storage, network devices
- **Memory-Mapped I/O:** Unified address space

## Extended Features

### 64-bit Mode Support
- Extended arithmetic operations
- Larger immediate values
- Enhanced addressing modes

### Debug Integration
- Real-time state monitoring
- Breakpoint support (via GUI)
- Instruction tracing

### Performance Features
- Optimized instruction decode
- Efficient memory access patterns
- Minimal overhead execution

## Usage Examples

### Basic Execution
```cpp
CPU cpu;
DeviceManager devices;
cpu.attach_device_manager(&devices);

std::vector<uint8_t> program = {
    0x01, 0x00, 42,     // LOAD_IMM R0, 42
    0x02, 0x00, 0x01,   // ADD R0, R1
    0xFF                // HALT
};

cpu.run(program);
```

### State Inspection
```cpp
// Check register values
uint32_t value = cpu.get_register(0);

// Examine memory
uint8_t byte = cpu.read_memory(0x100);

// Debug output
std::cout << cpu.get_state_string() << std::endl;
```

The CPU system forms the computational heart of VirtComp, providing a full-featured virtual processor suitable for educational use, embedded simulation, and assembly language learning.
