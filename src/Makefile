#--------------------------------------------------------------------------------
#
# Makefile for the PCAT AST generation
#
#--------------------------------------------------------------------------------

GCC = gcc
CFLAGS = -g -std=gnu99
YACC = bison
LEX = flex

all: parser

parser: main.c y.tab.c lex.yy.c ast.h ast.o
	$(GCC) $(CFLAGS) lex.yy.c y.tab.c main.c ast.o -o parser

ast.o:  ast.c ast.h
	$(GCC) $(CFLAGS) -c ast.c

y.tab.c: pcat.y
	$(YACC) -y -d pcat.y

lex.yy.c: pcat.lex
	   $(LEX) pcat.lex

clean:
	/bin/rm -f *.o *~ pcat.yy.c pcat.c pcat.output y.tab.* ast.o lex.yy.c parser core
