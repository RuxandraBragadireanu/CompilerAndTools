#ifndef INPUT_TYPES_H
#define INPUT_TYPES_H 

#include <stdio.h>
#include <list>
#include <vector>
#include "calc3_defs.h"
#include "CONFIGURATION.h"
#include "Streams.h"
#include <map>
#include <string>

// Direction where an input might came from
enum EInputDirection
{
	E_INPUT_DIR_NORTH,
	E_INPUT_DIR_WEST,
	E_NUM_DIRECTION,
};


typedef int DataTypes;
class SymbolTable;
class InputBlock;
class ProgramBase;
class BaseProcessInput;


typedef std::vector<BaseProcessInput*> ArrayOfBaseProcessInputs;
typedef ArrayOfBaseProcessInputs::iterator ArrayOfBaseProcessInputsIter;
typedef ArrayOfBaseProcessInputs::const_iterator ArrayOfBaseProcessInputsConstIter;

// Maps a item in a vector of processes to the index in the simple process  (index of the process, index in the process)
typedef std::map<std::string,std::pair<int,int>>		MapMemberToIndex;		
typedef MapMemberToIndex::iterator	MapMemberToIndexIter;

// Defines functions for a type serialization
#define DECLARE_TYPE_SERIALIZATION	\
	virtual unsigned int GetSerializedSize();	\
	virtual void Serialize(Streams::BytesStreamWriter& stream);	\
	virtual void Deserialize(Streams::BytesStreamReader& stream);	\

#define DECLARE_VIRTUAL_TYPE_SERIALIZATION	\
	virtual int GetSerializedDataTypeSize() = 0;	\
	virtual void SerializeAsDataType(Streams::BytesStreamWriter& stream) = 0;	\
	virtual void DeserializeAsDataType(Streams::BytesStreamReader& stream) = 0;	\


#ifdef IGNORE_AGAPIA_DECL_AND_ASIGNMENTS
typedef std::list<ProgramBase*> ListOfProgramsListener;
typedef ListOfProgramsListener::iterator ListOfProgramsListenerIter;
#endif

#define MAX_IDENTIFIER_SIZE	100
class IDataTypeItem
{
public:
	IDataTypeItem()
	#ifdef IGNORE_AGAPIA_DECL_AND_ASIGNMENTS
		: m_pProgramsListeners(NULL)
		, m_pParent(NULL)
	#endif
	{}
	virtual ~IDataTypeItem();

	const char *GetName() const { return m_szName; }
	void SetName(const char* szName);
	virtual void Equals(const IDataTypeItem* pInputItem) = 0;

	virtual void PrintDebugInfo(int iSpaceLeft);

	// Real time created data used specially for small types (not arrays)
	virtual IDataTypeItem* Clone() = 0;

	// Serialization utilities
	DECLARE_TYPE_SERIALIZATION

	static inline unsigned int GetSerializedDataTypeSize(IDataTypeItem* dataType);
	static inline IDataTypeItem* DeserializeDataType(Streams::BytesStreamReader& stream);
	static inline void SerializeDataType(Streams::BytesStreamWriter& stream, const IDataTypeItem* dataType);

	// Specialized constructs - you should only use the function specific for your type and leave the rest as default !
	// Uses preallocated memory for internal data
	virtual IDataTypeItem* Clone(int *pAllocatedData);
	virtual IDataTypeItem* Clone(char *pAllocatedData);
	virtual IDataTypeItem* Clone(float *pAllocatedData);
	virtual IDataTypeItem* Clone(bool *pAllocatedData);

	void CopyBase(IDataTypeItem* pItem);

	DataTypes	m_eDataType;
	DataTypes	m_eItemsDataType;	// Applicable only on arrays type! (if m_eDataType is array)

	// Parent base if it has one. It needs to inform its parent about different events
	BaseProcessInput*	m_pParent;  

	bool SameTypeAs(IDataTypeItem* pOtherItem) { return (m_eDataType == pOtherItem->m_eDataType && m_eItemsDataType == pOtherItem->m_eItemsDataType); }

#ifdef IGNORE_AGAPIA_DECL_AND_ASIGNMENTS
	// This is a collection of all listeners that needs to be informed when this input is received
	// A concrete example is the FOR_S(limit) , where limit is not an input to the FOR program, but FOR program is a listener for "limit".
	// If this is NULL, it wasn't ever created
	// However i don't see any reason for a program who links this input, to be listeners for it...
	ListOfProgramsListener*	m_pProgramsListeners;
	void AddProgramListener(ProgramBase* pProgram);

	// This must be called when this input variable becomes available
	inline void FireProgramListeners(); 
#endif

	char* m_szName;
private:

};

class BoolDataItem : public IDataTypeItem
{
public:
	BoolDataItem(bool* pAllocatedData) { m_eDataType = ETYPE_BOOL; m_pData = pAllocatedData; }
	BoolDataItem() { m_eDataType = ETYPE_BOOL; m_pData = new bool();}
	virtual ~BoolDataItem(){ delete m_pData; }

	DECLARE_TYPE_SERIALIZATION

	void SetValue(bool bValue) { *m_pData = bValue; }
	bool GetValue()			   { return *m_pData; }

	// Returns the reference to this item
	bool& GetValueRef()	const	{ return *m_pData;}

	void Equals(const IDataTypeItem* pInputItem) { SetValue(((BoolDataItem*)pInputItem)->GetValue());}
	virtual IDataTypeItem* Clone(bool* pAllocatedData = NULL);
	virtual IDataTypeItem* Clone() { bool* pMem = new bool(); return Clone(pMem); }

private:
	bool* m_pData;
};

class CharDataItem : public IDataTypeItem
{
public:
	CharDataItem(char* pAllocatedData) { m_eDataType = ETYPE_CHAR; m_pData = pAllocatedData; }
	CharDataItem() { m_eDataType = ETYPE_CHAR; m_pData = new char();}
	virtual ~CharDataItem(){ delete m_pData; }

	DECLARE_TYPE_SERIALIZATION

	void SetValue(char bValue) { *m_pData = bValue; }
	char GetValue()			   { return *m_pData; }

	// Returns the reference to this item
	char& GetValueRef()	const	{ return *m_pData;}

	void Equals(const IDataTypeItem* pInputItem) { SetValue(((CharDataItem*)pInputItem)->GetValue());}
	virtual IDataTypeItem* Clone(char* pAllocatedData = NULL);
	virtual IDataTypeItem* Clone() { char* pMem = new char(); return Clone(pMem); }

private:
	char* m_pData;
};

class IntDataItem : public IDataTypeItem
{
public:
	// This allocator uses a predefined allocated memory - for arrays of this type
	IntDataItem(int *pAllocatedData) { m_eDataType = ETYPE_INT; m_pData = pAllocatedData; }
	// This  allocator is used to create a single type of this item not an array
	IntDataItem() { m_eDataType = ETYPE_INT; m_pData = new int(); }

	DECLARE_TYPE_SERIALIZATION

	virtual ~IntDataItem(){ delete m_pData; }

	void SetValue(int bValue) 
	{ 
		*m_pData = bValue; 
	}

	// Returns the value of this item
	int GetValue()		const	
	{ 
		return *m_pData; 
	}

	// Returns the reference to this item
	int& GetValueRef()	const	{ return *m_pData;}

	void Equals(const IDataTypeItem* pInputItem) { SetValue(((IntDataItem*)pInputItem)->GetValue());}
	virtual IDataTypeItem* Clone(int* pAllocatedData = NULL);
	virtual IDataTypeItem* Clone() { int* pMem = new int(); return Clone(pMem); }

private:
	int* m_pData;
};

class FloatDataItem : public IDataTypeItem
{
public:
	// This allocator uses a predefined allocated memory - for arrays of this type
	FloatDataItem(float* pAllocatedData) { m_eDataType = ETYPE_FLOAT; m_pData = pAllocatedData; }
	// Simple type
	FloatDataItem() { m_eDataType = ETYPE_FLOAT; m_pData = new float(); }

	DECLARE_TYPE_SERIALIZATION

	virtual ~FloatDataItem(){ delete m_pData; }

	inline void SetValue(float bValue) { *m_pData = bValue; }
	float GetValue()	const	{ return *m_pData; }

	// Returns the reference to this item
	float& GetValueRef()	const	{ return *m_pData;}

	void Equals(const IDataTypeItem* pInputItem) { SetValue(((FloatDataItem*)pInputItem)->GetValue());}
	virtual IDataTypeItem* Clone(float* pAllocatedData = NULL);
	virtual IDataTypeItem* Clone() { float* pMem = new float(); return Clone(pMem); }

private:
	float* m_pData;
};

class StringDataItem : public IDataTypeItem
{
public:
	StringDataItem(char* pAllocatedData) { m_eDataType = ETYPE_STRING; m_pData = pAllocatedData; }
	StringDataItem() { m_eDataType = ETYPE_STRING; m_pData = NULL; }
	virtual ~StringDataItem() { delete [] m_pData; }

	DECLARE_TYPE_SERIALIZATION
	
	void SetValue(char *pSzData);
	char* GetValue() const	{ return m_pData; }
	// Returns the reference to this item
	char** GetValueRef()	
	{ 
		return &m_pData;
	}

	void Equals(const IDataTypeItem* pInputItem) 
	{ 
		SetValue(((StringDataItem*)pInputItem)->GetValue());
	}
	virtual IDataTypeItem* Clone(char* pAllocatedData = NULL);
	virtual IDataTypeItem* Clone() { char* pMem = new char(); return Clone(pMem); }

private:
	char* m_pData;
};

class BufferDataItem : public IDataTypeItem
{
public:
	/*
	struct BufferDataItemValue
	{
		// Contains pointers to the original data structures where it comes from
		char **pData;
		int *iDataSize;
	};
	*/

	BufferDataItem(char* pAllocatedData) { m_iBufferSize = 0; m_eDataType = ETYPE_DATA_BUFFER; m_pData = pAllocatedData; }
	BufferDataItem() { m_iBufferSize = 0; m_eDataType = ETYPE_DATA_BUFFER; m_pData = NULL;}
	virtual ~BufferDataItem(){ delete m_pData; }

	DECLARE_TYPE_SERIALIZATION
	
	void SetValue(char* pData, int iSize) 
	{ 
		m_pData = pData; 
		m_iBufferSize = iSize; 
	}

	BufferDataItem* /*BufferDataItemValue*/ GetValue()			  
	{ 
		/*
		BufferDataItemValue data; 
		data.iDataSize = &m_iBufferSize; 
		data.pData = &m_pData; 
		return data; 
		*/
		return this;
	}
	
	inline void CopyValuesFrom(const BufferDataItem* pItem);
	char* GetValueRef()	{ return m_pData; }

	void Equals(const IDataTypeItem* pInputItem) 
	{ 
		BufferDataItem* pBufferedDataItem = (BufferDataItem*)pInputItem; 
		m_pData = pBufferedDataItem->m_pData; 
		m_iBufferSize = pBufferedDataItem->m_iBufferSize;
	}
	virtual IDataTypeItem* Clone();

//private:
	// Allocated begin of data
	char* m_pData;
	int m_iBufferSize;
};


typedef std::vector<IntDataItem*> ArrayOfIntDataItems;
typedef ArrayOfIntDataItems::iterator ArrayOfBaseProcessIntInputsIter;
class IntVectorDataItem : public IDataTypeItem
{
public:
	IntVectorDataItem() { 
							m_eDataType = ETYPE_GENERAL_ARRAY; 
							m_eItemsDataType = ETYPE_INT; 
							m_iCurrentAllocatedSize = 0; m_iBiggestIndexUsed = 0; m_pDataBuffer = NULL;
	                    }

	DECLARE_TYPE_SERIALIZATION


	void SetValues(int *pData, int iNumElems);
	void SetValue(unsigned int iIndex, int iValue);
	int GetValue(int iIndex);
	void Equals(const IDataTypeItem* pInputItem){}
	virtual IDataTypeItem* Clone();

private:
	ArrayOfIntDataItems	m_InputItems;

	// Buffer from which the input data items are taking data
	int* m_pDataBuffer;
	// The current size of the buffer
	unsigned int m_iCurrentAllocatedSize;
	// The bigest array index used so far
	int m_iBiggestIndexUsed;
};

typedef std::vector<BoolDataItem*> ArrayOfBoolDataItems;
typedef ArrayOfBoolDataItems::iterator ArrayOfBaseProcessBoolInputsIter;
class BoolVectorDataItem : public IDataTypeItem
{
public:
	BoolVectorDataItem() { 
						m_eDataType = ETYPE_GENERAL_ARRAY; 
						m_eItemsDataType = ETYPE_BOOL; 
						m_iCurrentAllocatedSize = 0; m_iBiggestIndexUsed = 0; m_pDataBuffer = NULL;
	                     }

	DECLARE_TYPE_SERIALIZATION

	void SetValues(bool* pData, int iElementCount);
	void SetValue(unsigned int iIndex, bool iValue);
	bool GetValue(int iIndex);
	void Equals(const IDataTypeItem* pInputItem){}
	virtual IDataTypeItem* Clone();

private:
	ArrayOfBoolDataItems	m_InputItems;
	
	// Buffer from which the input data items are taking data
	bool* m_pDataBuffer;
	// The current size of the buffer
	unsigned int m_iCurrentAllocatedSize;
	// The biggest array index used so far
	int m_iBiggestIndexUsed;
};

typedef std::vector<CharDataItem*> ArrayOfCharDataItems;
typedef ArrayOfCharDataItems::iterator ArrayOfBaseProcessCharInputsIter;
class CharVectorDataItem : public IDataTypeItem
{
public:
	CharVectorDataItem() { 
						m_eDataType = ETYPE_GENERAL_ARRAY; 
						m_eItemsDataType = ETYPE_CHAR; 
						m_iCurrentAllocatedSize = 0; m_iBiggestIndexUsed = 0; m_pDataBuffer = NULL;
	                     }

	DECLARE_TYPE_SERIALIZATION

	void SetValues(char** pData, int iElementCount);
	void SetValue(unsigned int iIndex, char iValue);
	char GetValue(int iIndex);
	void Equals(const IDataTypeItem* pInputItem){}
	virtual IDataTypeItem* Clone();

private:
	ArrayOfCharDataItems	m_InputItems;
	
	// Buffer from which the input data items are taking data
	char* m_pDataBuffer;
	// The current size of the buffer
	unsigned int m_iCurrentAllocatedSize;
	// The biggest array index used so far
	int m_iBiggestIndexUsed;
};

typedef std::vector<FloatDataItem*> ArrayOfFloatDataItems;
typedef ArrayOfFloatDataItems::iterator ArrayOfBaseProcessFloatInputsIter;
class FloatVectorDataItem : public IDataTypeItem
{
public:
	FloatVectorDataItem() { m_eDataType = ETYPE_GENERAL_ARRAY; 
							m_eItemsDataType = ETYPE_FLOAT; 
						  m_iCurrentAllocatedSize = 0; m_iBiggestIndexUsed = 0; m_pDataBuffer = NULL;
	                    }

	DECLARE_TYPE_SERIALIZATION

	void SetValues(float* pData, int iElementCount);
	void SetValue(unsigned int iIndex, float iValue);
	float GetValue(int iIndex);
	void Equals(const IDataTypeItem* pInputItem){}
	virtual IDataTypeItem* Clone();

private:
	ArrayOfFloatDataItems	m_InputItems;
	
	// Buffer from which the input data items are taking data
	float* m_pDataBuffer;
	// The current size of the buffer
	unsigned int m_iCurrentAllocatedSize;
	// The bigest array index used so far
	int m_iBiggestIndexUsed;
};

typedef std::vector<StringDataItem*> ArrayOfStringDataItems;
typedef ArrayOfStringDataItems::iterator ArrayOfBaseProcessStringInputsIter;
class StringVectorDataItem : public IDataTypeItem
{
public:
	StringVectorDataItem() {	m_eDataType = ETYPE_GENERAL_ARRAY;  
								m_eItemsDataType = ETYPE_STRING; 
								m_iCurrentAllocatedSize = 0; m_iBiggestIndexUsed = 0; m_pDataBuffer = NULL;
							}

	DECLARE_TYPE_SERIALIZATION

	void SetValues(char** pData, int iElementCount);
	void SetValue(unsigned int iIndex, char* iValue);
	char* GetValue(int iIndex);
	void Equals(const IDataTypeItem* pInputItem){}
	virtual IDataTypeItem* Clone();

private:
	ArrayOfStringDataItems	m_InputItems;

	// Buffer from which the input data items are taking data
	char* m_pDataBuffer;
	// The current size of the buffer
	unsigned int m_iCurrentAllocatedSize;
	// The bigest array index used so far
	int m_iBiggestIndexUsed;
};

class ItemTypeFactory
{
public:
	static IDataTypeItem* CreateInputItem(DataTypes eDataType, const char* szName);
	static void DestroyDataInputItem(IDataTypeItem* pInputItem);
};

enum EInputTypes
{
	E_INPUT_SIMPLE,
	E_INPUT_ARRAY
};

// Checks if the two Array of base process inputs have the same types inside
bool CheckMatchingArrayOfBaseProcessInputs(const ArrayOfBaseProcessInputs& arr1, const ArrayOfBaseProcessInputs& arr2, bool bPerfect);

// Copy the cloned information from array source to array dest
void CloneAndCopyArrayOfBaseProcessInputs(ArrayOfBaseProcessInputs& dest, const ArrayOfBaseProcessInputs& source);

// Copy only the structure from source to dest, but leave all values to NULL
void CopyNullArrayOfBaseProcessInputs(ArrayOfBaseProcessInputs& dest, const ArrayOfBaseProcessInputs& source);

class VectorProcessItem;
class BaseProcessInput
{
public:
	BaseProcessInput(EInputTypes eType, InputBlock* pParent);
	~BaseProcessInput();
	EInputTypes GetType() { return m_eType; }

	DECLARE_TYPE_SERIALIZATION
	DECLARE_VIRTUAL_TYPE_SERIALIZATION

	virtual void PrintDebugInfo(int iSpaceLeft) const = 0; 
	virtual BaseProcessInput* Clone() = 0;
	virtual bool Validate(SymbolTable* pSymbolTable, bool bAtRuntimeSpawn) = 0;

	void CopyBase(BaseProcessInput* pCloned);
	virtual bool VerifyInputMatching(const BaseProcessInput* otherInput, bool bPerfectMatching = false) const { return false; }

	// Where we get input for this node
	BaseProcessInput* m_pFrom;
	// Where we send input from this node
	BaseProcessInput* m_pNext;

	static void SetOrientedLink(BaseProcessInput* pOutput, BaseProcessInput* pInput);
	static void SetOrientedLink(ArrayOfBaseProcessInputs& pOutputs, ArrayOfBaseProcessInputs& pInputs);

	// Applicable to array of processes only - Indexes for array next / from rule
	unsigned int m_iFromIndexRule;
	unsigned int m_iToIndexRule;
	bool IsNotVectorToVector() { return (m_iFromIndexRule > 0 || m_iToIndexRule > 0); }

	// This function is to send an input through links until it gets to atomic node
	// It will inform on its path various node type (like if/for) that this input arrived
	// iIndexBegin represent at each step, if THIS is a vector, from which index it will copy:  [iIndexBegin , ....end..]  ->  atomic vector input
	bool GoDownValue(BaseProcessInput* pOriginalInput);

	// This is used to set values to this from pInput
	virtual bool SetValues(BaseProcessInput* pInput) = 0;

	// True if we this node contains inputs observed by other modules but not liked through a node 
	// Examples: FOR, WHILE, IF conditions
	bool IsContainingInputObserved() { return m_bContainsObservedInput; }
	bool m_bContainsObservedInput;
	void SetContainingObservedInput() { m_bContainsObservedInput = true; }

	// Applicable at atomic modules - true if this is received by the module
	bool m_bInputReceived;
	
	void SetParentBlock(InputBlock* pParent) { m_pParentBlock = pParent; }
	InputBlock* GetParentBlock() { return m_pParentBlock; }
	void SetIndexInBlock(unsigned int iIndex) { m_iIndexInBlock = iIndex; }
	int GetIndexInBlock() { return m_iIndexInBlock; }

	/*ENodeType*/ int GetLastIterativeProgramVisited() { return m_eLastIterativeProgramVisited; }

//protected:
	EInputTypes	m_eType;

	// This is used to know where this input is in the block
	// If type of a vector is a,b;c,d; then b has m_iParentVectorItemIndex = 1
	unsigned int	m_iIndexInBlock;
	InputBlock*		m_pParentBlock;

	// Vector reference where this simple input is contained.
	// If it's not contained in a vector then it's NULL
	VectorProcessItem*	m_pParentVector;
	
	// Which index is this in the parent vector define above ?
	unsigned int m_iParentVectorIndex;

	/*ENodeType*/ int m_eLastIterativeProgramVisited;
};

typedef std::vector<IDataTypeItem*> ListOfInputItems;
typedef ListOfInputItems::const_iterator ListOfInputItemsConstIter;
typedef ListOfInputItems::iterator ListOfInputItemsIter;

class SimpleProcessItem : public BaseProcessInput
{
public:
	SimpleProcessItem(InputBlock* pParent = NULL) : BaseProcessInput(E_INPUT_SIMPLE, pParent) {}	
	void AddItem(DataTypes eDataType, char* szName);
	void AddItem(IDataTypeItem* pInputItem);

	void SetItemValue(int index, DataTypes eDataType, const char* szName, int value);
	void SetItemValue(int index, DataTypes eDataType, const char* szName, float value);
	void SetItemValue(int index, DataTypes eDataType, const char* szName, char* value);
	void SetItemValue(int index, DataTypes eDataType, const char* szName, char* value, int buffSize);

	void* GetItem(int iPosition) { return m_InputItems[iPosition]; }
	void MakeAvailablePos(int iPosition) 
	{ 
		while((unsigned int)iPosition >= m_InputItems.size())
		{
			m_InputItems.push_back(NULL);
		}
	}
	inline int GetNumItems() { return m_InputItems.size(); }

	virtual bool Validate(SymbolTable* pSymbolTable, bool bAtRuntimeSpawn);

	// This is output, other is input
	bool VerifyInputMatching(const BaseProcessInput* otherInput, bool bPerfectMatching = false) const;
	bool SendProcessInput(SimpleProcessItem& otherInput) const;

	virtual bool SetValues(BaseProcessInput* pInput);

	virtual void PrintDebugInfo(int iSpaceLeft) const;
	virtual BaseProcessInput* Clone();

	ListOfInputItems	m_InputItems;

	// Serialization utilities
	DECLARE_TYPE_SERIALIZATION

	inline int GetSerializedDataTypeSize();
	inline void SerializeAsDataType(Streams::BytesStreamWriter& stream);
	inline void DeserializeAsDataType(Streams::BytesStreamReader& stream);
};

typedef std::vector<ArrayOfBaseProcessInputs> ArrayOfItemsProcessInputs;
typedef ArrayOfItemsProcessInputs::iterator ArrayOfItemsProcessInputsIter;
typedef ArrayOfItemsProcessInputs::const_iterator ArrayOfItemsProcessInputsConstIter;

class VectorProcessItem : public BaseProcessInput
{
public:
	VectorProcessItem(InputBlock* pParent = NULL) 
		: BaseProcessInput(E_INPUT_ARRAY, pParent)
		, m_iNumberOfInputsToBeReceived(0)
	    , m_NumProcessesInItemType(0) { }
	void AddItemInput(ArrayOfBaseProcessInputs& input);
	void ClearAll();

	virtual bool Validate(SymbolTable* pSymbolTable, bool bAtRuntimeSpawn) { return true; }

	// Set the type of the array. 
	bool SetItemType(const ArrayOfBaseProcessInputs& inputInterface, const char* szName);
	const ArrayOfBaseProcessInputs& GetItemType() { return m_TypeOfArrayItems; }

	// This is output, other is input
	bool VerifyInputMatching(const BaseProcessInput* otherInput, bool bPerfectMatching = false);

	virtual void PrintDebugInfo(int iSpaceLeft) const;
	virtual BaseProcessInput* Clone();

	virtual bool SetValues(BaseProcessInput* pInput);

	ArrayOfBaseProcessInputs	m_TypeOfArrayItems;
	MapMemberToIndex			m_MapItemNameToIndex;

	char* GetName() const { return m_szName; }
	int GetNumItemsInType() const { return m_NumProcessesInItemType; }

	// Gets a pointer to a specified index from the current array.
	// If it doesn't exists, it adds it
	inline ArrayOfBaseProcessInputs& GetVectorItemByIndex(unsigned int iInt);

	std::pair<int,int> GetIndexOfItemName(const char* szItemName);

	// Serialization utilities
	DECLARE_TYPE_SERIALIZATION

	inline int GetSerializedDataTypeSize();
	inline void SerializeAsDataType(Streams::BytesStreamWriter& stream);
	inline void DeserializeAsDataType(Streams::BytesStreamReader& stream);

	// This number is used for special modules, such as the FOR output vector where we should 
	// wait for all input to come before sending the vector or a partial type to an atomic module!
	int		m_iNumberOfInputsToBeReceived;

//private:
	ArrayOfItemsProcessInputs	m_ArrayOfItemInputs;
	int							m_NumProcessesInItemType;	// Cached value for m_ArrayOfItemInputs.size()
	// Name of the array
	char*	m_szName;
};

// This describes an input block
class InputBlock
{
public:
	InputBlock(ProgramBase* pInputBlock = NULL) : m_pParentProgram(pInputBlock), m_bIsNorthBlock(false) {}
	void AddInput(BaseProcessInput* pProcessInput);

	// Copy the inputs from other block
	void AddInputs(InputBlock* pOtherBlock);

	// Call this when all the input has been loaded - to init the read
	void OnFinishedInputLoading();

	// Get the next input, returns NULL if no input available
	BaseProcessInput* GetNextInput();

	// Consider that node as a input generator for pChild1 and 2 (can be null)
	bool CheckAndLinkInputs(InputBlock* pChild1, InputBlock* pChild2, bool bChildsAsOutputs);

	// Create InputBlocks from two maximum child, either as input or output 
	static InputBlock* CreateAndLinkInputBlock(InputBlock* pChild1, InputBlock *pChild2, bool bChildsAsOutputs);

	bool VerifyInputMatching(const InputBlock *otherBlock, bool bPerfect = false) const;

	// Debug functionality
	virtual void PrintDebugInfo(int iSpaceLeft) const;
	virtual InputBlock* Clone();

	bool Validate(SymbolTable* pSymbolTable, bool bAtRuntimeSpawn);
	bool IsNil() { return m_InputsInBlock.size() == 0; }
	int GetProcessesCount() { return m_InputsInBlock.size(); }

	ArrayOfBaseProcessInputs	m_InputsInBlock;

	ProgramBase* m_pParentProgram;
	void SetParent(ProgramBase* pParent) { m_pParentProgram = pParent; }

	bool IsNorthBlock() { return m_bIsNorthBlock; }
	void SetAsNorthBlock() { m_bIsNorthBlock = true; }


	// Used for serialization - gets the size in bytes needed to serialize
	DECLARE_TYPE_SERIALIZATION

//private:
	ArrayOfBaseProcessInputsIter m_NextInputToMatch;

	// This is true for the input north blocks, and false otherwise (normally only west input should be considered)
	bool m_bIsNorthBlock;
};

#endif