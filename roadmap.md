> **Important:**
> This documentation was generated with AI assistance. While formatting is clear, some data may be inaccurate or incomplete. Please update and proofread as needed.

---

### Planned Opcode Additions

#### Stack & Call Operations *(Complete; further improvements possible)*

- **CALL**: Pushes return address onto stack and jumps to subroutine.
- **RET**: Pops return address from stack and returns.
- **PUSH_ARG**: Pushes argument onto stack.
- **POP_ARG**: Pops argument from stack.

#### Input/Output Operations *(Complete; virtual I/O, e.g., serial ports)*

- **IN**: Input data from port/device.
- **OUT**: Output data to port/device.
- **INB / OUTB**: Input/output byte.
- **INW / OUTW**: Input/output word.
- **INL / OUTL**: Input/output long.
- **INSTR / OUTSTR**: Input/output string.

#### Flags & Status Operations

- **PUSHF**: Pushes flags register onto stack.
- **POPF**: Pops flags register from stack.

#### Memory Operations

- **LEA**: Loads effective address (pointer arithmetic).
- **SWAP**: Swaps values of two registers.

#### Conditional Jumps (for finer control)

- **JC / JNC**: Jump if carry / no carry.
- **JO / JNO**: Jump if overflow / no overflow.
- **JG / JL / JGE / JLE**: Jump if greater, less, greater-or-equal, less-or-equal (signed/unsigned).

---

### Virtual Storage *(Planned)*

- Support for **RAM**, **ROM**, **VHDD**.
- Virtual storage controller, bus, interface, driver, manager.
- Virtual storage API.
- Virtual files and directories.

---

### GUI Issues

- Log output auto-scrolling malfunctioning.
- Debug GUI: Programs not executing properly.

---

> **Note:**
> Please enhance and update the database documentation as needed.
