#ifndef _AST_H_
#define _AST_H_


#include "malloc.h"
#include <string.h>
#include <stdio.h>       /* for printf */

/* Put the names of all the different kinds of ASTs here */

typedef enum {
Program,                BodyDef,            DeclareList,        VarDecs,                TypeDecs,
ProcDecs,               VarDec,             TypeDec,            ProcDec,                NamedTyp,
ArrayTyp,               RecordTyp,          NoTyp,              CompList,               Comp,
FormalParamList,        Param,              AssignSt,           CallSt,                 ReadSt,
WriteSt,                IfSt,               WhileSt,            LoopSt,                 ForSt,
ExitSt,                 RetSt,              SeqSt,              ExprList,               BinOpExp,
UnOpExp,                LvalExp,            CallExp,            RecordExp,              ArrayExp,
IntConst,               RealConst,          StringConst,        RecordInitList,         RecordInit,
ArrayInitList,          ArrayInit,          LvalList,           Var,                    ArrayDeref,RecordDeref,

Gt,Lt,Eq,Ge,Le,Ne,Plus,Minus,Times,Slash,Div,Mod,
And,Or,UPlus,UMinus,Not,

TypeInferenceNeeded,    VoidType,           EmptyStatement,     EmptyExpression
} ast_kind;



static char* ast_names[] = {
"Program",              "BodyDefine",       "DeclareList",      "VaribleDeclareList",   "TypeDeclareList",
"ProcedureDeclareList", "VaribleDeclare",   "TypeDeclare",      "ProcDeclare",          "NamedTyp",
"ArrayTyp",             "RecordTyp",        "NoTyp",            "ComponentList",        "Component",
"FormalParameterList",  "Parameter",        "AssignStatement",  "CallStatement",        "ReadStatement",
"WriteStatement",       "IfStatement",      "WhileStatement",   "LoopStatement",        "ForStatement",
"ExitStatement",        "ReturnStatement",  "StatementList",    "ExprList",             "BinOpExpr",
"UnOpExpr",             "LvalExpr",         "CallExpr",         "RecordExpr",           "ArrayExpr",
"IntConst",             "RealConst",        "StringConst",      "RecordInitList",       "RecordInit",
"ArrayInitList",        "ArrayInit",        "LvalList",         "Var",                  "ArrayDeref",           "RecordDeref",

"Gt","Lt","Eq","Ge","Le","Ne","Plus","Minus","Time","Slash","Div","Mod",
"And","Or","UPlus","UMinus","Not",

"TypeInferenceNeeded",  "VoidType",         "EmptyStatement",   "EmptyExpression"
};


/* This is a universal data structure that can capture any AST:
 * The node is an internal node and has a list of children (other ASTs),
 * while the other nodes are leaves       */

typedef struct ast {
  enum { int_ast, real_ast, var_ast, str_ast, node_ast } tag;
  union {
          /*long          integer;   */
          /*long would be 64-bit now, change to 32-bit int*/
            int           integer;   
            double        real;
            char*         variable;
            char*         string;
            struct { 
                ast_kind          tag;
                struct ast_list*  arguments;
            } node;
      } info;
} ast;


typedef struct ast_list { 
  ast*             elem;
  struct ast_list* next;
} ast_list;


/* create an integer AST leaf */
ast* mk_int ( const long x );


/* create a floating point AST leaf */
ast* mk_real ( const double x );


/* create an AST leaf for a name */
ast* mk_var ( const char* x );


/* create a string AST leaf */
ast* mk_str ( const char* x );


/* create an internal AST node */
ast* mk_node ( const ast_kind tag, ast_list* args);


/* put an AST e in the beginning of the list of ASTs r */
ast_list* cons ( ast* e, ast_list* r );

/* put an AST e in the endding of the list of ASTs r */
ast_list* append ( ast_list* r, ast* e );

/* join two lists of ASTs together */
ast_list *join( ast_list* a, ast_list* b );

/* pick k-th member of AST list */
ast* pick( ast_list* a, int k );

/* the empty list of ASTs */
#define null NULL


/* size of an AST list */
short length ( ast_list* );

/* reverse the order of ASTs in an AST list */
ast_list* reverse ( ast_list* );


/* printing functions for ASTs */
void print_ast_list ( ast_list* r );

void print_ast ( ast* x );

/* pretty printing */
void print_ast_pretty( ast* x );

/* prettier (code style) printing */
void print_ast_code_style( ast* x );

#endif