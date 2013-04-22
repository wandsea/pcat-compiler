#--------------------------------------------------------------------------------
#
# Makefile for the PCAT scanner
#
#--------------------------------------------------------------------------------

# The GC directory (the garbage collector)
GCDIR = /usr/local/lib

GCC = g++
CXXFLAGS = -g 
YACC = bison
LEX = flex
INCLUDE = util.h basic.h basic.templates

#lexer:	main.cc basic.o pcat.yy.c
#	$(GCC) $(CXXFLAGS) main.cc basic.o /usr/local/lib/libgc.a -o lexer

lexer:	main.cc pcat.yy.c
	$(GCC) $(CXXFLAGS) main.cc -o lexer

#basic.o: basic.cc $(INCLUDE)
#	 $(GCC) $(CXXFLAGS) -c basic.cc

pcat.yy.c: pcat.lex
	   $(LEX) pcat.lex
	   mv lex.yy.c pcat.yy.c

clean: 
	/bin/rm -f *.o *~ pcat.yy.c lexer core
