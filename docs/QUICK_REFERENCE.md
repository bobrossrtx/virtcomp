# VirtComp Instruction Set Quick Reference

## Basic Syntax
- Instructions are written in hexadecimal
- Comments start with `#`
- Registers: R0-R7 (encoded as 0x00-0x07)
- LOAD_IMM uses 3-byte format: opcode + register + immediate_value (8-bit)

## Instruction Categories

### Basic Operations
```hex
00                    # NOP - No operation
FF                    # HALT - Stop execution
01 <reg> <imm8>       # LOAD_IMM - Load immediate value (8-bit)
04 <dst> <src>        # MOV - Copy register
```

### Arithmetic
```hex
02 <dst> <src>        # ADD - Add registers
03 <dst> <src>        # SUB - Subtract registers
10 <dst> <src>        # MUL - Multiply registers
11 <dst> <src>        # DIV - Divide registers
12 <reg>              # INC - Increment register
13 <reg>              # DEC - Decrement register
```

### Memory Operations
```hex
06 <dst> <addr>       # LOAD - Load from memory
07 <addr> <src>       # STORE - Store to memory
20 <reg> <addr>       # LEA - Load Effective Address
21 <reg> <addr>       # SWAP - Swap register with memory
```

### Control Flow
```hex
05 <addr>             # JMP - Unconditional jump
0A <reg1> <reg2>      # CMP - Compare registers
0B <addr>             # JZ - Jump if zero
0C <addr>             # JNZ - Jump if not zero
0D <addr>             # JS - Jump if sign
0E <addr>             # JNS - Jump if not sign
0F <addr>             # JC - Jump if carry
22 <addr>             # JNC - Jump if no carry
23 <addr>             # JO - Jump if overflow
24 <addr>             # JNO - Jump if no overflow
```

### Stack Operations
```hex
08 <reg>              # PUSH - Push register to stack
09 <reg>              # POP - Pop from stack
1A <addr>             # CALL - Call subroutine
1B                    # RET - Return from subroutine
1C <reg>              # PUSH_ARG - Push function argument
1D <reg>              # POP_ARG - Pop function argument
1E <reg>              # PUSH_FLAG - Push flags onto stack
1F <reg>              # POP_FLAG - Pop flags from stack
```

### Bitwise Operations
```hex
14 <dst> <src>        # AND - Bitwise AND
15 <dst> <src>        # OR - Bitwise OR
16 <dst> <src>        # XOR - Bitwise XOR
17 <reg>              # NOT - Bitwise NOT
18 <reg> <amount>     # SHL - Shift left (immediate amount)
19 <reg> <amount>     # SHR - Shift right (immediate amount)
```

### I/O Operations
```hex
30 <dst> <port>       # IN - Read from port
31 <port> <src>       # OUT - Write to port
32 <dst> <port>       # INB - Read byte from port
33 <port> <src>       # OUTB - Write byte to port
34 <dst> <port>       # INW - Read word from port
35 <port> <src>       # OUTW - Write word to port
36 <dst> <port>       # INL - Read long from port
37 <port> <src>       # OUTL - Write long to port
38 <dst> <port>       # INSTR - Read string from port
39 <port> <src>       # OUTSTR - Write string to port
```

### Data Definition
```hex
40 <byte>             # DB - Define byte in program
```

## Common Patterns

### Hello World
```hex
01 00 10              # LOAD_IMM R0, 0x10 (string address)
39 01 00              # OUTSTR port=1, R0
FF                    # HALT

# String data at 0x10
48 65 6C 6C 6F 00     # "Hello\0"
```

### Simple Loop
```hex
01 00 0A              # LOAD_IMM R0, 10 (counter)
01 01 00              # LOAD_IMM R1, 0 (comparison value)
# Loop start (0x06):
13 00                 # DEC R0
0A 00 01              # CMP R0, R1
0C 06                 # JNZ 0x06 (loop back)
FF                    # HALT
```

### Function Call
```hex
01 00 05              # LOAD_IMM R0, 5
1A 10                 # CALL 0x10 (function address)
FF                    # HALT

# Function at 0x10:
12 00                 # INC R0 (increment parameter)
1B                    # RET
```

## Device Ports
- **Port 1**: Console (text I/O)
- **Port 2**: File device
- **Port 3**: Counter device
- **Port 4**: Serial port
- **Ports 5-255**: Available for custom devices

## Register Usage Conventions
- **R0-R3**: General purpose, function parameters/return values
- **R4-R6**: General purpose, local variables
- **R7**: Often used as temporary or frame pointer
- **PC**: Program counter (automatic)
- **SP**: Stack pointer (automatic)
- **FP**: Frame pointer (manual management)

## Memory Layout
```
High Memory  ┌─────────┐
            │  Stack  │ ← SP
            ├─────────┤
            │  Free   │
            ├─────────┤
            │  Heap   │
            ├─────────┤
            │  Data   │
            ├─────────┤
Low Memory  │  Code   │ ← PC starts here
            └─────────┘
```

## Common Gotchas
1. **8-bit immediates**: LOAD_IMM only supports 8-bit values (0-255)
2. **Stack direction**: Stack grows downward (SP decreases on push)
3. **String termination**: Strings must be null-terminated for OUTSTR
4. **Register encoding**: Registers are single bytes (0x00-0x07)
5. **Address sizes**: Jump addresses are 8-bit (0-255 program space)
6. **Carry flag**: Requires 32-bit overflow to trigger (use NOT + ADD technique)
