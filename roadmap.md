# VirtComp Development Roadmap

> **Last Updated:** July 17, 2025
> **Project Status:** Phase 2.5 Complete - Extended Architecture Ready, Assembly Language Development Next Priority

---

## 🎯 Project Overview

VirtComp is a virtual computer system with a custom CPU architecture, supporting instruction execution, device I/O, and debugging capabilities. This roadmap outlines the planned development phases and feature implementations.

--### 🌐 **Phase 3D: Advanced I/O & Networking** *(Q2 2026)*

**Priority: MEDIUM** | **Dependencies: Graphics ### 🌐 **Phase 3D: Advanced I/O & Networking** *(Q2 2026)*

**Priority: MEDIUM** | **D### 🌐 **Phase 3D: Advanced I/O & Networking** *(Q2 2026)*

**Priority: MEDIUM** | **Dependencies: Graphics System Complete**

Expand I/O capabilities for real-world integration and communication.

#### System Call Interface

**Core System Call Implementation**
- **SYSCALL Instruction**: Direct host OS system call interface
- **INT 0x80**: Linux/Unix style interrupt-based syscalls
- **SYSENTER/SYSEXIT**: Fast system call entry/exit for performance
- **Cross-Platform Support**: Unified interface for Linux, Windows, macOS
- **Security Sandboxing**: Controlled access to host system resources

**Standard System Call Library**
```asm
; File operations
SYS_READ    = 0         ; read(fd, buf, count)
SYS_WRITE   = 1         ; write(fd, buf, count)
SYS_OPEN    = 2         ; open(filename, flags, mode)
SYS_CLOSE   = 3         ; close(fd)
SYS_LSEEK   = 8         ; lseek(fd, offset, whence)

; Process operations
SYS_EXIT    = 60        ; exit(status)
SYS_FORK    = 57        ; fork()
SYS_EXECVE  = 59        ; execve(filename, argv, envp)
SYS_WAIT4   = 61        ; wait4(pid, status, options, rusage)

; Memory operations
SYS_MMAP    = 9         ; mmap(addr, length, prot, flags, fd, offset)
SYS_MUNMAP  = 11        ; munmap(addr, length)
SYS_BRK     = 12        ; brk(addr) - heap management

; Network operations
SYS_SOCKET  = 41        ; socket(domain, type, protocol)
SYS_BIND    = 49        ; bind(sockfd, addr, addrlen)
SYS_LISTEN  = 50        ; listen(sockfd, backlog)
SYS_ACCEPT  = 43        ; accept(sockfd, addr, addrlen)
SYS_CONNECT = 42        ; connect(sockfd, addr, addrlen)
```

**Example System Call Usage**
```asm
; Write "Hello World" to stdout
write_hello:
    MOV RAX, SYS_WRITE      ; system call number
    MOV RDI, 1              ; file descriptor (stdout)
    MOV RSI, hello_msg      ; message buffer
    MOV RDX, 12             ; message length
    SYSCALL                 ; invoke system call
    RET

; Open a file for reading
open_file:
    MOV RAX, SYS_OPEN       ; system call number
    MOV RDI, filename       ; file path string
    MOV RSI, 0              ; O_RDONLY flag
    MOV RDX, 0644           ; file permissions (octal)
    SYSCALL                 ; returns file descriptor in RAX
    RET

; Network socket creation
create_socket:
    MOV RAX, SYS_SOCKET     ; system call number
    MOV RDI, 2              ; AF_INET (IPv4)
    MOV RSI, 1              ; SOCK_STREAM (TCP)
    MOV RDX, 0              ; protocol (default)
    SYSCALL                 ; returns socket fd in RAX
    RET

hello_msg: .string "Hello World\n"
filename:  .string "/etc/passwd"
```

**Security & Sandboxing**
- **Permission System**: Controlled access to system resources
- **Capability-based Security**: Fine-grained permission model
- **Resource Limits**: CPU time, memory, file descriptors
- **Namespace Isolation**: Process, network, and filesystem isolation
- **System Call Filtering**: Whitelist/blacklist specific syscalls
- **Audit Logging**: Track all system call interactions

#### Enhanced I/O
- **Serial Communication**: UART device for external communication
- **Network Interface**: TCP/UDP socket simulation
- **Timer System**: Programmable intervals and real-time operations
- **Interrupt System**: Hardware interrupt simulation and handling: Graphics System Complete**

Expand I/O capabilities for real-world integration and communication.

#### System Call Interface

**Core System Call Implementation**
- **SYSCALL Instruction**: Direct host OS system call interface
- **INT 0x80**: Linux/Unix style interrupt-based syscalls
- **SYSENTER/SYSEXIT**: Fast system call entry/exit for performance
- **Cross-Platform Support**: Unified interface for Linux, Windows, macOS
- **Security Sandboxing**: Controlled access to host system resources

**Standard System Call Library**
```asm
; File operations
SYS_READ    = 0         ; read(fd, buf, count)
SYS_WRITE   = 1         ; write(fd, buf, count)
SYS_OPEN    = 2         ; open(filename, flags, mode)
SYS_CLOSE   = 3         ; close(fd)
SYS_LSEEK   = 8         ; lseek(fd, offset, whence)

; Process operations
SYS_EXIT    = 60        ; exit(status)
SYS_FORK    = 57        ; fork()
SYS_EXECVE  = 59        ; execve(filename, argv, envp)
SYS_WAIT4   = 61        ; wait4(pid, status, options, rusage)

; Memory operations
SYS_MMAP    = 9         ; mmap(addr, length, prot, flags, fd, offset)
SYS_MUNMAP  = 11        ; munmap(addr, length)
SYS_BRK     = 12        ; brk(addr) - heap management

; Network operations
SYS_SOCKET  = 41        ; socket(domain, type, protocol)
SYS_BIND    = 49        ; bind(sockfd, addr, addrlen)
SYS_LISTEN  = 50        ; listen(sockfd, backlog)
SYS_ACCEPT  = 43        ; accept(sockfd, addr, addrlen)
SYS_CONNECT = 42        ; connect(sockfd, addr, addrlen)
```

**Example System Call Usage**
```asm
; Write "Hello World" to stdout
write_hello:
    MOV RAX, SYS_WRITE      ; system call number
    MOV RDI, 1              ; file descriptor (stdout)
    MOV RSI, hello_msg      ; message buffer
    MOV RDX, 12             ; message length
    SYSCALL                 ; invoke system call
    RET

; Open a file for reading
open_file:
    MOV RAX, SYS_OPEN       ; system call number
    MOV RDI, filename       ; file path string
    MOV RSI, 0              ; O_RDONLY flag
    MOV RDX, 0644           ; file permissions (octal)
    SYSCALL                 ; returns file descriptor in RAX
    RET

; Network socket creation
create_socket:
    MOV RAX, SYS_SOCKET     ; system call number
    MOV RDI, 2              ; AF_INET (IPv4)
    MOV RSI, 1              ; SOCK_STREAM (TCP)
    MOV RDX, 0              ; protocol (default)
    SYSCALL                 ; returns socket fd in RAX
    RET

hello_msg: .string "Hello World\n"
filename:  .string "/etc/passwd"
```

**Security & Sandboxing**
- **Permission System**: Controlled access to system resources
- **Capability-based Security**: Fine-grained permission model
- **Resource Limits**: CPU time, memory, file descriptors
- **Namespace Isolation**: Process, network, and filesystem isolation
- **System Call Filtering**: Whitelist/blacklist specific syscalls
- **Audit Logging**: Track all system call interactions

#### Enhanced I/O
- **Serial Communication**: UART device for external communication
- **Network Interface**: TCP/UDP socket simulation
- **Timer System**: Programmable intervals and real-time operations
- **Interrupt System**: Hardware interrupt simulation and handlinglete**

Expand I/O capabilities for real-world integration and communication.## 🏆 Completed Features

### ✅ Core CPU Architecture
- **Complete Instruction Set**: Arithmetic, logic, memory, and control flow operations
- **Register System**: 8 general-purpose 32-bit registers
- **Memory Management**: 256-byte addressable memory space
- **Stack Operations**: PUSH/POP with stack pointer management
- **Flag System**: Zero, Sign, Carry, and Overflow flags for conditional operations
- **Jump Instructions**: Complete set including JG/JL/JGE/JLE comparison jumps

### ✅ Output Formatting & Polish *(100% Complete)*
- ✅ **ASCII Art Removal**: Cleaned up debug output for professional appearance
- ✅ **Debug Bar Cleanup**: Removed decorative horizontal bars and formatting tokens
- ✅ **Extended Register Display**: Implemented `-er`/`--extended-registers` command-line flag
- ✅ **Professional UI**: Extended registers shown after regular registers at program end
- ✅ **Logger Enhancement**: Level-only color highlighting for improved readability
- ✅ **Register Update Tracking**: Framework for individual register change notifications

### ✅ Device System
- **Device Manager**: Centralized I/O device management
- **Console Device**: Text input/output capabilities
- **Counter Device**: Programmable counter for timing operations
- **File Device**: Virtual file system access
- **RAM Disk**: In-memory block storage device

### ✅ Testing Framework
- **Modern Unit Testing**: Comprehensive test framework with 53 unit tests
- **Integration Testing**: 39 hex file execution validation tests
- **Test Coverage**: 100% pass rate (53/53 unit tests, 39/39 integration tests)
- **Automated Testing**: Integrated with build system (`make test` command)
- **Bug Resolution**: Successfully debugged and fixed all test failures

### ✅ Build System & Documentation
- **Makefile Integration**: Automated compilation and testing
- **Comprehensive Documentation**: API references, usage guides, troubleshooting
- **Memory Bank System**: Project knowledge management and decision tracking

### ✅ Assembly Language Integration *(COMPLETE - July 2025)*
- **VM Integration**: Full assembly language integrated with main VirtComp executable
- **Assembly Mode**: `-A/--assembly` flag for assembling and running .asm files
- **Complete Pipeline**: Lexer → Parser → Assembler → Bytecode → VM execution
- **Flag Validation**: Conflict detection between assembly, test, and program modes
- **Symbol Tables**: Verbose output showing assembled symbols and addresses
- **Error Handling**: Comprehensive error reporting for assembly, parsing, and runtime
- **Debug Integration**: Assembly mode works with `-v`, `-d`, and `--extended-registers`

---

## 🚀 Current Development Phase

### ✅ **Phase 1: CPU Instruction Set Completion** *(100% Complete)*

**Priority: COMPLETED** | **Timeline: Q3 2025** | **Status: All tests passing, ready for Phase 2**

#### Stack & Call Operations *(100% Complete)*
- ✅ **CALL**: Subroutine calls with return address management
- ✅ **RET**: Return from subroutine
- ✅ **PUSH_ARG/POP_ARG**: Function argument handling with context awareness
- ✅ **Stack Frame Management**: Fixed arg_offset initialization and frame pointer handling
- ✅ **Memory Layout**: Corrected subroutine positioning for proper CALL/RET execution

#### Enhanced I/O Operations *(100% Complete)*
- ✅ **Basic I/O**: IN/OUT operations for device communication
- ✅ **Sized Operations**: INB/OUTB, INW/OUTW, INL/OUTL for different data sizes
- ✅ **String I/O**: INSTR/OUTSTR for text processing
- ✅ **Device Integration**: Enhanced device protocol support

#### Advanced Conditional Operations *(100% Complete)*
- ✅ **Carry Flags**: JC/JNC for unsigned arithmetic overflow
- ✅ **Overflow Flags**: JO/JNO for signed arithmetic overflow
- ✅ **Comparison Jumps**: JG/JL/JGE/JLE for signed comparisons
- ✅ **Test Coverage**: All conditional operations validated

#### Memory Management Extensions *(Completed)*
- ✅ **LEA**: Load Effective Address for pointer arithmetic
- ✅ **SWAP**: Register-memory value swapping
- 🔜 **Memory Protection**: Basic bounds checking and access control

---

## 📋 Next Development Phase

### 🔥 **Phase 2: Assembly Language & Parser** *(IMMEDIATE NEXT PRIORITY - Starting Tomorrow)*

**Priority: CRITICAL** | **Dependencies: Phase 2.5 ✅ Complete**

Transform VirtComp from hex-based programming to human-readable assembly language with full extended register support.

> **Important Architecture Goals**: VirtComp is designed to be the new base for a future programming language project, serving as a C++ equivalent to the Java Virtual Machine. The assembly language implementation should consider this long-term vision of x64-like architecture emulation.

#### Core Language Design

- **Mnemonic System**: Human-readable instruction names (ADD, MOV, JMP, etc.)
- **Extended Register Set**: Support for all 50 registers (RAX, RBX, RCX, RDX, RSP, RBP, RSI, RDI, R8-R15, plus segment/control/debug registers)
- **Syntax Definition**: Consistent instruction format and operand specification
- **Addressing Modes**: Direct, indirect, immediate, and indexed addressing
- **Label System**: Named memory locations and jump targets
- **Comment Support**: Line comments (;) and block comments (/* */)

#### Advanced Language Features

- **Assembler Directives**: .data, .text, .org, .equ, .include
- **Data Types**: DB, DW, DD for bytes, words, and double words
- **String Literals**: Quoted string constants with escape sequences
- **Numeric Formats**: Decimal, hexadecimal (0x), binary (0b), octal (0o)
- **Expression Evaluation**: Arithmetic in operands (label+4, 2*SIZE, etc.)

#### Parser & Assembler Implementation

- **Two-Pass Assembly**: Symbol collection and code generation phases
- **Symbol Table**: Global label and constant management
- **Error Handling**: Detailed error messages with line numbers and context
- **Output Formats**: Binary, Intel HEX, listing files with addresses
- **Macro System**: Reusable code templates with parameters

#### Development Tools Integration

- **CLI Assembler**: Standalone command-line tool (virtasm)
- **IDE Integration**: Assembly editing with syntax highlighting
- **Debugger Enhancement**: Symbol-aware debugging with source mapping
- **Build System**: Makefile integration for assembly projects

---

### ✅ **Phase 2.5: CPU Architecture Expansion** *(100% Complete - Q4 2025)*

**Priority: COMPLETED** | **Dependencies: Phase 1 ✅ Complete**

Successfully expanded VirtComp's CPU architecture to support a comprehensive x64-like register set and dual-mode operation, preparing for its role as a virtual machine base for future programming language projects.

#### ✅ Extended Register Architecture *(100% Complete)*
- ✅ **50-Register System**: Complete implementation of RAX, RBX, RCX, RDX, RSP, RBP, RSI, RDI, R8-R15 (16 GP), plus segment, control, debug, and special registers
- ✅ **x64-Style Register Names**: Full register naming system with legacy aliases (EAX, AX, AL, etc.)
- ✅ **Register Size Support**: 8-bit, 16-bit, 32-bit, and 64-bit register operations
- ✅ **Extended Register Operations**: MOVEX, ADDEX, SUBEX for R8-R15 register access

#### ✅ Dual-Mode CPU Operation *(100% Complete)*
- ✅ **x32/x64 Mode Switching**: Dynamic switching between 32-bit and 64-bit operation modes
- ✅ **Mode Control Opcodes**: MODE32, MODE64, MODECMP for runtime mode management
- ✅ **Mode-Aware Arithmetic**: Operations automatically adapt width based on current CPU mode
- ✅ **Backward Compatibility**: All existing 32-bit code continues to work seamlessly

#### ✅ Massive Memory Expansion *(100% Complete)*
- ✅ **Memory Capacity**: Expanded from 256 bytes to 1MB (4,096x increase)
- ✅ **Dynamic Memory Resizing**: Runtime memory management with bounds checking
- ✅ **Test Compatibility**: Maintained 256-byte test environment for backward compatibility
- ✅ **Memory Architecture**: Flat memory model with full addressing support

#### ✅ Enhanced Instruction Set *(100% Complete)*
- ✅ **64-bit Arithmetic**: ADD64, SUB64, MUL64, DIV64, CMP64 with proper overflow handling
- ✅ **64-bit Data Movement**: MOV64, LOAD_IMM64 for full 64-bit value manipulation
- ✅ **Extended Register Access**: Full access to all 50 registers through enhanced opcodes
- ✅ **Mode Integration**: All operations respect current CPU mode for seamless dual-mode support

#### ✅ Validation & Testing *(100% Complete)*
- ✅ **Extended Register Test**: Comprehensive test demonstrating x32/x64 mode switching and extended register operations
- ✅ **100% Test Coverage**: All 53 unit tests and 39 integration tests continue to pass
- ✅ **Debug Infrastructure**: Enhanced debugging output with mode indicators and register name resolution
- ✅ **Performance Validation**: 1MB memory operations confirmed working efficiently

#### ✅ UI Enhancement & Professional Polish *(100% Complete)*
- ✅ **Extended Register Display**: Command-line flag (-er/--extended-registers) for 50-register visibility
- ✅ **Output Formatting Cleanup**: Removed ASCII art headers and debug decorative elements
- ✅ **Logger Color Enhancement**: Purple timestamps, colored log levels, clean message display
- ✅ **Register Display Optimization**: Extended registers shown once at end instead of repetitive debug output
- ✅ **Professional UI**: Clean, production-ready output formatting for end users

---

### 🚀 **Phase 2.7: Bytecode Compilation System** *(Long-term Goal - Overtime)*

**Priority: OVERTIME** | **Dependencies: Architecture Expansion**

Transform VirtComp from an interpreter-like system to a proper bytecode compilation target, enabling it to serve as the foundation for a new programming language project.

#### Compilation Architecture

- **Assembly Compilation**: Convert assembly programs to bytecode executable files
- **Bytecode Format**: Standardized executable file format with headers and metadata
- **Loader System**: Bytecode file loading and execution environment
- **Static Analysis**: Compilation-time optimization and error detection
- **Symbol Export**: Inter-module linking and library support

#### Virtual Machine Enhancement

- **Execution Engine**: Optimized bytecode interpreter or JIT compilation
- **Memory Management**: Heap allocation, garbage collection foundation
- **Module System**: Dynamic library loading and symbol resolution
- **Standard Library**: Core runtime functions and system interfaces
- **Exception Handling**: Structured error propagation and recovery

#### Programming Language Foundation

- **High-Level Frontend**: Target for future programming language compiler
- **Type System Support**: Runtime type information and checking
- **Object Model**: Class and object system primitives
- **Memory Safety**: Bounds checking, null pointer protection
- **Interoperability**: C/C++ FFI for external library integration

#### Compilation Codegen Infrastructure

- **AST Generation**: Abstract Syntax Tree representation of assembly programs
- **Intermediate Representation**: Three-address code (TAC) generation for optimization passes
- **Peephole Optimization**: Local instruction sequence improvements for efficiency
- **Register Allocation**: Graph coloring algorithm for optimal register usage
- **Dead Code Elimination**: Removal of unreachable or ineffective instructions
- **Constant Folding**: Compile-time evaluation of constant expressions
- **Instruction Scheduling**: Reordering for pipeline optimization and latency reduction
- **Platform-Specific Tuning**: Target-aware code generation for x32/x64 modes
- **Link-Time Optimization**: Cross-module code improvements
- **Debug Information**: Source mapping for symbolic debugging support

---

## 🚀 **NEW DEVELOPMENT ROADMAP** *(Updated July 2025)*

Based on the successful completion of assembly language integration, here is the planned development sequence:

### 📚 **Phase 3A: Assembly Language Documentation & Examples** *(COMPLETED - July 2025)*

**Priority: COMPLETED** | **Dependencies: Assembly Integration ✅ Complete**

Essential for usability - create comprehensive documentation and examples so developers can actually use the assembly language we've built.

#### ✅ Documentation Creation *(COMPLETED)*
- ✅ **Assembly Language Manual**: Complete instruction reference with syntax and examples
- ✅ **Tutorial Series**: Step-by-step guides from basic to advanced assembly programming
- ✅ **API Documentation**: Register descriptions, addressing modes, and calling conventions
- ✅ **Quick Reference**: Printable instruction set summary and register map
- ✅ **Module Documentation**: Comprehensive technical docs for all 6 core modules
- ✅ **Developer Codebase README**: Professional technical documentation structure

#### ✅ Example Programs *(COMPLETED)*
- ✅ **Hello World**: Basic text output demonstration with multiple approaches
- ✅ **Arithmetic Examples**: Addition, subtraction, multiplication, division
- ✅ **Control Flow**: Loops, conditionals, and function calls
- ✅ **Device I/O**: Console, file, and counter device usage
- ✅ **Advanced Examples**: Complex programs showcasing full feature set

#### ✅ Testing & Validation *(COMPLETED)*
- ✅ **Example Verification**: All examples compile and run correctly
- ✅ **Documentation Testing**: All code snippets and references validated
- ✅ **Professional Quality**: Documentation suitable for open source contributors

---

### 🔧 **Phase 3B: Enhanced Instruction Set & Assembly Language Features** *(Next Priority - Q4 2025)*

**Priority: CRITICAL** | **Dependencies: Phase 3A ✅ Complete**

Based on advanced assembly programming needs, implement missing opcodes and language features to support modern programming patterns like the user's enhanced Hello World example.

#### Missing Opcodes for Modern Assembly Programming

**🎯 Priority 1: Essential Missing Opcodes**
```asm
; User's target program style:
; .org 0x1000
; start:
;     LOAD_IMM RAX, string      ; ← LOAD_IMM with address/label support needed
;     LOAD_IMM RCX, 0           ; ← Basic immediate load (exists)
; print_loop:
;     LOAD RBX, [RAX+RCX]       ; ← LOAD with indexed addressing (MISSING)
;     CMP RBX, 0                ; ← Compare (exists)
;     JE exit                   ; ← Conditional jump (exists)
;     WRITE_PORT 0x03, RBX      ; ← WRITE_PORT instruction (MISSING)
;     INC RCX                   ; ← Increment (exists)
;     JMP print_loop            ; ← Jump (exists)
; exit:
;     HALT                      ; ← Halt (exists)
; string:
;     .string "Hello, World!\n" ; ← String literal support (MISSING)
```

**New Opcodes to Implement:**
- ✅ `WRITE_PORT <port>, <reg>` - Direct port output (reverse of OUT syntax)
- ✅ `READ_PORT <reg>, <port>` - Direct port input (alternative to IN syntax)
- ✅ `LOAD [reg+offset]` - Indexed memory addressing
- ✅ `STORE [reg+offset]` - Indexed memory store
- ✅ `MOD <dst>, <src>` - Modulo arithmetic operation
- ✅ `EXP <dst>, <src>` - Exponentiation operation
- ✅ `ABS <dst>, <src>` - Absolute value
- ✅ `MIN <dst>, <src1>, <src2>` - Minimum of two values
- ✅ `MAX <dst>, <src1>, <src2>` - Maximum of two values
- ✅ `SQRT <dst>, <src>` - Square root (integer approximation)

**🎯 Priority 2: Advanced Arithmetic Operations**
```asm
; Enhanced mathematical capabilities
MOV RAX, 15
MOV RBX, 4
MOD RAX, RBX        ; RAX = 15 % 4 = 3

MOV RAX, 2
MOV RBX, 8
EXP RAX, RBX        ; RAX = 2^8 = 256

MOV RAX, -42
ABS RAX, RAX        ; RAX = 42

MOV RAX, 10
MOV RBX, 25
MIN RCX, RAX, RBX   ; RCX = 10
MAX RDX, RAX, RBX   ; RDX = 25
```

**🎯 Priority 3: String and Data Handling**
```asm
; Assembler directive support
.string "Hello, World!\n"    ; Null-terminated string literal
.bytes 0x48, 0x65, 0x6C     ; Raw byte sequences
.word 0x1234, 0x5678        ; 16-bit word arrays
.dword 0x12345678           ; 32-bit double word
.qword 0x123456789ABCDEF0   ; 64-bit quad word
.space 256                  ; Reserve uninitialized space
.align 4                    ; Memory alignment directive
```

**🎯 Priority 4: Advanced Memory Operations**
```asm
; Enhanced addressing modes
LOAD RAX, [RBX]             ; Indirect addressing (exists)
LOAD RAX, [RBX+RCX]         ; Base + index addressing (MISSING)
LOAD RAX, [RBX+RCX*2]       ; Base + scaled index (MISSING)
LOAD RAX, [RBX+RCX*4+8]     ; Full x86-style addressing (MISSING)

; String operations
STRCPY RDI, RSI             ; String copy
STRLEN RAX, RSI             ; String length
STRCMP RAX, RSI, RDI        ; String comparison
STRCAT RDI, RSI             ; String concatenation
```

**🎯 Priority 5: Floating Point Operations**
```asm
; Basic floating point support
FADD XMM0, XMM1             ; Float addition
FSUB XMM0, XMM1             ; Float subtraction
FMUL XMM0, XMM1             ; Float multiplication
FDIV XMM0, XMM1             ; Float division
FSQRT XMM0, XMM1            ; Float square root
FSIN XMM0, XMM1             ; Sine function
FCOS XMM0, XMM1             ; Cosine function
```

**🎯 Priority 6: System Call Interface**
```asm
; Host OS interaction through syscalls
SYSCALL                     ; Generic system call interface
INT 0x80                    ; Linux/Unix style interrupt
SYSENTER                    ; Fast system call entry
SYSEXIT                     ; Fast system call exit

; Common syscall examples
MOV RAX, 1                  ; sys_write
MOV RDI, 1                  ; stdout
MOV RSI, msg_addr           ; message buffer
MOV RDX, msg_len            ; message length
SYSCALL                     ; invoke system call

MOV RAX, 60                 ; sys_exit
MOV RDI, 0                  ; exit status
SYSCALL                     ; terminate program

; File operations
MOV RAX, 2                  ; sys_open
MOV RDI, filename           ; file path
MOV RSI, 0                  ; O_RDONLY
SYSCALL                     ; returns file descriptor in RAX

MOV RAX, 0                  ; sys_read
MOV RDI, RAX                ; file descriptor
MOV RSI, buffer             ; read buffer
MOV RDX, 1024               ; bytes to read
SYSCALL                     ; read from file

MOV RAX, 3                  ; sys_close
MOV RDI, RAX                ; file descriptor
SYSCALL                     ; close file
```

#### Assembler Language Enhancements

**🎯 Enhanced Directive Support**
```asm
.org 0x1000                 ; Set origin address
.section .text              ; Code section
.section .data              ; Data section
.section .bss               ; Uninitialized data
.global start               ; Global symbol export
.extern printf              ; External symbol import
.include "macros.inc"       ; File inclusion
.if DEBUG                   ; Conditional assembly
.endif
```

**🎯 Macro System**
```asm
; Macro definitions
.macro PRINT_CHAR char
    LOAD_IMM RAX, \char
    WRITE_PORT 0x03, RAX
.endmacro

; Macro usage
PRINT_CHAR 'H'
PRINT_CHAR 'i'
```

**🎯 Enhanced Symbol Support**
```asm
; Constants and labels
BUFFER_SIZE = 256           ; Numeric constant
MAX_USERS   = 100           ; Named constant

start:                      ; Code label
    LOAD_IMM RAX, BUFFER_SIZE
    LOAD_IMM RBX, string_data
    JMP main_loop

string_data:                ; Data label
    .string "Hello World"
```

#### Implementation Strategy

**Phase 3B.1: Core Missing Opcodes**
- Implement WRITE_PORT/READ_PORT with reversed syntax
- Add indexed addressing: LOAD/STORE [reg+offset]
- Implement MOD, EXP, ABS arithmetic operations

**Phase 3B.2: Advanced Arithmetic**
- MIN/MAX operations with multiple operands
- SQRT integer approximation
- Enhanced mathematical function library

**Phase 3B.3: String & Data Directives**
- .string directive for null-terminated strings
- .bytes, .word, .dword, .qword data definition
- .space and .align memory management

**Phase 3B.4: Advanced Addressing**
- Full x86-style addressing modes
- Scaled index calculations
- Complex memory access patterns

**Phase 3B.5: String Operations**
- String manipulation instruction set
- Built-in string handling functions
- Text processing capabilities

**Phase 3B.6: System Call Interface**
- SYSCALL instruction implementation
- Host OS integration layer
- Standard system call mapping (Linux/Windows/macOS)
- Security sandboxing for safe system access

---

### 🎯 **User-Requested Features: Modern Assembly Example Support**

**Target Example Program Support:**
```asm
; Hello World program for VirtComp architecture
; Uses serial port output (port 0x03)

.org 0x1000          ; Start at memory address 0x1000

start:
    LOAD_IMM RAX, string  ; Load string address into RAX
    LOAD_IMM RCX, 0       ; Initialize counter

print_loop:
    LOAD RBX, [RAX+RCX]   ; Load next character
    CMP RBX, 0            ; Check for null terminator
    JE exit
    WRITE_PORT 0x03, RBX  ; Write character to serial port (0x03)
    INC RCX
    JMP print_loop

exit:
    HALT

string:
    .string "Hello, World!\n" ; Null-terminated string
```

**Required Implementation Tasks:**

1. **Enhanced LOAD_IMM with Labels**
   - `LOAD_IMM RAX, string` - Load label address into register
   - Symbol table resolution during assembly
   - Address calculation and label linking

2. **Indexed Memory Addressing**
   - `LOAD RBX, [RAX+RCX]` - Base + index addressing mode
   - Enhanced parser for bracket notation
   - Runtime address calculation

3. **WRITE_PORT Instruction**
   - `WRITE_PORT 0x03, RBX` - Direct port output syntax
   - Alternative to existing OUT instruction
   - More intuitive syntax for assembly programmers

4. **String Literal Support**
   - `.string "Hello, World!\n"` - Null-terminated string directive
   - Escape sequence processing (\n, \t, \", \\, etc.)
   - Automatic null termination

5. **Additional Mathematical Opcodes**
   - `MOD <dst>, <src>` - Modulo operation (x % y)
   - `EXP <dst>, <src>` - Exponentiation (x^y)
   - `ABS <dst>, <src>` - Absolute value |x|
   - Enhanced arithmetic instruction set

**Implementation Priority Order:**
1. ✅ **WRITE_PORT instruction** - Critical for user's example
2. ✅ **Indexed addressing [reg+reg]** - Essential for string processing
3. ✅ **.string directive** - Required for string literals
4. ✅ **Enhanced LOAD_IMM with labels** - Symbol resolution
5. ✅ **MOD and EXP operations** - Extended arithmetic
6. ✅ **String handling utilities** - Complete string support

### 🖥️ **Phase 3C: Graphics & Display System** *(Q1 2026)*

**Priority: HIGH** | **Dependencies: Enhanced Instruction Set Complete**

Add visual output capabilities to make VirtComp programs engaging and enable graphics programming.

#### Graphics Device Implementation
- **Framebuffer Device**: Memory-mapped pixel buffer for graphics output
- **Resolution Support**: Multiple display modes (80x25 text, 320x240, 640x480 graphics)
- **Color System**: 8-bit color palette with RGB mapping
- **Graphics Primitives**: Pixel plotting, line drawing, rectangle fill

#### Graphics API
- **Assembly Instructions**: Graphics-specific operations and registers
- **Memory Mapping**: Direct framebuffer access patterns
- **Graphics Library**: Reusable assembly routines for common operations
- **Text Rendering**: Character display in graphics modes

#### Visual Examples
- **Pixel Art**: Simple graphics demonstrations
- **Animation**: Moving objects and visual effects
- **Games**: Simple games like Pong or Snake
- **Visualizations**: Data plotting and graphic displays

### � **Phase 3C: Advanced I/O & Networking** *(Q2 2026)*

**Priority: MEDIUM** | **Dependencies: Graphics System Complete**

Expand I/O capabilities for real-world integration and communication.

#### Enhanced I/O
- **Serial Communication**: UART device for external communication
- **Network Interface**: TCP/UDP socket simulation
- **Timer System**: Programmable intervals and real-time operations
- **Interrupt System**: Hardware interrupt simulation and handling

#### File System Improvements
- **Directory Operations**: Folder creation, listing, navigation
- **File Metadata**: Size, timestamps, permissions
- **Stream I/O**: Buffered file operations and seeking
- **Virtual File System**: Multiple mount points and device types

### 🚀 **Phase 3E: High-Level Language Frontend** *(Q3 2026)*

**Priority: VERY HIGH** | **Dependencies: All Previous Phases Complete**

Transform VirtComp into a true language platform by implementing a high-level language that compiles to VirtComp assembly.

#### Language Design
- **C-like Syntax**: Familiar syntax for wide adoption
- **Type System**: Static typing with automatic memory management
- **Function System**: Procedures, parameters, local variables, return values
- **Control Structures**: if/else, while, for, switch statements

#### Compiler Implementation
- **Lexer & Parser**: Language front-end processing
- **Symbol Tables**: Variable and function scope management
- **Code Generation**: High-level to VirtComp assembly translation
- **Optimization**: Basic optimizations for efficiency

#### Standard Library
- **Built-in Functions**: Math, string, I/O operations
- **Graphics Library**: High-level graphics programming interface
- **System Interface**: Device access and system operations
- **Runtime Support**: Memory management and error handling

### ⚡ **Phase 3F: Performance & Optimization** *(Q4 2026)*

**Priority: MEDIUM** | **Dependencies: Language Frontend Complete**

Optimize VirtComp for speed and efficiency once all major features are complete.

#### Performance Improvements
- **JIT Compilation**: Just-in-time compilation for hot code paths
- **Bytecode Optimization**: Instruction combining and dead code elimination
- **Memory Optimization**: Efficient memory allocation and garbage collection
- **Profiling Tools**: Performance analysis and bottleneck identification

#### Advanced Features
- **Multi-threading**: Parallel execution capabilities
- **Native Compilation**: Compile VirtComp programs to native machine code
- **Plugin System**: Extensible architecture for custom devices and features
- **Development Tools**: Advanced debugging and development environment

---

### �🏗️ **Phase 4: Virtual Storage System** *(Planned - Q1 2027)*

**Priority: MEDIUM** | **Dependencies: Phase 2 completion**

#### Storage Architecture
- **Virtual Hard Disk**: Persistent block-level storage
- **ROM Support**: Read-only memory for system firmware
- **Enhanced RAM**: Expanded memory addressing beyond 256 bytes
- **Storage Controller**: Unified interface for all storage devices

#### File System Implementation
- **Virtual File System**: Hierarchical directory structure
- **File Operations**: Create, read, write, delete, rename
- **Directory Management**: Path resolution and navigation
- **Permissions**: Basic file access control

#### Storage APIs
- **Device Drivers**: Abstracted storage device interfaces
- **Block Manager**: Efficient block allocation and management
- **Buffer System**: Caching for improved performance

---

### 🎨 **Phase 4: Enhanced Development Environment** *(Planned - Q2 2026)*

**Priority: MEDIUM** | **Dependencies: Phase 3 completion**

#### IDE Improvements
- **Syntax Highlighting**: Assembly language code coloring
- **IntelliSense**: Code completion and error detection
- **Project Management**: Multi-file project organization
- **Integrated Debugger**: Step-through debugging with breakpoints

#### Debugging Tools
- **Memory Viewer**: Real-time memory inspection and editing
- **Register Watch**: Monitor register changes during execution
- **Call Stack Viewer**: Function call hierarchy visualization
- **Performance Profiler**: Instruction timing and hotspot analysis

#### Development Utilities
- **Disassembler**: Convert binary back to assembly
- **Hex Editor**: Binary file inspection and modification
- **Device Monitor**: Real-time I/O traffic visualization
- **Test Runner**: Advanced test execution and reporting

---

## 🐛 Known Issues & Bug Fixes

### ✅ Recently Resolved Issues
- ✅ **All Unit Tests Fixed**: Resolved 7 failing tests to achieve 100% test coverage (53/53)
- ✅ **Stack Operations**: Fixed arg_offset initialization bug in CPU reset function
- ✅ **Conditional Jumps**: Corrected jump target addresses in complex program flow
- ✅ **Memory Load/Store**: Fixed memory layout positioning for subroutine calls
- ✅ **Shift Operations**: Corrected bit shift implementations and overflow handling
- ✅ **Flag System**: Resolved infinite loop in comprehensive flag testing
- ✅ **Context-Aware Operations**: Implemented standalone vs function call detection for PUSH_ARG/POP_ARG

### Current Technical Debt
- **GUI Auto-scroll**: Log output scrolling optimization needed in debug interface
- **Memory Bounds**: Enhanced memory access validation for edge cases
- **Performance**: Instruction execution speed benchmarking and optimization

---

## 🎯 Long-term Vision *(2026+)*

### Advanced Features
- **Networking**: Virtual network interfaces and TCP/IP stack
- **Graphics**: Basic framebuffer and sprite support
- **Audio**: Sound generation and music synthesis
- **Multitasking**: Process scheduling and memory protection
- **Compiler Backend**: High-level language compilation target

### Platform Expansion
- **Cross-Platform**: Enhanced Linux, Windows, macOS support
- **Web Assembly**: Browser-based VirtComp execution
- **Educational Tools**: Interactive tutorials and learning modules
- **Community**: Plugin system and third-party extensions

---

## 📊 Development Metrics

| Phase | Completion | Lines of Code | Test Coverage | Documentation |
|-------|------------|---------------|---------------|---------------|
| Core CPU | 100% | ~3,000 | 100% (53/53 unit tests) | Complete |
| Device System | 100% | ~1,500 | 100% (39/39 integration tests) | Complete |
| Testing Framework | 100% | ~900 | 100% | Complete |
| Extended Architecture | 100% | ~2,000 | 100% | Complete |
| **Total Current** | **100%** | **~7,400** | **100%** | **Complete** |

### Recent Achievements ✅
- ✅ **Phase 2.5 Complete**: Extended register architecture with 50 registers and dual x32/x64 mode support
- ✅ **Extended Register Operations**: Full MOVEX, ADDEX, SUBEX instruction set for R8-R15 access
- ✅ **Memory Expansion**: Increased from 256 bytes to 1MB (4,096x increase) with backward compatibility
- ✅ **UI Polish**: Professional output formatting with purple timestamps and colored log levels
- ✅ **Command-Line Interface**: Extended register display via -er/--extended-registers flag
- ✅ **100% Test Coverage**: All 53 unit tests and 39 integration tests continue to pass

### Next Priority Tasks 🚀
1. **Assembly Language Design**: Define human-readable instruction syntax with extended register support
2. **Parser Implementation**: Build two-pass assembler with 50-register name resolution
3. **Register Name System**: Implement RAX, RBX, RCX, RDX, R8-R15 parsing and validation
4. **Development Tools**: Create CLI assembler (virtasm) with x64-style syntax support
5. **Extended Register Integration**: Seamless assembly-to-bytecode for all 50 registers

---

## 🤝 Contributing

This roadmap is a living document. Updates and refinements are made as development progresses and new requirements emerge. For questions or suggestions, please refer to the project documentation or submit issues through the appropriate channels.

---

*This roadmap represents the current development vision and may be adjusted based on technical discoveries, user feedback, and resource availability.*
