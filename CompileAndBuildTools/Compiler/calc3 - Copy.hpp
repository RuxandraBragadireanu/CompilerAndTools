
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NIL = 258,
     LISTEN = 259,
     READ = 260,
     SPEAK = 261,
     WRITE = 262,
     COMPOSITION_DIAGONAL = 263,
     COMPOSITION_VERTICAL = 264,
     COMPOSITION_HORIZONTAL = 265,
     WHILE = 266,
     IF = 267,
     PRINT = 268,
     MODULE = 269,
     FOR = 270,
     FOREACH_S = 271,
     FOREACH_T = 272,
     FOREACH_ST = 273,
     FORNORMAL_S = 274,
     FORNORMAL_T = 275,
     FORNORMAL_ST = 276,
     WHILE_S = 277,
     WHILE_T = 278,
     WHILE_ST = 279,
     AND = 280,
     OR = 281,
     EXEC_TARGET_MASTER = 282,
     DTYPE_INT = 283,
     DTYPE_CHAR = 284,
     DTYPE_BOOL = 285,
     DTYPE_FLOAT = 286,
     DTYPE_STRING = 287,
     DTYPE_ARRAY_INT = 288,
     DTYPE_ARRAY_FLOAT = 289,
     DTYPE_ARRAY_BOOL = 290,
     DTYPE_ARRAY_STRING = 291,
     DTYPE_ARRAY_CHAR = 292,
     DTYPE_DATA_BUFFER = 293,
     INTEGER = 294,
     IDENTIFIER_LOW = 295,
     IDENTIFIER_BIG = 296,
     FLOAT = 297,
     IFX = 298,
     ELSE = 299,
     EXPR_WITHOUT_BOOL = 300,
     NE = 301,
     EQ = 302,
     LE = 303,
     GE = 304,
     MOD = 305,
     UMINUS = 306,
     PDEC = 307,
     PINC = 308,
     DEC = 309,
     INC = 310
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 24 "calc3.y"

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



/* Line 1676 of yacc.c  */
#line 124 "calc3.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYLTYPE yylloc;

