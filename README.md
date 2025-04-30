# VirtComp

**Virtual Computation**  
*A project for simulating virtual environments on a custom architecture*

---

## Overview

VirtComp simulates a virtual computer running its own custom architecture. The goal is to create a system that can execute instructions in a custom assembly language, providing a platform for experimentation and learning.

---

## Project Structure

- **`src/`**: Source code for the virtual machine.
- **`bin/`**: Compiled binary files.
- **`tests/`**: Test programs for the virtual machine (in hex format).

---

## Building

To build the project, simply run:

```bash
make
```

This will compile the source code and place the binary in the `bin/` directory.

---

## Running Programs

To run a program file (hex format):

```bash
./bin/virtcomp -p <program_filename>
```

**Example:**

```bash
./bin/virtcomp -p tests/add.hex
./bin/virtcomp -p tests/sub.hex
./bin/virtcomp -p tests/mul.hex
```

> **Note:**  
> Program files are located in the `tests/` directory and contain hex-encoded instructions for the virtual machine.

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

---

## License

MIT License

---

*Happy hacking with VirtComp!*