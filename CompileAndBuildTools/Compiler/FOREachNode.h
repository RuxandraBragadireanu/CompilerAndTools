#ifndef FOR_NODE_H
#define FOR_NODE_H

#include "BaseNode.h"
#include "calc3_defs.h"
#include <vector>
#include <set>
#include "Expressions.h"

enum EForEachType
{
	E_FOREACH_UNDEFINED = -1,
	E_FOREACH_S = DTYPE_FOREACH_S,
	E_FOREACH_T = DTYPE_FOREACH_T,
	E_FOREACH_ST = DTYPE_FOREACH_ST,
};

//typedef std::vector<ArrayOfBaseProcessInputs>	BufferOfProcessInputs;
//typedef BufferOfProcessInputs::iterator			BufferOfProcessInputsIter;

class ForBufferItemType
{
public:
	int iVectorIndex;
	int iItemIndex;
	BaseProcessInput* pInput;

	ForBufferItemType(BaseProcessInput* _pInput, int _iVectorIndex, int _iItemIndex)
		: iVectorIndex(_iVectorIndex)
		, iItemIndex(_iItemIndex)
		, pInput(_pInput) {}
};

typedef std::list<ForBufferItemType> ForBufferOfProcessInputs;
typedef ForBufferOfProcessInputs::iterator ForBufferOfProcessInputsIter;

class ProgramFOREACH : public ProgramBase
{
public:
	ProgramFOREACH(int lineNo) : ProgramBase(E_NODE_TYPE_FOREACH, lineNo), m_pValueToGo(NULL), m_bExpanded(false), m_bShouldExpand(false),
					m_pBaseChildProgram(NULL), m_pIterationChilds(NULL), m_eType(E_FOREACH_UNDEFINED), m_iNumIterationChilds(NULL), m_FullArrayBuffered(NULL), m_iNrFinishedInternalPrograms(0)
	{}

	virtual ProgramBase* Clone();
	virtual bool SolveReferences();
	virtual bool SolveInputOutput();
	virtual bool Validate(SymbolTable* pParent, bool bAtRuntimeSpawn, bool bTemplateMOdule) override;

	// Called when this for receive a value
	void ChildsCreationLinkage();

	// Sends the buffered input inside to the FOR linked child
	void SendBufferedInput();

	// This is called when received an input through graph links
	bool OnInputReceived(BaseProcessInput* pOriginalInput, BaseProcessInput* pInputNodeReceiver, EInputDirection eDir);
	// This is called when received an input as listener
	virtual void OnInputItemReady(char* szName);

	void OnChildProgramFinished(ProgramBase* pChildFinished);

	// The expression, constant, simple identifier or expression value node which tells how many iterations this node has
	IExpression*	m_pValueToGo;
	
	// This is a set that contains the required variables from the FOR condition.
	// We expect this so we can continue our execution
	VariablesNameSet m_SetOfRequiredVariables;

	// True if the for child have been expanded
	bool m_bExpanded;

	// True if this module comes from something that was cloned after and parent was expanded
	bool m_bShouldExpand;

	// Base child program 
	ProgramBase*	m_pBaseChildProgram;

	// Cloned child - created when the first value is received by for
	ProgramBase**	m_pIterationChilds;
	unsigned int	m_iNumIterationChilds;

	// This is used to buffer the input received by the FOR when it wasn't expanded
	//BufferOfProcessInputs		m_BufferOfInputs[E_NUM_DIRECTION];
	ForBufferOfProcessInputs	m_BufferedInputs[E_NUM_DIRECTION];

	// This is not NULL if, for example it receives a full array from north or south
	BaseProcessInput*			m_FullArrayBuffered;

	// Type of the for
	EForEachType	m_eType;

private:
	void Expand();

	// Adds a process input in the local buffer
	// eDir - the direction where this input comes from
	// pOriginalInput - a pointer with the actual input received
	// iParentVectorIndex - the array index where this is going
	// iIndexInItem - index in item type
	void AddBufferedProcessInput(EInputDirection eDir, BaseProcessInput* pOriginalInput, int iParentVectorIndex, int iIndexInItem);
	
	// Uses the for type to create internal buffers at linkage time
	void CreateBufferForInputs();

	int m_iNrFinishedInternalPrograms;
};

#endif

