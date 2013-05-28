#!/bin/bash
if [ -f "./parser" ]; then
./parser $1
else
make all
./parser $1
fi
rm data.s code.s
gcc -g -m32 -c sup.s
gcc -g -m32 -c pcat.s
gcc -g -m32 sup.o pcat.o -o test.o
#rm pcat.s
#make clean

