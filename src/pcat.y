%{
#include "y.tab.h"
#include "ast.h"

extern int lineno;
extern char *yytext;

/* parse error */
void yyerror ( char* s ) {
  printf("*** %s (line : %d, token: %s)\n",
         s, lineno, yytext);
};

%}

%union {
        char*                  Tstring;
        struct ast*            Tast;
        struct ast_list*       Tast_list;
}

%token IDENTIFIER INTEGERT REALT STRINGT
       PROGRAM IS BEGINT END VAR TYPE PROCEDURE ARRAY RECORD
       IN OUT READ WRITE IF THEN ELSE ELSIF WHILE DO LOOP
       FOR EXIT RETURN TO BY AND OR NOT OF DIV MOD
       LPAREN  RPAREN LBRACKET RBRACKET LBRACE RBRACE COLON DOT
       SEMICOLON COMMA ASSIGN PLUS MINUS STAR SLASH BACKSLASH EQ
       NEQ LT LE GT GE LABRACKET RABRACKET EOFF ERROR

%type <Tast> id
%type <Tast> program

%%

start:		program	EOFF	{ print_ast($1);}
;

program: 	id ASSIGN id SEMICOLON	{ $$ = mk_node(assign_exp,cons($1,cons($3,NULL)));} 
;

id: 		 IDENTIFIER   	{ $$ = mk_var(yytext); }
		|INTEGERT	{ $$ = mk_int(atoi(yytext)); }
		|REALT		{ $$ = mk_real(atof(yytext)); }
		|STRINGT	{ $$ = mk_str(yytext); }
;

%%
