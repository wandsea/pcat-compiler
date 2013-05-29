#include "action.h"

#include "type.h"
#include "gen.h"

void action( ast* prog ){
    //print_ast_pretty(prog); 
    print_ast_code_style(prog);
    
    int has_error = typecheck(prog);
    if ( has_error )
    	return;

    gen_code(prog);
        
}