%{
#include <stdio.h>

static void DoBeforeEachAction();
#define YY_USER_ACTION DoBeforeEachAction();
%}

%x COMMENT

%%

<*>"\n" {
  lineno++;
  columnno=1;
}

[ ]+ ;

<*>[\t] { columnno += 8 - columnno%8 + 1; }

"(*" { 
  BEGIN(COMMENT); 
}
<COMMENT>"*)" { 
  BEGIN(INITIAL); 
}
<COMMENT><<EOF>> { 
  printf("%d:%d Unclosed comment.\n",lineno,columnno-yyleng);
  return EOFF; 
}
<COMMENT>. ;

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
\\	return BACKSLASH;
"="	return EQ;
"<>"	return NEQ;
"<"	return LT;
"<="	return LE;
">"	return GT;
">="	return GE;
"[<"	return LABRACKET;
">]"	return RABRACKET; 

[a-zA-Z_][a-zA-Z_0-9]* {
  if(yyleng>255){
    printf("%d:%d Identifier too long.\n",lineno,columnno-yyleng);
    return ERROR;
  }
  return IDENTIFIER;
}

[0-9]+\.[0-9]* {
  return REALT;
}


[0-9]+ {
  if(yyleng>10||(yyleng==10&&strcmp(yytext,"2147483647")>0)){
    printf("%d:%d Integer out of range.\n",lineno,columnno-yyleng);
    return ERROR;
  }
  return INTEGERT;
}

\"[^\"\n]*\" {
  if(yyleng>257){
    printf("%d:%d String too long.\n",lineno,columnno-yyleng);
    return ERROR;
  }
  for(int i=0;i<yyleng;i++)
    if(yytext[i]<32||yytext[i]==127){
      printf("%d:%d Invaild char.\n",lineno,columnno-yyleng);
      yytext[i]=' ';
    }
  return STRINGT;
}

\"[^\"\n]* {
  printf("%d:%d Unclosed string.\n",lineno,columnno-yyleng);
  return ERROR;
}


<<EOF>> return EOFF;

. {
  printf("%d:%d UnrecogChar.\n",lineno,columnno-yyleng);
  }

%%

int yywrap(){
    return 1;
}


static void DoBeforeEachAction()
{
  columnno += yyleng;

}
