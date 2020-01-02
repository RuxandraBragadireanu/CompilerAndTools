#include "IFNode.h"
#include "Expressions.h"
#include "ReferenceNode.h"
#include "BufferedDataTypes.h"

#include "calc3_utils.h"

ProgramBase* ProgramIF::Clone()
{
	ProgramIF* pClone = new ProgramIF(mDebugLineNo);
	CopyBase(pClone);

	// Clear the old symbol. Let the validate do its job
	pClone->m_SymbolTable->Clear();

	pClone->m_pCondition = m_pCondition->Clone();
	pClone->m_ExpectedInputsMap.insert(m_ExpectedInputsMap.begin(), m_ExpectedInputsMap.end());

	ProgramBase** pClonedBasePrograms[2] = {&pClone->m_pIfProgram, &pClone->m_pElseProgram};
	ProgramBase* pThisBasePrograms[2] = {m_pIfProgram, m_pElseProgram};

	for (int i = 0; i < 2; i++)
	{
		if (pThisBasePrograms[i]->GetType() == E_NODE_TYPE_MODULE_REF)
		{
			ProgramReference* pPRef = (ProgramReference*)pThisBasePrograms[i];
			(*pClonedBasePrograms[i]) = ProgramReference::FindAndCloneRef(pPRef);
		}
		else
			(*pClonedBasePrograms[i]) = pThisBasePrograms[i]->Clone();
	}
	
	pClone->CreateInputBuffers();

	return pClone;
}

bool ProgramIF::SolveReferences()
{
	if (m_pIfProgram->GetType() == E_NODE_TYPE_MODULE_REF)
	{
		m_pIfProgram = ProgramReference::FindAndCloneRef((ProgramReference*)m_pIfProgram);
		if (m_pIfProgram == NULL) return false;
	}
	else
		if (!m_pIfProgram->SolveReferences())
			return false;

	if (m_pElseProgram)
	{
		if (m_pElseProgram->GetType() == E_NODE_TYPE_MODULE_REF)
		{
			m_pElseProgram = ProgramReference::FindAndCloneRef((ProgramReference*)m_pElseProgram);
			if (m_pElseProgram == NULL) return false;
		}
		else
			if (!m_pElseProgram->SolveReferences())
				return false;
	}

	return true;
}

bool ProgramIF::SolveInputOutput()
{
	// Solve childs programs
	ProgramBase* pChilds[2] = { m_pIfProgram, m_pElseProgram };
	for (int i = 0; i < 2; i++)
	{
		if (pChilds[i] && !pChilds[i]->SolveInputOutput())
			return false;
	}

	// Create input blocks
	m_pInputNorth = InputBlock::CreateAndLinkInputBlock(pChilds[0]->m_pInputNorth, NULL, true);
	m_pInputWest = InputBlock::CreateAndLinkInputBlock(pChilds[0]->m_pInputWest, NULL, true);
	m_pOutputSouth = InputBlock::CreateAndLinkInputBlock(pChilds[0]->m_pOutputSouth, NULL, false);
	m_pOutputEast = InputBlock::CreateAndLinkInputBlock(pChilds[0]->m_pOutputEast, NULL, false);
	
	// Set input indexes for north and west block - we'll need them later
	ArrayOfBaseProcessInputs* inputBlocks[2] = { &m_pInputNorth->m_InputsInBlock, &m_pInputWest->m_InputsInBlock };
	for (int i = 0; i < 2; i++)
	{
		unsigned int iIndex = 0;
		for (ArrayOfBaseProcessInputsIter it = inputBlocks[i]->begin(); it != inputBlocks[i]->end(); it++)
			(*it)->SetIndexInBlock(iIndex++);
	}

	// Make these blocks childs of this program
	InputBlock* pBlocks[4] = {m_pInputNorth, m_pInputWest, m_pOutputEast, m_pOutputSouth };
	for (int i = 0; i < 4 ; i++)
		pBlocks[i]->SetParent(this);

	m_pInputNorth->SetAsNorthBlock();

	CreateInputBuffers();

	return true;
}	

void ProgramIF::CreateInputBuffers()
{
	// Create the north and south buffers
	ArrayOfBaseProcessInputs* inputBlocks[2] = { &m_pInputNorth->m_InputsInBlock, &m_pInputWest->m_InputsInBlock };

	for (int i = 0; i < E_NUM_DIRECTION; i++)
		m_BufferInputBlocks[i] = new BufferedInputBlock(inputBlocks[i]->size());
}

bool ProgramIF::Validate(SymbolTable* pParent, bool bAtRuntimeSpawn)
{
	ProgramBase::DoBaseSymbolTableInheritance(pParent);

	bool bHasElseProgram = true;
	if (m_pElseProgram == NULL) 
		bHasElseProgram = false;

	// Both programs , if and else, must have the same type
	//-----------------------------------------------------------
	if (bHasElseProgram)
	{
		InputBlock* if_childs[4] = {m_pIfProgram->m_pInputNorth, m_pIfProgram->m_pInputWest, m_pIfProgram->m_pOutputEast, m_pIfProgram->m_pOutputSouth};
		InputBlock* else_childs[4] = {m_pElseProgram->m_pInputNorth, m_pElseProgram->m_pInputWest, m_pElseProgram->m_pOutputEast, m_pElseProgram->m_pOutputSouth};

		for (int i = 0; i < 4; i++)
			if (!if_childs[i]->VerifyInputMatching(else_childs[i], true))
			{
				PrintCompileError(GetLineNo(), "The if and else programs must have the same interface types !");
				return false;
			}
	}
	else	// If the program doesn't have an else branch, we should ensure that the input and output of the if branch are the same (so that if the conditions doesn't met, the links are stable)
	{
		if (!m_pIfProgram->m_pInputNorth->VerifyInputMatching(m_pIfProgram->m_pOutputSouth, true))
		{
			PrintCompileError(GetLineNo(), "When you use an if program without else, make sure that the if branch program has the same interfaces for north-south and west-east!");
			return false;
		}

		if (!m_pIfProgram->m_pInputWest->VerifyInputMatching(m_pIfProgram->m_pOutputEast, true))
		{
			PrintCompileError(mDebugLineNo, "When you use an if program without else, make sure that the if branch program has the same interfaces for north-south and west-east!");
			return false;
		}
	}

	// Inherit one of the child symbol table - both sub-programs should have the same interfaces and names
	ProgramBase::DoSymbolTableInheritInputBlocks(m_pInputNorth, m_pInputWest, true);

	// Verify if all symbols within expression are contained in the symbol table
	//-----------------------------------------------------------
	if (!m_pCondition->ValidateExpression(m_SymbolTable))
	{
		PrintCompileError(mDebugLineNo, "Expression condition of IF couldn't be validated. It is possible to contain variables that are not accessible in that scope ?\n");
		return false;
	}

	// Cache the pointer to name if one exists
	m_pCondition->CollectInternalVariableNames(m_ExpectedInputsMap);

	// Check if the condition variables exists in the symbol table and link to them
	for (VariablesNameSetIter it = m_ExpectedInputsMap.begin(); it != m_ExpectedInputsMap.end(); it++)
	{
		IDataTypeItem* pItem = m_SymbolTable->GetIdentifierFromTable((char*)it->c_str());
		if (pItem == NULL)
		{
			PrintCompileError(mDebugLineNo, "The variable %s in the IF's condition was not found", (char*)it->c_str());
			return false;
		}
		pItem->AddProgramListener(this);	
	}

	// Validate the child programs
	//-----------------------------------------------------------
	ProgramBase* pChilds[2] = { m_pIfProgram, m_pElseProgram };

	for (int i = 0; i < 2; i++)
	{
		if (pChilds[i] == NULL) continue;
		bool bShouldInherit = ProgramBase::GetShouldGiveParentTableTo(pChilds[i]);
		if (!pChilds[i]->Validate(bShouldInherit ? pParent : NULL, bAtRuntimeSpawn))
			return false;
	}

	if (bHasElseProgram)
	{
		// Check if both branch programs have the same names for variables: 
		// In this version of AGAPIA, we need the same names. They have the same interface so, there is no reason to have different names..i think
		//-----------------------------------------------------------
		SymbolTable* pChildIF_ST = pChilds[0]->m_SymbolTable;
		SymbolTable* pChildELSE_ST = pChilds[1]->m_SymbolTable;
		if (pChildIF_ST->GetNumSymbols() != pChildELSE_ST->GetNumSymbols())
		{
			PrintCompileError(mDebugLineNo, "In the IF module, the if and else branch programs should have the same number of symbols\n");
			return false;
		}

		// Check symbols from both interfaces one by one.
		// If their names or types differ, then is not valid
		pChildIF_ST->BeginSymbolsIteration();
		pChildELSE_ST->BeginSymbolsIteration();

		while(1) 
		{
			bool bHasNextIF = pChildIF_ST->HasNext();
			bool bHasNextELSE = pChildELSE_ST->HasNext();

			if (bHasNextIF == bHasNextELSE && bHasNextIF == false) 
				break;

			SymbolEntryPair p0 = pChildIF_ST->GetNextSymbol();
			SymbolEntryPair p1 = pChildELSE_ST->GetNextSymbol();

			if (p0.first != p1.first || !p0.second->SameTypeAs(p1.second))
			{
				PrintCompileError(mDebugLineNo, "In the IF program: The types if/else programs differ. They should be the same.");
				return false;
			}
		}
	}

	return true;
}

SymbolArrivedResult ProgramIF::OnSymbolArrived(char *szName)
{
	SymbolArrivedResult result = SAR_NOT_NEEDED;
	ExpectedConditionInputsIter it = m_ExpectedInputsMap.find(szName);
	if (it != m_ExpectedInputsMap.end())
	{
		m_ExpectedInputsMap.erase(it);
		result = SAR_NEEDED;
	}

	if (m_ExpectedInputsMap.size() == 0)
		result = SAR_NEEDED_AND_FINAL;

	return result;
}

#define DEFINE_IF_NODE_FULL_LINKS	\
	InputBlock* pInputs[4] = { m_pInputNorth,						m_pInputWest,						m_pProgramToChoose->m_pOutputSouth,		m_pProgramToChoose->m_pOutputEast};	\
	InputBlock* pOutputs[4]= { m_pProgramToChoose->m_pInputNorth,	m_pProgramToChoose->m_pInputWest,	m_pOutputSouth,							m_pOutputEast};

#define DEFINE_IF_NODE_NULL_LINKS	\
	InputBlock* pInputs[4] = { m_pInputNorth, m_pInputWest, NULL, NULL };	\
	InputBlock* pOutputs[4] = { m_pOutputSouth, m_pOutputEast, NULL, NULL };

void RebuildLinks(InputBlock* pInputs[4], InputBlock* pOutputs[4], int nrEntries)
{
	// Link input to output 
	for (int i = 0; i < nrEntries; i++)
	{
		InputBlock* pInputBlock = pInputs[i];
		InputBlock* pOutputBlock = pOutputs[i];

		for (ArrayOfBaseProcessInputsIter itInput = pInputBlock->m_InputsInBlock.begin(), itOutput = pOutputBlock->m_InputsInBlock.begin(); itInput != pInputBlock->m_InputsInBlock.end(); itInput++, itOutput++)
		{
			BaseProcessInput* pInputProcess = *itInput;
			BaseProcessInput* pOutputProcess = *itOutput;

			BaseProcessInput* pOldNext = pInputProcess->m_pNext;
			if (pOldNext)
			{
				pOldNext->m_pFrom = NULL;
			}
			pInputProcess->m_pNext = NULL;

			BaseProcessInput::SetOrientedLink(pInputProcess, pOutputProcess);
		}
	}
}

void ProgramIF::OnAllSymbolsArrived()
{
	if (m_bConditionEvaluated)
		return ;

	// Choose the program to execute
	ProgramBase*	m_pProgramToChoose;
	bool bValue = m_pCondition->GetValueAsBool();
	if (bValue)
		m_pProgramToChoose = m_pIfProgram;
	else 
	{
		if (m_pElseProgram)
			m_pProgramToChoose = m_pElseProgram;
		else
			m_pProgramToChoose = NULL;
	}

	// Link input to output 
	if (m_pProgramToChoose == NULL)
	{
		DEFINE_IF_NODE_NULL_LINKS;
		RebuildLinks(pInputs, pOutputs, 2);
	}
	else
	{
		DEFINE_IF_NODE_FULL_LINKS;
		RebuildLinks(pInputs, pOutputs, 4);
	}

	m_pProgramToChoose->m_pProgramParent = this;

	m_bConditionEvaluated = true;

	// Move the input buffered to the chosen program
	if (m_pProgramToChoose)
	{
		InputBlock* pInputTargetBlocks[E_NUM_DIRECTION] = { m_pProgramToChoose->m_pInputNorth, m_pProgramToChoose->m_pInputWest };

		for (int dir = 0; dir < E_NUM_DIRECTION; dir++)
		{
			if (m_BufferInputBlocks[dir] == NULL)
			{
				PrintExecutionError("Problem inside AGAPIA - buffer is not created\n");
				continue;
			}
			
			const ArrayOfBufferedItems& bufferedItemsArr = m_BufferInputBlocks[dir]->m_bufferedItems;
			for (size_t i = 0; i < bufferedItemsArr.size(); i++)
			{
				if (bufferedItemsArr[i].mBufferedData != NULL)
					pInputTargetBlocks[dir]->m_InputsInBlock[i]->GoDownValue(bufferedItemsArr[i].mBufferedData);
			}
		}
	}
}

bool ProgramIF::OnInputReceived(BaseProcessInput *pOriginalInput, BaseProcessInput* pInputNodeReceiver, EInputDirection eDir)
{
	if (m_bConditionEvaluated)
		return true;

	// Store the buffered input
	m_BufferInputBlocks[eDir]->m_bufferedItems[pInputNodeReceiver->GetIndexInBlock()].mBufferedData = pOriginalInput;

	if (pOriginalInput->GetType() == E_INPUT_SIMPLE)
	{
		bool bIsAnyInputValueNeeded = false;		// Tells that if any input from pInputNodeReceiver is needed for evaluating our condition
		bool bIsReadyToEvaluateCondition = false;	// Tells us if all the variables in the conditions are available

		if (pInputNodeReceiver->GetType() == E_INPUT_SIMPLE)
		{
			SimpleProcessItem* pSPI = static_cast<SimpleProcessItem*>(pInputNodeReceiver);
			for (ListOfInputItemsConstIter it = pSPI->m_InputItems.begin(); it != pSPI->m_InputItems.end(); it++)
			{
				SymbolArrivedResult res = OnSymbolArrived((char*)(*it)->GetName());
				if (res == SAR_NEEDED)
				{
					bIsAnyInputValueNeeded = true;
				}
				else if (res == SAR_NEEDED_AND_FINAL)
				{
					bIsAnyInputValueNeeded = true;
					bIsReadyToEvaluateCondition = true;
				}
			}
		}

		// If any input needed, set the values
		if (bIsAnyInputValueNeeded)
		{	
			pInputNodeReceiver->SetValues(pOriginalInput);
		}

		if (bIsReadyToEvaluateCondition)
		{
			OnAllSymbolsArrived();
			return true;
		}

		return false;
	}
	else 
		return m_bConditionEvaluated;


	// For this version of AGAPIA accept only conditions on simple input types
	
	/*else
	{
		VectorProcessItem* pSVI = static_cast<VectorProcessItem*>(pOriginalInput);
		for (unsigned int i = 0; i < pSVI->m_ArrayOfSimpleInputs.size(); i++)
		{
			if (OnInputReceived(pSVI->m_ArrayOfSimpleInputs[i], pInputNodeReceiver, eDir))
			{
				// Recursive call already called OnAllSymbolsArrived !
				return true;
			}
		}

		return false;
	}
	*/
}

void ProgramIF::OnInputItemReady(char* szName)
{
	if (OnSymbolArrived(szName) == SAR_NEEDED_AND_FINAL)
	{
		OnAllSymbolsArrived();
	}
}

void ProgramIF::OnChildProgramFinished(ProgramBase* pChildFinished)
{
	if (!CheckFinishedCondition())
		return;

	SendProgramFinished(this);
}