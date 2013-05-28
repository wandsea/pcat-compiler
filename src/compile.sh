#!/bin/bash
if [ -f "./parser" ]; then
./parser $1 > $1.log
else
make all
./parser $1 > $1.log
fi
rm data.s code.s
gcc -g -m32 -c sup.c
gcc -g -m32 -c pcat.s
gcc -g -m32 sup.o pcat.o -o $1.o
#rm pcat.s
#make clean

