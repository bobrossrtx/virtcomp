; Simple Addition Program
; This program adds two numbers and demonstrates basic arithmetic
; Shows register operations and data movement

main:
    ; Load first number (42) into R0
    LOAD_IMM R0, 42
    
    ; Load second number (13) into R1  
    LOAD_IMM R1, 13
    
    ; Add R0 + R1, result goes into R0
    ADD R0, R1
    
    ; Copy result to other registers for demonstration
    MOV R2, R0             ; Copy result to R2
    MOV R3, R0             ; Copy result to R3
    
    ; Perform additional operations on the result
    LOAD_IMM R4, 10
    SUB R2, R4             ; R2 = result - 10 = 55 - 10 = 45
    
    ADD R3, R4             ; R3 = result + 10 = 55 + 10 = 65
    
    ; End program
    HALT

; The main result (55) is in R0
; Modified results are in R2 (45) and R3 (65)
; Note: Memory STORE/LOAD operations are not yet implemented in assembler
