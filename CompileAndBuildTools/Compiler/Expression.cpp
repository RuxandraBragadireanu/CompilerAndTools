#include "Expressions.h"
#include "calc3_utils.h"
#include <string.h>
#include <assert.h>
#include "InputTypes.h"
#include <stdio.h>

AsignmentCompatibility IExpression::gAsignmentCompatible[E_NUM_TYPES][E_NUM_TYPES]=
{
	// B	C	I	F	S	A	D
{/*B*/ OK,	WA,	WA, WA, ER,	ER, ER },
{/*C*/ WA,	OK, WA, WA, ER, ER, ER },
{/*I*/ WA,	WA,	OK,	WA,	ER, ER, ER },
{/*F*/ WA,	WA,	OK,	OK, ER, ER, ER },
{/*S*/ ER,	WA,	ER,	ER,	OK, ER, ER },
{/*A*/ ER,	ER,	ER,	ER,	ER,	OK, ER },
{/*D*/ ER,	ER, ER, ER, ER, ER, OK },
};

int IExpression::gTypeDominancyTable[E_NUM_TYPES][E_NUM_TYPES]=
{
		// B			C				I				F				S				A				D
/*B*/	{E_DOM_BOOL,	E_DOM_CHAR,		E_DOM_INT,		E_DOM_FLOAT,	-1,				-1,				-1},
/*C*/	{E_DOM_CHAR,	E_DOM_CHAR,		E_DOM_INT,		E_DOM_FLOAT,	-1,				-1				-1},		
/*I*/	{E_DOM_INT,		E_DOM_INT,		E_DOM_INT,		E_DOM_FLOAT,	-1,				-1				-1},
/*F*/	{E_DOM_FLOAT,	E_DOM_FLOAT,	E_DOM_FLOAT,	E_DOM_FLOAT,	-1,				-1				-1},
/*S*/	{-1,			-1,				-1,				-1,				E_DOM_STRING,	-1,				-1},
/*A*/	{-1,			-1,				-1,				-1,				-1,				E_DOM_ARRAY,	-1},
/*D*/	{-1,			-1,				-1,				-1,				-1,				-1,				E_DOM_DATA_BUFFER}
};

AsignmentCompatibility IExpression::GetAsignmentsTypesCompatibility(EDominantType type1, EDominantType type2)
{
	return gAsignmentCompatible[type1][type2];
}

int IExpression::GetDominantOfTwoTypes(EDominantType type1, EDominantType type2)
{
	return gTypeDominancyTable[type1][type2];
}

void IExpression::CopyBase(IExpression* pCloned)
{
	pCloned->m_eGeneralType = m_eGeneralType;
	pCloned->m_eDominantType = m_eDominantType;
}

bool IsLastChild(ExpressionBooleanNode* pNode, int iChildIndex)
{
	if (iChildIndex == 1)
		return true;

	if (iChildIndex == 0 && pNode->m_Childs[1] == NULL)
		return true;

	return false;
}

char* IExpression::GetNextInputItem()
{
	ExprTreeStack* pIterationStack = GetIterationStack();
	assert(pIterationStack);
	if (pIterationStack->empty())
		return NULL;

	// Try to find a next value - the first who has only a child
	while(true)
	{
		std::pair<IExpression*, int>& topPair = pIterationStack->top();
		switch(topPair.first->GetGeneralExpressionType())
		{
		case E_EXPR_ARRAY_ACCESS:
			{
				char* result = ((ExpressionArrayAccess*)topPair.first)->m_szArrayName;
				pIterationStack->pop();
				return result;
			}
			break;
		case E_EXPR_VARIABLE:
			{
				char* result = ((ExpressionVariable*)topPair.first)->m_szName;
				pIterationStack->pop();
				return result;
			}
			break;
		case E_EXPR_CONSTANT:
			{
				char* result = ((ExpressionConstant*)topPair.first)->m_szVariable;
				pIterationStack->pop();
				return result;
			}
			break;
		case E_EXPR_VALUE:
			{
				int iChildsCount = topPair.first->GetNumberOfChilds();
				if (iChildsCount >= topPair.second)	// End of the line, go up
				{
					pIterationStack->pop();
					break;
				}
				else
				{
					// Push the next child in the stack
					pIterationStack->push(std::make_pair(((ExpressionValueNode*)topPair.first)->m_Childs[topPair.second], 0));
						
					// Go to the next child next time
					topPair.second++;
					break;
				}
			}
			break;
		case E_EXPR_BOOLEAN:
			{
				int iChildsCount = topPair.first->GetNumberOfChilds();
				if (iChildsCount >= topPair.second)	// End of the line, go up
				{
					pIterationStack->pop();
					break;
				}
				else
				{
					// Push the next child in the stack
					pIterationStack->push(std::make_pair(((ExpressionBooleanNode*)topPair.first)->m_Childs[topPair.second], 0));

					// Go to the next child next time
					topPair.second++;
					break;
				}
			}
			break;
		default:
			{
				assert(false);
				return NULL;
			}
		}
	}

	return NULL;
}

void IExpression::BeginIteration()
{
	ExprTreeStack* pIterationStack = GetIterationStack();
	assert(pIterationStack);

	while (!pIterationStack) pIterationStack->pop();
	pIterationStack->push(std::make_pair(this, 0));
}

ExpressionArrayAccess::~ExpressionArrayAccess()
{
	if (m_szArrayName)
		delete [] m_szArrayName;
}

void ExpressionArrayAccess::SetIndicesAndName(char *szIdentifier, IExpression* pIndexExpression)
{
	m_pIndexExpression = pIndexExpression;
	m_szArrayName = _strdup(szIdentifier);
}

bool ExpressionArrayAccess::ValidateExpression(SymbolTable* pSymbolTable)
{
	IDataTypeItem* pItem = pSymbolTable->GetIdentifierFromTable(m_szArrayName);
	if (pItem == NULL || pItem->m_eDataType != ETYPE_GENERAL_ARRAY)
	{
		PrintCompileError(mDebugLineNo, "Unidentified variable or it's not an array");
		return false;
	}

	if (m_pIndexExpression->GetDominantType() != E_DOM_INT)
	{
		PrintCompileError(mDebugLineNo, "Trying to access a vector without an integer index !");
		return false;
	}

	m_eDominantType = (EDominantType) pItem->m_eDataType;

	m_InputRefItem = pItem;
	switch (pItem->m_eDataType)
	{
	case ETYPE_FLOAT:
		m_pAsArrayOfFloats = (FloatVectorDataItem*) pItem;
		break;
	case ETYPE_STRING:
		m_pAsArrayOfStrings = (StringVectorDataItem*) pItem;
		break;
	case ETYPE_INT:
		m_pAsArrayOfInts = (IntVectorDataItem*) pItem;
		break;
	case ETYPE_BOOL:
		m_pAsArrayOfBools = (BoolVectorDataItem*) pItem;
		break;
	case ETYPE_CHAR:
		m_pAsArrayOfChars = (CharVectorDataItem*) pItem;
		break;
	default:
		assert(false);
		return false;
	}

	return true;
}

void ExpressionArrayAccess::CollectInternalVariableNames(VariablesNameSet& setOfVariableNames)
{
	setOfVariableNames.insert(std::string(m_szArrayName));
}

IExpression* ExpressionArrayAccess::Clone()
{
	ExpressionArrayAccess* pCloned = new ExpressionArrayAccess(mDebugLineNo);
	CopyBase(pCloned);

	pCloned->m_pIndexExpression = m_pIndexExpression->Clone();
	pCloned->m_szArrayName = _strdup(m_szArrayName);

	return pCloned;
}

int ExpressionArrayAccess::GetValueAsInt()
{
	int index = m_pIndexExpression->GetValueAsInt();
	if (m_eDominantType == E_DOM_INT)
		return m_pAsArrayOfInts->GetValue(index);
	else if (m_eDominantType == E_DOM_FLOAT)
		return (int)m_pAsArrayOfFloats->GetValue(index);
	else if (m_eDominantType == E_DOM_BOOL)
		return (int)m_pAsArrayOfBools->GetValue(index);
	else if (m_eDominantType == E_DOM_CHAR)
		return (int) m_pAsArrayOfChars->GetValue(index);
	else { assert(false);return 0; }
}

float ExpressionArrayAccess::GetValueAsFloat()
{
	int index = m_pIndexExpression->GetValueAsInt();
	if (m_eDominantType == E_DOM_FLOAT)
		return m_pAsArrayOfFloats->GetValue(index);
	else if (m_eDominantType == E_DOM_INT)
		return (float)m_pAsArrayOfFloats->GetValue(index);
	else if (m_eDominantType == E_DOM_BOOL)
		return (float)m_pAsArrayOfBools->GetValue(index);
	else if (m_eDominantType == E_DOM_CHAR)
		return (float) m_pAsArrayOfChars->GetValue(index);
	else { assert(false);return 0.0f; }
}

#pragma warning(disable:4800)
bool ExpressionArrayAccess::GetValueAsBool()
{
	int index = m_pIndexExpression->GetValueAsBool();
	if (m_eDominantType == E_DOM_BOOL)
		return m_pAsArrayOfBools->GetValue(index);
	else if (m_eDominantType == E_DOM_FLOAT)
		return (bool)m_pAsArrayOfFloats->GetValue(index);
	else if (m_eDominantType == E_DOM_INT)
		return (bool)m_pAsArrayOfInts->GetValue(index);
	else if (m_eDominantType == E_DOM_CHAR)
		return (bool) m_pAsArrayOfChars->GetValue(index);
	else { assert(false);return false; }
}
#pragma warning(default:4800)

char* ExpressionArrayAccess::GetValueAsString()
{
	int index = m_pIndexExpression->GetValueAsBool();
	if (m_eDominantType == E_DOM_STRING)
		return m_pAsArrayOfStrings->GetValue(index);
	else { assert(false);return NULL; }
}

void ExpressionArrayAccess::SetValue(bool bValue)
{	
	m_pAsArrayOfBools->SetValue(m_pIndexExpression->GetValueAsInt(), bValue);
}

void ExpressionArrayAccess::SetValue(char cValue)
{
	m_pAsArrayOfChars->SetValue(m_pIndexExpression->GetValueAsInt(), cValue);
}	

void ExpressionArrayAccess::SetValue(int iValue)
{
	m_pAsArrayOfInts->SetValue(m_pIndexExpression->GetValueAsInt(), iValue);
}

void ExpressionArrayAccess::SetValue(float fValue)
{
	m_pAsArrayOfFloats->SetValue(m_pIndexExpression->GetValueAsInt(), fValue);
}

void ExpressionArrayAccess::SetValue(char* szValue)
{
	m_pAsArrayOfStrings->SetValue(m_pIndexExpression->GetValueAsInt(), szValue);
}

ExpressionConstant::~ExpressionConstant()
{
	if (m_szVariable)
		delete [] m_szVariable;
}

void ExpressionConstant::SetValue(char *szValue)
{
	if (szValue == NULL || strlen(szValue) > MAX_IDENTIFIER_SIZE)
	{
		assert(false && "Invalid string size to set atomic expression");
	}

	m_szVariable = _strdup(szValue);
	SetDominantType(E_DOM_STRING);
}

void ExpressionConstant::SetVariableName(char *szVariableName)
{
	SetDominantType(E_DOM_STRING);
	m_szVariable = _strdup(szVariableName);
}

void ExpressionConstant::CollectInternalVariableNames(VariablesNameSet& setOfVariableNames)
{
	// Nothing to add
}

IExpression* ExpressionConstant::Clone()
{
	ExpressionConstant* pCloned = new ExpressionConstant(mDebugLineNo);
	CopyBase(pCloned);

	pCloned->m_IntValue = m_IntValue;
	pCloned->m_CharValue = m_CharValue;
	pCloned->m_FloatValue = m_FloatValue;
	pCloned->m_BoolValue = m_BoolValue;
	pCloned->m_szVariable = (m_szVariable ? _strdup(m_szVariable): NULL);

	pCloned->m_eDominantType = m_eDominantType;

	return pCloned;
}

bool ExpressionVariable::ValidateExpression(SymbolTable* pSymbolTable)
{
	IDataTypeItem* pItem = pSymbolTable->GetIdentifierFromTable(m_szName);
	if (pItem == NULL)
		return false;

	m_eDominantType = (EDominantType) pItem->m_eDataType;
	switch(m_eDominantType)
	{
		case E_DOM_BOOL:
			m_pAsBoolData = (BoolDataItem*) pItem;
			break;
		case E_DOM_FLOAT:
			m_pAsFloatData = (FloatDataItem*) pItem;
			break;
		case E_DOM_INT:
			m_pAsIntData = (IntDataItem*) pItem;
			break;
		case E_DOM_CHAR:
			m_pAsCharData = (CharDataItem*) pItem;
			break;
		case E_DOM_STRING:
			m_pAsStringData = (StringDataItem*) pItem;
			break;
		default:
			assert(false);
			return false;
	}

	return true;
}

void ExpressionVariable::CollectInternalVariableNames(VariablesNameSet& setOfVariableNames)
{
	setOfVariableNames.insert(std::string(m_szName));
}

int ExpressionVariable::GetValueAsInt()
{ 
	if (m_eDominantType == E_DOM_INT)
		return m_pAsIntData->GetValue(); 
	else
	{
		if (m_eDominantType == E_DOM_FLOAT)
			return (int)m_pAsFloatData->GetValue();
		else if (m_eDominantType == E_DOM_BOOL)
			return (int)m_pAsBoolData->GetValue();
		else if (m_eDominantType == E_DOM_CHAR)
			return (char) m_pAsCharData->GetValue();
		else { assert(false); return 0;}
	}
}

void ExpressionVariable::SetValueAsInt(int value)
{
	m_pAsIntData->SetValue(value);
}

float ExpressionVariable::GetValueAsFloat()
{ 
	if (m_eDominantType == E_DOM_FLOAT)
		return m_pAsFloatData->GetValue(); 
	else
	{
		if (m_eDominantType == E_DOM_INT)
			return (float)m_pAsIntData->GetValue();
		else if (m_eDominantType == E_DOM_BOOL)
			return (float)m_pAsBoolData->GetValue();
		else if (m_eDominantType == E_DOM_CHAR)
			return (float) m_pAsCharData->GetValue();
		else { assert(false); return 0;}
	}
}

#pragma warning(disable : 4800)
bool ExpressionVariable::GetValueAsBool()
{ 
	if (m_eDominantType == E_DOM_BOOL)
		return m_pAsBoolData->GetValue(); 
	else
	{
		if (m_eDominantType == E_DOM_INT)
			return (bool)m_pAsIntData->GetValue();
		else if (m_eDominantType == E_DOM_FLOAT)
			return (bool)m_pAsFloatData->GetValue();
		else if (m_eDominantType == E_DOM_CHAR)
			return (bool) m_pAsCharData->GetValue();
		else { assert(false); return 0;}
	}
}
#pragma warning(default : 4800)

char* ExpressionVariable::GetValueAsString()
{
	if (m_eDominantType == E_DOM_STRING)
		return m_pAsStringData->GetValue();

	assert(false);
	return NULL;
}

void ExpressionVariable::SetName(char *szIdentifier)
{
	m_szName = _strdup(szIdentifier);
}

IExpression* ExpressionVariable::Clone()
{
	ExpressionVariable* pCloned = new ExpressionVariable(mDebugLineNo);
	CopyBase(pCloned);

	pCloned->m_szName = _strdup(m_szName);
	//pCloned->m_InputRefItem = m_InputRefItem->Clone();

	return pCloned;
}

void ExpressionValueNode::SetTypeAndChilds(EValueExpressionType eOpType, IExpression *pChild0, IExpression *pChild1)
{
	m_eType = eOpType;
	assert(pChild0);

	m_Childs[0] = pChild0;
	m_Childs[1] = pChild1;
	m_iChildsCount = (pChild1 != NULL ? 2 : 1);
}

bool ExpressionValueNode::ValidateExpression(SymbolTable* pSymbolTable)
{
	// Verify child
	bool bRes[2];
	bRes[0] = bRes[1] = true;
	for (int i = 0; i < m_iChildsCount; i++)
	{
		bRes[i] = m_Childs[i]->ValidateExpression(pSymbolTable);
	}

	bool bResult = (bRes[0] && bRes[1]);
	if (bResult == false)
		return false;

	// If child expressions are ok, check the types
	if (m_eType == E_UNARYMINUS || m_eType == E_OP_INCREMENT || m_eType == E_OP_PDECREMENT || m_eType == E_OP_DECREMENT || m_eType == E_OP_PINCREMENT)
	{
		assert(m_Childs[0] != NULL && m_Childs[1] == NULL && "For this type of expression second child should be NULL while first, valid");

		EDominantType eDomType = m_Childs[0]->GetDominantType();
		SetDominantType(eDomType);

		if (eDomType == E_DOM_STRING || eDomType == E_DOM_BOOL)
		{
			PrintCompileError(mDebugLineNo, "line", "Invalid types for operator -");
			return false;
		}
		return true;
	}
	else
	{
		assert(m_Childs[0] != NULL && m_Childs[1] != NULL && "For this type of expression both childs should be valid");

		int res = IExpression::GetDominantOfTwoTypes(m_Childs[0]->GetDominantType(), m_Childs[1]->GetDominantType());
		SetDominantType((EDominantType) res);

		if (res == -1)
		{
			PrintCompileError(mDebugLineNo, "line", "Invalid types %d and %d to make operation %d");//, pExpr1->GetDominantType(), pExpr2->GetDominantType(), type);
			return false;
		}		

		return true;
	}
}

void ExpressionValueNode::CollectInternalVariableNames(VariablesNameSet& setOfVariableNames)
{
	// Collect info from subtrees
	for (int i = 0; i < m_iChildsCount; i++)
		m_Childs[i]->CollectInternalVariableNames(setOfVariableNames);
}

#define VALUES_EXPRESSIONS_VALUE(val1, val2, res)			\
	switch(m_eType)											\
	{														\
		case E_UNARYMINUS:	res = -val1; break;				\
		case E_OP_PLUS:		res = (val1 + val2); break;		\
		case E_OP_MINUS:	res = (val1 - val2); break;		\
		case E_OP_MULT:		res = (val1 * val2); break;		\
		case E_OP_INCREMENT:								\
		case E_OP_PINCREMENT:								\
			res = val1+1; break;							\
		case E_OP_DECREMENT:								\
		case E_OP_PDECREMENT:								\
			res = val1-1; break;							\
		case E_OP_DIV:		res = (val1 / val2); break;		\
	}

bool ExpressionValueNode::GetValueAsBool()
{
	bool val1 = m_Childs[0]->GetValueAsBool();
	bool val2 = m_Childs[1]->GetValueAsBool();
	bool res = false;

	#pragma warning( disable : 4800 4804 )
	VALUES_EXPRESSIONS_VALUE(val1, val2, res);
	#pragma warning( default : 4800 4804 )

	return res;
}

int ExpressionValueNode::GetValueAsInt()
{
	int val1 = m_Childs[0]->GetValueAsInt();
	int val2 = (m_Childs[1] != NULL ? m_Childs[1]->GetValueAsInt() : 0);
	int res = 0;

	VALUES_EXPRESSIONS_VALUE(val1, val2, res);
	switch(m_eType)
	{
	case E_OP_MOD:
		{
			res = val1 % val2;
		}
		break;
	case E_OP_DECREMENT:
	case E_OP_INCREMENT:
		{
			m_Childs[0]->SetValueAsInt(res);
		}
		break;
	}
	return res;
}

float ExpressionValueNode::GetValueAsFloat()
{
	float val1 = m_Childs[0]->GetValueAsFloat();
	float val2 = m_Childs[1]->GetValueAsFloat();
	float res = 0;

	VALUES_EXPRESSIONS_VALUE(val1, val2, res);	

	return true;
}

char* ExpressionValueNode::GetValueAsString()
{
	assert(false && "NOT IMPLEMENTED");
	return NULL;
}

IExpression* ExpressionValueNode::Clone()
{
	ExpressionValueNode* pCloned = new ExpressionValueNode(mDebugLineNo);
	CopyBase(pCloned);

	pCloned->m_eType = m_eType;
	pCloned->m_iChildsCount = m_iChildsCount;
	for (int i = 0; i < 2; i++)
	{
		if (m_Childs[i]) pCloned->m_Childs[i] = m_Childs[i]->Clone();
		else pCloned->m_Childs[i] = NULL;
	}

	return pCloned;
}

void ExpressionBooleanNode::SetTypeAndChilds(EBooleanExpressionType eOpType, IExpression *pChild0, IExpression *pChild1)
{
	assert(pChild0 && pChild1);

	m_eType = eOpType;
	m_Childs[0] = pChild0;
	m_Childs[1] = pChild1;
}

bool ExpressionBooleanNode::ValidateExpression(SymbolTable* pSymbolTable)
{
	// Check if both expressions are valid
	bool bRes[2];
	bRes[0] = m_Childs[0]->ValidateExpression(pSymbolTable);
	bRes[1] = m_Childs[1]->ValidateExpression(pSymbolTable);

	if (!bRes[0] || !bRes[1])	return false;

	// Check if dominant types are compatible
	int res = IExpression::GetDominantOfTwoTypes(m_Childs[0]->GetDominantType(), m_Childs[1]->GetDominantType());
	if (res == -1)
		return false;

	m_eDominantType = E_DOM_BOOL; //(EDominantType) res;
	return true;
}

void ExpressionBooleanNode::CollectInternalVariableNames(VariablesNameSet& setOfVariableNames)
{
	// Collect info from subtrees
	m_Childs[0]->CollectInternalVariableNames(setOfVariableNames);
	m_Childs[1]->CollectInternalVariableNames(setOfVariableNames);
}

#define BOOLEAN_VALUES_COMPARISION(m_eType, val1, val2)	\
	switch(m_eType)										\
	{													\
	case E_DIFFERENT: return (val1 != val2);			\
	case E_EQUAL: return (val1 == val2);				\
	case E_BIG_OR_EQUAL_THAN: return (val1 >= val2);	\
	case E_BIG_THAN: return (val1 > val2);				\
	case E_SMALL_THAN: return (val1 < val2);			\
	case E_SMALL_OR_EQUAL_THAN: return (val1 <=val2);	\
	}


bool ExpressionBooleanNode::GetValueAsBool()
{
	EDominantType eType = (EDominantType) IExpression::GetDominantOfTwoTypes(m_Childs[0]->GetDominantType(), m_Childs[1]->GetDominantType());
	
	switch(eType)
	{
		case E_DOM_BOOL:
			{
				bool val1 = m_Childs[0]->GetValueAsBool();
				bool val2 = m_Childs[1]->GetValueAsBool();
				
				//BOOLEAN_VALUES_COMPARISION(m_eType, val1, val2);

				if (m_eType == E_OR)
					return (val1 || val2);
				else if (m_eType == E_AND)
					return (val1 && val2);
			}
			break;
		case E_DOM_CHAR:
			{
				char val1 = m_Childs[0]->GetValueAsChar();
				char val2 = m_Childs[1]->GetValueAsChar();
				BOOLEAN_VALUES_COMPARISION(m_eType, val1, val2);
			}
			break;
		case E_DOM_INT:
			{
				int val1 = m_Childs[0]->GetValueAsInt();
				int val2 = m_Childs[1]->GetValueAsInt();
				BOOLEAN_VALUES_COMPARISION(m_eType, val1, val2);
			}
			break;
		case E_DOM_FLOAT:
			{
				float val1 = m_Childs[0]->GetValueAsFloat();
				float val2 = m_Childs[1]->GetValueAsFloat();
				BOOLEAN_VALUES_COMPARISION(m_eType, val1, val2);
			}
			break;
		case E_DOM_STRING:
			{
				char *val1 = m_Childs[0]->GetValueAsString();
				char *val2 = m_Childs[1]->GetValueAsString();

				int compareValue = strcmp(val1, val2);
				switch(m_eType)
				{
					case E_EQUAL: return (compareValue == 0);
					case E_DIFFERENT: return (compareValue != 0);
					case E_BIG_THAN: return (compareValue > 0);
					case E_BIG_OR_EQUAL_THAN: return (compareValue >= 0);
					case E_SMALL_THAN: return (compareValue < 0);
					case E_SMALL_OR_EQUAL_THAN: return (compareValue <= 0);
					default:
						assert("Invalid operator");	
						return false;
				}
			}
			break;
		case E_DOM_ARRAY:
			return false;
	}

	PrintExecutionError("EXECUTION ERROR: Couldn't \n");
	assert(false && "Shouldn't arrive here without a result !!");
	return false;
}

float ExpressionBooleanNode::GetValueAsFloat() { return (float)GetValueAsBool(); }
int ExpressionBooleanNode::GetValueAsInt() { return (int)GetValueAsBool(); }
char* ExpressionBooleanNode::GetValueAsString() { assert(false); return NULL; }

IExpression* ExpressionBooleanNode::Clone()
{
	ExpressionBooleanNode* pCloned = new ExpressionBooleanNode(mDebugLineNo);
	CopyBase(pCloned);

	pCloned->m_eType = m_eType;
	for (int i = 0; i < 2; i++)
	{
		if (m_Childs[i]) pCloned->m_Childs[i] = m_Childs[i]->Clone();
		else pCloned->m_Childs[i] = NULL;
	}

	return pCloned;
}

int IExpression::GetSerializedDataTypeSize(const IExpression* expr)
{
	if (expr == NULL)
		return 0;

	int headerSize = sizeof(int) + sizeof(expr->m_eGeneralType);
	int insideDataSize = 0;

	switch(expr->GetGeneralExpressionType())
	{
	case E_EXPR_CONSTANT:
		{
			// + Dominant type | Constant value
			headerSize += sizeof(expr->m_eDominantType);

			const ExpressionConstant* constExpr = static_cast<const ExpressionConstant*>(expr);
			switch(expr->m_eDominantType)
			{
			case E_DOM_BOOL:
				insideDataSize += sizeof(int);
				break;
			case E_DOM_INT:
				insideDataSize += sizeof(constExpr->m_IntValue);
				break;
			case E_DOM_CHAR:
				insideDataSize += sizeof(int);
				break;
			case E_DOM_FLOAT:
				insideDataSize += sizeof(constExpr->m_FloatValue);
				break;
			case E_DOM_STRING:
				insideDataSize += Streams::GetStringSizeOnStream(constExpr->m_szVariable);
				break;
			default:
				{
					LOG(("This type of dominant type %d can't be serialized for constant expressions\n", expr->m_eDominantType));
					assert(false);
				}
				break;
			}
		}
		break;
	case E_EXPR_VARIABLE:
		{
			// Name of the variable
			const ExpressionVariable* exprVar = static_cast<const ExpressionVariable*> (expr);
			insideDataSize += Streams::GetStringSizeOnStream(exprVar->m_szName);
		}
		break;
	case E_EXPR_ARRAY_ACCESS:
		{
			// Name of the array + index expr serialization
			const ExpressionArrayAccess* exprArrAcc = static_cast<const ExpressionArrayAccess*> (expr);
			insideDataSize += Streams::GetStringSizeOnStream(exprArrAcc->m_szArrayName);
			insideDataSize += GetSerializedDataTypeSize(exprArrAcc->m_pIndexExpression);
		}
		break;
	case E_EXPR_BOOLEAN:
		{
			// size | boolean expr type | left child | right child
			const ExpressionBooleanNode* exprBoolVal = static_cast<const ExpressionBooleanNode*> (expr);				
			insideDataSize += sizeof(exprBoolVal->m_eType);
			insideDataSize += GetSerializedDataTypeSize(exprBoolVal->m_Childs[0]);
			insideDataSize += GetSerializedDataTypeSize(exprBoolVal->m_Childs[1]);
		}
		break;
	case E_EXPR_VALUE:
		{
			// size | expr type | left child | right child
			const ExpressionValueNode* exprVal = static_cast<const ExpressionValueNode*> (expr);				
			insideDataSize += sizeof(exprVal->m_eType);
			insideDataSize += GetSerializedDataTypeSize(exprVal->m_Childs[0]);
			insideDataSize += GetSerializedDataTypeSize(exprVal->m_Childs[1]);
		}
		break;
	default:
		{
			char buff[128];
			sprintf(buff, "This type of expression serialization was not implemented %d", expr->GetGeneralExpressionType());
			assert(false && buff);
		}
		break;
	}	
	
	const int totalSize = headerSize + insideDataSize;
	return totalSize;
}

int IExpression::SerializeDataType(const IExpression* expr, Streams::BytesStreamWriter& stream)
{
	if (expr == NULL)
		return 0;

	// Write: Size | General type |.... specific data ....

	char* headerExprPos = stream.m_BufferPos;
	stream.WriteSimpleType(0);	// dummy write, this will be rewritten later
	stream.WriteSimpleType(expr->m_eGeneralType);

	// TODO Optimization: remove this to serialize faster. This is done here because it is used just on compile time
	const int expectedDataTypeSize = GetSerializedDataTypeSize(expr);

	switch(expr->GetGeneralExpressionType())
	{
	case E_EXPR_CONSTANT:
		{
			// + Dominant type | Constant value
			stream.WriteSimpleType(expr->m_eDominantType);

			const ExpressionConstant* constExpr = static_cast<const ExpressionConstant*>(expr);
			switch(expr->m_eDominantType)
			{
			case E_DOM_BOOL:
				{
					LOG(("This version of AGAPIA supports as constant expressions only int and floats"));
					assert(false);
					//stream.WriteSimpleType((int)constExpr->m_BoolValue);
				}				
				break;
			case E_DOM_INT:
				{
					stream.WriteSimpleType(constExpr->m_IntValue);
				}
				break;
			case E_DOM_CHAR:
				{
					LOG(("This version of AGAPIA supports as constant expressions only int and floats"));
					assert(false);
					//stream.WriteSimpleType((int)constExpr->m_CharValue);
				}
				break;
			case E_DOM_FLOAT:
				{
					stream.WriteSimpleType(constExpr->m_FloatValue);
				}
				break;
			case E_DOM_STRING:
				{
					LOG(("This version of AGAPIA supports as constant expressions only int and floats"));
					assert(false);
					//stream.WriteString(constExpr->m_szVariable);
				}
				break;
			default:
				{
					LOG(("This type of dominant type %d can't be serialized for constant expressions\n", expr->m_eDominantType));
					assert(false);
				}
				break;
			}
		}
		break;
	case E_EXPR_VARIABLE:
		{
			// +  Name of the variable
			const ExpressionVariable* exprVar = static_cast<const ExpressionVariable*> (expr);
			stream.WriteString(exprVar->m_szName);
		}
		break;
	case E_EXPR_ARRAY_ACCESS:
		{
			// Name of the array + index expr serialization
			const ExpressionArrayAccess* exprArrAcc = static_cast<const ExpressionArrayAccess*> (expr);
			stream.WriteString(exprArrAcc->m_szArrayName);
			IExpression::SerializeDataType(exprArrAcc->m_pIndexExpression, stream);
		}
		break;
	case E_EXPR_BOOLEAN:
		{
			// size | boolean expr type | left child | right child
			const ExpressionBooleanNode* exprBoolVal = static_cast<const ExpressionBooleanNode*> (expr);				
			stream.WriteSimpleType(exprBoolVal->m_eType);
			IExpression::SerializeDataType(exprBoolVal->m_Childs[0], stream);
			IExpression::SerializeDataType(exprBoolVal->m_Childs[1], stream);
		}
		break;
	case E_EXPR_VALUE:
		{
			// size | expr type | left child | right child
			const ExpressionValueNode* exprVal = static_cast<const ExpressionValueNode*> (expr);				
			stream.WriteSimpleType(exprVal->m_eType);
			IExpression::SerializeDataType(exprVal->m_Childs[0], stream);
			IExpression::SerializeDataType(exprVal->m_Childs[1], stream);
		}
		break;
	default:
		{
			char buff[128];
			sprintf(buff, "This type of expression serialization was not implemented %d", expr->GetGeneralExpressionType());
			assert(false && buff);
		}
		break;
	}	

	const int totalSize = stream.m_BufferPos - headerExprPos;
	assert(expectedDataTypeSize == totalSize && "A different data size than expected was written for expression serialization");
	memcpy(headerExprPos, &totalSize, sizeof(int));

	return totalSize;
}

IExpression* IExpression::DeserializeDataType(Streams::BytesStreamReader& stream)
{
	const char* beginDesAddr = stream.m_BufferPos;

	int expectedDataSizeToRead = 0;
	stream.ReadSimpleType(expectedDataSizeToRead);	
	EGeneralExpressionType exprType = E_EXPR_CONSTANT;
	stream.ReadSimpleType(exprType);

	IExpression* result = NULL;

	switch(exprType)
	{
		case E_EXPR_CONSTANT:
			{
				EDominantType domType = E_DOM_INT;
				stream.ReadSimpleType(domType);

				switch (domType)
				{
				case E_DOM_BOOL:
					{
						LOG(("This version of AGAPIA supports as constant expressions only int and floats"));
						assert(false);
						/*bool v = false;
						result = static_cast<IExpression*>(ABSTFactory::CreateAtomicExpression(v));
							*/
					}
					break;
				case E_DOM_CHAR:
					{
						LOG(("This version of AGAPIA supports as constant expressions only int and floats"));
						assert(false);

						/*char v = 0;
						result = static_cast<IExpression*>(ABSTFactory::CreateAtomicExpression(v));*/
					}
					break;
				case E_DOM_INT:
					{
						int v = 0;
						stream.ReadSimpleType(v);
						result = static_cast<IExpression*>(ABSTFactory::CreateAtomicExpression(v));
					}
					break;
				case E_DOM_FLOAT:
					{
						float v = 0;
						stream.ReadSimpleType(v);
						result = static_cast<IExpression*>(ABSTFactory::CreateAtomicExpression(v));
					}
					break;
				default:
					assert(false && "This type of constant expression deserialization is not implement");
				}
			}
			break;
		case E_EXPR_VARIABLE:
			{
				char buff[128];
				stream.ReadString(buff, 128);
				result = static_cast<IExpression*>(ABSTFactory::CreateAtomicExprIdentifier(buff));
			}
			break;
		case E_EXPR_ARRAY_ACCESS:
			{
				char name[128];
				stream.ReadString(name, 128);

				IExpression* exprIndex = IExpression::DeserializeDataType(stream);
				result = static_cast<IExpression*>(ABSTFactory::CreateArrayAccessExpression(name, exprIndex));
			}
			break;
		case E_EXPR_BOOLEAN:
			{
				IExpression* exprLeft	= NULL;
				IExpression* exprRight	= NULL;

				EBooleanExpressionType type;
				stream.ReadSimpleType(type);

				// By default every boolean expr has 2 childs
				/*switch(type)
				{
				default:
				*/

					{
						exprLeft	= IExpression::DeserializeDataType(stream);
						exprRight	= IExpression::DeserializeDataType(stream);
					}
/*					break;
				}
*/
   			    result = static_cast<IExpression*>(ABSTFactory::CreateBooleanExpression(exprLeft, exprRight, type));
			}
			break;
		case E_EXPR_VALUE:
			{
				IExpression* exprLeft	= NULL;
				IExpression* exprRight	= NULL;

				EValueExpressionType type;
				stream.ReadSimpleType(type);

				// By default every boolean expr has 2 childs
				switch(type)
				{
					case E_UNARYMINUS:
					case E_OP_INCREMENT:
					case E_OP_PDECREMENT:
					case E_OP_DECREMENT:
					case E_OP_PINCREMENT:
						{
							exprLeft	= IExpression::DeserializeDataType(stream);
						}
						break;
					default:
						{
							exprLeft	= IExpression::DeserializeDataType(stream);
							exprRight	= IExpression::DeserializeDataType(stream);
						}
						break;
				}

				result = static_cast<IExpression*>(ABSTFactory::CreateValueExpression(exprLeft, exprRight, type));
			}
			break;
		default:
			{
				char buff[128];
				sprintf(buff, "This type of expression de-serialization was not implemented %d", exprType);
				assert(false && buff);
			}
			break;
	}

	assert(result != NULL && "Result variable was not set at the end of deserialization process");
	const int dataRead = stream.m_BufferPos - beginDesAddr;
	assert(dataRead == expectedDataSizeToRead && "Didn't read data as expected on deserialization process");
	return result;
}



