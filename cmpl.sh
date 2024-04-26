cd front/
./lan ../examples/$1.txt ../examples/tree_$1.txt
cd .. && cd middle
cd .. && cd back
./back ../examples/tree_$1.txt ../examples/$1.asm
cd ..