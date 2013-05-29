#--------------------------------------------------------------------------------
#
# Makefile for the PCAT AST generation
#
#--------------------------------------------------------------------------------

GCC = gcc
CFLAGS = -g -std=gnu99 -m32 -Wall -Wno-unused-function 
YACC = bison
YFLAGS= -d -t -y
LEX = flex

ACTIONS = action.o type.o table.o gen.o global.o

all: parser

parser: main.c y.tab.c lex.yy.c ast.h ast.o $(ACTIONS)
	$(GCC) $(CFLAGS) lex.yy.c y.tab.c main.c ast.o $(ACTIONS) -o parser

ast.o:  ast.c ast.h
	$(GCC) $(CFLAGS) -c ast.c

y.tab.c y.tab.h: pcat.y
	$(YACC) $(YFLAGS) pcat.y

lex.yy.c: pcat.lex
	$(LEX) pcat.lex
    
action.o: action.c action.h
	$(GCC) $(CFLAGS) -c action.c
    
table.o: table.c table.h
	$(GCC) $(CFLAGS) -c table.c

type.o: type.c type.h
	$(GCC) $(CFLAGS) -c type.c

gen.o: gen.c gen.h
	$(GCC) $(CFLAGS) -c gen.c
    
global.o: global.c global.h
	$(GCC) $(CFLAGS) -c global.c
   

clean:
	/bin/rm -f *.o *~ *.exe *.stackdump pcat.yy.c pcat.c pcat.output y.tab.* ast.o lex.yy.c parser core data.s code.s pcat.s
