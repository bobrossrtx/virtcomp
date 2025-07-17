# VirtComp Codebase Documentation

**Complete technical documentation for VirtComp virtual machine developers**

Welcome to the VirtComp codebase documentation. This guide provides comprehensive technical information for developers working on VirtComp itself, including architecture details, API references, and development guidelines.

## Quick Navigation

### Core Documentation
- **[API Reference](API_REFERENCE.md)** - Complete API documentation with examples
- **[Module Documentation](#module-documentation)** - Per-component technical details
- **[Architecture Overview](#architecture-overview)** - System design and patterns
- **[Development Workflow](#development-workflow)** - Building, testing, and contributing

### Developer Resources
- **[Code Style Guide](#code-style-guidelines)** - Coding standards and best practices
- **[Testing Framework](#testing-requirements)** - Unit tests and validation
- **[Build System](#build-system)** - Compilation and dependency management
- **[Debugging Tools](#debugging-and-profiling)** - Debug interfaces and profiling

## Module Documentation

VirtComp is organized into clearly defined modules, each with comprehensive documentation:

### [📱 Main Application (`main.cpp`)](modules/main.md)
**Application orchestration and command-line interface**
- Entry point and system initialization
- Command-line argument parsing with validation
- Execution mode management (hex, assembly, testing, GUI)
- Integration of all subsystems

### [🧠 CPU Core System (`vhardware/cpu.*`)](modules/cpu.md)
**Virtual processor implementation and execution engine**
- Complete instruction set architecture (50+ opcodes)
- Extended register system (50 registers: R0-R49)
- Memory management with 1MB virtual memory
- Flag system for conditional operations and status tracking

### [⚙️ Assembly Language System (`assembler/`)](modules/assembler.md)
**Complete assembly toolchain and compiler**
- **Lexer**: Token generation and syntax analysis
- **Parser**: Abstract syntax tree construction
- **Assembler**: Two-pass bytecode generation with symbol resolution
- Full instruction set support with comprehensive error handling

### [🔌 Device Management (`vhardware/device_manager.*`)](modules/device_manager.md)
**Hardware abstraction and I/O coordination**
- Unified device interface with extensible architecture
- Device factory pattern for component creation
- Console output device with future expansion support
- Memory-mapped I/O capabilities and port management

### [🐛 Debug and Logging System (`debug/`)](modules/debug.md)
**Development tools and runtime analysis**
- Structured logging framework with multiple severity levels
- Real-time GUI debugging interface (ImGui-based)
- Interactive breakpoint system and state inspection
- Performance monitoring and execution profiling

### [🧪 Test Framework (`test/`)](modules/testing.md)
**Comprehensive testing infrastructure**
- Unit testing for all CPU instructions and components
- Integration tests for complete system validation
- Assembly language toolchain testing
- Automated test execution with coverage analysis

## Architecture Overview

VirtComp follows a modular, layered architecture with clear separation of concerns:

```
┌─────────────────────────────────────────────────────────────┐
│                    Application Layer                        │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐  │
│  │ main.cpp    │  │ CLI Parser  │  │ Configuration       │  │
│  │ Entry Point │  │ & Validation│  │ Management          │  │
│  └─────────────┘  └─────────────┘  └─────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                     Core Systems Layer                      │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐  │
│  │ CPU Core    │  │ Assembly    │  │ Device Manager      │  │
│  │ • Execution │  │ Toolchain   │  │ • I/O Abstraction   │  │
│  │ • Registers │  │ • Lexer     │  │ • Device Factory    │  │
│  │ • Memory    │  │ • Parser    │  │ • Port Management   │  │
│  │ • Flags     │  │ • Assembler │  │ • Console Device    │  │
│  └─────────────┘  └─────────────┘  └─────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                  Development Tools Layer                    │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐  │
│  │ Debug GUI   │  │ Logging     │  │ Test Framework      │  │
│  │ • Real-time │  │ Framework   │  │ • Unit Tests        │  │
│  │ • State     │  │ • Structured│  │ • Integration Tests │  │
│  │ • Breakpts  │  │ • Multiple  │  │ • Coverage Analysis │  │
│  │ • Profiling │  │   Levels    │  │ • Automated Runs    │  │
│  └─────────────┘  └─────────────┘  └─────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

### Key Design Principles

#### 🔧 **Modularity**
Each component has a well-defined interface and clear responsibilities, enabling independent development and testing.

#### 🔄 **Extensibility**
Plugin architecture supports adding new devices, instructions, and debugging tools without modifying core systems.

#### 🎯 **Testability**
Dependency injection and interface segregation enable comprehensive unit testing and system validation.

#### 📊 **Observability**
Comprehensive logging, debugging interfaces, and profiling tools provide deep system insights.

#### 🛡️ **Reliability**
Robust error handling, input validation, and graceful degradation ensure stable operation.

## Development Workflow

### Building the System

```bash
# Complete clean build
make clean && make

# Debug build with symbols
make debug

# Release build (optimized)
make release

# Run test suite
make test

# Install system-wide (if supported)
make install
```

### Running VirtComp

```bash
# Execute hex bytecode
./bin/virtcomp -H program.hex

# Assemble and run source code
./bin/virtcomp -A program.asm

# Launch with GUI debugger
./bin/virtcomp -H program.hex --gui

# Run comprehensive test suite
./bin/virtcomp -t

# Compile to standalone executable
./bin/virtcomp -H program.hex -o my_program
```

### File Organization

```
src/                           # Source code
├── main.cpp                   # Application entry point
├── config.hpp                 # Global configuration
├── assembler/                 # Assembly language system
│   ├── lexer.hpp/cpp         # Token generation
│   ├── parser.hpp/cpp        # AST construction
│   ├── assembler.hpp/cpp     # Bytecode generation
│   ├── ast.hpp               # Abstract syntax tree nodes
│   ├── token.hpp             # Token type definitions
│   └── opcodes.hpp           # Instruction mappings
├── vhardware/                # Virtual hardware layer
│   ├── cpu.hpp/cpp           # CPU implementation
│   ├── cpu_registers.hpp/cpp # Register architecture
│   ├── cpu_flags.hpp         # Flag system definitions
│   ├── device_manager.hpp/cpp# Device coordination
│   ├── device_factory.hpp    # Device creation patterns
│   ├── device.hpp            # Device interface
│   └── opcodes/              # Instruction implementations
├── debug/                    # Development and debugging tools
│   ├── logger.hpp/cpp        # Logging framework
│   └── gui.hpp/cpp           # Debug interface (ImGui)
└── test/                     # Testing infrastructure
    ├── test_framework.hpp    # Test execution framework
    ├── test.hpp              # Test case definitions
    └── unit_tests.cpp        # Comprehensive test suite

docs/codebase/                # Technical documentation
├── README.md                 # This overview document
├── API_REFERENCE.md          # Complete API documentation
└── modules/                  # Per-module documentation
    ├── main.md              # Application entry point
    ├── cpu.md               # CPU system details
    ├── assembler.md         # Assembly toolchain
    ├── device_manager.md    # Device management
    ├── debug.md             # Debug and logging
    └── testing.md           # Test framework

examples/                     # Working assembly programs
├── hello_world.asm          # Basic output demonstration
├── calculator.asm           # Arithmetic operations
├── loops.asm               # Control flow examples
└── README.md               # Example documentation
```

## Code Style Guidelines

### Naming Conventions

```cpp
// Classes and Types: PascalCase
class CPURegister;
enum class TokenType;

// Functions and Variables: snake_case
void execute_instruction();
uint32_t program_counter;

// Constants: UPPER_SNAKE_CASE
const int MAX_REGISTERS = 50;

// Member Variables: trailing underscore
class CPU {
private:
    uint32_t program_counter_;
    std::vector<Register> registers_;
};

// Namespaces: lowercase
namespace assembler {
    class Lexer;
}
```

### Documentation Standards

```cpp
/**
 * @brief Execute a single CPU instruction
 * 
 * Decodes and executes the instruction at the current program counter,
 * updating CPU state and advancing PC as appropriate.
 * 
 * @param instruction The opcode and operands to execute
 * @return true if execution should continue, false if HALT encountered
 * @throws CPUException for invalid instructions or runtime errors
 * 
 * @example
 * ```cpp
 * CPU cpu;
 * bool continue_execution = cpu.execute_instruction(0x01); // LOAD_IMM
 * ```
 */
bool execute_instruction(uint8_t instruction);
```

### Error Handling Strategy

```cpp
// Use exceptions for unrecoverable errors
if (register_id >= MAX_REGISTERS) {
    throw std::out_of_range("Invalid register: R" + std::to_string(register_id));
}

// Use return codes for expected failures
enum class AssemblerResult {
    SUCCESS,
    SYNTAX_ERROR,
    UNDEFINED_SYMBOL,
    INVALID_INSTRUCTION
};

// Always validate inputs
bool CPU::set_register(uint8_t reg, uint32_t value) {
    if (reg >= TOTAL_REGISTERS) {
        LOG_ERROR("Invalid register access: R" + std::to_string(reg));
        return false;
    }
    registers_[reg].set_value(value);
    return true;
}
```

## Testing Requirements

### Test Coverage Goals
- **Unit Tests**: >95% coverage for all public APIs
- **Integration Tests**: End-to-end workflow validation
- **Assembly Examples**: Working programs for every instruction
- **Regression Tests**: Automated validation of bug fixes

### Test Organization

```cpp
// Test Naming Convention
class ArithmeticInstructionTests : public TestCase {
public:
    void test_add_basic_operation();
    void test_add_overflow_handling();
    void test_add_flag_updates();
};

// Assertion Helpers
void assert_register_equals(CPU& cpu, uint8_t reg, uint32_t expected);
void assert_flag_set(CPU& cpu, CPUFlag flag);
void assert_memory_contains(CPU& cpu, uint32_t addr, uint8_t expected);

// Test Data Management
namespace TestPrograms {
    const std::vector<uint8_t> SIMPLE_ADD = {
        0x01, 0x00, 10,     // LOAD_IMM R0, 10
        0x01, 0x01, 20,     // LOAD_IMM R1, 20
        0x02, 0x00, 0x01,   // ADD R0, R1
        0xFF                // HALT
    };
}
```

## Build System

### Dependencies

```makefile
# Required Compiler
CXX = g++ (C++17 compatible)

# Core Libraries
fmt                    # String formatting
ImGui                  # Debug GUI framework
OpenGL/GLFW           # Graphics for debug interface

# Build Tools
make                  # Build system
git                   # Version control
```

### Build Targets

```bash
make                  # Default build (optimized)
make debug           # Debug build with symbols
make test            # Build and run test suite
make clean           # Remove build artifacts
make install         # System installation
make docs            # Generate documentation
```

### Platform Support

- **Linux**: Primary development platform (full support)
- **macOS**: Supported with Homebrew dependencies
- **Windows**: Supported via MSYS2/MinGW-w64

## Debugging and Profiling

### Debug GUI Features

```cpp
// Launch debug interface
./bin/virtcomp -H program.hex --gui --debug

// Available debug windows:
// - CPU State: Real-time register and flag display
// - Memory Viewer: Hex dump with navigation
// - Disassembly: Instruction display with symbols
// - Device Monitor: I/O activity tracking
// - Console Output: Program output capture
// - Control Panel: Execution control and settings
```

### Logging Framework

```cpp
// Logging levels and usage
LOG_DEBUG("Detailed execution information");
LOG_INFO("General program flow information");
LOG_WARNING("Potential issues or deprecated usage");
LOG_ERROR("Recoverable errors with context");
LOG_SUCCESS("Positive completion notifications");

// Log configuration
Logger::getInstance().set_log_level(LogLevel::DEBUG);
Logger::getInstance().enable_file_logging("debug.log");
Logger::getInstance().enable_colors(true);
```

### Performance Profiling

```cpp
// Built-in profiling support
ExecutionProfiler profiler;
profiler.start_profiling();

// Run program...

auto stats = profiler.get_statistics();
std::cout << "Instructions/sec: " << stats.instructions_per_second << std::endl;
std::cout << "Memory hotspots: " << stats.memory_hotspots.size() << std::endl;
```

## Integration Points

### External Libraries

```cpp
// fmt: String formatting and output
#include <fmt/format.h>
std::string message = fmt::format("Register R{}: 0x{:08X}", reg, value);

// ImGui: Debug interface
#include "imgui.h"
ImGui::Text("CPU State");
ImGui::InputInt("Register", &register_value);

// Standard Library: Core functionality
#include <vector>      // Dynamic arrays
#include <unordered_map> // Hash tables
#include <memory>      // Smart pointers
#include <filesystem>  // File operations
```

### Version Control Integration

```bash
# Development workflow
git checkout -b feature/new-instruction
# Make changes...
git add . && git commit -m "Add: NEW_INSTRUCTION opcode implementation"
git push origin feature/new-instruction
# Create pull request...

# Testing before merge
./scripts/run_full_test_suite.sh
make clean && make test
```

## Contributing Guidelines

### Development Process

1. **Fork and Clone**: Create personal fork and local development environment
2. **Feature Branch**: Create descriptive branch name (`feature/add-graphics-device`)
3. **Test-Driven Development**: Write tests before implementing features
4. **Code Review**: Submit pull request with comprehensive description
5. **Documentation**: Update relevant documentation with changes
6. **Integration**: Merge after passing all checks and reviews

### Quality Standards

- **Code Coverage**: Maintain >90% test coverage
- **Documentation**: Document all public APIs with examples
- **Performance**: Profile critical paths and optimize bottlenecks
- **Compatibility**: Ensure cross-platform functionality
- **Security**: Validate all inputs and handle edge cases

### Pull Request Checklist

- [ ] All tests pass (`make test`)
- [ ] Code follows style guidelines
- [ ] Documentation updated for API changes
- [ ] Examples provided for new features
- [ ] Performance impact assessed
- [ ] Breaking changes documented

## Future Development

### Planned Enhancements

#### Phase 3: Documentation and Examples (Current)
- **Comprehensive Tutorials**: Step-by-step assembly programming guides
- **Advanced Examples**: Complex programs demonstrating VM capabilities
- **API Documentation**: Complete reference with interactive examples
- **Video Tutorials**: Visual learning resources for complex topics

#### Phase 4: Advanced Features
- **Graphics Device**: Display controller with framebuffer support
- **Audio Device**: Sound generation and audio I/O capabilities
- **Network Device**: TCP/UDP communication support
- **Storage Device**: Virtual disk with filesystem support

#### Phase 5: Performance and Tooling
- **JIT Compilation**: Runtime optimization for performance-critical code
- **Reverse Debugging**: Time-travel debugging capabilities
- **Plugin Architecture**: External device and instruction plugins
- **Remote Debugging**: Network-based debugging protocol

### Architecture Evolution

The VirtComp architecture is designed for extensibility and long-term growth:

- **Modular Design**: New components integrate without core changes
- **Interface Stability**: Public APIs maintain backward compatibility
- **Performance Scalability**: Optimized for both educational and production use
- **Platform Portability**: Clean abstraction enables multi-platform support

---

## Getting Started

Ready to contribute to VirtComp? Start here:

1. **Read the [API Reference](API_REFERENCE.md)** for complete technical details
2. **Explore [Module Documentation](#module-documentation)** for specific components
3. **Set up your [Development Environment](#development-workflow)**
4. **Run the [Test Suite](#testing-requirements)** to validate your setup
5. **Check our [Contributing Guidelines](#contributing-guidelines)** before making changes

For questions, issues, or feature requests, please use the project's issue tracker or discussion forums.

**Happy coding! 🚀**
