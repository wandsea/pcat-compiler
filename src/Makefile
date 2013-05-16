#--------------------------------------------------------------------------------
#
# Makefile for the PCAT AST generation
#
#--------------------------------------------------------------------------------

GCC = gcc
CFLAGS = -g -std=gnu99 -Wall -Wno-unused-function
YACC = bison
YFLAGS= -d -t -y
LEX = flex

all: parser

parser: main.c y.tab.c lex.yy.c ast.h ast.o
	$(GCC) $(CFLAGS) lex.yy.c y.tab.c main.c ast.o -o parser

ast.o:  ast.c ast.h
	$(GCC) $(CFLAGS) -c ast.c

y.tab.c y.tab.h: pcat.y
	$(YACC) $(YFLAGS) pcat.y

lex.yy.c: pcat.lex
	$(LEX) pcat.lex
    
table.o: table.c
	$(GCC) $(CFLAGS) -c table.c
    

clean:
	/bin/rm -f *.o *~ *.stackdump pcat.yy.c pcat.c pcat.output y.tab.* ast.o lex.yy.c parser core
