// This file contains the concrete implementations for functions that an user can put in their 
#include "InputTypes.h"
#include "AgapiaToCCode.h"
#include "calc3_defs.h"
#include "calc3_utils.h"

template <typename basicCType, typename basicAgapiaType>
inline void AddSimpleValueType_Private(SimpleProcessItem* pItem, void* pData)
{
	basicCType bValue = *((basicCType*)pData);
	basicAgapiaType* pAgapiaItem = new basicAgapiaType();
	pAgapiaItem->SetValue(bValue);
	pItem->AddItem(pAgapiaItem);
}

template <typename basicElementType, typename basicAgapiaVectorType>
inline void AddSimpleArrayValueType_Private(SimpleProcessItem* pItem, void* pData, int numElements)
{
	basicElementType* pValues = (basicElementType*) pData;
	basicAgapiaVectorType* pAgapiaArray = new basicAgapiaVectorType();
	pAgapiaArray->SetValues((basicElementType*)pData, numElements);
}
 
// PUBLIC TYPES
//----------------------------------------------------------------------------------------------
void AddProcessInput(ArrayOfBaseProcessInputs& arr, BaseProcessInput* pSPI)
{
	arr.push_back(pSPI);
}

void AddInputItemToVector(VectorProcessItem* pVector, ArrayOfBaseProcessInputs& arr)
{
	pVector->AddItemInput(arr);
}

//------ Functions to set different stuff for vector of processes -------
//------------------------------------------------------------------------------------------------------------------
static bool CheckAndReturnVectorComponent(int lineNo, VectorProcessItem* pVector, int iVectorIndex, const char* structItem, const ArrayOfBaseProcessInputs*& outVectorComponent, std::pair<int,int>& outIndex)
{
	outVectorComponent = &pVector->GetVectorItemByIndex(iVectorIndex);
	outIndex = pVector->GetIndexOfItemName(structItem);
	if (outIndex.first < 0 || outIndex.second < 0) // Invalid index, not found show an error
	{
		// Error
		PrintCompileError(lineNo, " Tried to access invalid item named %s from the vector", structItem);
		return false;
	}

	return true;
}

void SetInputItemToVector(int lineNo, VectorProcessItem* pVector, int iVectorIndex, const char* structItem, int value)
{
	std::pair<int,int> index;
	const ArrayOfBaseProcessInputs* outVectorComponent = NULL;
	if (!CheckAndReturnVectorComponent(lineNo, pVector, iVectorIndex, structItem, outVectorComponent, index))
		return;

	SimpleProcessItem* pSPI = (SimpleProcessItem*)(*outVectorComponent)[index.first];
	pSPI->SetItemValue(index.second, ETYPE_INT, structItem, value);
}

void SetInputItemToVector(int lineNo, VectorProcessItem* pVector, int iVectorIndex, const char* structItem, float value)
{
	std::pair<int,int> index;
	const ArrayOfBaseProcessInputs* outVectorComponent = NULL;
	if (!CheckAndReturnVectorComponent(lineNo, pVector, iVectorIndex, structItem, outVectorComponent, index))
		return;

	SimpleProcessItem* pSPI = (SimpleProcessItem*)(*outVectorComponent)[index.first];
	pSPI->SetItemValue(index.second, ETYPE_FLOAT, structItem, value);
}

void SetInputItemToVector(int lineNo, VectorProcessItem* pVector, int iVectorIndex, const char* structItem, char* value)
{
	std::pair<int,int> index;
	const ArrayOfBaseProcessInputs* outVectorComponent = NULL;
	if (!CheckAndReturnVectorComponent(lineNo, pVector, iVectorIndex, structItem, outVectorComponent, index))
		return;

	SimpleProcessItem* pSPI = (SimpleProcessItem*)(*outVectorComponent)[index.first];
	pSPI->SetItemValue(index.second, ETYPE_STRING, structItem, value);
}

// THIS IS TO BE CALLED DIRECTLY WITHOUT ANY TRANSLATION!!!!
void SetInputItemToVector(int lineNo, VectorProcessItem* pVector, int iVectorIndex, const char* structItem, char* value, int buffSize)
{
	std::pair<int,int> index;
	const ArrayOfBaseProcessInputs* outVectorComponent = NULL;
	if (!CheckAndReturnVectorComponent(0, pVector, iVectorIndex, structItem, outVectorComponent, index))
		return;

	SimpleProcessItem* pSPI = (SimpleProcessItem*)(*outVectorComponent)[index.first];
	pSPI->SetItemValue(index.second, ETYPE_DATA_BUFFER, structItem, value, buffSize);
}

//--------------------------------------------------------------------------------------------------------------------

void ClearVectorOfProcessItems(VectorProcessItem* pSourceVector)
{
	pSourceVector->ClearAll();
}

int GetNumItemsInVectorProcessItem(VectorProcessItem& pVector)
{
	return pVector.m_ArrayOfItemInputs.size();
}

BaseProcessInput* GetVectorItemByIndex(VectorProcessItem& pSourceVector, unsigned int iVectorIndex, unsigned int iProcessIndex)
{
	if (pSourceVector.m_ArrayOfItemInputs.size() <= iVectorIndex)
		PrintExecutionError("You used GetVectorItemByIndex to access an invalid vector index");

	ArrayOfBaseProcessInputs& item = pSourceVector.GetVectorItemByIndex(iVectorIndex);
	if (item.size() <= iProcessIndex)
		PrintExecutionError("You used GetVectorItemByIndex to access an invalid process index");

	return item[iProcessIndex];
}

void AddSimpleValueType(SimpleProcessItem* pItem, int fValue)
{
	AddSimpleValueType_Private<int, IntDataItem>(pItem, &fValue);
}

inline void AddSimpleValueType(SimpleProcessItem* pItem, float fValue)
{
	AddSimpleValueType_Private<float, FloatDataItem>(pItem, &fValue);
}

inline void AddSimpleValueType(SimpleProcessItem* pItem, char* fValue)
{
	AddSimpleValueType_Private<char*, StringDataItem>(pItem, &fValue);
}

inline void AddSimpleValueType(SimpleProcessItem* pItem, bool fValue)
{
	AddSimpleValueType_Private<bool, BoolDataItem>(pItem, &fValue);
}

inline void AddSimpleValueType(SimpleProcessItem* pItem, char fValue)
{
	AddSimpleValueType_Private<char, CharDataItem>(pItem, &fValue);
}

inline void AddSimpleArrayValueType(SimpleProcessItem* pItem, int* pData, int numElements)
{
	AddSimpleArrayValueType_Private<int, IntVectorDataItem>(pItem, pData, numElements);
} 

inline void AddSimpleArrayValueType(SimpleProcessItem* pItem, float* pData, int numElements)
{
	AddSimpleArrayValueType_Private<float, FloatVectorDataItem>(pItem, pData, numElements);
}

inline void AddSimpleArrayValueType(SimpleProcessItem* pItem, bool* pData, int numElements)
{
	AddSimpleArrayValueType_Private<bool, BoolVectorDataItem>(pItem, pData, numElements);
}

inline void AddSimpleArrayValueType(SimpleProcessItem* pItem, char* pData, int numElements)
{
	AddSimpleArrayValueType_Private<char*, CharVectorDataItem>(pItem, pData, numElements);
}

inline void AddSimpleArrayValueType(SimpleProcessItem* pItem, char** pData, int numElements)
{
	AddSimpleArrayValueType_Private<char*, StringVectorDataItem>(pItem, pData, numElements);
}

inline void* GetSimpleArrayItem(SimpleProcessItem* pItem, int iPosition)
{
	return pItem->GetItem(iPosition);
}
 
void AddBufferDataType(SimpleProcessItem* pItem, char* pData, int iSize)
{
	BufferDataItem *pBufferDataItem = new BufferDataItem();
	pBufferDataItem->SetValue(pData, iSize);
	pItem->AddItem(pBufferDataItem);
}

int& GetItemRefFromSimpleValueType_asInt(SimpleProcessItem* pItem, int iPosition)
{
	return ((IntDataItem*)pItem->GetItem(iPosition))->GetValueRef();
}

float& GetItemRefFromSimpleValueType_asFloat(SimpleProcessItem* pItem, int iPosition)
{
	return ((FloatDataItem*)pItem->GetItem(iPosition))->GetValueRef();
}

bool& GetItemRefFromSimpleValueType_asBool(SimpleProcessItem* pItem, int iPosition)
{
	return ((BoolDataItem*)pItem->GetItem(iPosition))->GetValueRef();
}

char& GetItemRefFromSimpleValueType_asChar(SimpleProcessItem* pItem, int iPosition)
{
	return ((CharDataItem*)pItem->GetItem(iPosition))->GetValueRef();

}

BufferDataItem* GetItemRefFromSimpleValueType_asBuffer(SimpleProcessItem* pItem, int iPosition)
{
	return ((BufferDataItem*)pItem->m_InputItems[iPosition]);
}

//----------------------------------------------------------------------------------------------

