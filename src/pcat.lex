%{
#include <stdio.h>
#include <stdlib.h>
  
#include "util.h"

static void DoBeforeEachAction();
#define YY_USER_ACTION DoBeforeEachAction();
  
int tCol=0;
%}

%x COMMENT

%%


"(*" { 
  BEGIN(COMMENT);
  tCol=columnno;
  yymore(); 
 }

<COMMENT>"*)" { 
  tCol+=2;
  printf(FMT_POS"Ln:%d\tCol:%d\t"
    FMT_CMT" Comment: %s \n"FMT_RESET,
    lineno,tCol-1,
    yytext);
  BEGIN(INITIAL); 
 }

<COMMENT><<EOF>> { 
  printf(FMT_POS"Ln:%d\tCol:%d\t"
    FMT_ERR" Unclosed comment.\n"FMT_RESET,
    lineno,tCol);
  return EOFF; 
 }
<COMMENT>. {
  yymore();
  tCol++;
 }
<COMMENT>"\n" { 
  yymore(); 
  lineno++;  
  columnno=1; 
  tCol=1;
}

<*>"\n" {
  lineno++;
  columnno=1;
}

[ ]+ ;

<*>[\t] { 
  columnno += 8 - columnno%8 + 1; 
}

"PROGRAM"    return PROGRAM;
"IS"         return IS;
"BEGIN"      return BEGINT;
"END"        return END;
"VAR"        return VAR;
"TYPE"       return TYPE;
"PROCEDURE"  return PROCEDURE;
"ARRAY"      return ARRAY;
"RECORD"     return RECORD;
"IN"         return IN;
"OUT"        return OUT;
"READ"       return READ;
"WRITE"      return WRITE;
"IF"         return IF;
"THEN"       return THEN;
"ELSE"       return ELSE;
"ELSIF"      return ELSIF;
"WHILE"      return WHILE;
"DO"         return DO;
"LOOP"       return LOOP;
"FOR"        return FOR;
"EXIT"       return EXIT;
"RETURN"     return RETURN;
"TO"         return TO;
"BY"         return BY;
"AND"        return AND;
"OR"         return OR;
"NOT"        return NOT;
"OF"         return OF;
"DIV"        return DIV;
"MOD"        return MOD;


"("	return LPAREN;
")"	return RPAREN;
"["	return LBRACKET;
"]"	return RBRACKET;
"{"	return LBRACE;
"}"	return RBRACE;
":"	return COLON;
"."	return DOT;
";"	return SEMICOLON;
","	return COMMA;
":="	return ASSIGN;
"+"	return PLUS;
"-"	return MINUS;
"*"	return STAR;
"/"	return SLASH;
"\\"	return BACKSLASH;
"="	return EQ;
"<>"	return NEQ;
"<"	return LT;
"<="	return LE;
">" return GT;
">="	return GE;
"[<"	return LABRACKET;
">]"	return RABRACKET; 

[a-zA-Z_][a-zA-Z_0-9]* {
  if(yyleng>255){
    printf(FMT_POS"Ln:%d\tCol:%d\t"
      FMT_ERR" Identifier too long.\n"FMT_RESET,
      (int)lineno,int(columnno-yyleng));
    return ERROR;
  }
  return IDENTIFIER;
}

[0-9]+\.[0-9]*   return REALT;

[0-9]+ {
  if(yyleng>10||(yyleng==10&&(strcmp(yytext,"2147483647")>0))){
    printf(FMT_POS"Ln:%d\tCol:%d\t"
      FMT_ERR" Integer out of range.\n"FMT_RESET,
      (int)lineno,int(columnno-yyleng));
    return ERROR;                  
  } 
  return INTEGERT;
}


\"[^\"\n]*\" {
  if(yyleng>257){
    printf(FMT_POS"Ln:%d\tCol:%d\t"
      FMT_ERR" String too long.\n"FMT_RESET,
      (int)lineno,int(columnno-yyleng));
    return ERROR;
  }
  for(int i=0;i<yyleng;i++)
    if(yytext[i]<32||yytext[i]==127){
      printf(FMT_POS"Ln:%d\tCol:%d\t"
        FMT_ERR" Invaild char.\n"FMT_RESET,
        (int)lineno,int(columnno-yyleng));
      yytext[i]=' ';
    }
  return STRINGT;
}

\"[^\"\n]* {
  printf(FMT_POS"Ln:%d\tCol:%d\t"
    FMT_ERR" Unclosed string.\n"FMT_RESET,
    (int)lineno,int(columnno-yyleng));
  return ERROR;
}


<<EOF>> return EOFF;

. {
  printf(FMT_POS"Ln:%d\tCol:%d\t"
    FMT_ERR" UnrecogChar.\n"FMT_RESET,
    (int)lineno,int(columnno-yyleng));
  }

%%

int yywrap(){
    return 1;
}


static void DoBeforeEachAction()
{
  columnno += yyleng;
}
