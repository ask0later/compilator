cd backend
./ir ../examples/tree_factorial.txt nasm_file.asm
cd ..
cd ELF_translator
./trans ../examples/text_segment.txt ../examples/data_segment.txt
cd ..
