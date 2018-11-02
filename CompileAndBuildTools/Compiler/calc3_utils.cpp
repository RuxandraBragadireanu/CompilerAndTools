#include "calc3_defs.h"
#include "calc3_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "Expressions.h"
#include "CompilationBlackbox.h"
#include "IFNode.h"
#include "FOREachNode.h"
#include "FORNormalNode.h"
#include "INTERMEDIATENode.h"
#include "WHILENode.h"
#include "ReferenceNode.h"
#include "DECLARATIONNode.h"
#include "ASIGNMENTNode.h"
#include "CModuleCode.h"
#include "ExecutionBlackbox.h"
#include <stdarg.h>
#include <iostream>

#include "Profiler.h"

extern "C" int yylineno;

extern Timer gTotalGlobalWatch;
extern int gExecutionType; 
extern int gUseSerializedAST;

const char *GetDataTypesString(int iDataType)
{
	switch(iDataType)
	{
	case ETYPE_INT:
		return "int";
	case ETYPE_FLOAT:
		return "float";
	case ETYPE_BOOL:
		return "bool";
	case ETYPE_STRING:
		return "string";
	case ETYPE_GENERAL_ARRAY:
		return "array";
	case ETYPE_CHAR:
		return "char";
	case ETYPE_DATA_BUFFER:
		return "databuffer";
	default:
		assert(0 && "data type invalid to get string");
		return "";
	}
}

void PrintCompileError(int line, const char *errorFormat, ...)
{
	const int iMaxStrSize = 2048;
	static char buff[iMaxStrSize];

	va_list va;
	va_start(va, errorFormat);
	vsnprintf(buff, iMaxStrSize, errorFormat, va);
	va_end(va);

	printf("COMPILE ERROR at line %d: %s\n", line, buff);
	//assert(false);
}

void PrintCompileWarning(int line, const char *errorFormat, ...)
{
	const int iMaxStrSize = 2048;
	static char buff[iMaxStrSize];

	va_list va;
	va_start(va, errorFormat);
	vsnprintf(buff, iMaxStrSize, errorFormat, va);
	va_end(va);

	printf("COMPILE ERROR at line %d: %s\n", line, buff);
	assert(false);
}

void PrintExecutionError(const char *errorString)
{
	printf("EXECUTION ERROR: %s\n", errorString);
	assert(false);
	exit(3);
}

void* ABSTFactory::CreateInputBlocks(void *pWest, void *pNorth)
{
	ProgramInput* pInput = new ProgramInput();
	pInput->north = (InputBlock*)pNorth;
	pInput->west = (InputBlock*)pWest;
	return pInput;
}

void *ABSTFactory::CreateOutputBlocks(void *pEast, void *pSouth)
{
	ProgramOutput* pOutput = new ProgramOutput();
	pOutput->east = (InputBlock*)pEast;
	pOutput->south = (InputBlock*)pSouth;
	return pOutput;
}

void* ABSTFactory::CreateAtomicExpression(int iConst)
{
	ExpressionConstant* pExpr = new ExpressionConstant(yylineno);
	pExpr->SetValue(iConst);
	return pExpr;
}

void* ABSTFactory::CreateAtomicExprIdentifier(char* szIdentifier)
{
	ExpressionVariable* pExpr = new ExpressionVariable(yylineno);
	pExpr->SetName(szIdentifier);
	return pExpr;
}

void* ABSTFactory::CreateAtomicExpression(float iConst)
{
	ExpressionConstant* pExpr = new ExpressionConstant(yylineno);
	pExpr->SetValue(iConst);
	return pExpr;
}

void *ABSTFactory::CreateBooleanExpression(void *pExpr1, void *pExpr2, EBooleanExpressionType type)
{
	ExpressionBooleanNode* pBoolean = new ExpressionBooleanNode(yylineno);
	pBoolean->SetTypeAndChilds(type, (IExpression*)pExpr1, (IExpression*)pExpr2);
	return pBoolean;
}

void* ABSTFactory::CreateArrayAccessExpression(char* szIdentifier, void* expresssion)
{
	IExpression* pExpresssion = (IExpression*) expresssion;
	if (pExpresssion->GetDominantType() != E_DOM_INT)
	{
		PrintCompileError(yylineno, "Assignment, Array access and type is not an integer");
		return NULL;
	}

	ExpressionArrayAccess* pExpr = new ExpressionArrayAccess(yylineno);
	pExpr->SetIndicesAndName(szIdentifier, pExpresssion);
	return pExpr;
}

void *ABSTFactory::CreateValueExpression(void *Expr1, void *Expr2, EValueExpressionType type)
{
	IExpression* pExpr1 = (IExpression*) Expr1;
	IExpression* pExpr2 = (IExpression*) Expr2;
	ExpressionValueNode* pNode = new ExpressionValueNode(yylineno);
	pNode->SetTypeAndChilds(type, pExpr1, pExpr2);
	return pNode;
}

void *ABSTFactory::CreateAsignmentProgram(void *expr1, void *expr2)
{
	IExpression* pExpr1 = (IExpression*) expr1;
	IExpression* pExpr2 = (IExpression*) expr2;

	EGeneralExpressionType type = pExpr1->GetGeneralExpressionType();
	if (type == E_EXPR_VARIABLE || type == E_EXPR_ARRAY_ACCESS)
	{
		ProgramAsignment* pAsignNode = new ProgramAsignment(yylineno);
		pAsignNode->SetChilds(pExpr1, pExpr2);
		return pAsignNode;
	}
	else
	{
		PrintCompileError(yylineno, "line", "Left side of an assignment must be a variable");
		return NULL;
	}
}

void *ABSTFactory::CreateDeclarationProgram(void *pInputItem)
{
	ProgramDeclaration* pDecl =  new ProgramDeclaration(yylineno);

	IDataTypeItem* pItem = (IDataTypeItem*) pInputItem;
	pDecl->SetIdentifierAndInputItem((char*)pItem->GetName(), pItem);

	return pDecl;
}

void *ABSTFactory::CreateAgapiaModule(const char *szModuleName, void *pInput, void *pBlock, void *pOutput)
{
	ProgramInput* pProgramInput = (ProgramInput*) pInput;
	ProgramOutput* pProgramOutput = (ProgramOutput*) pOutput;

	// Create this defined module
	ProgramIntermediateModule* pAgapiaModule = new ProgramIntermediateModule(yylineno);
	pAgapiaModule->m_szModuleName = _strdup(szModuleName);
	pAgapiaModule->m_bIsUserDefined = true;
	
	IProgram* pIProgram = (IProgram*) pBlock;
	ENodeType eType = pIProgram->GetType();
	switch(eType)
	{
	case E_NODE_TYPE_MODULE_REF:
		{
			ProgramReference* pRefProgram = (ProgramReference*) pBlock;
			pAgapiaModule->SetOperationAndChilds(/*ECompositionOperator::*/E_COMP_UNDEFINED, pRefProgram, NULL);
		}
		break;

	case E_NODE_TYPE_INTERMEDIATE:
		{
			ProgramIntermediateModule* pBlockProgram = (ProgramIntermediateModule*) pBlock;
			// It's a composition node ? Take it's child
			if (pBlockProgram->m_operator != /*ECompositionOperator::*/E_COMP_UNDEFINED) 
				pAgapiaModule->SetOperationAndChilds(pBlockProgram->m_operator, pBlockProgram->m_pChilds[0], pBlockProgram->m_pChilds[1]);
			else	// If not, add it directly
				pAgapiaModule->SetOperationAndChilds(/*ECompositionOperator::*/E_COMP_UNDEFINED, pBlockProgram, NULL);
		}
		break;

	case E_NODE_TYPE_IF:
	case E_NODE_TYPE_FOREACH:
	case E_NODE_TYPE_WHILE:
	case E_NODE_TYPE_FORNORMAL:
		{
			ProgramIntermediateModule* pBlockProgram = (ProgramIntermediateModule*) pBlock;
			pAgapiaModule->SetOperationAndChilds(/*ECompositionOperator::*/E_COMP_UNDEFINED, pBlockProgram, NULL);
		}
		break;
	case E_NODE_TYPE_IDENTITY:
		pAgapiaModule->SetOperationAndChilds(E_COMP_UNDEFINED, (ProgramIdentity*)pIProgram, NULL);
		break;

	case E_NODE_TYPE_C_CODE_ZONE_ALL:
	case E_NODE_TYPE_C_CODE_ZONE_MASTER:
		{
			CModuleCode* pCCode = (CModuleCode*) ABSTFactory::CreateCCodeObject(szModuleName, eType);
			if (pCCode == NULL)
			{
				assert(false);
				return NULL;
			}

			pAgapiaModule->m_pCCodeObject = pCCode;
			pAgapiaModule->m_bIsAtomic = true;

			pAgapiaModule->SetOperationAndChilds(E_COMP_UNDEFINED, static_cast<ProgramBase*>(pIProgram), NULL);			
		}
		break;

	default:
		printf("Module type not treated!!!!!!\n");
		assert(false);
		return NULL;
	}

	pAgapiaModule->SetInputBlocks(pProgramInput->north, pProgramInput->west);
	pAgapiaModule->SetOutputBlocks(pProgramOutput->south, pProgramOutput->east);
	if (pAgapiaModule->m_pInputNorth->IsNil() && pAgapiaModule->m_pInputWest->IsNil())
	{
		// The main input node should not be considered to have no input, even it has :)
		if (strcmp(szModuleName, "MAIN"))
			pAgapiaModule->m_bHasNoInput = true;
	}
		

	CompilationBlackbox::Get()->AddModule(szModuleName, pAgapiaModule);

	return pAgapiaModule;
}

// This function tries to read the file with the same name as the module
void* ABSTFactory::CreateCCodeObject(const char* szModuleName, int eTargetType)
{
	CModuleCode* pMC = new CModuleCode(eTargetType);

	// Currently we need only the target when executing the program.
	// The source code must be parsed only when creating AGAPIAToCCode.cpp file
	if (gExecutionType == EXECT_GENERATE_CODE)
	{
		FILE* f;
		const char* filePath = GetCompleteFilePathToSlnDirTemp(szModuleName);
		fopen_s(&f, filePath, "r");
		if (f == NULL)
		{
			PrintCompileError(yylineno, "The C module file %s doesn't exists\n", szModuleName);
			fclose(f);
			return NULL;
		}

		const int iMaxLineSize = 1024;
		char buff[iMaxLineSize];

		while(!feof(f))
		{
			fgets(buff, iMaxLineSize - 2, f);

			if (feof(f))
				break;

			if (strlen(buff) >= iMaxLineSize - 2)
			{
				PrintCompileError(yylineno, "Line too long on C module file %s\n", "line", szModuleName);
				return NULL;
			}

			buff[iMaxLineSize - 2] = '\n';
			buff[iMaxLineSize - 1] = '\0';
			pMC->AppendLineCode(buff);
		}

		pMC->AppendLineCode("\n");

		fclose(f);
	}

	return pMC;
}

void *ABSTFactory::CreateIntermediateProgram(void *pProgramChild1, void *pProgramChild2, ECompositionOperator eCompositionOperator)
{
	ProgramIntermediateModule* pIntermediateProgram = new ProgramIntermediateModule(yylineno);
	pIntermediateProgram->SetOperationAndChilds(eCompositionOperator, (ProgramBase*)pProgramChild1, (ProgramBase*)pProgramChild2);
	return pIntermediateProgram;
}

void *ABSTFactory::CreateCCodeProgramType(int iType)
{
	ProgramCCodeType*	pProgram = new ProgramCCodeType(iType, yylineno);
	return pProgram;
}

void *ABSTFactory::CreateIFProgram(void* pConditionExpression, void *pProgramFirstBranch, void *pProgramSecondBranch)
{
	ProgramIF* pIFProgram = new ProgramIF(yylineno);
	pIFProgram->m_pCondition = (IExpression*) pConditionExpression;
	pIFProgram->m_pIfProgram = (ProgramBase*) pProgramFirstBranch;
	pIFProgram->m_pElseProgram = (ProgramBase*) pProgramSecondBranch;
	return pIFProgram;
}

void *ABSTFactory::CreateFOREACHProgram(int iFORType, void *pNumericLimitExpression, void *pBaseChildProgram)
{
	ProgramFOREACH* pFORProgram = new ProgramFOREACH(yylineno);
	pFORProgram->m_eType = (EForEachType) iFORType;
	pFORProgram->m_pValueToGo = (IExpression*) pNumericLimitExpression;
	pFORProgram->m_pBaseChildProgram = (ProgramBase*) pBaseChildProgram;
	return pFORProgram;
}

void* ABSTFactory::CreateFORNormalProgram(int iFORType, void *pInitAssignment, void *pNumericLimitExpression, void *postIncrementExp, void *pBaseChildProgram)
{
	ProgramFORNormal* pFORProgram = new ProgramFORNormal(yylineno);
	pFORProgram->mForType = (EForNormalType) iFORType;
	pFORProgram->m_pInitExpression = (ProgramAsignment*) pInitAssignment;
	pFORProgram->m_pCondition = (IExpression*) pNumericLimitExpression;
	pFORProgram->m_pPostExpression = (IExpression*) postIncrementExp;
	pFORProgram->m_pBaseProgram = (ProgramBase*) pBaseChildProgram;
	return pFORProgram;
}

void* ABSTFactory::CreateWHILEProgram(int iWhileType, void *pConditionExpression, void *pBaseChildProgram)
{
	ProgramWHILE* pWHILEProgram = new ProgramWHILE(yylineno);
	pWHILEProgram->m_eWHILEType = (EWHILEType) iWhileType;
	pWHILEProgram->m_pCondition = (IExpression*) pConditionExpression;
	pWHILEProgram->m_pBaseProgram = (ProgramBase*) pBaseChildProgram;
	return pWHILEProgram;
}

void *ABSTFactory::CreateModuleRef(char *szModuleName)
{
	ProgramReference* pRefProgram = new ProgramReference(yylineno);
	pRefProgram->m_strModuleReference = _strdup(szModuleName);
	return pRefProgram;
}

void *ABSTFactory::CreateIdentityProgram()
{
	ProgramIdentity* pIdentityProgram = new ProgramIdentity(yylineno);
	return pIdentityProgram;
}

void ABSTFactory::RunCompilation()
{ 	
	// Do the compilation and binary generation code
	if (gExecutionType != EXECT_GENERATE_CODE)
	{
		// There are two types of AST: one that is created from scratch by calling yyparse and one that is loaded from a file.
		// If we are building it with yyparse it means that it should be saved now on disk otherwise load it from disk
		// TODO: normally the serialization processes should be done after all compile steps but there was no dev time implement for this so i just saved the parsed tree and do compile steps again to build info
		if (gUseSerializedAST)
		{
			CompilationBlackbox::Get()->LoadAST();
		}

		//std::cout<<"Finished tree loading after: "<<totalGlobalWatch.GetCounter()<<std::endl;
		//totalGlobalWatch.Reset();
	}
	else
	{
		CompilationBlackbox::Get()->SaveAST();
	}	

	if (!CompilationBlackbox::Get()->DoCompileSteps(gExecutionType))
		exit(0);

	ExecutionBlackbox::Sync();

	std::cout<<"Finished program loading. Starting execution after: "<<gTotalGlobalWatch.GetCounter()<<std::endl;
	gTotalGlobalWatch.Reset();

	// If we just generate code then we have to exit and rebuild
	if (gExecutionType == EXECT_GENERATE_CODE)
	{
		CompilationBlackbox::Get()->CleanupCompileFiles();

		return ;
	}

	// Initialize the execution blackbox
	ExecutionBlackbox::Initialize();
	//std::cout<<"Execution blackbox finished initialization at "<<totalGlobalWatch.GetCounter()<<std::endl;

	// Do Work !
	ExecutionBlackbox::DoWork();
	//std::cout<<"Execution blackbox finished initialization at "<<totalGlobalWatch.GetCounter()<<std::endl;
}

//----------------------------------------------------------------
const char* GetPathToSlnDir()
{
	static char buff[4096];	
	static const char* szAgapiaPath = getenv("AGAPIAPATH");
	if (szAgapiaPath == NULL || strlen(szAgapiaPath) == 0)
	{
		printf("ERROR: PLEASE DECLARE THE ENVIRONMENT VARIABLE AGAPIAPATH\n");
		exit(0);
	}
	sprintf_s(buff, 4096, "%s\\Compiler", szAgapiaPath);
	return buff;
}
const char* GetCompleteFilePathToSlnDirTemp(const char* fileName)
{
	static char buff[4096];	
	static const char* szAgapiaPath = getenv("AGAPIAPATH");
	if (szAgapiaPath == NULL || strlen(szAgapiaPath) == 0)
	{
		printf("ERROR: PLEASE DECLARE THE ENVIRONMENT VARIABLE AGAPIAPATH\n");
		exit(0);
	}

	sprintf_s(buff, 4096, "%s\\Compiler\\temp\\%s", szAgapiaPath, fileName);
	return buff;
}
//----------------------------------------------------------------


void RunDebugModulesAnalyzer()
{
	static char buff[4096];	
	static const char* szAgapiaPath = getenv("AGAPIAPATH");
	if (szAgapiaPath == NULL || strlen(szAgapiaPath) == 0)
	{
		printf("ERROR: PLEASE DECLARE THE ENVIRONMENT VARIABLE AGAPIAPATH\n");
		exit(0);
	}

	sprintf_s(buff, 4096, "%s\\ModulesAnalyzer\\Debug\\ModulesAnalyzer.exe", szAgapiaPath);
	const int res = system(buff);
	if (res != 0)
	{
		// Internal error in tool ?
		if (res < 0)	
		{
			printf("Internal errors occurred while processing your file. Please solve the above errors and re-run\n");
			exit(-1);
		}
		else 
		{
			printf("Didn't find the tool where it should be: %s\n", buff);
			exit(-1);
		}							
	}
}
