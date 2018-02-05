#ifndef EXEC_STRATEGY_MPI_H
#define EXEC_STRATEGY_MPI_H

#include "../Compiler/ExecutionStrategy_Defines.h"
#if EXECUTION_STRATEGY == USE_MPI_STRATEGY

namespace Streams
{
	class BytesStreamWriter;
};

#include <map>
#include "../ScedulersDistributed/SchedulerCODef.h"
#include "ExecutionStrategy.h"

class ProgramIntermediateModule;

typedef std::map<int, ProgramIntermediateModule*>	MapOfTasks;
typedef MapOfTasks::iterator	MapOfTasksIter;

class ExecutionStrategyConcrete_MPI_Master : public ExecutionStrategy, public SchedulerCO::Master
{
public:
	ExecutionStrategyConcrete_MPI_Master(int iNrWorkers) : SchedulerCO::Master(iNrWorkers) { }
	virtual void OnModuleReadyForExecution(ProgramIntermediateModule* pModule);
	void ExecuteModule(ProgramIntermediateModule* pModule);
	void SerializeModule(ProgramIntermediateModule* pModule, Streams::BytesStreamWriter& writer);
	virtual void OnResultsReceived(char *pDataBuffer, int iDataBufferSize, int iWorkerID);

	virtual void DoWork();

	// Poll for messages
	void Update();

private:
	MapOfTasks	m_TasksInProgress;

};

class ExecutionStrategyConcrete_MPI_Worker : public ExecutionStrategy, public SchedulerCO::Worker
{
public:
	ExecutionStrategyConcrete_MPI_Worker(int rank) : SchedulerCO::Worker(rank) {}
	// Poll for messages
	void OnExecuteTask(SchedulerCO::TaskDef *pTaskDef);
	virtual void DoWork();

	void ExecutionStrategy::OnModuleReadyForExecution(ProgramIntermediateModule *);
};

#endif

#endif // EXECUTION_STRATEGY == USE_MPI_STRATEGY
