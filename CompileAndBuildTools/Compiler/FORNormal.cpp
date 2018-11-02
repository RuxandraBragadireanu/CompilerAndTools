#include "FORNormalNode.h"
#include "Expressions.h"
#include "SymbolTable.h"
#include "ASIGNMENTNode.h"
#include "calc3_utils.h"
#include "ReferenceNode.h"

ProgramBase* ProgramFORNormal::Clone()
{
	ProgramFORNormal* pClone = new ProgramFORNormal(mDebugLineNo);
	CopyBase(pClone);

	pClone->m_pCondition = m_pCondition->Clone();
	pClone->m_pInitExpression = static_cast<ProgramAsignment*>(m_pInitExpression->Clone());
	pClone->m_pPostExpression = m_pPostExpression->Clone();
	pClone->m_eWHILEType = m_eWHILEType;
	pClone->mForType = mForType;

	pClone->m_pBaseProgram = m_pBaseProgram->Clone();

	if (m_pBaseProgram->GetType() == E_NODE_TYPE_MODULE_REF)
	{
		ProgramReference* pPRef = (ProgramReference*)m_pBaseProgram;
		pClone->m_pBaseProgram = ProgramReference::FindAndCloneRef(pPRef);
	}
	else
		pClone->m_pBaseProgram = m_pBaseProgram->Clone();

	pClone->m_ExpectedInputsMap.insert(m_ExpectedInputsMap.begin(), m_ExpectedInputsMap.end());

	// TODO HERE
	pClone->m_iNrInputsNeededInNorth = m_iNrInputsInBufferInNorth;
	pClone->m_iNrInputsInBufferInWest = m_iNrInputsNeededInWest;

	return pClone;
}

bool ProgramFORNormal::DoSpecificValidationOrSymbolInherit(SymbolTable* pParent)
{
	// Validate and do the assignment initialization	
	//-----------------------------------------------------
	// Left side must be a variable
	if (m_pInitExpression->m_ChildExpressions[0]->GetGeneralExpressionType() != E_EXPR_VARIABLE)
	{
		PrintCompileError(mDebugLineNo, "In a FOR initialization expression, the left side of assignment must be a variable");
		return false;
	}


	// Add the variable to the symbol table - we know that only IntData is valid for this in the current AGAPIA version so use this.
	ExpressionVariable* pLeftExpression = static_cast<ExpressionVariable*>(m_pInitExpression->m_ChildExpressions[0]);
	if (!m_SymbolTable->AddSymbol(pLeftExpression->m_szName, new IntDataItem())
		|| !pLeftExpression->ValidateExpression(m_SymbolTable))
	{
		PrintCompileError(mDebugLineNo, "In a FOR initialization expression, the left side of assignment must be a variable.|||| which is also not used in the current scope");
		return false;
	}

	// 
	EDominantType eExprDomType = m_pInitExpression->m_ChildExpressions[1]->GetDominantType();
	if (eExprDomType != E_DOM_INT)
	{
		PrintCompileError(mDebugLineNo, "In this version of AGAPIA: In a FOR initialization expression, the right side must be an integer value OR integer variable and declared in this scope.");
		return false;
	}

	// Do the iteration statement initialization 
	//------------------------------------------------
	if (!m_pPostExpression->ValidateExpression(m_SymbolTable))
	{
		// TODO: any msg??
		return false;
	}
	if (m_pPostExpression->GetGeneralExpressionType() != E_EXPR_VALUE)
	{
		PrintCompileError(mDebugLineNo, "In this version of AGAPIA: In a FOR post statement we must have either a increment or decrement operation (++ or --.");
		return false;
	}
	
	EValueExpressionType valueEvaltype = static_cast<ExpressionValueNode*>(m_pPostExpression)->m_eType;
	if (valueEvaltype != E_OP_INCREMENT && valueEvaltype != E_OP_DECREMENT)
	{
		PrintCompileError(mDebugLineNo, "In this version of AGAPIA: In a FOR post statement we must have either a increment or decrement operation (++ or --.");
		return false;
	}

	//------------------------------------------------

	// OBSERVATION: The condition initialization will be in the parent class WHILENode
	return true;
}

void ProgramFORNormal::OnBeforeFirstTimeConditionEval()
{
	// Do the initialization
	m_pInitExpression->DoAsignment(*m_SymbolTable);
}

void ProgramFORNormal::OnAfterChildProgramFinished()
{
	// TODO !! Should be an assignment or an expression ????
	m_pPostExpression->GetValueAsInt();
}