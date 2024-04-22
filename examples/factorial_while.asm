jmp fun_1
fun_0:
pop [0]
push 1
pop [1]
push 1
pop [2]
while_0:
push [2]
push [0]
jae end_while_0
push [2]
push 1
add 
pop [2]
push [1]
push [2]
mul 
pop [1]
jmp while_0
end_while_0:
push [1]
ret
fun_1:
in
pop [3]
push [3]
call fun_0
pop [4]
push [4]
out
