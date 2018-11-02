#define _CRT_SECURE_NO_DEPRECATE

#include "MainInputParser.h"
#include "CompilationBlackbox.h"
#include "BaseNode.h"
#include "calc3_utils.h"

#define INPUT_FILE_NAME	"MainInput.txt"


void MainInputParser::ParseSimpleDataType(FILE *&f, IDataTypeItem *pDataType, DataTypes eDataType)
{
	char *szVarName = (char*)pDataType->GetName();
	const int iMaxBuffSize = 1024;
	char strBufferOfRead[iMaxBuffSize];

	// Read and check the variable name
	fscanf(f, "%s", strBufferOfRead);
	if (strcmp(szVarName, strBufferOfRead))
	{
		char szErrorBuff[1024];
		sprintf_s(szErrorBuff, 1024, "Input for module main: Expected value for variable %s but the next variable in input file is for variable %s. The variables should be given in order, from north and then west.\n", szVarName, strBufferOfRead);
		PrintCompileError(-1, szErrorBuff);
		return ;
	}

	switch(eDataType)
	{
		case ETYPE_BOOL:
			{
				fscanf(f, "%s", strBufferOfRead);
				if (!strcmp(strBufferOfRead, "true"))
					((BoolDataItem*)pDataType)->SetValue(true);
				else
					((BoolDataItem*)pDataType)->SetValue(false);
			}
			break;
		case ETYPE_CHAR:
			{
				fscanf(f, "%c", strBufferOfRead);
				if (!strcmp(strBufferOfRead, "true"))
					((CharDataItem*)pDataType)->SetValue(true);
				else
					((CharDataItem*)pDataType)->SetValue(false);
			}
			break;
		case ETYPE_INT:
			{			
				int iValue;
				fscanf(f, "%d", &iValue);
				((IntDataItem*)pDataType)->SetValue(iValue);
			}
			break;
		case ETYPE_FLOAT:
			{
				float fValue;
				fscanf(f, "%f", &fValue);
				((FloatDataItem*)pDataType)->SetValue(fValue);
			}
			break;
		case ETYPE_STRING: 
			{
				char szBuff[1024];
				fscanf(f, "%s", szBuff);
				((StringDataItem*)pDataType)->SetValue(szBuff);
			}
			break;
		
		default:
			assert(false && "Unknown fact");
	}
}

void MainInputParser::ParseVectorDataType(FILE*& f, IDataTypeItem* pDataType, DataTypes eVectorDataType)
{
	assert(false && "Not implemented");
}

void MainInputParser::ParseVectorOfProcessesType(FILE *&f, VectorProcessItem *pDataType, const ArrayOfBaseProcessInputs& pElementType)
{
	assert(false && "Not implemented");
}

void MainInputParser::ParseInputFile()
{
	ProgramBase* pMainModule = (ProgramBase*)(CompilationBlackbox::Get()->GetModuleByName("MAIN"));
	if (pMainModule == NULL)
	{
		assert(false && "We shouldn't arrive here if we don't have a main module!");
		PrintCompileError(-1, "I don't have a MAIN module");
		return;
	}

	FILE* f = NULL;
	fopen_s(&f, INPUT_FILE_NAME, "r");
	if (f == NULL)
	{
		
		// If the main module doesn't have any input, we don't need the input file
		if (pMainModule->m_pInputNorth->GetProcessesCount() == 0 && pMainModule->m_pInputWest->GetProcessesCount() == 0)
			return;

		PrintCompileError(-1, "MainInput.txt was not found !!!!");
		return;
	}

	InputBlock* arrInputsBlocks[2] = { pMainModule->m_pInputNorth, pMainModule->m_pInputWest };
	for (int i = 0; i < 2; i++)
	{
		InputBlock *pBlock = arrInputsBlocks[i];
		for (unsigned int j = 0; j < pBlock->m_InputsInBlock.size(); j++)
		{
			BaseProcessInput* pBasePI = pBlock->m_InputsInBlock[j];
			if (pBasePI->GetType() != E_INPUT_ARRAY)
			{
				SimpleProcessItem* pThisPI = (SimpleProcessItem*) pBasePI;
				SimpleProcessItem* pSimplePI = new SimpleProcessItem();

				for (ListOfInputItemsIter it = pThisPI->m_InputItems.begin(); it != pThisPI->m_InputItems.end(); it++)
				{
					IDataTypeItem* pClonedType = (*it)->Clone();
					if ((*it)->m_eDataType != ETYPE_GENERAL_ARRAY)
					{
						ParseSimpleDataType(f, pClonedType, pClonedType->m_eDataType);
					}
					else
					{
						ParseVectorDataType(f, pClonedType, pClonedType->m_eItemsDataType);
					}

					pSimplePI->AddItem(pClonedType);
				}

				pThisPI->GoDownValue(pSimplePI);
			}
			else
			{
				VectorProcessItem* pThisVPI = (VectorProcessItem*) pBasePI;
				VectorProcessItem* pVectorPI = new VectorProcessItem();
				pVectorPI->SetItemType(pThisVPI->GetItemType(), pThisVPI->GetName());
				ParseVectorOfProcessesType(f, pVectorPI, pVectorPI->GetItemType());

				pThisVPI->GoDownValue(pVectorPI);
			}				
		}
	}

	fclose(f);
}
