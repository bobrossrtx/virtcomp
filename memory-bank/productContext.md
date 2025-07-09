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

VirtComp is a virtual computer simulation that implements a complete 32-bit computing environment. It features a custom CPU with 8 general-purpose registers, a comprehensive instruction set supporting arithmetic, logic, memory, and I/O operations, and a modular device system. The system supports hex-based programming with comment support and provides both command-line and GUI execution modes.



## Architecture

The VirtComp system uses a layered architecture with the CPU at the core, surrounded by a device management system, memory management, and I/O abstraction layers. The system employs an opcode dispatcher pattern for instruction execution, with each instruction type having its own handler function. The GUI debugger is built on top of ImGui and provides real-time visualization of CPU state, memory, and device interactions.



## Technologies

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

- fmt - Modern C++ formatting library
- ImGui - Immediate mode GUI library
- GLFW - OpenGL window and input library
- GLEW - OpenGL extension wrangler
- OpenGL - Graphics rendering
- Standard C++ Libraries (filesystem, chrono, memory, etc.)

