#include "DECLARATIONNode.h"
#include "Expressions.h"
#include "ExecutionBlackbox.h"

#include "calc3_utils.h"

void IDataTypeItem::SetName(const char *szName) 
{
	m_szName = _strdup(szName);
}

ProgramBase* ProgramDeclaration::Clone()
{
	ProgramDeclaration* pCloned = new ProgramDeclaration(mDebugLineNo);
	CopyBase(pCloned);

	pCloned->m_szIdentifier = _strdup(m_szIdentifier);
	pCloned->m_pInputItem = m_pInputItem->Clone();
	return pCloned;
}

bool ProgramDeclaration::DoDeclaration(SymbolTable& pSymbolTable)
{
	IDataTypeItem* pItem = pSymbolTable.GetIdentifierFromTable(m_szIdentifier);
	if (pItem != NULL)
	{
		PrintCompileError(mDebugLineNo, "This identifier %s already exits!", m_szIdentifier);
		return false;
	}

	pSymbolTable.AddSymbol(m_szIdentifier, m_pInputItem);
	return true;
}

void ProgramDeclaration::SetIdentifierAndInputItem(char *szIdentifier, IDataTypeItem *pInputItem)
{
	m_szIdentifier = _strdup(szIdentifier);
	m_pInputItem = pInputItem;
}

bool ProgramDeclaration::Validate(SymbolTable* pParent, bool bAtRuntimeSpawn)
{
	if (!pParent->AddSymbol(m_szIdentifier, m_pInputItem))
		return false;

	return true;
}
