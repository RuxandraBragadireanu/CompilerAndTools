#include "WHILENode.h"
#include "ReferenceNode.h"
#include "calc3_utils.h"

#include "CommonTypes_Conditionals.h"

ProgramBase* ProgramWHILE::Clone()
{
	ProgramWHILE* pClone = new ProgramWHILE(mDebugLineNo);
	CopyBase(pClone);

	pClone->m_pCondition = m_pCondition->Clone();
	pClone->m_eWHILEType = m_eWHILEType;
	
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

bool ProgramWHILE::SolveReferences()
{
	if (m_pBaseProgram->GetType() == E_NODE_TYPE_MODULE_REF)
	{
		m_pBaseProgram = ProgramReference::FindAndCloneRef((ProgramReference*)m_pBaseProgram);
		if (m_pBaseProgram == NULL) return false;
	}
	else
		if (!m_pBaseProgram->SolveReferences())
			return false;

	return true;
}

bool ProgramWHILE::SolveInputOutput()
{
	m_pBaseProgram->SolveInputOutput();

	//--------------------------------------
	InputBlock* pBaseChildTypes[4] = {	m_pBaseProgram->m_pInputNorth, m_pBaseProgram->m_pInputWest, 
		m_pBaseProgram->m_pOutputSouth,  m_pBaseProgram->m_pOutputEast };

	InputBlock** pTargets[4] = { &m_pInputNorth, &m_pInputWest, &m_pOutputSouth , &m_pOutputEast };

	if (IsSpatialIteration())
	{
		// North and south childs are (childtype ;)[]
		// East and west are childtype
		for (int i = 0; i < 4; i++)
		{
			(*pTargets[i]) = new InputBlock(this);

			//InputBlock
			if (pBaseChildTypes[i]->GetProcessesCount() == 0)	// Leave it as nil
				continue;	

			// West or East?
			if (i == 1 || i == 3)
			{
				(*pTargets[i])->AddInputs(pBaseChildTypes[i]);
			}
			else // North or South 
			{
				VectorProcessItem* pVPI = new VectorProcessItem(*pTargets[i]);
				pVPI->SetItemType(pBaseChildTypes[i]->m_InputsInBlock, "");
				(*pTargets[i])->AddInput(pVPI);
			}		
		}
	}
	else if (IsTemporalIteration())
	{
		// East and west childs are (childtype ;)[]
		// North and south are childtype
		for (int i = 0; i < 4; i++)
		{
			(*pTargets[i]) = new InputBlock(this);

			//InputBlock
			if (pBaseChildTypes[i]->GetProcessesCount() == 0)	// Leave it as nil
				continue;	

			// North or South  
			if (i == 0 || i == 2)
			{
				(*pTargets[i])->AddInputs(pBaseChildTypes[i]);
			}
			else // West or East?
			{
				VectorProcessItem* pVPI = new VectorProcessItem(*pTargets[i]);
				pVPI->SetItemType(pBaseChildTypes[i]->m_InputsInBlock, "");
				(*pTargets[i])->AddInput(pVPI);
			}		
		}
	}	
	else if (IsDiagonalIteration())
	{
		// All interfaces are childtype
		for (int i = 0; i < 4; i++)
		{
			(*pTargets[i]) = new InputBlock(this);

			if (pBaseChildTypes[i]->GetProcessesCount() == 0)	// Leave it as nil
				continue;	

			(*pTargets[i])->AddInputs(pBaseChildTypes[i]);
		}
	}
	else
	{
		assert(false && "Not defined well the type of FOR/WHILE! ");
		return false;
	}

	// -------------------------------------
	m_pInputNorth->SetAsNorthBlock();

	// Type checking of the internal child module on specific for type
	if (IsSpatialIteration())
	{
		// The east and west interface should be the same
		if (!m_pInputWest->VerifyInputMatching(m_pOutputEast, true))
		{
			PrintCompileError(mDebugLineNo, "The WHILE node has bad interfaces of its program inside: east and west should match for a spatial iteration");
			return false;
		}
	}
	else if (IsTemporalIteration())
	{
		// The north and south interface should be the same
		if (!m_pOutputSouth->VerifyInputMatching(m_pInputNorth, true))
		{
			PrintCompileError(mDebugLineNo, "The WHILE node has bad interfaces of its program inside: north and south should match for a temporal iteration");
			return false;
		}
	}
	else if (IsDiagonalIteration())
	{
		// The north and south interface should be the same 
		// AND (east - west)
		if (!m_pInputWest->VerifyInputMatching(m_pOutputEast, true)) 
		{
			PrintCompileError(mDebugLineNo, "The WHILE node has bad interfaces of its program inside: east and west should match for a diagonal iteration");
			return false;
		}

		if (!m_pOutputSouth->VerifyInputMatching(m_pInputNorth, true))
		{
			PrintCompileError(mDebugLineNo, "The WHILE node has bad interfaces of its program inside: north and south should match for a diagonal iteration");
			return false;
		}
	}
	else
	{
		assert(false);
		PrintCompileError(mDebugLineNo, "Unknown WHILE/FOR type");
		return false;
	}

	return true;
}

bool ProgramWHILE::DoSpecificValidationOrSymbolInherit(SymbolTable* pParent)
{
	// Inherit the child symbol table - the WHILE program has the same interface as its child
	return ProgramBase::DoSymbolTableInheritInputBlocks(m_pInputNorth, m_pInputWest, true);
}

SymbolArrivedResult ProgramWHILE::OnSymbolArrived(char *szName)
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

	VerifyFirstIterationSpawn();

	return result;
}

bool ProgramWHILE::Validate(SymbolTable* pParent, bool bAtRuntimeSpawn, bool bTemplateMOdule)
{
	ProgramBase::DoBaseSymbolTableInheritance(pParent);

	// Do some stuff depending on the type WHILE/FOR 
	if (!DoSpecificValidationOrSymbolInherit(pParent))
	{
		return false;
	}

	// Verify if all symbols within expression are contained in the symbol table
	//-----------------------------------------------------------
	if (!m_pCondition->ValidateExpression(m_SymbolTable) || m_pCondition->GetDominantType() != E_DOM_BOOL)
	{
		PrintCompileError(mDebugLineNo, "Expression condition of WHILE/FOR couldn't be validated. It is possible to contain variables that are not accessible in that scope ?\n");
		return false;
	}

	// Cache the pointer to name if one exists

	m_pCondition->CollectInternalVariableNames(m_expectedInputMap);

	// Check if the condition variables exists in the symbol table and link to them
	for (VariablesNameSetIter it = m_expectedInputMap.begin(); it != m_expectedInputMap.end(); it++)
	{
		IDataTypeItem* pItem = m_SymbolTable->GetIdentifierFromTable((char*)it->c_str());
		if (pItem == NULL)
		{
			PrintCompileError(mDebugLineNo, "The variable named %s in the WHILE/FOR's condition was not declared", (char*)it->c_str());
			return false;
		}
		pItem->AddProgramListener(this);	
	}

	// Validate the child program
	bool bShouldInherit = ProgramBase::GetShouldGiveParentTableTo(m_pBaseProgram);
	if (!m_pBaseProgram->Validate(bShouldInherit ? pParent : NULL, bAtRuntimeSpawn, true))
		return false;

	m_iNrInputsNeededInNorth = m_pBaseProgram->m_pInputNorth->m_InputsInBlock.size();
	m_iNrInputsNeededInWest = m_pBaseProgram->m_pInputWest->m_InputsInBlock.size();

	if (m_iNrInputsNeededInNorth + m_iNrInputsNeededInWest == 0)
	{
		PrintCompileError(mDebugLineNo, "In a WHILE/FOR module, the inside program must have at least one input process in eighter north or west. If you are using a FOR without dependencies, use a FOREACH statement");
		return false;
	}

	// Test if the 
	if (m_pProgramParent)
	{

	}

	return true;
}

void ProgramWHILE::CreateAndLinkNewInternalChild()
{
	if (m_pIterationProgram)
		delete m_pIterationProgram;

	m_iIterationIdx++;

	// Clone the base
	m_pIterationProgram = m_pBaseProgram->Clone();
	m_pIterationProgram->SolveInputOutput();
	m_pIterationProgram->m_pProgramParent = this;

	if (IsTemporalIteration())
	{
		// Link the WHILE north to the north of iterated module, and south of the iterated module to the WHILE north (like in a loop)
		BaseProcessInput::SetOrientedLink(m_pInputNorth->m_InputsInBlock, m_pIterationProgram->m_pInputNorth->m_InputsInBlock);
		BaseProcessInput::SetOrientedLink(m_pIterationProgram->m_pOutputSouth->m_InputsInBlock, m_pInputNorth->m_InputsInBlock);

		// link the west and east sides if exists
		if (!m_pInputWest->IsNil())
		{
			VectorProcessItem* pWestArray = (VectorProcessItem*)m_pInputWest->m_InputsInBlock[0];
			ArrayOfBaseProcessInputs& from = pWestArray->GetVectorItemByIndex(m_iIterationIdx);
			BaseProcessInput::SetOrientedLink(from, m_pIterationProgram->m_pInputWest->m_InputsInBlock);
		}		

		if (!m_pOutputEast->IsNil())
		{
			VectorProcessItem* pEastArray = (VectorProcessItem*)m_pOutputEast->m_InputsInBlock[0];
			ArrayOfBaseProcessInputs& to = pEastArray->GetVectorItemByIndex(m_iIterationIdx);
			BaseProcessInput::SetOrientedLink(m_pIterationProgram->m_pOutputEast->m_InputsInBlock, to);
		}
	}
	else if (IsSpatialIteration())
	{
		// Link the WHILE west to the west of iterated module, and east of the iterated module to the WHILE west (like in a loop)
		BaseProcessInput::SetOrientedLink(m_pInputWest->m_InputsInBlock, m_pIterationProgram->m_pInputWest->m_InputsInBlock);
		BaseProcessInput::SetOrientedLink(m_pIterationProgram->m_pOutputEast->m_InputsInBlock, m_pInputWest->m_InputsInBlock);

		// link the north and south sides if exists
		if (!m_pInputNorth->IsNil())
		{
			VectorProcessItem* pNorthArray = (VectorProcessItem*)m_pInputNorth->m_InputsInBlock[0];
			ArrayOfBaseProcessInputs& from = pNorthArray->GetVectorItemByIndex(m_iIterationIdx);
			BaseProcessInput::SetOrientedLink(from, m_pIterationProgram->m_pInputNorth->m_InputsInBlock);
		}

		if (!m_pOutputSouth->IsNil())
		{
			VectorProcessItem* pSouthArray = (VectorProcessItem*)m_pOutputSouth->m_InputsInBlock[0];
			ArrayOfBaseProcessInputs& to = pSouthArray->GetVectorItemByIndex(m_iIterationIdx);
			BaseProcessInput::SetOrientedLink(m_pIterationProgram->m_pOutputSouth->m_InputsInBlock, to);
		}
	}
	else if (IsDiagonalIteration())
	{
		// Link the north of the WHILE to the north of the iterated and the South of the iterated to the north of the while
		BaseProcessInput::SetOrientedLink(m_pInputNorth->m_InputsInBlock, m_pIterationProgram->m_pInputNorth->m_InputsInBlock);
		BaseProcessInput::SetOrientedLink(m_pIterationProgram->m_pOutputSouth->m_InputsInBlock, m_pInputNorth->m_InputsInBlock);

		// Link the WHILE west to the west of iterated module, and east of the iterated module to the WHILE west (like in a loop)
		BaseProcessInput::SetOrientedLink(m_pInputWest->m_InputsInBlock, m_pIterationProgram->m_pInputWest->m_InputsInBlock);
		BaseProcessInput::SetOrientedLink(m_pIterationProgram->m_pOutputEast->m_InputsInBlock, m_pInputWest->m_InputsInBlock);
	}
	else
	{
		assert(false && "Incorrect while type");
	}
}

bool ProgramWHILE::OnInputReceived(BaseProcessInput* pOriginalInput, BaseProcessInput* pInputNodeReceiver, EInputDirection eDir)
{
	pOriginalInput->m_eLastIterativeProgramVisited = GetType();	// E NODE_TYPE_WHILE here, but on for it will be _FOR

	if (IsWhileModuleFinishing())
		return true;

	// in some cases an array of processes might be sent together !
	// 

	// TODO: Extend here please to support array or even queued input ??? 
	pInputNodeReceiver->SetValues(pOriginalInput);

	// If it's the first iteration test if we should create another child program inside
	if (m_iIterationIdx == -1)
	{
		// Work out with the variables needed in the condition
		bool bIsReadyToEvaluateCondition = false;	// Tells us if all the variables in the conditions are available
		if (pInputNodeReceiver->GetType() == E_INPUT_SIMPLE)
		{
			SimpleProcessItem* pSPI = static_cast<SimpleProcessItem*>(pInputNodeReceiver);
			for (ListOfInputItemsConstIter it = pSPI->m_InputItems.begin(); it != pSPI->m_InputItems.end(); it++)
			{
				OnSymbolArrived((char*)(*it)->GetName());
			}
		}
		////

		if (eDir == E_INPUT_DIR_NORTH)
			m_iNrInputsInBufferInNorth++;
		else
			m_iNrInputsInBufferInWest++;

		VerifyFirstIterationSpawn();
	}

	return false;
}

void ProgramWHILE::VerifyFirstIterationSpawn()
{
	// Do we meet the required number of inputs and condition have all variables available?
	if (m_ExpectedInputsMap.size() == 0 && m_iNrInputsInBufferInNorth == m_iNrInputsNeededInNorth && m_iNrInputsInBufferInWest == m_iNrInputsNeededInWest)
	{
		// Because it is the first child, maybe we must some initializations
		OnBeforeFirstTimeConditionEval();

		// Check the condition and spawn the first child
		CheckIfShouldSpawnNewChild();
	}
}

void ProgramWHILE::OnChildProgramFinished(ProgramBase* pChildFinished)
{
	OnAfterChildProgramFinished();
	CheckIfShouldSpawnNewChild();
}

void ProgramWHILE::SendBufferedInputs(bool bIsWhileEnding)
{
	// Send the accumulated output on the iterated side 
	if (bIsWhileEnding)
	{
		// In temporal iteration, send the array of processes accumulated in the east side
		InputBlock* pOutputBlockToSend = nullptr;
		if (IsTemporalIteration())
		{
			pOutputBlockToSend = m_pOutputEast;
		}
		else if (IsSpatialIteration()) // In spatial, send the array accumulated in the south side
		{
			pOutputBlockToSend = m_pOutputSouth;
		}


		if (pOutputBlockToSend)
		{
			for (int j = 0; j < pOutputBlockToSend->m_InputsInBlock.size(); j++)
			{
				BaseProcessInput* pBPI = pOutputBlockToSend->m_InputsInBlock[j];
				if (pBPI->m_pNext)
				{
					pBPI->m_pNext->GoDownValue(pBPI);
				}
			}
		}
	}

	// Send the accumulated output in the input blocks
	{
		InputBlock* pInputBlocks[2] = { m_pInputNorth, m_pInputWest };
		bool sendMask[2] = { false, false };
		if (IsDiagonalIteration())
		{
			sendMask[0] = sendMask[1] = true;
		}
		else if (IsTemporalIteration())
		{
			sendMask[0] = true; // North and south are connected
		}
		else if (IsSpatialIteration())
		{
			sendMask[1] = true; // West and east are connected
		}

		int sizes[2] = { (int)m_pInputNorth->m_InputsInBlock.size(), (int)m_pInputWest->m_InputsInBlock.size() };

		for (int i = 0; i < 2; i++)
		{
			int sz = pInputBlocks[i]->m_InputsInBlock.size();
			if (sz == 0 || sendMask[i] == false) 
				continue;

			for (int j = 0; j < sz - 1; j++)
			{
				BaseProcessInput* pBPI = pInputBlocks[i]->m_InputsInBlock[j];
				pBPI->m_pNext->GoDownValue(pBPI);
			}

			// Send the last input in this direction and mark the WHILE program as sending the last input
			m_bIsLastInputSent = true;
			BaseProcessInput* pBPI = pInputBlocks[i]->m_InputsInBlock[sz - 1];
			pBPI->m_pNext->GoDownValue(pBPI);
			m_bIsLastInputSent = false;
		}
	}
	
}

void ProgramWHILE::CheckIfShouldSpawnNewChild()
{
	// 1. Test the condition
	//	  if  the condition is true
	//		Delete the actual internal module
	//		Create another internal module and make the right linkage
	//		Send buffered input to this module
	//	  else
	//		Send buffered input to WHILE output

	bool bCondValue = m_pCondition->GetValueAsBool();
	if (bCondValue)
	{
		CreateAndLinkNewInternalChild();
		SendBufferedInputs(m_bIsWhileModuleFinishing);
	}
	else
	{
		if (IsTemporalIteration()) 		// Link from north to south, west to east, then send the buffered input
		{
			BaseProcessInput::SetOrientedLink(m_pInputNorth->m_InputsInBlock, m_pOutputSouth->m_InputsInBlock);
		}
		else if (IsSpatialIteration())  // Link from west to east, west to east, then send the buffered input
		{
			BaseProcessInput::SetOrientedLink(m_pInputWest->m_InputsInBlock, m_pOutputEast->m_InputsInBlock);
		}
		else 
		{
			BaseProcessInput::SetOrientedLink(m_pInputNorth->m_InputsInBlock, m_pOutputSouth->m_InputsInBlock);
			BaseProcessInput::SetOrientedLink(m_pInputWest->m_InputsInBlock, m_pOutputEast->m_InputsInBlock);
		}

		// Mark the module as finished and send the latest buffered input
		m_bIsWhileModuleFinishing = true;
		SendBufferedInputs(m_bIsWhileModuleFinishing);

		SendProgramFinished(this);
	}
}

