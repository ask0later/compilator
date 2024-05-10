cd front_end/
./lan ../examples/$1.txt ../examples/tree_$1.txt
cd ..
cd back_end
./ir ../examples/tree_$1.txt nasm_file.asm ../hex_dump/dump_$1.txt
cd ..
cd ELF_translator
./trans ../data_and_text/text_segment.txt ../data_and_text/data_segment.txt ../$1
cd ..
chmod +x $1