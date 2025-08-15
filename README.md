# DemiEngine

[![Build Status](https://github.com/bobrossrtx/demi/actions/workflows/build.yml/badge.svg)](https://github.com/bobrossrtx/demi-engine/actions/workflows/build.yml)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

#### **Built Using:**
![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![OpenGL](https://img.shields.io/badge/OpenGL-%23FFFFFF.svg?style=for-the-badge&logo=opengl)

**Virtual Computer Simulation**
*A complete virtual computer environment with custom 32-bit architecture and 100% test coverage*

DemiEngine provides a comprehensive virtual computer simulation featuring a custom CPU architecture, device-based I/O system, and visual debugger. With 53 unit tests and 39 integration tests all passing, DemiEngine offers rock-solid reliability for learning computer architecture, assembly programming, and system design.

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
git clone https://github.com/bobrossrtx/demi-engine.git
cd demi-engine

# Build the project
make

# Run unit and integration tests  
make test

# Run a hex program
./bin/demi-engine -H tests/helloworld.hex

# Compile a program
./bin/demi-engine -H tests/helloworld -o

# Enable debug GUI
./bin/demi-engine -H tests/helloworld.hex --gui
```

---

## Features

- **Custom 32-bit CPU Architecture:** Complete instruction set with arithmetic, logic, memory, and I/O operations
- **Device-Based I/O:** Modular device system supporting console, file access, serial ports, and more
- **Visual Debugger:** ImGui-based interface for real-time debugging and system inspection
- **Comprehensive Testing:** 100% test coverage with 53 unit tests and 39 integration tests
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
./bin/demi-engine -h
```

```
demi-engine Usage: demi-engine [options]
  --help           -h      Shows help information
  --debug          -d      Enable debug mode
  --verbose        -v      Show informational messages (use --verbose=false to disable)
  --debug-file     -f      Debug file path
  --hex            -H      Path to hex file (hex bytes, space or newline separated)
  --test           -t      Run tests
  --gui            -g      Enable debug GUI
  --compile        -o      Compile program into a standalone executable (optionally specify output name)

Examples:
  demi-engine program.hex           # Run program
  demi-engine program.hex --gui     # Run with debugger
```

---

## Running the Test Suite

DemiEngine includes a suite of unit tests to verify core functionality. To run all tests:

```sh
./bin/demi-engine -t
```

Add `-d` for debug output:

```sh
./bin/demi-engine -t -d
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

- `0x00` (NOP) — No operation
- `0x01` (LOAD_IMM) — Load immediate value into register
- `0x02` (ADD) — Add register values
- `0x03` (SUB) — Subtract register values  
- `0x04` (MOV) — Move value between registers
- `0x05` (JMP) — Jump to address
- `0x06` (LOAD) — Load value from memory to register
- `0x07` (STORE) — Store value from register to memory
- `0x08` (PUSH) — Push register onto stack
- `0x09` (POP) — Pop value from stack to register
- `0x0A` (CMP) — Compare register values
- `0x0B` (JZ) — Jump if zero flag set
- `0x0C` (JNZ) — Jump if zero flag not set
- `0x0D` (JS) — Jump if sign flag set
- `0x0E` (JNS) — Jump if sign flag not set
- `0x0F` (JC) — Jump if carry flag set
- `0x10` (MUL) — Multiply register values
- `0x11` (DIV) — Divide register values
- `0x12` (INC) — Increment register
- `0x13` (DEC) — Decrement register
- `0x14` (AND) — Bitwise AND
- `0x15` (OR) — Bitwise OR
- `0x16` (XOR) — Bitwise XOR
- `0x17` (NOT) — Bitwise NOT
- `0x18` (SHL) — Shift left
- `0x19` (SHR) — Shift right
- `0x1A` (CALL) — Call subroutine
- `0x1B` (RET) — Return from subroutine
- `0x1C` (PUSH_ARG) — Push argument onto stack
- `0x1D` (POP_ARG) — Pop argument from stack
- `0x1E` (PUSH_FLAG) — Push flags onto stack
- `0x1F` (POP_FLAG) — Pop flags from stack
- `0x20` (LEA) — Load effective address
- `0x21` (SWAP) — Swap register and memory values
- `0x22` (JNC) — Jump if carry flag not set
- `0x30` (IN) — Input from port to register
- `0x31` (OUT) — Output from register to port
- `0x32-0x39` — Additional I/O operations
- `0x40` (DB) — Define byte
- `0xFF` (HALT) — Halt execution

#### Device I/O Instructions

- `0x30` (IN) — Input from port/device to register
- `0x31` (OUT) — Output from register to port/device
- `0x32` (INB) — Input byte from port
- `0x33` (OUTB) — Output byte to port
- `0x34` (INW) — Input word from port
- `0x35` (OUTW) — Output word to port
- `0x36` (INL) — Input long from port
- `0x37` (OUTL) — Output long to port
- `0x38` (INSTR) — Input instruction from port
- `0x39` (OUTSTR) — Output string to port

---

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

*Happy hacking with DemiEngine!*
