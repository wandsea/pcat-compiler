#!/bin/bash

gcc -g -m32 -c sup.c
gcc -g -m32 -c pcat.s
gcc -g -m32 sup.o pcat.o -o exe
