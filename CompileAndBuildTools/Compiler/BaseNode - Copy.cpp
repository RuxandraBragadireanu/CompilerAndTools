#include "BaseNode.h"
#include "INTERMEDIATENode.h"
#include "calc3_utils.h"

bool ProgramBase::DoBaseSymbolTableInheritance(SymbolTable *pParent)
{
	// Inherit if possible from parent symbol table
	if (pParent)
	{
		if (!m_SymbolTable->InheritTable(*pParent))
			return false;
	}	

	// Insert variables from input / output
	InputBlock* pBlocks[4] = {m_pInputNorth, m_pInputWest, m_pOutputEast, m_pOutputSouth };
	for (int i = 0; i < 4; i++)
	{
		InputBlock* pBlock = pBlocks[i];
		for (unsigned int i = 0; i < pBlock->m_InputsInBlock.size(); i++)
		{
			if (pBlock->m_InputsInBlock[i]->GetType() == E_INPUT_ARRAY)
				continue;

			SimpleProcessItem* pSimpleProcessInput = (SimpleProcessItem*) pBlock->m_InputsInBlock[i];
			for (ListOfInputItemsIter it = pSimpleProcessInput->m_InputItems.begin(); it != pSimpleProcessInput->m_InputItems.end(); it++)
			{
				IDataTypeItem* pItem = *it;
				if (!m_SymbolTable->AddSymbol((char*)pItem->GetName(), pItem))
					return false;
			}
		}
	}

	return true;
}

bool ProgramBase::DoSymbolTableInheritInputBlocks(InputBlock* pNorthBlock, InputBlock* pWestBlock, bool useLastVar)
{
	InputBlock* pBlocks[2] = {pNorthBlock, pWestBlock };
	for (int i = 0; i < 2; i++)
	{
		InputBlock* pBlock = pBlocks[i];
		for (unsigned int i = 0; i < pBlock->m_InputsInBlock.size(); i++)
		{
			if (pBlock->m_InputsInBlock[i]->GetType() == E_INPUT_ARRAY)
				continue;

			SimpleProcessItem* pSimpleProcessInput = (SimpleProcessItem*) pBlock->m_InputsInBlock[i];
			for (ListOfInputItemsIter it = pSimpleProcessInput->m_InputItems.begin(); it != pSimpleProcessInput->m_InputItems.end(); it++)
			{
				IDataTypeItem* pItem = *it;

				// Check if the symbol already exists, where its data points
				char *szItemName = (char*)pItem->GetName();
				IDataTypeItem* pDataItemAddr = m_SymbolTable->GetIdentifierFromTable(szItemName);

				// If there is another data item in the symbol table with the same name, we have to check that 
				// this item has the same address with the one specified by the child. 
				// If not it means that there are two variables with the same names and different meaning/memory zone
				// so we have to issue an warning. Otherwise, it's ok.
				// In both cases, we don't add the exiting variable.
				if (pDataItemAddr)	
				{
					if (pDataItemAddr != pItem)
					{	
						if (!useLastVar)
						{
							PrintCompileWarning(mDebugLineNo , "The program contains the same variable NAMED: %s , but with different meanings. Possible redeclaration of a global variable inside a IF/WHILE program ?\n", szItemName);							
						}

						// Decisional design - use the value as a reference. If you want a copy below line outside brackets
						m_SymbolTable->SetIdentifierAddress(pItem, szItemName);
					}
				}
				else
				{
					if (!m_SymbolTable->AddSymbol((char*)pItem->GetName(), pItem))
						return false;
				}
			}
		}
	}

	return true;
}

bool ProgramBase::GetShouldGiveParentTableTo(ProgramBase *pProgram)
{
	ENodeType eNodeType = pProgram->GetType();
	switch(eNodeType)
	{
	case E_NODE_TYPE_INTERMEDIATE:
		// If it's user defined, it should have it's own symbol table
		if (((ProgramIntermediateModule*)pProgram)->m_bIsUserDefined)
			return false;
		else
			return true;
		break;
	case E_NODE_TYPE_FOREACH:
	case E_NODE_TYPE_WHILE:
	case E_NODE_TYPE_FORNORMAL:
	case E_NODE_TYPE_IF:
	case E_NODE_TYPE_ASIGNMENT:
	case E_NODE_TYPE_DECLARATION:
		return true;
	case E_NODE_TYPE_IDENTITY:
		return false;
	case E_NODE_TYPE_C_CODE_ZONE_ALL:
	case E_NODE_TYPE_C_CODE_ZONE_MASTER:
		return false;
		break;
	}

	assert(false);
	return false;
}

void ProgramBase::CopyBase(ProgramBase* pCloned)
{
	pCloned->m_eNodeType = m_eNodeType;
	pCloned->m_SymbolTable = m_SymbolTable->Clone(pCloned);
	if (m_pInputNorth != NULL) 
	{
		pCloned->m_pInputNorth = m_pInputNorth->Clone();
		pCloned->m_pInputNorth->SetParent(pCloned);
	}

	if (m_pInputWest != NULL)
	{
		pCloned->m_pInputWest = m_pInputWest->Clone();
		pCloned->m_pInputWest->SetParent(pCloned);
	}

	if (m_pOutputSouth != NULL)
	{
		pCloned->m_pOutputSouth = m_pOutputSouth->Clone();
		pCloned->m_pOutputSouth->SetParent(pCloned);
	}

	if (m_pOutputEast != NULL)
	{
		pCloned->m_pOutputEast = m_pOutputEast->Clone();
		pCloned->m_pOutputEast->SetParent(pCloned);
	}

	pCloned->m_bHasNoInput = m_bHasNoInput;
	pCloned->mDebugLineNo = mDebugLineNo;
}

void ProgramBase::SendProgramFinished(ProgramBase* pChildFinished)
{
	pChildFinished->m_bIsProgramFinished = true;

	if (m_pProgramParent)
		m_pProgramParent->OnChildProgramFinished(this);
	else
	{
		// It should be the main program or it is an error!
		if (strcmp(GetName(), "MAIN"))
		{
			PrintExecutionError("Internal error, we don't have the m_pPrograParent to send the program finished messages and it appears to be not the main program !\n");
		}
	}
}

bool ProgramBase::CheckFinishedCondition()
{
	if (m_bIsProgramFinished)
	{
		PrintExecutionError("Internal error- wrong OnCHildProgramFinished received after already module was finished");
		return false;
	}

	return true;
}
