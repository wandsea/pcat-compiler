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



ast_list* cons ( ast* e, ast_list* r ) {
  ast_list* res = (ast_list*) malloc(sizeof(ast_list));
  res->elem = e;
  res->next = r;
  return res;
};

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
            case str_ast: printf("STR(\"%s\")\n",x->info.string); break;
            case node_ast: {
                printf("%s, %d:%d~%d:%d",ast_names[x->info.node.tag],x->l1,x->c1,x->l2,x->c2);
                printf("\n");
                _print_ast_list_pretty(x->info.node.arguments,offset+2);
                /*
                for( i = 0; i < offset; i++ ) printf(" ");
                printf(")");
                */
                break;
            };
        }
        
    }
}

void print_ast_pretty( ast* x ){
    _print_ast_pretty( x, 0 );
}
