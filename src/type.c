#include "type.h"

#include "ast.h"
#include "table.h"

/*
 If it is an expression, it returns the type of the expression; otherwise it returns the AST NoType
 3 basic name type: INT / REAL / BOOLEAN / STR
 
 
 Remaining Issus:
 1: No support for RECORD now..
 2: Variables/procedures/types share the same name space.
    it's sanity has not been carefully verified.
    
*/

// used for universal basic types
ast *basic_int, *basic_real, *basic_bool, *basic_str;
ast *no_type, *need_infer, *void_type;


void error(ast* x, const char* s){
    printf("Error at line %d near \"",x->line_no);
    switch (x->tag){
        case int_ast:   break;
        case real_ast:  break;
        case var_ast:   printf("%s",x->info.variable); break;
        case str_ast:   break;
        case node_ast:  printf("%s",ast_names[x->info.node.tag]);  break;
    }
    printf("\": %s\n",s);
}


void show_trace(ast* x){
    printf("Current at line %2d near \"",x->line_no);
    switch (x->tag){
        case int_ast:   printf("%d",x->info.integer);               break;
        case real_ast:  printf("%f",x->info.real);                  break;
        case var_ast:   printf("%s",x->info.variable);              break;
        case str_ast:   printf("%s",x->info.string);                break;
        case node_ast:  printf("%s",ast_names[x->info.node.tag]);   break;
    }
    printf("\"\n");
}


ast* _check_type( ast* x ){
#define PICK(k)        pick(x->info.node.arguments,k)
#define gop(k)         _check_type( PICK(k) )
#define FOREACH        for(l=x->info.node.arguments;l;l=l->next)
#define ELEM           l->elem
#define go(e)         _check_type(e)

    //show_trace(x);
    
    ast* result = no_type;
    ast* decl;
    if ( x == NULL )
        printf("[!EMPTY!]\n");
    else switch (x->tag){
        case int_ast:    result = basic_int;        break;
        case real_ast:   result = basic_real;       break;
        case var_ast:    
            decl = lookup(x->info.variable,NULL);
            if ( decl )
                result = decl;
            break;
        case str_ast:    result = basic_str;        break;
        case node_ast : {
            ast_list* l;           // loop variable
            char* id;              // identifier
            ast *t0,*t1,*t2,*t3;   // temp
            int level;             
            ast *nx;
            ast *ap,*fp;
            ast_list * lap,*lfp;
            ast_list *ail;
            ast *array_elem_type;
            switch (x->info.node.tag){
                case Program:    
                    begin_scope();       
                    gop(0);        
                    result=no_type;  
                    end_scope();
                    break;
                case BodyDef:      
                    gop(0);gop(1); 
                    result=no_type; 
                    break;
                case DeclareList:    
                    // order: 
                    // 1. type  () TODO: check forwrading...  
                    // 2. var   (forwarding type)
                    // 3. proc  (forwarding type/var)
                    
                    FOREACH if (ELEM->info.node.tag==TypeDecs) go(ELEM);
                    FOREACH if (ELEM->info.node.tag==VarDecs ) go(ELEM);
                    FOREACH if (ELEM->info.node.tag==ProcDecs) go(ELEM);
                    break;
                case VarDecs:
                    FOREACH go(ELEM); 
                    break;
                case TypeDecs:    
                    FOREACH go(ELEM); 
                    break;       
                case ProcDecs:
                    FOREACH go(ELEM); 
                    break;
                case VarDec:
                    id = PICK(0)->info.variable;
                    t1 = gop(1);
                    t2 = gop(2);
                    if (t1==need_infer)
                        if (t2==no_type)
                            error(x,"Cannot infer the type");
                        else
                        // set t1
                            x->info.node.arguments->next->elem = t2;
                    else
                        if (t2==no_type)
                            ;
                        else
                            if ( t1 != t2 )
                                error(x,"Type conflict");
                    decl = lookup(id,&level);
                    if ( decl && !(level<curr_level()) ){
                        error(x,"Name conflict");
                    }else
                        insert( id, gop(1) );
                    break;
                case TypeDec:                
                    id = PICK(0)->info.variable;
                    
                    decl = lookup(id,&level);
                    if ( !decl ){
                        insert(id,gop(1));
                    }else
                        error(x,"Name conflict");
                    break;
                case ProcDec:                    
                    id = PICK(0)->info.variable;
                    decl = lookup(id,&level);
                    if ( !decl ){
                        insert(id,x);
                    }else
                        error(x,"Name conflict");                       
                    
                    gop(2);  //check return type
                    begin_scope();
                    gop(1);  //check formal list, adding to scope
                    gop(3);  //check procedure body
                    end_scope();
                    break;
                case NamedTyp:
                    id = PICK(0)->info.variable;
                    //printf("%s\n",id);
                    if ( same_name(id,"INT") || same_name(id,"INTEGER") )
                        result = basic_int;
                    else if ( same_name(id,"REAL") )
                        result = basic_real;
                    else if ( same_name(id,"BOOLEAN") )
                        result = basic_bool;
                    else{
                        nx = lookup( id, NULL );
                        if (!nx)
                            error(nx,"Type Name not found");
                        result = nx;
                    }
                    break;
                case ArrayTyp:
                    t0 = gop(0); // check inner element's type
                    if ( t0 == no_type )
                        ;
                    else
                        result = x;
                    break;
                case RecordTyp:
                /*
                    // TODO: what if error in components?
                    gop(0); // check components type
                    result = x;
                */
                    break;
                case NoTyp:
                    result = no_type;
                    break;
                case CompList:
                /*
                    begin_scope();
                    FOREACH go(ELEM);    // check each components
                    end_scope();
                */
                    break;
                case Comp:
                /*
                // TODO: new fress scope???
                    id = PICK(0)->info.variable;
                    decl = lookup(id,&level);
                    if ( decl )
                        error(x,"Name conflict is not allowed");
                    else{
                        t1 = gop(1);
                        insert(id,t1);
                    }
                */
                    break;
                case FormalParamList:
                    FOREACH go(ELEM);   // check each param
                    break;
                case Param:
                // TODO: treat name conflict like var
                    id = PICK(0)->info.variable;
                    decl = lookup(id,&level);
                    if ( decl && !(level < curr_level() ) ){
                        error(x,"Name conflict is not allowed");
                        printf("%d %d\n",level,curr_level());
                    }else{
                        t1 = gop(1);
                        insert(id,t1);
                        result = t1;
                    }
                    break;
                case AssignSt:
                    t0 = gop(0);
                    t1 = gop(1);
                    if ( t0==no_type || t1 == no_type )
                        ;
                    else if ( t0 != t1 ){
                        error(x,"Assgining between diffrent types is not allowed");
                        //printf("%s\n",t0->info.node.arguments->elem->info.string);
                        //printf("%s\n",t1->info.node.arguments->elem->info.string);                        
                    }
                    break;
                case CallSt:
                    id = PICK(0)->info.variable;                    
                    decl = lookup(id,&level);
                    if ( !decl )
                        error(x,"Cannot find the called procedure");
                    else{
                        //ap -> actual parameters
                        //fp -> formal paramaters
                        ap = pick(x->info.node.arguments,1);
                        fp = pick(decl->info.node.arguments,1);
                        lap = ap->info.node.arguments;
                        lfp = fp->info.node.arguments;
                        // lap -> list of ap
                        // lfp -> list of fp
                        for(;lap && lfp; lap=lap->next, lfp=lfp->next ){
                            t0 = _check_type(lap->elem);
                            t1 = _check_type(pick(lfp->elem->info.node.arguments,1));
                            if ( t0 != t1 )
                                error(x,"Formal and actual parameters don't match");
                        }
                        if ( lap && !lfp )
                            error(x,"Too many actual parameters");
                        if ( !lap && lfp )
                            error(x,"Need more actual parameters");
                    }
                    break;
                case ReadSt:
                    nx = PICK(0);
                    for(l=nx->info.node.arguments;l;l=l->next){
                        t0 = _check_type(l->elem);
                        
                        if ( t0 != basic_int && 
                             t0 != basic_real &&
                             t0 != basic_str )
                            error(x,"In READ statement only basic types (INT/REAL) are allowed");
                    }
                    break;
                case WriteSt:
                    nx = PICK(0);
                    for(l=nx->info.node.arguments;l;l=l->next){
                        t0 = _check_type(l->elem);
                        if ( t0 != basic_int && 
                             t0 != basic_real &&
                             t0 != basic_str ){
                            error(x,"In WRITE statement only basic types (INT/REAL) and STRING are allowed");
                            //printf("%s\n",t0->info.)
                        }
                    }
                    break;
                case IfSt:
                    t0 = gop(0);
                    if ( t0 != basic_bool )
                        error(x,"Condition in IF must be of BOOLEAN type");
                    gop(1);
                    gop(2);
                    break;
                case WhileSt:
                    t0 = gop(0);
                    if ( t0 != basic_bool )
                        error(x,"Condition in WHILE must be of BOOLEAN type");
                    gop(1);
                    break;
                case LoopSt:
                    gop(1);
                    break;
                case ForSt:
                    id = PICK(0)->info.variable; // for *I*
                    t1 = gop(1);                 // =   *a*
                    t2 = gop(2);                 // to  *b*
                    t3 = gop(3);                 // by  *c*
                    
                    decl = lookup(id,&level);
                    if ( !decl )
                        error(x,"Unknow index variable in FOR");
                    else if (t1 != basic_int)
                        error(x,"Start of range in FOR must be of INT type");
                    else if (t2 != basic_int)
                        error(x,"End of range in FOR must be of INT type");
                    else if (t3 != basic_int)
                        error(x,"Step of range in FOR must be of INT type");
                        
                    gop(4);                      // do  *e*
                    break;
                case ExitSt:
                    break;
                case RetSt:
                    //if ( PICK(0)->info.node.tag == EmptyExpression && 
                    break;  // TODO: check against procedure type
                case SeqSt:                    
                    FOREACH go(ELEM); break;
                case ExprList: 
                    FOREACH go(ELEM); break;
                    break;
                /*
                    For expression, return no-type if something wrong or some 
                        component is of no-type, which means I couldn't
                        handle the type of this expression.
                */
                case BinOpExp:
                    t1 = gop(1);
                    t2 = gop(2);
                    
                    if ( t1==no_type || t2 == no_type )
                        ;
                    else if ( t1!=t2 )
                        error(x,"Different type around binary operator");
                    else if ( t1!=basic_int && t1!=basic_real && t1!=basic_bool )
                        error(x,"Non-basic type couldn't be used for binary operation");
                    else if ( (PICK(0)->info.node.tag == And || PICK(0)->info.node.tag == Or) && 
                              t1!=basic_bool )
                        error(x,"Binary arithmic operation not for BOOLEAN");
                    else if ( !(PICK(0)->info.node.tag == And || PICK(0)->info.node.tag == Or) &&
                              t1!=basic_int )
                        error(x,"Binary boolean operation not for INT");
                    else{
                        if ( PICK(0)->info.node.tag == Gt ||
                             PICK(0)->info.node.tag == Lt ||
                             PICK(0)->info.node.tag == Eq ||
                             PICK(0)->info.node.tag == Ge ||
                             PICK(0)->info.node.tag == Le ||
                             PICK(0)->info.node.tag == Ne ||
                             PICK(0)->info.node.tag == And||
                             PICK(0)->info.node.tag == Or )
                            result = basic_bool;
                        else
                            result = basic_int;
                    }
                    break;
                case UnOpExp:
                    t1 = gop(1);                    
                    if ( t1==no_type )
                        ;
                    else if ( t1!=basic_int && t1!=basic_real && t1!=basic_bool )
                        error(x,"Non-basic type couldn't be used for unary operation");
                    else if ( (PICK(0)->info.node.tag == Not) && t1!=basic_bool )
                        error(x,"Unary arithmic operation not for BOOLEAN");
                    else if ( !(PICK(0)->info.node.tag == And) && t1!=basic_int )
                        error(x,"Unary boolean operation not for INT");
                    else{
                        if ( PICK(0)->info.node.tag == Not )
                            result = basic_bool;
                        else
                            result = basic_int;
                    }
                    break;
                case LvalExp:
                    result = gop(0);
                    break;
                case CallExp:                    
                    id = PICK(0)->info.variable;                    
                    decl = lookup(id,&level);
                    if ( !decl )
                        error(x,"Cannot find the called procedure");
                    else{
                        //ap -> actual parameters
                        //fp -> formal paramaters
                        ap = pick(x->info.node.arguments,1);
                        fp = pick(decl->info.node.arguments,1);
                        lap = ap->info.node.arguments;
                        lfp = fp->info.node.arguments;
                        // lap -> list of ap
                        // lfp -> list of fp
                        for(;lap && lfp; lap=lap->next, lfp=lfp->next ){
                            t0 = _check_type(lap->elem);
                            t1 = _check_type(pick(lfp->elem->info.node.arguments,1));
                            if ( t0 != t1 )
                                error(x,"Formal and actual parameters don't match");
                        }
                        if ( lap && !lfp )
                            error(x,"Too many actual parameters");
                        if ( !lap && lfp )
                            error(x,"Need more actual parameters");
                            
                        // type of procedure
                        result = _check_type( pick(decl->info.node.arguments,2) );
                    }
                    break;
                case RecordExp:
                    error(x,"RecordExp checking not implement");
                    break;
                case ArrayExp:                 
                    id = PICK(0)->info.variable;                    
                    decl = lookup(id,&level);
                    if ( !decl )
                        error(x,"Cannot find the ARRAY constructor's type");
                    
                    array_elem_type = _check_type( pick(decl->info.node.arguments,0) );
                    
                    
                    ail = PICK(1)->info.node.arguments;
                    for(;ail;ail=ail->next){
                        t0 = _check_type( pick(ail->elem->info.node.arguments,0) );
                        t1 = _check_type( pick(ail->elem->info.node.arguments,1) );
                        if ( t0 != basic_int )
                            error(x,"Counter in ARRAY constructor must be of INT type");
                        if ( t1 != array_elem_type )
                            error(x,"Type in constructor and type in ARRAY definition don't match");
                    }
                    
                    result = decl;
                    break;
                case IntConst:
                    result = basic_int;
                    break;
                case RealConst:
                    result = basic_real;
                    break;
                case StringConst:
                    result = basic_str;
                    break;
                case RecordInitList:
                    break;
                case RecordInit:
                    break;
                case ArrayInitList:
                    break;
                case ArrayInit:
                    break;
                case LvalList:
                    result = gop(0);
                    break;
                case Var:                    
                    id = PICK(0)->info.variable;                    
                    decl = lookup(id,&level);
                    if ( !decl )
                        error(x,"Unknow variable name");
                    else
                        result = decl;
                    break;
                case ArrayDeref:
                    t0 = gop(0);
                    t1 = gop(1);
                    
                    if ( t1 == no_type )
                        ;
                    else if ( t1 != basic_int )
                        error(x,"Array index can only be of INT type");
                    else if ( t0 == no_type )
                        ;
                    else if ( t0->info.node.tag != ArrayTyp )
                        error(x,"Array dereference can only be apply to ARRAY type");
                    else
                        result = _check_type( pick(t0->info.node.arguments,0) );
                    break;
                case RecordDeref:
                    break;
                
                /* binary/unary operator wouldn't be used here */
                
                case Gt:
                case Lt:
                case Eq:
                case Ge:
                case Le:
                case Ne:
                case Plus:
                case Minus:
                case Times:
                case Slash:
                case Div:
                case Mod:
                case And:
                case Or:
                case UPlus:
                case UMinus:
                case Not:
                    printf("!!!!!!!!!!shouldn't be here!!!!\n");                
                    break;
                
                case TypeInferenceNeeded:
                    result = need_infer;
                    break;
                case VoidType:
                    result = void_type;
                    break;
                case EmptyStatement:
                    break;
                case EmptyExpression:
                    printf("!!!!!!!!!!shouldn't be here!!!!\n");
                    break;
            }
            break;
        }
    }
    return result;
}


/* check the type of ast. return 1 if ok, 0 if no-passed*/

void typecheck( ast* x ){
    //basic types
    basic_int  = mk_node(NamedTyp,cons(mk_var("INT"),NULL));
    basic_real = mk_node(NamedTyp,cons(mk_var("REAL"),NULL));
    basic_bool = mk_node(NamedTyp,cons(mk_var("BOOLEAN"),NULL));
    basic_str  = mk_node(NamedTyp,cons(mk_var("basic_string"),NULL));
    no_type    = mk_node(NamedTyp,cons(mk_var("no_type"),NULL));
    need_infer = mk_node(NamedTyp,cons(mk_var("need_infer"),NULL));
    void_type  = mk_node(NamedTyp,cons(mk_var("void_type"),NULL));
    
    // recursively check
    scope_init();    
    _check_type(x);
    
    printf("type checking done!\n");
}

