cd back_end
nasm -f elf64 nasm_file.asm 
ld -o nasm_file nasm_file.o
./nasm_file
cd ..