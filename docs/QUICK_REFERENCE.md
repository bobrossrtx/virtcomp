# VirtComp Instruction Set Quick Reference

## Basic Syntax
- Instructions are written in hexadecimal
- Comments start with `#`
- Registers: R0-R7 (encoded as 0x00-0x07)
- 32-bit immediate values are little-endian

## Instruction Categories

### Basic Operations
```hex
00                    # NOP - No operation
FF                    # HALT - Stop execution
01 <reg> <imm32>      # LOAD_IMM - Load immediate value
06 <dst> <src>        # MOV - Copy register
```

### Arithmetic
```hex
04 <dst> <src>        # ADD - Add registers
05 <dst> <src>        # SUB - Subtract registers  
11 <dst> <src>        # MUL - Multiply registers
12 <dst> <src>        # DIV - Divide registers
13 <reg>              # INC - Increment register
14 <reg>              # DEC - Decrement register
```

### Memory Operations
```hex
02 <dst> <addr_reg>   # LOAD - Load from memory
03 <addr_reg> <src>   # STORE - Store to memory
```

### Control Flow
```hex
07 <addr32>           # JMP - Unconditional jump
0E <reg1> <reg2>      # CMP - Compare registers
0F <addr32>           # JZ - Jump if zero
10 <addr32>           # JNZ - Jump if not zero
15 <addr32>           # JS - Jump if sign
16 <addr32>           # JNS - Jump if not sign
```

### Stack Operations
```hex
0A <reg>              # PUSH - Push register to stack
0B <reg>              # POP - Pop from stack
0C <addr32>           # CALL - Call subroutine
0D                    # RET - Return from subroutine
08 <reg>              # PUSH_ARG - Push function argument
09 <reg>              # POP_ARG - Pop function argument
```

### Bitwise Operations
```hex
17 <dst> <src>        # AND - Bitwise AND
18 <dst> <src>        # OR - Bitwise OR
19 <dst> <src>        # XOR - Bitwise XOR
20 <reg>              # NOT - Bitwise NOT
21 <reg> <amount>     # SHL - Shift left
22 <reg> <amount>     # SHR - Shift right
```

### I/O Operations
```hex
1A <dst> <port>       # IN - Read byte from port
1B <port> <src>       # OUT - Write byte to port
1C <dst> <port>       # INW - Read word from port
1D <port> <src>       # OUTW - Write word to port
1E <dst> <port>       # INL - Read dword from port
1F <port> <src>       # OUTL - Write dword to port
23 <dst> <port>       # INSTR - Read string from port
24 <port> <src>       # OUTSTR - Write string to port
```

### Data Definition
```hex
25 <byte>             # DB - Define byte in program
```

## Common Patterns

### Hello World
```hex
01 00 10 00 00 00     # LOAD_IMM R0, 0x10 (string address)
24 01 00              # OUTSTR port=1, R0
FF                    # HALT

# String data at 0x10
48 65 6C 6C 6F 00     # "Hello\0"
```

### Simple Loop
```hex
01 00 0A 00 00 00     # LOAD_IMM R0, 10 (counter)
# Loop start (0x06):
14 00                 # DEC R0
0E 00 01              # CMP R0, R1 (R1 should be 0)
10 06 00 00 00        # JNZ 0x06 (loop back)
FF                    # HALT
```

### Function Call
```hex
01 00 05 00 00 00     # LOAD_IMM R0, 5
0C 10 00 00 00        # CALL 0x10 (function address)
FF                    # HALT

# Function at 0x10:
13 00                 # INC R0 (increment parameter)
0D                    # RET
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
1. **Little-endian**: Multi-byte values are stored least significant byte first
2. **Stack direction**: Stack grows downward (SP decreases on push)
3. **String termination**: Strings must be null-terminated for OUTSTR
4. **Register encoding**: Registers are single bytes (0x00-0x07)
5. **Address alignment**: 32-bit operations should use 4-byte aligned addresses
