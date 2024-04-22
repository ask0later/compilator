jmp fun_1
fun_0:
pop [0]
push [0]
push 0
jne end_if_0
push 1
ret
end_if_0:
push [0]
push 1
jne end_if_1
push 1
ret
end_if_1:
push [0]
push [0]
push 1
sub 
call fun_0
mul 
ret
fun_1:
in
pop [1]
push [1]
call fun_0
pop [2]
push [2]
out
