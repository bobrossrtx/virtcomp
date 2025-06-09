# VirtComp Documentation

Welcome to the VirtComp documentation! This virtual computer simulation provides a complete environment for learning computer architecture, assembly programming, and system design.

## Documentation Sections

### [Codebase Documentation](./codebase/README.md)
Technical documentation for developers working on VirtComp itself. Includes detailed API references, architecture overviews, and implementation details that mirror and expand upon the /** documentation comments **/ found in the source code.

- **[Architecture & Components](./codebase/README.md)** - Core system design and implementation
- **[API Reference](./codebase/API_REFERENCE.md)** - Complete class and function documentation

### [Usage Documentation](./usage/README.md)
User guides for programming the virtual computer. Covers writing hex programs, understanding the instruction set, working with devices, and (future) assembly language programming.

### [Quick References](./QUICK_REFERENCE.md)
Fast lookup for instruction set, device ports, and common patterns.

### [Example Programs](./examples/README.md)
Well-documented example programs demonstrating various programming concepts and VirtComp features.

### [Troubleshooting Guide](./TROUBLESHOOTING.md)
Solutions for common problems, debugging tips, and best practices.

## Quick Start

1. **Build VirtComp**: Run `make` in the project root
2. **Run a program**: `./bin/virtcomp tests/helloworld.hex`
3. **Enable debug GUI**: `./bin/virtcomp tests/helloworld.hex --gui`
4. **Interactive mode**: `./bin/virtcomp --interactive`

## Project Overview

VirtComp is a virtual computer simulation featuring:
- Custom 32-bit CPU architecture with comprehensive instruction set
- Device-based I/O system with virtual and real hardware support
- ImGui-based visual debugger
- Extensible device framework
- Hex-based program format with comment support

## Getting Help

- Check the [Usage Documentation](./usage/README.md) for programming guides
- Refer to the [Codebase Documentation](./codebase/README.md) for technical details
- Look at example programs in the `tests/` directory
- Use the `--help` flag for command-line options
