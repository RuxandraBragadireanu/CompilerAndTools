#ifndef IF_NODE_H
#define IF_NODE_H

#include "BaseNode.h"
#include "CommonTypes_Conditionals.h"

class BufferedInputBlock;

class ProgramIF : ProgramBase
{
public:
	ProgramIF(int lineNo) 
		: ProgramBase(E_NODE_TYPE_IF, lineNo)
		, m_bConditionEvaluated(false)
		{
			m_BufferInputBlocks[0] = m_BufferInputBlocks[1] = NULL;
		}  


	virtual ProgramBase* Clone();
	virtual bool SolveReferences();
	virtual bool SolveInputOutput();

	virtual bool Validate(SymbolTable* pParent, bool bAtRuntimeSpawn);

	// Expected inputs functionality
	//------------------------------------------
	// Called when we receive an input. SAR_NEEDED_AND_FINAL if the condition can be evaluated
	SymbolArrivedResult OnSymbolArrived(char *szName);
	ExpectedConditionInputs	m_ExpectedInputsMap;

	// Called when all inputs necessary for condition evaluation are in the symbol table. 
	// Now, it's time to decide what program to choose for execution inside if and to update the links
	void OnAllSymbolsArrived();

	// Returns true when the other symbols from pOriginalInput makes no relevance so we should stop processing
	virtual bool OnInputReceived(BaseProcessInput* pOriginalInput, BaseProcessInput* pInputNodeReceiver, EInputDirection eDir);
	
	// For input items listeners outside this if module
	virtual void OnInputItemReady(char* szName);

	virtual void OnChildProgramFinished(ProgramBase* pChildFinished);

	// This is one of the if / else programs. If this is NULL, it means that the condition hasn't been evaluated
	//ProgramBase*	m_pProgramToChoose;
	//------------------------------------------

	IExpression*	m_pCondition;
	ProgramBase*	m_pIfProgram;
	ProgramBase*	m_pElseProgram;

	// This is true if the decision was made and condition was evaluated
	bool m_bConditionEvaluated;

private:	
	void CreateInputBuffers();

	// This is used to buffer the input received by the IF when we didn't take any decision to which program to choose
	BufferedInputBlock*	m_BufferInputBlocks[E_NUM_DIRECTION];
};

#endif