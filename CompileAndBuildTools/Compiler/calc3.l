%{
#include <stdlib.h>
#include <string.h>
#include "calc3_defs.h"
#include "calc3.hpp"


void yyerror(char *);
%}


%option yylineno

DIGIT [0-9]
LETTER_LOW [a-z]
LETTER_BIG [A-Z]

%%
            
[0-9]{DIGIT}*	{
					yylval.iValue = atoi(yytext);
					return INTEGER;
				}
				
{DIGIT}+.{DIGIT}+	{
						yylval.iValue = (int)atof(yytext); 
						return FLOAT;	
					}
					
"]"				{ return *yytext; }
"["				{ return *yytext; }

[-()<>=+*/;{}.,":@]	{
						return *yytext;
					}

"int"			{ yylval.iDataType = ETYPE_INT; return DTYPE_INT; }
"char"			{ yylval.iDataType = ETYPE_CHAR; return DTYPE_CHAR; }
"bool"			{ yylval.iDataType = ETYPE_BOOL; return DTYPE_BOOL; }
"string"		{ yylval.iDataType = ETYPE_STRING; return DTYPE_STRING; }
"float"			{ yylval.iDataType = ETYPE_FLOAT;	return DTYPE_FLOAT; }

"int[]"			{ yylval.iDataType = ETYPE_ARRAY_INT; return DTYPE_ARRAY_INT; }
"float[]"		{ yylval.iDataType = ETYPE_ARRAY_FLOAT; return DTYPE_ARRAY_FLOAT; }
"char[]"		{ yylval.iDataType = ETYPE_ARRAY_CHAR; return DTYPE_ARRAY_CHAR; }
"bool[]"		{ yylval.iDataType = ETYPE_ARRAY_BOOL; return DTYPE_ARRAY_BOOL; }
"string[]"		{ yylval.iDataType = ETYPE_ARRAY_STRING; return DTYPE_ARRAY_STRING; }

"buffer"		{ yylval.iDataType = ETYPE_DATA_BUFFER; return DTYPE_DATA_BUFFER; }
					
"$"				return COMPOSITION_DIAGONAL;					
"#"				return COMPOSITION_HORIZONTAL;
"%"				return COMPOSITION_VERTICAL;

"nil"			return NIL;

"listen"		return LISTEN;
"read"			return READ;
"speak"			return SPEAK;
"write"			return WRITE;

"while"         return WHILE;
"for"			return FOR;
"if"            return IF;
"else"          return ELSE;
"print"         return PRINT;
"module"		return MODULE;
"for_s"			return FORNORMAL_S;
"for_t"			return FORNORMAL_T;
"for_st"		return FORNORMAL_ST;
"foreach_s"		return FOREACH_S;
"foreach_t"		return FOREACH_T;
"foreach_st"	return FOREACH_ST;
"while_s"		return WHILE_S;
"while_t"		return WHILE_T;
"while_st"		return WHILE_ST;

"MASTER"		return EXEC_TARGET_MASTER;

"&&"			return AND;
"||"			return OR;

"mod"			return MOD;
">="            return GE;
"<="            return LE;
"=="            return EQ;
"!="            return NE;


{LETTER_LOW}+{DIGIT}*	{
						if (strlen(yytext) >= MAX_IDENTIFIER_SIZE)
						{
							yyerror("Error identifier %s is too long Check if the error line appears or not !!");
							printf("%s\n", yytext);
						} 
						
						strcpy_s(yylval.identifier, MAX_IDENTIFIER_SIZE - 1, yytext);
						return IDENTIFIER_LOW;
					}

{LETTER_BIG}+{DIGIT}*	{
						if (strlen(yytext) >= MAX_IDENTIFIER_SIZE)
						{
							yyerror("Error identifier %s is too long Check if the error line appears or not !!");
							printf("%s\n", yytext);
						} 
						
						strcpy_s(yylval.identifier, MAX_IDENTIFIER_SIZE - 1, yytext);
						return IDENTIFIER_BIG;
					}

[ \t\n]+        ;       /* ignore whitespace */            
.               { 
					yyerror("Uknown character. Check if the error line appears or not !!\n");
					printf("%s\n", yytext);
				}
				
%%

int yywrap(void) {
    return 1;
}

