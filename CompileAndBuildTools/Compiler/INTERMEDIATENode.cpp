#include "INTERMEDIATENode.h"
#include "ReferenceNode.h"
#include "CModuleCode.h"
#include "calc3_utils.h"
#include "CompilationBlackbox.h"
#include "ExecutionBlackbox.h"

void ProgramIntermediateModule::SetOperationAndChilds(ECompositionOperator eOperator, ProgramBase* pChild1, ProgramBase* pChild2)
{
	assert(m_pChilds[0] == NULL);

	m_operator = eOperator;
	m_pChilds[0] = pChild1;
	m_pChilds[1] = pChild2;

	if (m_operator == E_COMP_UNDEFINED)
	{
		assert(m_pChilds[1] == NULL);
		m_iNrChilds = 1;
	}
	else
	{
		assert(m_pChilds[1]);
		m_iNrChilds = 2;
	}

	for (int i = 0; i < m_iNrChilds; i++)
	{
		if (m_pChilds[i])
		{
			m_pChilds[i]->m_pProgramParent = this;
			m_bChildFinished[i] = false;
		}
		else
		{
			m_bChildFinished[i] = true;
		}
	}
}

ProgramBase* ProgramIntermediateModule::Clone()
{
	ProgramIntermediateModule* pCloned = new ProgramIntermediateModule(mDebugLineNo);
	CopyBase(pCloned);

	for (int i = 0; i < 2; i++)
	{
		if (m_pChilds[i]) 
		{
			if (m_pChilds[i]->GetType() == E_NODE_TYPE_MODULE_REF)
				pCloned->m_pChilds[i] = ProgramReference::FindAndCloneRef((ProgramReference*)m_pChilds[i]);
			else
				pCloned->m_pChilds[i] = m_pChilds[i]->Clone();

			m_bChildFinished[i] = false;
			pCloned->m_pChilds[i]->m_pProgramParent = pCloned;
		}
		else 
		{
			pCloned->m_pChilds[i] = NULL;
			m_bChildFinished[i] = true;
		}
	}

	pCloned->m_bIsUserDefined = m_bIsUserDefined;
	pCloned->m_iNrChilds = m_iNrChilds;
	pCloned->m_operator = m_operator;
	pCloned->m_bIsAtomic = m_bIsAtomic;

	if (m_pCCodeObject)
		pCloned->m_pCCodeObject = m_pCCodeObject;

	pCloned->m_szModuleName = (m_szModuleName ? _strdup(m_szModuleName) : NULL);
	m_bIsAllInputReceived = false; //???

	pCloned->ComputeNumInputsToReceive();

	return pCloned;
}

bool ProgramIntermediateModule::SolveReferences()
{
	for (int i = 0; i < 2; i++)
	{
		if (m_pChilds[i])
		{
			if (m_pChilds[i]->GetType() == E_NODE_TYPE_MODULE_REF)
			{
				ProgramBase* newChild = ProgramReference::FindAndCloneRef((ProgramReference*)m_pChilds[i]);
				delete m_pChilds[i];
				m_pChilds[i] = newChild;

				if (m_pChilds[i] == NULL) return false;
				m_pChilds[i]->m_pProgramParent = this;
			}
			else
				if (!m_pChilds[i]->SolveReferences())
					return false;
		}
	}

	return true;
}

bool ProgramIntermediateModule::SolveInputOutput()
{
	if (m_bIsUserDefined)
	{
		if (CompilationBlackbox::Get()->IsModuleInSet(m_szModuleName))
			return true;

		CompilationBlackbox::Get()->InsertModuleName(m_szModuleName);

		m_pInputNorth->SetAsNorthBlock();

		// On atomic modules, we should have full-link vectors (0:0 preferable)
		if (m_bIsAtomic)
		{
			InputBlock* pAllBlocks[4] = {m_pInputNorth, m_pInputWest, m_pOutputSouth, m_pOutputEast};
			for (int i = 0; i < 4; i++)
			{
				for (ArrayOfBaseProcessInputsIter it = pAllBlocks[i]->m_InputsInBlock.begin(); it != pAllBlocks[i]->m_InputsInBlock.end(); it++)
					if ((*it)->GetType() == E_INPUT_ARRAY && (*it)->IsNotVectorToVector()) 
					{
						PrintExecutionError("For this version of AGAPIA all atomic modules must receive/send in their interfaces only full-link vectors\n");
						return false;
					}
			}
		}

		// If it's an C module code we shouldn't link internally nothing
		if (m_bIsAtomic)
			return true;

		// Solve child
		for (int i = 0; i < m_iNrChilds; i++)
		{
			bool bRes = m_pChilds[i]->SolveInputOutput();
			if (!bRes)
				return false;
		}

		// Check if inputs and outputs are compatible, and assign links here
		switch(m_operator)
		{
		case E_COMP_UNDEFINED:
			if (!m_pInputNorth->CheckAndLinkInputs(m_pChilds[0]->m_pInputNorth, NULL, true)) return false;
			if (!m_pInputWest->CheckAndLinkInputs(m_pChilds[0]->m_pInputWest, NULL, true)) return false;
			if (!m_pOutputSouth->CheckAndLinkInputs(m_pChilds[0]->m_pOutputSouth, NULL, false)) return false;
			if (!m_pOutputEast->CheckAndLinkInputs(m_pChilds[0]->m_pOutputEast, NULL, false)) return false;
			break;
		case E_COMP_HORIZONTAL:
			// Borders solve
			if (!m_pInputNorth->CheckAndLinkInputs(m_pChilds[0]->m_pInputNorth, m_pChilds[1]->m_pInputNorth, true)) return false;
			if (!m_pInputWest->CheckAndLinkInputs(m_pChilds[0]->m_pInputWest, NULL, true)) return false;
			if (!m_pOutputSouth->CheckAndLinkInputs(m_pChilds[0]->m_pOutputSouth, m_pChilds[1]->m_pOutputSouth, false)) return false;
			if (!m_pOutputEast->CheckAndLinkInputs(m_pChilds[1]->m_pOutputEast, NULL, false)) return false;

			// Internal solve
			if (!m_pChilds[0]->m_pOutputEast->CheckAndLinkInputs(m_pChilds[1]->m_pInputWest, NULL, true)) return false;
			break;
		case E_COMP_VERTICAL:
			if (!m_pInputNorth->CheckAndLinkInputs(m_pChilds[0]->m_pInputNorth, NULL, true)) return false;
			if (!m_pInputWest->CheckAndLinkInputs(m_pChilds[0]->m_pInputWest, m_pChilds[1]->m_pInputWest, true)) return false;
			if (!m_pOutputSouth->CheckAndLinkInputs(m_pChilds[1]->m_pOutputSouth, NULL, false)) return false;
			if (!m_pOutputEast->CheckAndLinkInputs(m_pChilds[0]->m_pOutputEast, m_pChilds[1]->m_pOutputEast, false)) return false;

			// Internal solve
			if (!m_pChilds[0]->m_pOutputSouth->CheckAndLinkInputs(m_pChilds[1]->m_pInputNorth, NULL, true)) return false;
			break;
		case E_COMP_DIAGONAL:
			if (!m_pInputNorth->CheckAndLinkInputs(m_pChilds[0]->m_pInputNorth, NULL, true)) return false;
			if (!m_pInputWest->CheckAndLinkInputs(m_pChilds[0]->m_pInputWest, NULL, true)) return false;
			if (!m_pOutputSouth->CheckAndLinkInputs(m_pChilds[1]->m_pOutputSouth, NULL, false)) return false;
			if (!m_pOutputEast->CheckAndLinkInputs(m_pChilds[1]->m_pOutputEast, NULL, false)) return false;

			// Internal solve
			if (!m_pChilds[0]->m_pOutputEast->CheckAndLinkInputs(m_pChilds[1]->m_pInputWest, NULL, true)) return false;
			if (!m_pChilds[0]->m_pOutputSouth->CheckAndLinkInputs(m_pChilds[1]->m_pInputNorth, NULL, true)) return false;
			break;
		default:
			assert(false);
			return false;
		}
	
		return true;
	}
	else
	{
		// Solve child
		for (int i = 0; i < m_iNrChilds; i++)
		{
			bool bRes = m_pChilds[i]->SolveInputOutput();
			if (!bRes)
				return false;
		}

		switch(m_operator)
		{
		case E_COMP_UNDEFINED:
			{
				m_pInputNorth = InputBlock::CreateAndLinkInputBlock(m_pChilds[0]->m_pInputNorth, NULL, true);
				m_pInputWest = InputBlock::CreateAndLinkInputBlock(m_pChilds[0]->m_pInputWest, NULL, true);
				m_pOutputSouth = InputBlock::CreateAndLinkInputBlock(m_pChilds[0]->m_pOutputSouth, NULL, false);
				m_pOutputEast = InputBlock::CreateAndLinkInputBlock(m_pChilds[0]->m_pOutputEast, NULL, false);
			}
			break;
		case E_COMP_HORIZONTAL:
			{
				m_pInputNorth = InputBlock::CreateAndLinkInputBlock(m_pChilds[0]->m_pInputNorth, m_pChilds[1]->m_pInputNorth, true);
				m_pInputWest = InputBlock::CreateAndLinkInputBlock(m_pChilds[0]->m_pInputWest, NULL, true);
				m_pOutputSouth = InputBlock::CreateAndLinkInputBlock(m_pChilds[0]->m_pOutputSouth, m_pChilds[1]->m_pOutputSouth, false);
				m_pOutputEast = InputBlock::CreateAndLinkInputBlock(m_pChilds[1]->m_pOutputEast, NULL, false);

				// Internal solve
				if (!m_pChilds[0]->m_pOutputEast->CheckAndLinkInputs(m_pChilds[1]->m_pInputWest, NULL, true)) return false;
			}
			break;
		case E_COMP_VERTICAL:
			{
				m_pInputNorth = InputBlock::CreateAndLinkInputBlock(m_pChilds[0]->m_pInputNorth, NULL, true);
				m_pInputWest = InputBlock::CreateAndLinkInputBlock(m_pChilds[0]->m_pInputWest, m_pChilds[1]->m_pInputWest, true);
				m_pOutputSouth = InputBlock::CreateAndLinkInputBlock(m_pChilds[1]->m_pOutputSouth, NULL, false);
				m_pOutputEast = InputBlock::CreateAndLinkInputBlock(m_pChilds[0]->m_pOutputEast, m_pChilds[1]->m_pOutputEast, false);

				// Internal solve
				if (!m_pChilds[0]->m_pOutputSouth->CheckAndLinkInputs(m_pChilds[1]->m_pInputNorth, NULL, true)) return false;
			}
			break;
		case E_COMP_DIAGONAL:
			{
				m_pInputNorth = InputBlock::CreateAndLinkInputBlock(m_pChilds[0]->m_pInputNorth, NULL, true);
				m_pInputWest = InputBlock::CreateAndLinkInputBlock(m_pChilds[0]->m_pInputWest, NULL, true);
				m_pOutputSouth = InputBlock::CreateAndLinkInputBlock(m_pChilds[1]->m_pOutputSouth, NULL, false);
				m_pOutputEast = InputBlock::CreateAndLinkInputBlock(m_pChilds[1]->m_pOutputEast, NULL, false);

				// Internal solve
				if (!m_pChilds[0]->m_pOutputEast->CheckAndLinkInputs(m_pChilds[1]->m_pInputWest, NULL, true)) return false;
				if (!m_pChilds[0]->m_pOutputSouth->CheckAndLinkInputs(m_pChilds[1]->m_pInputNorth, NULL, true)) return false;
			}
			break;
		}

		// Make these blocks childs of this program
		InputBlock* pBlocks[4] = {m_pInputNorth, m_pInputWest, m_pOutputEast, m_pOutputSouth };
		for (int i = 0; i < 4 ; i++)
			pBlocks[i]->SetParent(this);

		m_pInputNorth->SetAsNorthBlock();
		return true;
	}
}

void ProgramIntermediateModule::ComputeNumInputsToReceive()
{
	// On atomic nodes, compute how many inputs need to be received
	m_iNumberOfInputsRemainedToReceive = 0;
	if (m_bIsAtomic)
	{
		InputBlock* pInputBlocks[2] = {m_pInputNorth, m_pInputWest};
		for (int i = 0; i < 2; i++)
			m_iNumberOfInputsRemainedToReceive += pInputBlocks[i]->m_InputsInBlock.size();
	}
}

bool ProgramIntermediateModule::Validate(SymbolTable* pParent, bool bAtRuntimeSpawn)
{
	if (!bAtRuntimeSpawn)
		ProgramBase::DoBaseSymbolTableInheritance(pParent);

	// Declare the interface defined variables
	if (m_bIsUserDefined)
	{
		// Compilation black box checks
		if (CompilationBlackbox::Get()->IsModuleInSet(m_szModuleName))
			return true;

		CompilationBlackbox::Get()->InsertModuleName(m_szModuleName);

		if (m_bHasNoInput)
			CompilationBlackbox::Get()->AddInputFreeProgram(this);
		// End black box checks

		/*
		InputBlock* pInputBlocks[4] = {m_pInputNorth, m_pInputWest, m_pOutputEast, m_pOutputSouth };
		for (int i = 0; i < 4; i++)
			if (pInputBlocks[i])
				if (!pInputBlocks[i]->Validate(m_SymbolTable))
					return false;
		*/
	}


	for (int i = 0; i < m_iNrChilds; i++)
	{
		bool bShouldInherit = ProgramBase::GetShouldGiveParentTableTo(m_pChilds[i]);
		if (!m_pChilds[i]->Validate(bShouldInherit ? m_SymbolTable : NULL, bAtRuntimeSpawn))
			return false;
	}

	ComputeNumInputsToReceive();
	
	return true;
}

void ProgramIntermediateModule::OnChildProgramFinished(ProgramBase* pChildFinished)
{
	if (!CheckFinishedCondition())
		return;

	if (m_pChilds[0] == pChildFinished)
	{
		m_bChildFinished[0] = true;
	}
	else if (m_pChilds[1] == pChildFinished)
	{
		m_bChildFinished[1] = true;
	}
	else
	{
		PrintExecutionError("Internal error in OnChildProgramFinished. Don't recognize the parameter as a child\n");
	}

	if (m_bChildFinished[0] && m_bChildFinished[1])
	{
		SendProgramFinished(this);
	}
}

bool ProgramIntermediateModule::OnInputReceived(BaseProcessInput* pOriginalInput, BaseProcessInput* pInputNodeReceiver, EInputDirection eDir)
{
	if (!m_bIsAtomic)
		return true;

	m_iNumberOfInputsRemainedToReceive--;
	return true;
}

void ProgramIntermediateModule::OnAfterInputReceived()
{
	// If no inputs need to be received
	if (m_iNumberOfInputsRemainedToReceive <= 0)
		ExecutionBlackbox::Get()->OnModuleReadyForExecution(this);
}

