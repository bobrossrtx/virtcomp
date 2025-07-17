# Device Management System

**Files:** `src/vhardware/device_manager.hpp`, `src/vhardware/device_manager.cpp`  
**Purpose:** Hardware abstraction layer for I/O devices

## Overview

The device management system provides a unified interface for all I/O operations in VirtComp. It abstracts hardware devices behind a consistent API, enabling the CPU to communicate with various peripherals through standardized device IDs.

## Architecture

### Core Components

#### DeviceManager Class
Central coordinator for all device operations:

```cpp
class DeviceManager {
public:
    DeviceManager();
    ~DeviceManager();
    
    // Device registration
    void register_device(int device_id, std::unique_ptr<Device> device);
    void unregister_device(int device_id);
    
    // I/O operations
    bool write_to_device(int device_id, uint32_t data);
    uint32_t read_from_device(int device_id);
    
    // Device queries
    bool device_exists(int device_id) const;
    std::vector<int> get_registered_devices() const;
    
    // Debugging
    void list_devices() const;
    std::string get_device_info(int device_id) const;
};
```

#### Device Base Class
Abstract interface that all devices must implement:

```cpp
class Device {
public:
    virtual ~Device() = default;
    
    // Core I/O operations
    virtual bool write(uint32_t data) = 0;
    virtual uint32_t read() = 0;
    
    // Device information
    virtual std::string get_name() const = 0;
    virtual std::string get_description() const = 0;
    virtual bool is_readable() const = 0;
    virtual bool is_writable() const = 0;
    
    // State management
    virtual void reset() = 0;
    virtual bool is_ready() const = 0;
};
```

## Standard Devices

### Console Output Device (ID: 1)
**Purpose:** Character output to terminal/console

```cpp
class ConsoleDevice : public Device {
public:
    bool write(uint32_t data) override {
        char c = static_cast<char>(data & 0xFF);
        std::cout << c << std::flush;
        return true;
    }
    
    uint32_t read() override {
        // Console is write-only
        return 0;
    }
    
    std::string get_name() const override { return "Console"; }
    bool is_writable() const override { return true; }
    bool is_readable() const override { return false; }
};
```

**Usage in Assembly:**
```assembly
LOAD_IMM R0, 72        ; Load 'H' (ASCII 72)
OUT 1, R0              ; Output to console device
```

### Future Device Types
The architecture supports expansion for:

- **Input Devices:** Keyboard, mouse, sensors
- **Storage Devices:** Disk drives, memory cards
- **Network Devices:** Ethernet, wireless interfaces
- **Graphics Devices:** Display controllers, GPU interfaces
- **Audio Devices:** Sound cards, speakers, microphones

## Device Factory Pattern

### DeviceFactory Class
Centralizes device creation and configuration:

```cpp
class DeviceFactory {
public:
    static std::unique_ptr<Device> create_console_device();
    static std::unique_ptr<Device> create_storage_device(const std::string& path);
    static std::unique_ptr<Device> create_network_device(const std::string& interface);
    
    // Device type enumeration
    enum class DeviceType {
        CONSOLE_OUTPUT,
        KEYBOARD_INPUT,
        STORAGE_DRIVE,
        NETWORK_INTERFACE,
        GRAPHICS_DISPLAY
    };
    
    static std::unique_ptr<Device> create_device(DeviceType type, 
                                                const std::string& config = "");
};
```

### Initialization Process
Device setup occurs during application startup:

```cpp
void initialize_devices() {
    using namespace vhw;
    
    DeviceManager& dm = DeviceManager::get_instance();
    
    // Register standard console output
    auto console = DeviceFactory::create_console_device();
    dm.register_device(1, std::move(console));
    
    // Future: Register additional devices
    // auto keyboard = DeviceFactory::create_keyboard_device();
    // dm.register_device(2, std::move(keyboard));
}
```

## CPU Integration

### I/O Instructions
The CPU communicates with devices through dedicated instructions:

#### OUT Instruction
```cpp
case Opcode::OUT: {
    uint8_t device_id = program[pc + 1];
    uint8_t reg = program[pc + 2];
    uint32_t data = registers[reg].get_value();
    
    if (device_manager && device_manager->device_exists(device_id)) {
        device_manager->write_to_device(device_id, data);
    } else {
        log_error("Device " + std::to_string(device_id) + " not found");
    }
    
    pc += 3;
    break;
}
```

#### IN Instruction
```cpp
case Opcode::IN: {
    uint8_t device_id = program[pc + 1];
    uint8_t reg = program[pc + 2];
    
    if (device_manager && device_manager->device_exists(device_id)) {
        uint32_t data = device_manager->read_from_device(device_id);
        registers[reg].set_value(data);
    } else {
        log_error("Device " + std::to_string(device_id) + " not found");
    }
    
    pc += 3;
    break;
}
```

### Memory-Mapped I/O (Future)
Planned extension for memory-mapped device access:

```cpp
// Memory-mapped device regions
const uint32_t DEVICE_MEMORY_START = 0x80000;
const uint32_t DEVICE_MEMORY_END   = 0x8FFFF;

bool CPU::write_memory(uint32_t address, uint8_t value) {
    if (address >= DEVICE_MEMORY_START && address <= DEVICE_MEMORY_END) {
        // Route to device manager
        int device_id = (address - DEVICE_MEMORY_START) >> 12;  // 4KB per device
        uint32_t device_offset = address & 0xFFF;
        return device_manager->write_memory_mapped(device_id, device_offset, value);
    }
    
    // Regular memory access
    memory[address] = value;
    return true;
}
```

## Error Handling

### Device Error Types
```cpp
enum class DeviceError {
    DEVICE_NOT_FOUND,
    DEVICE_NOT_READY,
    DEVICE_READ_ONLY,
    DEVICE_WRITE_ONLY,
    DEVICE_BUSY,
    INVALID_DATA,
    HARDWARE_FAILURE
};
```

### Error Recovery
- **Graceful Degradation:** Continue execution when possible
- **Error Logging:** Detailed device operation logs
- **Fallback Mechanisms:** Default behaviors for missing devices

## Debugging Support

### Device Monitoring
```cpp
class DeviceDebugger {
public:
    void log_device_access(int device_id, bool is_write, uint32_t data);
    void dump_device_state(int device_id);
    std::vector<DeviceAccessLog> get_access_history(int device_id);
    
private:
    struct DeviceAccessLog {
        int device_id;
        bool is_write;
        uint32_t data;
        std::chrono::steady_clock::time_point timestamp;
    };
};
```

### Debug Commands
- **List Devices:** Show all registered devices
- **Device Status:** Check device ready state
- **I/O History:** Track device access patterns
- **Performance Metrics:** Monitor device throughput

## Configuration

### Device Configuration Files
```json
{
    "devices": {
        "console": {
            "id": 1,
            "type": "console_output",
            "enabled": true
        },
        "storage": {
            "id": 3,
            "type": "storage_device",
            "path": "/tmp/virt_storage.img",
            "size": "10MB"
        }
    }
}
```

### Runtime Configuration
```cpp
// Enable/disable devices at runtime
device_manager.set_device_enabled(1, true);   // Enable console
device_manager.set_device_enabled(2, false);  // Disable keyboard
```

## Performance Considerations

### Optimization Features
- **Device Caching:** Frequently accessed device states
- **Lazy Initialization:** Create devices only when needed
- **Batch Operations:** Combine multiple I/O operations
- **Asynchronous I/O:** Non-blocking device operations (future)

### Memory Management
- **RAII Pattern:** Automatic device cleanup
- **Smart Pointers:** Safe device lifecycle management
- **Resource Pooling:** Reuse device objects when possible

## Usage Examples

### Basic Console Output
```assembly
; Output "Hello" to console
LOAD_IMM R0, 72        ; 'H'
OUT 1, R0
LOAD_IMM R0, 101       ; 'e'
OUT 1, R0
LOAD_IMM R0, 108       ; 'l'
OUT 1, R0
LOAD_IMM R0, 108       ; 'l'
OUT 1, R0
LOAD_IMM R0, 111       ; 'o'
OUT 1, R0
```

### C++ Device Management
```cpp
// Initialize device system
DeviceManager dm;
auto console = DeviceFactory::create_console_device();
dm.register_device(1, std::move(console));

// CPU integration
CPU cpu;
cpu.attach_device_manager(&dm);

// Device access through CPU
std::vector<uint8_t> program = {
    0x01, 0x00, 72,     // LOAD_IMM R0, 72 ('H')
    0x1B, 0x01, 0x00,   // OUT 1, R0
    0xFF                // HALT
};

cpu.run(program);
```

The device management system provides a clean, extensible foundation for all I/O operations in VirtComp, supporting both current console output needs and future expansion to complex peripheral devices.
