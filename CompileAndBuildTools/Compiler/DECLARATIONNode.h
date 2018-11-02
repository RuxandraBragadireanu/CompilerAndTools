#ifndef DECLARATION_NODE_H
#define DECLARATION_NODE_H

#include "BaseNode.h"

class ProgramDeclaration : ProgramBase
{
public:
	ProgramDeclaration(int lineNo) : ProgramBase(E_NODE_TYPE_DECLARATION, lineNo), m_pInputItem(NULL) {}
	~ProgramDeclaration() {}

	// Should add in the pSymbolTable this declaration
	virtual bool DoDeclaration(SymbolTable& pSymbolTable);
	void SetIdentifierAndInputItem(char *szIdentifier, IDataTypeItem *pInputItem);

	virtual void OnChildProgramFinished(ProgramBase* pChildFinished) {  }

	virtual bool Validate(SymbolTable* pParent, bool bAtRuntimeSpawn);

	virtual ProgramBase* Clone();
	virtual bool SolveReferences() { return true; }
	virtual bool SolveInputOutput() { return true; }

	char* m_szIdentifier;
	IDataTypeItem*	m_pInputItem;
};

#endif