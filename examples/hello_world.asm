; Hello World Assembly Example
; This program prints "Hello World!" to the console using OUT instructions

main:
    ; Print "Hello World!"
    LOAD_IMM R0, 72        ; ASCII 'H'
    OUT R0, 1              ; Output to console

    LOAD_IMM R0, 101       ; ASCII 'e'
    OUT R0, 1

    LOAD_IMM R0, 108       ; ASCII 'l'
    OUT R0, 1

    LOAD_IMM R0, 108       ; ASCII 'l'
    OUT R0, 1

    LOAD_IMM R0, 111       ; ASCII 'o'
    OUT R0, 1

    LOAD_IMM R0, 32        ; ASCII ' ' (space)
    OUT R0, 1

    LOAD_IMM R0, 87        ; ASCII 'W'
    OUT R0, 1

    LOAD_IMM R0, 111       ; ASCII 'o'
    OUT R0, 1

    LOAD_IMM R0, 114       ; ASCII 'r'
    OUT R0, 1

    LOAD_IMM R0, 108       ; ASCII 'l'
    OUT R0, 1

    LOAD_IMM R0, 100       ; ASCII 'd'
    OUT R0, 1

    LOAD_IMM R0, 33        ; ASCII '!'
    OUT R0, 1

    LOAD_IMM R0, 10        ; ASCII '\n' (newline)
    OUT R0, 1

    ; End program
    HALT
