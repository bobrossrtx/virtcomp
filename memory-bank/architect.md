# MemoriPilot: System Architect

## Overview
This file contains the architectural decisions and design patterns for the MemoriPilot project.

## Architectural Decisions

- Custom 32-bit architecture for educational clarity
- Hex-based programming format for transparency
- Device-based I/O system for modularity
- ImGui for cross-platform GUI debugging
- Opcode handler pattern for maintainability
- Singleton device manager for centralized I/O
- fmt library for modern C++ string formatting



1. **Decision 1**: Description of the decision and its rationale.
2. **Decision 2**: Description of the decision and its rationale.
3. **Decision 3**: Description of the decision and its rationale.



## Design Considerations

- Extensible device system allows adding new hardware types
- Opcode dispatcher enables easy instruction set expansion
- Memory-mapped I/O provides unified access model
- Little-endian architecture matches common x86 systems
- Stack-based function calling convention
- Hex format provides human-readable program representation
- Singleton device manager ensures consistent I/O handling



## Components

### CPU Core

32-bit CPU with 8 general-purpose registers (R0-R7), program counter, stack pointer, frame pointer, and flags register

**Responsibilities:**

- Instruction fetch and decode
- Arithmetic and logic operations
- Memory access
- Program flow control
- Stack management

### Memory Management

256-byte default memory space with little-endian addressing and stack growing downward from high memory

**Responsibilities:**

- Memory allocation
- Address translation
- Stack management
- Memory protection

### Device Manager

Singleton pattern manager for all I/O devices with port-based addressing

**Responsibilities:**

- Device registration
- Port mapping
- I/O operations
- Device lifecycle management

### Opcode Dispatcher

Pattern-based instruction execution system with individual handlers for each opcode

**Responsibilities:**

- Instruction dispatch
- Opcode validation
- Handler execution
- Error handling

### Device Factory

Factory pattern for creating and registering various device types

**Responsibilities:**

- Device creation
- Device configuration
- Port assignment
- Device initialization

### GUI Debugger

ImGui-based visual debugging interface for real-time system inspection

**Responsibilities:**

- State visualization
- Memory inspection
- Register display
- Program execution control

### Logger System

Hierarchical logging system with debug, info, warning, and error levels

**Responsibilities:**

- Debug output
- Error reporting
- Execution tracing
- Performance monitoring

### Test Framework

Automated testing system for hex programs with result validation

**Responsibilities:**

- Test execution
- Result validation
- Test reporting
- Program validation



