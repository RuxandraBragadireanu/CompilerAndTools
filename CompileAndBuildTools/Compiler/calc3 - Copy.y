%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "calc3_defs.h"
#include "calc3_utils.h"
#include "CompilationBlackbox.h"
#include "InputTypes.h"

/* prototypes */

extern "C"
{
	int yylex(void);
	void yyerror(char *s);
}

extern "C" 	FILE *yyin;

%}

%locations

%union {
    int iValue;								/* integer value */
    int iDataType;							/* for data types */
    float fValue;							/* float value */
    void *pNodeModule;						/* node module - program  --- Node */
    void *pExpression;						/* NodeExpression */
    void *pBlockInput;						/* InputBlock*/
    void *pInputItem;						/* IInputItem */
    void *pProcessInput;					/* BaseProcessInput */
    void *pAsignment;						/* Asignment */
    void *pProgramNode;						/* Program node */
    void *pInputOutput;						/* InputOutput */
    char identifier[MAX_IDENTIFIER_SIZE];	/* identifier */
};


%token NIL LISTEN READ SPEAK WRITE COMPOSITION_DIAGONAL COMPOSITION_VERTICAL COMPOSITION_HORIZONTAL WHILE IF PRINT MODULE FOR FOREACH_S FOREACH_T FOREACH_ST FORNORMAL_S FORNORMAL_T FORNORMAL_ST WHILE_S WHILE_T WHILE_ST AND OR EXEC_TARGET_MASTER
%token <iDataType> DTYPE_INT DTYPE_CHAR DTYPE_BOOL DTYPE_FLOAT DTYPE_STRING DTYPE_ARRAY_INT DTYPE_ARRAY_FLOAT DTYPE_ARRAY_BOOL DTYPE_ARRAY_STRING DTYPE_ARRAY_CHAR DTYPE_DATA_BUFFER
%token <iValue> INTEGER
%token <identifier> IDENTIFIER_LOW IDENTIFIER_BIG
%token <fValue> FLOAT

%nonassoc IFX
%nonassoc ELSE

%nonassoc EXPR_WITHOUT_BOOL
%nonassoc ')'

// !!!!!! VERIFY IF THESE WORK, AND IF NOT, GIVE THEM PRIORITIES!!!!!!!!!
//%nonassoc COMPOSITION_DIAGONAL
//%nonassoc COMPOSITION_VERTICAL
//%nonassoc COMPOSITION_HORIZONTAL
%left COMPOSITION_DIAGONAL
%left COMPOSITION_VERTICAL
%left COMPOSITION_HORIZONTAL

%left GE LE EQ NE '>' '<'
%left MOD
%left '+' '-'
%left '*' '/'
%left AND OR
%nonassoc UMINUS
%nonassoc PDEC
%nonassoc PINC
%nonassoc DEC
%nonassoc INC

%type <pNodeModule>		program
%type <pNodeModule>		module
%type <pNodeModule>		module_body
%type <pInputOutput>	module_input
%type <pInputOutput>	module_output
%type <pBlockInput>		processinputlist;
%type <pInputItem>		variable;
%type <pProcessInput>	simplevariablelist;
%type <iDataType>		datatype;
%type <pBlockInput>		singleproceslist;
%type <pExpression>		expr;
%type <pExpression>		boolean_expr;
%type <pAsignment>		asignment;

%%

agapiacode:
        modules_list                
        { 
			ABSTFactory::RunCompilation();
		}
        ;

modules_list:
          modules_list module         
        | module
        ;
        
module:
		MODULE IDENTIFIER_BIG module_input '{' module_body '}' module_output { ABSTFactory::CreateAgapiaModule($2, $3, $5, $7); }
		;
		
module_input:
		'{' LISTEN processinputlist '}' '{' READ processinputlist '}'  
		{ 
			$$ = ABSTFactory::CreateInputBlocks($3, $7);		
			InputBlock* pIB = (InputBlock*) $3; 
			pIB = (InputBlock*) $7; 		
		}
		;
		
module_output:
		'{' SPEAK processinputlist '}' '{' WRITE processinputlist '}'  
		{
			$$ = ABSTFactory::CreateOutputBlocks($3, $7);
		
			InputBlock* pIB = (InputBlock*) $3; pIB = (InputBlock*) $7;
		}
		;
		
module_body:
			program	{ $$ = $1; }
		|	'@' EXEC_TARGET_MASTER	{ $$ = ABSTFactory::CreateCCodeProgramType(E_CCODE_FORMASTER); }
		|   '@'						{ $$ = ABSTFactory::CreateCCodeProgramType(E_CCODE_FORALL); }
		|   NIL						{ $$ = ABSTFactory::CreateIdentityProgram(); }
		; 
		
processinputlist:
			processinputlist ';' singleproceslist    { InputBlock* pIB = (InputBlock*) $1; pIB->AddInput((BaseProcessInput*)$3); $$ = pIB; }
		|	singleproceslist						 { InputBlock* pIB = new InputBlock(); pIB->AddInput((BaseProcessInput*)$1); $$ = pIB; }
		|   NIL										 { InputBlock* pIB = new InputBlock(); $$ = pIB; } 
		;
		
singleproceslist:
			simplevariablelist						 { $$ = $1; }
		|	'(' '(' processinputlist ')' IDENTIFIER_LOW '[' ']' ';' ')' { VectorProcessItem* pVPI = new VectorProcessItem(); pVPI->SetItemType(((InputBlock*)$3)->m_InputsInBlock, $5); $$ = pVPI; }
		;

simplevariablelist:								
			simplevariablelist ',' variable		{ SimpleProcessItem* pSPI = (SimpleProcessItem*) $1; pSPI->AddItem((IDataTypeItem*)$3); $$ = pSPI; }
		|   variable							{ SimpleProcessItem* pSPI = new SimpleProcessItem(); pSPI->AddItem((IDataTypeItem*)$1); $$ = pSPI; }
		;
		
variable:
			IDENTIFIER_LOW ':' datatype { $$ = ItemTypeFactory::CreateInputItem($3, $1); }
		|	IDENTIFIER_LOW ':' datatype '[' ']' { $$ = ItemTypeFactory::CreateInputItem($3, $1); }
		;

datatype:
			DTYPE_INT		{ $$ = $1; }
		|	DTYPE_BOOL		{ $$ = $1; }
		|	DTYPE_STRING	{ $$ = $1; }
		|	DTYPE_FLOAT		{ $$ = $1; }
		|	DTYPE_CHAR		{ $$ = $1; }
		
		|	DTYPE_ARRAY_INT		{ $$ = $1; }
		|	DTYPE_ARRAY_BOOL	{ $$ = $1; }
		|	DTYPE_ARRAY_STRING	{ $$ = $1; }
		|	DTYPE_ARRAY_FLOAT	{ $$ = $1; }
		|	DTYPE_ARRAY_CHAR	{ $$ = $1; }	
		
		| 	DTYPE_DATA_BUFFER   { $$ = $1; }
		;
		
program:
			IDENTIFIER_BIG														{ $$ = ABSTFactory::CreateModuleRef($1); } 
		|   asignment															{ $$ = $1; }
		|	variable															{ $$ = ABSTFactory::CreateDeclarationProgram($1); }
		|	program COMPOSITION_HORIZONTAL program								{ $$ = ABSTFactory::CreateIntermediateProgram($1, $3, E_COMP_HORIZONTAL); }
		|   program COMPOSITION_VERTICAL program								{ $$ = ABSTFactory::CreateIntermediateProgram($1, $3, E_COMP_VERTICAL); }				
		|	program COMPOSITION_DIAGONAL program								{ $$ = ABSTFactory::CreateIntermediateProgram($1, $3, E_COMP_DIAGONAL); }
		|	'(' program ')'														{ $$ = $2; }
		|	IF '(' boolean_expr ')' '{' program '}' %prec IFX {}				{ $$ = ABSTFactory::CreateIFProgram($3, $6, NULL); }
		|	IF '(' boolean_expr ')' '{' program '}' ELSE '{' program '}' { }	{ $$ = ABSTFactory::CreateIFProgram($3, $6, $10); }
		|   WHILE_S '(' boolean_expr ')' '{' program '}'						{ $$ = ABSTFactory::CreateWHILEProgram(DTYPE_WHILE_S, $3, $6);}
		|   WHILE_ST '(' boolean_expr ')' '{' program '}'						{ $$ = ABSTFactory::CreateWHILEProgram(DTYPE_WHILE_ST, $3, $6);}
		|   WHILE_T '(' boolean_expr ')' '{' program '}'						{ $$ = ABSTFactory::CreateWHILEProgram(DTYPE_WHILE_T, $3, $6);}
		|   FOREACH_S '(' expr ')' '{' program '}'  { $$ = ABSTFactory::CreateFOREACHProgram(DTYPE_FOREACH_S, $3, $6); }
		|   FOREACH_T '(' expr ')' '{' program '}' { $$ = ABSTFactory::CreateFOREACHProgram(DTYPE_FOREACH_T, $3, $6); }
		|   FOREACH_ST '(' expr ')' '{' program '}'  { $$ = ABSTFactory::CreateFOREACHProgram(DTYPE_FOREACH_ST, $3, $6); }
		|   FORNORMAL_ST '(' asignment ';' expr ';' expr ')' '{' program '}'  { $$ = ABSTFactory::CreateFORNormalProgram(DTYPE_FORNORMAL_ST, $3, $5, $7, $10); }
		|   FORNORMAL_S '(' asignment ';' expr ';' expr ')' '{' program '}'  { $$ = ABSTFactory::CreateFORNormalProgram(DTYPE_FORNORMAL_S, $3, $5, $7, $10); }
		|   FORNORMAL_T '(' asignment ';' expr ';' expr ')' '{' program '}' { $$ = ABSTFactory::CreateFORNormalProgram(DTYPE_FORNORMAL_T, $3, $5, $7, $10); }
		|	{ } 
		;
		
asignment:
		expr '=' expr { $$ = ABSTFactory::CreateAsignmentProgram($1, $3); }
		;

expr:
          INTEGER			{ $$ = ABSTFactory::CreateAtomicExpression($1); }
        | FLOAT				{ $$ = ABSTFactory::CreateAtomicExpression($1); }
        | IDENTIFIER_LOW		 { $$ = ABSTFactory::CreateAtomicExprIdentifier($1); }
        | IDENTIFIER_LOW '[' expr ']' { $$ = ABSTFactory::CreateArrayAccessExpression($1, $3); }
        | '(' expr ')'          { $$ = $2; }
        | '-' expr %prec UMINUS { $$ = ABSTFactory::CreateValueExpression($2, NULL, E_UNARYMINUS); }
		| expr '+''+' %prec INC { $$ = ABSTFactory::CreateValueExpression($1, NULL, E_OP_INCREMENT); }
		| expr '-''-' %prec PDEC { $$ = ABSTFactory::CreateValueExpression($1, NULL, E_OP_DECREMENT); }
        | expr '+' expr         { $$ = ABSTFactory::CreateValueExpression($1, $3, E_OP_PLUS); }
        | expr '-' expr         { $$ = ABSTFactory::CreateValueExpression($1, $3, E_OP_MINUS);}
        | expr '*' expr         { $$ = ABSTFactory::CreateValueExpression($1, $3, E_OP_MULT);}
        | expr '/' expr         { $$ = ABSTFactory::CreateValueExpression($1, $3, E_OP_DIV);}
        | boolean_expr		%prec EXPR_WITHOUT_BOOL	{ $$ = $1; }
		| expr MOD expr			{ $$ = ABSTFactory::CreateValueExpression($1, $3, E_OP_MOD);}	
        ;
      
boolean_expr:
		  expr '<' expr         { $$ = ABSTFactory::CreateBooleanExpression($1, $3, E_SMALL_THAN);}
        | expr '>' expr         { $$ = ABSTFactory::CreateBooleanExpression($1, $3, E_BIG_THAN);}			
        | expr GE expr          { $$ = ABSTFactory::CreateBooleanExpression($1, $3, E_BIG_OR_EQUAL_THAN);}			
        | expr LE expr          { $$ = ABSTFactory::CreateBooleanExpression($1, $3, E_SMALL_OR_EQUAL_THAN);}			
        | expr NE expr          { $$ = ABSTFactory::CreateBooleanExpression($1, $3, E_DIFFERENT);}			
        | expr EQ expr          { $$ = ABSTFactory::CreateBooleanExpression($1, $3, E_EQUAL);}			
        | expr AND expr			{ $$ = ABSTFactory::CreateBooleanExpression($1, $3, E_AND);}			
        | expr OR expr			{ $$ = ABSTFactory::CreateBooleanExpression($1, $3, E_OR);}			
        | '(' boolean_expr ')'  { $$ = $2;}
		;
		
%%

extern "C" int yylineno;

void yyerror(char *s) 
{
	fprintf(stdout, "Line %d: %s\n", yylineno, s);
}
