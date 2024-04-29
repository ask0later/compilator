section     .text
global _start
_start:         
                call my_input

                call my_output

                mov rax, 0x3c
                mov rdi, 1
                syscall


my_output:
                push rax
                push rdi
                push rdx
                push rsi
                push r11
                
                mov rsi, 10
                mov r11, 49

                .loop:
                        xor rdx, rdx
                        div rsi
                        add rdx, '0'

                        mov BYTE [BUF_PTR + r11], dl
                        dec r11
                        cmp rax, 0
                        jne .loop

                mov rax, 0x01           
                mov rdi, 1  
                mov rdx, 50
                sub rdx, r11
                mov rsi, BUF_PTR
                add rsi, r11
                syscall

                pop r11
                pop rsi
                pop rdx
                pop rdi
                pop rax
                
                ret

my_input:       
                push rdi
                push rsi
                push rdx
                push r11
                push r10

                mov rax, 0
                mov rdi, 1
                mov rsi, BUF_PTR 
                mov rdx, 50
                syscall

                dec rax
                mov rdi, rax

                xor rsi, rsi
                xor rax, rax
                mov r10, 10

                .loop:
                        xor r11, r11
                        mov r11b, byte [BUF_PTR + rsi]
                        inc rsi
                        sub r11b, '0'
                        mul r10
                        add rax, r11
                        
                        dec rdi
                        cmp rdi, 0
                        jne .loop

                pop r10
                pop r11
                pop rdx
                pop rsi
                pop rdi

                ret


section     .data
BUF_PTR:        db 50 dup 0

; nasm -f elf64 in_out_lib.asm  ;  ld -o test in_out_lib.o