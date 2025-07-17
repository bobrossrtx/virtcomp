; Simple Number Output
; Output the digit 5

main:
    LOAD_IMM R0, 5      ; Load the number 5
    LOAD_IMM R1, 48     ; Load ASCII '0' value
    ADD R0, R1          ; Convert to ASCII ('0' + 5 = '5')
    OUT R0, 1           ; Output '5'

    LOAD_IMM R0, 10     ; Newline
    OUT R0, 1

    HALT