
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "calc3.y"

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
	void yyerror(const char *s);
}

extern "C" 	FILE *yyin;



/* Line 189 of yacc.c  */
#line 95 "calc3.cpp"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


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

/* Line 214 of yacc.c  */
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



/* Line 214 of yacc.c  */
#line 203 "calc3.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

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


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 228 "calc3.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   572

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  73
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  18
/* YYNRULES -- Number of rules.  */
#define YYNRULES  76
/* YYNRULES -- Number of states.  */
#define YYNSTATES  214

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   310

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      67,    46,    56,    54,    70,    55,     2,    57,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    71,    66,
      48,    72,    47,     2,    65,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    68,     2,    69,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    63,     2,    64,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    49,    50,    51,    52,    53,    58,    59,    60,    61,
      62
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    18,    27,    36,    38,
      41,    43,    45,    49,    51,    53,    55,    65,    69,    71,
      75,    81,    83,    85,    87,    89,    91,    93,    95,    97,
      99,   101,   103,   105,   107,   109,   113,   117,   121,   125,
     126,   135,   136,   149,   157,   165,   173,   181,   189,   197,
     209,   221,   233,   234,   238,   240,   242,   244,   249,   253,
     256,   260,   264,   268,   272,   276,   280,   282,   286,   290,
     294,   298,   302,   306,   310,   314,   318
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      74,     0,    -1,    75,    -1,    75,    76,    -1,    76,    -1,
      14,    41,    77,    63,    79,    64,    78,    -1,    63,     4,
      80,    64,    63,     5,    80,    64,    -1,    63,     6,    80,
      64,    63,     7,    80,    64,    -1,    85,    -1,    65,    27,
      -1,    65,    -1,     3,    -1,    80,    66,    81,    -1,    81,
      -1,     3,    -1,    82,    -1,    67,    67,    80,    46,    40,
      68,    69,    66,    46,    -1,    82,    70,    83,    -1,    83,
      -1,    40,    71,    84,    -1,    40,    71,    84,    68,    69,
      -1,    28,    -1,    30,    -1,    32,    -1,    31,    -1,    29,
      -1,    33,    -1,    35,    -1,    36,    -1,    34,    -1,    37,
      -1,    38,    -1,    41,    -1,    88,    -1,    83,    -1,    85,
      10,    85,    -1,    85,     9,    85,    -1,    85,     8,    85,
      -1,    67,    85,    46,    -1,    -1,    12,    67,    90,    46,
      63,    85,    64,    86,    -1,    -1,    12,    67,    90,    46,
      63,    85,    64,    44,    63,    85,    64,    87,    -1,    22,
      67,    90,    46,    63,    85,    64,    -1,    24,    67,    90,
      46,    63,    85,    64,    -1,    23,    67,    90,    46,    63,
      85,    64,    -1,    16,    67,    89,    46,    63,    85,    64,
      -1,    17,    67,    89,    46,    63,    85,    64,    -1,    18,
      67,    89,    46,    63,    85,    64,    -1,    21,    67,    88,
      66,    89,    66,    89,    46,    63,    85,    64,    -1,    19,
      67,    88,    66,    89,    66,    89,    46,    63,    85,    64,
      -1,    20,    67,    88,    66,    89,    66,    89,    46,    63,
      85,    64,    -1,    -1,    89,    72,    89,    -1,    39,    -1,
      42,    -1,    40,    -1,    40,    68,    89,    69,    -1,    67,
      89,    46,    -1,    55,    89,    -1,    89,    54,    54,    -1,
      89,    55,    55,    -1,    89,    54,    89,    -1,    89,    55,
      89,    -1,    89,    56,    89,    -1,    89,    57,    89,    -1,
      90,    -1,    89,    53,    89,    -1,    89,    48,    89,    -1,
      89,    47,    89,    -1,    89,    52,    89,    -1,    89,    51,
      89,    -1,    89,    49,    89,    -1,    89,    50,    89,    -1,
      89,    25,    89,    -1,    89,    26,    89,    -1,    67,    90,
      46,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    88,    88,    95,    96,   100,   104,   113,   122,   123,
     124,   125,   129,   130,   131,   135,   136,   140,   141,   145,
     146,   150,   151,   152,   153,   154,   156,   157,   158,   159,
     160,   162,   166,   167,   168,   169,   170,   171,   172,   173,
     173,   174,   174,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   188,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   209,   210,
     211,   212,   213,   214,   215,   216,   217
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NIL", "LISTEN", "READ", "SPEAK",
  "WRITE", "COMPOSITION_DIAGONAL", "COMPOSITION_VERTICAL",
  "COMPOSITION_HORIZONTAL", "WHILE", "IF", "PRINT", "MODULE", "FOR",
  "FOREACH_S", "FOREACH_T", "FOREACH_ST", "FORNORMAL_S", "FORNORMAL_T",
  "FORNORMAL_ST", "WHILE_S", "WHILE_T", "WHILE_ST", "AND", "OR",
  "EXEC_TARGET_MASTER", "DTYPE_INT", "DTYPE_CHAR", "DTYPE_BOOL",
  "DTYPE_FLOAT", "DTYPE_STRING", "DTYPE_ARRAY_INT", "DTYPE_ARRAY_FLOAT",
  "DTYPE_ARRAY_BOOL", "DTYPE_ARRAY_STRING", "DTYPE_ARRAY_CHAR",
  "DTYPE_DATA_BUFFER", "INTEGER", "IDENTIFIER_LOW", "IDENTIFIER_BIG",
  "FLOAT", "IFX", "ELSE", "EXPR_WITHOUT_BOOL", "')'", "'>'", "'<'", "NE",
  "EQ", "LE", "GE", "MOD", "'+'", "'-'", "'*'", "'/'", "UMINUS", "PDEC",
  "PINC", "DEC", "INC", "'{'", "'}'", "'@'", "';'", "'('", "'['", "']'",
  "','", "':'", "'='", "$accept", "agapiacode", "modules_list", "module",
  "module_input", "module_output", "module_body", "processinputlist",
  "singleproceslist", "simplevariablelist", "variable", "datatype",
  "program", "$@1", "$@2", "asignment", "expr", "boolean_expr", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,    41,    62,    60,   301,
     302,   303,   304,   305,    43,    45,    42,    47,   306,   307,
     308,   309,   310,   123,   125,    64,    59,    40,    91,    93,
      44,    58,    61
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    73,    74,    75,    75,    76,    77,    78,    79,    79,
      79,    79,    80,    80,    80,    81,    81,    82,    82,    83,
      83,    84,    84,    84,    84,    84,    84,    84,    84,    84,
      84,    84,    85,    85,    85,    85,    85,    85,    85,    86,
      85,    87,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    88,    89,    89,    89,    89,    89,    89,
      89,    89,    89,    89,    89,    89,    89,    89,    90,    90,
      90,    90,    90,    90,    90,    90,    90
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     7,     8,     8,     1,     2,
       1,     1,     3,     1,     1,     1,     9,     3,     1,     3,
       5,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     3,     3,     0,
       8,     0,    12,     7,     7,     7,     7,     7,     7,    11,
      11,    11,     0,     3,     1,     1,     1,     4,     3,     2,
       3,     3,     3,     3,     3,     3,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     2,     4,     0,     1,     3,     0,     0,
       0,    52,    14,     0,     0,     0,    13,    15,    18,    11,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      54,    56,    32,    55,     0,    10,    52,     0,    34,     8,
      33,     0,    66,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    56,
       0,    59,     9,     0,     0,    66,     0,    52,    52,    52,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    21,    25,    22,    24,    23,    26,
      29,    27,    28,    30,    31,    19,     0,     0,    12,    17,
       0,    66,     0,     0,     0,     0,     0,     0,    66,    66,
      66,     0,     0,    38,    58,    76,     0,     5,    37,    36,
      35,    74,    75,    69,    68,    72,    73,    71,    70,    67,
      60,    62,    61,    63,    64,    65,    53,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      57,     0,    20,     0,     0,    52,    52,    52,    52,     0,
       0,     0,    52,    52,    52,     0,     0,     6,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      39,    46,    47,    48,     0,     0,     0,    43,    45,    44,
       0,     0,     0,    40,     0,     0,     0,     0,    16,    52,
      52,    52,    52,     0,     0,     0,     0,     0,     7,    41,
      50,    51,    49,    42
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,     4,     9,   117,    37,    15,    16,    17,
      38,    95,    39,   193,   213,    40,    41,    42
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -44
static const yytype_int16 yypact[] =
{
      38,   -22,    20,    38,   -44,    -7,   -44,   -44,    53,     0,
       6,     9,   -44,    -1,    52,    13,   -44,    50,   -44,   -44,
      84,    98,   100,   101,   103,   104,   106,   115,   121,   144,
     -44,    74,   -44,   -44,    85,    48,   174,    37,   -44,     5,
     -44,   195,   -44,   534,     6,   126,    -2,   123,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,   149,
      85,   -44,   -44,   113,   153,   118,   155,   174,   174,   174,
      85,    85,    85,    85,    85,    85,    85,    85,    85,   490,
     494,    85,    85,    85,   -44,   -44,   -44,   -44,   -44,   -44,
     -44,   -44,   -44,   -44,   -44,   151,    34,   207,   -44,   -44,
     471,   176,   325,   337,   370,   157,   158,   160,   181,   182,
     184,   206,   382,   -44,   -44,   -44,   227,   -44,    -4,   224,
     -44,   -44,   -44,   244,   244,   244,   244,   244,   244,   277,
     -44,   124,   -32,   124,   122,   122,   471,   166,   196,     6,
     175,   177,   203,   205,    85,    85,    85,   208,   209,   210,
     -44,     6,   -44,   169,    62,   174,   174,   174,   174,   239,
     259,   292,   174,   174,   174,    80,   170,   -44,    -6,     8,
      14,    31,    85,    85,    85,    45,    89,    94,   211,   212,
     232,   -44,   -44,   -44,   415,   427,   460,   -44,   -44,   -44,
     270,   233,   217,   -44,   218,   219,   220,     6,   -44,   174,
     174,   174,   174,    96,    97,   102,   105,   108,   -44,   -44,
     -44,   -44,   -44,   -44
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -44,   -44,   -44,   301,   -44,   -44,   -44,   -43,   273,   -44,
     -10,   -44,   -25,   -44,   -44,    82,    11,   495
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      18,    96,    67,    68,    69,    68,    69,    30,    59,    12,
      33,    63,    19,    67,    68,    69,    67,    68,    69,     5,
       6,    20,    67,    68,    69,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    18,    60,    18,    99,    13,    67,
      68,    69,   118,   119,   120,    61,    13,    64,    30,    31,
      32,    33,     1,    67,    68,    69,     8,    10,   180,   100,
     102,   103,   104,    11,    34,    14,   100,   100,   100,   111,
      43,   112,   181,    14,    35,    62,    36,    45,   182,    46,
     138,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     131,   133,   134,   135,   136,   183,   154,    67,    68,    69,
      46,    66,    67,    68,    69,    67,    68,    69,   165,   187,
      67,    68,    69,    67,    68,    69,    67,    68,    69,    44,
      47,    67,    68,    69,    30,    59,   167,    33,    46,    18,
     168,   169,   170,   171,   105,   106,   107,   175,   176,   177,
      34,    18,    58,    61,   178,    43,    46,    70,    71,    70,
      71,    48,    60,   188,   203,   159,   160,   161,   189,   113,
     208,   209,    46,    13,   115,    49,   210,    50,    51,   211,
      52,    53,   212,    54,   204,   205,   206,   207,    70,    71,
      81,    82,    55,   184,   185,   186,    20,    18,    56,    97,
      21,    22,    23,    24,    25,    26,    27,    28,    29,   114,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    57,   139,    30,    31,    32,    33,    58,   116,   137,
      70,    71,   140,   144,   145,    83,   146,   147,   148,    34,
     149,    70,    71,   151,    69,   152,   153,   166,   155,   179,
     156,    36,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    70,    71,   157,    83,   158,    70,
      71,   162,   163,   164,   190,   150,   192,   197,   191,   198,
     199,   200,   201,   202,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    78,    79,    80,
      81,    82,    70,    71,     7,   172,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    70,    71,    98,
       0,     0,     0,     0,     0,   173,     0,     0,     0,     0,
       0,    79,    80,    81,    82,     0,     0,     0,     0,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      70,    71,     0,     0,     0,     0,     0,     0,   174,     0,
       0,     0,    70,    71,     0,     0,     0,     0,     0,     0,
       0,   141,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,   142,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    70,    71,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    70,    71,     0,
       0,     0,     0,     0,     0,     0,   143,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,   114,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      70,    71,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    70,    71,     0,     0,     0,     0,     0,     0,
       0,   194,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,   195,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    70,    71,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    70,    71,     0,     0,
       0,     0,     0,     0,     0,     0,   196,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    30,
      59,    65,    33,    30,    59,     0,    33,     0,     0,     0,
       0,     0,     0,   101,   130,    34,     0,     0,     0,   132,
     108,   109,   110,     0,     0,    65,     0,    60,     0,     0,
       0,    60,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94
};

static const yytype_int16 yycheck[] =
{
      10,    44,     8,     9,    10,     9,    10,    39,    40,     3,
      42,    36,     3,     8,     9,    10,     8,     9,    10,    41,
       0,    12,     8,     9,    10,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    44,    67,    46,    47,    40,     8,
       9,    10,    67,    68,    69,    34,    40,    36,    39,    40,
      41,    42,    14,     8,     9,    10,    63,     4,    64,    48,
      49,    50,    51,    63,    55,    67,    55,    56,    57,    58,
      71,    60,    64,    67,    65,    27,    67,    64,    64,    66,
      46,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    64,   139,     8,     9,    10,
      66,    64,     8,     9,    10,     8,     9,    10,   151,    64,
       8,     9,    10,     8,     9,    10,     8,     9,    10,    67,
      70,     8,     9,    10,    39,    40,    64,    42,    66,   139,
     155,   156,   157,   158,    52,    53,    54,   162,   163,   164,
      55,   151,    68,   132,    64,    71,    66,    25,    26,    25,
      26,    67,    67,    64,   197,   144,   145,   146,    64,    46,
      64,    64,    66,    40,    46,    67,    64,    67,    67,    64,
      67,    67,    64,    67,   199,   200,   201,   202,    25,    26,
      56,    57,    67,   172,   173,   174,    12,   197,    67,    63,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    67,     5,    39,    40,    41,    42,    68,    63,    68,
      25,    26,    46,    66,    66,    72,    66,    46,    46,    55,
      46,    25,    26,     6,    10,    69,    40,    68,    63,    69,
      63,    67,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    25,    26,    63,    72,    63,    25,
      26,    63,    63,    63,    63,    69,    44,     7,    66,    46,
      63,    63,    63,    63,    25,    26,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    53,    54,    55,
      56,    57,    25,    26,     3,    66,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    25,    26,    46,
      -1,    -1,    -1,    -1,    -1,    66,    -1,    -1,    -1,    -1,
      -1,    54,    55,    56,    57,    -1,    -1,    -1,    -1,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      25,    26,    -1,    -1,    -1,    -1,    -1,    -1,    66,    -1,
      -1,    -1,    25,    26,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    25,    26,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    25,    26,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      25,    26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    25,    26,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    25,    26,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    25,    26,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    39,
      40,    36,    42,    39,    40,    -1,    42,    -1,    -1,    -1,
      -1,    -1,    -1,    48,    54,    55,    -1,    -1,    -1,    55,
      55,    56,    57,    -1,    -1,    60,    -1,    67,    -1,    -1,
      -1,    67,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    14,    74,    75,    76,    41,     0,    76,    63,    77,
       4,    63,     3,    40,    67,    80,    81,    82,    83,     3,
      12,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      39,    40,    41,    42,    55,    65,    67,    79,    83,    85,
      88,    89,    90,    71,    67,    64,    66,    70,    67,    67,
      67,    67,    67,    67,    67,    67,    67,    67,    68,    40,
      67,    89,    27,    85,    89,    90,    64,     8,     9,    10,
      25,    26,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    72,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    84,    80,    63,    81,    83,
      89,    90,    89,    89,    89,    88,    88,    88,    90,    90,
      90,    89,    89,    46,    46,    46,    63,    78,    85,    85,
      85,    89,    89,    89,    89,    89,    89,    89,    89,    89,
      54,    89,    55,    89,    89,    89,    89,    68,    46,     5,
      46,    46,    46,    46,    66,    66,    66,    46,    46,    46,
      69,     6,    69,    40,    80,    63,    63,    63,    63,    89,
      89,    89,    63,    63,    63,    80,    68,    64,    85,    85,
      85,    85,    66,    66,    66,    85,    85,    85,    64,    69,
      64,    64,    64,    64,    89,    89,    89,    64,    64,    64,
      63,    66,    44,    86,    46,    46,    46,     7,    46,    63,
      63,    63,    63,    80,    85,    85,    85,    85,    64,    64,
      64,    64,    64,    87
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
extern "C" {
YYSTYPE yylval;
}

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[2];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 89 "calc3.y"
    { 
			ABSTFactory::RunCompilation();
		;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 100 "calc3.y"
    { ABSTFactory::CreateAgapiaModule((yyvsp[(2) - (7)].identifier), (yyvsp[(3) - (7)].pInputOutput), (yyvsp[(5) - (7)].pNodeModule), (yyvsp[(7) - (7)].pInputOutput)); ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 105 "calc3.y"
    { 
			(yyval.pInputOutput) = ABSTFactory::CreateInputBlocks((yyvsp[(3) - (8)].pBlockInput), (yyvsp[(7) - (8)].pBlockInput));		
			InputBlock* pIB = (InputBlock*) (yyvsp[(3) - (8)].pBlockInput); 
			pIB = (InputBlock*) (yyvsp[(7) - (8)].pBlockInput); 		
		;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 114 "calc3.y"
    {
			(yyval.pInputOutput) = ABSTFactory::CreateOutputBlocks((yyvsp[(3) - (8)].pBlockInput), (yyvsp[(7) - (8)].pBlockInput));
		
			InputBlock* pIB = (InputBlock*) (yyvsp[(3) - (8)].pBlockInput); pIB = (InputBlock*) (yyvsp[(7) - (8)].pBlockInput);
		;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 122 "calc3.y"
    { (yyval.pNodeModule) = (yyvsp[(1) - (1)].pNodeModule); ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 123 "calc3.y"
    { (yyval.pNodeModule) = ABSTFactory::CreateCCodeProgramType(E_CCODE_FORMASTER); ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 124 "calc3.y"
    { (yyval.pNodeModule) = ABSTFactory::CreateCCodeProgramType(E_CCODE_FORALL); ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 125 "calc3.y"
    { (yyval.pNodeModule) = ABSTFactory::CreateIdentityProgram(); ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 129 "calc3.y"
    { InputBlock* pIB = (InputBlock*) (yyvsp[(1) - (3)].pBlockInput); pIB->AddInput((BaseProcessInput*)(yyvsp[(3) - (3)].pBlockInput)); (yyval.pBlockInput) = pIB; ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 130 "calc3.y"
    { InputBlock* pIB = new InputBlock(); pIB->AddInput((BaseProcessInput*)(yyvsp[(1) - (1)].pBlockInput)); (yyval.pBlockInput) = pIB; ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 131 "calc3.y"
    { InputBlock* pIB = new InputBlock(); (yyval.pBlockInput) = pIB; ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 135 "calc3.y"
    { (yyval.pBlockInput) = (yyvsp[(1) - (1)].pProcessInput); ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 136 "calc3.y"
    { VectorProcessItem* pVPI = new VectorProcessItem(); pVPI->SetItemType(((InputBlock*)(yyvsp[(3) - (9)].pBlockInput))->m_InputsInBlock, (yyvsp[(5) - (9)].identifier)); (yyval.pBlockInput) = pVPI; ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 140 "calc3.y"
    { SimpleProcessItem* pSPI = (SimpleProcessItem*) (yyvsp[(1) - (3)].pProcessInput); pSPI->AddItem((IDataTypeItem*)(yyvsp[(3) - (3)].pInputItem)); (yyval.pProcessInput) = pSPI; ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 141 "calc3.y"
    { SimpleProcessItem* pSPI = new SimpleProcessItem(); pSPI->AddItem((IDataTypeItem*)(yyvsp[(1) - (1)].pInputItem)); (yyval.pProcessInput) = pSPI; ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 145 "calc3.y"
    { (yyval.pInputItem) = ItemTypeFactory::CreateInputItem((yyvsp[(3) - (3)].iDataType), (yyvsp[(1) - (3)].identifier)); ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 146 "calc3.y"
    { (yyval.pInputItem) = ItemTypeFactory::CreateInputItem((yyvsp[(3) - (5)].iDataType), (yyvsp[(1) - (5)].identifier)); ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 150 "calc3.y"
    { (yyval.iDataType) = (yyvsp[(1) - (1)].iDataType); ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 151 "calc3.y"
    { (yyval.iDataType) = (yyvsp[(1) - (1)].iDataType); ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 152 "calc3.y"
    { (yyval.iDataType) = (yyvsp[(1) - (1)].iDataType); ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 153 "calc3.y"
    { (yyval.iDataType) = (yyvsp[(1) - (1)].iDataType); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 154 "calc3.y"
    { (yyval.iDataType) = (yyvsp[(1) - (1)].iDataType); ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 156 "calc3.y"
    { (yyval.iDataType) = (yyvsp[(1) - (1)].iDataType); ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 157 "calc3.y"
    { (yyval.iDataType) = (yyvsp[(1) - (1)].iDataType); ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 158 "calc3.y"
    { (yyval.iDataType) = (yyvsp[(1) - (1)].iDataType); ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 159 "calc3.y"
    { (yyval.iDataType) = (yyvsp[(1) - (1)].iDataType); ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 160 "calc3.y"
    { (yyval.iDataType) = (yyvsp[(1) - (1)].iDataType); ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 162 "calc3.y"
    { (yyval.iDataType) = (yyvsp[(1) - (1)].iDataType); ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 166 "calc3.y"
    { (yyval.pNodeModule) = ABSTFactory::CreateModuleRef((yyvsp[(1) - (1)].identifier)); ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 167 "calc3.y"
    { (yyval.pNodeModule) = (yyvsp[(1) - (1)].pAsignment); ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 168 "calc3.y"
    { (yyval.pNodeModule) = ABSTFactory::CreateDeclarationProgram((yyvsp[(1) - (1)].pInputItem)); ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 169 "calc3.y"
    { (yyval.pNodeModule) = ABSTFactory::CreateIntermediateProgram((yyvsp[(1) - (3)].pNodeModule), (yyvsp[(3) - (3)].pNodeModule), E_COMP_HORIZONTAL); ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 170 "calc3.y"
    { (yyval.pNodeModule) = ABSTFactory::CreateIntermediateProgram((yyvsp[(1) - (3)].pNodeModule), (yyvsp[(3) - (3)].pNodeModule), E_COMP_VERTICAL); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 171 "calc3.y"
    { (yyval.pNodeModule) = ABSTFactory::CreateIntermediateProgram((yyvsp[(1) - (3)].pNodeModule), (yyvsp[(3) - (3)].pNodeModule), E_COMP_DIAGONAL); ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 172 "calc3.y"
    { (yyval.pNodeModule) = (yyvsp[(2) - (3)].pNodeModule); ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 173 "calc3.y"
    {;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 173 "calc3.y"
    { (yyval.pNodeModule) = ABSTFactory::CreateIFProgram((yyvsp[(3) - (8)].pExpression), (yyvsp[(6) - (8)].pNodeModule), NULL); ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 174 "calc3.y"
    { ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 174 "calc3.y"
    { (yyval.pNodeModule) = ABSTFactory::CreateIFProgram((yyvsp[(3) - (12)].pExpression), (yyvsp[(6) - (12)].pNodeModule), (yyvsp[(10) - (12)].pNodeModule)); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 175 "calc3.y"
    { (yyval.pNodeModule) = ABSTFactory::CreateWHILEProgram(DTYPE_WHILE_S, (yyvsp[(3) - (7)].pExpression), (yyvsp[(6) - (7)].pNodeModule));;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 176 "calc3.y"
    { (yyval.pNodeModule) = ABSTFactory::CreateWHILEProgram(DTYPE_WHILE_ST, (yyvsp[(3) - (7)].pExpression), (yyvsp[(6) - (7)].pNodeModule));;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 177 "calc3.y"
    { (yyval.pNodeModule) = ABSTFactory::CreateWHILEProgram(DTYPE_WHILE_T, (yyvsp[(3) - (7)].pExpression), (yyvsp[(6) - (7)].pNodeModule));;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 178 "calc3.y"
    { (yyval.pNodeModule) = ABSTFactory::CreateFOREACHProgram(DTYPE_FOREACH_S, (yyvsp[(3) - (7)].pExpression), (yyvsp[(6) - (7)].pNodeModule)); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 179 "calc3.y"
    { (yyval.pNodeModule) = ABSTFactory::CreateFOREACHProgram(DTYPE_FOREACH_T, (yyvsp[(3) - (7)].pExpression), (yyvsp[(6) - (7)].pNodeModule)); ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 180 "calc3.y"
    { (yyval.pNodeModule) = ABSTFactory::CreateFOREACHProgram(DTYPE_FOREACH_ST, (yyvsp[(3) - (7)].pExpression), (yyvsp[(6) - (7)].pNodeModule)); ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 181 "calc3.y"
    { (yyval.pNodeModule) = ABSTFactory::CreateFORNormalProgram(DTYPE_FORNORMAL_ST, (yyvsp[(3) - (11)].pAsignment), (yyvsp[(5) - (11)].pExpression), (yyvsp[(7) - (11)].pExpression), (yyvsp[(10) - (11)].pNodeModule)); ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 182 "calc3.y"
    { (yyval.pNodeModule) = ABSTFactory::CreateFORNormalProgram(DTYPE_FORNORMAL_S, (yyvsp[(3) - (11)].pAsignment), (yyvsp[(5) - (11)].pExpression), (yyvsp[(7) - (11)].pExpression), (yyvsp[(10) - (11)].pNodeModule)); ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 183 "calc3.y"
    { (yyval.pNodeModule) = ABSTFactory::CreateFORNormalProgram(DTYPE_FORNORMAL_T, (yyvsp[(3) - (11)].pAsignment), (yyvsp[(5) - (11)].pExpression), (yyvsp[(7) - (11)].pExpression), (yyvsp[(10) - (11)].pNodeModule)); ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 184 "calc3.y"
    { ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 188 "calc3.y"
    { (yyval.pAsignment) = ABSTFactory::CreateAsignmentProgram((yyvsp[(1) - (3)].pExpression), (yyvsp[(3) - (3)].pExpression)); ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 192 "calc3.y"
    { (yyval.pExpression) = ABSTFactory::CreateAtomicExpression((yyvsp[(1) - (1)].iValue)); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 193 "calc3.y"
    { (yyval.pExpression) = ABSTFactory::CreateAtomicExpression((yyvsp[(1) - (1)].fValue)); ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 194 "calc3.y"
    { (yyval.pExpression) = ABSTFactory::CreateAtomicExprIdentifier((yyvsp[(1) - (1)].identifier)); ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 195 "calc3.y"
    { (yyval.pExpression) = ABSTFactory::CreateArrayAccessExpression((yyvsp[(1) - (4)].identifier), (yyvsp[(3) - (4)].pExpression)); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 196 "calc3.y"
    { (yyval.pExpression) = (yyvsp[(2) - (3)].pExpression); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 197 "calc3.y"
    { (yyval.pExpression) = ABSTFactory::CreateValueExpression((yyvsp[(2) - (2)].pExpression), NULL, E_UNARYMINUS); ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 198 "calc3.y"
    { (yyval.pExpression) = ABSTFactory::CreateValueExpression((yyvsp[(1) - (3)].pExpression), NULL, E_OP_INCREMENT); ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 199 "calc3.y"
    { (yyval.pExpression) = ABSTFactory::CreateValueExpression((yyvsp[(1) - (3)].pExpression), NULL, E_OP_DECREMENT); ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 200 "calc3.y"
    { (yyval.pExpression) = ABSTFactory::CreateValueExpression((yyvsp[(1) - (3)].pExpression), (yyvsp[(3) - (3)].pExpression), E_OP_PLUS); ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 201 "calc3.y"
    { (yyval.pExpression) = ABSTFactory::CreateValueExpression((yyvsp[(1) - (3)].pExpression), (yyvsp[(3) - (3)].pExpression), E_OP_MINUS);;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 202 "calc3.y"
    { (yyval.pExpression) = ABSTFactory::CreateValueExpression((yyvsp[(1) - (3)].pExpression), (yyvsp[(3) - (3)].pExpression), E_OP_MULT);;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 203 "calc3.y"
    { (yyval.pExpression) = ABSTFactory::CreateValueExpression((yyvsp[(1) - (3)].pExpression), (yyvsp[(3) - (3)].pExpression), E_OP_DIV);;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 204 "calc3.y"
    { (yyval.pExpression) = (yyvsp[(1) - (1)].pExpression); ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 205 "calc3.y"
    { (yyval.pExpression) = ABSTFactory::CreateValueExpression((yyvsp[(1) - (3)].pExpression), (yyvsp[(3) - (3)].pExpression), E_OP_MOD);;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 209 "calc3.y"
    { (yyval.pExpression) = ABSTFactory::CreateBooleanExpression((yyvsp[(1) - (3)].pExpression), (yyvsp[(3) - (3)].pExpression), E_SMALL_THAN);;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 210 "calc3.y"
    { (yyval.pExpression) = ABSTFactory::CreateBooleanExpression((yyvsp[(1) - (3)].pExpression), (yyvsp[(3) - (3)].pExpression), E_BIG_THAN);;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 211 "calc3.y"
    { (yyval.pExpression) = ABSTFactory::CreateBooleanExpression((yyvsp[(1) - (3)].pExpression), (yyvsp[(3) - (3)].pExpression), E_BIG_OR_EQUAL_THAN);;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 212 "calc3.y"
    { (yyval.pExpression) = ABSTFactory::CreateBooleanExpression((yyvsp[(1) - (3)].pExpression), (yyvsp[(3) - (3)].pExpression), E_SMALL_OR_EQUAL_THAN);;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 213 "calc3.y"
    { (yyval.pExpression) = ABSTFactory::CreateBooleanExpression((yyvsp[(1) - (3)].pExpression), (yyvsp[(3) - (3)].pExpression), E_DIFFERENT);;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 214 "calc3.y"
    { (yyval.pExpression) = ABSTFactory::CreateBooleanExpression((yyvsp[(1) - (3)].pExpression), (yyvsp[(3) - (3)].pExpression), E_EQUAL);;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 215 "calc3.y"
    { (yyval.pExpression) = ABSTFactory::CreateBooleanExpression((yyvsp[(1) - (3)].pExpression), (yyvsp[(3) - (3)].pExpression), E_AND);;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 216 "calc3.y"
    { (yyval.pExpression) = ABSTFactory::CreateBooleanExpression((yyvsp[(1) - (3)].pExpression), (yyvsp[(3) - (3)].pExpression), E_OR);;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 217 "calc3.y"
    { (yyval.pExpression) = (yyvsp[(2) - (3)].pExpression);;}
    break;



/* Line 1455 of yacc.c  */
#line 2229 "calc3.cpp"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[0] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 220 "calc3.y"


extern "C" int yylineno;

void yyerror(char *s) 
{
	fprintf(stdout, "Line %d: %s\n", yylineno, s);
}


