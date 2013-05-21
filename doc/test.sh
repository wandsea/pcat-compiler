gcc -c f.s
gcc -c g.c
gcc -c main.c
gcc main.o f.o g.o -o main
