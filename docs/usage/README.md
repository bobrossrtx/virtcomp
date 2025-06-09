# VirtComp Usage Documentation

This section provides user guides for programming the VirtComp virtual computer. Learn how to write programs, use the instruction set, and interact with devices.

## Table of Contents

- [Getting Started](#getting-started)
- [Hex Programming](#hex-programming)
  - [Program Format](#program-format)
  - [Writing Your First Program](#writing-your-first-program)
  - [Comments and Documentation](#comments-and-documentation)
- [Instruction Set Reference](#instruction-set-reference)
- [Device Programming](#device-programming)
- [Debugging and Testing](#debugging-and-testing)
- [Advanced Topics](#advanced-topics)
- [Assembly Language](#assembly-language) (Future)

## Getting Started

### Installation and Setup

1. **Build VirtComp**:
   ```bash
   cd virtcomp
   make
   ```

2. **Test the installation**:
   ```bash
   ./bin/virtcomp tests/helloworld.hex
   ```

3. **Enable debug mode**:
   ```bash
   ./bin/virtcomp tests/helloworld.hex --gui
   ```

### Command-Line Options

```bash
virtcomp [OPTIONS] [PROGRAM_FILE]

Options:
  --gui          Enable graphical debugger
  --interactive  Start in interactive mode
  --help         Show help message
  --version      Show version information

Examples:
  virtcomp program.hex           # Run program
  virtcomp program.hex --gui     # Run with debugger
  virtcomp --interactive         # Interactive mode
```

## Hex Programming

VirtComp programs are written in hexadecimal format with support for comments and labels. This section teaches you how to write effective hex programs.

### Program Format

Hex programs consist of:
- **Hexadecimal bytes**: The actual machine code
- **Comments**: Documentation starting with `#`
- **Whitespace**: Spaces and newlines for formatting

#### Basic Structure

```hex
# Program: Hello World
# Author: Your Name
# Description: Prints "Hello, World!" to console

# Load string address into R0
01 00 10 00 00 00    # LOAD_IMM R0, 0x10

# Print string to console
1F 00 01            # OUTSTR port=1, R0

# Halt program
FF                  # HALT

# Data section (at address 0x10)
48 65 6C 6C 6F 2C 20 57 6F 72 6C 64 21 00  # "Hello, World!\0"
```

### Writing Your First Program

Let's create a simple program that adds two numbers:

```hex
# Program: Simple Addition
# Adds 5 + 3 and stores result in R2

# Load first number (5) into R0
01 00 05 00 00 00    # LOAD_IMM R0, 5

# Load second number (3) into R1  
01 01 03 00 00 00    # LOAD_IMM R1, 3

# Add R0 and R1, store in R2
04 02 00            # ADD R2, R0

# Add R1 to R2 (R2 = R0 + R1)
04 02 01            # ADD R2, R1

# Halt
FF                  # HALT
```

### Program Layout Guidelines

1. **Header Comments**: Always include program description, author, and purpose
2. **Section Organization**: Separate code and data clearly
3. **Instruction Comments**: Document complex operations
4. **Data Alignment**: Place data at known addresses for easy access

#### Recommended Memory Layout

```hex
# Code section starts at 0x0000
01 00 20 00 00 00    # LOAD_IMM R0, 0x20  (data address)
...
FF                  # HALT

# Data section starts at 0x20
48 65 6C 6C 6F 00    # "Hello\0"
```

### Comments and Documentation

Good commenting makes programs maintainable and educational:

```hex
# =================================
# Program: Stack Example
# Demonstrates push/pop operations
# =================================

# Initialize stack with values
01 00 0A 00 00 00    # LOAD_IMM R0, 10    ; First value
0A 00               # PUSH R0             ; Push to stack

01 00 14 00 00 00    # LOAD_IMM R0, 20    ; Second value  
0A 00               # PUSH R0             ; Push to stack

# Pop values back
0B 01               # POP R1              ; R1 = 20
0B 02               # POP R2              ; R2 = 10

FF                  # HALT
```

#### Comment Styles

- `#` for line comments
- `;` for inline comments (after instruction)
- `# ===` for section dividers
- Indentation for code structure

## Instruction Set Reference

This section provides detailed information about each instruction in the VirtComp architecture.

### Instruction Format

Instructions use variable-length encoding:

```
Single Byte:    [OPCODE]
Two Bytes:      [OPCODE] [REG/IMM]  
Multi-Byte:     [OPCODE] [REG] [32-BIT-IMM]
```

### Register Encoding

Registers are encoded as single bytes:
- `0x00` = R0, `0x01` = R1, ..., `0x07` = R7
- Special registers accessible through specific instructions

### Basic Operations

#### NOP (0x00)
**Format**: `00`
**Purpose**: No operation, consumes one CPU cycle
**Example**: 
```hex
00    # NOP - do nothing
```

#### HALT (0xFF)
**Format**: `FF`
**Purpose**: Stop program execution
**Example**:
```hex
FF    # HALT - end program
```

#### LOAD_IMM (0x01)
**Format**: `01 <reg> <32-bit-value>`
**Purpose**: Load immediate 32-bit value into register
**Example**:
```hex
01 00 2A 00 00 00    # LOAD_IMM R0, 42
01 02 FF FF FF FF    # LOAD_IMM R2, 0xFFFFFFFF
```

#### MOV (0x06)
**Format**: `06 <dst_reg> <src_reg>`
**Purpose**: Copy value from source to destination register
**Example**:
```hex
06 01 00    # MOV R1, R0  (R1 = R0)
```

### Arithmetic Operations

#### ADD (0x04)
**Format**: `04 <dst_reg> <src_reg>`
**Purpose**: Add src_reg to dst_reg, store in dst_reg
**Flags**: Updates zero and sign flags
**Example**:
```hex
04 00 01    # ADD R0, R1  (R0 = R0 + R1)
```

#### SUB (0x05)
**Format**: `05 <dst_reg> <src_reg>`
**Purpose**: Subtract src_reg from dst_reg, store in dst_reg
**Flags**: Updates zero and sign flags
**Example**:
```hex
05 02 03    # SUB R2, R3  (R2 = R2 - R3)
```

#### MUL (0x11)
**Format**: `11 <dst_reg> <src_reg>`
**Purpose**: Multiply dst_reg by src_reg, store in dst_reg
**Example**:
```hex
11 00 01    # MUL R0, R1  (R0 = R0 * R1)
```

#### DIV (0x12)
**Format**: `12 <dst_reg> <src_reg>`
**Purpose**: Divide dst_reg by src_reg, store in dst_reg
**Note**: Division by zero halts program with error
**Example**:
```hex
12 05 06    # DIV R5, R6  (R5 = R5 / R6)
```

#### INC (0x13)
**Format**: `13 <reg>`
**Purpose**: Increment register by 1
**Example**:
```hex
13 00    # INC R0  (R0 = R0 + 1)
```

#### DEC (0x14)
**Format**: `14 <reg>`
**Purpose**: Decrement register by 1
**Example**:
```hex
14 07    # DEC R7  (R7 = R7 - 1)
```

### Memory Operations

#### LOAD (0x02)
**Format**: `02 <dst_reg> <addr_reg>`
**Purpose**: Load 32-bit value from memory address in addr_reg
**Example**:
```hex
02 01 00    # LOAD R1, [R0]  (R1 = memory[R0])
```

#### STORE (0x03)
**Format**: `03 <addr_reg> <src_reg>`
**Purpose**: Store 32-bit value from src_reg to memory address in addr_reg
**Example**:
```hex
03 02 01    # STORE [R2], R1  (memory[R2] = R1)
```

### Control Flow

#### JMP (0x07)
**Format**: `07 <32-bit-address>`
**Purpose**: Unconditional jump to address
**Example**:
```hex
07 10 00 00 00    # JMP 0x10  (jump to address 16)
```

#### CMP (0x0E)
**Format**: `0E <reg1> <reg2>`
**Purpose**: Compare two registers, set flags
**Flags**: Sets zero flag if equal, sign flag if reg1 < reg2
**Example**:
```hex
0E 00 01    # CMP R0, R1  (compare R0 with R1)
```

#### JZ (0x0F)
**Format**: `0F <32-bit-address>`
**Purpose**: Jump if zero flag is set (after CMP)
**Example**:
```hex
0E 00 01          # CMP R0, R1
0F 20 00 00 00    # JZ 0x20  (jump if R0 == R1)
```

#### JNZ (0x10)
**Format**: `10 <32-bit-address>`
**Purpose**: Jump if zero flag is clear
**Example**:
```hex
0E 02 03          # CMP R2, R3
10 30 00 00 00    # JNZ 0x30  (jump if R2 != R3)
```

### Stack Operations

The stack grows downward from high memory. The Stack Pointer (SP) always points to the top of the stack.

#### PUSH (0x0A)
**Format**: `0A <reg>`
**Purpose**: Push register value onto stack
**Effect**: SP decreases by 4, memory[SP] = reg
**Example**:
```hex
0A 00    # PUSH R0  (push R0 onto stack)
```

#### POP (0x0B)
**Format**: `0B <reg>`
**Purpose**: Pop value from stack into register
**Effect**: reg = memory[SP], SP increases by 4
**Example**:
```hex
0B 01    # POP R1  (pop top of stack into R1)
```

#### CALL (0x0C)
**Format**: `0C <32-bit-address>`
**Purpose**: Call subroutine (push return address, jump)
**Effect**: Push PC+5 onto stack, jump to address
**Example**:
```hex
0C 50 00 00 00    # CALL 0x50  (call function at 0x50)
```

#### RET (0x0D)
**Format**: `0D`
**Purpose**: Return from subroutine
**Effect**: Pop return address from stack, jump to it
**Example**:
```hex
0D    # RET  (return to caller)
```

### I/O Operations

VirtComp uses port-based I/O to communicate with devices.

#### IN (0x18)
**Format**: `18 <dst_reg> <port>`
**Purpose**: Read byte from device port
**Example**:
```hex
18 00 01    # IN R0, 1  (read byte from port 1 into R0)
```

#### OUT (0x19)
**Format**: `19 <port> <src_reg>`
**Purpose**: Write byte to device port
**Example**:
```hex
19 01 00    # OUT 1, R0  (write R0 to port 1)
```

#### INW (0x1A) / OUTW (0x1B)
**Purpose**: 16-bit word I/O operations
**Example**:
```hex
1A 00 02    # INW R0, 2   (read word from port 2)
1B 02 01    # OUTW 2, R1  (write word to port 2)
```

#### INL (0x1C) / OUTL (0x1D)
**Purpose**: 32-bit dword I/O operations
**Example**:
```hex
1C 03 04    # INL R3, 4   (read dword from port 4)
1D 04 03    # OUTL 4, R3  (write dword to port 4)
```

#### INSTR (0x1E) / OUTSTR (0x1F)
**Purpose**: String I/O operations
**Example**:
```hex
1E 00 01    # INSTR R0, 1   (read string from port 1, address in R0)
1F 01 02    # OUTSTR 1, R2  (write string to port 1, address in R2)
```

### Bitwise Operations

#### AND (0x15)
**Format**: `15 <dst_reg> <src_reg>`
**Purpose**: Bitwise AND operation
**Example**:
```hex
15 00 01    # AND R0, R1  (R0 = R0 & R1)
```

#### OR (0x16)
**Format**: `16 <dst_reg> <src_reg>`
**Purpose**: Bitwise OR operation
**Example**:
```hex
16 02 03    # OR R2, R3  (R2 = R2 | R3)
```

#### XOR (0x17)
**Format**: `17 <dst_reg> <src_reg>`
**Purpose**: Bitwise XOR operation
**Example**:
```hex
17 04 05    # XOR R4, R5  (R4 = R4 ^ R5)
```

#### NOT (0x20)
**Format**: `20 <reg>`
**Purpose**: Bitwise NOT operation
**Example**:
```hex
20 00    # NOT R0  (R0 = ~R0)
```

#### SHL (0x21) / SHR (0x22)
**Format**: `21 <reg> <shift_amount>`
**Purpose**: Shift left/right by immediate amount
**Example**:
```hex
21 00 02    # SHL R0, 2  (R0 = R0 << 2)
22 01 03    # SHR R1, 3  (R1 = R1 >> 3)
```

## Device Programming

VirtComp's modular device system allows programs to interact with various virtual and real hardware components through port-mapped I/O.

### Available Devices

#### Console Device (Port 1)
**Purpose**: Text input/output for user interaction

**Operations**:
- `OUT 1, reg`: Write character to console
- `IN reg, 1`: Read character from console
- `OUTSTR 1, reg`: Write null-terminated string (address in reg)
- `INSTR reg, 1`: Read line into buffer (address in reg)

**Example - Hello World**:
```hex
# Hello World using console device

# Load string address
01 00 20 00 00 00    # LOAD_IMM R0, 0x20

# Output string to console
1F 01 00            # OUTSTR port=1, R0

# Halt
FF                  # HALT

# String data at address 0x20
48 65 6C 6C 6F 2C 20 57 6F 72 6C 64 21 0A 00
# "Hello, World!\n\0"
```

**Example - User Input**:
```hex
# Read user input and echo it back

# Allocate buffer at address 0x50
01 00 50 00 00 00    # LOAD_IMM R0, 0x50

# Read string from user
1E 00 01            # INSTR R0, 1

# Echo it back
1F 01 00            # OUTSTR 1, R0

FF                  # HALT
```

#### File Device (Port 2)
**Purpose**: File system access

**Protocol**:
1. Write filename address to port
2. Write operation code (0=read, 1=write)
3. Write data address
4. Read result status

**Example - Read File**:
```hex
# Read contents of "data.txt"

# Load filename address
01 00 40 00 00 00    # LOAD_IMM R0, 0x40
19 02 00            # OUT 2, R0  (send filename)

# Set operation to read (0)
01 01 00 00 00 00    # LOAD_IMM R1, 0
19 02 01            # OUT 2, R1

# Set buffer address
01 02 60 00 00 00    # LOAD_IMM R2, 0x60
19 02 02            # OUT 2, R2

# Read status
18 03 02            # IN R3, 2   (0=success, 1=error)

FF                  # HALT

# Data section
# Address 0x40: filename
64 61 74 61 2E 74 78 74 00  # "data.txt\0"

# Address 0x60: buffer (allocated space for file contents)
```

#### Counter Device (Port 3)
**Purpose**: Simple counter for testing and timing

**Operations**:
- `IN reg, 3`: Read current counter value
- `OUT 3, reg`: Set counter value
- Counter auto-increments each CPU cycle

**Example - Timing Loop**:
```hex
# Measure loop execution time

# Read initial counter
18 00 03            # IN R0, 3

# Perform some work (loop 1000 times)
01 01 E8 03 00 00    # LOAD_IMM R1, 1000

# Loop start (address 0x0A)
14 01               # DEC R1
0E 01 00            # CMP R1, R0  (compare with 0)
10 0A 00 00 00      # JNZ 0x0A    (loop if not zero)

# Read final counter
18 02 03            # IN R2, 3

# Calculate elapsed time (R2 - R0)
05 02 00            # SUB R2, R0  (R2 = final - initial)

FF                  # HALT
```

### Device Communication Patterns

#### Polling Pattern
```hex
# Poll for device ready status
# Loop:
18 00 05            # IN R0, 5      (read status)
0E 00 01            # CMP R0, R1    (compare with ready value)
0F 10 00 00 00      # JZ 0x10       (jump if ready)
07 03 00 00 00      # JMP 0x03      (loop back)
# Ready:
# ... proceed with operation
```

#### String Processing
```hex
# Process null-terminated strings
01 00 30 00 00 00    # LOAD_IMM R0, 0x30  (string address)

# Loop through string
02 01 00            # LOAD R1, [R0]     (load character)
0E 01 02            # CMP R1, R2        (compare with 0)
0F 20 00 00 00      # JZ 0x20           (end if null)

# Process character in R1
# ... processing code ...

# Next character
13 00               # INC R0            (next address)
07 06 00 00 00      # JMP 0x06          (loop back)

# End
FF                  # HALT
```

## Debugging and Testing

### Using the Debug GUI

The debug GUI provides powerful tools for program development and troubleshooting.

**Enable GUI**:
```bash
./bin/virtcomp your_program.hex --gui
```

**GUI Features**:
- **CPU State**: View all registers, PC, SP, flags in real-time
- **Memory View**: Hexadecimal dump with address navigation
- **Device Monitor**: Live status of all connected devices
- **Step Execution**: Execute one instruction at a time
- **Breakpoints**: Pause execution at specific addresses (future)

### Common Programming Patterns

#### Function Calls
```hex
# Main function
01 00 0A 00 00 00    # LOAD_IMM R0, 10
01 01 05 00 00 00    # LOAD_IMM R1, 5
0C 20 00 00 00      # CALL add_function  (0x20)
# Result now in R0
FF                  # HALT

# Add function at 0x20
04 00 01            # ADD R0, R1    (R0 = R0 + R1)
0D                  # RET
```

#### Array Processing
```hex
# Process array of 5 numbers
01 00 50 00 00 00    # LOAD_IMM R0, 0x50  (array address)
01 01 05 00 00 00    # LOAD_IMM R1, 5     (array length)
01 02 00 00 00 00    # LOAD_IMM R2, 0     (sum)

# Loop
02 03 00            # LOAD R3, [R0]     (load array element)
04 02 03            # ADD R2, R3        (add to sum)
04 00 04            # ADD R0, R4        (next element - assuming R4=4)
14 01               # DEC R1            (decrement counter)
10 08 00 00 00      # JNZ 0x08          (loop if more elements)

FF                  # HALT

# Array data at 0x50
0A 00 00 00         # 10
14 00 00 00         # 20  
1E 00 00 00         # 30
28 00 00 00         # 40
32 00 00 00         # 50
```

### Error Handling

Common errors and solutions:

#### Invalid Memory Access
```hex
# Bad: accessing uninitialized memory
02 00 01            # LOAD R0, [R1]  # R1 not initialized!

# Good: initialize address first
01 01 10 00 00 00    # LOAD_IMM R1, 0x10
02 00 01            # LOAD R0, [R1]
```

#### Stack Overflow
```hex
# Bad: infinite recursion
0C 00 00 00 00      # CALL 0x00  (calls itself)

# Good: proper base case
0E 00 01            # CMP R0, R1        (check condition)
0F 10 00 00 00      # JZ 0x10           (base case)
0C 00 00 00 00      # CALL 0x00         (recursive case)
0D                  # RET
```

#### Division by Zero
```hex
# Good: check before division
0E 01 02            # CMP R1, R2        (check if R1 == 0)
0F 10 00 00 00      # JZ error_handler  (handle zero case)
12 00 01            # DIV R0, R1        (safe division)
```

### Testing Strategies

#### Unit Testing Pattern
```hex
# Test addition function
01 00 05 00 00 00    # LOAD_IMM R0, 5
01 01 03 00 00 00    # LOAD_IMM R1, 3
0C 50 00 00 00      # CALL add_func
01 02 08 00 00 00    # LOAD_IMM R2, 8  (expected result)
0E 00 02            # CMP R0, R2
0F 30 00 00 00      # JZ test_pass
# Test failed
FF                  # HALT

# Test passed
30: # Continue with next test...
```

## Advanced Topics

### Memory Management

VirtComp provides a flat memory model, but you can implement your own memory management:

#### Simple Heap Allocator
```hex
# Simple bump allocator
# R7 = heap pointer (starts at 0x1000)
01 07 00 10 00 00    # LOAD_IMM R7, 0x1000

# Allocate function (size in R0, returns address in R0)
06 01 07            # MOV R1, R7        (save current heap top)
04 07 00            # ADD R7, R0        (advance heap pointer)
06 00 01            # MOV R0, R1        (return old heap top)
0D                  # RET
```

#### Stack Frame Management
```hex
# Function prologue
0A 06               # PUSH R6           (save old frame pointer)
06 06 07            # MOV R6, R7        (set new frame pointer)
# ... allocate local variables by adjusting SP

# Function epilogue  
06 07 06            # MOV R7, R6        (restore stack pointer)
0B 06               # POP R6            (restore frame pointer)
0D                  # RET
```

### Performance Optimization

#### Loop Unrolling
```hex
# Unrolled loop (process 4 elements at once)
02 01 00            # LOAD R1, [R0]     # Element 0
04 02 01            # ADD R2, R1
02 01 00            # LOAD R1, [R0+4]   # Element 1  
04 02 01            # ADD R2, R1
02 01 00            # LOAD R1, [R0+8]   # Element 2
04 02 01            # ADD R2, R1  
02 01 00            # LOAD R1, [R0+12]  # Element 3
04 02 01            # ADD R2, R1

# Advance by 16 bytes (4 elements)
01 03 10 00 00 00    # LOAD_IMM R3, 16
04 00 03            # ADD R0, R3
```

#### Register Allocation
```hex
# Good: keep frequently used values in registers
01 00 64 00 00 00    # LOAD_IMM R0, 100  # loop counter
01 01 00 00 00 00    # LOAD_IMM R1, 0    # accumulator
01 02 04 00 00 00    # LOAD_IMM R2, 4    # constant stride

# Loop using only registers
04 01 03            # ADD R1, R3        # use R3 for data
04 03 02            # ADD R3, R2        # advance by stride  
14 00               # DEC R0            # decrement counter
10 XX XX XX XX      # JNZ loop_start    # branch
```

## Assembly Language (Future)

*This section will be updated when the assembly language compiler is implemented.*

The planned assembly language will provide:

- **Symbolic labels** instead of numeric addresses
- **Mnemonics** for easier instruction writing  
- **Directives** for data definition and memory layout
- **Macros** for code reuse
- **Include files** for modular programming

**Planned syntax example**:
```assembly
.data
    message: .string "Hello, World!"
    counter: .dword 0

.text
main:
    load_imm r0, message
    outstr 1, r0
    halt

.end
```

This will compile to the hex format currently used by VirtComp.

## Examples and Tutorials

### Complete Program Examples

Check the `tests/` directory for complete example programs:

- **helloworld.hex**: Basic output
- **fibonacci.hex**: Recursive algorithm
- **factorial.hex**: Loop and arithmetic
- **string_ops.hex**: String manipulation
- **device_test.hex**: Device I/O demonstration

### Learning Path

1. **Start with basic arithmetic**: Add, subtract, load immediate values
2. **Learn memory operations**: Load from and store to memory
3. **Master control flow**: Jumps, comparisons, loops
4. **Practice stack operations**: Function calls and returns
5. **Explore device I/O**: Console, file, and other device interaction
6. **Advanced topics**: Memory management, optimization, complex algorithms

### Best Practices

1. **Plan your memory layout** before writing code
2. **Use consistent register allocation** within functions
3. **Comment extensively** - hex is hard to read
4. **Test incrementally** - build up complex programs step by step
5. **Use the debugger** - step through code to understand execution
6. **Handle edge cases** - check for division by zero, null pointers, etc.

## Reference Quick Guide

### Instruction Quick Reference

| Mnemonic | Opcode | Format | Description |
|----------|--------|--------|-------------|
| NOP      | 0x00   | `00`   | No operation |
| LOAD_IMM | 0x01   | `01 reg imm32` | Load immediate |
| LOAD     | 0x02   | `02 dst src` | Load from memory |
| STORE    | 0x03   | `03 addr src` | Store to memory |
| ADD      | 0x04   | `04 dst src` | Add registers |
| SUB      | 0x05   | `05 dst src` | Subtract registers |
| MOV      | 0x06   | `06 dst src` | Move register |
| JMP      | 0x07   | `07 addr32` | Jump unconditional |
| PUSH     | 0x0A   | `0A reg` | Push to stack |
| POP      | 0x0B   | `0B reg` | Pop from stack |
| CALL     | 0x0C   | `0C addr32` | Call function |
| RET      | 0x0D   | `0D` | Return |
| CMP      | 0x0E   | `0E r1 r2` | Compare registers |
| JZ       | 0x0F   | `0F addr32` | Jump if zero |
| JNZ      | 0x10   | `10 addr32` | Jump if not zero |
| OUTSTR   | 0x1F   | `1F port reg` | Output string |
| HALT     | 0xFF   | `FF` | Stop execution |

### Device Port Map

| Port | Device | Purpose |
|------|--------|---------|
| 1    | Console | Text I/O |
| 2    | File | File system access |
| 3    | Counter | Timer/counter |
| 4    | Serial | Hardware serial port |
| 5-255| Available | For custom devices |

This completes the comprehensive usage documentation for VirtComp programming!
