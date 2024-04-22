cd Processor/
cd asm
make
./asm ../../examples/$1.asm ../../binary.bin
cd ..
cd spu
make
./spu ../../binary.bin