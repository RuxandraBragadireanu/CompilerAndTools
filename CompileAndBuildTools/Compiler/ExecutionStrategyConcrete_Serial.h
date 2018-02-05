#ifndef EXEC_SERIAL_H
#define EXEC_SERIAL_H

#include "ExecutionStrategy.h"
#include <queue>

class ProgramIntermediateModule;

class ExecutionStrategyConcrete_Serial : public ExecutionStrategy
{
public:
	virtual void OnModuleReadyForExecution(ProgramIntermediateModule* pModule);

	// Call the correct function for a module
	static void ExecuteModule(ProgramIntermediateModule* pModule);

	// Sends the results from a computed module through the hypergraph
	static void SendModuleOutputResults(ProgramIntermediateModule* pModule);

	virtual void DoWork();

private:
	std::queue<ProgramIntermediateModule*>	m_ReadyForExecutionModules;
};

#endif