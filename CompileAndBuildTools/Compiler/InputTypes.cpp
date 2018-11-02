#include "InputTypes.h"
#include "calc3_defs.h"
#include "BaseNode.h"
#include "calc3_utils.h"
#include "CommonUtils.h"
#include "SymbolTable.h"
#include "INTERMEDIATENode.h"
#include <string.h>
#include <assert.h>
#include "WHILENode.h"
#include "FOREachNode.h"

#include "CodeSerializationFactory.h"

#ifdef RUN_FROM_VS_DEBUGGER
// These are used to verify if an object writes on the stream the exact quantity it says to write
#define SERIALIZE_CHECKER_INIT_OBJECT	\
	const int iEstimatedSize = GetSerializedSize();	\
	char* pBeforeWriteAddress = stream.GetHeaderAddress();

#define SERIALIZE_CHECKER_END_OBJECT	\
	char *pCurrentWriteAddress = stream.GetHeaderAddress();	\
	assert(iEstimatedSize == (pCurrentWriteAddress - pBeforeWriteAddress) && "INCORRECT SIZE ESTIMATION FOR THIS OBJECT");

#define CSF_INIT_SERIALIZE_CHECK \
	char* totalSizeAddressToWrite = stream.m_BufferPos; \
	stream.WriteSimpleType(0);

#define CSF_END_SERIALIZE_CHECK	\
	const int totalSize = stream.m_BufferPos - totalSizeAddressToWrite;	\
	assert(expectedSize == totalSize && "A different data size than expected was written for expression serialization");	\
	memcpy(totalSizeAddressToWrite, &totalSize, sizeof(int));

#define CSF_INIT_DESERIALIZE_CHECK	\
	const char* beginDesAddr = stream.m_BufferPos;	\
	int expectedSize = 0;	\
	stream.ReadSimpleType(expectedSize);	

#define CSF_END_DESERIALIZE_CHECK	\
	const int dataRead = stream.m_BufferPos - beginDesAddr;	\
	assert(dataRead == expectedSize && "Didn't read data as expected on deserialization process");

//assert(result != NULL && "Result variable was not set at the end of deserialization process");	\	
#else
#define SERIALIZE_CHECKER_INIT_OBJECT
#define SERIALIZE_CHECKER_END_OBJECT

#define CSF_INIT_SERIALIZE_CHECK
#define CSF_END_SERIALIZE_CHECK
#define CSF_INIT_DESERIALIZE_CHECK
#define CSF_END_DESERIALIZE_CHECK
#endif


BaseProcessInput::BaseProcessInput(EInputTypes eType, InputBlock* pParent) : 
m_pFrom(NULL), m_pNext(NULL), m_eType(eType), m_iFromIndexRule(0), m_iToIndexRule(0), m_bInputReceived(false)
	,m_pParentBlock(pParent)
	,m_bContainsObservedInput(false)
	,m_iIndexInBlock(0)
	, m_pParentVector(NULL)
	, m_iParentVectorIndex(-1)
	, m_eLastIterativeProgramVisited(E_NODE_TYPE_IDENTITY)
{
}

BaseProcessInput::~BaseProcessInput()
{
}

void BaseProcessInput::SetOrientedLink(BaseProcessInput* pOutput, BaseProcessInput* pInput)
{
	pOutput->m_pNext = pInput;
	pInput->m_pFrom = pOutput;
}

void BaseProcessInput::SetOrientedLink(ArrayOfBaseProcessInputs& pOutputs, ArrayOfBaseProcessInputs& pInputs)
{
	// They should have the same number of params and types.
	// I don't check them here too for speed reasons
	ArrayOfBaseProcessInputsIter itOut = pOutputs.begin();
	ArrayOfBaseProcessInputsIter itInputs = pInputs.begin();

	for (; itOut != pOutputs.end() && itInputs != pInputs.end(); itOut++, itInputs++)
	{
		BaseProcessInput::SetOrientedLink(*itOut, *itInputs);
	}
}

bool BaseProcessInput::GoDownValue(BaseProcessInput* pOriginalInput)
{
	ProgramBase* pProgramParent = m_pParentBlock->m_pParentProgram;

	// Send message to the parent program that this input arrived
	// If this can't process it, don't go further !
	if (!pProgramParent->OnInputReceived(pOriginalInput, this, (m_pParentBlock->IsNorthBlock() ? E_INPUT_DIR_NORTH : E_INPUT_DIR_WEST)))
		return false;

	bool bResult = true;
	if (pProgramParent->IsAtomicProgram())
	{
		// When buffering input data it may happen that an input to come twice - once from buffering and another one through hypergraph.
		if (!m_bInputReceived)
		{
			m_bInputReceived = true; 

			//ProgramItermediateModule* pIntermediateModule = (ProgramItermediateModule*)pOriginalInput;
		
			// Copy the values to this process input
			bool bRes = SetValues(pOriginalInput);
			pProgramParent->OnAfterInputReceived();
			return bRes;
		}
	}
	else
	{
		if (pOriginalInput->GetType() == E_INPUT_SIMPLE)
		{
			// If this simple node input is containing input items observed by different components (like FOR, WHILE, IF etc)
			// We should set their values and inform all the observers
			// TODO: OPTIMIZE ME:  - instead of assigning values to the whole simple node, we should keep a map where the desired inputs are
			// and avoid unnecessary writes
			if (IsContainingInputObserved())
				SetValues(pOriginalInput);
	
			// If the next item is NULL then:
			// Check if this item is in a vector, and the next of a parent vector exist
			// If this happens, we must redirect it to the next vector as the same index as in the original array
			if (!m_pNext)
			{
				// This is expected to be a simple process item because the original input is...
				SimpleProcessItem* pSPI = (SimpleProcessItem*) this;				
				VectorProcessItem* pThisParentVector = pSPI->m_pParentVector;
				if (pThisParentVector != NULL && pThisParentVector->m_pNext != NULL)
				{
					// If vector has a next then it must be a vector:)
					VectorProcessItem* pVPINext = (VectorProcessItem*)pThisParentVector->m_pNext;
					assert(pSPI->m_iParentVectorIndex >= 0 && pSPI->m_iIndexInBlock >=0 );

					int iParentVector_FromIndexRule = pThisParentVector->m_iFromIndexRule;
					int iParentVector_ToIndexRule = pThisParentVector->m_iToIndexRule;

					// Set the correct m_pNext
					const ArrayOfBaseProcessInputs* itemArray = NULL;
					if (iParentVector_FromIndexRule > 0) 
						itemArray = &(pVPINext->GetVectorItemByIndex(pSPI->m_iParentVectorIndex - iParentVector_FromIndexRule));
					else if (iParentVector_ToIndexRule > 0)
						itemArray = &(pVPINext->GetVectorItemByIndex(pSPI->m_iParentVectorIndex + iParentVector_ToIndexRule));
					else
						itemArray = &(pVPINext->GetVectorItemByIndex(pSPI->m_iParentVectorIndex));

					m_pNext = (*itemArray)[pSPI->m_iIndexInBlock];
				}
				else	// No link, save the input there at the end of the chain , see the reason below
				{
					// If this parent program is not atomic and it has no next, it is a stop at it must assign it's value to be used in the module's body
					// For the moment we use this only for simple process items not for arrays of processes.
					SetValues(pOriginalInput);
				}
			}
			
			if(m_pNext)	// It has a simple input node link ?
			{
//#ifdef IS_DEBUG_ANALYSIS_ENABLED
				if (m_pNext->GetType() != E_INPUT_SIMPLE)
				{
					assert(false);
					return false;
				}
//#endif				

				// Special cases for modules FOR_type(condition), FOR_type(i = 0; i < ....;i++), WHILE_type(condition)
				// in which we connect to atomic modules through vectors.
				// The solution in this case is to wait for all the values to arrive in the output vector and
				// then send it.  m_pNext is now a SimpleProcessInput	
				//-------------------------------------------------------------------------------------------
				VectorProcessItem *pParentVector = ((SimpleProcessItem*)m_pNext)->m_pParentVector;
				VectorProcessItem *pFromParentVector = (VectorProcessItem*)(pParentVector ? pParentVector->m_pFrom : NULL);
				ProgramBase* pBase = m_pNext->GetParentBlock()->m_pParentProgram;
				if (pFromParentVector && pParentVector && pBase->IsAtomicProgram() && pOriginalInput->GetLastIterativeProgramVisited() != E_NODE_TYPE_IDENTITY)
				{
					switch(pOriginalInput->GetLastIterativeProgramVisited())
					{
						case E_NODE_TYPE_WHILE:
						case E_NODE_TYPE_FORNORMAL:
							{
								// Store the value in the atomic module vector but don't inform it yet that this input is sent
								m_pNext->SetValues(pOriginalInput);

								// Decrement number of expected values
								pFromParentVector->m_iNumberOfInputsToBeReceived--;

								// If the WHILE program is finished, then we can send a message to inform the atomic module 
								ProgramWHILE* pWhileProgram = (ProgramWHILE*) pProgramParent;
								if (pWhileProgram->IsLastArrayInputSent() && pWhileProgram->IsWhileModuleFinishing())
								{
									pBase->OnAfterInputReceived();
									pBase->OnInputReceived(pOriginalInput, this, (m_pNext->GetParentBlock()->IsNorthBlock() ? E_INPUT_DIR_NORTH : E_INPUT_DIR_WEST));
								}
							}
							break;
						case E_NODE_TYPE_FOREACH:
							{
								// In the current FOR we know the number of iterations
								if(pFromParentVector->m_iNumberOfInputsToBeReceived > 0)
								{
									// Store the value in the atomic module vector but don't inform it yet that this input is sent
									m_pNext->SetValues(pOriginalInput);

									// Decrement number of expected values
									pFromParentVector->m_iNumberOfInputsToBeReceived--;

									// If this is zero, we inform that this input vector has been received
									if (pFromParentVector->m_iNumberOfInputsToBeReceived == 0)
									{
										pBase->OnInputReceived(pOriginalInput, this, (m_pNext->GetParentBlock()->IsNorthBlock() ? E_INPUT_DIR_NORTH : E_INPUT_DIR_WEST));
										pBase->OnAfterInputReceived();
									}
								}							
							}
							break;
						default:
							assert(false && "Internal error: Not implemented yes");
					}
				}
				else
				{
					m_pNext->GoDownValue(pOriginalInput);
				}
			}
		}
		else 
		{
			//--- This must be an array now 
			VectorProcessItem* pCurrentVector = static_cast<VectorProcessItem*>(this);
			VectorProcessItem* pVPI = static_cast<VectorProcessItem*>(pOriginalInput);

			// If the next node is not null, we send this to another array, total or partially
			if (m_pNext)
			{	
#ifdef IS_DEBUG_ANALYSIS_ENABLED
				if (m_pNext->m_iToIndexRule == -1)
				{
					assert(false);
					return false;
				}
#endif
				// We can do this send because each internal item from/to point to the right next 
				if (m_iToIndexRule > 0 || m_iFromIndexRule > 0)
				{
					// Send manually each item, split the array !
					for (unsigned int i = 0; i < pVPI->m_ArrayOfItemInputs.size(); i++)
					{
						// Get this and original item
						ArrayOfBaseProcessInputs& thisItem = pCurrentVector->GetVectorItemByIndex(i);
						ArrayOfBaseProcessInputs& originalItem = pVPI->GetVectorItemByIndex(i);

						// Modify the vector index to be correct when sending to the destination vector
						int iNewVectorIndex = i;
						if (m_iFromIndexRule > 0 && m_iFromIndexRule <= i)
						{
							iNewVectorIndex = i - m_iFromIndexRule;							
						}
						else if (m_iToIndexRule > 0)
						{
							iNewVectorIndex = i + m_iToIndexRule;
						}

						for (ArrayOfBaseProcessInputsIter it_this = thisItem.begin(), it_original = originalItem.begin(); it_this != thisItem.end(); it_this++, it_original++)
						{						
							BaseProcessInput* pOriginalInput = *it_original;
							pOriginalInput->m_iParentVectorIndex = iNewVectorIndex;
							(*it_this)->GoDownValue(pOriginalInput);
						}
					}
				}
				else	// All array !
				{
					m_pNext->GoDownValue(pOriginalInput);
				}
			}
			else
			{
				// We may need to split the entire array on components (FOR is an example)
				//, or it may be a dead end
				for (unsigned int i = 0; i < pCurrentVector->m_ArrayOfItemInputs.size(); i++)
				{
					ArrayOfBaseProcessInputs& thisArrayOfProcesses = pCurrentVector->m_ArrayOfItemInputs[i];
					for (ArrayOfBaseProcessInputsIter it_this = thisArrayOfProcesses.begin(), it_original = pVPI->m_ArrayOfItemInputs[i].begin(); it_this != thisArrayOfProcesses.end(); it_this++, it_original++)
					{
						BaseProcessInput* pSI = *it_this;
						if (pSI == NULL)
						{
							// Just let's see what enters through here
							//assert(false);
							//PrintCompileError("file", "line", "PLEASE SEND THIS PROGRAM TO ME! I just want to see where is this special case\n");
							return true;
						}

						if (pSI->m_pNext == NULL)
							continue;

						pSI->m_pNext->GoDownValue(*it_original);
					}
				}
			}
		}
	}

	return bResult; //pProgramParent->OnInputReceived(pOriginalInput);
}

IDataTypeItem::~IDataTypeItem()
{
#ifdef IGNORE_AGAPIA_DECL_AND_ASIGNMENTS
	if (m_pProgramsListeners)
		delete m_pProgramsListeners;
#endif 

	if (m_szName) delete m_szName;
}

unsigned int IDataTypeItem::GetSerializedSize()
{
	LOG(("Should never get here and serialize from thhhhhh"));
	assert(false);
	return 0;
}

void IDataTypeItem::Serialize(Streams::BytesStreamWriter& writer)
{
	LOG(("Should never get here and serialize from thhhhhh"));
	assert(false);
}

void IDataTypeItem::Deserialize(Streams::BytesStreamReader& writer)
{
	LOG(("Should never get here and serialize from thhhhhh"));
	assert(false);
}

#ifdef IGNORE_AGAPIA_DECL_AND_ASIGNMENTS
void IDataTypeItem::AddProgramListener(ProgramBase* pProgram)
{
	// Add it to the listeners, if not exists then create 
	if (m_pProgramsListeners == NULL)
		m_pProgramsListeners = new ListOfProgramsListener();

	m_pProgramsListeners->push_back(pProgram);

	// Inform its parent that it's an observed input type - it needs to know this because 
	if (m_pParent)
	{
		m_pParent->SetContainingObservedInput();
	}
}

inline void IDataTypeItem::FireProgramListeners()
{
	if (m_pProgramsListeners == NULL)
		return;

	for (ListOfProgramsListenerIter it = m_pProgramsListeners->begin(); it != m_pProgramsListeners->end(); it++)
		(*it)->OnInputItemReady(m_szName);
}
#endif


void IDataTypeItem::PrintDebugInfo(int iSpaceLeft)
{
	PutsEmpty(iSpaceLeft);

	if (m_eDataType == ETYPE_GENERAL_ARRAY)
		printf("Name %s | DataType: Array of %s\n", m_szName, GetDataTypesString(m_eItemsDataType));
	else
		printf("Name %s | DataType: %s\n", m_szName, GetDataTypesString(m_eDataType));
}

inline unsigned int IDataTypeItem::GetSerializedDataTypeSize(IDataTypeItem* dataType)
{
	// TODO: if this is critical then cache this value instead of computing strlen each time
	// Currently we are not using this at runtime (only compile time)
	return Streams::GetStringSizeOnStream(dataType->m_szName) + sizeof(dataType->m_eDataType);
}

void IDataTypeItem::SerializeDataType(Streams::BytesStreamWriter& stream, const IDataTypeItem* dataTypeItem)
{
	stream.WriteSimpleType(dataTypeItem->m_eDataType);
	stream.WriteString(dataTypeItem->m_szName);
}

IDataTypeItem* IDataTypeItem::DeserializeDataType(Streams::BytesStreamReader& stream)
{
	DataTypes dataType;
	char buffString[1024];
	stream.ReadSimpleType(dataType);
	stream.ReadString(buffString, 1024);

	IDataTypeItem* dataTypeItem = ItemTypeFactory::CreateInputItem(dataType, buffString);
	return dataTypeItem;
}

IDataTypeItem* IDataTypeItem::Clone(int *pAllocatedData /* = NULL */)
{
	assert(false && "This type shouldn't use this clone!");
	return NULL;
}

IDataTypeItem* IDataTypeItem::Clone(bool *pAllocatedData /* = NULL */)
{
	assert(false && "This type shouldn't use this clone!");
	return NULL;
}

IDataTypeItem* IDataTypeItem::Clone(float *pAllocatedData /* = NULL */)
{
	assert(false && "This type shoudn't use this clone!");
	return NULL;
}

IDataTypeItem* IDataTypeItem::Clone(char *pAllocatedData /* = NULL */)
{
	assert(false && "This type shoudn't use this clone!");
	return NULL;
}

void IDataTypeItem::CopyBase(IDataTypeItem* pItem)
{
	pItem->m_eDataType = m_eDataType;
	pItem->m_eItemsDataType = m_eItemsDataType;
	pItem->m_szName = _strdup(m_szName);
}

void StringDataItem::SetValue(char *pSzData)
{
	assert(m_pData == NULL && "Is this the wanted behavior ?!?!");
	m_pData = _strdup(pSzData);
}

IDataTypeItem* StringDataItem::Clone(char* pAllocatedData)
{
	StringDataItem* pDataItem = (pAllocatedData ? new StringDataItem(pAllocatedData) : new StringDataItem());
	CopyBase(pDataItem);
	pDataItem->m_pData = _strdup(m_pData);
	return pDataItem;
}

unsigned int StringDataItem::GetSerializedSize()
{
	return (sizeof(int) /*nr of items*/ + strlen(m_szName));
}

void StringDataItem::Serialize(Streams::BytesStreamWriter &stream)
{
	int iLen = strlen(m_pData);
	stream.WriteSimpleType<int>(iLen);
	stream.WriteByteArray(m_pData, iLen);
}

void StringDataItem::Deserialize(Streams::BytesStreamReader &stream)
{
	// [NUMBER OF CHARS | CHARS]
	int iNrChars = 0;
	stream.ReadSimpleType<int>(iNrChars);
	if (m_pData)
		delete m_pData;

	m_pData = new char[iNrChars + 1];
	stream.ReadByteArray(m_pData, iNrChars);
	m_pData[iNrChars] = '\0';
}

IDataTypeItem* BoolDataItem::Clone(bool* pAllocatedData)
{	
	BoolDataItem* pDataItem = (pAllocatedData ? new BoolDataItem(pAllocatedData) : new BoolDataItem());
	CopyBase(pDataItem);
	pDataItem->m_pData = new bool;
	*pDataItem->m_pData = *m_pData;

	return pDataItem;
}

unsigned int BoolDataItem::GetSerializedSize() { return sizeof(char); }
void BoolDataItem::Serialize(Streams::BytesStreamWriter& stream)
{
	SERIALIZE_CHECKER_INIT_OBJECT
	
	char c = (char) *m_pData;
	stream.WriteSimpleType<char>(c);
	
	SERIALIZE_CHECKER_END_OBJECT
}

void BoolDataItem::Deserialize(Streams::BytesStreamReader& stream)
{
	SERIALIZE_CHECKER_INIT_OBJECT
	
	char c = 0;
	stream.ReadSimpleType<char>(c);
	*m_pData = (c == 0 ? false : true);
	
	SERIALIZE_CHECKER_END_OBJECT
}

IDataTypeItem* CharDataItem::Clone(char* pAllocatedData)
{	
	CharDataItem* pDataItem = (pAllocatedData ? new CharDataItem(pAllocatedData) : new CharDataItem());
	CopyBase(pDataItem);
	pDataItem->m_pData = new char;
	*pDataItem->m_pData = *m_pData;
	return pDataItem;
}

unsigned int CharDataItem::GetSerializedSize() { return sizeof(char); }
void CharDataItem::Serialize(Streams::BytesStreamWriter& stream)
{
	SERIALIZE_CHECKER_INIT_OBJECT

	stream.WriteSimpleType<char>(*m_pData);

	SERIALIZE_CHECKER_END_OBJECT
}

void CharDataItem::Deserialize(Streams::BytesStreamReader& stream)
{
	SERIALIZE_CHECKER_INIT_OBJECT

	stream.ReadSimpleType<char>(*m_pData);

	SERIALIZE_CHECKER_END_OBJECT
}

IDataTypeItem* IntDataItem::Clone(int* pAllocatedData)
{	
	IntDataItem* pDataItem = (pAllocatedData ? new IntDataItem(pAllocatedData) : new IntDataItem());
	CopyBase(pDataItem);
	pDataItem->m_pData = new int;
	*pDataItem->m_pData = *m_pData;
	return pDataItem;
}

unsigned int IntDataItem::GetSerializedSize() { return sizeof(int); }
void IntDataItem::Serialize(Streams::BytesStreamWriter& stream)
{
	SERIALIZE_CHECKER_INIT_OBJECT

	stream.WriteSimpleType<int>(*m_pData);

	SERIALIZE_CHECKER_END_OBJECT
}

void IntDataItem::Deserialize(Streams::BytesStreamReader& stream)
{
	SERIALIZE_CHECKER_INIT_OBJECT

	stream.ReadSimpleType<int>(*m_pData);

	SERIALIZE_CHECKER_END_OBJECT
}

IDataTypeItem* FloatDataItem::Clone(float* pAllocatedData)
{	
	FloatDataItem* pDataItem = (pAllocatedData ? new FloatDataItem(pAllocatedData) : new FloatDataItem());
	CopyBase(pDataItem);
	pDataItem->m_pData = new float;
	*pDataItem->m_pData = *m_pData;
	return pDataItem;
}

unsigned int FloatDataItem::GetSerializedSize() { return sizeof(float); }
void FloatDataItem::Serialize(Streams::BytesStreamWriter& stream)
{
	SERIALIZE_CHECKER_INIT_OBJECT

	stream.WriteSimpleType<float>(*m_pData);

	SERIALIZE_CHECKER_END_OBJECT
}

void FloatDataItem::Deserialize(Streams::BytesStreamReader& stream)
{
	SERIALIZE_CHECKER_INIT_OBJECT

	stream.ReadSimpleType<float>(*m_pData);

	SERIALIZE_CHECKER_END_OBJECT
}

void BoolVectorDataItem::SetValue(unsigned int iIndex, bool bValue)
{
	// Check the internal data buffer first
	if (m_iCurrentAllocatedSize <= iIndex)
	{
		if (m_pDataBuffer == NULL)
		{
			m_iBiggestIndexUsed = iIndex;
			m_iCurrentAllocatedSize = iIndex<<1;
			m_pDataBuffer = new bool[m_iCurrentAllocatedSize];
		}
		else
		{
			bool* pOldDataBuffer = m_pDataBuffer;
			m_pDataBuffer = new bool[iIndex<<1];
			memcpy(m_pDataBuffer, pOldDataBuffer, sizeof(bool) * m_iCurrentAllocatedSize);
			m_iCurrentAllocatedSize = iIndex<<1;
			m_iBiggestIndexUsed = iIndex;
		}
	}

	// Check the std::vector then and add values until we can put this index
	while(m_InputItems.size() <= iIndex)
	{
		BoolDataItem* item = new BoolDataItem();
		m_InputItems.push_back(item);
	}
	
	m_InputItems[iIndex]->SetValue(bValue);
}

void BoolVectorDataItem::SetValues(bool* pData, int iNumElems)
{
	m_InputItems.clear();
	for (int i = 0; i < iNumElems; i++)
	{
		BoolDataItem* pItem = new BoolDataItem();
		pItem->SetValue(pData[i]);
		m_InputItems.push_back(pItem);
	}
}

bool BoolVectorDataItem::GetValue(int iIndex)
{
	return m_InputItems[iIndex]->GetValue();
}

IDataTypeItem* BoolVectorDataItem::Clone()
{
	BoolVectorDataItem*	pCopy = new BoolVectorDataItem();
	CopyBase(pCopy);
	
	m_iBiggestIndexUsed = pCopy->m_iBiggestIndexUsed;
	m_iCurrentAllocatedSize = pCopy->m_iCurrentAllocatedSize;
	memcpy(m_pDataBuffer, pCopy->m_pDataBuffer, m_iCurrentAllocatedSize);

	int iIndex =0 ;
	for (ArrayOfBaseProcessBoolInputsIter it = m_InputItems.begin(); it != m_InputItems.end(); it++, iIndex++)
	{
		IDataTypeItem* pOriginalItem = *it;
		IDataTypeItem* pClonedItem = pOriginalItem->Clone(&m_pDataBuffer[iIndex]);
		m_InputItems.push_back((BoolDataItem*)pClonedItem);
	}

	return pCopy;
}

unsigned int BoolVectorDataItem::GetSerializedSize() { assert(false); return 0;}
void BoolVectorDataItem::Deserialize(Streams::BytesStreamReader &stream){ assert(false); return ;}
void BoolVectorDataItem::Serialize(Streams::BytesStreamWriter& stream) { assert(false); return ;}

//------------------------ BEGIN  CharVectorData item --------------------------------

void CharVectorDataItem::SetValue(unsigned int iIndex, char cValue)
{
	// Check the internal data buffer first
	if (m_iCurrentAllocatedSize <= iIndex)
	{
		if (m_pDataBuffer == NULL)
		{
			m_iBiggestIndexUsed = iIndex;
			m_iCurrentAllocatedSize = iIndex<<1;
			m_pDataBuffer = new char[m_iCurrentAllocatedSize];
		}
		else
		{
			char* pOldDataBuffer = m_pDataBuffer;
			m_pDataBuffer = new char[iIndex<<1];
			memcpy(m_pDataBuffer, pOldDataBuffer, sizeof(char) * m_iCurrentAllocatedSize);
			m_iCurrentAllocatedSize = iIndex<<1;
			m_iBiggestIndexUsed = iIndex;
		}
	}

	// Check the std::vector then and add values until we can put this index
	while(m_InputItems.size() <= iIndex)
	{
		CharDataItem* item = new CharDataItem();
		m_InputItems.push_back(item);
	}
	
	m_InputItems[iIndex]->SetValue(cValue);
}

void CharVectorDataItem::SetValues(char** pData, int iNumElems)
{
	m_InputItems.clear();
	m_InputItems.reserve(iNumElems);
	char *pIter = *pData;
	for (int i = 0; i < iNumElems; i++, pIter++)
	{
		CharDataItem* pItem = new CharDataItem();
		pItem->SetValue(*pIter);
		m_InputItems.push_back(pItem);
	}
}

char CharVectorDataItem::GetValue(int iIndex)
{
	return m_InputItems[iIndex]->GetValue();
}

IDataTypeItem* CharVectorDataItem::Clone()
{
	CharVectorDataItem*	pCopy = new CharVectorDataItem();
	CopyBase(pCopy);
	
	m_iBiggestIndexUsed = pCopy->m_iBiggestIndexUsed;
	m_iCurrentAllocatedSize = pCopy->m_iCurrentAllocatedSize;
	memcpy(m_pDataBuffer, pCopy->m_pDataBuffer, m_iCurrentAllocatedSize);

	int iIndex =0 ;
	for (ArrayOfBaseProcessCharInputsIter it = m_InputItems.begin(); it != m_InputItems.end(); it++, iIndex++)
	{
		IDataTypeItem* pOriginalItem = *it;
		IDataTypeItem* pClonedItem = pOriginalItem->Clone(&m_pDataBuffer[iIndex]);
		m_InputItems.push_back((CharDataItem*)pClonedItem);
	}

	return pCopy;
}

unsigned int CharVectorDataItem::GetSerializedSize() { assert(false); return 0;}
void CharVectorDataItem::Deserialize(Streams::BytesStreamReader &stream){ assert(false); return ;}
void CharVectorDataItem::Serialize(Streams::BytesStreamWriter& stream) { assert(false); return ;}

//------------------------ END  CharVectorData item --------------------------------

IDataTypeItem* BufferDataItem::Clone()
{
	BufferDataItem* pClonedBuffer = new BufferDataItem();
	CopyBase(pClonedBuffer);
	return pClonedBuffer;
}

void BufferDataItem::CopyValuesFrom(const BufferDataItem* pItem)
{
	m_iBufferSize = pItem->m_iBufferSize;
	memcpy(m_pData, pItem->m_pData, pItem->m_iBufferSize);
}

unsigned int BufferDataItem::GetSerializedSize()
{
	return m_iBufferSize + sizeof(int);
}

void BufferDataItem::Serialize(Streams::BytesStreamWriter& stream)
{
	SERIALIZE_CHECKER_INIT_OBJECT

	// Buffer size
	stream.WriteSimpleType<int>(m_iBufferSize);

	// Buffer data
	stream.WriteByteArray(m_pData, m_iBufferSize);

	SERIALIZE_CHECKER_END_OBJECT
}

void BufferDataItem::Deserialize(Streams::BytesStreamReader& stream)
{
	// [BUFFER SIZE | BUFFER_DATA]
	stream.ReadSimpleType<int>(m_iBufferSize);

	m_pData = new char[m_iBufferSize];
	stream.ReadByteArray(m_pData, m_iBufferSize);
}

void IntVectorDataItem::SetValue(unsigned int iIndex, int iValue)
{
	// Check the internal data buffer first
	if (m_iCurrentAllocatedSize <= iIndex)
	{
		if (m_pDataBuffer == NULL)
		{
			m_iBiggestIndexUsed = iIndex;
			m_iCurrentAllocatedSize = iIndex<<1;
			m_pDataBuffer = new int[m_iCurrentAllocatedSize];
		}
		else
		{
			int* pOldDataBuffer = m_pDataBuffer;
			m_pDataBuffer = new int[iIndex<<1];
			memcpy(m_pDataBuffer, pOldDataBuffer, sizeof(int) * m_iCurrentAllocatedSize);
			m_iCurrentAllocatedSize = iIndex<<1;
			m_iBiggestIndexUsed = iIndex;
		}
	}

	// Check the std::vector then and add values until we can put this index
	while(m_InputItems.size() <= iIndex)
	{
		IntDataItem* item = new IntDataItem(&m_pDataBuffer[iIndex]);
		m_InputItems.push_back(item);
	}

	m_InputItems[iIndex]->SetValue(iValue);
}

void IntVectorDataItem::SetValues(int* pData, int iNumElems)
{
	m_InputItems.clear();
	for (int i = 0; i < iNumElems; i++)
	{
		IntDataItem* pItem = new IntDataItem();
		pItem->SetValue(pData[i]);
		m_InputItems.push_back(pItem);
	}
}

int IntVectorDataItem::GetValue(int iIndex)
{
	return m_InputItems[iIndex]->GetValue();
}

IDataTypeItem* IntVectorDataItem::Clone()
{
	IntVectorDataItem*	pCopy = new IntVectorDataItem();
	CopyBase(pCopy);

	m_iBiggestIndexUsed = pCopy->m_iBiggestIndexUsed;
	m_iCurrentAllocatedSize = pCopy->m_iCurrentAllocatedSize;
	memcpy(m_pDataBuffer, pCopy->m_pDataBuffer, m_iCurrentAllocatedSize);

	int iIndex =0 ;
	for (ArrayOfBaseProcessIntInputsIter it = m_InputItems.begin(); it != m_InputItems.end(); it++, iIndex++)
	{
		IDataTypeItem* pOriginalItem = *it;
		IDataTypeItem* pClonedItem = pOriginalItem->Clone(&m_pDataBuffer[iIndex]);
		m_InputItems.push_back((IntDataItem*)pClonedItem);
	}

	return pCopy;
}

unsigned int IntVectorDataItem::GetSerializedSize()
{
	int iSize = sizeof(m_InputItems.size());
	return sizeof(int) * m_InputItems.size();
}

void IntVectorDataItem::Serialize(Streams::BytesStreamWriter& stream)
{
	unsigned int size = m_InputItems.size();
	stream.WriteSimpleType<unsigned int>(size);
	for (ArrayOfBaseProcessIntInputsIter it = m_InputItems.begin(); it != m_InputItems.end(); it++)
		(*it)->Serialize(stream);
}

void IntVectorDataItem::Deserialize(Streams::BytesStreamReader& stream)
{
	// [ NUMBER OF ITEMS | ITEM 0 | ITEM 1 | ..... ]
	unsigned int size = 0;
	stream.ReadSimpleType<unsigned int>(size);
	for (unsigned int i = 0; i < size; i++)
	{
		int temp;
		stream.ReadSimpleType<int>(temp);
		SetValue(i, temp);
	}
}

void FloatVectorDataItem::SetValue(unsigned int iIndex, float fValue)
{
	// Check the internal data buffer first
	if (m_iCurrentAllocatedSize <= iIndex)
	{
		if (m_pDataBuffer == NULL)
		{
			m_iBiggestIndexUsed = iIndex;
			m_iCurrentAllocatedSize = iIndex<<1;
			m_pDataBuffer = new float[m_iCurrentAllocatedSize];
		}
		else
		{
			float* pOldDataBuffer = m_pDataBuffer;
			m_pDataBuffer = new float[iIndex<<1];
			memcpy(m_pDataBuffer, pOldDataBuffer, sizeof(float) * m_iCurrentAllocatedSize);
			m_iCurrentAllocatedSize = iIndex<<1;
			m_iBiggestIndexUsed = iIndex;
		}
	}

	// Check the std::vector then and add values until we can put this index
	while(m_InputItems.size() <= iIndex)
	{
		FloatDataItem* item = new FloatDataItem();
		m_InputItems.push_back(item);
	}

	m_InputItems[iIndex]->SetValue(fValue);
}

void FloatVectorDataItem::SetValues(float* pData, int iNumElems)
{
	m_InputItems.clear();
	for (int i = 0; i < iNumElems; i++)
	{
		FloatDataItem* item = new FloatDataItem();
		item->SetValue(pData[i]);
		m_InputItems.push_back(item);
	}
}

float FloatVectorDataItem::GetValue(int iIndex)
{
	return m_InputItems[iIndex]->GetValue();
}

IDataTypeItem* FloatVectorDataItem::Clone()
{
	FloatVectorDataItem* pCopy = new FloatVectorDataItem();
	CopyBase(pCopy);

	m_iBiggestIndexUsed = pCopy->m_iBiggestIndexUsed;
	m_iCurrentAllocatedSize = pCopy->m_iCurrentAllocatedSize;
	memcpy(m_pDataBuffer, pCopy->m_pDataBuffer, m_iCurrentAllocatedSize);

	int iIndex =0 ;
	for (ArrayOfBaseProcessFloatInputsIter it = m_InputItems.begin(); it != m_InputItems.end(); it++, iIndex++)
	{
		IDataTypeItem* pOriginalItem = *it;
		IDataTypeItem* pClonedItem = pOriginalItem->Clone(&m_pDataBuffer[iIndex]);
		m_InputItems.push_back((FloatDataItem*)pClonedItem);
	}
	
	return pCopy;
}

unsigned int FloatVectorDataItem::GetSerializedSize()
{
	int iSize = sizeof(m_InputItems.size());
	return sizeof(int) * m_InputItems.size();
}

void FloatVectorDataItem::Serialize(Streams::BytesStreamWriter& stream)
{
	unsigned int size = m_InputItems.size();
	stream.WriteSimpleType<unsigned int>(size);
	for (ArrayOfBaseProcessFloatInputsIter it = m_InputItems.begin(); it != m_InputItems.end(); it++)
		(*it)->Serialize(stream);
}

void FloatVectorDataItem::Deserialize(Streams::BytesStreamReader& stream)
{
	// [ NUMBER OF ITEMS | ITEM 0 | ITEM 1 | ..... ]
	unsigned int size = 0;
	stream.ReadSimpleType<unsigned int>(size);
	for (unsigned int i = 0; i < size; i++)
	{
		float temp;
		stream.ReadSimpleType<float>(temp);
		SetValue(i, temp);
	}
}

void StringVectorDataItem::SetValue(unsigned int iIndex, char *szValue)
{	
	// Check the internal data buffer first
	if (m_iCurrentAllocatedSize <= iIndex)
	{
		if (m_pDataBuffer == NULL)
		{
			m_iBiggestIndexUsed = iIndex;
			m_iCurrentAllocatedSize = iIndex<<1;
			m_pDataBuffer = new char[m_iCurrentAllocatedSize];
		}
		else
		{
			char* pOldDataBuffer = m_pDataBuffer;
			m_pDataBuffer = new char[iIndex<<1];
			memcpy(m_pDataBuffer, pOldDataBuffer, sizeof(char) * m_iCurrentAllocatedSize);
			m_iCurrentAllocatedSize = iIndex<<1;
			m_iBiggestIndexUsed = iIndex;
		}
	}

	// Check the std::vector then and add values until we can put this index
	while(m_InputItems.size() <= iIndex)
	{
		StringDataItem* item = new StringDataItem();
		m_InputItems.push_back(item);
	}

	m_InputItems[iIndex]->SetValue(szValue);
}

void StringVectorDataItem::SetValues(char** pData, int iNumElems)
{
	m_InputItems.clear();
	for (int i = 0; i < iNumElems; i++)
	{
		StringDataItem* pItem = new StringDataItem();
		pItem->SetValue(pData[i]);
		m_InputItems.push_back(pItem);
	}
}

char* StringVectorDataItem::GetValue(int iIndex)
{
	return m_InputItems[iIndex]->GetValue();
}

IDataTypeItem* StringVectorDataItem::Clone()
{
	StringVectorDataItem* pCopy = new StringVectorDataItem();
	CopyBase(pCopy);

	m_iBiggestIndexUsed = pCopy->m_iBiggestIndexUsed;
	m_iCurrentAllocatedSize = pCopy->m_iCurrentAllocatedSize;
	memcpy(m_pDataBuffer, pCopy->m_pDataBuffer, m_iCurrentAllocatedSize);

	int iIndex =0 ;
	for (ArrayOfBaseProcessStringInputsIter it = m_InputItems.begin(); it != m_InputItems.end(); it++, iIndex++)
	{
		IDataTypeItem* pOriginalItem = *it;
		IDataTypeItem* pClonedItem = pOriginalItem->Clone(&m_pDataBuffer[iIndex]);
		m_InputItems.push_back((StringDataItem*)pClonedItem);
	}

	return pCopy;
}

unsigned int StringVectorDataItem::GetSerializedSize()
{
	assert(false);
	return 0;
}

void StringVectorDataItem::Serialize(Streams::BytesStreamWriter &stream)
{
	assert(false);
	return;
}

void StringVectorDataItem::Deserialize(Streams::BytesStreamReader &stream)
{
	assert(false);
	/*
	// [Number of items in vector | item 0 | item 1 | .... item n]
	int iNumItems = 0;
	stream.ReadSimpleType<int>(iNumItems);

	for (int i = 0; i < iNumItems; i+)
	*/
		
}

IDataTypeItem* ItemTypeFactory::CreateInputItem(DataTypes eDataType, const char *szName)
{
	IDataTypeItem *pItem = NULL;
	switch(eDataType)
	{
	case ETYPE_BOOL:
		pItem = new BoolDataItem();
		break;
	case ETYPE_ARRAY_BOOL:
		pItem = new BoolVectorDataItem();
		break;
	case ETYPE_CHAR:
		pItem = new CharDataItem();
		break;
	case ETYPE_ARRAY_CHAR:
		pItem = new CharVectorDataItem();
		break;
	case ETYPE_INT:
		pItem = new IntDataItem();
		break;
	case ETYPE_ARRAY_INT:
		pItem = new IntVectorDataItem();
		break;
	case ETYPE_FLOAT:
		pItem = new FloatDataItem();
		break;
	case ETYPE_ARRAY_FLOAT:
		pItem = new FloatVectorDataItem();
		break;
	case ETYPE_STRING:
		pItem = new StringDataItem();
		break;
	case ETYPE_DATA_BUFFER:
		pItem = new BufferDataItem();
		break;
	case ETYPE_ARRAY_STRING:
		pItem = new StringVectorDataItem();
		break;
	default:
		assert(false && "Unknown type given! ");
		return NULL;
	}

	pItem->SetName(szName);
	return pItem;
}

void ItemTypeFactory::DestroyDataInputItem(IDataTypeItem *pInputItem)
{
	delete pInputItem;
}

void BaseProcessInput::CopyBase(BaseProcessInput* pCloned)
{
	pCloned->m_eType = m_eType;
}

bool SimpleProcessItem::VerifyInputMatching(const BaseProcessInput *oth, bool bPerfectMatching) const
{
	SimpleProcessItem* otherInput = (SimpleProcessItem*) oth;

	if (m_InputItems.size() < otherInput->m_InputItems.size())
		return 0;

	if (bPerfectMatching && m_InputItems.size() != otherInput->m_InputItems.size())
		return 0;

	for (ListOfInputItemsConstIter it = m_InputItems.begin(), it2 = otherInput->m_InputItems.begin(); it != m_InputItems.end() && it2 != otherInput->m_InputItems.end(); it++, it2++)
	{
		if ((*it)->m_eDataType != (*it2)->m_eDataType)
			return false;
	}

	return true;
}

bool SimpleProcessItem::SendProcessInput(SimpleProcessItem &otherInput) const
{
	if (m_InputItems.size() < otherInput.m_InputItems.size())
		return 0;

	for (ListOfInputItemsConstIter it = m_InputItems.begin(), it2 = otherInput.m_InputItems.begin(); it != m_InputItems.end() && it2 != m_InputItems.end(); it++, it2++)
	{
		if ((*it)->m_eDataType != (*it2)->m_eDataType)
			return false;

		(*it2)->Equals(*it);
	}

	return true;
}

void SimpleProcessItem::AddItem(DataTypes eDataType, char *szName)
{
	IDataTypeItem* pItem = ItemTypeFactory::CreateInputItem(eDataType, szName);
	m_InputItems.push_back(pItem);

	pItem->m_pParent = this;
}

void SimpleProcessItem::AddItem(IDataTypeItem* pInputItem)
{
	m_InputItems.push_back(pInputItem);
	pInputItem->m_pParent = this;
}

void SimpleProcessItem::SetItemValue(int index, DataTypes eDataType, const char* szName, int value)
{	 
	MakeAvailablePos(index);

	IDataTypeItem* pItem = ItemTypeFactory::CreateInputItem(eDataType, szName);
	pItem->m_pParent = this;
	((IntDataItem*)pItem)->SetValue(value);

	m_InputItems[index] = pItem;
}

void SimpleProcessItem::SetItemValue(int index, DataTypes eDataType, const char* szName, float value)
{
	MakeAvailablePos(index);

	IDataTypeItem* pItem = ItemTypeFactory::CreateInputItem(eDataType, szName);
	pItem->m_pParent = this;
	((FloatDataItem*)pItem)->SetValue(value);

	m_InputItems[index] = pItem;
}

void SimpleProcessItem::SetItemValue(int index, DataTypes eDataType, const char* szName, char* value)
{
	MakeAvailablePos(index);

	IDataTypeItem* pItem = ItemTypeFactory::CreateInputItem(eDataType, szName);
	pItem->m_pParent = this;
	((StringDataItem*)pItem)->SetValue(value);

	m_InputItems[index] = pItem;
}

void SimpleProcessItem::SetItemValue(int index, DataTypes eDataType, const char* szName, char* value, int buffSize)
{
	MakeAvailablePos(index);

	IDataTypeItem* pItem = ItemTypeFactory::CreateInputItem(eDataType, szName);
	pItem->m_pParent = this;
	((BufferDataItem*)pItem)->SetValue(value, buffSize);

	m_InputItems[index] = pItem;
}

void SimpleProcessItem::PrintDebugInfo(int iSpaceLeft) const
{
	PutsEmpty(iSpaceLeft);
	printf("--- BEGIN: SIMPLE PROCESS ITEMS DEBUG ---\n");

	PutsEmpty(iSpaceLeft);printf("There are %d small inputs here \n", m_InputItems.size());
	for (ListOfInputItemsConstIter it = m_InputItems.begin(); it != m_InputItems.end(); it++)
	{
		PutsEmpty(iSpaceLeft); (*it)->PrintDebugInfo(iSpaceLeft + 2);
	}

	PutsEmpty(iSpaceLeft);printf("--- END: SIMPLE PROCESS ITEMS DEBUG ---\n");
}

bool SimpleProcessItem::Validate(SymbolTable* pSymbolTable, bool bAtRuntimeSpawn)
{
	for (ListOfInputItemsConstIter it = m_InputItems.begin(); it != m_InputItems.end(); it++)
	{
		IDataTypeItem* pItemType = (IDataTypeItem*) *it;
		if (!pSymbolTable->AddSymbol((char*)pItemType->GetName(), pItemType))
			return false;
	}

	return true;
}

bool SimpleProcessItem::SetValues(BaseProcessInput* pInput)
{
	if (pInput->GetType() == E_INPUT_ARRAY)
	{
		assert(false);
		return false;
	}

	SimpleProcessItem* pSPInput = static_cast<SimpleProcessItem*>(pInput);
	for (ListOfInputItemsIter it = m_InputItems.begin(), itInput = pSPInput->m_InputItems.begin(); it != m_InputItems.end(); it++, itInput++)
	{
		IDataTypeItem* pItem = *it;
		pItem->Equals(*itInput);
		pItem->FireProgramListeners();
	}

	return true;
}

BaseProcessInput* SimpleProcessItem::Clone()
{
	SimpleProcessItem* pCloned = new SimpleProcessItem();
	CopyBase(pCloned);
	
	for (ListOfInputItemsConstIter it = m_InputItems.begin(); it != m_InputItems.end(); it++)
	{
		IDataTypeItem* pOriginalDataItem = *it;
		IDataTypeItem* pClonedDataItem = pOriginalDataItem->Clone();
		pCloned->AddItem(pClonedDataItem);
	}

	return pCloned;
}

unsigned int SimpleProcessItem::GetSerializedSize()
{
	unsigned int size = 0;

	//size += sizeof(size_t);

	for (ListOfInputItemsIter it = m_InputItems.begin(); it != m_InputItems.end(); it++)
		size += (*it)->GetSerializedSize();

	return size;
}

void SimpleProcessItem::Serialize(Streams::BytesStreamWriter& stream)
{
	SERIALIZE_CHECKER_INIT_OBJECT

	unsigned int size = 0;
	//stream.WriteSimpleType<size_t>(m_InputItems.size());

	for (ListOfInputItemsIter it = m_InputItems.begin(); it != m_InputItems.end(); it++)
	{
		// Serialize the type then the entire item. Don't save the name - i don't think we need it at this point

		(*it)->Serialize(stream);
	}

	SERIALIZE_CHECKER_END_OBJECT
}

void SimpleProcessItem::Deserialize(Streams::BytesStreamReader& stream)
{
	SERIALIZE_CHECKER_INIT_OBJECT

	// [/*NUMBER_OF_ITEMS(INT) |*/  ITEM0 | ITEM1 | ITEM 2| .... | ITEM LAST]
	for (ListOfInputItemsIter it = m_InputItems.begin(); it != m_InputItems.end(); it++)
	{
		// Deserialize the type then the entire item. Don't save the name - i don't think we need it at this point
		//char dataType = 0;
		//stream.ReadSimpleType<char>(dataType);
		//IDataTypeItem* pItem = ItemTypeFactory::CreateInputItem(dataType);


		(*it)->Deserialize(stream);
	}

	SERIALIZE_CHECKER_END_OBJECT
}

inline int SimpleProcessItem::GetSerializedDataTypeSize()
{
	// [SIZE | NUMBER_OF_ITEMS(INT) | ITEM0 | ITEM1 | ITEM 2| .... | ITEM LAST]
	unsigned int totalSize = sizeof(m_InputItems.size()) + sizeof(int);
	for (ListOfInputItemsIter it = m_InputItems.begin(); it != m_InputItems.end(); it++)
	{
		totalSize += IDataTypeItem::GetSerializedDataTypeSize(*it);
	}

	return totalSize;
}

void SimpleProcessItem::SerializeAsDataType(Streams::BytesStreamWriter& stream)
{
	const int expectedSize = GetSerializedDataTypeSize();
	CSF_INIT_SERIALIZE_CHECK

	stream.WriteSimpleType(m_InputItems.size());
	for (ListOfInputItemsIter it = m_InputItems.begin(); it != m_InputItems.end(); it++)
	{
		const IDataTypeItem* dataItem = *it;
		IDataTypeItem::SerializeDataType(stream, dataItem);
	}

	CSF_END_SERIALIZE_CHECK
}

void SimpleProcessItem::DeserializeAsDataType(Streams::BytesStreamReader& stream)
{
	CSF_INIT_DESERIALIZE_CHECK

	int numItems = 0;
	stream.ReadSimpleType(numItems);
	for (int itemIter = 0; itemIter < numItems; itemIter++)
	{
		DataTypes dataType = 0;
	
		IDataTypeItem* dataTypeItem = IDataTypeItem::DeserializeDataType(stream);
		AddItem(dataTypeItem);
	}

	CSF_END_DESERIALIZE_CHECK
}

void VectorProcessItem::AddItemInput(ArrayOfBaseProcessInputs& input)
{
	int iItemsCount = 0;
	for (ArrayOfBaseProcessInputsIter it = input.begin(); it != input.end(); it++)
	{
		SimpleProcessItem *pSI = static_cast<SimpleProcessItem*>(*it);
		pSI->m_pParentVector = this;
		pSI->SetParentBlock(m_pParentBlock);
		pSI->m_iParentVectorIndex = m_ArrayOfItemInputs.size();
		pSI->m_iIndexInBlock = iItemsCount;
		iItemsCount++;
	}

	m_ArrayOfItemInputs.push_back(input);
}

void VectorProcessItem::ClearAll()
{
	m_ArrayOfItemInputs.clear();
}

ArrayOfBaseProcessInputs& VectorProcessItem::GetVectorItemByIndex(unsigned int iIndex)
{
	//assert(iIndex < m_ArrayOfItemInputs.size() && "Invalid index request");
	while(m_ArrayOfItemInputs.size() <= iIndex)
	{
		ArrayOfBaseProcessInputs arrayOfProcesses;
		CloneAndCopyArrayOfBaseProcessInputs(arrayOfProcesses, m_TypeOfArrayItems);		
		AddItemInput(arrayOfProcesses);
	}

	return m_ArrayOfItemInputs[iIndex];
}

/*
const SimpleProcessItem& VectorProcessItem::GetValue(int position)
{
	return *m_ArrayOfItemInputs[position];
}
*/

void VectorProcessItem::PrintDebugInfo(int iSpaceLeft) const
{
	PutsEmpty(iSpaceLeft);
	printf("--- BEGIN: VECTOR OF PROCESSES DEBUG ---\n");
	PutsEmpty(iSpaceLeft + 2); printf("There are %d items currently \n", m_ArrayOfItemInputs.size());
	PutsEmpty(iSpaceLeft + 2); printf("The type of an item is: \n"); 
	for (ArrayOfBaseProcessInputsConstIter it = m_TypeOfArrayItems.begin(); it != m_TypeOfArrayItems.end(); it++)
		(*it)->PrintDebugInfo(iSpaceLeft + 2);
	
	PutsEmpty(iSpaceLeft + 2);printf("--- END: VECTOR OF PROCESSES DEBUG ---\n");
}

bool VectorProcessItem::VerifyInputMatching(const BaseProcessInput* otherInput, bool bPerfectMatching /* = false */)
{   
	VectorProcessItem* otherVector = (VectorProcessItem*) otherInput;

	// Verify if they have the same input type
	const ArrayOfBaseProcessInputs& otherVectorType = otherVector->GetItemType();
	if (m_TypeOfArrayItems.size() != otherVectorType.size())
		return false;
	
	for (ArrayOfBaseProcessInputsConstIter itThis = m_TypeOfArrayItems.begin(), itOther = otherVectorType.begin(); 
			itThis != m_TypeOfArrayItems.end(); itThis++, itOther++)
	{
		if (!(*itThis)->VerifyInputMatching((*itOther)))
		{
			ProgramBase* otherParentProgram = otherVector->GetParentBlock()->m_pParentProgram;
			ProgramBase* thisParentProgram = this->GetParentBlock()->m_pParentProgram; 
			PrintCompileError(thisParentProgram->GetLineNo(), "two vectors type doesn't match between parent programs: %s and %s. Verify the type composition between these", otherParentProgram->GetName(), thisParentProgram->GetName());
			return false;
		}
	}

	return true;
}

BaseProcessInput* VectorProcessItem::Clone()
{
	VectorProcessItem* pCloned = new VectorProcessItem(NULL);
	CopyBase(pCloned);

	pCloned->m_szName = _strdup(m_szName);
	
	pCloned->m_TypeOfArrayItems.clear();
	for (ArrayOfBaseProcessInputsIter it = m_TypeOfArrayItems.begin(); it != m_TypeOfArrayItems.end(); it++)
		pCloned->m_TypeOfArrayItems.push_back((*it)->Clone());
	pCloned->m_NumProcessesInItemType = m_NumProcessesInItemType;

	for (ArrayOfItemsProcessInputsIter it = m_ArrayOfItemInputs.begin(); it != m_ArrayOfItemInputs.end(); it++)
	{	
		ArrayOfBaseProcessInputs& pOriginalItem = *it;
		ArrayOfBaseProcessInputs pClonedItem;
		
		for (ArrayOfBaseProcessInputsConstIter it = pOriginalItem.begin(); it != pOriginalItem.end(); it++)
		{
			BaseProcessInput* pBPI = (*it)->Clone();
			pClonedItem.push_back(pBPI);
		}

		pCloned->AddItemInput(pClonedItem);
	}

	pCloned->m_MapItemNameToIndex.insert(m_MapItemNameToIndex.begin(), m_MapItemNameToIndex.end());

	return pCloned;
}

bool VectorProcessItem::SetItemType(const ArrayOfBaseProcessInputs& inputInterface, const char* szName)
{ 
	m_TypeOfArrayItems.clear();
	int indexInProcessesList = 0;
	int indexInProcessItems = 0;
	for (ArrayOfBaseProcessInputsConstIter it = inputInterface.begin(); it != inputInterface.end(); it++)
	{
		BaseProcessInput* pProcessInput = (*it); 				
		if (pProcessInput->GetType() == E_INPUT_SIMPLE)
		{
			indexInProcessItems = 0;
			SimpleProcessItem* pSPI = (SimpleProcessItem*) pProcessInput;
			for (ListOfInputItemsIter it = pSPI->m_InputItems.begin(); it != pSPI->m_InputItems.end(); it++)
			{				
				m_MapItemNameToIndex[std::string((*it)->GetName())] = std::make_pair(indexInProcessesList, indexInProcessItems);
				indexInProcessItems++;
			}
		}
		else
		{
		}

		indexInProcessesList++;
		m_TypeOfArrayItems.push_back(pProcessInput->Clone());
	}

	m_NumProcessesInItemType = m_TypeOfArrayItems.size();

	assert(strlen(szName) < MAX_IDENTIFIER_SIZE);
	m_szName = _strdup(szName);

	return false;
}

std::pair<int,int>  VectorProcessItem::GetIndexOfItemName(const char* szItemName)
{
	MapMemberToIndexIter it = m_MapItemNameToIndex.find(std::string(szItemName));
	if (it == m_MapItemNameToIndex.end())
		return std::make_pair(-1,-1);

	return it->second;
}

bool VectorProcessItem::SetValues(BaseProcessInput* pInput)
{
	if (pInput->GetType() == E_INPUT_SIMPLE)
	{
		assert(false);
		return false;
	}

	VectorProcessItem* pVectorInput = static_cast<VectorProcessItem*>(pInput);
	for (unsigned int i = 0; i < pVectorInput->m_ArrayOfItemInputs.size(); i++)
	{
		ArrayOfBaseProcessInputs& arrayOfProcesses_input = pVectorInput->GetVectorItemByIndex(i);
		ArrayOfBaseProcessInputs& arrayOfProcesses_this = GetVectorItemByIndex(i); 	
		// At this point they should be verified so both should vectors should have the same interface

		for (ArrayOfBaseProcessInputsIter it_input = arrayOfProcesses_input.begin(), it_this = arrayOfProcesses_this.end();
				it_input != arrayOfProcesses_input.end(); it_input++, it_this++)
		{
			BaseProcessInput* pBPInput = *it_input;
			BaseProcessInput* pBPOutput = *it_this;

			pBPOutput->SetValues(pBPInput);
		}
	}

	return true;
}

unsigned int VectorProcessItem::GetSerializedSize()
{
	unsigned int size = sizeof(m_ArrayOfItemInputs.size());	//
	for (ArrayOfItemsProcessInputsIter it = m_ArrayOfItemInputs.begin(); it != m_ArrayOfItemInputs.end(); it++)
	{
		size += sizeof(it->size());
		for (ArrayOfBaseProcessInputsConstIter itProcessItems = it->begin(); itProcessItems != it->end(); itProcessItems++)
		{
			size += sizeof(char);	// The size of type
			size += (*itProcessItems)->GetSerializedSize();
		}
	}

	return size;
}

void VectorProcessItem::Serialize(Streams::BytesStreamWriter &stream)
{
	SERIALIZE_CHECKER_INIT_OBJECT

	unsigned int iVectorSize = m_ArrayOfItemInputs.size();
	stream.WriteSimpleType<unsigned int>(iVectorSize);
	for (ArrayOfItemsProcessInputsIter it = m_ArrayOfItemInputs.begin(); it != m_ArrayOfItemInputs.end(); it++)
	{
		for (ArrayOfBaseProcessInputsConstIter processIt = it->begin(); processIt != it->end(); processIt++)
		{
			(*processIt)->Serialize(stream);
		}
	}

	SERIALIZE_CHECKER_END_OBJECT
}

void VectorProcessItem::Deserialize(Streams::BytesStreamReader &stream)
{
	SERIALIZE_CHECKER_INIT_OBJECT

	ClearAll();

	// [NUMBER OF SIMPLE PROCESS ITEMS | ITEM 0 | ITEM 1 |.... | ITEM N]
	unsigned int iNumberOfItems = 0;
	stream.ReadSimpleType<unsigned int>(iNumberOfItems);
	unsigned int iNrOfProcessesperItem = m_TypeOfArrayItems.size();
	for (unsigned int i = 0; i < iNumberOfItems; i++)
	{
		ArrayOfBaseProcessInputs& arrayOfProcesses = GetVectorItemByIndex(i);
		for (unsigned int j = 0; j < iNrOfProcessesperItem; j++)
		{
			arrayOfProcesses[j]->Deserialize(stream);
		}
	}

	SERIALIZE_CHECKER_END_OBJECT
}

void VectorProcessItem::SerializeAsDataType(Streams::BytesStreamWriter& stream)
{
	// [SIZE | STRING NAME | ARRAY TYPE]
	const int expectedSize = GetSerializedDataTypeSize();
	CSF_INIT_SERIALIZE_CHECK

	stream.WriteString(m_szName);
	CodeSerializationFactory::SerializeDataTypeArrayOfBaseProcess(m_TypeOfArrayItems, stream);

	CSF_END_SERIALIZE_CHECK
}

int VectorProcessItem::GetSerializedDataTypeSize()
{
	// [SIZE | STRING | ARRAY TYPE]
	int totalSize = sizeof(int);
	totalSize += Streams::GetStringSizeOnStream(m_szName);
	totalSize += CodeSerializationFactory::GetSerializedDataSizeArrayOfBaseProcess(m_TypeOfArrayItems);
	return totalSize;
}

void VectorProcessItem::DeserializeAsDataType(Streams::BytesStreamReader& stream)
{
	CSF_INIT_DESERIALIZE_CHECK

	char buff[1024];
	stream.ReadString(buff, 1024);
	m_szName = _strdup(buff);

	ArrayOfBaseProcessInputs typeDeserialized;
	CodeSerializationFactory::DeserializeDataTypeArrayOfBaseProcess(typeDeserialized, stream);
	SetItemType(typeDeserialized, m_szName);

	CSF_END_DESERIALIZE_CHECK
}

void CloneAndCopyArrayOfBaseProcessInputs(ArrayOfBaseProcessInputs& dest, const ArrayOfBaseProcessInputs& source)
{
	dest.clear();
	int iItemsCount = 0;
	for (ArrayOfBaseProcessInputsConstIter it = source.begin(); it != source.end(); it++)
	{
		BaseProcessInput* pBPI = (*it)->Clone();
		pBPI->m_iIndexInBlock = iItemsCount++;
		dest.push_back(pBPI);
	}
}

void CopyNullArrayOfBaseProcessInputs(ArrayOfBaseProcessInputs& dest, const ArrayOfBaseProcessInputs& source)
{
	dest.clear();
	//int iItemsCount = 0;
	for (ArrayOfBaseProcessInputsConstIter it = source.begin(); it != source.end(); it++)
	{
		BaseProcessInput* pBPI = NULL;
		//pBPI->m_iParentItemIndex = iItemsCount++;
		dest.push_back(pBPI);
	}
}

BaseProcessInput* InputBlock::GetNextInput()
{
	if (m_NextInputToMatch == m_InputsInBlock.end())
		return NULL;

	return (*m_NextInputToMatch);
	m_NextInputToMatch++;
}

void InputBlock::AddInput(BaseProcessInput *pProcessInput)
{
	pProcessInput->SetParentBlock(this);
	
	pProcessInput->m_iIndexInBlock = m_InputsInBlock.size();
	m_InputsInBlock.push_back(pProcessInput);

	OnFinishedInputLoading();
}

void InputBlock::AddInputs(InputBlock* pOtherBlock)
{
	for (ArrayOfBaseProcessInputsConstIter it = pOtherBlock->m_InputsInBlock.begin(); it != pOtherBlock->m_InputsInBlock.end(); it++)
	{
		BaseProcessInput* pOriginal = *it;
		BaseProcessInput* pClonedBaseProcessInput = pOriginal->Clone();
		AddInput(pClonedBaseProcessInput);
	}
}

void InputBlock::OnFinishedInputLoading()
{
	m_NextInputToMatch = m_InputsInBlock.begin();
}

void InputBlock::PrintDebugInfo(int iSpaceLeft) const
{
	PutsEmpty(iSpaceLeft);
	printf("--- BEGIN: INPUT BLOCK DEBUG ---\n");
	PutsEmpty(iSpaceLeft);printf("There are %d inputs on this block\n", m_InputsInBlock.size());
	for (ArrayOfBaseProcessInputsConstIter it = m_InputsInBlock.begin(); it != m_InputsInBlock.end(); it++)
	{
		PutsEmpty(iSpaceLeft); (*it)->PrintDebugInfo(iSpaceLeft + 2);
	}
	PutsEmpty(iSpaceLeft);printf("--- END: INPUT BLOCK DEBUG ---\n");
}

InputBlock* InputBlock::Clone()
{
	InputBlock* pClonedBlock = new InputBlock();
	for (ArrayOfBaseProcessInputsConstIter it = m_InputsInBlock.begin(); it != m_InputsInBlock.end(); it++)
	{
		BaseProcessInput* pOriginal = *it;
		BaseProcessInput* pClonedBaseProcessInput = pOriginal->Clone();
		pClonedBaseProcessInput->SetParentBlock(this);

		pClonedBlock->AddInput(pClonedBaseProcessInput);
	}

	pClonedBlock->m_bIsNorthBlock = m_bIsNorthBlock;

	return pClonedBlock;
}

void SetSimpleLinkForInputs(SimpleProcessItem *pParent, SimpleProcessItem* pChild)
{
	pParent->m_pNext = pChild;
	pChild->m_pFrom = pParent;
}

/*
void SetVectorParentToSimpleChildInput(VectorProcessItem* pParent, unsigned int iIndex, SimpleProcessItem *pChild)
{
	BaseProcessInput::SetOrientedLink(pParent->GetArrayItemByIndex(iIndex), pChild);
}

void SetSimpleParentToVectorChild(SimpleProcessItem* pParent, VectorProcessItem *pChild, unsigned int iIndex)
{
	BaseProcessInput::SetOrientedLink(pParent, pChild->GetArrayItemByIndex(iIndex));	
}
*/

void SetVectorParentToVectorChild(VectorProcessItem* pParent, int iIndexParent, VectorProcessItem* pChild, int iIndexChild)
{
	BaseProcessInput::SetOrientedLink(pParent, pChild);

	pParent->m_iToIndexRule = iIndexChild;
	pParent->m_iFromIndexRule = iIndexParent;
}

void PRINT_MODULES_NOT_MATCHING_CORRECTLY(InputBlock* pThis, InputBlock* pChild1, InputBlock* pChild2)
{
	char const* child1Program = pChild1 ? pChild1->m_pParentProgram->GetName() : "NONE";
	char const* child2Program = pChild2 ? pChild2->m_pParentProgram->GetName() : "NONE";
	char const* thisProgram = pThis ? pThis->m_pParentProgram->GetName() : "NONE";

	PrintCompileError(pThis->m_pParentProgram->GetLineNo(), "Invalid input matching between composition inside module %s of modules %s and %s", 
		thisProgram,
		child1Program, 
		child2Program);
}

bool InputBlock::CheckAndLinkInputs(InputBlock *pChild1, InputBlock *pChild2, bool bChildsAsOutputs)
{
	const int iInputsCount = m_InputsInBlock.size();

	// If not output to match it should matter
	// Should we give an warning if an output doesn't have any linkage ?
	if (iInputsCount == 0)
		return true;

	int iNextInputToMatch = 0;
	
	unsigned int iThisBlockIndexInArray = 0;	// Index in vector array for this input block
	unsigned int iThisBlockSubIndexInType = 0;	// Sub index in a vector, iterating through its item type interface

	unsigned int iOtherBlockIndexInArray = 0;	// Index in vector array for child
	unsigned int iOtherBlockSubIndexInType = 0;	// Sub index in a vector, iterating through its item type interface

	InputBlock* pChilds[2] = { pChild1 , pChild2 };
	for (int iChildIter = 0; iChildIter < 2; iChildIter++)
	{
		if (pChilds[iChildIter] == NULL) continue;
		ArrayOfBaseProcessInputs& pInputToMatch = pChilds[iChildIter]->m_InputsInBlock;
		for (unsigned int i = 0; i < pInputToMatch.size(); i++)
		{
			if (iNextInputToMatch == iInputsCount)
			{
				PRINT_MODULES_NOT_MATCHING_CORRECTLY(this, pChild1, pChild2);
				return false;
			}

			if (pInputToMatch[i]->GetType() == E_INPUT_SIMPLE)
			{
				SimpleProcessItem* pOutputProcess = (SimpleProcessItem*) pInputToMatch[i];
				if (m_InputsInBlock[iNextInputToMatch]->GetType() == E_INPUT_SIMPLE)
				{
					// Types must match !
					SimpleProcessItem* pInputProcess = (SimpleProcessItem*) m_InputsInBlock[iNextInputToMatch];
					if (!pInputProcess->VerifyInputMatching(pOutputProcess)) 
					{
						PRINT_MODULES_NOT_MATCHING_CORRECTLY(this, pChild1, pChild2);
						return false;
					}

					if (bChildsAsOutputs) SetSimpleLinkForInputs(pInputProcess, pOutputProcess);
					else				  SetSimpleLinkForInputs(pOutputProcess, pInputProcess);
					
					iNextInputToMatch++;
				}
				else // It's an array !
				{
					VectorProcessItem* pInputVectorProcesses = (VectorProcessItem*) m_InputsInBlock[iNextInputToMatch];
					ArrayOfBaseProcessInputs& arrayItem = pInputVectorProcesses->GetVectorItemByIndex(iThisBlockIndexInArray);
					if (arrayItem.size() <= iThisBlockIndexInArray)
					{
						PRINT_MODULES_NOT_MATCHING_CORRECTLY(this, pChild1, pChild2);
						return false;
					}

					SimpleProcessItem* pSPItem = (SimpleProcessItem*)arrayItem[iThisBlockSubIndexInType];
					if (pSPItem->GetType() != E_INPUT_SIMPLE)
					{
						PRINT_MODULES_NOT_MATCHING_CORRECTLY(this, pChild1, pChild2);
						return false;
					}

					if (pSPItem->VerifyInputMatching(pOutputProcess))
					{
						if (bChildsAsOutputs) SetSimpleLinkForInputs(pSPItem, pOutputProcess);	//SetVectorParentToSimpleChildInput(pInputVectorProcesses, iThisBlockIndexInArray, iThisBlockSubIndexInType, pOutputProcess);
						else				  SetSimpleLinkForInputs(pOutputProcess, pSPItem);	//SetSimpleParentToVectorChild(pOutputProcess, pInputVectorProcesses, iThisBlockIndexInArray, iThisBlockSubIndexInType);

						iThisBlockSubIndexInType++;
						if (iThisBlockSubIndexInType == pInputVectorProcesses->GetNumItemsInType())
						{
							iThisBlockSubIndexInType = 0;
							iThisBlockIndexInArray++;
						}
					}
					else // If they don't match
					{			
						// If the array wasn't matched at least once or it stopped matching in the middle of the type matching, error
						if (iThisBlockSubIndexInType != 0 || iThisBlockIndexInArray == 0) 
						{
							PRINT_MODULES_NOT_MATCHING_CORRECTLY(this, pChild1, pChild2);
							return false;
						}

						// Continue, but go to the next input to match
						iThisBlockIndexInArray = 0;
						iThisBlockSubIndexInType = 0;
						iNextInputToMatch++;
						i--;
						continue;
					}
				}
			}
			else // the output is an array !
			{
				VectorProcessItem* pChildInputVector = (VectorProcessItem*) pInputToMatch[i];
				if (m_InputsInBlock[iNextInputToMatch]->GetType() == E_INPUT_SIMPLE)
				{
					//----- Get the subcomponent
					ArrayOfBaseProcessInputs& arrayItem = pChildInputVector->GetVectorItemByIndex(iOtherBlockIndexInArray);
					if (arrayItem.size() <= iThisBlockIndexInArray)
					{
						PRINT_MODULES_NOT_MATCHING_CORRECTLY(this, pChild1, pChild2);
						return false;
					}

					SimpleProcessItem* pSPItem = (SimpleProcessItem*)arrayItem[iOtherBlockSubIndexInType];
					if (pSPItem->GetType() != E_INPUT_SIMPLE)
					{
						PRINT_MODULES_NOT_MATCHING_CORRECTLY(this, pChild1, pChild2);
						return false;					
					}
					//----- End get the subcomponent

					// They match ?
					if (m_InputsInBlock[iNextInputToMatch]->VerifyInputMatching(pSPItem))
					{						 
						if (bChildsAsOutputs) SetSimpleLinkForInputs((SimpleProcessItem*)m_InputsInBlock[iNextInputToMatch], pSPItem);//SetSimpleParentToVectorChild((SimpleProcessItem*)m_InputsInBlock[iNextInputToMatch], pChildInputVector, iOtherBlockIndexInArray);
						else				  SetSimpleLinkForInputs(pSPItem, (SimpleProcessItem*)m_InputsInBlock[iNextInputToMatch]);//SetVectorParentToSimpleChildInput(pChildInputVector, iOtherBlockIndexInArray, (SimpleProcessItem*)m_InputsInBlock[iNextInputToMatch]);

						iOtherBlockSubIndexInType++;
						if (iOtherBlockSubIndexInType == pChildInputVector->GetNumItemsInType())
						{
							iOtherBlockSubIndexInType = 0;
							iOtherBlockIndexInArray++;
						}

						i--;
						iNextInputToMatch++;
					}
					else
					{
						if (iOtherBlockSubIndexInType != 0 || iOtherBlockIndexInArray == 0) 
						{	
							PRINT_MODULES_NOT_MATCHING_CORRECTLY(this, pChild1, pChild2);
							return false;
						}
						
						iOtherBlockIndexInArray = 0;
					}
				}
				else	// Array to array!
				{					
					if (!pChildInputVector->VerifyInputMatching((VectorProcessItem*)m_InputsInBlock[iNextInputToMatch])) 
					{
						PRINT_MODULES_NOT_MATCHING_CORRECTLY(this, pChild1, pChild2);
						return false;
					}

					if (bChildsAsOutputs) SetVectorParentToVectorChild((VectorProcessItem*)m_InputsInBlock[iNextInputToMatch], iThisBlockIndexInArray, pChildInputVector, iOtherBlockIndexInArray);
					else				  SetVectorParentToVectorChild(pChildInputVector, iOtherBlockIndexInArray, (VectorProcessItem*)m_InputsInBlock[iNextInputToMatch], iThisBlockIndexInArray);

					iNextInputToMatch++;
				}
			}
		}
	}

	return true;
}

InputBlock* InputBlock::CreateAndLinkInputBlock(InputBlock* pChild1, InputBlock *pChild2, bool bAsInputToChilds)
{
	InputBlock* pInputBlock = new InputBlock();
	int iIdx = 0;

	InputBlock* pChilds[2] = {pChild1, pChild2};
	for (int iChildIter = 0; iChildIter < 2; iChildIter++)
	{
		if (pChilds[iChildIter] == NULL) 
			continue;

		for (unsigned int i = 0; i < pChilds[iChildIter]->m_InputsInBlock.size(); i++)
		{
			BaseProcessInput* pClonedItem = pChilds[iChildIter]->m_InputsInBlock[i]->Clone();
			pInputBlock->AddInput(pClonedItem);
			
			if (bAsInputToChilds)
				BaseProcessInput::SetOrientedLink(pClonedItem, pChilds[iChildIter]->m_InputsInBlock[i]);
			else
				BaseProcessInput::SetOrientedLink(pChilds[iChildIter]->m_InputsInBlock[i], pClonedItem);
		}
	}

	return pInputBlock;
}

bool InputBlock::VerifyInputMatching(const InputBlock *otherBlock, bool bPerfect) const
{
	if (bPerfect && otherBlock->m_InputsInBlock.size() != m_InputsInBlock.size()) 
		return false;

	if (otherBlock->m_InputsInBlock.size() > m_InputsInBlock.size())
		return false;

	CheckMatchingArrayOfBaseProcessInputs(m_InputsInBlock, otherBlock->m_InputsInBlock, bPerfect);

	return true;
}

bool InputBlock::Validate(SymbolTable* pSymbolTable, bool bAtRuntimeSpawn)
{
	for (ArrayOfBaseProcessInputsConstIter it = m_InputsInBlock.begin(); it != m_InputsInBlock.end(); it++)
		if (!(*it)->Validate(pSymbolTable, bAtRuntimeSpawn))
			return false;

	return true;
}

unsigned int InputBlock::GetSerializedSize()
{
	unsigned int size = 0;
	for (ArrayOfBaseProcessInputsConstIter it = m_InputsInBlock.begin(); it != m_InputsInBlock.end(); it++)
		size += (*it)->GetSerializedSize();

	return size;
}

void InputBlock::Serialize(Streams::BytesStreamWriter& stream)
{
	SERIALIZE_CHECKER_INIT_OBJECT;

	for (ArrayOfBaseProcessInputsConstIter it = m_InputsInBlock.begin(); it != m_InputsInBlock.end(); it++)
		(*it)->Serialize(stream);

	SERIALIZE_CHECKER_END_OBJECT;
}

void InputBlock::Deserialize(Streams::BytesStreamReader& stream)
{
	SERIALIZE_CHECKER_INIT_OBJECT;

	for (ArrayOfBaseProcessInputsConstIter it = m_InputsInBlock.begin(); it != m_InputsInBlock.end(); it++)
		(*it)->Deserialize(stream);

	SERIALIZE_CHECKER_END_OBJECT;
}

unsigned int BaseProcessInput::GetSerializedSize()
{
	assert(false && "This isn't expected to be called. derived classes should solve this");
	return 0;
}

void BaseProcessInput::Serialize(Streams::BytesStreamWriter& stream)
{
	assert(false && "This isn't expected to be called. derived classes should solve this");		
}

void BaseProcessInput::Deserialize(Streams::BytesStreamReader& stream)
{
	assert(false && "This isn't expected to be called. derived classes should solve this");		
}

bool CheckMatchingArrayOfBaseProcessInputs(const ArrayOfBaseProcessInputs& arr1, const ArrayOfBaseProcessInputs& arr2, bool bPerfect)
{
	for (ArrayOfBaseProcessInputsConstIter it = arr1.begin(), it2 = arr2.begin(); it2 != arr2.end(); it++, it2++)
	{
		if (!(*it)->VerifyInputMatching(*it2, bPerfect))
			return false;
	}

	return true;
}
