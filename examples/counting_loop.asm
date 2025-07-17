; Simple Loop Example
; This program demonstrates a counting loop
; Counts from 1 to 5 and outputs each number

main:
    ; Initialize counter in R0
    load_imm R0, 1     ; Start from 1
    load_imm R1, 5     ; Loop limit
    
loop_start:
    ; Output current counter value (will show as ASCII character)
    ; Add 48 to convert number to ASCII digit
    mov R2, R0         ; Copy counter to R2
    load_imm R3, 48    ; ASCII offset for '0'
    add R2, R3         ; Convert to ASCII digit
    out R2, 1          ; Output to console
    
    ; Output a space
    load_imm R2, 32    ; ASCII space
    out R2, 1
    
    ; Increment counter
    inc R0
    
    ; Check if we've reached the limit
    cmp R0, R1
    jle loop_start     ; Jump if R0 <= R1
    
    ; Output newline at end
    load_imm R0, 10    ; ASCII newline
    out R0, 1
    
    ; End program
    halt

; Expected output: "1 2 3 4 5 " followed by newline
