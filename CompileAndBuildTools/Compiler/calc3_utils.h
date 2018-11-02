// This is a liant between bison (y file) and our cpp structures

#ifndef CALC3_UTILS_H
#define CALC3_UTILS_H

#define	EXECT_GENERATE_EXE	0		// Generate the executable
#define EXECT_GENERATE_CODE	1		// Just generate the AGAPIAToCCode.cpp file

#include "Expressions.h"
#include "BaseNode.h"

#define LOG(params)	printf params

const char *GetDataTypesString(int iDataType);

void PrintCompileError(int line, const char *errorFormat, ...);
void PrintCompileWarning(int line, const char *errorFormat, ...);
void PrintExecutionError(const char *errorString);

void DoStep1();
void DoStep2();

// WARNING FUNC IS NOT THREAD SAFE
const char* GetCompleteFilePathToSlnDirTemp(const char* fileName);
const char* GetPathToSlnDir();

void RunDebugModulesAnalyzer();

static const char* TEMP_DIR_NAME = "temp";

struct ProgramInput
{
	InputBlock *north, *west;
};

struct ProgramOutput
{
	InputBlock *south, *east;
};

class ABSTFactory
{
public:
	// I/O expressions
	static void* CreateInputBlocks(void *pNorth, void *pWest);
	static void* CreateOutputBlocks(void *pEast, void *pSouth);

	// Expressions
	static void* CreateValueExpression(void *pExpr1, void *pExpr2, EValueExpressionType type);
	static void* CreateBooleanExpression(void *pExpr1, void *pExpr2, EBooleanExpressionType type);
	static void* CreateArrayAccessExpression(char* szIdentifier, void* pExpresssion);
	static void* CreateAtomicExpression(int iConst);
	static void* CreateAtomicExpression(float fConst);
	static void* CreateAtomicExpression(char* szConst) { return NULL; }
	static void* CreateAtomicExpression(bool bConst) { return NULL; }
	static void* CreateAtomicExprIdentifier(char* szIdentifier);

	static void RunCompilation();

	// Assignment
	static void* CreateAsignmentProgram(void *pExpr1, void *pExpr2);

	// Declaration
	static void* CreateDeclarationProgram(void *pExpr);

	// Module reference
	static void* CreateModuleRef(char *szModuleName);

	// Create identity module
	static void* CreateIdentityProgram();

	// Create a program with the type of a C code block
	static void* CreateCCodeProgramType(int type);

	// Create a program code object
	static void* CreateCCodeObject(const char* szCodeLine, int eTargetType);

	// Intermediate node
	static void *CreateIntermediateProgram(void *pProgramChild1, void *pProgramChild2, ECompositionOperator eCompositionOperator); 

	// IF node
	static void* CreateIFProgram(void* pConditionExpression, void *pProgramFirstBranch, void *pProgramSecondBranch);

	// FOREACH program
	static void* CreateFOREACHProgram(int iFORType, void *pNumericLimitExpression, void *pBaseChildProgram);

	// FORNORMAL program
	static void* CreateFORNormalProgram(int iFORType, void *pInitAssignment, void *pNumericLimitExpression, void *postIncrementExp, void *pBaseChildProgram);

	// WHILE program
	static void* CreateWHILEProgram(int iWhileType, void *pConditionExpression, void *pBaseChildProgram);

	// Module node
	static void *CreateAgapiaModule(const char *szModuleName, void *pInput, void *pBlock, void *pOutput);




	static bool ExactMachingStrings(const char *str1, const char *str2, int iChars)
	{
		int iIter = 0;
		while(iIter < iChars && *str1 != '\0' && *str2 != '\0')
		{
			if (*str1 != *str2)
				return false;

			iIter++;
		}
		return true;
	}
};

#endif