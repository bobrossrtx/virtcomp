# Product Context

Describe the product.

## Overview

Provide a high-level overview of the project.

## Core Features

- Feature 1
- Feature 2

## Technical Stack

- Tech 1
- Tech 2

## Project Description

VirtComp is a complete virtual computer system with a custom 32-bit CPU architecture, comprehensive instruction set, and modern development tools. Features 8 registers, 256-byte memory, full arithmetic/logic operations, stack management, function calls, I/O operations, and device management. Now includes 100% test coverage with 53 unit tests ensuring rock-solid reliability.



VirtComp is a virtual computer simulation that implements a complete 32-bit computing environment. It features a custom CPU with 8 general-purpose registers, a comprehensive instruction set supporting arithmetic, logic, memory, and I/O operations, and a modular device system. The system supports hex-based programming with comment support and provides both command-line and GUI execution modes.



## Architecture

32-bit virtual CPU with 8 registers (R0-R7), 256-byte addressable memory, stack-based function calls with CALL/RET/PUSH_ARG/POP_ARG, comprehensive instruction set including arithmetic, logic, jumps, I/O, and stack operations. Modular design with separate CPU core, device manager, opcode handlers, and testing framework.



The VirtComp system uses a layered architecture with the CPU at the core, surrounded by a device management system, memory management, and I/O abstraction layers. The system employs an opcode dispatcher pattern for instruction execution, with each instruction type having its own handler function. The GUI debugger is built on top of ImGui and provides real-time visualization of CPU state, memory, and device interactions.



## Technologies

- C++17
- Modern CMake
- Unit Testing Framework
- imgui
- OpenGL
- fmt library
- Makefile build system
- Git version control
- Memory bank knowledge management



- C++17
- OpenGL
- ImGui
- Make build system
- Git version control
- Linux development environment
- Custom hex assembly format
- Device abstraction layer
- Opcode dispatcher pattern



## Libraries and Dependencies

- imgui (GUI)
- fmt (formatting)
- OpenGL/GLFW (graphics)
- Custom test framework
- Custom opcode dispatcher
- Device management system



- fmt - Modern C++ formatting library
- ImGui - Immediate mode GUI library
- GLFW - OpenGL window and input library
- GLEW - OpenGL extension wrangler
- OpenGL - Graphics rendering
- Standard C++ Libraries (filesystem, chrono, memory, etc.)

