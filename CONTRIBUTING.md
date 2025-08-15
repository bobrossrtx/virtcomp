# Contributing to DemiEngine

Thank you for your interest in contributing to DemiEngine! This guide explains how to contribute to the project effectively.

## Table of Contents

- [Ways to Contribute](#ways-to-contribute)
- [Development Setup](#development-setup)
- [Code Style Guidelines](#code-style-guidelines)
- [Documentation Standards](#documentation-standards)
- [Testing Requirements](#testing-requirements)
- [Submission Process](#submission-process)
- [Areas for Contribution](#areas-for-contribution)

## Ways to Contribute

### Code Contributions
- **Bug fixes**: Fix issues in CPU emulation, device drivers, or GUI
- **New features**: Add instructions, devices, or debugging capabilities
- **Performance improvements**: Optimize execution speed or memory usage
- **Platform support**: Add support for new operating systems

### Documentation
- **API documentation**: Improve /** comments **/ and technical docs
- **User guides**: Create tutorials and programming examples
- **Code examples**: Add well-commented example programs
- **Troubleshooting**: Document solutions to common problems

### Testing
- **Test programs**: Create comprehensive hex programs for testing
- **Edge cases**: Test boundary conditions and error scenarios
- **Platform testing**: Verify functionality on different systems
- **Performance benchmarks**: Create and run performance tests

## Development Setup

### Prerequisites

**Required Tools**:
- C++17 compatible compiler (GCC 7+, Clang 6+)
- Make build system
- Git for version control

**Required Libraries**:
- fmt library for string formatting
- OpenGL and GLFW for GUI (optional, for debug mode)

**Installation on Ubuntu/Debian**:
```bash
sudo apt update
sudo apt install g++ make git libfmt-dev libgl1-mesa-dev libglfw3-dev
```

**Installation on other platforms**:
- **macOS**: Use Homebrew to install dependencies
- **Windows**: Use vcpkg or manual installation
- **Other Linux**: Use your distribution's package manager

### Building from Source

```bash
# Clone the repository
git clone <repository-url>
cd demi-engine

# Build the project
make clean
make

# Run tests
make test  # (if test target exists)

# Build with debug symbols
make debug
```

### Development Workflow

1. **Fork the repository** and create a feature branch
2. **Set up your development environment** with required tools
3. **Make your changes** following the style guidelines
4. **Test thoroughly** with existing and new test programs
5. **Update documentation** as needed
6. **Submit a pull request** with clear description

## Code Style Guidelines

### General Principles

1. **Consistency**: Follow existing code patterns and naming conventions
2. **Clarity**: Write self-documenting code with clear variable names
3. **Simplicity**: Prefer simple, straightforward solutions
4. **Performance**: Consider performance implications of changes

### Naming Conventions

```cpp
// Classes: PascalCase
class DeviceManager {
    // ...
};

// Functions and variables: snake_case
void execute_instruction();
uint32_t memory_address;

// Constants: UPPER_CASE
constexpr uint8_t MAX_DEVICES = 255;

// Private members: trailing underscore
class CPU {
private:
    uint32_t program_counter_;
    std::vector<uint8_t> memory_;
};

// Enums: PascalCase with descriptive prefix
enum class InstructionType {
    ARITHMETIC,
    MEMORY_ACCESS,
    CONTROL_FLOW
};
```

### Code Formatting

```cpp
// Indentation: 4 spaces (no tabs)
if (condition) {
    do_something();
}

// Braces: Opening brace on same line
void function() {
    // implementation
}

// Line length: 80-100 characters maximum
void long_function_name(int parameter1, int parameter2,
                       int parameter3) {
    // implementation
}

// Include order: system headers, then project headers
#include <iostream>
#include <vector>

#include "cpu.hpp"
#include "device.hpp"
```

### Documentation Comments

Use /** */ style comments for API documentation:

```cpp
/**
 * @brief Execute a single CPU instruction
 *
 * Fetches the instruction at the current program counter,
 * decodes it, and executes the corresponding operation.
 * Updates CPU state including registers and flags.
 *
 * @throws CPUError if instruction is invalid or causes error
 * @throws std::out_of_range if memory access is invalid
 */
void step();
```

### Error Handling

```cpp
// Use exceptions for unrecoverable errors
if (address >= memory_size_) {
    throw std::out_of_range("Memory address out of bounds");
}

// Use return codes for expected failures
bool load_program(const std::string& filename) {
    if (!file_exists(filename)) {
        return false;  // Expected failure
    }
    // ...
    return true;
}

// Always validate input parameters
void set_register(Register reg, uint32_t value) {
    if (reg < Register::R0 || reg > Register::R7) {
        throw std::invalid_argument("Invalid register");
    }
    registers_[static_cast<int>(reg)] = value;
}
```

## Documentation Standards

### Code Documentation

- **Public APIs**: Must have /** */ documentation comments
- **Complex algorithms**: Include inline comments explaining logic
- **Magic numbers**: Define as named constants with explanations
- **File headers**: Include purpose, author, and license information

### User Documentation

- **Examples**: Provide complete, working examples
- **Step-by-step guides**: Break complex tasks into clear steps
- **Cross-references**: Link related sections and concepts
- **Up-to-date**: Ensure documentation matches current implementation

### Example Documentation Format

```cpp
/**
 * @file cpu.hpp
 * @brief CPU emulation for DemiEngine virtual computer
 * @author Your Name
 * @date 2025-06-09
 *
 * This file contains the CPU class that emulates a 32-bit processor
 * with custom instruction set architecture. The CPU supports arithmetic,
 * logic, memory, and I/O operations through a device-based system.
 */

/**
 * @brief Virtual CPU implementation
 *
 * The CPU class emulates a complete 32-bit processor including:
 * - 8 general-purpose registers (R0-R7)
 * - Program counter, stack pointer, frame pointer
 * - Status flags (zero, sign, carry, overflow)
 * - Memory management with configurable size
 * - Device I/O through port-mapped interface
 *
 * The CPU executes instructions in hex format and provides
 * comprehensive debugging and introspection capabilities.
 *
 * @example
 * ```cpp
 * CPU cpu(64 * 1024);  // 64KB memory
 * cpu.load_hex_program("program.hex");
 * cpu.run();
 * ```
 */
class CPU {
    // ...
};
```

## Testing Requirements

### Test Categories

1. **Unit Tests**: Test individual functions and classes
2. **Integration Tests**: Test component interactions
3. **System Tests**: Test complete program execution
4. **Performance Tests**: Measure execution speed and memory usage

### Test Program Format

Create test programs in hex format with comprehensive comments:

```hex
# ==========================================
# Test: Arithmetic Operations
# Purpose: Verify ADD, SUB, MUL, DIV instructions
# Expected: R0=10, R1=5, R2=15, R3=5, R4=50, R5=2
# ==========================================

# Test addition
01 00 0A 00 00 00    # LOAD_IMM R0, 10
01 01 05 00 00 00    # LOAD_IMM R1, 5
04 02 00            # MOV R2, R0
02 02 01            # ADD R2, R1      ; R2 = 10 + 5 = 15

# Test subtraction
04 03 00            # MOV R3, R0
03 03 01            # SUB R3, R1      ; R3 = 10 - 5 = 5

# Test multiplication
04 04 00            # MOV R4, R0
10 04 01            # MUL R4, R1      ; R4 = 10 * 5 = 50

# Test division
04 05 00            # MOV R5, R0
11 05 01            # DIV R5, R1      ; R5 = 10 / 5 = 2

FF                  # HALT

# Validation can be done through debug GUI or additional output code
```

### Testing Guidelines

- **Test edge cases**: Zero values, maximum values, boundary conditions
- **Test error conditions**: Invalid inputs, memory violations
- **Test device interactions**: I/O operations, device states
- **Document expected results**: Clear pass/fail criteria

## Submission Process

### Before Submitting

1. **Code compiles without warnings** on supported platforms
2. **All tests pass** including existing and new tests
3. **Documentation is updated** for new features or changes
4. **Code follows style guidelines** consistently
5. **Commit messages are clear** and descriptive

### Pull Request Guidelines

**Title Format**: `[Component] Brief description of change`

Examples:
- `[CPU] Add support for floating-point arithmetic`
- `[Device] Implement network device for TCP communication`
- `[GUI] Add memory search functionality`
- `[Docs] Update instruction set reference`

**Description Should Include**:
- **What**: Summary of changes made
- **Why**: Motivation and use case
- **How**: Brief explanation of implementation approach
- **Testing**: Description of tests performed
- **Breaking changes**: Any backwards compatibility issues

**Example Pull Request Description**:
```
[CPU] Add floating-point arithmetic instructions

What:
- Added FADD, FSUB, FMUL, FDIV instructions for 32-bit float operations
- Extended CPU class with floating-point register set
- Updated instruction decoder to handle new opcodes

Why:
- Enable scientific computing applications on DemiEngine
- Support graphics and simulation programs requiring floating-point

How:
- Added 8 floating-point registers (F0-F7)
- Implemented IEEE 754 single-precision arithmetic
- Used existing instruction format with new opcodes

Testing:
- Created comprehensive test suite in tests/float_ops.hex
- Verified precision and rounding behavior
- Tested edge cases (infinity, NaN, denormal numbers)

Breaking Changes:
- None - new instructions use previously unused opcodes
```

### Review Process

1. **Automated checks**: Code style, compilation, basic tests
2. **Peer review**: Code quality, design, documentation
3. **Testing**: Comprehensive testing on multiple platforms
4. **Integration**: Merge after approval and final testing

## Areas for Contribution

### High Priority

1. **Assembly Language Compiler**
   - Lexer and parser for assembly syntax
   - Symbol table management for labels
   - Code generation to hex format
   - Error reporting and debugging

2. **Advanced Debugging Features**
   - Breakpoints and watchpoints
   - Call stack visualization
   - Variable inspection
   - Performance profiling

3. **Additional Devices**
   - Graphics/display device
   - Network device (TCP/UDP)
   - Audio device
   - Storage devices (disk, flash)

### Medium Priority

1. **Performance Optimization**
   - Instruction caching
   - JIT compilation exploration
   - Memory access optimization
   - GUI rendering performance

2. **Enhanced Testing**
   - Automated test framework
   - Continuous integration setup
   - Performance benchmarks
   - Cross-platform testing

3. **Developer Tools**
   - Profiler for program analysis
   - Memory analyzer
   - Instruction trace viewer
   - Device monitor

### Documentation and Examples

1. **Educational Content**
   - Computer architecture tutorials
   - Assembly programming course
   - Algorithm implementations
   - System programming examples

2. **Reference Materials**
   - Comprehensive instruction reference
   - Device programming guide
   - Performance optimization tips
   - Porting guide for new platforms

## Getting Help

### Communication Channels

- **Issues**: Report bugs and request features via GitHub issues
- **Discussions**: General questions and design discussions
- **Documentation**: Check existing docs before asking questions

### Development Questions

When asking for help:
1. **Search existing issues** and documentation first
2. **Provide context**: What you're trying to achieve
3. **Include details**: Platform, compiler, error messages
4. **Share code**: Minimal example that reproduces the issue

### Learning Resources

- **Source code**: Well-documented codebase serves as reference
- **Example programs**: Study existing test programs
- **Documentation**: Comprehensive guides and API reference
- **Debug GUI**: Visual tool for understanding execution

---

Thank you for contributing to DemiEngine! Your contributions help make this educational tool better for everyone learning computer architecture and systems programming.
