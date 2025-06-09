# VirtComp Codebase Documentation

This section provides technical documentation for developers working on VirtComp itself. The documentation mirrors and expands upon the `/** documentation comments **/` found in the source code.

## Table of Contents

- [Architecture Overview](#architecture-overview)
- [Core Components](#core-components)
  - [CPU Implementation](#cpu-implementation)
  - [Device System](#device-system)
  - [Debug Interface](#debug-interface)
  - [Main Application](#main-application)
- [API Reference](API_REFERENCE.md)
- [Build System](#build-system)
- [Development Guidelines](#development-guidelines)

## Architecture Overview

VirtComp follows a modular architecture with clear separation of concerns:

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Main App      │───▶│      CPU        │───▶│     Memory      │
│   (main.cpp)    │    │   (cpu.cpp)     │    │                 │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │
         ▼                       ▼
┌─────────────────┐    ┌─────────────────┐
│   Debug GUI     │    │ Device Manager  │
│   (gui.cpp)     │    │ (device_*.cpp)  │
└─────────────────┘    └─────────────────┘
                               │
                               ▼
                    ┌─────────────────┐
                    │    Devices      │
                    │ (Console, File, │
                    │  Serial, etc.)  │
                    └─────────────────┘
```

### Key Design Principles

1. **Modularity**: Each component has a clear, well-defined interface
2. **Extensibility**: New devices can be added without modifying core CPU code
3. **Portability**: Platform-specific code is isolated and abstracted
4. **Debuggability**: Comprehensive debugging support through GUI and logging

## Core Components

### CPU Implementation

**File**: `src/vhardware/cpu.hpp`, `src/vhardware/cpu.cpp`

The CPU is the heart of the virtual computer, implementing a custom 32-bit architecture.

#### Class: `CPU`

**Purpose**: Emulates a complete CPU with registers, memory, and instruction execution.

```cpp
/**
 * Virtual CPU implementation with 32-bit architecture
 * Supports a comprehensive instruction set and device I/O
 */
class CPU {
public:
    CPU(size_t memory_size = 64 * 1024);
    ~CPU();
    
    // Core execution
    void reset();
    void step();
    void run();
    
    // State access
    uint32_t get_register(Register reg) const;
    void set_register(Register reg, uint32_t value);
    uint32_t get_pc() const { return pc; }
    void set_pc(uint32_t address) { pc = address; }
    
    // Memory operations
    uint8_t read_memory(uint32_t address) const;
    void write_memory(uint32_t address, uint8_t value);
    uint32_t read_memory_word(uint32_t address) const;
    void write_memory_word(uint32_t address, uint32_t value);
    
    // Device management
    void set_device_manager(DeviceManager* dm) { device_manager = dm; }
    
    // Program loading
    bool load_hex_program(const std::string& filename);
    
    // Debug support
    bool is_halted() const { return halted; }
    const std::string& get_last_error() const { return last_error; }
};
```

#### Register Architecture

The CPU provides 8 general-purpose registers plus special-purpose registers:

- **R0-R7**: General-purpose 32-bit registers
- **PC**: Program Counter (32-bit)
- **SP**: Stack Pointer (32-bit)
- **FP**: Frame Pointer (32-bit)
- **FLAGS**: Status flags (32-bit)

#### Memory Model

- **Address Space**: 32-bit addressing (4GB theoretical, configurable actual size)
- **Endianness**: Little-endian
- **Word Size**: 32-bit (4 bytes)
- **Stack**: Grows downward from high memory

#### Instruction Format

Instructions use variable-length encoding:

1. **Single Byte**: Opcode only (e.g., `NOP`, `HALT`)
2. **Two Bytes**: Opcode + register/immediate (e.g., `PUSH R0`)
3. **Multiple Bytes**: Opcode + register + 32-bit immediate (e.g., `LOAD_IMM R0, 0x1234`)

### Device System

**Files**: `src/vhardware/device*.hpp`, `src/vhardware/device*.cpp`

The device system provides a flexible I/O framework that allows the CPU to interact with various virtual and real hardware components.

#### Class: `DeviceManager`

**Purpose**: Manages device lifecycle and port mapping for I/O operations.

```cpp
/**
 * Central manager for all devices connected to the virtual computer
 * Handles port mapping, device lifecycle, and I/O routing
 */
class DeviceManager {
public:
    DeviceManager();
    ~DeviceManager();
    
    // Device registration
    void register_device(std::unique_ptr<Device> device, uint8_t port);
    void unregister_device(uint8_t port);
    
    // I/O operations
    uint8_t read_port(uint8_t port);
    void write_port(uint8_t port, uint8_t value);
    uint16_t read_port_word(uint8_t port);
    void write_port_word(uint8_t port, uint16_t value);
    uint32_t read_port_dword(uint8_t port);
    void write_port_dword(uint8_t port, uint32_t value);
    
    // String I/O
    std::string read_string(uint8_t port);
    void write_string(uint8_t port, const std::string& str);
    
    // Management
    void update_all();
    Device* get_device(uint8_t port);
    const std::map<uint8_t, std::unique_ptr<Device>>& get_devices() const;
};
```

#### Base Device Interface

```cpp
/**
 * Abstract base class for all devices in the virtual computer
 * Provides standard interface for I/O operations
 */
class Device {
public:
    virtual ~Device() = default;
    
    // Basic I/O
    virtual uint8_t read() = 0;
    virtual void write(uint8_t value) = 0;
    
    // Extended I/O (optional)
    virtual uint16_t read_word() { return read() | (read() << 8); }
    virtual void write_word(uint16_t value) { write(value & 0xFF); write(value >> 8); }
    virtual uint32_t read_dword() { return read_word() | (read_word() << 16); }
    virtual void write_dword(uint32_t value) { write_word(value & 0xFFFF); write_word(value >> 16); }
    
    // String I/O (optional)
    virtual std::string read_string() { return ""; }
    virtual void write_string(const std::string& str) {}
    
    // Management
    virtual void update() {}
    virtual std::string get_type() const = 0;
    virtual std::string get_info() const { return get_type(); }
};
```

### Available Devices

#### ConsoleDevice
**Purpose**: Provides text-based input/output for program interaction.

```cpp
/**
 * Console device for text I/O operations
 * Supports both character and string-based communication
 */
class ConsoleDevice : public Device {
    // Implements standard input/output with buffering
    // Supports both blocking and non-blocking reads
    // Handles string operations for text processing
};
```

#### FileDevice
**Purpose**: Enables file system access for the virtual computer.

```cpp
/**
 * File system access device
 * Allows programs to read from and write to host filesystem
 */
class FileDevice : public Device {
    // File operations: open, close, read, write
    // Path resolution and security sandboxing
    // Error handling and status reporting
};
```

#### SerialPortDevice
**Purpose**: Interfaces with real hardware serial ports.

```cpp
/**
 * Real hardware serial port interface
 * Enables communication with external devices
 */
class SerialPortDevice : public Device {
    // Hardware abstraction for serial communication
    // Configurable baud rate, parity, stop bits
    // Asynchronous operation with buffering
};
```

### Debug Interface

**File**: `src/debug/gui.cpp`

The debug interface provides comprehensive visualization and control over the virtual computer's execution.

#### Class: `DebugGUI`

**Purpose**: ImGui-based graphical debugger for real-time system inspection.

```cpp
/**
 * ImGui-based debug interface for VirtComp
 * Provides real-time visualization of CPU state, memory, and device status
 */
class DebugGUI {
public:
    DebugGUI(CPU* cpu, DeviceManager* device_manager);
    ~DebugGUI();
    
    // Main interface
    bool initialize();
    void render();
    void shutdown();
    
    // Control
    bool should_close() const;
    void handle_events();
    
private:
    // UI Panels
    void render_cpu_state();
    void render_memory_view();
    void render_device_status();
    void render_control_panel();
    void render_disassembly();
    
    // State management
    CPU* cpu;
    DeviceManager* device_manager;
    bool show_cpu_window;
    bool show_memory_window;
    bool show_device_window;
    uint32_t memory_view_address;
    size_t memory_view_size;
};
```

#### Debug Features

1. **CPU State Display**: Real-time register values, flags, and execution status
2. **Memory Viewer**: Hexadecimal memory dump with navigation
3. **Device Monitor**: Live device status and I/O activity
4. **Execution Control**: Step-by-step execution, breakpoints, and flow control
5. **Disassembly**: Instruction disassembly with address mapping

### Main Application

**File**: `src/main.cpp`

The main application handles command-line parsing, system initialization, and execution modes.

#### Key Functions

```cpp
/**
 * Main application entry point
 * Handles command-line arguments and system initialization
 */
int main(int argc, char* argv[]);

/**
 * Parse and validate command-line arguments
 * Returns parsed configuration or exits on error
 */
Config parse_arguments(int argc, char* argv[]);

/**
 * Initialize and run the virtual computer
 * Supports both GUI and headless modes
 */
int run_virtcomp(const Config& config);
```

#### Command-Line Interface

The application supports various execution modes:

- **Program Execution**: `virtcomp program.hex`
- **GUI Mode**: `virtcomp program.hex --gui`
- **Interactive Mode**: `virtcomp --interactive`
- **Help**: `virtcomp --help`

## API Reference

### Instruction Set Architecture

The CPU implements a comprehensive instruction set organized into categories:

#### Basic Operations
- `NOP`: No operation
- `HALT`: Stop execution
- `LOAD_IMM Rd, imm32`: Load immediate value into register
- `MOV Rd, Rs`: Copy register value

#### Arithmetic Operations
- `ADD Rd, Rs`: Add registers
- `SUB Rd, Rs`: Subtract registers
- `MUL Rd, Rs`: Multiply registers
- `DIV Rd, Rs`: Divide registers
- `INC Rd`: Increment register
- `DEC Rd`: Decrement register

#### Memory Operations
- `LOAD Rd, [Rs]`: Load from memory address in Rs
- `STORE [Rd], Rs`: Store Rs to memory address in Rd

#### Control Flow
- `JMP addr`: Unconditional jump
- `CMP Rd, Rs`: Compare registers (sets flags)
- `JZ addr`: Jump if zero flag set
- `JNZ addr`: Jump if zero flag clear
- `JS addr`: Jump if sign flag set
- `JNS addr`: Jump if sign flag clear

#### Stack Operations
- `PUSH Rs`: Push register to stack
- `POP Rd`: Pop from stack to register
- `CALL addr`: Call subroutine
- `RET`: Return from subroutine
- `PUSH_ARG Rs`: Push function argument
- `POP_ARG Rd`: Pop function argument

#### Bitwise Operations
- `AND Rd, Rs`: Bitwise AND
- `OR Rd, Rs`: Bitwise OR
- `XOR Rd, Rs`: Bitwise XOR
- `NOT Rd`: Bitwise NOT
- `SHL Rd, imm`: Shift left
- `SHR Rd, imm`: Shift right

#### I/O Operations
- `IN Rd, port`: Read byte from port
- `OUT port, Rs`: Write byte to port
- `INW Rd, port`: Read word from port
- `OUTW port, Rs`: Write word to port
- `INL Rd, port`: Read dword from port
- `OUTL port, Rs`: Write dword to port
- `INSTR Rd, port`: Read string from port
- `OUTSTR port, Rs`: Write string to port

#### Data Definition
- `DB value`: Define byte in program

### Error Handling

The system uses a combination of return codes and exception handling:

```cpp
enum class CPUError {
    NONE,
    INVALID_INSTRUCTION,
    MEMORY_ACCESS_VIOLATION,
    STACK_OVERFLOW,
    STACK_UNDERFLOW,
    DIVISION_BY_ZERO,
    DEVICE_ERROR
};
```

### Memory Layout

```
High Memory  ┌─────────────────┐
            │     Stack       │  ← SP points here
            ├─────────────────┤
            │       ▼         │  (Stack grows down)
            │                 │
            │   Free Space    │
            │                 │
            │       ▲         │  (Heap grows up)
            ├─────────────────┤
            │      Heap       │
            ├─────────────────┤
            │      Data       │
            ├─────────────────┤
            │      Code       │  ← PC starts here
Low Memory  └─────────────────┘  Address 0x0000
```

## Build System

**File**: `Makefile`

The build system uses GNU Make with the following targets:

- `make` or `make all`: Build the main executable
- `make clean`: Remove build artifacts
- `make debug`: Build with debug symbols
- `make release`: Build optimized version
- `make install`: Install to system (if supported)

### Dependencies

- **C++17** compatible compiler (GCC, Clang)
- **fmt library** for string formatting
- **ImGui** for debug interface
- **OpenGL** for GUI rendering (when using debug mode)

### Build Configuration

```makefile
# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Include paths
INCLUDES = -Isrc -Ithird_party

# Libraries
LIBS = -lfmt -lGL -lglfw

# Source files
SOURCES = $(wildcard src/**/*.cpp) $(wildcard src/*.cpp)

# Output
TARGET = bin/virtcomp
```

## Development Guidelines

### Code Style

1. **Naming Convention**:
   - Classes: `PascalCase`
   - Functions/Variables: `snake_case`
   - Constants: `UPPER_CASE`
   - Private members: trailing underscore

2. **Documentation**:
   - Use `/** */` comments for API documentation
   - Include purpose, parameters, and return values
   - Document complex algorithms inline

3. **Error Handling**:
   - Use exceptions for unrecoverable errors
   - Return error codes for expected failures
   - Always validate input parameters

### Testing

Test programs are located in the `tests/` directory and use hex format:

```hex
# Hello World Program
01 00 48 65 6C 6C 6F  # LOAD_IMM R0, "Hello"
...
FF                    # HALT
```

### Contributing

1. Follow the existing code style
2. Add appropriate documentation
3. Include test cases for new features
4. Update this documentation for API changes

## Future Enhancements

- Assembly language compiler
- Advanced debugging features (breakpoints, watchpoints)
- Network device support
- Graphics/display devices
- Performance optimization tools
