; Core Instructions Demo
; This program demonstrates the basic instructions that are currently
; implemented in the Demi Engine assembler

main:
    ; Basic immediate loading
    LOAD_IMM R0, 100
    LOAD_IMM R1, 50
    LOAD_IMM R2, 25

    ; Arithmetic operations
    ADD R0, R1             ; R0 = 100 + 50 = 150
    SUB R1, R2             ; R1 = 50 - 25 = 25
    MUL R2, R2             ; R2 = 25 * 25 = 625

    ; Data movement
    MOV R3, R0             ; Copy R0 to R3
    MOV R4, R1             ; Copy R1 to R4
    MOV R5, R2             ; Copy R2 to R5

    ; Comparison (sets flags but doesn't jump yet)
    CMP R0, R1             ; Compare 150 vs 25

    ; Logical operations
    LOAD_IMM R6, 0xFF
    LOAD_IMM R7, 0x0F
    AND R6, R7             ; R6 = 0xFF & 0x0F = 0x0F

    OR R6, R0              ; R6 = 0x0F | 150 = bitwise OR
    XOR R7, R1             ; R7 = 0x0F ^ 25 = bitwise XOR

    ; Stack operations
    PUSH R0                ; Push 150 onto stack
    PUSH R1                ; Push 25 onto stack
    PUSH R2                ; Push 625 onto stack

    ; Pop in reverse order
    POP R3                 ; R3 = 625
    POP R4                 ; R4 = 25
    POP R5                 ; R5 = 150

    ; Increment/Decrement
    INC R0                 ; R0 = 151
    DEC R1                 ; R1 = 24

    ; Bitwise NOT
    NOT R7                 ; R7 = ~R7

    ; End program
    HALT

; This program demonstrates all the core instructions that are
; currently implemented in the assembler:
; - LOAD_IMM: Load immediate values
; - ADD, SUB, MUL, DIV: Arithmetic operations
; - MOV: Data movement between registers
; - CMP: Comparison (sets flags)
; - AND, OR, XOR: Logical operations
; - PUSH, POP: Stack operations
; - INC, DEC: Increment/Decrement
; - NOT: Bitwise complement
; - HALT: End program
