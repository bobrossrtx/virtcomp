# DemiEngine Assembly Examples

This directory contains working assembly language examples for the DemiEngine virtual machine. All examples use the complete instruction set and produce readable output.

## Quick Start
Run any example with: `./bin/demi-engine -A examples/<filename>.asm`

## Examples

### Basic Demonstrations
- **`hello_world.asm`** - Classic "Hello World!" program
  - Output: `Hello World!`
  - Demonstrates: Basic I/O with OUT instruction

- **`simple_digit.asm`** - Outputs a single digit
  - Output: `5`
  - Demonstrates: Number-to-ASCII conversion, basic arithmetic

### Arithmetic & Logic
- **`readable_calculator.asm`** - Arithmetic with readable output
  - Output: `5 + 3 = 8`
  - Demonstrates: Arithmetic operations, formatted output

- **`decimal_output.asm`** - Number-to-string conversion
  - Output: `123`
  - Demonstrates: Multi-digit number display, division/modulo simulation

- **`simple_addition.asm`** - Basic arithmetic operations
  - Demonstrates: ADD, SUB, register operations

### Control Flow
- **`counting_loop.asm`** - Loop demonstration
  - Output: `1 2 3 4 5`
  - Demonstrates: Loops, conditional jumps, counters

- **`simple_jump_test.asm`** - Jump instruction test
  - Output: `P`
  - Demonstrates: Jump instructions, program flow control

### Advanced Features
- **`core_instructions.asm`** - Comprehensive instruction showcase
  - Demonstrates: Full range of CPU instructions

## Instruction Set Coverage

These examples demonstrate:
- ✅ Arithmetic: ADD, SUB, MUL, DIV
- ✅ Logic: AND, OR, XOR, NOT
- ✅ Comparison: CMP with conditional jumps (JZ, JNZ, JG, JL, JGE, JLE)
- ✅ Memory: LOAD_IMM, MOV, STORE, LOAD
- ✅ I/O: OUT (console output)
- ✅ Control: JMP, HALT
- ✅ Bit operations: SHL, SHR

## Assembly Language Format
- Instructions are **UPPERCASE** (`LOAD_IMM`, `ADD`, `OUT`)
- Register names use format `R0`, `R1`, `R2`, etc.
- Comments start with `;`
- Labels end with `:`
- Immediate values are decimal numbers

## Tips
- Most arithmetic instructions work with registers only (no immediate operands)
- To output readable numbers, convert to ASCII by adding 48 (`'0'`)
- For multi-digit numbers, use division to extract individual digits
- Use OUT instruction with device ID 1 for console output
