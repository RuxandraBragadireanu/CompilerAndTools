#ifndef REFERENCE_NODE_H
#define REFERENCE_NODE_H

#include "BaseNode.h"

class ProgramReference : public ProgramBase
{
public:
	ProgramReference(int lineNo)
		: ProgramBase(E_NODE_TYPE_MODULE_REF, lineNo), m_strModuleReference(NULL) {}

	virtual ProgramBase* Clone() { return NULL; }
	static ProgramBase* FindAndCloneRef(ProgramReference* pRef);
	virtual bool SolveReferences() { return false; }
	virtual bool SolveInputOutput() { return true; }

	virtual bool Validate(SymbolTable* pParent, bool bAtRuntimeSpawn, bool bTemplateMOdule) override { return false; }
	virtual void OnChildProgramFinished(ProgramBase* pChildFinished) {}

	// This is not null only if this node refers to a module name!
	char *m_strModuleReference;
};

class ProgramIdentity : public ProgramBase
{
public:
	ProgramIdentity(int lineNo)
		: ProgramBase(E_NODE_TYPE_IDENTITY, lineNo){}

	virtual ProgramBase* Clone() { return new ProgramIdentity(mDebugLineNo); }
	virtual bool SolveReferences() { return true; }
	virtual bool SolveInputOutput() { return true; }

	virtual bool Validate(SymbolTable* pParent, bool bAtRuntimeSpawn, bool bTemplateMOdule) override { return true; }
	virtual void OnChildProgramFinished(ProgramBase* pChildFinished) {}
};

class ProgramCCodeType : public ProgramBase
{
public:
	ProgramCCodeType(int iType, int lineNo)
		: ProgramBase(iType == E_CCODE_FORMASTER ? E_NODE_TYPE_C_CODE_ZONE_MASTER : E_NODE_TYPE_C_CODE_ZONE_ALL, lineNo) {}

	virtual ProgramBase* Clone() { return new ProgramCCodeType(m_eNodeType, mDebugLineNo); }
	virtual bool SolveReferences() { return false; }
	virtual bool SolveInputOutput() { return true; }

	virtual bool Validate(SymbolTable* pParent, bool bAtRuntimeSpawn, bool bTemplateMOdule) override { return true; }
	virtual void OnChildProgramFinished(ProgramBase* pChildFinished) {}

	// This is not null only if this node refers to a module name!
};


#endif
