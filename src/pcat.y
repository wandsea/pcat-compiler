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

%token ID INTEGERT REALT STRINGT
       PROGRAM IS BEGINT END VAR TYPE PROCEDURE ARRAY RECORD
       IN OUT READ WRITE IF THEN ELSE ELSIF WHILE DO LOOP
       FOR EXIT RETURN TO BY AND OR NOT OF DIV MOD
       LPAREN  RPAREN LBRACKET RBRACKET LBRACE RBRACE COLON DOT
       SEMICOLON COMMA ASSIGN PLUS MINUS STAR SLASH BACKSLASH EQ
       NEQ LT LE GT GE LABRACKET RABRACKET EOFF ERROR

%type <Tast> program
%type <Tast> body
%type <Tast> declaration_S
%type <Tast> statement_S
%type <Tast> declaration
%type <Tast> var_decl_S 
%type <Tast> type_decl_S
%type <Tast> procedure_decl_S
%type <Tast> var_decl
%type <Tast> var_decl_id_S
%type <Tast> var_decl_type_O 
%type <Tast> type_decl
%type <Tast> procedure_decl
%type <Tast> procedure_decl_type_O
%type <Tast> typename
%type <Tast> type
%type <Tast> component
%type <Tast> formal_params
%type <Tast> fp_section_S
%type <Tast> fp_section
%type <Tast> fp_section_id_S
%type <Tast> statement
%type <Tast> statement_lvalue_S
%type <Tast> statement_elsif_S
%type <Tast> statement_else_O
%type <Tast> statement_by_O
%type <Tast> write_params
%type <Tast> write_params_expr_S
%type <Tast> write_expr
%type <Tast> expression_O
%type <Tast> expression
%type <Tast> lvalue
%type <Tast> actual_params
%type <Tast> actual_params_expr_S
%type <Tast> record_inits
%type <Tast> record_inits_pair_S
%type <Tast> array_inits
%type <Tast> array_inits_array_init_S
%type <Tast> array_init
%type <Tast> array_init_expr_of_S 
%type <Tast> number

%%
//id ASSIGN id SEMICOLON  { $$ = mk_node(assign_exp,cons($1,cons($3,NULL)));} 

start:                program { print_ast($1);}
;
program:              PROGRAM IS body SEMICOLON { $$=$1; }
;
body:                 declaration_S BEGINT statement_S END { $$=$1; }
;
declaration_S:        declaration_S declaration { $$=$1; }
                     |
;
statement_S:          statement_S statement { $$=$1; }
                     |
;
declaration:          VAR var_decl_S  { $$=$1; }
                     |TYPE type_decl_S  { $$=$1; }
                     |PROCEDURE procedure_decl_S { $$=$1; }
;
var_decl_S:           var_decl_S var_decl { $$=$1; }
                     |
;
type_decl_S:          type_decl_S type_decl { $$=$1; }
                     |
;
procedure_decl_S:     procedure_decl_S procedure_decl { $$=$1; }
;
var_decl:             ID var_decl_id_S var_decl_type_O ASSIGN expression SEMICOLON { $$=$1; }
;
var_decl_id_S:        var_decl_id_S COMMA ID { $$=$1; }
                     |
;
var_decl_type_O:      COLON typename { $$=$1; }
                     |
;
type_decl:            ID IS type SEMICOLON { $$=$1; }
;
procedure_decl:       ID formal_params procedure_decl_type_O IS body SEMICOLON { $$=$1; }
;
procedure_decl_type_O: COLON typename  { $$=$1; }
                     |
;
typename:             ID { $$=$1; }
;
type:                 ARRAY OF typename { $$=$1; }
                     |RECORD component component_S END { $$=$1; }
;
component_S:          component_S component { $$=$!; }
component:            ID COLON typename SEMICOLON { $$=$1; }
;
formal_params:        LPAREN fp_section fp_section_S RPAREN   { $$=$1; }
                     |LPAREN RPAREN   { $$=$1; }
;
fp_section_S:         fp_section_S SEMICOLON fp_section  { $$=$1; }
                     |
;
fp_section:           ID fp_section_id_S COLON typename  { $$=$1; }
;
fp_section_id_S:      fp_section_id_S COMMA ID  { $$=$1; }
                     |
;
statement:            lvalue ASSIGN expression SEMICOLON  { $$=$1; }
                     |ID actual_params SEMICOLON   { $$=$1; }
                     |READ LPAREN lvalue statement_lvalue_S RPAREN SEMICOLON    { $$=$1; }
                     |WRITE write_params SEMICOLON     { $$=$1; }
                     |IF expression THEN statement_S statement_elsif_S statement_else_O END SEMICOLON { $$=$1; }
                     |WHILE expression DO statement_S END SEMICOLON     { $$=$1; }
                     |LOOP statement_S END SEMICOLON        { $$=$1; }
                     |FOR ID ASSIGN expression TO expression statement_by_O DO statement_S END SEMICOLON { $$=$1; }
                     |EXIT SEMICOLON { $$=$1; }
                     |RETURN expression_O SEMICOLON { $$=$1; }
;
statement_lvalue_S:   statement_lvalue_S COMMA lvalue { $$=$1; }
                     |
;
statement_elsif_S:    statement_elsif_S ELSIF expression THEN statement_S { $$=$1; }
                     |
;
statement_else_O:     ELSE statement_S { $$=$1; }
                     |
;
statement_by_O:       statement_by_O BY expression { $$=$1; }
                     |
;
write_params:         LPAREN write_expr write_params_expr_S RPAREN { $$=$1; }
                     |LPAREN RPAREN { $$=$1; }
;
write_params_expr_S:  write_params_expr_S COMMA write_expr { $$=$1; }
                     |
;
write_expr:           STRINGT  { $$=$1; }
                     |expression { $$=$1; }
;
expression_O:         expression { $$=$1; }
                     |
;
expression:           number    { $$=$1; }
                     |lvalue       { $$=$1; }
                     |LPAREN expression RPAREN     { $$=$1; }
                     |PLUS expression     { $$=$1; }
                     |MINUS expression     { $$=$1; }
                     |NOT expression     { $$=$1; }
                     |expression PLUS expression    { $$=$1; }
                     |expression MINUS expression    { $$=$1; }
                     |expression STAR expression    { $$=$1; }
                     |expression SLASH expression    { $$=$1; }
                     |expression DIV expression    { $$=$1; }
                     |expression MOD expression    { $$=$1; }
                     |expression OR expression    { $$=$1; }
                     |expression AND expression    { $$=$1; }
                     |expression EQ expression    { $$=$1; }
                     |expression NEQ expression    { $$=$1; }
                     |expression LT expression    { $$=$1; }
                     |expression LE expression    { $$=$1; }
                     |expression GT expression    { $$=$1; }
                     |expression GE expression    { $$=$1; }
                     |ID actual_params  { $$=$1; }
                     |ID record_inits  { $$=$1; }
                     |ID array_inits    { $$=$1; }
;
lvalue:               ID  { $$=$1; }
                     |lvalue LBRACKET expression RBRACKET  { $$=$1; }
                     |lvalue DOT ID   { $$=$1; }
;
actual_params:        LPAREN expression actual_params_expr_S RPAREN  { $$=$1; }
                     |LPAREN RPAREN  { $$=$1; }
;
actual_params_expr_S: actual_params_expr_S COMMA expression  { $$=$1; }
                     |
;
record_inits:         LBRACE ID ASSIGN expression record_inits_pair_S RBRACE { $$=$1; }
;
record_inits_pair_S:  record_inits_pair_S SEMICOLON ID ASSIGN expression { $$=$1; }
                     |
;
array_inits:          LABRACKET array_init array_inits_array_init_S RABRACKET { $$=$1; }
;
array_inits_array_init_S: array_inits_array_init_S COMMA array_init { $$=$1; }
                     |
;
array_init:           array_init_expr_of_S expression { $$=$1; }
;
array_init_expr_of_S: array_init_expr_of_S expression OF { $$=$1; }
                     |
;
number:               INTEGERT { $$=$1; }
                     |REALT { $$=$1; }
;


%%
