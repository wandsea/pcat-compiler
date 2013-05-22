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
    
}

void store_int( char* reg, ast* x ){
    
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
            char *l2,*l3;
            switch (x->info.node.tag){
                case Program: 
                    fprintf(code_out,"_%s:\n","MainEntry");  // name for main
                    GO(0);    // body
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
                    // name for sub-routine
                    GOPICK(3); // generate code for body    
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
                    break;
                case CallSt:
                    break;
                case ReadSt:
                    break;
                case WriteSt:
                    break;
                case IfSt:
                    break;
                case WhileSt:
                    break;
                case LoopSt:
                    break;
                case ForSt:
                    break;
                case ExitSt:
                    break;
                case RetSt:
                    break;
                case SeqSt:                  
                    break;
                case ExprList:
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
                                fprintf(code_out,"\t addl %%eax, %%ecx\n");
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
                            
                            fprintf(code_out,"\t cmpl %%eax, %%ebx\n");
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
                    }
                         /*
                         
                             tag(pick_ast(x,0)) == And||
                             tag(pick_ast(x,0)) == Or 
                         */
                    break;
                case UnOpExp:
                    break;
                case LvalExp:
                    break;
                case CallExp:   
                    break;
                case RecordExp:
                /*
                    // Not Implemented !!
                */
                    break;
                case ArrayExp:   
                    break;
                case IntConst:
                    break;
                case RealConst:
                    break;
                case StringConst:
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
                    break;
                case Var:                    
                    break;
                case ArrayDeref:
                /*
                    // Not Implemented !!
                */                
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
                    break;
                
                case TypeInferenceNeeded:
                    break;
                case VoidType:
                    break;
                case EmptyStatement:
                    break;
                case EmptyExpression:
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