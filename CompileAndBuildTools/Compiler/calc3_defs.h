// This files contains things common to C and CPP for using in both lex and yacc

#ifndef CALC3_H
#define CALC3_H

#define MAX_IDENTIFIER_SIZE	100

// Data types
#define ETYPE_BOOL		0
#define ETYPE_CHAR		1
#define ETYPE_INT		2
#define ETYPE_FLOAT		3
#define ETYPE_STRING	4

#define ETYPE_ARRAY_BOOL	5
#define ETYPE_ARRAY_CHAR	6
#define ETYPE_ARRAY_INT		7
#define ETYPE_ARRAY_FLOAT	8
#define ETYPE_ARRAY_STRING	9

#define ETYPE_DATA_BUFFER	10

// Used to identify one of the above array types
#define ETYPE_GENERAL_ARRAY	11

// For types
#define DTYPE_FOREACH_S		0
#define DTYPE_FOREACH_T		1
#define DTYPE_FOREACH_ST	2

#define DTYPE_FORNORMAL_S   3
#define DTYPE_FORNORMAL_T	4
#define DTYPE_FORNORMAL_ST	5

// While types
#define DTYPE_WHILE_S	0
#define DTYPE_WHILE_T	1
#define DTYPE_WHILE_ST	2

// Types of target execution for a module on a distributed usage
#define E_CCODE_FORMASTER		0	// Master
#define E_CCODE_FORALL			1	// All

// Modules definition types
#define EATOMIC_MODULE_DEF		0
#define ENOTATOMIC_MODULE_DEF	1

#include <assert.h>

typedef enum { typeCon, typeId, typeOpr } nodeEnum;

/* constants */
typedef struct {
    int value;                  /* value of constant */
} conNodeType;

/* identifiers */
typedef struct {
    int i;                      /* subscript to sym array */
} idNodeType;

/* operators */
typedef struct {
    int oper;                   /* operator */
    int nops;                   /* number of operands */
    struct nodeTypeTag **op;	/* operands */
} oprNodeType;

typedef struct nodeTypeTag {
    nodeEnum type;              /* type of node */

    union {
        conNodeType con;        /* constants */
        idNodeType id;          /* identifiers */
        oprNodeType opr;        /* operators */
    };
} nodeType;

nodeType *con(int value);


#define YYERROR_VERBOSE

#endif
