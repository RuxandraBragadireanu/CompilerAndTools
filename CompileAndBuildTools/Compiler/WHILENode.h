#ifndef WHILE_NODE_H
#define WHILE_NODE_H

#include "BaseNode.h"
#include <queue>
#include "CommonTypes_Conditionals.h"

enum EWHILEType
{
	E_WHILE_UNDEFINED = -1,
	E_WHILE_S = DTYPE_WHILE_S,
	E_WHILE_T = DTYPE_WHILE_T,
	E_WHILE_ST = DTYPE_WHILE_ST,
};

typedef std::queue<BaseProcessInput*> QueueOfInputsProcesses;
#include <set>
typedef std::set<std::string> ExpectedConditionInputs;
typedef ExpectedConditionInputs::iterator ExpectedConditionInputsIter;

class ProgramWHILE : public ProgramBase
{
public:
	ProgramWHILE(int lineNo) 
		: ProgramBase(E_NODE_TYPE_WHILE, lineNo)
		, m_eWHILEType(E_WHILE_UNDEFINED)
		, m_iIterationIdx(-1)
		, m_pIterationProgram(NULL)
		, m_iNrInputsNeededInNorth(0)
		, m_iNrInputsNeededInWest(0)
		, m_iNrInputsInBufferInNorth(0)
		, m_iNrInputsInBufferInWest(0)
		, m_bIsLastInputSent(false)
		, m_bIsWhileModuleFinishing(0)
	{
		//m_BufferInputBlocks[0] = m_BufferInputBlocks[1] = NULL;
	}  


	virtual ProgramBase* Clone();
	virtual bool SolveReferences();
	virtual bool SolveInputOutput();

	virtual bool Validate(SymbolTable* pParent, bool bAtRuntimeSpawn);

	// This will return true when the last input from a buffered input is sent. 
	bool IsLastArrayInputSent() { return m_bIsLastInputSent; }

	// True when is transferring the last internal module output to the end
	bool IsWhileModuleFinishing() { return m_bIsWhileModuleFinishing ;}

	// Create and link a new internal child
	void CreateAndLinkNewInternalChild();

	// Expected inputs functionality
	//------------------------------------------
	// Ready for a condition check and eventually a new iteration of the child program
	void OnAllSymbolsArrived();

	// Returns true when we are ready to spawn, check condition and execute the inside module
	virtual bool OnInputReceived(BaseProcessInput* pOriginalInput, BaseProcessInput* pInputNodeReceiver, EInputDirection eDir);
	SymbolArrivedResult OnSymbolArrived(char *szName);
	ExpectedConditionInputs	m_ExpectedInputsMap;

	void OnChildProgramFinished(ProgramBase* pChildFinished);

	// Send the input buffered in north and west sides
	// If the parameter IsWhileEnding is true when the while will end
	void SendBufferedInputs(bool IsWhileEnding = false);

	// Checks the condition if it should create a new child or not. 
	// If yes, then it will make the correct linkage and send the input buffered
	// Otherwise, it will make the connection from input to output WHILE and send to the exit the input buffered
	void CheckIfShouldSpawnNewChild();

	// Verifies and launches the first iteration child if the conditions are met
	void VerifyFirstIterationSpawn();

	virtual bool DoSpecificValidationOrSymbolInherit(SymbolTable* pParent);

	IExpression*	m_pCondition;
	ProgramBase*	m_pBaseProgram;	

	EWHILEType m_eWHILEType;

	virtual bool IsSpatialIteration()  { return m_eWHILEType == E_WHILE_S; }
	virtual bool IsTemporalIteration() { return m_eWHILEType == E_WHILE_T; }
	virtual bool IsDiagonalIteration() { return m_eWHILEType == E_WHILE_ST; }
	virtual void OnBeforeFirstTimeConditionEval() { }
	virtual void OnAfterChildProgramFinished() {} 

protected:

	// This is used to buffer the input received by the FOR when it wasn't expanded
	QueueOfInputsProcesses	m_BufferInputBlocks[E_NUM_DIRECTION];

	// This is used to buffer the input received by from the upper program and inside one.
	// At the end it will serve for the output outside the WHILE module.
	// Every instance of inside module will send it's output to this buffer.
	QueueOfInputsProcesses m_QueueOfReceivedItemsArrayType;

	// The index of iteration
	int m_iIterationIdx;

	// The current internal program in execution in this WHILE. If it's NULL, then there is no program inside spawned
	ProgramBase*	m_pIterationProgram;

	// The number of inputs needed for north and west to create another inside module
	int m_iNrInputsNeededInNorth;
	int m_iNrInputsNeededInWest;

	// Used only for the first inside module instantiation. When the number of inputs required (defined above) is met, we can spawn the first module inside
	int m_iNrInputsInBufferInNorth;
	int m_iNrInputsInBufferInWest;

	// True when the last input from any output interface is sent
	bool m_bIsLastInputSent;

	// True when the while module is finishing
	bool m_bIsWhileModuleFinishing;

	ExpectedConditionInputs m_expectedInputMap;
};

#endif
