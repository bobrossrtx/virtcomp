# VirtComp

**Virtual Computer Simulation**  
*A complete virtual computer environment with custom 32-bit architecture*

VirtComp provides a comprehensive virtual computer simulation featuring a custom CPU architecture, device-based I/O system, and visual debugger. Perfect for learning computer architecture, assembly programming, and system design.

## Quick Start

```bash
# Build the project
make

# Run a program
./bin/virtcomp tests/helloworld.hex

# Enable debug GUI
./bin/virtcomp tests/helloworld.hex --gui
```

## Features

- **Custom 32-bit CPU Architecture**: Complete instruction set with arithmetic, logic, memory, and I/O operations
- **Device-Based I/O**: Modular device system supporting console, file access, serial ports, and more
- **Visual Debugger**: ImGui-based interface for real-time debugging and system inspection
- **Hex Programming**: Human-readable hex format with comments and documentation support
- **Extensible Design**: Easy to add new devices and extend functionality

## Documentation

📚 **[Complete Documentation](docs/README.md)** - Start here for comprehensive guides

### Quick Links
- **[Usage Guide](docs/usage/README.md)** - Learn to write hex programs and use the system
- **[Codebase Documentation](docs/codebase/README.md)** - Technical details for developers
- **[Examples](tests/)** - Sample programs demonstrating various features

## Project Structure

- **`src/`**: Source code for the virtual computer
  - `vhardware/`: CPU, memory, and device implementations
  - `debug/`: Visual debugger interface
- **`docs/`**: Comprehensive documentation
- **`tests/`**: Example programs in hex format
- **`bin/`**: Compiled executables

## Building

Requirements: C++17 compiler, fmt library, OpenGL (for GUI mode)

```bash
make          # Build release version
make debug    # Build with debug symbols
make clean    # Clean build artifacts
```

## Command Line Usage

```bash
virtcomp [OPTIONS] [PROGRAM_FILE]

Options:
  --gui          Enable graphical debugger
  --interactive  Start in interactive mode
  --help         Show help message

Examples:
  virtcomp program.hex           # Run program
  virtcomp program.hex --gui     # Run with debugger
```

---

## Running the Test Suite

VirtComp includes a suite of unit tests to verify core functionality. To run all tests:

```bash
./bin/virtcomp -t
```

Add `-d` for debug output:

```bash
./bin/virtcomp -t -d
```

---

### Example Test Output

```
[25-04-30 | 22:54:53.302] [INFO]                   [RUN] | add.hex
[25-04-30 | 22:54:53.303] [INFO] --------------- Comment | Addition: R0 = 5, R1 = 7, R0 = R0 + R1, HALT
[25-04-30 | 22:54:53.303] [SUCCESS]               [PASS] | add.hex
...
[/] add.hex                     [/] dec.hex                     [X] invalid_opcode.hex          [/] out_of_bounds_reg.hex   
[/] sub.hex                     [/] shr.hex                     [/] push_pop.hex                [/] xor.hex                 
[/] mul.hex                     [/] inc.hex                     [/] and.hex                     [/] cmp_jz.hex              
[/] shl.hex                     [X] divzero.hex                 [/] not.hex                     [/] jmp.hex                 
[/] or.hex                      [/] div.hex                     
Tests passed: 16 / 18
```

- `[RUN]` — Test is running
- `[INFO] Comment` — Description from the `.hex` file
- `[PASS]` or `[FAIL]` — Test result
- `/` — Passed, `X` — Failed

---

## Writing Programs

Disclaimer: This is a work in progress and the assembly language may change.
Currently, the assembly language is a simple hex-encoded format. and a program has to be written in a specific way.
Support for an actual assembly language is planned for the future (and even support for the [Demi](https://demi-website.fly.dev/) Language).

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

### Future Instructions
- `0x12` (CALL) — Call subroutine
- `0x13` (RET) — Return from subroutine
- `0x14` (PUSH_ARG) — Push argument onto stack
- `0x15` (POP_ARG) — Pop argument from stack

---

*Happy hacking with VirtComp!*