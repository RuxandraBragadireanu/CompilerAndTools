#include "ReferenceNode.h"
#include "INTERMEDIATENode.h"
#include "CompilationBlackbox.h"

#include "calc3_utils.h"

ProgramBase* ProgramReference::FindAndCloneRef(ProgramReference* pRef)
{
	ProgramBase* pReferencedModule = CompilationBlackbox::Get()->GetModuleByName(pRef->m_strModuleReference);
	if (pReferencedModule == NULL)
	{
		PrintCompileError(pRef->mDebugLineNo, "Module called %s not found", pRef->m_strModuleReference);
		return NULL;
	}

	return pReferencedModule->Clone();
}
