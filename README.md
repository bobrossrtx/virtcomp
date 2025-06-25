# VirtComp

[![Build Status](https://github.com/bobrossrtx/virtcomp/actions/workflows/build.yml/badge.svg)](https://github.com/bobrossrtx/virtcomp/actions/workflows/build.yml)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

#### **Built Using:**
![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![OpenGL](https://img.shields.io/badge/OpenGL-%23FFFFFF.svg?style=for-the-badge&logo=opengl)

**Virtual Computer Simulation**
*A complete virtual computer environment with custom 32-bit architecture*

VirtComp provides a comprehensive virtual computer simulation featuring a custom CPU architecture, device-based I/O system, and visual debugger. Perfect for learning computer architecture, assembly programming, and system design.

---

## Quick Start

### Prerequisites
- C++17 compatible compiler
- `fmt` library
- OpenGL (for GUI mode)
- `make` utility

### Build and Run
```sh
# Clone the repository
git clone https://github.com/bobrossrtx/virtcomp.git
cd virtcomp

# Build the project
make build

# Run a program
./bin/virtcomp -p tests/helloworld.hex

# Compile a program
./bin/virtcomp -p tests/helloworld -o

# Enable debug GUI
./bin/virtcomp -p tests/helloworld.hex --gui
```

---

## Features

- **Custom 32-bit CPU Architecture:** Complete instruction set with arithmetic, logic, memory, and I/O operations
- **Device-Based I/O:** Modular device system supporting console, file access, serial ports, and more
- **Visual Debugger:** ImGui-based interface for real-time debugging and system inspection
- **Hex Programming:** Human-readable hex format with comments and documentation support
- **Extensible Design:** Easily add new devices and extend functionality

---

## Documentation

📚 **[Complete Documentation](docs/README.md)** – Start here for comprehensive guides

### Quick Links
- **[Usage Guide](docs/usage/README.md)** – Learn to write hex programs and use the system
- **[Codebase Documentation](docs/codebase/README.md)** – Technical details for developers
- **[Examples](tests/)** – Sample programs demonstrating various features

---

## Project Structure

- **`src/`**: Source code for the virtual computer
  - `vhardware/`: CPU, memory, and device implementations
  - `debug/`: Visual debugger interface
  - `test/`: Unit tests
- **`docs/`**: Comprehensive documentation
- **`tests/`**: Example programs in hex format
- **`bin/`**: Compiled executables

---

## Building

```sh
make          # Build release version
make debug    # Build with debug symbols
make clean    # Clean build artifacts
```

---

## Command Line Usage

```sh
./bin/virtcomp -h
```

```
virtcomp Usage: virtcomp [options]
  --help           -h      Shows help information
  --debug          -d      Enable debug mode
  --verbose        -v      Show informational messages (use --verbose=false to disable)
  --debug-file     -f      Debug file path
  --program        -p      Path to program file (hex bytes, space or newline separated)
  --test           -t      Run tests
  --gui            -g      Enable debug GUI
  --compile        -o      Compile program into a standalone executable (optionally specify output name)

Examples:
  virtcomp program.hex           # Run program
  virtcomp program.hex --gui     # Run with debugger
```

---

## Running the Test Suite

VirtComp includes a suite of unit tests to verify core functionality. To run all tests:

```sh
./bin/virtcomp -t
```

Add `-d` for debug output:

```sh
./bin/virtcomp -t -d
```

## Writing Programs

> **Note:** This is a work in progress. The assembly language may change. Support for a full assembler and [Demi](https://demi-website.fly.dev/) language is planned.

- Test programs are `.hex` files in the `tests/` directory.
- Each file can start with a comment line (beginning with `#`) describing the test.
- Instructions are written as space-separated hex bytes.

**Example (`add.hex`):**
```
# Addition: R0 = 5, R1 = 7, R0 = R0 + R1, HALT
01 00 05 01 01 07 02 00 01 FF
```

### Current Instructions
- `0x00` (NOP) — NOP (No operation)
- `0x01` (LOAD_IMM) — Load immediate value into register
- `0x02` (ADD) — Add register values
- `0x03` (SUB) — Subtract register values
- `0x04` (MUL) — Multiply register values
- `0x05` (DIV) — Divide register values
- `0x06` (SHL) — Shift left
- `0x07` (SHR) — Shift right
- `0x08` (AND) — Bitwise AND
- `0x09` (OR) — Bitwise OR
- `0x0A` (XOR) — Bitwise XOR
- `0x0B` (NOT) — Bitwise NOT
- `0x0C` (CMP) — Compare register values
- `0x0D` (JMP) — Jump to address
- `0x0E` (JZ) — Jump if zero
- `0x0F` (JNZ) — Jump if not zero
- `0x10` (PUSH) — Push value onto stack
- `0x11` (POP) — Pop value from stack

#### Future Instructions
- `0x12` (CALL) — Call subroutine
- `0x13` (RET) — Return from subroutine
- `0x14` (PUSH_ARG) — Push argument onto stack
- `0x15` (POP_ARG) — Pop argument from stack

---

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

*Happy hacking with VirtComp!*
