/********************************************************************************
*
* File: ast.c
* The abstract syntax trees for the calculator
* Programmer: Leonidas Fegaras, UTA
* Date: 12/17/97
*
* modified to fit contemporary 64-bit gcc
*
********************************************************************************/


#include "ast.h"


ast* mk_int ( const long x ) {
  ast* res = (ast*) malloc(sizeof(ast));
  res->tag = int_ast;
  res->info.integer = x;
  return res;
};


ast* mk_real ( const double x ) {
  ast* res = (ast*) malloc(sizeof(ast));
  res->tag = real_ast;
  res->info.real = x;
  return res;
};


ast* mk_var ( const char* x ) {
  ast* res = (ast*) malloc(sizeof(ast));
  res->tag = var_ast;
  res->info.variable = (char*) malloc(strlen(x)+1);
  strcpy(res->info.variable,x);
  return res;
};


ast* mk_str ( const char* x ) {
  ast* res = (ast*) malloc(sizeof(ast));
  res->tag = str_ast;
  res->info.variable = (char*) malloc(strlen(x)+1);
  strncpy(res->info.variable,x+1,strlen(x)-2);
  return res;
};

ast* mk_node ( const ast_kind tag, ast_list* args ) {
  ast* res = (ast*) malloc(sizeof(ast));
  res->tag = node_ast;
  res->info.node.tag = tag;
  res->info.node.arguments = args;
  return res;
};


ast_list* cons ( ast* e, ast_list* r ) {
  ast_list* res = (ast_list*) malloc(sizeof(ast_list));
  res->elem = e;
  res->next = r;
  return res;
};

ast_list* append ( ast_list* r, ast* e ){
    return reverse(cons(e,reverse(r)));
}

ast_list* join ( ast_list* a, ast_list* b ) {
  ast_list* res = NULL;
  for( ; a != NULL && a->elem != NULL; a = a -> next )
    res = cons( a->elem, res );
  for( ; b != NULL && b->elem != NULL; b = b -> next )
    res = cons( b->elem, res );
  res = reverse(res);
  return res;
}


short length ( ast_list* r ) {
  short i = 0;
  for(; r != null; r=r->next) i++;
  return i;
};

ast_list* rev ( ast_list* r, ast_list* s ) {
  if (r == null)
     return s;
  return rev(r->next,cons(r->elem,s));
};


ast_list* reverse ( ast_list* r ) {
  return rev(r,null);
};


ast* pick( ast_list* a, int k ){
    for ( ; k > 0; k-- )
        a = a->next;
    return a->elem;
}


void print_ast_list ( ast_list* r ) {
  if (r == null)
     return;
  printf(" ");
  print_ast(r->elem);
  print_ast_list(r->next);
};


void print_ast ( ast* x ) {
  if ( x==NULL){
    printf("[!EMPTY!]");
    return;
  }
  switch (x->tag) {
  case int_ast: printf("%d",x->info.integer); break;
  case real_ast: printf("%f",x->info.real); break;
  case var_ast: printf("%s",x->info.variable); break;
  case str_ast: printf("\"%s\"",x->info.string); break;
  case node_ast: {
      printf("(%s",ast_names[x->info.node.tag]);
      print_ast_list(x->info.node.arguments);
      printf(")");
      break;
    };
  };
};

void _print_ast_pretty( ast* x, int offset );

void _print_ast_list_pretty ( ast_list* r,int offset  ) {
  if (r == null)
     return;
  _print_ast_pretty(r->elem,offset);
  _print_ast_list_pretty(r->next,offset);
};

void _print_ast_pretty( ast* x, int offset ){
    int i;
    for( i = 0; i < offset; i++ ) printf(" ");
    if ( x== NULL )
        printf("[!EMPTY!]\n");
    else{
        switch (x->tag){
            case int_ast: printf("INT(%d)\n",x->info.integer); break;
            case real_ast: printf("REAL(%f)\n",x->info.real); break;
            case var_ast: printf("VAR(%s)\n",x->info.variable); break;
            case str_ast: printf("STRING(\"%s\")\n",x->info.string); break;
            case node_ast: {
                printf("%s",ast_names[x->info.node.tag]);
                printf("\n");
                _print_ast_list_pretty(x->info.node.arguments,offset+2);
                break;
            };
        }
        
    }
}

void print_ast_pretty( ast* x ){
    printf("========= S-Expr Start ===============\n");
    _print_ast_pretty( x, 0 );
    printf("=========  S-Expr End  ===============\n");
}


/* better printing */

int print_line_no;

void make_offset( int offset ){
    print_line_no++;
    printf("%3d | ",print_line_no);
    
    int i;
    for ( i = 0; i < offset; i++ ) putchar(' ');
}

void _print_ast_code_style( ast* x, int offset ){
#define PICKX(k)       pick(x->info.node.arguments,k)
#define next_offset    (offset+2)
#define gopi(k)        _print_ast_code_style( PICKX(k), next_offset );
#define gop(k)         _print_ast_code_style( PICKX(k), offset );
#define goi(t)         _print_ast_code_style( t, next_offset );
#define go(t)          _print_ast_code_style( t, offset );
#define mo()           make_offset(offset)
#define FOREACH        for(l=x->info.node.arguments;l;l=l->next)
#define ELEM           l->elem
#define EACHGO         FOREACH go(ELEM) 
#define EACHGOI        FOREACH goi(ELEM) 
#define p              printf
#define SEPLIST(sep)   i=0; FOREACH{ if ( i > 0 ){p(sep);p(" ");}go(ELEM); i += 1; }     
#define recline        x->line_no = print_line_no
    if ( x == NULL )
        printf("[!EMPTY!]\n");
    else{
        recline;
        switch (x->tag){
            case int_ast:  printf("%d",x->info.integer); break;
            case real_ast: printf("%f",x->info.real); break;
            case var_ast:  printf("%s",x->info.variable); break;
            case str_ast:  printf("\"%s\"",x->info.string); break;
            case node_ast : {
                struct ast_list* l;
                int i;
                switch (x->info.node.tag){
                    case Program:
                        printf("========= Program Start ===============\n");
                        mo();recline;p("PROGRAM IS\n");
                        gop(0);
                        printf("=========  Program End  ===============\n");
                        break;
                    case BodyDef:
                        gopi(0);
                        mo();recline;p("BEGIN\n");
                        gopi(1);
                        mo();p("END;\n");
                        break;
                    case DeclareList:
                        EACHGO;
                        break;
                    case VarDecs:
                    case TypeDecs:
                    case ProcDecs:
                        EACHGO;
                        break;
                    case VarDec:
                        mo();recline;p("VAR ");gop(0);p(" : ");gop(1);p(" = ");gop(2);p(";\n");
                        break;
                    case TypeDec:
                        mo();recline;p("TYPE ");gop(0);p(" is ");gop(1);p(";\n");
                        break;
                    case ProcDec:
                        mo();recline;p("PROCEDURE ");gop(0);p(" (");gop(1);p(") : ");gop(2);p("\n");
                        gop(3);
                        break;
                    case NamedTyp:
                        gop(0);
                        break;
                    case ArrayTyp:
                        p("ARRAY OF ");gop(0);
                        break;
                    case RecordTyp:
                        p("RECORD\n");
                        gopi(0);
                        mo();p("END;\n");                    
                        break;
                    case NoTyp:
                        p("[No Type]");
                        break;
                    case CompList:
                        EACHGOI;
                        break;
                    case Comp:
                        mo();gop(0);p(" : ");gop(1);p(";\n");
                        break;
                    case FormalParamList:
                        SEPLIST(",");
                        break;
                    case Param:
                        gop(0);p(" : ");gop(1);                
                        break;
                    case AssignSt:
                        mo();recline;gop(0);p(" := ");gop(1);p(";\n");
                        break;
                    case CallSt:
                        mo();recline;gop(0);p("(");gop(1);p(")");p(";\n");
                        break;
                    case ReadSt:
                        mo();recline;p("READ(");gop(0);p(")");p(";\n");
                        break;
                    case WriteSt:
                        mo();recline;p("WRITE(");gop(0);p(")");p(";\n");
                        break;
                    case IfSt:
                        mo();recline;p("IF ");gop(0);p(" THEN ");p("\n");
                        gopi(1);
                        mo();p("ELSE");p("\n");
                        gopi(2);
                        mo();p("END");p(";\n");
                        break;
                    case WhileSt:
                        mo();recline;p("WHILE ");gop(0);p(" DO\n");
                        mo();gopi(1);p("\n");
                        mo();p("END");p(";\n");;
                        break;    
                    case LoopSt:
                        mo();recline;p("LOOP ");p("\n");
                        gopi(0);
                        mo();p("END");p(";\n");
                        break;
                    case ForSt:
                        mo();recline;p("FOR ");gop(0);p(" := ");gop(1);p(" TO ");gop(2);p(" BY ");gop(3);p(" DO");p("\n");
                        gopi(4);
                        mo();p("END");p(";\n");
                        break;
                    case ExitSt:
                        mo();recline;p("EXIT");p(";\n");
                        break;
                    case RetSt:
                        mo();recline;p("RETURN ");gop(0);p(";\n");
                        break;
                    case SeqSt:
                        EACHGO;
                        break;
                    case ExprList:
                        SEPLIST(",");
                        break;
                    case BinOpExp:
                        p("(");gop(1);p(" ");gop(0);p(" ");gop(2);p(")");
                        break;
                    case UnOpExp:
                        p("(");gop(0);p(" ");gop(1);p(")");                    
                        break;
                    case LvalExp:
                        gop(0);
                        break;
                    case CallExp:
                        gop(0);p("(");gop(1);p(")");
                        break;
                    case RecordExp:
                        gop(0);p("{");gop(1);p("}");
                        break;
                    case ArrayExp:
                        gop(0);p("[<");gop(1);p(">]");
                        break;
                    case IntConst:
                    case RealConst:
                    case StringConst:
                        gop(0);
                        break;
                    case RecordInitList:
                        SEPLIST(";"); 
                        break;
                    case RecordInit:
                        gop(0);p(" := ");gop(1);
                        break;
                    case ArrayInitList:
                        SEPLIST(",");                       
                        break;
                    case ArrayInit:
                        gop(0);p(" OF ");gop(1);
                        break;
                    case LvalList:
                        SEPLIST(",");                         
                        break;
                    case Var:
                        gop(0);
                        break;
                    case ArrayDeref:
                        gop(0);p("[");gop(1);p("]");
                        break;
                    case RecordDeref:
                        gop(0);p(".");gop(1);                
                        break;
                        
                    case Gt:p(">");break;
                    case Lt:p("<");break;
                    case Eq:p("=");break;
                    case Ge:p(">=");break;
                    case Le:p("<=");break;
                    case Ne:p("<>");break;
                    case Plus:p("+");break;
                    case Minus:p("-");break;
                    case Times:p("*");break;
                    case Slash:p("/");break;
                    case Div:p(" div ");break;
                    case Mod:p(" mod ");break;
                    case And:p(" and ");break;
                    case Or:p(" or ");break;
                    case UPlus:p("+");break;
                    case UMinus:p("-");break;
                    case Not:p(" not ");break;
                    
                    case TypeInferenceNeeded:
                        p("[Type Inference Needed]");
                        break;
                    case VoidType:
                        p("[Void Type]");
                        break;
                    case EmptyStatement:
                        mo();recline;p("[Empty Statement]");p(";\n");
                        break;
                    case EmptyExpression:
                        p("[Empty Expression]");
                        break;
                }
                break;
            };
        }
        // record the line number for current ast node
        
    }
}

void print_ast_code_style( ast* x ){
    print_line_no = 0;
    _print_ast_code_style( x, 0 );
}
