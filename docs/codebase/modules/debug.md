# Debug and Logging System

**Files:** `src/debug/logger.hpp`, `src/debug/logger.cpp`, `src/debug/gui.hpp`, `src/debug/gui.cpp`  
**Purpose:** Comprehensive debugging, logging, and graphical debug interface

## Overview

The debug system provides multiple layers of debugging support for VirtComp, including structured logging, real-time GUI debugging, and comprehensive error tracking. It enables both development debugging and educational use cases.

## Logging Framework

### Logger Class

The centralized logging system supports multiple severity levels and output destinations:

```cpp
namespace Logging {
    enum class LogLevel {
        DEBUG = 0,
        INFO = 1,
        WARNING = 2,
        ERROR = 3,
        SUCCESS = 4
    };
    
    class Logger {
    public:
        static Logger& getInstance();
        
        // Core logging methods
        void log(LogLevel level, const std::string& message);
        void debug(const std::string& message);
        void info(const std::string& message);
        void warning(const std::string& message);
        void error(const std::string& message);
        void success(const std::string& message);
        
        // Configuration
        void set_log_level(LogLevel level);
        void enable_file_logging(const std::string& filename);
        void enable_console_logging(bool enabled);
        
        // Formatting
        void set_timestamp_format(const std::string& format);
        void enable_colors(bool enabled);
    };
}
```

### Usage Examples

```cpp
using Logging::Logger;

// Basic logging
Logger::getInstance().info("Starting VirtComp execution");
Logger::getInstance().error("Invalid opcode: 0xFF");
Logger::getInstance().success("Assembly completed successfully");

// Macro shortcuts (if defined)
LOG_DEBUG("CPU state: PC=" + std::to_string(pc));
LOG_ERROR("Memory access violation at address: 0x" + 
          std::hex + address);
```

### Log Output Format

```
[25-07-17 19:47:05.070] [INFO] Starting VirtComp execution
[25-07-17 19:47:05.071] [DEBUG] Loading program: examples/hello_world.hex
[25-07-17 19:47:05.072] [SUCCESS] Assembly completed successfully
[25-07-17 19:47:05.073] [ERROR] Invalid register access: R99
```

## Graphical Debug Interface

### GUI System Architecture

The GUI debugging system is built on ImGui and provides real-time visualization:

```cpp
class DebugGUI {
public:
    DebugGUI();
    ~DebugGUI();
    
    // Lifecycle management
    bool initialize();
    void cleanup();
    bool should_close() const;
    
    // Main rendering loop
    void begin_frame();
    void render();
    void end_frame();
    
    // CPU state integration
    void set_cpu(CPU* cpu);
    void update_cpu_state();
    
    // Debug controls
    void set_breakpoint(uint32_t address);
    void clear_breakpoint(uint32_t address);
    bool is_breakpoint(uint32_t address) const;
    
    // Execution control
    void step_instruction();
    void continue_execution();
    void pause_execution();
    void reset_cpu();
};
```

### GUI Windows and Panels

#### 1. CPU State Window
- **Registers:** All 50 registers with real-time values
- **Flags:** Visual indicators for Z, C, O, S flags
- **Program Counter:** Current execution address
- **Stack Pointer:** Stack state monitoring

#### 2. Memory Viewer
- **Hex Dump:** Raw memory contents
- **Address Navigation:** Jump to specific addresses
- **Search Functionality:** Find patterns in memory
- **Memory Editing:** Runtime memory modification

#### 3. Disassembly Window
- **Current Instruction:** Highlighted current opcode
- **Assembly View:** Human-readable instruction display
- **Breakpoints:** Visual breakpoint indicators
- **Jump Targets:** Label and address resolution

#### 4. Device Monitor
- **Device List:** All registered devices
- **I/O History:** Recent device operations
- **Device Status:** Ready/busy state indicators
- **Traffic Analysis:** I/O bandwidth monitoring

#### 5. Console Output
- **Program Output:** Live console device output
- **Clear/Save:** Output management functions
- **Formatting:** ASCII and hex display modes

#### 6. Control Panel
- **Execution Controls:** Play, pause, step, reset buttons
- **Speed Control:** Execution speed adjustment
- **Debug Settings:** Configure debug behavior
- **File Operations:** Load/save debug sessions

### Breakpoint System

```cpp
class BreakpointManager {
public:
    void set_breakpoint(uint32_t address, const std::string& condition = "");
    void clear_breakpoint(uint32_t address);
    void clear_all_breakpoints();
    
    bool should_break(uint32_t address, const CPU& cpu) const;
    std::vector<uint32_t> get_breakpoints() const;
    
    // Conditional breakpoints
    void set_conditional_breakpoint(uint32_t address, 
                                  const std::string& condition);
    
private:
    struct Breakpoint {
        uint32_t address;
        std::string condition;
        bool enabled;
        uint32_t hit_count;
    };
    
    std::vector<Breakpoint> breakpoints;
};
```

### Interactive Features

#### Memory Editing
```cpp
// Runtime memory modification
gui.memory_editor.set_read_only(false);
gui.memory_editor.highlight_address(0x1000);

// Memory search
auto results = gui.memory_search("48 65 6C 6C 6F");  // "Hello"
```

#### Register Modification
```cpp
// Interactive register editing
if (gui.register_editor.was_modified()) {
    uint8_t reg = gui.register_editor.get_modified_register();
    uint32_t value = gui.register_editor.get_new_value();
    cpu.set_register(reg, value);
}
```

## Integration with Main Application

### Debug Mode Activation

```cpp
int main(int argc, char* argv[]) {
    // Command line parsing enables debug mode
    if (Config::enable_gui) {
        DebugGUI gui;
        if (!gui.initialize()) {
            LOG_ERROR("Failed to initialize debug GUI");
            return 1;
        }
        
        // Debug execution loop
        while (!gui.should_close() && cpu.is_running()) {
            gui.begin_frame();
            
            // Check for breakpoints
            if (gui.is_breakpoint(cpu.get_pc())) {
                gui.pause_execution();
            }
            
            // Step execution if not paused
            if (!gui.is_paused()) {
                cpu.step();
            }
            
            gui.update_cpu_state();
            gui.render();
            gui.end_frame();
        }
        
        gui.cleanup();
    }
}
```

### Debug Information Collection

```cpp
class CPUStateCollector {
public:
    struct StateSnapshot {
        uint32_t pc;
        uint32_t sp;
        std::array<uint32_t, 50> registers;
        CPUFlags flags;
        std::vector<uint8_t> memory_sample;
        std::chrono::steady_clock::time_point timestamp;
    };
    
    void capture_state(const CPU& cpu);
    std::vector<StateSnapshot> get_history() const;
    void clear_history();
    
private:
    std::vector<StateSnapshot> state_history;
    size_t max_history_size = 1000;
};
```

## Error Tracking and Debugging

### Debug Assertions

```cpp
#ifdef DEBUG_MODE
    #define ASSERT(condition, message) \
        do { \
            if (!(condition)) { \
                LOG_ERROR("Assertion failed: " + std::string(message)); \
                LOG_ERROR("File: " + std::string(__FILE__) + \
                         ", Line: " + std::to_string(__LINE__)); \
                std::abort(); \
            } \
        } while(0)
#else
    #define ASSERT(condition, message) ((void)0)
#endif
```

### Error Context Collection

```cpp
class ErrorContext {
public:
    void push_context(const std::string& context);
    void pop_context();
    std::string get_full_context() const;
    
    void add_cpu_state(const CPU& cpu);
    void add_memory_dump(uint32_t address, size_t length);
    
private:
    std::vector<std::string> context_stack;
    std::optional<CPUStateCollector::StateSnapshot> cpu_snapshot;
    std::vector<uint8_t> memory_dump;
};
```

## Performance Monitoring

### Execution Profiling

```cpp
class ExecutionProfiler {
public:
    void start_profiling();
    void stop_profiling();
    
    void record_instruction(uint8_t opcode);
    void record_memory_access(uint32_t address, bool is_write);
    void record_device_io(int device_id, bool is_write);
    
    // Analysis
    std::map<uint8_t, uint64_t> get_instruction_counts() const;
    std::map<uint32_t, uint64_t> get_memory_hotspots() const;
    double get_instructions_per_second() const;
    
private:
    bool profiling_enabled;
    std::chrono::steady_clock::time_point start_time;
    std::map<uint8_t, uint64_t> instruction_counts;
    std::map<uint32_t, uint64_t> memory_access_counts;
    uint64_t total_instructions;
};
```

### Debug Configuration

```cpp
struct DebugConfig {
    bool enable_gui = false;
    bool enable_file_logging = true;
    bool enable_console_logging = true;
    bool enable_profiling = false;
    bool enable_memory_tracking = false;
    std::string log_file = "debug.log";
    LogLevel minimum_log_level = LogLevel::INFO;
    
    // GUI settings
    bool show_memory_window = true;
    bool show_registers_window = true;
    bool show_disassembly_window = true;
    bool show_device_monitor = true;
    int gui_refresh_rate = 60;  // FPS
};
```

## Debug Build Features

### Compile-Time Debug Options

```cpp
// Debug macros for development builds
#ifdef DEBUG_BUILD
    #define DEBUG_TRACE(msg) LOG_DEBUG(msg)
    #define DEBUG_ASSERT(cond) ASSERT(cond, #cond)
    #define DEBUG_DUMP_CPU(cpu) dump_cpu_state(cpu)
#else
    #define DEBUG_TRACE(msg) ((void)0)
    #define DEBUG_ASSERT(cond) ((void)0)
    #define DEBUG_DUMP_CPU(cpu) ((void)0)
#endif
```

### Memory Debugging

```cpp
#ifdef DEBUG_MEMORY
class MemoryDebugger {
public:
    void track_allocation(void* ptr, size_t size);
    void track_deallocation(void* ptr);
    void dump_leaks();
    
private:
    std::map<void*, size_t> allocations;
};
#endif
```

## Usage Examples

### Basic Debug Logging

```cpp
// In CPU execution loop
LOG_DEBUG("Executing instruction: " + opcode_to_string(opcode));
LOG_DEBUG("PC: 0x" + to_hex(pc) + ", SP: 0x" + to_hex(sp));

if (invalid_register(reg)) {
    LOG_ERROR("Invalid register access: R" + std::to_string(reg));
    return false;
}

LOG_SUCCESS("Program execution completed successfully");
```

### GUI Debug Session

```bash
# Launch with debug GUI
./bin/virtcomp -H program.hex --gui --debug

# Debug controls:
# - Space: Step single instruction
# - F5: Continue execution
# - F9: Toggle breakpoint at current address
# - Ctrl+R: Reset CPU state
```

### Conditional Breakpoint Example

```cpp
// Break when R0 contains value 42
gui.set_conditional_breakpoint(0x100, "R0 == 42");

// Break when accessing specific memory
gui.set_conditional_breakpoint(0x200, "memory[0x1000] != 0");
```

The debug system provides comprehensive development and educational debugging capabilities, making VirtComp suitable for both learning assembly programming and developing complex virtual machine applications.
