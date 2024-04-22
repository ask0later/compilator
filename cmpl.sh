cd front/
./lan ../examples/$1.txt ../examples/tree.txt
cd .. && cd middle
./mid ../examples/tree.txt
cd .. && cd back
./back ../examples/tree.txt ../examples/$1.asm
cd ..