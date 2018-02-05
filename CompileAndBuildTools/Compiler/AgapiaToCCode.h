#ifndef AGAPIA_TO_C_CODE
#define AGAPIA_TO_C_CODE

class InputBlock;
class SimpleProcessItem;
class VectorProcessItem;
class BufferDataItem;

//class ArrayOfBaseProcessInputs;

#include "InputTypes.h"

typedef void(*AgapiaToCFunction)(InputBlock* pInputNorth, InputBlock* pInputWest, InputBlock* pOutputSouth, InputBlock* pOutputEast);

void InitializeAgapiaToCFunctions();


// Private types
//------------------------------------------------------------------------------------
// Add a single item value type to Simple process item
template <typename basicCType, typename basicAgapiaType>
inline void AddSimpleValueType_Private(SimpleProcessItem* pItem, void* pData);

// Add a vector item value type to Simple Process item
template <typename basicElementType, typename basicAgapiaVectorType>
inline void AddSimpleArrayValueType_Private(SimpleProcessItem* pItem, void* pData, int numElements);

// Add a simple process item to an array of processes
void AddProcessInput(ArrayOfBaseProcessInputs& arr, BaseProcessInput* pSPI);

// Adds and array o processes (= item instance) to a vector
void AddInputItemToVector(VectorProcessItem* pVector, ArrayOfBaseProcessInputs& arr);

// Set input item to a vector of processes: pVector[iVectorIndex].structItem = value;
void SetInputItemToVector(int lineNo, VectorProcessItem* pVector, int iVectorIndex, const char* structItem, int value);
void SetInputItemToVector(int lineNo, VectorProcessItem* pVector, int iVectorIndex, const char* structItem, float value);
void SetInputItemToVector(int lineNo, VectorProcessItem* pVector, int iVectorIndex, const char* structItem, char* value);
void SetInputItemToVector(int lineNo, VectorProcessItem* pVector, int iVectorIndex, const char* structItem, char* value, int buffSize);

// Gets the dimension of the vector of process
int GetNumItemsInVectorProcessItem(VectorProcessItem& pVector);

// Gets a process item from a vector at a specified vector index and a process index
BaseProcessInput* GetVectorItemByIndex(VectorProcessItem& pSourceVector, unsigned int iVectorIndex, unsigned int iProcessIndex);

// Clears all the items from the vector of process
void ClearVectorOfProcessItems(VectorProcessItem* pSourceVector);
//-------------------------------------------------------------------------------------


// Public types - to be used by the user
//------------------------------------------------------------------------------------
void AddSimpleValueType(SimpleProcessItem* pItem, int iValue);
void AddSimpleValueType(SimpleProcessItem* pItem, float fValue);
void AddSimpleValueType(SimpleProcessItem* pItem, char cValue);
void AddSimpleValueType(SimpleProcessItem* pItem, char* fValue, bool bAsString);
void AddSimpleValueType(SimpleProcessItem* pItem, bool fValue);

void AddSimpleArrayValueType(SimpleProcessItem* pItem, int* pData, int numElements);
void AddSimpleArrayValueType(SimpleProcessItem* pItem, float* pData, int numElements);
void AddSimpleArrayValueType(SimpleProcessItem* pItem, char** pData, int numElements);
void AddSimpleArrayValueType(SimpleProcessItem* pItem, bool* pData, int numElements);
void AddSimpleArrayValueType(SimpleProcessItem* pItem, char* pData, int numElements);

void AddBufferDataType(SimpleProcessItem* pItem, char* pData, int iSize);

int& GetItemRefFromSimpleValueType_asInt(SimpleProcessItem* pItem, int iPosition);
float& GetItemRefFromSimpleValueType_asFloat(SimpleProcessItem* pItem, int iPosition);
bool& GetItemRefFromSimpleValueType_asBool(SimpleProcessItem* pItem, int iPosition);
char& GetItemRefFromSimpleValueType_asChar(SimpleProcessItem* pItem, int iPosition);
BufferDataItem* GetItemRefFromSimpleValueType_asBuffer(SimpleProcessItem* pItem, int iPosition);

// TODO : add the rest of get values

//------------------------------------------------------------------------------------
#endif


