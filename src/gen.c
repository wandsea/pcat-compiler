#include "gen.h"

#include "table.h"
#include "ast.h"
#include "type.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *code_out, *data_out;

int label_count;
char * make_label(){
    label_count ++;
    
    char * p = malloc( 30 );
    sprintf(p,"%d",label_count);
    return p;
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
            switch (x->info.node.tag){
                case Program: 
                    begin_scope(); 
                    GO(0);    // body
                    end_scope();
                    break;
                case BodyDef:
                    GOPICK(0); // declarations-list
                    GOPICK(1); // statements-list
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

                    break;
                case TypeDec:                
                    break;
                case ProcDec:       
                    break;
                case NamedTyp:
                    break;
                case ArrayTyp:
                    break;
                case RecordTyp:
                /*
                    // Not Implemented!
                */
                    break;
                case NoTyp:
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
                    break;
                case Param:
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