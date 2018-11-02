#include "SymbolTable.h"
#include "InputTypes.h"
#include "calc3_utils.h"

bool SymbolTable::AddSymbol(char* szIdentifier, void* pItem)
{
	std::string str(szIdentifier);
	VariableEntriesIter itVar = m_Symbols.find(szIdentifier);
	if (itVar != m_Symbols.end() && (itVar->second != pItem))
	{
		PrintCompileError(GetParent()->mDebugLineNo, "Symbol %s already exist in program %s", (char*)itVar->first.c_str(), GetParent()->GetName());
		return false;
	}

	m_Symbols.insert(std::make_pair(str, (IDataTypeItem*)pItem));
	return true;
}

bool SymbolTable::InheritTable(const SymbolTable& table)
{
	for (VariableEntriesConstIter it = table.m_Symbols.begin(); it != table.m_Symbols.end(); it++)
		if (!AddSymbol((char*)it->first.c_str(), it->second))
			return false;

	return false;
}

IDataTypeItem* SymbolTable::GetIdentifierFromTable(char* szIdentifier) const
{
	std::string str(szIdentifier);
	VariableEntriesConstIter it = m_Symbols.find(str);
	if (it == m_Symbols.end())
		return NULL;

	return it->second;
}

void SymbolTable::SetIdentifierAddress(IDataTypeItem* pNewAddress, char* szIdentifier)
{
	std::string str(szIdentifier);
	VariableEntriesIter it = m_Symbols.find(str);
	if (it == m_Symbols.end())
	{
		assert(false && "You should call this only on duplicated identifiers!");
	}

	it->second = pNewAddress;
}

SymbolTable* SymbolTable::Clone(ProgramBase* newParent)
{
	SymbolTable* pCopy = new SymbolTable(newParent);
	for (VariableEntriesConstIter it = m_Symbols.begin(); it != m_Symbols.end(); it++)
	{
		IDataTypeItem* pDataItem = it->second;//it->second->Clone();
		pCopy->AddSymbol((char*)it->first.c_str(), pDataItem);
	}

	return pCopy;
}

const SymbolEntryPair SymbolTable::GetNextSymbol()
{
	if (m_SymbolsIter == m_Symbols.end())
		return *m_Symbols.begin();
	else
	{
		SymbolEntryPair pPair = *m_SymbolsIter;
		m_SymbolsIter++;
		return pPair;
	}
	// CONTINUE
}