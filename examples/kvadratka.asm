jmp fun_4
fun_0:
pop [0]
pop [1]
push 0
push [1]
push [0]
div 
sub 
pop [2]
push [2]
ret
fun_1:
pop [3]
pop [4]
pop [5]
push [4]
push [4]
mul 
push 4
push [3]
mul 
push [5]
mul 
sub 
pop [6]
push [6]
ret
fun_2:
pop [7]
pop [8]
pop [9]
push 0
push [8]
sub 
push [9]
sqrt 
add 
push 2
push [7]
mul 
div 
pop [10]
push [10]
ret
fun_3:
pop [11]
pop [12]
pop [13]
push 0
push [12]
sub 
push [13]
sqrt 
sub 
push 2
push [11]
mul 
div 
pop [14]
push [14]
ret
fun_4:
in
pop [15]
in
pop [16]
in
pop [17]
push [15]
push 0
jne end_if_0
push [16]
push 0
jne end_if_1
push [17]
push 0
jne end_if_2
push 111
pop [18]
push [18]
out
end_if_2:
push [17]
push 0
je end_if_3
push 222
pop [18]
push [18]
out
end_if_3:
end_if_1:
push [16]
push 0
je end_if_4
push [17]
push [16]
call fun_0
pop [18]
push [18]
out
end_if_4:
end_if_0:
push [15]
push 0
je end_if_5
push [17]
push [16]
push [15]
call fun_1
pop [19]
push [19]
push 0
jne end_if_6
push 0
push [16]
sub 
push 2
push [15]
mul 
div 
pop [18]
push [18]
out
end_if_6:
push [19]
push 0
jbe end_if_7
push [19]
push [16]
push [15]
call fun_2
pop [18]
push [18]
out
push [19]
push [16]
push [15]
call fun_3
pop [18]
push [18]
out
end_if_7:
push [19]
push 0
jae end_if_8
push 222
pop [18]
push [18]
out
end_if_8:
end_if_5:
