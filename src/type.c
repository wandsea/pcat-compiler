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
 3: Forward reference is not supported now....
    
*/

// used for universal basic types
ast *basic_int, *basic_real, *basic_bool, *basic_str;
ast *no_type, *need_infer, *void_type;

// global var: any error in type checking
int has_error;

void print_repr(ast* x){  
#define ARGS(x)        x->info.node.arguments
#define PICK(x,k)      pick(ARGS(x),k)
#define PICKX(k)       PICK(x,k)
#define GOPICK(k)      print_repr( PICKX(k) ) 
#define p              printf 
    switch (x->tag){
        case int_ast:   break;
        case real_ast:  break;
        case var_ast:   printf("%s",x->info.variable); break;
        case str_ast:   break;
        case node_ast: {
                switch (x->info.node.tag){
                    case Program: p("PROGRAM IS\n");    break;
                    case BodyDef: p("BEGIN\n");         break;
                    case DeclareList:                   break;
                    case VarDecs:                       break;
                    case TypeDecs:                      break;
                    case ProcDecs:                      break;
                    case VarDec:p("VAR");               break;
                    case TypeDec:p("TYPE");             break;
                    case ProcDec: p("PROCEDURE");       break;
                    case NamedTyp: GOPICK(0);           break;
                    case ArrayTyp: p("ARRAY OF");       break;
                    case RecordTyp:p("RECORD");         break;
                    case NoTyp:p("[No Type]");          break;
                    case CompList:                      break;
                    case Comp:p(" : ");                 break;
                    case FormalParamList:               break;
                    case Param:p(" : ");                break;
                    case AssignSt:p(" := ");            break;
                    case CallSt:p("(");                 break;
                    case ReadSt:p("READ");              break;
                    case WriteSt:p("WRITE");            break;
                    case IfSt:p("IF");                  break;
                    case WhileSt:p("WHILE");            break;    
                    case LoopSt:p("LOOP");              break;
                    case ForSt:p("FOR");                break;
                    case ExitSt:p("EXIT");              break;
                    case RetSt:p("RETURN");             break;
                    case SeqSt:                         break;
                    case ExprList:                      break;
                    case BinOpExp:GOPICK(0);            break;
                    case UnOpExp:GOPICK(0);             break;
                    case LvalExp:                       break;
                    case CallExp:GOPICK(0);             break;
                    case RecordExp:                     break;
                    case ArrayExp:GOPICK(0);            break;
                    case IntConst:GOPICK(0);            break;
                    case RealConst:GOPICK(0);           break;
                    case StringConst: GOPICK(0);        break;
                    case RecordInitList:                break;
                    case RecordInit:                    break;
                    case ArrayInitList:                 break;
                    case ArrayInit:p("OF");             break;
                    case LvalList:                      break;
                    case Var:GOPICK(0);                 break;
                    case ArrayDeref:                    break;
                    case RecordDeref:                   break;
                        
                    case Gt:p(">");         break;
                    case Lt:p("<");         break;
                    case Eq:p("=");         break;
                    case Ge:p(">=");        break;
                    case Le:p("<=");        break;
                    case Ne:p("<>");        break;
                    case Plus:p("+");       break;
                    case Minus:p("-");      break;
                    case Times:p("*");      break;
                    case Slash:p("/");      break;
                    case Div:p(" div ");    break;
                    case Mod:p(" mod ");    break;
                    case And:p(" and ");    break;
                    case Or:p(" or ");      break;
                    case UPlus:p("+");      break;
                    case UMinus:p("-");     break;
                    case Not:p(" not ");    break;
                    
                    case TypeInferenceNeeded:           break;
                    case VoidType:                      break;
                    case EmptyStatement:                break;
                    case EmptyExpression:               break;
                };
                break;
        };
        break;
    }
#undef GOPICK
}

void error(ast* x, const char* s){
    has_error = 1;
    printf("ERROR: at line %d near \"",x->line_no);    
    print_repr( x );    
    printf("\" (");
    
    switch (x->tag){
        case int_ast:   break;
        case real_ast:  break;
        case var_ast:   printf(" (%s)",x->info.variable); break;
        case str_ast:   break;
        case node_ast:  printf("%s",ast_names[x->info.node.tag]);   break;
    }
    
    printf("): %s\n",s);
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

/* 

notes:
    for expr, return type
    for param, return type
    for var, return its decleration
    
 */
ast* _check_type( ast* x ){
#define TAG(x)         x->info.node.tag
#define ARGS(x)        x->info.node.arguments
#define PICK(x,k)      pick(ARGS(x),k)
#define PICKX(k)       PICK(x,k)
#define GOPICK(k)      _check_type( PICKX(k) )
#define GO(e)          _check_type( e )
#define FOREACH(x)     for(l=x->info.node.arguments;l;l=l->next)
#define ELEM(l)        l->elem
#define ELEML          ELEM(l)
#define APPEND(l,e)    l=append(l,e)

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
                    GOPICK(0);        
                    result=no_type;  
                    end_scope();
                    break;
                case BodyDef:      
                    GOPICK(0);GOPICK(1); 
                    result=no_type; 
                    break;
                case DeclareList:    
                    // order: 
                    // 1. type  () TODO: check forwrading...  
                    // 2. var   (forwarding type)
                    // 3. proc  (forwarding type/var)                    
                    FOREACH(x) if (TAG(ELEML)==TypeDecs) GO(ELEML);
                    FOREACH(x) if (TAG(ELEML)==VarDecs ) GO(ELEML);
                    FOREACH(x) if (TAG(ELEML)==ProcDecs) GO(ELEML);
                    break;
                case VarDecs:
                    FOREACH(x) GO(ELEML);
                    break;
                case TypeDecs:    
                    FOREACH(x) GO(ELEML);
                    break;       
                case ProcDecs:
                    FOREACH(x) GO(ELEML);
                    break;
                case VarDec:
                    // append level/offset
                    APPEND(ARGS(x),mk_int(curr_level()));
                    APPEND(ARGS(x),mk_int(0));
                    //x->info.node.arguments = append( x->info.node.arguments, mk_int(curr_level()) );
                    //x->info.node.arguments = append( x->info.node.arguments, mk_int(0) );

                    // real works
                    id = PICKX(0)->info.variable;
                    t1 = GOPICK(1);
                    t2 = GOPICK(2);
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
                        insert( id, x );

                    //printf("Var %s at level %d defined\n",id,PICKX(3)->info.integer);
                    break;
                case TypeDec:                
                    id = PICKX(0)->info.variable;
                    
                    decl = lookup(id,&level);
                    if ( !decl ){
                        insert(id,GOPICK(1));
                    }else
                        error(x,"Name conflict");
                    break;
                case ProcDec:       
                    // append level/offset
                    APPEND(ARGS(x),mk_int(curr_level()));
                    //x->info.node.arguments = append( x->info.node.arguments, mk_int(curr_level()) );

                    // real works             
                    id = PICKX(0)->info.variable;
                    decl = lookup(id,&level);
                    if ( !decl ){
                        insert(id,x);
                    }else
                        error(x,"Name conflict");                       
                    
                    GOPICK(2);  //check return type
                    begin_scope();
                    GOPICK(1);  //check formal list, adding to scope
                    GOPICK(3);  //check procedure body
                    end_scope();
                    break;
                case NamedTyp:
                    id = PICKX(0)->info.variable;
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
                    t0 = GOPICK(0); // check inner element's type
                    if ( t0 == no_type )
                        ;
                    else
                        result = x;
                    break;
                case RecordTyp:
                /*
                    // TODO: what if error in components?
                */
                    break;
                case NoTyp:
                    result = no_type;
                    break;
                case CompList:
                /*
                    // TODO
                */
                    break;
                case Comp:
                /*
                */
                    break;
                case FormalParamList:
                    FOREACH(x) GO(ELEML);   // check each param
                    break;
                case Param:
                    id = PICKX(0)->info.variable;
                    decl = lookup(id,&level);
                    if ( decl && !(level < curr_level() ) ){
                        error(x,"Name conflict is not allowed");
                        printf("%d %d\n",level,curr_level());
                    }else{
                        t1 = GOPICK(1);
                        insert(id,x);
                        result = t1;
                    }
                    break;
                case AssignSt:
                    t0 = GOPICK(0);
                    t1 = GOPICK(1);
                    if ( t0==no_type || t1 == no_type )
                        ;
                    else if ( t0 != t1 )
                        error(x,"Assgining between diffrent types is not allowed");   
                    break;
                case CallSt:
                    id = PICKX(0)->info.variable;                    
                    decl = lookup(id,&level);
                    if ( !decl )
                        error(x,"Cannot find the called procedure");
                    else{
                        //ap -> actual parameters
                        //fp -> formal paramaters
                        ap = PICK(x,1);
                        fp = PICK(decl,1);
                        // lap -> list of ap
                        // lfp -> list of fp
                        lap = ARGS(ap);
                        lfp = ARGS(fp);
                        for(;lap && lfp; lap=lap->next, lfp=lfp->next ){
                            t0 = GO(ELEM(lap));
                            t1 = GO(PICK(ELEM(lfp),1));
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
                    nx = PICKX(0);
                    FOREACH(nx){
                        t0 = GO(ELEML);
                        
                        if ( t0 != basic_int && 
                             t0 != basic_real &&
                             t0 != basic_str )
                            error(x,"In READ statement only basic types (INT/REAL) are allowed");
                    }
                    break;
                case WriteSt:
                    nx = PICKX(0);
                    FOREACH(nx){
                        t0 = GO(ELEML);
                        if ( t0 != basic_int && 
                             t0 != basic_real &&
                             t0 != basic_str ){
                            error(x,"In WRITE statement only basic types (INT/REAL) and STRING are allowed");
                        }
                    }
                    break;
                case IfSt:
                    t0 = GOPICK(0);
                    if ( t0 != basic_bool )
                        error(x,"Condition in IF must be of BOOLEAN type");
                    GOPICK(1);
                    GOPICK(2);
                    break;
                case WhileSt:
                    t0 = GOPICK(0);
                    if ( t0 != basic_bool )
                        error(x,"Condition in WHILE must be of BOOLEAN type");
                    GOPICK(1);
                    break;
                case LoopSt:
                    GOPICK(1);
                    break;
                case ForSt:
                    // For I = a to b by c do e
                    id = PICKX(0)->info.variable;   // for *I*
                    t1 = GOPICK(1);                 // =   *a*
                    t2 = GOPICK(2);                 // to  *b*
                    t3 = GOPICK(3);                 // by  *c*
                    
                    decl = lookup(id,&level);
                    if ( !decl )
                        error(x,"Unknow index variable in FOR");
                    else if (t1 != basic_int)
                        error(x,"Start of range in FOR must be of INT type");
                    else if (t2 != basic_int)
                        error(x,"End of range in FOR must be of INT type");
                    else if (t3 != basic_int)
                        error(x,"Step of range in FOR must be of INT type");
                        
                    GOPICK(4);                      // do  *e*
                    break;
                case ExitSt:
                    break;
                case RetSt:
                    //if ( PICKX(0)->info.node.tag == EmptyExpression && 
                    break;  // TODO: check against procedure type
                case SeqSt:                    
                    FOREACH(x) GO(ELEML); break;
                case ExprList: 
                    FOREACH(x) GO(ELEML); break;
                    break;
                /*
                    For expression, return no-type if something wrong or some 
                        component is of no-type, which means I couldn't
                        handle the type of this expression.
                */
                case BinOpExp:
                    t1 = GOPICK(1);
                    t2 = GOPICK(2);
                    
                    if ( t1==no_type || t2 == no_type )
                        ;
                    else if ( t1!=t2 )
                        error(x,"Different type around binary operator");
                    else if ( t1!=basic_int && t1!=basic_real && t1!=basic_bool )
                        error(x,"Non-basic type couldn't be used for binary operation");
                    else if ( (TAG(PICKX(0)) == And || TAG(PICKX(0)) == Or) && 
                              t1!=basic_bool )
                        error(x,"Binary arithmic operation not for BOOLEAN");
                    else if ( !(TAG(PICKX(0)) == And || TAG(PICKX(0)) == Or) &&
                              t1!=basic_int )
                        error(x,"Binary boolean operation not for INT");
                    else{
                        if ( TAG(PICKX(0)) == Gt ||
                             TAG(PICKX(0)) == Lt ||
                             TAG(PICKX(0)) == Eq ||
                             TAG(PICKX(0)) == Ge ||
                             TAG(PICKX(0)) == Le ||
                             TAG(PICKX(0)) == Ne ||
                             TAG(PICKX(0)) == And||
                             TAG(PICKX(0)) == Or )
                            result = basic_bool;
                        else
                            result = basic_int;
                    }
                    break;
                case UnOpExp:
                    t1 = GOPICK(1);                    
                    if ( t1==no_type )
                        ;
                    else if ( t1!=basic_int && t1!=basic_real && t1!=basic_bool )
                        error(x,"Non-basic type couldn't be used for unary operation");
                    else if ( (TAG(PICKX(0)) == Not) && t1!=basic_bool )
                        error(x,"Unary arithmic operation not for BOOLEAN");
                    else if ( !(TAG(PICKX(0)) == Not) && t1!=basic_int )
                        error(x,"Unary boolean operation not for INT");
                    else{
                        if ( TAG(PICKX(0)) == Not )
                            result = basic_bool;
                        else
                            result = basic_int;
                    }
                    break;
                case LvalExp:
                    result = GOPICK(0);
                    break;
                case CallExp:                    
                    id = PICKX(0)->info.variable;                    
                    decl = lookup(id,&level);
                    if ( !decl )
                        error(x,"Cannot find the called procedure");
                    else{
                        //ap -> actual parameters
                        //fp -> formal paramaters
                        ap = PICK(x,1);
                        fp = PICK(decl,1);
                        // lap -> list of ap
                        // lfp -> list of fp
                        lap = ARGS(ap);
                        lfp = ARGS(fp);
                        for(;lap && lfp; lap=lap->next, lfp=lfp->next ){
                            t0 = GO(ELEM(lap));
                            t1 = GO(PICK(ELEM(lfp),1));
                            if ( t0 != t1 )
                                error(x,"Formal and actual parameters don't match");
                        }
                        if ( lap && !lfp )
                            error(x,"Too many actual parameters");
                        if ( !lap && lfp )
                            error(x,"Need more actual parameters");
                        
                        // type of procedure
                        result = GO( PICK(decl,2) );
                    }
                    break;
                case RecordExp:
                    error(x,"RecordExp checking not implement");
                    break;
                case ArrayExp:                 
                    id = PICKX(0)->info.variable;                    
                    decl = lookup(id,&level);
                    if ( !decl )
                        error(x,"Cannot find the ARRAY constructor's type");
                    
                    
                    array_elem_type = GO( PICK(decl,0) );
                    
                    ail = PICKX(1)->info.node.arguments;
                    for(;ail;ail=ail->next){
                        t0 = GO( PICK(ELEM(ail),0) );
                        t1 = GO( PICK(ELEM(ail),1) );
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
                    result = GOPICK(0);
                    break;
                case Var:                    
                    id = PICKX(0)->info.variable;                    
                    decl = lookup(id,&level);
                    if ( !decl )
                        error(x,"Unknow variable name");
                    else{
                        result = GO( PICK(decl,1) );
                    }
                    //printf("note: at line %d, type of \"%s\" is defined at line %d\n",x->line_no,id,decl->line_no);

                    break;
                case ArrayDeref:
                    t0 = GOPICK(0);
                    t1 = GOPICK(1);
                    
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

int typecheck( ast* x ){
    //basic types
    basic_int  = mk_node(NamedTyp,cons(mk_var("INT"),NULL));
    basic_real = mk_node(NamedTyp,cons(mk_var("REAL"),NULL));
    basic_bool = mk_node(NamedTyp,cons(mk_var("BOOLEAN"),NULL));
    basic_str  = mk_node(NamedTyp,cons(mk_var("basic_string"),NULL));
    no_type    = mk_node(NamedTyp,cons(mk_var("no_type"),NULL));
    need_infer = mk_node(NamedTyp,cons(mk_var("need_infer"),NULL));
    void_type  = mk_node(NamedTyp,cons(mk_var("void_type"),NULL));
    
    // recursively check
    has_error = 0;
    scope_init();    

    printf("========== Type Checking Start ========\n");
    _check_type(x);    
    if ( has_error )
        printf("Type Checking finished with *ERRORS*\n");
    else
        printf("Type Checking finished *SUCCESSFULLY*\n");
    printf("==========  Type Checking End  ========\n");

    return has_error;
}

