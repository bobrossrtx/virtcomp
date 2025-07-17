; Simple jump test
; Test the basic jump instructions individually

main:
    load_imm R0, 5
    load_imm R1, 10
    
    ; Test JL (less than)
    cmp R0, R1          ; 5 < 10, should be true
    jl test_passed      ; Should jump
    
    ; This should not execute
    load_imm R7, 99
    halt
    
test_passed:
    ; Output success indicator
    load_imm R2, 80     ; ASCII 'P' for Pass
    out R2, 1
    
    halt
