#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <map>
#include <string>

class IDataTypeItem;
class ProgramBase;

typedef std::pair<std::string, IDataTypeItem*> SymbolEntryPair;
typedef std::map<std::string, IDataTypeItem*> VariablesEntries;
typedef VariablesEntries::iterator VariableEntriesIter;
typedef VariablesEntries::const_iterator VariableEntriesConstIter;
class SymbolTable
{
public:
	SymbolTable(ProgramBase* parent) { mParent = parent; }

	// To use on declarations
	bool AddSymbol(char* szIdentifier, void* pItem);

	// To use on symbol table inheritance
	bool InheritTable(const SymbolTable& table);

	// Check if an identifier is in the table.
	// Returns NULL if not found in table or 
	IDataTypeItem* GetIdentifierFromTable(char* szIdentifier) const;
	void SetIdentifierAddress(IDataTypeItem* pNewAddress, char* szIdentifier);

	void Clear() { m_Symbols.clear(); }
	SymbolTable* Clone(ProgramBase* newParent);
	ProgramBase* GetParent() { return mParent; }

	int GetNumSymbols() { return m_Symbols.size(); }
	void BeginSymbolsIteration() { m_SymbolsIter = m_Symbols.begin();}
	bool HasNext() { return m_SymbolsIter!=m_Symbols.end(); }
	const SymbolEntryPair GetNextSymbol();
private:
	VariablesEntries	m_Symbols;
	VariableEntriesIter m_SymbolsIter;
	ProgramBase*		mParent;
};

#endif