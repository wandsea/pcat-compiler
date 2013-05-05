%{
#include "y.tab.h"
#include "ast.h"

extern int lineno;
extern char *yytext;

/* parse error */
void yyerror ( char* s ) {
  printf("\n*** %s (line : %d, token: %s)\n",
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
//id ASSIGN id SEMICOLON  { $$ = mk_node(assign_exp,cons($1,cons($3,NULL)));} 

start:                program                                         { print_ast($1);};
program:              PROGRAM IS body SEMICOLON                       { $$=$1; };
body:                 declaration_S BEGIN statement_S END             { $$=$1; };
declaration_S:        declaration_S declaration                       { $$=$1; };
statement_S:          statement_S statement
declaration:          VAR var_decl_S
                     |TYPE type_decl_S
                     |PROCEDURE procedure_decl_S
var_decl_S:           var_decl_S var_decl
type_decl_S:          type_decl_S type_decl
porcedure_decl_S:     porcedure_decl_S porcedure_decl
var_decl:             ID var_decl_id_S var_decl_type_O ':=' expression ';'
var_decl_id_S:        var_decl_id_S ',' ID
var_decl_type_O:      ':' typename
type_decl:            ID IS type ';' 
procedure_decl:       ID formal_params procedure_decl_type_O IS body ';'
procedure_decl_type_O: ':' typename
typename:             ID
type:                 ARRAY OF typename
                     |RECORD component component_S END
component:            ID ':' typename ';'
formal_params:        '(' fp_section fp_section_S ')' 
                     |'(' ')'
fp_section_S:         fp_section_S ';' fp_section
fp_section:           ID fp_section_id_S ':' typename
fp_section_id_S:      fp_section_id_S ',' ID
statement:            lvalue ':=' expression ';'
                     |ID actual_params ';'
                     |READ '(' lvalue statement_lvalue_S ')' ';'
                     |WRITE write_params ';'
                     |IF expression THEN statement_S statement_elsif_S statement_else_O END ';'
                     |WHILE expression DO statement_S END ';'
                     |LOOP statement_S END ';'
                     |FOR ID ':=' expression TO expression statement_by_O DO statement_S END ';'
                     |EXIT ';'
                     |RETURN expression_O ';'
statement_lvalue_S:   statement_lvalue_S ',' lvalue




id:              IDENTIFIER     { $$ = mk_var(yytext); }
                |INTEGERT       { $$ = mk_int(atoi(yytext)); }
                |REALT          { $$ = mk_real(atof(yytext)); }
                |STRINGT        { $$ = mk_str(yytext); }
;

%%
