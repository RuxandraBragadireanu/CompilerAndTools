#ifndef EXECUTION_STRATEGY_H
#define EXECUTION_STRATEGY_H

#include "ExecutionStrategy_Defines.h"

class ProgramIntermediateModule;

class ExecutionStrategy
{
public:
	virtual void OnModuleReadyForExecution(ProgramIntermediateModule* pModule) = 0;

	// Do things before starting the initial work
	void DoMainModuleInputInitialization();
	virtual void DoWork() = 0;
};

#endif
