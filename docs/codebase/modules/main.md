# Main Application Entry Point

**File:** `src/main.cpp`
**Purpose:** Application entry point, command line argument parsing, and orchestration of VirtComp components

## Overview

The main.cpp file serves as the central orchestrator for the VirtComp virtual machine. It handles command line argument parsing, initializes subsystems, and manages the execution flow between different modes (assembly, hex execution, testing, GUI debugging).

## Key Components

### ArgParser Class
- **Purpose:** Type-safe command line argument parsing
- **Features:**
  - Boolean flags with callbacks
  - Value arguments with validation
  - Action arguments for immediate execution
  - Comprehensive help generation

### Main Functions

#### `initialize_devices()`
- Initializes the device factory with standard I/O devices
- Sets up console output device (ID: 1)
- Configures device manager for hardware emulation

#### `load_program_file(const std::string& path, std::vector<uint8_t>& out)`
- Loads hex bytecode files from disk
- Supports space and newline-separated hex values
- Validates hex format and provides error reporting
- Returns vector of assembled bytecode

#### `generate_executable_name(const std::string& program_file)`
- Generates appropriate executable names for compilation mode
- Strips file extensions and adds platform-appropriate suffixes
- Handles cross-platform file naming conventions

#### `run_tests()`
- Orchestrates unit test execution
- Integrates with the test framework
- Provides test result reporting

### Command Line Options

| Option | Short | Description |
|--------|--------|-------------|
| `--help` | `-h` | Display help information |
| `--debug` | `-d` | Enable debug mode with verbose logging |
| `--verbose` | `-v` | Show informational messages |
| `--extended-registers` | `-er` | Display all 50 registers in output |
| `--debug-file` | `-f` | Specify debug log file path |
| `--hex` | `-H` | Path to hex file for execution |
| `--test` | `-t` | Run unit test suite |
| `--gui` | `-g` | Enable graphical debugger |
| `--assembly` | `-A` | Assembly mode for .asm files |
| `--compile` | `-o` | Compile to standalone executable |

### Execution Modes

#### 1. Hex File Execution Mode
```bash
./bin/virtcomp -H program.hex
```
- Loads pre-assembled hex bytecode
- Executes directly on virtual CPU
- Primary runtime mode

#### 2. Assembly Mode
```bash
./bin/virtcomp -A program.asm
```
- Invokes lexer → parser → assembler pipeline
- Compiles assembly source to bytecode
- Executes assembled program

#### 3. Test Mode
```bash
./bin/virtcomp -t
```
- Runs comprehensive unit test suite
- Tests all CPU instructions and subsystems
- Validates register architecture

#### 4. GUI Debug Mode
```bash
./bin/virtcomp -H program.hex --gui
```
- Launches graphical debugger interface
- Provides real-time CPU state visualization
- Interactive debugging capabilities

#### 5. Compile Mode
```bash
./bin/virtcomp -H program.hex -o [output_name]
```
- Generates standalone executable
- Bundles VM runtime with program
- Cross-platform binary generation

### Error Handling

The main application implements comprehensive error handling:

- **Argument Validation:** Conflicting flags detection
- **File I/O:** Graceful handling of missing/invalid files
- **Mode Conflicts:** Assembly mode vs hex file validation
- **Resource Management:** Proper cleanup on exit

### Integration Points

- **CPU System:** Direct integration with vhardware/cpu.hpp
- **Assembly Pipeline:** Full assembler toolchain integration
- **Device Management:** Hardware abstraction layer
- **Logging System:** Centralized debug and error logging
- **GUI Framework:** Optional graphical interface

### Dependencies

```cpp
#include "config.hpp"                    // Global configuration
#include "vhardware/cpu.hpp"             // CPU emulation core
#include "vhardware/device_factory.hpp"  // Hardware abstraction
#include "debug/logger.hpp"              // Logging framework
#include "debug/gui.hpp"                 // Graphical debugger
#include "test/test.hpp"                 // Unit testing
#include "assembler/virtcomp_assembler.hpp" // Assembly pipeline
```

### Build Integration

The main.cpp compiles to the primary `bin/virtcomp` executable and serves as the unified interface to all VirtComp functionality.

### Usage Examples

```bash
# Basic execution
./bin/virtcomp -H examples/hello_world.hex

# Assembly compilation and execution
./bin/virtcomp -A examples/calculator.asm

# Debug with GUI
./bin/virtcomp -H tests/complex_program.hex --gui --debug

# Run test suite
./bin/virtcomp -t

# Compile standalone executable
./bin/virtcomp -H program.hex -o my_program
```

This file represents the primary user interface to VirtComp and orchestrates all major subsystems while maintaining clean separation of concerns.
