# VirtComp Troubleshooting Guide

This guide helps you diagnose and fix common issues when using VirtComp.

## Table of Contents

- [Program Errors](#program-errors)
- [Build Issues](#build-issues)
- [Runtime Problems](#runtime-problems)
- [Debug Tips](#debug-tips)
- [Performance Issues](#performance-issues)
- [Common Mistakes](#common-mistakes)

## Program Errors

### Invalid Instruction Error

**Symptom**: Program halts with "Invalid instruction" error

**Causes**:
1. Incorrect opcode in hex program
2. Jumping to data section or uninitialized memory
3. Corrupted program file

**Solutions**:
```hex
# Bad: Invalid opcode
FE                  # Invalid opcode (not defined)

# Good: Valid opcode
FF                  # HALT (valid)
```

**Debug Steps**:
1. Check the address where error occurred
2. Verify opcodes against instruction set reference
3. Ensure program counter isn't jumping to data

### Memory Access Violation

**Symptom**: "Memory access violation" or segmentation fault

**Causes**:
1. Accessing memory outside allocated range
2. Uninitialized address registers
3. Stack overflow

**Solutions**:
```hex
# Bad: Uninitialized register
06 00 01            # LOAD R0, [R1]  ; R1 not initialized!

# Good: Initialize first
01 01 10 00 00 00    # LOAD_IMM R1, 0x10
06 00 01            # LOAD R0, [R1]   ; Now safe
```

**Prevention**:
- Always initialize address registers before use
- Check array bounds before access
- Monitor stack pointer in recursive functions

### Stack Overflow/Underflow

**Symptom**: Stack-related errors or unexpected behavior

**Causes**:
1. Infinite recursion
2. Too many PUSH without POP
3. POP from empty stack

**Solutions**:
```hex
# Bad: Infinite recursion
my_function:
0C 00 00 00 00      # CALL my_function  ; Calls itself!

# Good: Base case
my_function:
0E 00 01            # CMP R0, R1        ; Check condition
0F 10 00 00 00      # JZ base_case      ; Exit condition
0C 00 00 00 00      # CALL my_function  ; Recursive call
base_case:
0D                  # RET
```

### Division by Zero

**Symptom**: Program halts on DIV instruction

**Cause**: Attempting to divide by zero

**Solution**:
```hex
# Good: Check before division
0E 01 02            # CMP R1, R2        ; Check if divisor is 0
0F 20 00 00 00      # JZ error_handler  ; Handle zero case
12 00 01            # DIV R0, R1        ; Safe division

error_handler:
# Handle division by zero appropriately
```

## Build Issues

### Compilation Errors

**Missing Dependencies**:
```bash
# Install required packages on Ubuntu/Debian
sudo apt update
sudo apt install g++ libfmt-dev libgl1-mesa-dev libglfw3-dev

# On other systems, check documentation for equivalent packages
```

**C++17 Support**:
```bash
# Ensure compiler supports C++17
g++ --version

# If too old, update compiler or use newer version
# Example for Ubuntu:
sudo apt install g++-9
```

### Linking Errors

**Missing Libraries**:
```bash
# Check if libraries are installed
pkg-config --cflags --libs fmt
pkg-config --cflags --libs glfw3

# Add library paths to Makefile if needed
LIBS += -L/usr/local/lib -lfmt
```

### Build Configuration Issues

**Wrong Architecture**:
```bash
# Clean and rebuild for correct architecture
make clean
make

# For debug build
make debug
```

## Runtime Problems

### Program Won't Start

**File Not Found**:
```bash
# Check file path and permissions
ls -la tests/your_program.hex
./bin/virtcomp tests/your_program.hex

# Use absolute path if needed
./bin/virtcomp $(pwd)/tests/your_program.hex
```

**Permission Issues**:
```bash
# Make executable
chmod +x bin/virtcomp

# Check file permissions
ls -la bin/virtcomp
```

### GUI Won't Start

**Display Issues**:
```bash
# Check if running in graphical environment
echo $DISPLAY

# For SSH sessions, enable X11 forwarding
ssh -X username@hostname

# For WSL, install X server
# Use VcXsrv or similar X server for Windows
```

**OpenGL Issues**:
```bash
# Check OpenGL support
glxinfo | grep "OpenGL version"

# Install mesa utilities if needed
sudo apt install mesa-utils
```

### Device Communication Problems

**Console Not Responding**:
1. Check if console device is registered at port 1
2. Verify OUTSTR/INSTR instructions are correct
3. Ensure strings are null-terminated

**File Device Issues**:
1. Check file permissions and paths
2. Verify file exists and is readable
3. Check if file device is properly initialized

## Debug Tips

### Using the Debug GUI

**Enable Step-by-Step Execution**:
```bash
./bin/virtcomp your_program.hex --gui
```

**Key Debug Windows**:
- **CPU State**: Monitor registers and flags
- **Memory View**: Examine memory contents
- **Device Status**: Check device states

### Manual Debugging

**Add Debug Output**:
```hex
# Add debug prints at key points
01 00 XX 00 00 00    # LOAD_IMM R0, debug_message_addr
39 01 00            # OUTSTR port=1, R0
```

**Register Dumps**:
```hex
# Output register values for debugging
02 00 01            # ADD R0, R1    ; R1 = ASCII '0'
31 01 00            # OUT port=1, R0 ; Print R0 as digit
```

### Trace Execution

**Program Counter Tracking**:
- Use debug GUI to step through instructions
- Note PC values at each step
- Compare with expected execution flow

**Memory Monitoring**:
- Watch memory changes in debug GUI
- Set memory view to key addresses
- Monitor stack pointer changes

## Performance Issues

### Slow Execution

**Infinite Loops**:
```hex
# Bad: No exit condition
loop_start:
07 00 00 00 00      # JMP loop_start  ; Infinite loop!

# Good: Proper loop control
01 00 0A 00 00 00    # LOAD_IMM R0, 10   ; Counter
loop_start:
13 00               # DEC R0            ; Decrement
0A 00 01            # CMP R0, R1        ; Compare with 0
0C XX XX XX XX      # JNZ loop_start    ; Continue if not zero
```

**Inefficient Algorithms**:
- Use better algorithms for sorting, searching
- Minimize memory access in tight loops
- Cache frequently used values in registers

### Memory Usage

**Large Programs**:
- Increase memory size if needed
- Optimize data structures
- Remove unused code and data

## Common Mistakes

### Hex Format Errors

**Invalid Hex Characters**:
```hex
# Bad: Invalid characters
01 0G 10 00 00 00    # 'G' is not valid hex

# Good: Valid hex only
01 06 10 00 00 00    # All characters 0-9, A-F
```

**Missing Bytes**:
```hex
# Bad: Incomplete instruction
01 00               # LOAD_IMM needs 6 bytes total

# Good: Complete instruction
01 00 10 00 00 00    # LOAD_IMM R0, 0x10
```

### Register Usage

**Register Corruption**:
```hex
# Bad: Overwriting important values
01 00 10 00 00 00    # LOAD_IMM R0, 16
# ... later in code ...
01 00 05 00 00 00    # LOAD_IMM R0, 5  ; Overwrites previous value!

# Good: Use different registers or save/restore
01 00 10 00 00 00    # LOAD_IMM R0, 16
0A 00               # PUSH R0          ; Save value
01 00 05 00 00 00    # LOAD_IMM R0, 5   ; Temporary use
# ... use R0 ...
0B 00               # POP R0           ; Restore value
```

### Control Flow

**Missing Return Statements**:
```hex
# Bad: Function falls through
my_function:
02 00 01            # ADD R0, R1
# Missing RET!

# Good: Always return
my_function:
02 00 01            # ADD R0, R1
1B                  # RET
```

**Wrong Jump Addresses**:
```hex
# Calculate addresses carefully
# Use comments to mark target addresses

# Address 0x10:
target_address:
01 00 05 00 00 00    # LOAD_IMM R0, 5

# Jump to address 0x10
05 10 00 00 00      # JMP target_address
```

### String Handling

**Missing Null Terminators**:
```hex
# Bad: String without null terminator
48 65 6C 6C 6F      # "Hello" - missing \0

# Good: Null-terminated string
48 65 6C 6C 6F 00    # "Hello\0"
```

**Buffer Overflows**:
```hex
# Allocate sufficient buffer space for strings
# Check string lengths before copying
# Always null-terminate strings
```

## Getting Help

### Debug Information

When reporting issues, include:
1. Complete program that reproduces the issue
2. Error messages (exact text)
3. Platform and compiler information
4. Steps to reproduce

### Log Analysis

**Enable Detailed Logging**:
```bash
# Compile with debug symbols
make debug

# Run with debug output
./bin/virtcomp program.hex --gui 2>&1 | tee debug.log
```

### Community Resources

- Check example programs in `tests/` directory
- Review documentation in `docs/` folder
- Use debug GUI for visual inspection
- Start with simple programs and build complexity gradually

Remember: Most issues stem from incorrect hex formatting, uninitialized registers, or logic errors in program flow. The debug GUI is your best tool for understanding what's happening during execution.
