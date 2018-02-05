#include "ASIGNMENTNode.h"
#include "Expressions.h"

#include "calc3_utils.h"

bool ProgramAsignment::Validate(SymbolTable* pSymbolTable, bool bAtRuntimeSpawn)
{
	if (m_ChildExpressions[0]->m_eGeneralType == E_EXPR_VARIABLE)
	{
		ExpressionVariable* pVar = (ExpressionVariable*) m_ChildExpressions[0];

		IDataTypeItem* pItem = pSymbolTable->GetIdentifierFromTable(pVar->m_szName);
		if (pItem == NULL)
		{
			PrintCompileError(mDebugLineNo, "Assignment, left side identifier is not declared ");
			return false;
		}

		AsignmentCompatibility res = IExpression::GetAsignmentsTypesCompatibility((EDominantType)pItem->m_eDataType, m_ChildExpressions[1]->GetDominantType());
		if (res == ER)
		{
			PrintCompileError(mDebugLineNo, "Assignment, Invalid operator assignment 1");
			return false;
		}
		else if (res == WA)
			PrintCompileWarning(mDebugLineNo, "Assignment, Warning types on assignment");

		return true;
	}
	else if (m_ChildExpressions[0]->m_eGeneralType == E_EXPR_ARRAY_ACCESS)
	{
		if (!((ExpressionArrayAccess*)m_ChildExpressions[0])->ValidateExpression(pSymbolTable))
			return false;
	}

	return m_ChildExpressions[1]->ValidateExpression(pSymbolTable);
}

void ProgramAsignment::DoAsignment(SymbolTable& pSymbolTable)
{
	if (m_ChildExpressions[0]->m_eGeneralType == E_EXPR_VARIABLE)
	{
		ExpressionVariable* pVar = (ExpressionVariable*) m_ChildExpressions[0];
		IDataTypeItem* pDataItem = pSymbolTable.GetIdentifierFromTable(pVar->m_szName);

		// Assign the values
		switch(pDataItem->m_eDataType)
		{
		case ETYPE_BOOL:
			{
				BoolDataItem* pItem = (BoolDataItem*)pDataItem;
				pItem->SetValue(m_ChildExpressions[1]->GetValueAsBool());
			}
			break;
		case ETYPE_CHAR:
			{
				CharDataItem* pItem = (CharDataItem*)pDataItem;
				pItem->SetValue(m_ChildExpressions[1]->GetValueAsChar());
			}
			break;
		case ETYPE_INT:
			{
				IntDataItem* pItem = (IntDataItem*)pDataItem;
				pItem->SetValue(m_ChildExpressions[1]->GetValueAsInt());
			}
			break;
		case ETYPE_FLOAT:
			{
				FloatDataItem* pItem = (FloatDataItem*)pDataItem;
				pItem->SetValue(m_ChildExpressions[1]->GetValueAsFloat());
			}
			break;
		/*case ETYPE_STRING:
			{
				StringDataItem* pItem = (StringDataItem*)pDataItem;
				pItem->SetValue(m_ChildExpressions[1]->GetValueAsString());
			}
			break;
		*/
		case ETYPE_GENERAL_ARRAY:
			{
				assert(false && "Not implemented yet!");
				PrintCompileError(mDebugLineNo, "Assignment, Not implemented yet assignment of array to array!");
				// nothing !
			}
			break;
		default:
			assert(false);
		}		
	}
	else
	{
		ExpressionArrayAccess* pArrayAccess = (ExpressionArrayAccess*) m_ChildExpressions[0];
		switch(pArrayAccess->m_eGeneralType)
		{	
		case E_DOM_BOOL:
			pArrayAccess->SetValue(m_ChildExpressions[1]->GetValueAsBool());
			break;
		case E_DOM_CHAR:
			pArrayAccess->SetValue(m_ChildExpressions[1]->GetValueAsChar());
			break;
		case E_DOM_INT:
			pArrayAccess->SetValue(m_ChildExpressions[1]->GetValueAsInt());
			break;
		case E_DOM_FLOAT:
			pArrayAccess->SetValue(m_ChildExpressions[1]->GetValueAsFloat());
			break;
		case E_DOM_STRING:
			pArrayAccess->SetValue(m_ChildExpressions[1]->GetValueAsString());
			break;
		default:
			assert(false);
		}
	}
}

ProgramBase* ProgramAsignment::Clone()
{
	ProgramAsignment* pCloned = new ProgramAsignment(mDebugLineNo);
	CopyBase(pCloned);

	for (int i = 0; i < 2; i++)
	{
		if (m_ChildExpressions[i]) pCloned->m_ChildExpressions[i] = m_ChildExpressions[i]->Clone();
		else pCloned->m_ChildExpressions[i] = NULL;
	}
	pCloned->m_pInputItem = m_pInputItem->Clone();

	return pCloned;
}
