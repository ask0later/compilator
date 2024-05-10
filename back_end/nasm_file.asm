section .text
global _start
_start:
mov r10, RAM_PTR
 func_1 :
mov r10 , rsp 
sub r10 , 8 
call my_input 
push rax 
push r10 
push 0 
sub r10 , 0 
push  QWORD [r10] 
add r10 , 0 
add rsp , 16 
call func_0 
pop r8 
pop r10 
push r8 
sub r10 , 8 
pop  QWORD [r10] 
add r10 , 8 
sub rsp , 8 
sub r10 , 8 
push  QWORD [r10] 
add r10 , 8 
pop rax 
call my_output 
mov rsp , r10 
push 60 
pop rax 
push 0 
pop rdi 
syscall 
 func_0 :
mov r10 , rsp 
sub r10 , 8 
sub rsp , 8 
sub r10 , 0 
push  QWORD [r10] 
add r10 , 0 
sub r10 , 8 
pop  QWORD [r10] 
add r10 , 8 
sub rsp , 8 
 .while_0 :
sub r10 , 0 
push  QWORD [r10] 
add r10 , 0 
push 1 
pop r11 
pop r12 
cmp r11 , r12 
je .end_while_0 
sub r10 , 0 
push  QWORD [r10] 
add r10 , 0 
push 1 
pop r12 
pop r11 
sub r11 , r12 
push r11 
sub r10 , 0 
pop  QWORD [r10] 
add r10 , 0 
sub r10 , 0 
push  QWORD [r10] 
add r10 , 0 
sub r10 , 8 
push  QWORD [r10] 
add r10 , 8 
pop r12 
pop r11 
mov rax , r11 
mul r12 
push rax 
sub r10 , 8 
pop  QWORD [r10] 
add r10 , 8 
jmp .while_0 
 .end_while_0 :
sub r10 , 8 
push  QWORD [r10] 
add r10 , 8 
pop r8 
mov rsp , r10 
add rsp , 8 
pop r9 
push r8 
push r9 
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
my_output:
push rax
push rdi
push rdx
push rsi
push r11
mov rsi, 10
mov r11, 49
mov dl, 0xa
mov byte [BUF_PTR + r11], dl
dec r11
.loop:
xor rdx, rdx
div rsi
add rdx, '0'
mov byte [BUF_PTR + r11], dl
dec r11
cmp rax, 0
jne .loop
mov rax, 1
mov rdi, 1
mov rdx, 50
sub rdx, r11
inc r11
mov rsi, BUF_PTR
add rsi, r11
syscall
pop r11
pop rsi
pop rdx
pop rdi
pop rax
ret
section .data
BUF_PTR db 50 dup 0 
RAM_PTR db 300 dup 0 
