#include "ExecutionStrategy.h"
#include "ExecutionStrategyConcrete_MPI.h"
#include "ExecutionStrategyConcrete_Serial.h"
#include "CompilationBlackbox.h"
#include <queue>
#include <assert.h>
#include "INTERMEDIATENode.h"
#include "Streams.h"
#include "CModuleCode.h"

#include "../ScedulersDistributed/SchedulerCODef.h"

#include "../Compiler/ExecutionStrategy_Defines.h"
#if EXECUTION_STRATEGY == USE_MPI_STRATEGY

void ExecutionStrategyConcrete_MPI_Master::OnModuleReadyForExecution(ProgramIntermediateModule* pModule)
{
	// Can be send to workers or only the master can execute this task ?
	if (pModule->m_pCCodeObject->GetType() == E_NODE_TYPE_C_CODE_ZONE_ALL)
	{
		// Send this task to someone.....
		Streams::BytesStreamWriter writer;

		// Serialize it for now only
		SerializeModule(pModule, writer);

		// Put in the task in progress queue
		m_TasksInProgress.insert(std::make_pair((int)pModule, pModule));
		OnTaskCreated(writer.GetBufferStart(), writer.GetAllocatedSize());
	}
	else
	{
		ExecuteModule(pModule);
	}
}

void ExecutionStrategyConcrete_MPI_Master::SerializeModule(ProgramIntermediateModule* pModule, Streams::BytesStreamWriter& writer)
{
	// Compute the serialized size of the module
	unsigned int iSerializedSize = sizeof(int) + pModule->m_pInputNorth->GetSerializedSize() + pModule->m_pInputWest->GetSerializedSize();
	int iModuleNameLen = strlen(pModule->m_szModuleName);
	iSerializedSize += iModuleNameLen + sizeof(int);
	writer.Alloc(iSerializedSize);

	// Write the ID of the message
	writer.WriteSimpleType<int>((int)pModule);

	// Write the text of the function (task) to execute
	writer.WriteSimpleType<int>(iModuleNameLen);
	writer.WriteByteArray(pModule->m_szModuleName, iModuleNameLen);

	// Write the North and West modules
	pModule->m_pInputNorth->Serialize(writer);
	pModule->m_pInputWest->Serialize(writer);
}

void ExecutionStrategyConcrete_MPI_Master::ExecuteModule(ProgramIntermediateModule* pModule)
{
	ExecutionStrategyConcrete_Serial::ExecuteModule(pModule);
	ExecutionStrategyConcrete_Serial::SendModuleOutputResults(pModule);
}

void ExecutionStrategyConcrete_MPI_Master::OnResultsReceived(char *pDataBuffer, int iDataBufferSize, int iWorkerID)
{
	Streams::BytesStreamReader	reader;
	reader.SetWorkingBuffer(pDataBuffer, iDataBufferSize);

	int iModuleInstanceID;
	reader.ReadSimpleType<int>(iModuleInstanceID);
	
	// Call master function first
	Master::OnResultsReceived(pDataBuffer, iDataBufferSize, iWorkerID);

	MapOfTasksIter it = m_TasksInProgress.find(iModuleInstanceID);

	if (it == m_TasksInProgress.end())
	{
		assert(false && "Result of a task not found");
		return;
	}

	// Deserialize the input received
	ProgramIntermediateModule* pModule = it->second;	
	pModule->m_pOutputSouth->Deserialize(reader);
	pModule->m_pOutputEast->Deserialize(reader);

	// Remove it from tasks in progress map
	m_TasksInProgress.erase(it);

	// Send the output results
	ExecutionStrategyConcrete_Serial::SendModuleOutputResults(pModule);
}

void ExecutionStrategyConcrete_MPI_Master::DoWork()
{
	// Init the main module
	DoMainModuleInputInitialization();	

	// Do job as master
	OnUpdate();

		// Execute the task graph
	while(IsAnyTaskInProgress())
		OnUpdate();

	// Start and wait for termination to end
	StartTermination();
	while(IsTerminationFinished())
		OnUpdate();

	MPI_Finalize();
}

//////////////////// ------------------------- WORKER ------------------------- //////////////////////////

void ExecutionStrategyConcrete_MPI_Worker::DoWork()
{
	OnUpdate();
	MPI_Finalize();
}

void ExecutionStrategyConcrete_MPI_Worker::OnExecuteTask(SchedulerCO::TaskDef *pTaskDef)
{
	//----
	char* pTaskData = (char*)pTaskDef->pTaskData;
	int iTaskDataSize = pTaskDef->iSizeOfTask;

	//---
	Streams::BytesStreamReader	reader;
	reader.SetWorkingBuffer(pTaskData, iTaskDataSize);

	// Deserialize the taskID and the task module name
	int iTaskID = 0, iNrCharsOfModuleName = 0;
	reader.ReadSimpleType<int>(iTaskID);
	reader.ReadSimpleType<int>(iNrCharsOfModuleName);
	char *szModuleName = new char[iNrCharsOfModuleName + 1];
	reader.ReadByteArray(szModuleName, iNrCharsOfModuleName);
	szModuleName[iNrCharsOfModuleName] = '\0';

	// Find and clone an instance by name
	ProgramIntermediateModule* pModule = CompilationBlackbox::Get()->GetModuleByName(szModuleName);
	assert(pModule != NULL);
	ProgramIntermediateModule* pClonedModuleInstance = (ProgramIntermediateModule*) pModule->Clone();

	// Deserialize the buffer data into the input modules of the cloned module
	pClonedModuleInstance->m_pInputNorth->Deserialize(reader);
	pClonedModuleInstance->m_pInputWest->Deserialize(reader);

	// Execute the module now
	ExecutionStrategyConcrete_Serial::ExecuteModule(pClonedModuleInstance);

	// Serialize the results and send back
	// [ TaskID, SouthData, WestData]
	Streams::BytesStreamWriter writer;
	writer.Alloc(sizeof(int) + pClonedModuleInstance->m_pOutputSouth->GetSerializedSize() + pClonedModuleInstance->m_pOutputEast->GetSerializedSize());
	writer.WriteSimpleType<int>(iTaskID);
	pClonedModuleInstance->m_pOutputSouth->Serialize(writer);
	pClonedModuleInstance->m_pOutputEast->Serialize(writer);

	// Send writer.GetBufferStart to module 0
	// TODO
	OnSendResults(writer.GetBufferStart(), writer.GetAllocatedSize());
}

void ExecutionStrategyConcrete_MPI_Worker::OnModuleReadyForExecution(ProgramIntermediateModule *)
{
	assert(false && "Shouldn't be called!\n");
}

#endif // EXECUTION_STRATEGY == USE_MPI_STRATEGY
