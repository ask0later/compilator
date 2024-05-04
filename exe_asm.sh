cd backend
nasm -f elf64 nasm_file.asm 
ld -o test nasm_file.o
./nasm_file
cd ..