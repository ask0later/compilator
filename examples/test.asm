jmp fun_1
fun_0:
push 5
pop [0]
push [0]
ret
fun_1:
push 2
pop [1]
push 3
pop [2]
call fun_0
pop [2]
push [1]
out
