%{

#include <stdio.h>
#include <stdlib.h>

#include "y.tab.h"
#include "ast.h"


extern int lineno;
extern char *yytext;

/* parse error */
void yyerror ( char* s ) {
  printf("\n*** %s (line : %d, token: '%s')\n",
         s, lineno, yytext);
};
ast* mk_node ( const ast_kind tag, ast_list* args, YYLTYPE *yylloc);

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
       
//vitual tokens
%token VUMinus

%type <Tast> program
%type <Tast> body
%type <Tast> declaration_list
%type <Tast_list> declaration_S
%type <Tast> statement_list
%type <Tast_list> statement_S
%type <Tast> declaration
%type <Tast_list> var_decl_S 
%type <Tast_list> type_decl_S
%type <Tast_list> procedure_decl_S
%type <Tast_list> var_decl            // return ast_list of ( VarDec (..) )
%type <Tast_list> var_decl_id_S
%type <Tast> var_decl_type_O 
%type <Tast> type_decl
%type <Tast> procedure_decl
%type <Tast> procedure_decl_type_O
%type <Tast> typename
%type <Tast> type
%type <Tast_list> component_S
%type <Tast> component
%type <Tast> formal_params
%type <Tast_list> fp_section_S
%type <Tast_list> fp_section        // return ast_list of ( Param (..) )
%type <Tast_list> fp_section_id_S
%type <Tast> statement
%type <Tast_list> statement_lvalue_S
%type <Tast_list> statement_elsif_S
%type <Tast> statement_else_O
%type <Tast> statement_by_O
%type <Tast_list> write_params
%type <Tast_list> write_params_expr_S
%type <Tast> write_expr
%type <Tast> expression_O
%type <Tast> expression
%type <Tast> lvalue
%type <Tast> actual_params   // return ExprList
%type <Tast_list> actual_params_expr_S
%type <Tast> record_inits
%type <Tast_list> record_inits_pair_S
%type <Tast> array_inits
%type <Tast_list> array_inits_array_init_S
%type <Tast> array_init
%type <Tast> number
%type <Tast> identifier
%type <Tast> string

// precedence assigment
%nonassoc    ASSIGN
%left        OR
%left        AND
%nonassoc    EQ NEQ
%nonassoc    LT LE GT GE
%left        PLUS MINUS
%left        STAR DIV MOD
%nonassoc    VUMinus
%nonassoc    LBRACKET DOT



%%

start:                program { print_ast_pretty($1); }
;
program:              PROGRAM IS body SEMICOLON { $$=mk_node(Program,cons($3,NULL),&@$); }
;
body:                 declaration_list BEGINT statement_list END { $$=mk_node(BodyDef,cons($1,cons($3,NULL)),&@$); }
;
declaration_list:     declaration_S { $$=mk_node(DeclareList,reverse($1),&@$);}
;
declaration_S:        declaration_S declaration { $$=cons($2,$1); }
                     | {$$=NULL;}
;
statement_list:       statement_S { $$=mk_node(SeqSt,reverse($1),&@$); }
;
statement_S:          statement_S statement { $$=cons($2,$1); }
                     | {$$=NULL;}
;
declaration:          VAR var_decl_S  { $$=mk_node(VarDecs,$2,&@$); }       // by using join, it's in right order, no need for reverse
                     |TYPE type_decl_S  { $$=mk_node(TypeDecs,reverse($2),&@$); }
                     |PROCEDURE procedure_decl_S { $$=mk_node(ProcDecs,reverse($2),&@$); }
;
var_decl_S:           var_decl_S var_decl { $$=join($1,$2); }
                     | {$$=NULL;}
;
type_decl_S:          type_decl_S type_decl { $$=cons($2,$1); }
                     | {$$=NULL;}
;
procedure_decl_S:     procedure_decl_S procedure_decl { $$=cons($2,$1); }
                     | {$$=NULL;}
;
var_decl:             identifier var_decl_id_S var_decl_type_O ASSIGN expression SEMICOLON
                            {
                                struct ast_list* id_list = cons($1,reverse($2)); $$=NULL; 
                                struct ast_list* var_list = NULL;
                                while( id_list != NULL ){
                                    var_list = cons( mk_node(VarDec,cons(id_list->elem,cons($3,cons($5,NULL))),&@$), var_list );
                                    id_list = id_list->next;
                                }
                                $$=reverse(var_list);
                            }
;
var_decl_id_S:        var_decl_id_S COMMA identifier { $$=cons($3,$1); }
                     | {$$=NULL;}
;
var_decl_type_O:      COLON typename { $$=$2; }
                     | {$$=mk_node(TypeInferenceNeeded,NULL,&@$);}
;
type_decl:            identifier IS type SEMICOLON { $$=mk_node(TypeDec,cons($1,cons($3,NULL)),&@$); }
;
procedure_decl:       identifier formal_params procedure_decl_type_O IS body SEMICOLON { $$=mk_node(ProcDec,cons($1,cons($2,cons($3,cons($5,NULL)))),&@$); }
;
procedure_decl_type_O: COLON typename  { $$=$2; }
                     | {$$=mk_node(VoidType,NULL,&@$);}
;
typename:             identifier { $$=mk_node(NamedTyp,cons($1,NULL),&@$); }
;
type:                 ARRAY OF typename { $$=mk_node(ArrayTyp,cons($3,NULL),&@$); }
                     |RECORD component component_S END { $$=mk_node(RecordTyp,cons(mk_node(CompList,cons($2,reverse($3)),&@$),NULL),&@$); }
;
component_S:          component_S component { $$=cons($2,$1); }
                     | {$$=NULL;}
component:            identifier COLON typename SEMICOLON { $$=mk_node(Comp,cons($1,cons($3,NULL)),&@$); }
;
formal_params:        LPAREN fp_section fp_section_S RPAREN   { $$=mk_node(FormalParamList,join($2,$3),&@$); }
                     |LPAREN RPAREN {$$=mk_node(FormalParamList,NULL,&@$);}
;
fp_section_S:         fp_section_S SEMICOLON fp_section  { $$=join($1,$3); }
                     | {$$=NULL;}
;
fp_section:           identifier fp_section_id_S COLON typename  
                            {  
                                struct ast_list* id_list = cons($1,reverse($2)); $$=NULL; 
                                struct ast_list* fp_list = NULL;
                                while( id_list != NULL ){
                                    fp_list = cons( mk_node(Param,cons(id_list->elem,cons($4,NULL)),&@$), fp_list );
                                    id_list = id_list->next;
                                }
                                $$=reverse(fp_list);
                            }
;
fp_section_id_S:      fp_section_id_S COMMA identifier  { $$=cons($3,$1); }
                     | {$$=NULL;}
;
statement:            lvalue ASSIGN expression SEMICOLON  { $$=mk_node(AssignSt,cons($1,cons($3,NULL)),&@$); }
                     |identifier actual_params SEMICOLON  { $$=mk_node(CallSt,cons($1,cons($2,NULL)),&@$); }
                     |READ LPAREN lvalue statement_lvalue_S RPAREN SEMICOLON    { $$=mk_node(ReadSt,cons($3,reverse($4)),&@$); }
                     |WRITE write_params SEMICOLON        { $$=mk_node(WriteSt,$2,&@$); }
                     |IF expression THEN statement_list statement_elsif_S statement_else_O END SEMICOLON 
                                {
                                    struct ast* if_ast = mk_node(IfSt,cons($2,cons($4,cons(NULL,NULL))),&@$);
                                    struct ast* current_if = if_ast;
                                    struct ast_list* middle_list = reverse($5);
                                    for( ; middle_list != NULL; middle_list = middle_list->next ){
                                        current_if->info.node.arguments->next->next = cons(middle_list->elem,NULL);
                                        current_if = current_if->info.node.arguments->next->next->elem;
                                    }
                                    current_if->info.node.arguments->next->next = cons($6,NULL);
                                    $$ = if_ast;
                                }
                     |WHILE expression DO statement_list END SEMICOLON     { $$=mk_node(WhileSt,cons($2,cons($4,NULL)),&@$); }
                     |LOOP statement_list END SEMICOLON   { $$=mk_node(LoopSt,cons($2,NULL),&@$); }
                     |FOR identifier ASSIGN expression TO expression statement_by_O DO statement_list END SEMICOLON 
                                { 
                                    $$=mk_node(ForSt,cons($2,cons($4,cons($6,cons($7,cons($9,NULL))))),&@$);
                                }
                     |EXIT SEMICOLON                      { $$=mk_node(ExitSt,NULL,&@$); }
                     |RETURN expression_O SEMICOLON       { $$=mk_node(RetSt,cons($2,NULL),&@$); }
;
statement_lvalue_S:   statement_lvalue_S COMMA lvalue     { $$=cons($3,$1); }       
                     |                                    { $$=NULL; }
;
statement_elsif_S:    statement_elsif_S ELSIF expression THEN statement_list 
                                { 
                                    $$=cons(mk_node(IfSt,cons($3,cons($5,cons(NULL,NULL))),&@$),$1);
                                }
                     | { $$=NULL; }
;
statement_else_O:     ELSE statement_list { $$=$2; }
                     | {$$=mk_node(EmptyExpression,NULL,&@$);}
;
statement_by_O:       BY expression { $$=$2; }
                     | {$$=mk_int(1);}
;
write_params:         LPAREN write_expr write_params_expr_S RPAREN { $$=cons($2,reverse($3)); }
                     |LPAREN RPAREN { $$=NULL; }
;
// reversed
write_params_expr_S:  write_params_expr_S COMMA write_expr { $$=cons($3,$1); } 
                     | {$$=NULL;}
;
write_expr:           string  { $$=$1; }
                     |expression { $$=$1; }
;
expression_O:         expression { $$=$1; }
                     | {$$=mk_node(EmptyExpression,NULL,&@$);}
;
expression:           number                       { $$=$1; }
                     |lvalue                       { $$=mk_node(LvalExp,cons($1,NULL),&@$); }
                     |LPAREN expression RPAREN     { $$=$2; }
                     |PLUS expression              { $$=mk_node(UnOpExp,cons(mk_node(UPlus,NULL,&@$),cons($2,NULL)),&@$); }
                     |MINUS expression %prec VUMinus            { $$=mk_node(UnOpExp,cons(mk_node(UMinus,NULL,&@$),cons($2,NULL)),&@$); }
                     |NOT expression               { $$=mk_node(UnOpExp,cons(mk_node(Not,NULL,&@$),cons($2,NULL)),&@$); }
                     |expression PLUS expression   { $$=mk_node(BinOpExp,cons(mk_node(Plus,NULL,&@$),cons($1,cons($3,NULL))),&@$); }
                     |expression MINUS expression  { $$=mk_node(BinOpExp,cons(mk_node(Minus,NULL,&@$),cons($1,cons($3,NULL))),&@$); }
                     |expression STAR expression   { $$=mk_node(BinOpExp,cons(mk_node(Times,NULL,&@$),cons($1,cons($3,NULL))),&@$); }
                     |expression SLASH expression  { $$=mk_node(BinOpExp,cons(mk_node(Slash,NULL,&@$),cons($1,cons($3,NULL))),&@$); }
                     |expression DIV expression    { $$=mk_node(BinOpExp,cons(mk_node(Div,NULL,&@$),cons($1,cons($3,NULL))),&@$); }
                     |expression MOD expression    { $$=mk_node(BinOpExp,cons(mk_node(Mod,NULL,&@$),cons($1,cons($3,NULL))),&@$); }
                     |expression OR expression     { $$=mk_node(BinOpExp,cons(mk_node(Or,NULL,&@$),cons($1,cons($3,NULL))),&@$); }
                     |expression AND expression    { $$=mk_node(BinOpExp,cons(mk_node(And,NULL,&@$),cons($1,cons($3,NULL))),&@$); }
                     |expression EQ expression     { $$=mk_node(BinOpExp,cons(mk_node(Eq,NULL,&@$),cons($1,cons($3,NULL))),&@$); }
                     |expression NEQ expression    { $$=mk_node(BinOpExp,cons(mk_node(Ne,NULL,&@$),cons($1,cons($3,NULL))),&@$); }
                     |expression LT expression     { $$=mk_node(BinOpExp,cons(mk_node(Lt,NULL,&@$),cons($1,cons($3,NULL))),&@$); }
                     |expression LE expression     { $$=mk_node(BinOpExp,cons(mk_node(Le,NULL,&@$),cons($1,cons($3,NULL))),&@$); }
                     |expression GT expression     { $$=mk_node(BinOpExp,cons(mk_node(Gt,NULL,&@$),cons($1,cons($3,NULL))),&@$); }
                     |expression GE expression     { $$=mk_node(BinOpExp,cons(mk_node(Ge,NULL,&@$),cons($1,cons($3,NULL))),&@$); }
                     |identifier actual_params     { $$=mk_node(CallExp,cons($1,cons($2,NULL)),&@$);}
                     |identifier record_inits      { $$=mk_node(RecordExp,cons($1,cons($2,NULL)),&@$);}
                     |identifier array_inits       { $$=mk_node(ArrayExp,cons($1,cons($2,NULL)),&@$);}
;
lvalue:               identifier  { $$=mk_node(Var,cons($1,NULL),&@$); }
                     |lvalue LBRACKET expression RBRACKET  { $$=mk_node(ArrayDeref,cons($1,cons($3,NULL)),&@$); }
                     |lvalue DOT identifier   { $$=mk_node(RecordDeref,cons($1,cons($3,NULL)),&@$); }
;
actual_params:        LPAREN expression actual_params_expr_S RPAREN  { $$=mk_node(ExprList,cons($2,reverse($3)),&@$); }
                     |LPAREN RPAREN  { $$=mk_node(ExprList,NULL,&@$); }
;
actual_params_expr_S: actual_params_expr_S COMMA expression  { $$=cons($3,$1); }
                     | { $$=NULL; }
;
record_inits:         LBRACE identifier ASSIGN expression record_inits_pair_S RBRACE { $$=mk_node(RecordInitList,cons(mk_node(RecordInit,cons($2,cons($4,NULL)),&@$),reverse($5)),&@$); }
;
record_inits_pair_S:  record_inits_pair_S SEMICOLON identifier ASSIGN expression { $$=cons(mk_node(RecordInit,cons($3,cons($5,NULL)),&@$),$1); }
                     | { $$=NULL; }
;
array_inits:          LABRACKET array_init array_inits_array_init_S RABRACKET { $$=mk_node(ArrayInitList,cons($2,reverse($3)),&@$); }
;
array_inits_array_init_S: array_inits_array_init_S COMMA array_init { $$=cons($3,$1); }
                     | { $$=NULL; }
;
array_init:          expression { $$=mk_node(ArrayInit,cons($1,NULL),&@$); }
                    |expression OF expression { $$=mk_node(ArrayInit,cons($1,cons($3,NULL)),&@$); }
;
number:               INTEGERT { $$=mk_int(atoi(yytext)); }
                     |REALT { $$=mk_real(atof(yytext)); }
;
identifier:          ID { $$ = mk_var(yytext); }
;
string:              STRINGT { $$=mk_str(yytext); }
;

%%

ast* mk_node ( const ast_kind tag, ast_list* args, YYLTYPE *yylloc) {
  ast* res = (ast*) malloc(sizeof(ast));
  res->tag = node_ast;
  res->info.node.tag = tag;
  res->info.node.arguments = args;
  res->l1=yylloc->first_line;
  res->c1=yylloc->first_column;
  res->l2=yylloc->last_line;
  res->c2=yylloc->last_column;
  return res;
};
