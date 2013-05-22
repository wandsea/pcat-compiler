#include "gen.h"

#include "table.h"
#include "ast.h"
#include "type.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *code_out, *data_out;

int label_count;
char * make_label(){
    label_count ++;
    
    char * p = malloc( 30 );
    sprintf(p,"label_%d",label_count);
    return p;
}

void load_int( ast* x, char* reg ){
    int level_diff,offset;
    int i;
    switch( tag(x) ){
        case Var:
            level_diff = ast_int( pick_ast(x,1) );
            offset = ast_int( pick_ast(x,2) );
            assert(level_diff>=0);
            if ( level_diff == 0 )
                fprintf(code_out,"\t movl %d(%%ebp) %s\n",offset,reg);
            else{
                fprintf(code_out,"\t movl (%%ebp), %%edx\n");
                for(i = 0; i < level_diff-1; i++ )
                    fprintf(code_out, "\t movl (%%edx), %%edx");
                fprintf(code_out,"\t movl %d(%%ebp) %s\n",offset,reg);
            }
            break;
        case BinOpExp:
            offset = ast_int( pick_ast(x,4) );
            fprintf(code_out,"\t movl %d(%%ebp) %s\n",offset,reg);
            break;
        case UnOpExp:
            offset = ast_int( pick_ast(x,3) );
            fprintf(code_out,"\t movl %d(%%ebp) %s\n",offset,reg);
            break;
        case CallExp:
            offset = ast_int( pick_ast(x,3) );
            fprintf(code_out,"\t movl %d(%%ebp) %s\n",offset,reg);
            break;
        case IntConst:
            fprintf(code_out,"\t movl $%d, %s\n",ast_int(pick_ast(x,1)),reg);
            break;
        case RealConst:
            assert(0); // shouln't be here
            // cannot load real as int
            break;
        case StringConst:
            assert(0); // shouln't be here
            // cannot load real as int
            break;
        default: break;
    }
}

void store_int( char* reg, ast* x ){
    
}

void store_int_lvalue( char* reg, ast* x ){
    
}

void load_float( ast* x ){
    
}

void store_float( ast* x ){
    
}

// change: to generate intermediate code
void _gen_code( ast* x ){
    #define GOPICK(k)      _gen_code( pick_ast(x,k) )
    #define GO(e)          _gen_code( e )
    #define FOREACH(x)     for(l=args(x);l;l=l->next)
    #define ELEM(l)        l->elem
    #define ELEML          ELEM(l)

    if ( x == NULL )
        ;
    else switch (x->tag){
        case int_ast:            
            break;
        case real_ast:          
            break;
        case var_ast:    
            break;
        case str_ast:      
            break;
        case node_ast : {
            ast_list * l;
            ast *t,*t1,*t2;
            char *l2,*l3,*l4;
            int i;
            int level_diff;
            ast *var;
            switch (x->info.node.tag){
                case Program: 
                    fprintf(code_out,"_%s:\n","MainEntry");  // name for main
                    fprintf(code_out,"\t pushl %%ebp\n\t movl %%esp, %%ebp\n");// prologue
                    fprintf(code_out,"\t andl $-16, %%esp\n");
                    fprintf(code_out,"\t subl $%d, %%esp\n",ast_int(pick_ast(x,5)));
                    fprintf(code_out,"\t andl $-16, %%esp\n");
                    GO(0);    // body
                    fprintf(code_out,"\t leave\n\t ret\n");// epilogue
                    break;
                case BodyDef:
                    GOPICK(0); // declarations-list
                    GOPICK(1); // statements-list
                    break;
                case DeclareList:   
                    // type & var not need for generating code
                    // only generate code of sub-routine 
                    FOREACH(x) if (tag(ELEML)==ProcDecs) GO(ELEML); 
                    break;
                case VarDecs:
                    // Shouldn't reach here
                    assert(0);
                    break;
                case TypeDecs:
                    // Shouldn't reach here
                    assert(0);
                    break;       
                case ProcDecs:
                    FOREACH(x) GO(ELEML);
                    break;
                case VarDec:
                    // Shouldn't reach here
                    assert(0);
                    break;
                case TypeDec:    
                    // Shouldn't reach here
                    assert(0);            
                    break;
                case ProcDec:   
                    fprintf(code_out,"_%s:\n",ast_str(pick_ast(x,0)));  
                    fprintf(code_out,"\t pushl %%ebp\n\t movl %%esp, %%ebp\n");// prologue
                    fprintf(code_out,"\t andl $-16, %%esp\n");
                    fprintf(code_out,"\t subl $%d, %%esp\n",ast_int(pick_ast(x,5)));
                    fprintf(code_out,"\t andl $-16, %%esp\n");
                    // name for sub-routine
                    GOPICK(3); // generate code for body   
                    fprintf(code_out,"\t leave\n\t ret\n");// epilogue 
                    break;
                case NamedTyp: 
                    // Shouldn't reach here
                    assert(0); 
                    break;
                case ArrayTyp: 
                    // Shouldn't reach here
                    assert(0); 
                    break;
                case RecordTyp:
                /*
                    // Not Implemented!
                */ 
                    // Shouldn't reach here
                    assert(0); 
                    break;
                case NoTyp: 
                    // Shouldn't reach here
                    assert(0); 
                    break;
                case CompList:
                /*
                    // Not Implemented!
                */ 
                    // Shouldn't reach here
                    assert(0); 
                    break;
                case Comp:
                /*
                    // Not Implemented!
                */ 
                    // Shouldn't reach here
                    assert(0); 
                    break;
                case FormalParamList: 
                    // Shouldn't reach here
                    assert(0); 
                    break;
                case Param: 
                    // Shouldn't reach here
                    assert(0); 
                    break;
                case AssignSt:
                    GOPICK(1);
                    load_int(pick_ast(x,0),"%eax");
                    store_int_lvalue("%eax",pick_ast(x,0));
                    break;
                case CallSt:
                // synchronize with CallExpr
                    FOREACH(pick_ast(x,1))
                        _gen_code(ELEML);
                    // static link
                    level_diff = ast_int( pick_ast(x,4) );
                    if ( level_diff == -1 )
                        fprintf(code_out,"\t pushl %%ebp\n");
                    else{
                        fprintf(code_out,"\t movl (%%ebp), %%edx\n");
                        for(i = 0; i < level_diff; i++ )
                            fprintf(code_out, "\t movl (%%edx), %%edx\n");
                        fprintf(code_out,"\t pushl %%edx\n");
                    }
                    // parameters
                    FOREACH(pick_ast(x,1)){
                        load_int(ELEML,"%eax");
                        fprintf(code_out,"\t pushl %%eax\n");
                    }
                    fprintf(code_out,"\t call _%s\n",ast_str(pick_ast(x,0)));
                    fprintf(code_out,"\t addl $%d, %%esp\n",4+4*length(args(pick_ast(x,1))));

                    break;
                case ReadSt:
                    FOREACH(pick_ast(x,0)){
                        // TODO
                    }
                    break;
                case WriteSt:
                    FOREACH(pick_ast(x,0)){
                        // TODO
                    }
                    break;
                case IfSt:
                /*
                    If A then B else C:
                    
                    [get A]
                    cmpl $0,A
                    jne L2
                    C
                    jmp L3:
                L2: 
                    B
                L3:
                */
                    l2 = make_label(); l3 = make_label();

                    GOPICK(0);
                    load_int(pick_ast(x,0),"%eax");
                    fprintf(code_out,"\t cmpl $0, %%eax\n");
                    fprintf(code_out,"\t jne %s\n",l2);
                    GOPICK(2);
                    fprintf(code_out,"\t jmp %s\n",l3);
                    fprintf(code_out,"%s:\n",l2);
                    GOPICK(1);
                    fprintf(code_out,"%s:\n",l3);
                    break;
                case WhileSt:
                /*
                    while A do B

                L2:
                    [get A]
                    cmpl $0,A
                    je L3
                    B
                    jmp L2;
                L3:
                */
                    l2 = make_label(); l3 = make_label();
                    fprintf(code_out,"%s:\n",l2);
                    GOPICK(0);
                    load_int(pick_ast(x,0),"%eax");
                    fprintf(code_out,"\t cmpl $0, %%eax\n");
                    fprintf(code_out,"\t je %s\n",l3);
                    GOPICK(1);
                    fprintf(code_out,"\t jmp %s\n",l2);
                    fprintf(code_out,"%s:\n",l3);
                    break;
                case LoopSt:
                /*
                    Loop B:

                L2:
                    B
                    jmp L2;
                */
                    l2 = make_label();
                    fprintf(code_out,"%s:",l2);
                    GOPICK(0);
                    fprintf(code_out,"\t jmp %s\n",l2);
                    break;
                case ForSt:
                /*
                    For A := B to C by D do E:

                    A := B
                    while A <= C:
                        E
                        A = A + E

                    [A := B]
                L2:
                    [test A,C]
                    je L3
                    E
                    [ A = A + D ]
                    jmp L2
                L3;
                */
                    l2 = make_label(); l3 = make_label();
                    var = mk_node(Var,
                                  cons(pick_ast(x,0),
                                       cons(mk_int(0),
                                            cons(pick_ast(x,5),
                                                 NULL))));

                    _gen_code(mk_node(AssignSt,
                                      cons(var,
                                           cons(pick_ast(x,1),
                                                NULL))));
                    fprintf(code_out,"%s:",l2);
                    GOPICK(2);
                    load_int(var,"%eax");
                    load_int(pick_ast(x,2),"%ecx");
                    fprintf(code_out,"\t testl %%eax, %%ecx\n");
                    fprintf(code_out,"\t je %s\n",l3);
                    GOPICK(4);
                    GOPICK(3);
                    load_int(var,"%eax");
                    load_int(pick_ast(x,3),"%ecx");
                    fprintf(code_out,"\t addl %%ecx, %%eax\n");
                    store_int("%eax",var);
                    fprintf(code_out,"\t jmp %s\n",l2);
                    fprintf(code_out,"%s:",l3);
                    break;
                case ExitSt:
                    fprintf(code_out,"\t leave\n\t ret\n");// epilogue 
                    break;
                case RetSt:
                    if ( tag(pick_ast(x,0)) == EmptyExpression )
                        ;
                    else{
                        GOPICK(0);
                        load_int(pick_ast(x,0),"%eax");
                    } 
                    fprintf(code_out,"\t leave\n\t ret\n");// epilogue 
                    break;
                case SeqSt:  
                    FOREACH(x) _gen_code(ELEML);                
                    break;
                case ExprList:
                    assert(0);
                    break;
                /*
                    For expression, return no-type if something wrong or some 
                        component is of no-type, which means I couldn't
                        handle the type of this expression.
                */
                case BinOpExp:                    
                    // result type
                    t = pick_ast(x,4);
                    t1 = pick_ast(pick_ast(x,1),4);
                    t2 = pick_ast(pick_ast(x,2),4);
                    
                    if ( tag(pick_ast(x,0)) == Plus ||
                         tag(pick_ast(x,0)) == Minus ||
                         tag(pick_ast(x,0)) == Times ) {
                    // Arithmic, integer/real
                        // value of two sub-expr
                        GOPICK(1);
                        GOPICK(2);
                        if ( same_name(ast_str(pick_ast(t,0)),"basic_int") ){
                            load_int(pick_ast(x,1),"%eax");
                            load_int(pick_ast(x,2),"%ecx");
                            
                            if ( tag(pick_ast(x,0)) == Plus )
                                fprintf(code_out,"\t addl %%ecx, %%eax\n");
                            else if ( tag(pick_ast(x,0)) == Minus ){
                                fprintf(code_out,"\t subl %%eax, %%ecx\n");
                                fprintf(code_out,"\t movl %%ecx, %%eax\n");
                            }else if ( tag(pick_ast(x,0)) == Times )
                                fprintf(code_out,"\t imull %%ecx, %%eax\n");
                            else
                                assert(0); // shouldn't be here!
                                
                            store_int("%eax",x);
                        }else if ( same_name(ast_str(pick_ast(t,0)),"basic_float") ){
                            load_float(pick_ast(x,1));
                            load_float(pick_ast(x,2));
                            
                            if ( tag(pick_ast(x,0)) == Plus )
                                fprintf(code_out,"\t faddp %%st, %%st(1)\n");
                            else if ( tag(pick_ast(x,0)) == Minus ){
                                fprintf(code_out,"\t fsubp %%st, %%st(1)\n");
                            }else if ( tag(pick_ast(x,0)) == Times )
                                fprintf(code_out,"\t fmulp %%st, %%st(1)\n");
                            else
                                assert(0); // shouldn't be here!
                            
                            store_float(x);
                        }else
                            assert(0); // shouldn't be here!
                    }else if (tag(pick_ast(x,0)) == Div ||
                              tag(pick_ast(x,0)) == Mod){                    
                    // Arithmic, integer
                        // value of two sub-expr
                        GOPICK(1);
                        GOPICK(2);
                        if ( same_name(ast_str(pick_ast(t,0)),"basic_int") ){
                            load_int(pick_ast(x,1),"%eax");
                            load_int(pick_ast(x,2),"%ecx");
                            
                            if ( tag(pick_ast(x,0)) == Div ){             
                                fprintf(code_out,"\t cltd\n");  
                                fprintf(code_out,"\t idivl %%ecx\n");
                            }else if ( tag(pick_ast(x,0)) == Mod ){             
                                fprintf(code_out,"\t cltd\n");  
                                fprintf(code_out,"\t idivl %%ecx\n");
                                fprintf(code_out,"\t movl %%edx %%eax\n");
                            }else
                                assert(0); // shouldn't be here!
                                
                            store_int("%eax",x);                            
                         }else
                            assert(0); // shouldn't be here!
                    }else if ( tag(pick_ast(x,0)) == Slash ){             
                    // Arithmic, real
                        // value of two sub-expr
                        GOPICK(1);
                        GOPICK(2);
                        if ( same_name(ast_str(pick_ast(t,0)),"basic_real") ){
                            load_float(pick_ast(x,1));
                            load_float(pick_ast(x,2));
                            
                            fprintf(code_out,"\t fdivrp %%st, %%st(1)\n");
                                
                            store_float(x);                            
                        }else
                            assert(0); // shouldn't be here!
                    }else if ( tag(pick_ast(x,0)) == Gt ||
                             tag(pick_ast(x,0)) == Lt ||
                             tag(pick_ast(x,0)) == Eq ||
                             tag(pick_ast(x,0)) == Ge ||
                             tag(pick_ast(x,0)) == Le ||
                             tag(pick_ast(x,0)) == Ne ){          
                    // Comparation
                        // value of two sub-expr
                        GOPICK(1);
                        GOPICK(2);
                        if ( same_name(ast_str(pick_ast(t1,0)),"basic_int") && 
                             same_name(ast_str(pick_ast(t2,0)),"basic_int") ){
                            load_int(pick_ast(x,1),"%eax");
                            load_int(pick_ast(x,2),"%ecx");                            
                            
                            fprintf(code_out,"\t cmpl %%eax, %%ecx\n");
                            switch (tag(pick_ast(x,0))){
                                case Gt: fprintf(code_out,"\t setg %%al\n"); break;
                                case Lt: fprintf(code_out,"\t setl %%al\n"); break;
                                case Eq: fprintf(code_out,"\t sete %%al\n"); break;
                                case Ge: fprintf(code_out,"\t setge %%al\n"); break;
                                case Le: fprintf(code_out,"\t setle %%al\n"); break;
                                case Ne: fprintf(code_out,"\t setne %%al\n"); break;
                                default: break;
                            }
                            fprintf(code_out,"\t movzbl	%%al, %%eax\n");
                            
                            store_int("%eax",x);
                        }else{
                            load_float(pick_ast(x,1));
                            load_float(pick_ast(x,2));
                            
                            fprintf(code_out,"\t fucomip %%st(1), %%st\n");
                            switch (tag(pick_ast(x,0))){
                                case Gt: fprintf(code_out,"\t setg %%al\n"); break;
                                case Lt: fprintf(code_out,"\t setl %%al\n"); break;
                                case Eq: fprintf(code_out,"\t sete %%al\n"); break;
                                case Ge: fprintf(code_out,"\t setge %%al\n"); break;
                                case Le: fprintf(code_out,"\t setle %%al\n"); break;
                                case Ne: fprintf(code_out,"\t setne %%al\n"); break;
                                default: break;
                            }
                            fprintf(code_out,"\t movzbl	%%al, %%eax\n");
                            
                            store_float(x);   
                        }
                    } if ( tag(pick_ast(x,0)) == And ||
                           tag(pick_ast(x,0)) == Or ){
                    // Boolean operation: and / or
                        // value of two sub-expr
                        if ( tag(pick_ast(x,0)) == And ){
                            l2 = make_label(); l3 = make_label();
                            
                            GOPICK(1);
                            load_int(pick_ast(x,1),"%eax");
                            fprintf(code_out,"\t cmpl 0, %%eax\n");
                            fprintf(code_out,"\t je %s\n",l2);                        
                            GOPICK(2);
                            load_int(pick_ast(x,2),"%eax");
                            fprintf(code_out,"\t cmpl 0, %%eax\n");
                            fprintf(code_out,"\t je %s\n",l2);    
                            fprintf(code_out,"\t movl $1, %%eax\n");    
                            fprintf(code_out,"\t jmp %s\n",l3);
                            fprintf(code_out,"%s:\n",l2);
                            fprintf(code_out,"\t movl $0, %%eax\n");                         
                            fprintf(code_out,"%s:\n",l3);
                        }else{                            
                            l2 = make_label(); l3 = make_label(); l4 = make_label();
                            
                            GOPICK(1);
                            load_int(pick_ast(x,1),"%eax");
                            fprintf(code_out,"\t cmpl 0, %%eax\n");
                            fprintf(code_out,"\t jne %s\n",l2);  
                            GOPICK(2);
                            load_int(pick_ast(x,2),"%eax");
                            fprintf(code_out,"\t cmpl 0, %%eax\n");
                            fprintf(code_out,"\t je %s\n",l3);
                            fprintf(code_out,"%s:\n",l2);
                            fprintf(code_out,"\t movl $1, %%eax\n");       
                            fprintf(code_out,"\t jmp %s\n",l4);                      
                            fprintf(code_out,"%s:\n",l3);       
                            fprintf(code_out,"\t movl $0, %%eax\n");                 
                            fprintf(code_out,"%s:\n",l4);
                        }
                    }
                    break;
                case UnOpExp:
                    
                    // result type
                    t = pick_ast(x,4);
                    t1 = pick_ast(pick_ast(x,1),4);                    
                    GOPICK(1);
                    if ( tag(pick_ast(x,0)) == UPlus ||
                         tag(pick_ast(x,0)) == UMinus ){
                        if ( same_name(ast_str(pick_ast(t,0)),"basic_int") ){
                            load_int(pick_ast(x,1),"%eax");
                            
                            if ( tag(pick_ast(x,0)) == UPlus )
                                ;
                            else if ( tag(pick_ast(x,0)) == UMinus )
                                fprintf(code_out,"\t negl %%eax\n");
                                
                            store_int("%eax",x);
                        }else if ( same_name(ast_str(pick_ast(t,0)),"basic_real") ){
                            load_float(pick_ast(x,1));
                            
                            if ( tag(pick_ast(x,0)) == UPlus )
                                ;
                            else if ( tag(pick_ast(x,0)) == UMinus )
                                fprintf(code_out,"\t fchs\n");
                                
                            store_float(x);
                        }
                    }else if ( tag(pick_ast(x,0)) == Not ){
                        load_int(pick_ast(x,1),"%eax");
                        fprintf(code_out,"\t cmpl $0, %%eax\n");
                        fprintf(code_out,"\t sete %%al\n");
                        fprintf(code_out,"\t movzbl %%al, %%eax\n");
                        store_int("%eax",x);
                    }
                    break;
                case LvalExp:
                    load_int(pick_ast(x,0),"%eax");
                    store_int("%eax",x);
                    break;
                case CallExp:                
                // synchronize with CallSt
                    FOREACH(pick_ast(x,1))
                        _gen_code(ELEML);
                    // static link
                    level_diff = ast_int( pick_ast(x,4) );
                    if ( level_diff == -1 )
                        fprintf(code_out,"\t pushl %%ebp\n");
                    else{
                        fprintf(code_out,"\t movl (%%ebp), %%edx\n");
                        for(i = 0; i < level_diff; i++ )
                            fprintf(code_out, "\t movl (%%edx), %%edx");
                        fprintf(code_out,"\t pushl %%edx");
                    }
                    // parameters
                    FOREACH(pick_ast(x,1)){
                        load_int(ELEML,"%eax");
                        fprintf(code_out,"\t pushl %%eax\n");
                    }
                    fprintf(code_out,"\t call _%s\n",ast_str(pick_ast(x,0)));
                    fprintf(code_out,"\t addl $%d, %%esp\n",4+4*length(args(pick_ast(x,1))));

                    // store return
                    store_int("%eax",x);
                    break;
                case RecordExp:
                /*
                    // Not Implemented !!
                */
                    break;
                case ArrayExp:   
                    assert(0); // shouln't be here
                    break;
                case IntConst:
                    // do nothing
                    break;
                case RealConst:
                    // do nothing
                    break;
                case StringConst:
                    // do nothing
                    break;
                case RecordInitList:
                    assert(0); // shouln't be here
                    break;
                case RecordInit:
                    assert(0); // shouln't be here
                    break;
                case ArrayInitList:
                    assert(0); // shouln't be here
                    break;
                case ArrayInit:
                    assert(0); // shouln't be here
                    break;
                case LvalList:
                    assert(0); // shouln't be here
                    break;
                case Var:                 
                    // do nothing
                    break;
                case ArrayDeref:
                    assert(0); // shouln't be here
                /*
                    // Not Implemented !!
                */                
                    break;
                case RecordDeref:
                    assert(0); // shouln't be here
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
                    assert(0); // shouln't be here      
                    break;
                
                case TypeInferenceNeeded:                
                    assert(0); // shouln't be here
                    break;
                case VoidType:                
                    assert(0); // shouln't be here
                    break;
                case EmptyStatement:
                    // do nothing
                    break;
                case EmptyExpression:
                    // do nothing
                    break;
            }
            break;
        }
    }
}

void combine(char *s1, char *s2, char* t){
    FILE *fs1 = fopen(s1,"r"),*fs2 = fopen(s2,"r"),*ft = fopen(t,"w");
    char ch;
    if ( fs1 != NULL && fs2 != NULL ){
        while( ( ch = fgetc(fs1) ) != EOF )fputc(ch,ft);
        while( ( ch = fgetc(fs2) ) != EOF )fputc(ch,ft);
    }
    fclose(fs1);fclose(fs2);fclose(ft);
}

void gen_code( ast* x ){
    
    code_out = fopen("code.s","w");
    data_out = fopen("data.s","w");
    
    label_count = 0;
    scope_init();    
        
    _gen_code(x);
    
    fclose(code_out);
    fclose(data_out);
    
    combine("code.s","data.s","pcat.s");
}