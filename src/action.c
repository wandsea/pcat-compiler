#include "action.h"

#include "type.h"

void action( ast* prog ){
    //print_ast_pretty(prog); 
    print_ast_code_style(prog);
    typecheck(prog);
}