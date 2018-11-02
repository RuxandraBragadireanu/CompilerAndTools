#ifndef EXECUTION_CONTEXT
#define EXECUTION_CONTEXT

#include <map>
#include <string>
#include <set>
#include <list>

class ProgramIntermediateModule;
class ProgramBase;

typedef std::map<std::string, ProgramIntermediateModule*>	ModulesHash;
typedef ModulesHash::iterator ModulesHashIter;

typedef std::set<char*>	SetOfStrings;
typedef SetOfStrings::iterator SetOfStringsIter;

typedef std::list<ProgramBase*>	ListOfPrograms;
typedef ListOfPrograms::const_iterator ListOfProgramsIterConst;

class CompilationBlackbox
{
public:
	void Destroy();
	static CompilationBlackbox* Get();

	void AddModule(const char *szName, ProgramIntermediateModule* pModule);
	ProgramIntermediateModule* GetModuleByName(const char *szName);
	bool DoCompileSteps(int executionType);

	// The main module
	ProgramIntermediateModule* m_pEntryPoint;

	// Iterated modules functionality
	SetOfStrings m_IteratedModules;
	void ClearIteration();
	bool IsModuleInSet(char* szModuleName);
	void InsertModuleName(char* szModuleName);

	void AddInputFreeProgram(ProgramBase* pProgram);
	const ListOfPrograms& GetListOfFreeModules() { return m_ListOfInputFreeModules; }

	bool WriteCCodeFile();

	void SaveAST();
	void LoadAST();

	void CleanupCompileFiles();

private:
	CompilationBlackbox(){}
	static CompilationBlackbox* gpInstance;

	// This is a hash with pair (name of the module, pointer to the module)
	ModulesHash			m_ModulesHash;

	// A list of all programs without input, that should/may be executed at the beggining of the program
	ListOfPrograms		m_ListOfInputFreeModules;
};

#endif