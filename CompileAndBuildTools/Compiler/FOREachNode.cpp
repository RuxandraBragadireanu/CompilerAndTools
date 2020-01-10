#include "FOREachNode.h"
#include "ReferenceNode.h"
#include "Expressions.h"

#include "calc3_utils.h"

ProgramBase* ProgramFOREACH::Clone()
{
	ProgramFOREACH* pClonedFOR = new ProgramFOREACH(mDebugLineNo);

	// If the base program is a reference, find the ref at it recursively and solve it before cloning !
	// We can't clone a reference module 
	if (m_pBaseChildProgram->GetType() == E_NODE_TYPE_MODULE_REF)
	{
		ProgramReference* pPRef = (ProgramReference*)m_pBaseChildProgram;
		pClonedFOR->m_pBaseChildProgram = ProgramReference::FindAndCloneRef(pPRef);
	}
	else
		pClonedFOR->m_pBaseChildProgram = m_pBaseChildProgram->Clone();

	pClonedFOR->m_eType = m_eType;
	pClonedFOR->m_iNumIterationChilds = 0;

	// Inherit the symbol table and clone the value to go expression
	pClonedFOR->DoBaseSymbolTableInheritance(this->m_SymbolTable);
	pClonedFOR->m_pValueToGo = m_pValueToGo->Clone();
	pClonedFOR->m_pValueToGo->ValidateExpression(pClonedFOR->m_SymbolTable);

	
	// This will help input output solver to understand that it needs to expand children too after cloning process !
	pClonedFOR->m_bShouldExpand = this->m_bExpanded; 
	// Solving the the input output blocks will be done from exterior not from clone: CHECK the the WHILE NODE for example how it calls SolveInputOutput for children hierarchly 
	// DO NOT CALL THE FOLLOWING LINE FROM CLONE IT IS TOTALLY INTERDICTED ACCORDING TO DESIGN! CLONE AND SOLVING ARE TWO SEPARATE - ITERATIVE STEPS !
	//pClonedFOR->SolveInputOutput();
	
	// Check if the number of expanded children is the same
	//assert(pClonedFOR->m_iNumIterationChilds == this->m_iNumIterationChilds && "The number of iterated children count is different");

	// Check if the buffering items is the same. WARNING: THIS IS EXPECTED BECAUSE WE DON'T clone them
	// We only want to be sure that the target clone doesn't have anything buffered!
	{
		for (int i = 0; i < E_NUM_DIRECTION; i++)
		{
			assert(pClonedFOR->m_BufferedInputs[i].size() == this->m_BufferedInputs[i].size() && "problem with buffering. see comment above in code");
		}
		assert(pClonedFOR->m_FullArrayBuffered == this->m_FullArrayBuffered && "problem with buffering. see comment above in code");
	}	

	return pClonedFOR;
}

bool ProgramFOREACH::Validate(SymbolTable *pParent, bool bAtRuntimeSpawn)
{
	ProgramBase::DoBaseSymbolTableInheritance(pParent);

	// Test the goto expression
	EGeneralExpressionType type = m_pValueToGo->GetGeneralExpressionType();
	bool bGoToExpressionIsOk = false;
	if (type == E_EXPR_CONSTANT /*|| type == E_EXPR_VALUE*/)
		bGoToExpressionIsOk = true;
	else if ((type == E_EXPR_VARIABLE || type == E_EXPR_VALUE) && m_pValueToGo->GetDominantType() != E_DOM_ARRAY)
	{
		bGoToExpressionIsOk = true;

		if (!m_pValueToGo->ValidateExpression(m_SymbolTable))
			return false;

		// Cache the pointer to name if one exists
		m_pValueToGo->CollectInternalVariableNames(m_SetOfRequiredVariables);

		// Check if the condition variables exists in the symbol table and link to them
		for (VariablesNameSetIter it = m_SetOfRequiredVariables.begin(); it != m_SetOfRequiredVariables.end(); it++)
		{
			IDataTypeItem* pItem = pParent->GetIdentifierFromTable((char*)it->c_str());
			if (pItem == NULL)
			{
				PrintCompileError(mDebugLineNo, "The variable %s in the FOR condition was not found", (char*)it->c_str());
				return false;
			}
			pItem->AddProgramListener(this);	
		}
	}
	else
	{
		PrintCompileError(mDebugLineNo, "You must provide a CONSTANT, VARIABLE, OR VALUE EXPRESSION (not boolean) to a FOR program!\n");
		return false;
	}

	if (!bGoToExpressionIsOk)
	{
		PrintCompileError(mDebugLineNo, "The parameter given to for must be a constant or variable (not array!)");
		return false;
	}

	/*
	// Test if the child has a single process item as input - required by the current version of AGAPIA compiler
	InputBlock* pChildBlocks[4] = {m_pBaseChildProgram->m_pInputNorth, m_pBaseChildProgram->m_pInputWest, m_pBaseChildProgram->m_pOutputSouth, m_pBaseChildProgram->m_pOutputEast };
	for (int i = 0; i < 4; i++)
	{
		int iChildsCount = pChildBlocks[i]->GetProcessesCount();
		if (iChildsCount > 1)
		{
			PrintCompileError("file", "line", "For this version of AGAPIA, the base child process of a FOR must have at most a single process input/output item");
			return false;
		}
	}
	*/
	
	if (!m_pBaseChildProgram->Validate(pParent, bAtRuntimeSpawn))
		return false;

	return true;
}

bool ProgramFOREACH::SolveReferences()
{
	if (m_pBaseChildProgram)
	{
		if (m_pBaseChildProgram->GetType() == E_NODE_TYPE_MODULE_REF)
		{
			m_pBaseChildProgram = ProgramReference::FindAndCloneRef((ProgramReference*)m_pBaseChildProgram);
			if (m_pBaseChildProgram == NULL) return false;
		}
		else
			return m_pBaseChildProgram->SolveReferences();
	}

	return true;
}

bool ProgramFOREACH::SolveInputOutput()
{
	m_pBaseChildProgram->SolveInputOutput();

	//--------------------------------------
	InputBlock* pBaseChildTypes[4] = {	m_pBaseChildProgram->m_pInputNorth, m_pBaseChildProgram->m_pInputWest, 
										m_pBaseChildProgram->m_pOutputSouth,  m_pBaseChildProgram->m_pOutputEast };

	InputBlock** pTargets[4] = { &m_pInputNorth, &m_pInputWest, &m_pOutputSouth , &m_pOutputEast };

	switch(m_eType)
	{
	case E_FOREACH_S:
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
		break;
	case E_FOREACH_T:
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
					(*pTargets[i])->AddInputs(pBaseChildTypes[i]);
				else // West or East?
				{
					VectorProcessItem* pVPI = new VectorProcessItem(*pTargets[i]);
					pVPI->SetItemType(pBaseChildTypes[i]->m_InputsInBlock, "");
					(*pTargets[i])->AddInput(pVPI);
				}		
			}
		}
		break;
	case E_FOREACH_ST:
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
		break;
	default:
		assert(false && "Not defined well the type of FOR! ");
		return false;
	}

	// -------------------------------------
	m_pInputNorth->SetAsNorthBlock();

	// Type checking of the internal child module on specific for type
	switch(m_eType)
	{
	case E_FOREACH_S:
		{
			// The east and west interface should be the same
			if (!m_pInputWest->VerifyInputMatching(m_pOutputEast, true)) 
				return false;
		}
		break;
	case E_FOREACH_T:
		{
			// The north and south interface should be the same
			if (!m_pOutputSouth->VerifyInputMatching(m_pInputNorth, true)) 
				return false;
		}
		break;
	case E_FOREACH_ST:
		{
			// The north and south interface should be the same 
			// AND (east - west)
			if (!m_pInputWest->VerifyInputMatching(m_pOutputEast, true)) 
				return false;

			if (!m_pOutputSouth->VerifyInputMatching(m_pInputNorth, true)) 
				return false;
		}
		break;

	default:
		assert(false);
		PrintCompileError(mDebugLineNo, "Unknown FOR type");
		return false;
	}


	// If it should expand, expand the base children too
	if (m_bShouldExpand)
	{
		Expand();
		m_bShouldExpand = false;
	}
	
	return true;
}

void ProgramFOREACH::ChildsCreationLinkage()
{
	// If already expanded don't do anything
	m_bExpanded = true;

	// Expand the child iterations
	m_iNumIterationChilds = m_pValueToGo->GetValueAsInt();
	if (m_iNumIterationChilds < 0 || m_iNumIterationChilds > 1000000)
	{
		assert(false);
		PrintExecutionError("In the current implementation of interpreter you should have between >=0 and <= 10000000 iterations in a FOR module");
		return;
	}

	m_pIterationChilds = new ProgramBase*[m_iNumIterationChilds];
	for (unsigned int i = 0; i < m_iNumIterationChilds; i++)
	{
		// Clone the child
		m_pIterationChilds[i] = m_pBaseChildProgram->Clone();

		// Validate it - to inherit the symbol table and compute number of inputs to be received
		m_pIterationChilds[i]->Validate(m_SymbolTable, true);

		m_pIterationChilds[i]->m_pProgramParent = this;
	}

	// Crate the linkage between expanded modules
	switch(m_eType)
	{
	case E_FOREACH_S:
		{
			// North linkage
			if (!m_pInputNorth->IsNil())
			{
				VectorProcessItem* pVPI = static_cast<VectorProcessItem*>(m_pInputNorth->m_InputsInBlock[0]);
				pVPI->ClearAll();
				for (unsigned int i = 0; i < m_iNumIterationChilds; i++)
				{
					ArrayOfBaseProcessInputs& arrayOfProcesses = pVPI->GetVectorItemByIndex(i);
					BaseProcessInput::SetOrientedLink(arrayOfProcesses, m_pIterationChilds[i]->m_pInputNorth->m_InputsInBlock);
				}
			}
			// South linkage
			if (!m_pOutputSouth->IsNil())
			{
				VectorProcessItem* pVPI = static_cast<VectorProcessItem*>(m_pOutputSouth->m_InputsInBlock[0]);

				// Add to the south output block the iteration child south outputs
				for (unsigned int i = 0; i < m_iNumIterationChilds; i++)
				{
					ArrayOfBaseProcessInputs& arrayOfProcesses = pVPI->GetVectorItemByIndex(i);
					BaseProcessInput::SetOrientedLink(m_pIterationChilds[i]->m_pOutputSouth->m_InputsInBlock, arrayOfProcesses);
				}

				// TODO: need to rethink this ? -> same as FOR_S !!
				// Need to send this all down the tree, until the next component is a full vector link
				// This helps us to correctly consider the remaining number of inputs for the below atomic modules that have a chain linkage to this FOR node
				BaseProcessInput *pIterFullAray = static_cast<BaseProcessInput*>(pVPI);
				while(pIterFullAray && pIterFullAray->GetType() == E_INPUT_ARRAY)
				{
					int numProcessesInSouthInterface = (m_iNumIterationChilds > 0 ? m_pIterationChilds[0]->m_pOutputSouth->m_InputsInBlock.size() : 0);
					((VectorProcessItem*)pIterFullAray)->m_iNumberOfInputsToBeReceived = m_iNumIterationChilds * numProcessesInSouthInterface;
					pIterFullAray = pIterFullAray->m_pNext;
				}				
			}
			// West linkage
			if (!m_pInputWest->IsNil())
			{
				BaseProcessInput::SetOrientedLink(m_pInputWest->m_InputsInBlock, m_pIterationChilds[0]->m_pInputWest->m_InputsInBlock);
			}
			// East linkage
			if (!m_pOutputEast->IsNil())
			{
				BaseProcessInput::SetOrientedLink(m_pIterationChilds[m_iNumIterationChilds - 1]->m_pOutputEast->m_InputsInBlock, m_pOutputEast->m_InputsInBlock);
			}

			// Build between iteration modules links
			// In this case, link the east output to the west input of the next module
			if (!m_pInputWest->IsNil() && !m_pOutputEast->IsNil())
			{
				for (unsigned int i = 0; i < m_iNumIterationChilds - 1; i++)
				{
					ArrayOfBaseProcessInputs& pThisNode_EAST = m_pIterationChilds[i]->m_pOutputEast->m_InputsInBlock;
					ArrayOfBaseProcessInputs& pNextNode_WEST = m_pIterationChilds[i + 1]->m_pInputWest->m_InputsInBlock;
					BaseProcessInput::SetOrientedLink(pThisNode_EAST, pNextNode_WEST);
				}
			}
		}
		break;
	case E_FOREACH_T:
		{
			// North linkage
			if (!m_pInputNorth->IsNil())
			{
				BaseProcessInput::SetOrientedLink(m_pInputNorth->m_InputsInBlock, m_pIterationChilds[0]->m_pInputNorth->m_InputsInBlock);
			}

			// South linkage
			if (!m_pOutputSouth->IsNil())
			{
				BaseProcessInput::SetOrientedLink(m_pIterationChilds[m_iNumIterationChilds - 1]->m_pOutputSouth->m_InputsInBlock, m_pOutputSouth->m_InputsInBlock);
			}

			// West linkage
			if (!m_pInputNorth->IsNil())
			{
				// Add to the west output block the iteration childs west outputs
				VectorProcessItem* pVPI = static_cast<VectorProcessItem*>(m_pInputWest->m_InputsInBlock[0]);
				for (unsigned int i = 0; i < m_iNumIterationChilds; i++)
				{
					ArrayOfBaseProcessInputs& arrayOfProcesses = pVPI->GetVectorItemByIndex(i);
					BaseProcessInput::SetOrientedLink(arrayOfProcesses, m_pIterationChilds[i]->m_pInputWest->m_InputsInBlock);
				}
			}

			// East linkage
			if (!m_pOutputEast->IsNil())
			{
				VectorProcessItem* pVPI = static_cast<VectorProcessItem*>(m_pOutputEast->m_InputsInBlock[0]);
				for (unsigned int i = 0; i < m_iNumIterationChilds; i++)
				{
					ArrayOfBaseProcessInputs& arrayOfProcesses = pVPI->GetVectorItemByIndex(i);
					BaseProcessInput::SetOrientedLink(m_pIterationChilds[i]->m_pOutputEast->m_InputsInBlock, arrayOfProcesses);
				}

				//////////////////
				// TODO: need to rethink this -> same as FOR_T !!
				// Need to send this all down the tree, until the next component is a full vector link
				// This helps us to correctly consider the remaining number of inputs for the below atomic modules that have a chain linkage to this FOR node
				int numProcessesInWestInterface = (m_iNumIterationChilds > 0 ? m_pIterationChilds[0]->m_pOutputEast->m_InputsInBlock.size() : 0);
				int iNrIndicesRemained = m_iNumIterationChilds;
				
				BaseProcessInput *pIterFullAray = static_cast<BaseProcessInput*>(pVPI);
				while(pIterFullAray && pIterFullAray->GetType() == E_INPUT_ARRAY)
				{
					VectorProcessItem* currVector = (VectorProcessItem*)pIterFullAray;
					iNrIndicesRemained -= currVector->m_iFromIndexRule;
					currVector->m_iNumberOfInputsToBeReceived = iNrIndicesRemained * numProcessesInWestInterface;
					pIterFullAray = pIterFullAray->m_pNext;
				}
				//////////////////
			}

			// Build between iteration modules links
			// In this case, link the east output to the west input of the next module
			if (!m_pOutputSouth->IsNil() && !m_pInputNorth->IsNil())
			{
				for (unsigned int i = 0; i < m_iNumIterationChilds - 1; i++)
				{
					ArrayOfBaseProcessInputs& pThisNode_SOUTH = m_pIterationChilds[i]->m_pOutputSouth->m_InputsInBlock;
					ArrayOfBaseProcessInputs& pNextNode_NORTH = m_pIterationChilds[i + 1]->m_pInputNorth->m_InputsInBlock;
					BaseProcessInput::SetOrientedLink(pThisNode_SOUTH, pNextNode_NORTH);
				}
			}
		}
		break;
	case E_FOREACH_ST:
		{
			bool bHasNorthAndSouthInterface = (!m_pInputNorth->IsNil() && !m_pOutputSouth->IsNil());
			bool bHasEastAndWestInterface = (!m_pInputWest->IsNil() && !m_pOutputEast->IsNil());

			// North interface (link to first node north)
			if (bHasNorthAndSouthInterface)
			{
				BaseProcessInput::SetOrientedLink(m_pInputNorth->m_InputsInBlock, m_pIterationChilds[0]->m_pInputNorth->m_InputsInBlock);
			}
			// West interface (link to first node west)
			if (bHasEastAndWestInterface)
			{
				BaseProcessInput::SetOrientedLink(m_pInputWest->m_InputsInBlock, m_pIterationChilds[0]->m_pInputWest->m_InputsInBlock);
			}
			// South interface (link from last node south)
			if (bHasNorthAndSouthInterface)
			{
				BaseProcessInput::SetOrientedLink(m_pIterationChilds[m_iNumIterationChilds - 1]->m_pOutputSouth->m_InputsInBlock, m_pOutputSouth->m_InputsInBlock);
			}
			// East interface (link from last node east)
			if (bHasEastAndWestInterface)
			{
				BaseProcessInput::SetOrientedLink(m_pIterationChilds[m_iNumIterationChilds - 1]->m_pOutputEast->m_InputsInBlock, m_pOutputEast->m_InputsInBlock);
			}

			// Links between the iteration childs
			for (unsigned int i = 0; i < m_iNumIterationChilds - 1; i++)
			{
				if (bHasNorthAndSouthInterface)
				{
					ArrayOfBaseProcessInputs& pThisNode_SOUTH = m_pIterationChilds[i]->m_pOutputSouth->m_InputsInBlock;
					ArrayOfBaseProcessInputs& pNextNode_NORTH = m_pIterationChilds[i + 1]->m_pInputNorth->m_InputsInBlock;
					BaseProcessInput::SetOrientedLink(pThisNode_SOUTH, pNextNode_NORTH);
				}

				if (bHasEastAndWestInterface)
				{
					ArrayOfBaseProcessInputs& pThisNode_EAST = m_pIterationChilds[i]->m_pOutputEast->m_InputsInBlock;
					ArrayOfBaseProcessInputs& pNextNode_WEST = m_pIterationChilds[i + 1]->m_pInputWest->m_InputsInBlock;
					BaseProcessInput::SetOrientedLink(pThisNode_EAST, pNextNode_WEST);
				}
			}
		}
		break;
	default:
		assert(false && "Not defined well the type of FOR! ");
		return;
	}
}

bool ProgramFOREACH::OnInputReceived(BaseProcessInput* pOriginalInput, BaseProcessInput* pInputNodeReceiver, EInputDirection eDir)
{
	pOriginalInput->m_eLastIterativeProgramVisited = E_NODE_TYPE_FOREACH;

	// We must see when we receive the input that made the expression
	if (m_bExpanded /*||  pOriginalInput->GetType() == EInputTypes::E_INPUT_ARRAY*/)
		return true;

	// If we didn't expanded it, then we should buffer the input
	assert(eDir >=0 && eDir < E_NUM_DIRECTION);

	AddBufferedProcessInput(eDir, pOriginalInput, pInputNodeReceiver->m_iParentVectorIndex, pInputNodeReceiver->m_iIndexInBlock);

	return false;
}

void ProgramFOREACH::SendBufferedInput()
{
	switch(m_eType)
	{
	case E_FOREACH_S:
		{
			// Send the north input
			//---------------------
				// Send full array?
			if (m_FullArrayBuffered)
			{
				m_pInputNorth->m_InputsInBlock[0]->GoDownValue(m_FullArrayBuffered);
			}
			// Send split ?
			else
			{
				VectorProcessItem* pVPI = static_cast<VectorProcessItem*>(m_pInputNorth->m_InputsInBlock[0]);
				for (ForBufferOfProcessInputsIter it = m_BufferedInputs[E_INPUT_DIR_NORTH].begin(); it != m_BufferedInputs[E_INPUT_DIR_NORTH].end(); it++)
				{
					const ForBufferItemType& item = *it;
					ArrayOfBaseProcessInputs& destination = pVPI->GetVectorItemByIndex(item.iVectorIndex);
					destination[item.iItemIndex]->GoDownValue(item.pInput);
				}
			}
			
			// Send the west input
			//---------------------------------
			ArrayOfBaseProcessInputs& destination = m_pInputWest->m_InputsInBlock;
			for (ForBufferOfProcessInputsIter it = m_BufferedInputs[E_INPUT_DIR_WEST].begin(); it != m_BufferedInputs[E_INPUT_DIR_WEST].end(); it++)
			{
				const ForBufferItemType& item = *it;
				destination[item.iItemIndex]->GoDownValue(item.pInput);
			}
		} 
		break;
	case E_FOREACH_T:
		{
			// Send the west input
			//---------------------
			// Send full array?
			if (m_FullArrayBuffered)
			{
				m_pInputWest->m_InputsInBlock[0]->GoDownValue(m_FullArrayBuffered);
			}
			// Send split ?
			else
			{
				VectorProcessItem* pVPI = static_cast<VectorProcessItem*>(m_pInputWest->m_InputsInBlock[0]);
				for (ForBufferOfProcessInputsIter it = m_BufferedInputs[E_INPUT_DIR_WEST].begin(); it != m_BufferedInputs[E_INPUT_DIR_WEST].end(); it++)
				{
					const ForBufferItemType& item = *it;
					ArrayOfBaseProcessInputs& destination = pVPI->GetVectorItemByIndex(item.iVectorIndex);
					destination[item.iItemIndex]->GoDownValue(item.pInput);
				}
			}

			// Send the north input
			//---------------------------------
			ArrayOfBaseProcessInputs& destination = m_pInputNorth->m_InputsInBlock;
			for (ForBufferOfProcessInputsIter it = m_BufferedInputs[E_INPUT_DIR_NORTH].begin(); it != m_BufferedInputs[E_INPUT_DIR_NORTH].end(); it++)
			{
				const ForBufferItemType& item = *it;
				destination[item.iItemIndex]->GoDownValue(item.pInput);
			}
		}
		break;
	case E_FOREACH_ST:
		{
			// Send the west and north input
			//---------------------------------
			ArrayOfBaseProcessInputs* destinations[2] = {&m_pInputWest->m_InputsInBlock, &m_pInputNorth->m_InputsInBlock};
			EInputDirection directions[2] = {E_INPUT_DIR_WEST, E_INPUT_DIR_NORTH };

			for (int i = 0; i < 2; i++)
			{
				ArrayOfBaseProcessInputs& destination = *destinations[i];
				for (ForBufferOfProcessInputsIter it = m_BufferedInputs[directions[i]].begin(); it != m_BufferedInputs[directions[i]].end(); it++)
				{
					const ForBufferItemType& item = *it;
					destination[item.iItemIndex]->GoDownValue(item.pInput);
				}
			}
		}
		break;
	default:
		{
			assert(false && "unknown for type");
			PrintCompileError(mDebugLineNo, "Unknown for type");
			return;
		}
	}
}

void ProgramFOREACH::OnInputItemReady(char *szName)
{
	std::string strVarName(szName);
	
	VariablesNameSetIter it = m_SetOfRequiredVariables.find(strVarName);
	if (it != m_SetOfRequiredVariables.end())
		m_SetOfRequiredVariables.erase(it);

	// If the module hasn't been previously expanded and we don't expect any new input variables for condition, expand the module
	if (!m_bExpanded && m_SetOfRequiredVariables.empty())
	{
		Expand();
	}
}

void ProgramFOREACH::Expand()
{
	assert(m_bExpanded == false);
	ChildsCreationLinkage();
	SendBufferedInput();
}

void ProgramFOREACH::AddBufferedProcessInput(EInputDirection eDir, BaseProcessInput* pOriginalInput, int iParentVectorIndex, int iIndexInItem)
{
	bool bIsFullArray = false;
	if (eDir == E_INPUT_DIR_NORTH && m_eType == E_FOREACH_S)
	{
		bIsFullArray = (pOriginalInput->GetType() == E_INPUT_ARRAY && CheckMatchingArrayOfBaseProcessInputs( ((VectorProcessItem*)pOriginalInput)->m_TypeOfArrayItems, m_pBaseChildProgram->m_pInputNorth->m_InputsInBlock, false));	
	}
	else if (eDir == E_INPUT_DIR_WEST && m_eType == E_FOREACH_T)
	{
		bIsFullArray = bIsFullArray = (pOriginalInput->GetType() == E_INPUT_ARRAY && CheckMatchingArrayOfBaseProcessInputs( ((VectorProcessItem*)pOriginalInput)->m_TypeOfArrayItems, m_pBaseChildProgram->m_pInputWest->m_InputsInBlock, false));	
	}

	if (bIsFullArray)
	{
		m_FullArrayBuffered = pOriginalInput;
	}
	else
	{
		m_BufferedInputs[eDir].push_back(ForBufferItemType(pOriginalInput, iParentVectorIndex, iIndexInItem));
	}
}

void ProgramFOREACH::OnChildProgramFinished(ProgramBase* pChildFinished)
{	
	if (!CheckFinishedCondition())
		return;

	m_iNrFinishedInternalPrograms++;
	if (m_iNrFinishedInternalPrograms == m_iNumIterationChilds)
	{
		SendProgramFinished(this);
	}
}