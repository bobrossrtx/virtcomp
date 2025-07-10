# System Patterns

## Architectural Patterns

- Pattern 1: Description

## Design Patterns

- Pattern 1: Description

## Common Idioms

- Idiom 1: Description

## Opcode Handler Pattern

Each CPU instruction is implemented as a separate handler function following a consistent signature pattern. The opcode dispatcher uses a switch statement to route instructions to their appropriate handlers, enabling easy extension of the instruction set.

### Examples

- handle_add(), handle_sub(), handle_mul() in opcodes_consolidated.cpp
- dispatch_opcode() function routes to appropriate handlers
- All handlers follow void handle_X(CPU&, const std::vector<uint8_t>&, bool&) signature


## Singleton Device Manager

The DeviceManager uses the singleton pattern to provide a global access point for all I/O operations. This ensures consistent device management across the entire system while maintaining a centralized registry of all connected devices.

### Examples

- DeviceManager::instance() in device_manager.hpp
- Used throughout CPU I/O operations
- Device registration and port mapping centralized


## Device Factory Pattern

The DeviceFactory provides static methods to create and automatically register various device types. This centralizes device creation logic and ensures proper initialization and registration in the device manager.

### Examples

- DeviceFactory::createConsoleDevice() in device_factory.hpp
- DeviceFactory::createFileDevice() for file I/O
- DeviceFactory::createRamDiskDevice() for storage


## Centralized Logging System

The Logger system provides a centralized logging facility with different severity levels (debug, info, warning, error). It uses a singleton pattern and provides colored output for different log levels.

### Examples

- Logger::instance().debug() for debug messages
- Logger::instance().error() for error reporting
- Used throughout CPU execution for tracing


## VirtComp CPU Opcodes

Complete reference of all VirtComp CPU opcodes with their hex values, descriptions, and usage patterns.

**Basic Operations:**
- 0x00 NOP - No operation
- 0x01 LOAD_IMM - Load immediate value into register
- 0x02 ADD - Add reg1, reg2
- 0x03 SUB - Subtract reg1, reg2
- 0x04 MOV - Move reg1, reg2 (reg1 = reg2)
- 0x05 JMP - Jump to address
- 0x06 LOAD - Load value from memory to register
- 0x07 STORE - Store value from register to memory

**Stack Operations:**
- 0x08 PUSH - Push register onto stack
- 0x09 POP - Pop value from stack to register
- 0x0A CMP - Compare reg1, reg2

**Conditional Jumps:**
- 0x0B JZ - Jump if zero flag set
- 0x0C JNZ - Jump if zero flag not set
- 0x0D JS - Jump if sign flag set
- 0x0E JNS - Jump if sign flag not set

**Arithmetic Operations:**
- 0x10 MUL - Multiply reg1, reg2
- 0x11 DIV - Divide reg1, reg2
- 0x12 INC - Increment register
- 0x13 DEC - Decrement register

**Bitwise Operations:**
- 0x14 AND - Bitwise AND reg1, reg2
- 0x15 OR - Bitwise OR reg1, reg2
- 0x16 XOR - Bitwise XOR reg1, reg2
- 0x17 NOT - Bitwise NOT register
- 0x18 SHL - Shift Left reg, immediate
- 0x19 SHR - Shift Right reg, immediate

**Function Operations:**
- 0x1A CALL - Call subroutine
- 0x1B RET - Return from subroutine
- 0x1C PUSH_ARG - Push argument onto stack
- 0x1D POP_ARG - Pop argument from stack
- 0x1E PUSH_FLAG - Push flags onto stack
- 0x1F POP_FLAG - Pop flags from stack

**Memory Operations:**
- 0x20 LEA - Load Effective Address
- 0x21 SWAP - Swap values between register and memory

**I/O Operations:**
- 0x30 IN - Input from port/device to register
- 0x31 OUT - Output from register to port/device
- 0x32 INB - Input byte from port/device to register
- 0x33 OUTB - Output byte from register to port/device
- 0x34 INW - Input word from port/device to register
- 0x35 OUTW - Output word from register to port/device
- 0x36 INL - Input long from port/device to register
- 0x37 OUTL - Output long from register to port/device
- 0x38 INSTR - Input instruction from port/device to register
- 0x39 OUTSTR - Output string from register to port/device

**Data Definition:**
- 0x40 DB - Define byte

**Control:**
- 0xFF HALT - Halt execution

**CPU Flags:**
- FLAG_ZERO (0x01) - Zero flag, set when operation result is zero
- FLAG_SIGN (0x02) - Sign flag, set when operation result is negative

**Common Usage Patterns:**
- Immediate load: 0x01 reg value(32-bit)
- Register-register operations: 0x02-0x0A reg1 reg2
- Memory operations: 0x06-0x07 reg reg_with_address
- Conditional jumps: 0x0B-0x0E address(32-bit)
- I/O operations: 0x30-0x39 reg port

### Examples

- src/test/unit_tests.cpp - test cases showing opcode usage
- tests/*.hex - integration test files with real opcode sequences
- src/vhardware/cpu.hpp - opcode enum definitions


## VirtComp Instruction Format Standards

All VirtComp instructions follow consistent byte patterns: LOAD_IMM uses 3 bytes (opcode + register + immediate), arithmetic operations use 3 bytes (opcode + reg1 + reg2), jumps use 2 bytes (opcode + address), and shift operations use immediate values not register values.

### Examples

- LOAD_IMM: 0x01 0x00 0x0A = Load 10 into R0
- ADD: 0x02 0x00 0x01 = Add R1 to R0
- JC: 0x0F 0x15 = Jump to address 0x15 if carry flag set
- SHL: 0x18 0x00 0x02 = Shift R0 left by 2 positions


## VirtComp Documentation Quality Standards

Established comprehensive standards for VirtComp documentation accuracy and consistency. All opcode references must match cpu.hpp canonical definitions. All instruction format examples must use correct byte counts (especially 3-byte LOAD_IMM). All code examples must be executable and logically correct. Cross-references between documentation files must be consistent. Documentation must reflect actual implementation behavior, not idealized descriptions.

### Examples

- LOAD_IMM must always show 3-byte format: 01 reg low_byte 00 00 00
- Opcode references verified against cpu.hpp: CMP=0x0A, IN=0x30, OUT=0x31, JC=0x0F, JNC=0x22
- Code examples must include proper HALT instruction (FF) for completeness
- Carry flag testing uses NOT + ADD pattern, not direct CMP instruction
- All instruction formats documented with accurate byte layouts and addressing modes


## VirtComp Git Workflow Standards

Established standardized workflow patterns for VirtComp development. Uses custom merge-to-main.sh script for branch merging instead of direct git commands. The script takes the current branch name as a parameter: `merge-to-main.sh {current_branch_name}`. This ensures consistent merge procedures, proper validation, and maintains project-specific merge standards across all contributors.

### Examples

- ./merge-to-main.sh private-memory-bank
- Always use the custom script rather than git merge main
- Script likely includes validation, formatting, and project-specific merge steps
- Maintains consistency across all development branches and merges
