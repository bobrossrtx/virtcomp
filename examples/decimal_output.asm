; Decimal Output Example
; This program demonstrates how to output numbers as decimal digits
; Shows the number 123 as individual decimal digits

main:
    ; We want to output the number 123
    LOAD_IMM R0, 123

    ; Extract hundreds digit: 123 / 100 = 1
    LOAD_IMM R1, 100
    MOV R2, R0          ; Copy original number
    DIV R2, R1          ; R2 = 123 / 100 = 1
    LOAD_IMM R5, 48     ; ASCII '0'
    ADD R2, R5          ; Convert to ASCII ('0' + 1 = '1')
    OUT R2, 1           ; Output '1'

    ; Calculate remainder: 123 - (1 * 100) = 23
    MOV R3, R2          ; Copy quotient (1 + 48 = 49)
    SUB R3, R5          ; Convert back to number (49 - 48 = 1)
    MUL R3, R1          ; 1 * 100 = 100
    MOV R4, R0          ; Copy original (123)
    SUB R4, R3          ; 123 - 100 = 23

    ; Extract tens digit: 23 / 10 = 2
    LOAD_IMM R1, 10
    MOV R2, R4          ; Copy remainder (23)
    DIV R2, R1          ; R2 = 23 / 10 = 2
    ADD R2, R5          ; Convert to ASCII
    OUT R2, 1           ; Output '2'

    ; Calculate final remainder: 23 - (2 * 10) = 3
    MOV R3, R2          ; Copy quotient (2 + 48 = 50)
    SUB R3, R5          ; Convert back to number (50 - 48 = 2)
    MUL R3, R1          ; 2 * 10 = 20
    SUB R4, R3          ; 23 - 20 = 3

    ; Output ones digit
    ADD R4, R5          ; Convert to ASCII
    OUT R4, 1           ; Output '3'

    ; Output newline
    LOAD_IMM R0, 10
    OUT R0, 1

    HALT

; This demonstrates manual modulo using subtraction
; Shows how to convert numbers to readable decimal output
