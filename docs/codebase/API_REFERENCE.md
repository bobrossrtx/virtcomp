# DemiEngine API Reference

This document provides detailed API documentation for all classes, functions, and interfaces in the DemiEngine codebase.

## Table of Contents

- [CPU Class](#cpu-class)
- [Device System](#device-system)
- [Device Manager](#device-manager)
- [Individual Devices](#individual-devices)
- [Debug Interface](#debug-interface)
- [Utility Functions](#utility-functions)
- [Error Handling](#error-handling)

## CPU Class

### `class CPU`

**File**: `src/vhardware/cpu.hpp`, `src/vhardware/cpu.cpp`

The core CPU emulation class implementing the DemiEngine 32-bit architecture.

#### Constructor

```cpp
/**
 * @brief Construct a new CPU with specified memory size
 * @param memory_size Size of memory in bytes (default: 64KB)
 */
CPU(size_t memory_size = 64 * 1024);
```

#### Core Execution Methods

```cpp
/**
 * @brief Reset CPU to initial state
 * Clears all registers, resets PC to 0, initializes stack pointer
 */
void reset();

/**
 * @brief Execute a single instruction
 * Fetches instruction at PC, decodes and executes it, updates PC
 * @throws std::runtime_error on invalid instruction or memory access
 */
void step();

/**
 * @brief Run CPU until HALT instruction or error
 * Continuously calls step() until halted flag is set
 */
void run();
```

#### Register Access

```cpp
/**
 * @brief Get value of specified register
 * @param reg Register identifier (R0-R7)
 * @return 32-bit register value
 */
uint32_t get_register(Register reg) const;

/**
 * @brief Set value of specified register  
 * @param reg Register identifier (R0-R7)
 * @param value 32-bit value to store
 */
void set_register(Register reg, uint32_t value);

/**
 * @brief Get current program counter value
 * @return 32-bit PC address
 */
uint32_t get_pc() const { return pc; }

/**
 * @brief Set program counter to specified address
 * @param address 32-bit target address
 */
void set_pc(uint32_t address) { pc = address; }

/**
 * @brief Get current stack pointer value
 * @return 32-bit SP address
 */
uint32_t get_sp() const { return sp; }

/**
 * @brief Get current frame pointer value
 * @return 32-bit FP address  
 */
uint32_t get_fp() const { return fp; }

/**
 * @brief Get current CPU flags
 * @return 32-bit flags register
 */
uint32_t get_flags() const { return flags; }
```

#### Memory Operations

```cpp
/**
 * @brief Read single byte from memory
 * @param address 32-bit memory address
 * @return 8-bit value at address
 * @throws std::out_of_range if address is invalid
 */
uint8_t read_memory(uint32_t address) const;

/**
 * @brief Write single byte to memory
 * @param address 32-bit memory address
 * @param value 8-bit value to write
 * @throws std::out_of_range if address is invalid
 */
void write_memory(uint32_t address, uint8_t value);

/**
 * @brief Read 32-bit word from memory (little-endian)
 * @param address 32-bit memory address
 * @return 32-bit value at address
 */
uint32_t read_memory_word(uint32_t address) const;

/**
 * @brief Write 32-bit word to memory (little-endian)
 * @param address 32-bit memory address
 * @param value 32-bit value to write
 */
void write_memory_word(uint32_t address, uint32_t value);
```

#### Device Integration

```cpp
/**
 * @brief Set device manager for I/O operations
 * @param dm Pointer to DeviceManager instance
 */
void set_device_manager(DeviceManager* dm) { device_manager = dm; }
```

#### Program Loading

```cpp
/**
 * @brief Load hex program from file
 * @param filename Path to hex program file
 * @return true if successful, false on error
 */
bool load_hex_program(const std::string& filename);
```

#### Status and Debug

```cpp
/**
 * @brief Check if CPU is halted
 * @return true if HALT instruction executed or error occurred
 */
bool is_halted() const { return halted; }

/**
 * @brief Get last error message
 * @return String describing last error, empty if no error
 */
const std::string& get_last_error() const { return last_error; }
```

#### Register Enumeration

```cpp
enum class Register {
    R0 = 0, R1 = 1, R2 = 2, R3 = 3,
    R4 = 4, R5 = 5, R6 = 6, R7 = 7
};
```

## Device System

### `class Device`

**File**: `src/vhardware/device.hpp`

Abstract base class for all devices in the DemiEngine system.

#### Pure Virtual Methods

```cpp
/**
 * @brief Read single byte from device
 * @return 8-bit value from device
 */
virtual uint8_t read() = 0;

/**
 * @brief Write single byte to device
 * @param value 8-bit value to write
 */
virtual void write(uint8_t value) = 0;

/**
 * @brief Get device type identifier
 * @return String identifying device type
 */
virtual std::string get_type() const = 0;
```

#### Virtual Methods (Optional Override)

```cpp
/**
 * @brief Read 16-bit word from device
 * Default implementation calls read() twice
 * @return 16-bit value from device
 */
virtual uint16_t read_word();

/**
 * @brief Write 16-bit word to device  
 * Default implementation calls write() twice
 * @param value 16-bit value to write
 */
virtual void write_word(uint16_t value);

/**
 * @brief Read 32-bit dword from device
 * @return 32-bit value from device
 */
virtual uint32_t read_dword();

/**
 * @brief Write 32-bit dword to device
 * @param value 32-bit value to write
 */
virtual void write_dword(uint32_t value);

/**
 * @brief Read string from device
 * @return String read from device (empty by default)
 */
virtual std::string read_string();

/**
 * @brief Write string to device
 * @param str String to write (ignored by default)
 */
virtual void write_string(const std::string& str);

/**
 * @brief Update device state
 * Called periodically to allow device processing
 */
virtual void update();

/**
 * @brief Get detailed device information
 * @return String with device details (type by default)
 */
virtual std::string get_info() const;
```

## Device Manager

### `class DeviceManager`

**File**: `src/vhardware/device_manager.hpp`, `src/vhardware/device_manager.cpp`

Central manager for all devices connected to the virtual computer.

#### Device Registration

```cpp
/**
 * @brief Register device at specified port
 * @param device Unique pointer to device instance
 * @param port 8-bit port number (0-255)
 * @throws std::invalid_argument if port already occupied
 */
void register_device(std::unique_ptr<Device> device, uint8_t port);

/**
 * @brief Unregister device from port
 * @param port 8-bit port number
 */
void unregister_device(uint8_t port);

/**
 * @brief Get device at specified port
 * @param port 8-bit port number
 * @return Pointer to device or nullptr if no device at port
 */
Device* get_device(uint8_t port);
```

#### I/O Operations

```cpp
/**
 * @brief Read byte from device port
 * @param port 8-bit port number
 * @return 8-bit value from device (0 if no device)
 */
uint8_t read_port(uint8_t port);

/**
 * @brief Write byte to device port
 * @param port 8-bit port number
 * @param value 8-bit value to write
 */
void write_port(uint8_t port, uint8_t value);

/**
 * @brief Read 16-bit word from device port
 * @param port 8-bit port number  
 * @return 16-bit value from device
 */
uint16_t read_port_word(uint8_t port);

/**
 * @brief Write 16-bit word to device port
 * @param port 8-bit port number
 * @param value 16-bit value to write
 */
void write_port_word(uint8_t port, uint16_t value);

/**
 * @brief Read 32-bit dword from device port
 * @param port 8-bit port number
 * @return 32-bit value from device
 */
uint32_t read_port_dword(uint8_t port);

/**
 * @brief Write 32-bit dword to device port
 * @param port 8-bit port number
 * @param value 32-bit value to write
 */
void write_port_dword(uint8_t port, uint32_t value);

/**
 * @brief Read string from device port
 * @param port 8-bit port number
 * @return String from device
 */
std::string read_string(uint8_t port);

/**
 * @brief Write string to device port
 * @param port 8-bit port number
 * @param str String to write
 */
void write_string(uint8_t port, const std::string& str);
```

#### Management

```cpp
/**
 * @brief Update all registered devices
 * Calls update() on each device to allow periodic processing
 */
void update_all();

/**
 * @brief Get map of all registered devices
 * @return Const reference to device map (port -> device)
 */
const std::map<uint8_t, std::unique_ptr<Device>>& get_devices() const;
```

## Individual Devices

### ConsoleDevice

**File**: `src/vhardware/devices/console_device.hpp`

Provides text-based console I/O functionality.

```cpp
class ConsoleDevice : public Device {
public:
    /**
     * @brief Construct console device with optional input/output streams
     * @param input Input stream (default: std::cin)
     * @param output Output stream (default: std::cout)
     */
    ConsoleDevice(std::istream& input = std::cin, std::ostream& output = std::cout);
    
    // Device interface implementation
    uint8_t read() override;
    void write(uint8_t value) override;
    std::string read_string() override;
    void write_string(const std::string& str) override;
    std::string get_type() const override { return "Console"; }
};
```

### FileDevice

**File**: `src/vhardware/devices/file_device.hpp`

Provides file system access functionality.

```cpp
class FileDevice : public Device {
public:
    /**
     * @brief Construct file device with optional base directory
     * @param base_dir Base directory for file operations (default: current)
     */
    FileDevice(const std::string& base_dir = ".");
    
    // Device interface implementation
    uint8_t read() override;
    void write(uint8_t value) override;
    std::string get_type() const override { return "File"; }
    
    // File-specific operations
    bool open_file(const std::string& filename, bool write_mode = false);
    void close_file();
    bool is_open() const;
};
```

### CounterDevice

**File**: `src/vhardware/devices/counter_device.hpp`

Simple counter device for testing and timing operations.

```cpp
class CounterDevice : public Device {
public:
    CounterDevice();
    
    // Device interface implementation  
    uint8_t read() override;
    void write(uint8_t value) override;
    void update() override;
    std::string get_type() const override { return "Counter"; }
    
    // Counter-specific operations
    uint32_t get_count() const;
    void set_count(uint32_t value);
    void reset();
};
```

### SerialPortDevice

**File**: `src/vhardware/devices/serial_port_device.hpp`

Interface to real hardware serial ports.

```cpp
class SerialPortDevice : public Device {
public:
    /**
     * @brief Construct serial port device
     * @param port_name System name of serial port (e.g., "/dev/ttyUSB0")
     * @param baud_rate Communication speed (default: 9600)
     */
    SerialPortDevice(const std::string& port_name, int baud_rate = 9600);
    
    // Device interface implementation
    uint8_t read() override;
    void write(uint8_t value) override;
    std::string get_type() const override { return "SerialPort"; }
    
    // Serial-specific operations
    bool open();
    void close();
    bool is_open() const;
    void set_baud_rate(int baud_rate);
};
```

## Debug Interface

### `class DebugGUI`

**File**: `src/debug/gui.hpp`, `src/debug/gui.cpp`

ImGui-based graphical debugger interface.

```cpp
class DebugGUI {
public:
    /**
     * @brief Construct debug GUI
     * @param cpu Pointer to CPU instance to debug
     * @param device_manager Pointer to device manager
     */
    DebugGUI(CPU* cpu, DeviceManager* device_manager);
    
    /**
     * @brief Initialize GUI system
     * @return true if successful, false on error
     */
    bool initialize();
    
    /**
     * @brief Render one frame of GUI
     * Should be called in main loop
     */
    void render();
    
    /**
     * @brief Shutdown GUI system
     * Cleanup resources and windows
     */
    void shutdown();
    
    /**
     * @brief Check if user requested to close window
     * @return true if should exit
     */
    bool should_close() const;
    
    /**
     * @brief Handle window and input events
     */
    void handle_events();
};
```

## Utility Functions

### Hex Program Loading

**File**: `src/main.cpp`

```cpp
/**
 * @brief Load hex program from file into CPU memory
 * @param filename Path to hex program file
 * @param cpu CPU instance to load program into
 * @return true if successful, false on error
 */
bool load_hex_program(const std::string& filename, CPU& cpu);

/**
 * @brief Parse hex string into byte value
 * @param hex_str Two-character hex string (e.g., "FF")
 * @return Byte value (0-255)
 * @throws std::invalid_argument if invalid hex
 */
uint8_t parse_hex_byte(const std::string& hex_str);
```

### Command Line Parsing

```cpp
/**
 * @brief Parse command line arguments
 * @param argc Argument count
 * @param argv Argument vector
 * @return Configuration structure
 */
struct Config {
    std::string program_file;  // Hex file path
    bool enable_gui;
    bool interactive_mode;
    bool show_help;
};

Config parse_arguments(int argc, char* argv[]);
```

## Error Handling

### Exception Types

```cpp
/**
 * @brief CPU execution error
 */
class CPUError : public std::runtime_error {
public:
    enum Type {
        INVALID_INSTRUCTION,
        MEMORY_ACCESS_VIOLATION,
        STACK_OVERFLOW,
        STACK_UNDERFLOW,
        DIVISION_BY_ZERO,
        DEVICE_ERROR
    };
    
    CPUError(Type type, const std::string& message);
    Type get_type() const { return type_; }
};

/**
 * @brief Device operation error
 */
class DeviceError : public std::runtime_error {
public:
    DeviceError(const std::string& device_type, const std::string& message);
};
```

### Error Codes

```cpp
enum class ErrorCode {
    SUCCESS = 0,
    INVALID_ARGUMENTS,
    FILE_NOT_FOUND,
    PROGRAM_LOAD_ERROR,
    EXECUTION_ERROR,
    DEVICE_INIT_ERROR,
    GUI_INIT_ERROR
};
```

## Constants and Macros

### Memory Layout

```cpp
constexpr uint32_t DEFAULT_MEMORY_SIZE = 64 * 1024;  // 64KB
constexpr uint32_t STACK_START = 0xFFFFFFFF;         // Stack starts at top
constexpr uint32_t PROGRAM_START = 0x00000000;       // Programs start at 0
```

### CPU Flags

```cpp
constexpr uint32_t FLAG_ZERO = 0x01;    // Zero flag
constexpr uint32_t FLAG_SIGN = 0x02;    // Sign flag  
constexpr uint32_t FLAG_CARRY = 0x04;   // Carry flag
constexpr uint32_t FLAG_OVERFLOW = 0x08; // Overflow flag
```

### Instruction Opcodes

```cpp
constexpr uint8_t OP_NOP = 0x00;
constexpr uint8_t OP_LOAD_IMM = 0x01;
constexpr uint8_t OP_ADD = 0x02;
constexpr uint8_t OP_SUB = 0x03;
constexpr uint8_t OP_MOV = 0x04;
constexpr uint8_t OP_JMP = 0x05;
constexpr uint8_t OP_LOAD = 0x06;
constexpr uint8_t OP_STORE = 0x07;
// ... (see full list in cpu.hpp)
constexpr uint8_t OP_HALT = 0xFF;
```

This API reference provides comprehensive documentation for all public interfaces in the DemiEngine codebase, enabling developers to understand and extend the system effectively.
