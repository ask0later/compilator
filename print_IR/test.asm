section     .code
global _start


section     .data
        RAM_PTR: dq 50 dup 0
        STACK_PTR: dq 25 dup 0

_start:
        nop
        nop
        nop
        call func_0     ; E8 11 00 00 00
        nop

        jne end_if_0    ; 75 0C - offset
        nop
        je end_if_0     ; 74 09
        nop
        jbe end_if_0    ; 76 06
        nop
        jae end_if_0    ; 73 03
        nop
        ret             ; C3
        nop
        nop
        nop
        