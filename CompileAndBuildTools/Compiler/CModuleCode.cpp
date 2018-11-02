#include "CModuleCode.h"
#include "CompilationBlackbox.h"
#include "calc3_utils.h"
#include <string>

/*
const char szKIncludeString[] = "INCLUDE_FILES\n";
const char szKCodeString[] =	"CODE_ZONE\n";
*/

void CModuleCode::AppendLineCode(const char *szCodeLine)
{
	m_strSourceCode.append("\t");
	m_strSourceCode.append(szCodeLine);
	//m_strSourceCode.append(1, '\n');
}

bool CModuleCode::WriteCodeToFile(char *pModuleName, FILE *f)
{
	char *szStringIterator = (char*) m_strSourceCode.c_str() + 1;

	// Write headers for this function - note that you should protect each header to avoid duplicating code text
	// ---------------------------------------------------------------------------------------------------------
	/*
	// Step 1: read the INCLUDE string
	
	int iStringSize = m_strSourceCode.size();
	//int iStringiterator_Index = 0; 

	if (!ABSTFactory::ExactMachingStrings(szStringIterator, szKIncludeString, sizeof(szKIncludeString)))
	{
		PrintCompileError(pModuleName, "line", "Not the correct format for C code file - \n");
		return false;
	}
	szStringIterator += sizeof(szKIncludeString);
	//iStringiterator_Index += sizeof(szIncludeString);

	// Step 2: read all the includes
	while(*szStringIterator != '\0'  && !ABSTFactory::ExactMachingStrings(szStringIterator, szKCodeString, sizeof(szKCodeString)))
	{
		char* szNextLine = strchr(szStringIterator, '\n');
		if (szNextLine == NULL)
		{
			PrintCompileError(pModuleName, "line", "Incorrect includes format - \n");
			return false;
		}

		const int lineCapMax = 1024;
		char lineBuff[lineCapMax];
		int iIndex = 0;
		for (char* szBegin = szStringIterator; szBegin != szNextLine && iIndex < lineCapMax - 1; szBegin++, iIndex++)
			lineBuff[iIndex] = *szBegin;

		lineBuff[iIndex] = '\0';

		//printf("next: %s\n", szNextLine);
		//printf("iter: %s\n", szStringIterator);

		fprintf(f, "%s\n", lineBuff);
		szStringIterator = szNextLine + 2;
	}
	// ---------------------------------------------------------------------------------------------------------
	if (*szStringIterator == '\0' || !ABSTFactory::ExactMachingStrings(szStringIterator, szKCodeString, sizeof(szKCodeString)))
	{
		PrintCompileError(pModuleName, "line", "Not the correct format the C code marker not found - \n");
		return false;
	}
	
	szStringIterator += sizeof(szKCodeString);
	*/
	fprintf(f, "\n\n");
	// Write the function header
	fprintf(f, "void %s(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)\n", pModuleName);
	fprintf(f, "{\n");
	
	// Write the local variables
	fprintf(f, "\t// Local variables declaration: \n");
	ProgramBase* pProgram = (ProgramBase*)CompilationBlackbox::Get()->GetModuleByName(pModuleName);
	ArrayOfBaseProcessInputs* pArraysOfInputs[4] = { &pProgram->m_pInputNorth->m_InputsInBlock, &pProgram->m_pInputWest->m_InputsInBlock
		, &pProgram->m_pOutputSouth->m_InputsInBlock, &pProgram->m_pOutputEast->m_InputsInBlock};

	const char* pStringsOfBlocks[4] = {"pNorth", "pWest", "pSouth", "pEast"};
	for (int iZonesIter = 0; iZonesIter < 4; iZonesIter++)
	{
		int iProcessItemCounter = 0;
		for (ArrayOfBaseProcessInputsConstIter it = pArraysOfInputs[iZonesIter]->begin(); it != pArraysOfInputs[iZonesIter]->end(); it++, iProcessItemCounter++)
		{
			BaseProcessInput* pBasePI = *it;
			// Simple items - arrays of atomic types or atomic types
			if (pBasePI->GetType() == E_INPUT_SIMPLE)
			{
				SimpleProcessItem* pSPI = (SimpleProcessItem*) pBasePI;
				int iSimpleInputCounter = 0;
				for (ListOfInputItemsConstIter it = pSPI->m_InputItems.begin(); it != pSPI->m_InputItems.end(); it++, iSimpleInputCounter++)
				{
					const int iMaxStrSize = 1024;
					char szStringOfSimpleProcesItem[iMaxStrSize];
					sprintf_s(szStringOfSimpleProcesItem, iMaxStrSize-1, "((SimpleProcessItem*)%s->m_InputsInBlock[%d])->m_InputItems[%d])", pStringsOfBlocks[iZonesIter], iProcessItemCounter, iSimpleInputCounter);

					IDataTypeItem* pSimpleItem = *it;
					switch(pSimpleItem->m_eDataType)
					{					
					case ETYPE_BOOL:
						fprintf_s(f, "\tbool& %s = ((BoolDataItem*)%s->GetValueRef();\n", pSimpleItem->GetName(), szStringOfSimpleProcesItem);
						break;
					case ETYPE_CHAR:
						fprintf_s(f, "\tchar* %s = ((CharDataItem*)%s->GetValueRef();\n", pSimpleItem->GetName(), szStringOfSimpleProcesItem);
						break;
					case ETYPE_INT:
						fprintf_s(f, "\tint& %s = ((IntDataItem*)%s->GetValueRef();\n", pSimpleItem->GetName(), szStringOfSimpleProcesItem);
						break;
					case ETYPE_FLOAT:
						fprintf_s(f, "\tfloat& %s = ((FloatDataItem*)%s->GetValueRef();\n", pSimpleItem->GetName(), szStringOfSimpleProcesItem);
						break;
					case ETYPE_STRING:
						fprintf_s(f, "\tchar** %s = ((StringDataItem*)%s->GetValueRef();\n", pSimpleItem->GetName(), szStringOfSimpleProcesItem);
						break;
					case ETYPE_DATA_BUFFER:
						fprintf_s(f, "\tBufferDataItem* %s = ((BufferDataItem*)%s->GetValue();\n", pSimpleItem->GetName(), szStringOfSimpleProcesItem);
						break;
					case ETYPE_GENERAL_ARRAY:
						{
							switch(pSimpleItem->m_eDataType)
							{					
							case ETYPE_BOOL:
								fprintf_s(f, "\tbool* %s = ((BoolDataItem*)%s->GetValueRef();\n", pSimpleItem->GetName(), szStringOfSimpleProcesItem);
								break;
							case ETYPE_CHAR:
								fprintf_s(f, "\tchar* %s = ((CharDataItem*)%s->GetValueRef();\n", pSimpleItem->GetName(), szStringOfSimpleProcesItem);
								break;
							case ETYPE_INT:
								fprintf_s(f, "\tint* %s = ((IntDataItem*)%s->GetValueRef();\n", pSimpleItem->GetName(), szStringOfSimpleProcesItem);
								break;
							case ETYPE_FLOAT:
								fprintf_s(f, "\tfloat* %s = ((FloatDataItem*)%s->GetValueRef();\n", pSimpleItem->GetName(), szStringOfSimpleProcesItem);
								break;
							case ETYPE_STRING:
								fprintf_s(f, "\tchar*** %s = ((StringDataItem*)%s->GetValueRef();\n", pSimpleItem->GetName(), szStringOfSimpleProcesItem);							
								break;
							
							default:
								assert(false && "Undefined type");
							}
						};
						break;
					default:
						assert(false && "Undefined type");
						break;
					}
				}
			}
			else // Array - vector of processes
			{
				const int iMaxStrSize = 1024;
				char szStringOfSimpleProcesItem[iMaxStrSize];
				sprintf_s(szStringOfSimpleProcesItem, iMaxStrSize-1, "*((VectorProcessItem*)%s->m_InputsInBlock[%d])", pStringsOfBlocks[iZonesIter], iProcessItemCounter);

				VectorProcessItem* pVPI = (VectorProcessItem*) pBasePI;
				fprintf(f, "\tVectorProcessItem& %s = %s;\n", pVPI->GetName(), szStringOfSimpleProcesItem);
			}
		}
	}

	// Spaces between variables declaration and user code
	fprintf(f, "\n\n");

	// Write the code given by user
	fprintf(f, "\t// User code: \n");
	fprintf(f, "\t%s\n", szStringIterator /*m_strSourceCode.c_str()*/);

	fprintf(f, "}\n");
	fprintf(f, "\n\n");

	return true;
}

