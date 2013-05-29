#include "type.h"

#include "ast.h"
#include "table.h"

#include "stdlib.h"
#include "assert.h"

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
#define GO_PICK(k)          print_repr( pick_ast(x,k) )
#define GO_PICK_COMP(k)     print_repr( pick_ast_comp(x,k) )
#define p                   printf 
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
                    case NamedTyp: GO_PICK(0);          break;
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
                    case BinOpExp:GO_PICK(0);           break;
                    case UnOpExp:GO_PICK(0);            break;
                    case LvalExp:                       break;
                    case CallExp:GO_PICK(0);            break;
                    case RecordExp:                     break;
                    case ArrayExp:GO_PICK(0);           break;
                    case IntConst:GO_PICK(0);           break;
                    case RealConst:GO_PICK(0);          break;
                    case StringConst: GO_PICK(0);       break;
                    case RecordInitList:                break;
                    case RecordInit:                    break;
                    case ArrayInitList:                 break;
                    case ArrayInit:p("OF");             break;
                    case LvalList:                      break;
                    case Var:GO_PICK(0);                break;
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
#undef GO_PICK
#undef GO_PICK_COMP
#undef p
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

    exit(1);
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

int scope_offset[1000];
ast* scope_return_type[1000];
int scope_offset_top;
#define SCOPE_PUSH scope_offset[scope_offset_top++] = 0
#define SCOPE_POP  scope_offset[--scope_offset_top] = 0
#define CURR_LOCAL_OFFSET scope_offset[scope_offset_top-1]
#define CURR_RETURN_TYPE  scope_return_type[scope_offset_top-1]
#define TAKE_LOCAL_OFFSET (CURR_LOCAL_OFFSET-=4,CURR_LOCAL_OFFSET)

int param_offset;
#define TAKE_PARAM_OFFSET (param_offset+=4,param_offset)

ast* _check_type( ast* x ){
#define GO_PICK(k)          _check_type( pick_ast(x,k) )
#define GO_PICK_COMP(k)     _check_type( pick_ast_comp(x,k) )
#define GO(e)               _check_type( e )
#define FOREACH(x)          for(l=args(x);l;l=l->next)
#define ELEM(l)             l->elem
#define ELEML               ELEM(l)

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
                    SCOPE_PUSH;
                    CURR_RETURN_TYPE = void_type;
                    GO_PICK_COMP("body");        
                    append_ast(x,mk_int(TAKE_LOCAL_OFFSET));
                    SCOPE_POP; 
                    end_scope();
                    break;
                case BodyDef:      
                    GO_PICK_COMP("declarations-list");
                    GO_PICK_COMP("statements-list"); 
                    result=no_type; 
                    break;
                case DeclareList:    
                    // order: 
                    // 1. type  () TODO: check forwrading...  
                    // 2. var   (forwarding type)
                    // 3. proc  (forwarding type/var)                    
                    FOREACH(x) if (tag(ELEML)==TypeDecs) GO(ELEML);
                    FOREACH(x) if (tag(ELEML)==VarDecs ) GO(ELEML);
                    FOREACH(x) if (tag(ELEML)==ProcDecs) GO(ELEML);
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
                    append_ast(x,mk_int(curr_level()));
                    append_ast(x,mk_int(TAKE_LOCAL_OFFSET));

                    // real works
                    id = ast_var(pick_ast_comp(x,"ID"));
                    t1 = GO_PICK_COMP("type");
                    t2 = GO_PICK_COMP("expression");
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

                    break;
                case TypeDec:                
                    id = ast_var(pick_ast_comp(x,"ID"));
                    
                    decl = lookup(id,&level);
                    if ( !decl ){
                        insert(id,GO_PICK_COMP("type"));
                    }else
                        error(x,"Name conflict");
                    break;
                case ProcDec:           
                    id = ast_var(pick_ast_comp(x,"ID"));
                    decl = lookup(id,&level);
                    if ( !decl ){
                        insert(id,x);
                    }else
                        error(x,"Name conflict");                       
                    
                    t2 = GO_PICK_COMP("type");  //check return type
                    begin_scope();
                    SCOPE_PUSH;
                    CURR_RETURN_TYPE = t2;
                    // append level
                    append_ast(x,mk_int(curr_level()));
                    GO_PICK_COMP("formal-param-list");  //check formal list, adding to scope
                    GO_PICK_COMP("body");  //check procedure body
                    append_ast(x,mk_int(TAKE_LOCAL_OFFSET));
                    SCOPE_POP;
                    end_scope();
                    break;
                case NamedTyp:
                    id = ast_var(pick_ast_comp(x,"ID"));
                    if ( same_name(id,"INT") || same_name(id,"INTEGER") || same_name(id,"basic_int") )
                        result = basic_int;
                    else if ( same_name(id,"REAL") || same_name(id,"basic_real") )
                        result = basic_real;
                    else if ( same_name(id,"BOOLEAN") || same_name(id,"basic_bool") )
                        result = basic_bool;
                    else{
                        nx = lookup( id, NULL );
                        if (!nx)
                            error(nx,"Type Name not found");
                        result = nx;
                    }
                    break;
                case ArrayTyp:
                    t0 = GO_PICK_COMP("type"); // check inner element's type
                    if ( t0 == no_type )
                        ;
                    else
                        result = x;
                    break;
                case RecordTyp:
                /*
                    // Not Implemented!
                */
                    break;
                case NoTyp:
                    result = no_type;
                    break;
                case CompList:
                /*
                    // Not Implemented!
                */
                    break;
                case Comp:
                /*
                    // Not Implemented!
                */
                    break;
                case FormalParamList:
                    param_offset = 8;       // first(8) reserved for static links
                    FOREACH(x) GO(ELEML);   // check each param
                    break;
                case Param:
                    id = ast_var(pick_ast_comp(x,"ID"));
                    decl = lookup(id,&level);
                    if ( decl && !(level < curr_level() ) ){
                        error(x,"Name conflict is not allowed");
                        printf("%d %d\n",level,curr_level());
                    }else{
                        t1 = GO_PICK(1);
                        insert(id,x);
                        result = t1;
                    }
                    append_ast(x,mk_int(curr_level()));
                    append_ast(x,mk_int(TAKE_PARAM_OFFSET));
                    break;
                case AssignSt:
                    t0 = GO_PICK_COMP("lvalue");
                    t1 = GO_PICK_COMP("expression");
                    if ( t0==no_type || t1 == no_type )
                        ;
                    else if ( t0 != t1 )
                        error(x,"Assgining between diffrent types is not allowed");   
                    break;
                case CallSt:
                    id = ast_var(pick_ast_comp(x,"ID"));                 
                    decl = lookup(id,&level);
                    if ( !decl )
                        error(x,"Cannot find the called procedure");
                    else{
                        //ap -> actual parameters
                        //fp -> formal paramaters
                        ap = pick_ast_comp(x,"expression-list");
                        fp = pick_ast_comp(decl,"formal-param-list");
                        // lap -> list of ap
                        // lfp -> list of fp
                        lap = args(ap);
                        lfp = args(fp);
                        for(;lap && lfp; lap=lap->next, lfp=lfp->next ){
                            t0 = GO(ELEM(lap));
                            t1 = GO(pick_ast_comp(ELEM(lfp),"type"));
                            if ( t0 != t1 )
                                error(x,"Formal and actual parameters don't match");
                        }
                        if ( lap && !lfp )
                            error(x,"Too many actual parameters");
                        if ( !lap && lfp )
                            error(x,"Need more actual parameters");
                    }

                    append_ast(x,result);
                    append_ast(x,mk_int(curr_level()-pick_ast_comp(decl,"level")->info.integer));
                    break;
                case ReadSt:
                    FOREACH(pick_ast_comp(x,"lvalue-list")){
                        t0 = GO(ELEML);
                        
                        if ( t0 != basic_int && 
                             t0 != basic_real &&
                             t0 != basic_str )
                            error(x,"In READ statement only basic types (INT/REAL) are allowed");
                    }
                    break;
                case WriteSt:
                    FOREACH(pick_ast_comp(x,"expression-list")){
                        t0 = GO(ELEML);
                        if ( t0 != basic_int && 
                             t0 != basic_real &&
                             t0 != basic_str &&
                             t0 != basic_bool ){
                            error(x,"In WRITE statement only basic types (INT/REAL), STRING and BOOL are allowed");
                        }
                    }
                    break;
                case IfSt:
                    t0 = GO_PICK_COMP("expression");
                    if ( t0 != basic_bool )
                        error(x,"Condition in IF must be of BOOLEAN type");
                    GO_PICK_COMP("statement");
                    GO_PICK_COMP("statement-else");
                    break;
                case WhileSt:
                    t0 = GO_PICK_COMP("expression");
                    if ( t0 != basic_bool )
                        error(x,"Condition in WHILE must be of BOOLEAN type");
                    GO_PICK_COMP("statement");
                    break;
                case LoopSt:
                    GO_PICK_COMP("statement");
                    break;
                case ForSt:
                    // For I = a to b by c do e
                    id = ast_var(pick_ast_comp(x,"ID")); 
                    t1 = GO_PICK_COMP("expression-from");               
                    t2 = GO_PICK_COMP("expression-to");                
                    t3 = GO_PICK_COMP("expression-by");               
                    
                    decl = lookup(id,&level);
                    if ( !decl )
                        error(x,"Unknow index variable in FOR");
                    else if (t1 != basic_int)
                        error(x,"Start of range in FOR must be of INT type");
                    else if (t2 != basic_int)
                        error(x,"End of range in FOR must be of INT type");
                    else if (t3 != basic_int)
                        error(x,"Step of range in FOR must be of INT type");
                    
                    GO_PICK_COMP("statement");                     

                    append_ast(x,pick_ast_comp(decl,"offset"));
                    break;
                case ExitSt:
                    break;
                case RetSt:
                    t1 = GO_PICK_COMP("expression");
                    t2 = CURR_RETURN_TYPE;
                    if ( t1 != t2 )
                        error(x,"This type (maybe EMPTY-TYPE) conflicts with PROCEDURE declaration");
                    break;
                case SeqSt:                    
                    FOREACH(x) GO(ELEML); 
                    break;
                case ExprList: 
                    FOREACH(x) GO(ELEML);
                    break;
                /*
                    For expression, return no-type if something wrong or some 
                        component is of no-type, which means I couldn't
                        handle the type of this expression.
                */
                case BinOpExp:
                    t1 = GO_PICK_COMP("expression-left");
                    t2 = GO_PICK_COMP("expression-right");
                    
                    if ( t1==no_type || t2 == no_type )
                        ;
                    else if ( ( t1!=basic_int && t1!=basic_real && t1!=basic_bool ) ||
                              ( t2!=basic_int && t2!=basic_real && t2!=basic_bool ) )
                        error(x,"Non-basic type couldn't be used for binary operation");
                    else{
                        switch( tag(pick_ast_comp(x,"binop")) ){
                            case Plus: case Minus: case Times:
                                if ( ( t1==basic_int || t1==basic_real ) &&
                                     ( t2==basic_int || t2==basic_real ) ){
                                    if ( t1==basic_int && t2==basic_int )
                                        result = basic_int;
                                    else
                                        result = basic_real;
                                }else
                                    error(x,"Binary arithmic operation expects INTEGER or REAL type on both sides");
                                break;
                            case Div: case Mod:
                                if ( t1==basic_int && t2==basic_int )
                                    result = basic_int;
                                else
                                    error(x,"Binary arithmic operation expects INTEGER type on both sides");
                                break;
                            case Slash:
                                if ( ( t1==basic_int || t1==basic_real ) &&
                                     ( t2==basic_int || t2==basic_real ) )
                                    result = basic_real;
                                else
                                    error(x,"Binary arithmic operation expects INTEGER or type on both sides");
                                break;
                            case Gt: case Lt: case Eq: case Ge: case Le: case Ne:
                                if ( ( t1==basic_int || t1==basic_real ) &&
                                     ( t2==basic_int || t2==basic_real ) )
                                    result = basic_bool;
                                else
                                    error(x,"Binary Comparation operation expects INTEGER or REAL type on both sides");
                                break;
                            case And: case Or:
                                if ( t1==basic_bool && t2==basic_bool )
                                    result = basic_bool;
                                else
                                    error(x,"Binary boolean operation expects BOOLEAN type on both sides");
                                break;
                            default:
                                break;
                        }
                    }
                    
                    append_ast(x,result);
                    append_ast(x,mk_int(TAKE_LOCAL_OFFSET));
                    break;
                case UnOpExp:
                    t1 = GO_PICK_COMP("expression");                   
                    if ( t1==no_type )
                        ; 
                    else if ( t1!=basic_int && t1!=basic_real && t1!=basic_bool )
                        error(x,"Non-basic type couldn't be used for unary operation");
                    else{
                        switch( tag(pick_ast_comp(x,"unop")) ){
                            case UPlus: case UMinus:
                                if ( t1==basic_int || t1==basic_real ){
                                    if ( t1==basic_int )
                                        result = basic_int;
                                    else
                                        result = basic_real;
                                }else
                                    error(x,"Unary arithmic operation expects INTEGER or REAL type");                      
                                break;
                            case Not:
                                if ( t1==basic_bool )
                                    result = basic_bool;
                                else
                                    error(x,"Unary boolean operation expects BOOLEAN type");
                                break;
                            default:
                                break;
                        }
                    }
                   
                    append_ast(x,result);
                    append_ast(x,mk_int(TAKE_LOCAL_OFFSET));
                    break;
                case LvalExp:
                    result = GO_PICK_COMP("lvalue");
                    append_ast(x,result);
                    append_ast(x,mk_int(TAKE_LOCAL_OFFSET));
                    break;
                case CallExp:                    
                    id = ast_var(pick_ast_comp(x,"ID"));                 
                    decl = lookup(id,&level);
                    if ( !decl )
                        error(x,"Cannot find the called procedure");
                    else{
                        //ap -> actual parameters
                        //fp -> formal paramaters
                        ap = pick_ast_comp(x,"expression-list");
                        fp = pick_ast_comp(decl,"formal-param-list");
                        // lap -> list of ap
                        // lfp -> list of fp
                        lap = args(ap);
                        lfp = args(fp);
                        for(;lap && lfp; lap=lap->next, lfp=lfp->next ){
                            t0 = GO(ELEM(lap));
                            t1 = GO(pick_ast_comp(ELEM(lfp),"type"));
                            if ( t0 != t1 )
                                error(x,"Formal and actual parameters don't match");
                        }
                        if ( lap && !lfp )
                            error(x,"Too many actual parameters");
                        if ( !lap && lfp )
                            error(x,"Need more actual parameters");
                   
                        // type of procedure
                        result = GO( pick_ast_comp(decl,"type") );
                    }
                    
                    append_ast(x,result);
                    append_ast(x,mk_int(curr_level()-ast_int(pick_ast_comp(decl,"level"))));
                    append_ast(x,mk_int(TAKE_LOCAL_OFFSET));
                    break;
                case RecordExp:
                    error(x,"RecordExp checking not implement");
                    break;
                case ArrayExp:                 
                    id = ast_var(pick_ast_comp(x,"ID"));                    
                    decl = lookup(id,&level);
                    if ( !decl )
                        error(x,"Cannot find the ARRAY constructor's type");
                    
                    
                    array_elem_type = GO( pick_ast_comp(decl,"type") );
                    
                    ail = args(pick_ast_comp(x,"array-init-list"));
                    for(;ail;ail=ail->next){
                        t0 = GO( pick_ast_comp(ELEM(ail),"expression-count") );
                        t1 = GO( pick_ast_comp(ELEM(ail),"expression-instance") );
                        if ( t0 != basic_int )
                            error(x,"Counter in ARRAY constructor must be of INT type");
                        if ( t1 != array_elem_type )
                            error(x,"Type in constructor and type in ARRAY definition don't match");
                    }
                    
                    result = decl;
                    break;
                case IntConst:
                    result = basic_int;
                    append_ast(x,result);
                    break;
                case RealConst:
                    result = basic_real;
                    append_ast(x,result);
                    break;
                case StringConst:
                    result = basic_str;
                    append_ast(x,result);
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
                // right??
                    result = GO_PICK(0);
                    break;
                case Var:                    
                    id = ast_var(pick_ast_comp(x,"ID"));                    
                    if ( same_name(id,"TRUE") || same_name(id,"FALSE") ){
                        result = basic_bool;
                        append_ast(x,result);
                    }else{
                        decl = lookup(id,&level);
                        if ( !decl )
                            error(x,"Unknow variable name");
                        else{
                            result = GO( pick_ast_comp(decl,"type") );
                        }
                        append_ast(x,result);                        
                        append_ast(x,mk_int(curr_level()-ast_int(pick_ast_comp(decl,"level"))));
                        append_ast(x,mk_int(ast_int(pick_ast_comp(decl,"offset"))));                        
                    }
                    
                    break;
                case ArrayDeref:
                    t0 = GO_PICK_COMP("lvalue");
                    t1 = GO_PICK_COMP("expression");
                    
                    if ( t1 == no_type )
                        ;
                    else if ( t1 != basic_int )
                        error(x,"Array index can only be of INT type");
                    else if ( t0 == no_type )
                        ;
                    else if ( t0->info.node.tag != ArrayTyp )
                        error(x,"Array dereference can only be apply to ARRAY type");
                    else
                        result = _check_type( pick_ast_list(t0->info.node.arguments,0) );
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
                    assert(0);            
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
                    result = void_type;
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
    basic_int  = mk_node(NamedTyp,cons(mk_var("basic_int"),NULL));
    basic_real = mk_node(NamedTyp,cons(mk_var("basic_real"),NULL));
    basic_bool = mk_node(NamedTyp,cons(mk_var("basic_bool"),NULL));
    basic_str  = mk_node(NamedTyp,cons(mk_var("basic_str"),NULL));
    no_type    = mk_node(NamedTyp,cons(mk_var("no_type"),NULL));
    need_infer = mk_node(NamedTyp,cons(mk_var("need_infer"),NULL));
    void_type  = mk_node(NamedTyp,cons(mk_var("void_type"),NULL));
    
    // recursively check
    has_error = 0;
    scope_init();    
    scope_offset_top = 0;

//    printf("========== Type Checking Start ========\n");
    _check_type(x);    
    if ( has_error )
        printf("Type Checking finished with *ERRORS*\n");
    else
        printf("Type Checking finished *SUCCESSFULLY*\n");
//    printf("==========  Type Checking End  ========\n");

    return has_error;
}

