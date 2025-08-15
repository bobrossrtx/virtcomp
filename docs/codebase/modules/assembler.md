# Assembly Language System

**Files:** `src/assembler/` directory  
**Purpose:** Complete assembly language toolchain for DemiEngine

## Overview

The assembly system provides a complete toolchain for converting human-readable assembly language into executable bytecode. It consists of four main components working in sequence: Lexer → Parser → Assembler → Bytecode Generator.

## Architecture

### Pipeline Flow

```
Source Code (.asm) → Lexer → Tokens → Parser → AST → Assembler → Bytecode
```

## Components

### 1. Lexer (`lexer.hpp`, `lexer.cpp`)

**Purpose:** Converts raw assembly text into structured tokens

#### Token Types
```cpp
enum class TokenType {
    IDENTIFIER,     // Labels, mnemonics, symbols
    REGISTER,       // R0, R1, R2, etc.
    IMMEDIATE,      // Numeric values
    LABEL_DEF,      // Labels ending with ':'
    NEWLINE,        // Line separators
    EOF_TOKEN,      // End of file
    COMMENT,        // ; comment text
    COMMA,          // Argument separator
    STRING,         // Quoted strings
    DIRECTIVE       // Assembly directives
};
```

#### Key Features
- **Tokenization:** Breaks source into meaningful units
- **Comment Handling:** Strips ; comments automatically
- **Number Parsing:** Supports decimal, hex (0x), binary (0b)
- **Register Recognition:** Automatic R0-R49 register parsing
- **Error Recovery:** Continues parsing after lexical errors

#### Usage Example
```cpp
Lexer lexer("LOAD_IMM R0, 42\nADD R0, R1");
while (lexer.has_next()) {
    Token token = lexer.next_token();
    // Process token
}
```

### 2. Parser (`parser.hpp`, `parser.cpp`)

**Purpose:** Converts token stream into Abstract Syntax Tree (AST)

#### AST Node Types
```cpp
struct Instruction {
    std::string mnemonic;           // e.g., "ADD", "LOAD_IMM"
    std::vector<Operand> operands;  // Instruction arguments
    uint32_t line_number;           // Source line for errors
};

struct Label {
    std::string name;               // Label identifier
    uint32_t line_number;           // Definition location
};

struct Program {
    std::vector<Instruction> instructions;
    std::vector<Label> labels;
    std::vector<std::string> errors;
};
```

#### Parsing Features
- **Two-Pass Design:** Symbol collection and instruction parsing
- **Error Recovery:** Continues after syntax errors
- **Label Resolution:** Forward and backward label references
- **Operand Validation:** Type checking for instruction arguments

#### Grammar Support
```
program     := (instruction | label | directive)*
instruction := MNEMONIC operand_list?
operand_list := operand (',' operand)*
operand     := register | immediate | identifier
register    := 'R' [0-9]+
immediate   := number
label       := IDENTIFIER ':'
```

### 3. Assembler Engine (`assembler.hpp`, `assembler.cpp`)

**Purpose:** Converts AST into executable bytecode

#### Symbol Management
```cpp
struct Symbol {
    std::string name;    // Symbol identifier
    uint32_t address;    // Memory location
    bool defined;        // Definition status
};
```

#### Two-Pass Assembly

**Pass 1: Symbol Collection**
- Scans all labels and calculates addresses
- Builds symbol table
- Determines instruction sizes

**Pass 2: Code Generation**
- Resolves all symbol references
- Generates final bytecode
- Handles forward references

#### Instruction Encoding

The assembler maps assembly mnemonics to CPU opcodes:

```cpp
std::unordered_map<std::string, uint8_t> mnemonic_to_opcode = {
    {"NOP",       0x00},    {"LOAD_IMM",  0x01},
    {"ADD",       0x02},    {"SUB",       0x03},
    {"MOV",       0x04},    {"JMP",       0x05},
    {"LOAD",      0x06},    {"STORE",     0x07},
    {"PUSH",      0x08},    {"POP",       0x09},
    {"CMP",       0x0A},    {"JZ",        0x0B},
    {"JNZ",       0x0C},    {"JS",        0x0D},
    // ... complete instruction set
};
```

#### Bytecode Generation
- **Immediate Values:** 1-byte for LOAD_IMM, 4-byte for addresses
- **Register Encoding:** R0-R49 mapped to 0x00-0x31
- **Address Resolution:** Labels converted to memory addresses
- **Size Calculation:** Accurate instruction size for jumps

### 4. DemiEngine Assembler Interface (`demi-engine_assembler.hpp`)

**Purpose:** High-level interface for assembly operations

```cpp
namespace DemiEngineAssembler {
    std::vector<uint8_t> assemble_file(const std::string& filename);
    std::vector<uint8_t> assemble_string(const std::string& source);
    void save_bytecode(const std::vector<uint8_t>& bytecode, 
                       const std::string& filename);
}
```

## Instruction Support

### Complete Instruction Set
- ✅ **Arithmetic:** ADD, SUB, MUL, DIV, INC, DEC
- ✅ **Logic:** AND, OR, XOR, NOT, SHL, SHR
- ✅ **Memory:** LOAD_IMM, MOV, LOAD, STORE, LEA
- ✅ **Control:** JMP, JZ, JNZ, JG, JL, JGE, JLE, JS, JNS, JC, JNC, JO, JNO
- ✅ **Stack:** PUSH, POP, PUSHF, POPF
- ✅ **I/O:** IN, OUT
- ✅ **System:** CMP, NOP, HALT, SWAP
- ✅ **Extended:** 64-bit variants (ADD64, SUB64, MUL64, DIV64)

### Addressing Modes
- **Register Direct:** `ADD R0, R1`
- **Immediate:** `LOAD_IMM R0, 42`
- **Label/Symbol:** `JMP main_loop`
- **Memory Direct:** `LOAD R0, 0x1000`

## Error Handling

### Error Types
- **Lexical Errors:** Invalid characters, malformed numbers
- **Syntax Errors:** Invalid instruction format, missing operands
- **Semantic Errors:** Undefined labels, invalid registers
- **Assembly Errors:** Address out of range, symbol conflicts

### Error Reporting
```cpp
class AssemblerEngine {
    const std::vector<std::string>& get_errors() const;
    bool has_errors() const;
};
```

Errors include:
- Line number references
- Descriptive error messages
- Context information
- Recovery suggestions

## Assembly Language Syntax

### Basic Format
```assembly
; Comments start with semicolon
label_name:              ; Label definition
    LOAD_IMM R0, 42     ; Load immediate value
    ADD R0, R1          ; Add registers
    JMP label_name      ; Jump to label
    HALT                ; Terminate program
```

### Register Naming
- Standard: `R0`, `R1`, `R2`, ..., `R7`
- Extended: `R8`, `R9`, ..., `R49`
- Case sensitive (must be uppercase)

### Number Formats
- Decimal: `42`, `123`
- Hexadecimal: `0x2A`, `0xFF`
- Binary: `0b101010`

### String Literals
```assembly
; Character output
LOAD_IMM R0, 'H'        ; ASCII character
OUT 1, R0               ; Output to console
```

## Integration Points

### Main Application
```cpp
// In main.cpp
if (Config::assembly_mode) {
    auto bytecode = DemiEngineAssembler::assemble_file(filename);
    cpu.run(bytecode);
}
```

### Build System
The assembler compiles into the main DemiEngine executable and is available via the `-A` flag:

```bash
./bin/demi-engine -A program.asm
```

## Usage Examples

### Simple Program
```assembly
; hello_world.asm
main:
    LOAD_IMM R0, 72     ; 'H'
    OUT 1, R0
    LOAD_IMM R0, 101    ; 'e'
    OUT 1, R0
    HALT
```

### With Control Flow
```assembly
; counting_loop.asm
start:
    LOAD_IMM R0, 1      ; Counter
    LOAD_IMM R1, 5      ; Limit

loop:
    OUT 1, R0           ; Output counter
    INC R0              ; Increment
    CMP R0, R1          ; Compare with limit
    JLE loop            ; Continue if less or equal
    HALT
```

### Error Handling Example
```assembly
; This will generate errors:
invalid_instruction R0, R1    ; Unknown mnemonic
ADD R99, R0                   ; Invalid register
JMP undefined_label           ; Undefined symbol
```

The assembly system provides a complete, robust toolchain for DemiEngine development, supporting educational use cases while maintaining professional-grade error handling and debugging capabilities.
