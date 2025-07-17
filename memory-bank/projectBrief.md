# VirtComp Virtual Machine with Extended Register Architecture

## Purpose

Define the main purpose of this project.

## Target Users

Describe who will use this.


## Project Summary

VirtComp is a comprehensive virtual computer system featuring a custom x64-style CPU architecture with 50 registers, dual-mode operation (x32/x64), 1MB memory, and complete instruction set. Designed as the foundation for a future programming language project, serving as a C++ equivalent to the Java Virtual Machine. Currently transitioning from hex-based programming to human-readable assembly language development.



VirtComp is a comprehensive virtual computer simulation featuring a custom 32-bit CPU architecture, device-based I/O system, and visual debugger. It provides a complete environment for learning computer architecture, assembly programming, and system design.



## Goals

- Implement complete assembly language parser with x64-style register support
- Create CLI assembler tool (virtasm) for human-readable programming
- Establish foundation for high-level programming language compilation target
- Maintain 100% backward compatibility with existing hex-based programs
- Provide professional development environment with debugging tools



- Provide educational platform for computer architecture and assembly programming
- Implement complete virtual computer with CPU, memory, and I/O devices
- Support hex-based programming with comments and documentation
- Offer visual debugging through ImGui-based interface
- Enable extensible device system for virtual and real hardware
- Support program compilation to standalone executables



## Constraints

- Must maintain backward compatibility with existing test suite
- Assembly language must support all 50 registers
- Performance must remain efficient with expanded architecture
- Two-pass assembler design for proper symbol resolution



- C++17 compatible compiler required
- Uses custom hex format for programs
- Limited to 32-bit addressing
- Memory size constrained to 256 bytes default
- Linux-focused development environment



## Stakeholders

- Developer (primary user)
- Future programming language users
- Virtual machine enthusiasts
- Assembly programming community



- Students learning computer architecture
- Educators teaching system design
- Developers interested in virtual machine implementation
- Assembly language programmers
- System designers and architects

