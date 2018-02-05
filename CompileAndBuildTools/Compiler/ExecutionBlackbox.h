#ifndef EXECUTION_BLACKBOX_H
#define EXECUTION_BLACKBOX_H

#include <map>
#include <string>

class ProgramIntermediateModule;
class ExecutionStrategy;
class InputBlock;
class MainInputParser;

#include "Common\ExecutionCommon.h"

class ExecutionBlackbox
{
public:
	// Call this to create the singleton object
	static void Initialize();
	static void Sync();

	static void DoWork();

	// Make global initialization depending on the strategy used
	static void PreInitStrategy(int argc, char *argv[]);

	static void ShutdownNodeIfNotNeeded();

	static void Destroy();
	static void ParseMainInput();
	static ExecutionBlackbox* Get() { return gInstance; }

	// This will launch tasks on workers !
	void OnModuleReadyForExecution(ProgramIntermediateModule* pProgram);

	// Used to add C code blocks
	void AddAgapiaToCFunction(const char* szName, AgapiaToCFunction);
	AgapiaToCFunction GetAgapiaToCFunction(const char *szName);


private:
	ExecutionBlackbox() {}
	static ExecutionBlackbox* gInstance;

	// This is used to map AGAPIA to C functions to strings
	MapOfAgapiaToCFunctions m_AgapiaToCFunctions;

	// Current strategy used
	ExecutionStrategy* m_pStrategy;

	// Tool for parsing the input
	static MainInputParser* m_pInputParserTool;
};

#endif
