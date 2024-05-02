section .text
global _start
_start:

label:
        nop
        nop
        nop
        mov rax, rbx
        nop
        mov rax, rcx
        nop
        mov rcx, r11
        nop
        nop
        nop
        mov rax, 1
        nop
        mov rcx, 3
        nop
        mov r11, 5
        nop
        nop
        nop
        cmp rax, rcx
        jmp label


section .data
BUF_PTR db 50 dup 0 

