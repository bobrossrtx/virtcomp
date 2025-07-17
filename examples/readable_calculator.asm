; Improved Calculator Example
; This program performs arithmetic and outputs readable results
; Demonstrates: 5 + 3 = 8

main:
    ; Define two single-digit numbers for simplicity
    LOAD_IMM R0, 5     ; First number
    LOAD_IMM R1, 3     ; Second number

    ; Output first number
    LOAD_IMM R2, 48    ; ASCII '0'
    ADD R0, R2         ; Convert to ASCII
    OUT R0, 1          ; Output '5'

    ; Output " + "
    LOAD_IMM R0, 32    ; Space
    OUT R0, 1
    LOAD_IMM R0, 43    ; '+'
    OUT R0, 1
    LOAD_IMM R0, 32    ; Space
    OUT R0, 1

    ; Output second number
    MOV R0, R1         ; Get second number (3)
    ADD R0, R2         ; Convert to ASCII
    OUT R0, 1          ; Output '3'

    ; Output " = "
    LOAD_IMM R0, 32    ; Space
    OUT R0, 1
    LOAD_IMM R0, 61    ; '='
    OUT R0, 1
    LOAD_IMM R0, 32    ; Space
    OUT R0, 1

    ; Calculate and output result
    LOAD_IMM R0, 5     ; Reload first number
    LOAD_IMM R1, 3     ; Reload second number
    ADD R0, R1         ; 5 + 3 = 8
    ADD R0, R2         ; Convert to ASCII
    OUT R0, 1          ; Output '8'

    ; Output newline
    LOAD_IMM R0, 10
    OUT R0, 1

    HALT

; This demonstrates readable arithmetic output
; Result: "5 + 3 = 8"
