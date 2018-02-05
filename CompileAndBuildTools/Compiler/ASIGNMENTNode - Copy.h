#ifndef ASIGNMENT_NODE_H
#define ASIGNMENT_NODE_H

#include "BaseNode.h"

class ProgramAsignment : public ProgramBase
{
public:
	ProgramAsignment(int lineNo) : ProgramBase(E_NODE_TYPE_ASIGNMENT, lineNo), m_pInputItem(NULL){}

	void SetChilds(IExpression *pChild0, IExpression *pChild1) { m_ChildExpressions[0] = pChild0; m_ChildExpressions[1] = pChild1;}
	virtual void DoAsignment(SymbolTable& pSymbolTable);

	virtual bool Validate(SymbolTable* pParent, bool bAtRuntimeSpawn);

	virtual ProgramBase* Clone();
	virtual bool SolveReferences() { return true; }
	virtual bool SolveInputOutput() { return true; }
	virtual void OnChildProgramFinished(ProgramBase* pChildFinished) { SendProgramFinished(this); }

	IExpression* m_ChildExpressions[2];
	IDataTypeItem*	m_pInputItem;
};

#endif