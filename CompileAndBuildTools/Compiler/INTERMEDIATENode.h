#ifndef INTERMEDIATE_NODE_H
#define INTERMEDIATE_NODE_H

#include "BaseNode.h"

class CModuleCode;

// A node can have at maximum 2 child with an operator (single operator dominance rule)
class ProgramIntermediateModule : public ProgramBase
{
public:
	ProgramIntermediateModule(int lineNo)
		: ProgramBase(E_NODE_TYPE_INTERMEDIATE, lineNo), m_iNrChilds(0), m_operator(E_COMP_UNDEFINED), m_bIsAtomic(false), 
		m_bIsAllInputReceived(false), m_strModuleReference(NULL), m_bIsUserDefined(false), m_pCCodeObject(NULL), m_szModuleName(NULL)
	{
		m_pChilds[0] = m_pChilds[1] = NULL;
		m_bChildFinished[0] = m_bChildFinished[1] = false;
	}

	virtual ProgramBase* Clone();

	void SetOperationAndChilds(ECompositionOperator eOperator, ProgramBase* pChild1, ProgramBase* pChild2);

	virtual bool SolveReferences();
	virtual bool SolveInputOutput();
	virtual bool Validate(SymbolTable* pParent, bool bAtRuntimeSpawn);

	virtual void ComputeNumInputsToReceive();

	// This should be called to create input linkage and all kinds of stuff depending by node
	virtual bool OnInputReceived(BaseProcessInput* pOriginalInput, BaseProcessInput* pInputNodeReceiver, EInputDirection eDir);
	// This should be called after the module has been linked and input assigned
	virtual void OnAfterInputReceived();

	virtual bool IsAtomicProgram() { return m_bIsAtomic; }

	virtual void OnChildProgramFinished(ProgramBase* pChildFinished);

	virtual const char* GetName() { return m_szModuleName; }

	// child for the node
	ProgramBase*			m_pChilds[2];

	// Number of child of this node
	int						m_iNrChilds;

	// True if child finished or doesn't exist, false otherwise
	bool					m_bChildFinished[2];

	// operator for composition
	ECompositionOperator	m_operator;

	// True if this node is atomic
	bool m_bIsAtomic;
	// Not null if atomic - this must not be copied !
	CModuleCode*	m_pCCodeObject;

	// Is user defined module ?
	bool m_bIsUserDefined;

	// True if all input needed was received
	bool m_bIsAllInputReceived;

	// This is not null only if this node refers to a module name!
	char *m_strModuleReference;

	// Only if it's a user defined module
	char *m_szModuleName;
};


#endif