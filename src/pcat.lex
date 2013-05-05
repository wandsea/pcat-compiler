/********************************************************************************
*
* File: pcat.lex
* Title: The PCAT scanner
* Student names and IDs: 
*
********************************************************************************/
%{
  #include <stdlib.h>
  #include "y.tab.h"
  extern int lineno;
  //extern int yylloc;
  void yyerror(char *);
  int yycolumn = 1;
  #define YY_USER_ACTION yylloc.first_line = yylloc.last_line = lineno; \
  yylloc.first_column = yycolumn; yylloc.last_column = yycolumn + yyleng - 1; \
  yycolumn += yyleng;
%}
%option noyywrap
%x COMMENT

%%
"(*" { 
  BEGIN(COMMENT);
  yymore(); 
 }

<COMMENT>"*)" { 
  BEGIN(INITIAL); 
 }

<COMMENT><<EOF>> { 
  yyerror("Unclosed comment.\n");
  return EOFF; 
 }
<COMMENT>. {
  yymore();
 }
<COMMENT>"\n" { 
  yycolumn = 1;
  lineno++;
  yymore(); 
 }

<*>"\n" {
  yycolumn = 1;
  lineno++;
 }

[ ]+ ;

<*>[\t] { 
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
">"	return GT;
">="	return GE;
"[<"	return LABRACKET;
">]"	return RABRACKET; 

[a-zA-Z_][a-zA-Z_0-9]* {
  if(yyleng>255){
    yyerror("Identifier too long.\n");
    return ERROR;
  }
  return ID;
}

[0-9]+\.[0-9]* {
  return REALT;
}

[0-9]+ {
  if(yyleng>10||(yyleng==10&&(strcmp(yytext,"2147483647")>0))){
    yyerror("Integer out of range.\n");
    return ERROR;                  
  } 
  return INTEGERT;
}


\"[^\"\n]*\" {
  if(yyleng>257){
    yyerror("String too long.\n");
    return ERROR;
  }
  for(int i=0;i<yyleng;i++)
    if(yytext[i]<32||yytext[i]==127){
      yyerror("Invaild char.\n");
      yytext[i]=' ';
    }
  return STRINGT;
}

\"[^\"\n]* {
  yyerror("Unclosed string.\n");
  return 0;
}


<<EOF>> return 0;

. {
    yyerror("UnrecogChar.\n");
  }

%%

