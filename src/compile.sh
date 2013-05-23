#!/bin/bash

gcc -m32 -c sup.c
gcc -m32 -c pcat.s
gcc -m32 sup.o pcat.o -o exe
