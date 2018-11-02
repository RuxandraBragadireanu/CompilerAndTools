#include "ExecutionStrategyConcrete_Serial.h"

//TODO: This should be auto generated
#include "AgapiaToCCode.h"
// TODO

#include "ExecutionBlackbox.h"
#include "INTERMEDIATENode.h"
#include <stdio.h>
#include "calc3_utils.h"

#define IS_BFS_RUN_ENABLED	// Comment this to run in DFS (bigger stack, not practical but might be easier to debug)

void ExecutionStrategyConcrete_Serial::OnModuleReadyForExecution(ProgramIntermediateModule* pModule)
{
	// Put the module on the queue...
	m_ReadyForExecutionModules.push(pModule);

#ifndef IS_BFS_RUN_ENABLED
	// Like in a DFS run, take it and execute :)
	ProgramIntermediateModule* pModuleRef = m_ReadyForExecutionModules.front();
	m_ReadyForExecutionModules.pop();

	ExecuteModule(pModuleRef);
	SendModuleOutputResults(pModuleRef);
#endif
}

void ExecutionStrategyConcrete_Serial::ExecuteModule(ProgramIntermediateModule* pModule)
{
	AgapiaToCFunction func = ExecutionBlackbox::Get()->GetAgapiaToCFunction(pModule->m_szModuleName);
	if (func == NULL)
	{
		char buff[1024];
		sprintf(buff, "Module-Function named %s couldn't be found\n", pModule->m_szModuleName);
		PrintExecutionError(buff);
		assert(false);
	}
	(*func)(pModule->m_pInputNorth, pModule->m_pInputWest, pModule->m_pOutputSouth, pModule->m_pOutputEast);
}

void ExecutionStrategyConcrete_Serial::SendModuleOutputResults(ProgramIntermediateModule* pModule)
{
		// TODO others:
	// Distribute the modules outputs
	InputBlock* pOutputs[2] = { pModule->m_pOutputSouth, pModule->m_pOutputEast};
	for (int i = 0; i < 2; i++)
	{
		for (ArrayOfBaseProcessInputsIter it = pOutputs[i]->m_InputsInBlock.begin(); it != pOutputs[i]->m_InputsInBlock.end(); it++)
		{
			//assert((*it)->m_pNext != NULL && "Are you sure you don't want "
			if ((*it)->m_pNext)
				(*it)->m_pNext->GoDownValue(*it);
		}		
	}

	pModule->SendProgramFinished(pModule);

	// How the AgapiaToCCode is written - 
}

void ExecutionStrategyConcrete_Serial::DoWork()
{
	DoMainModuleInputInitialization();

#ifdef IS_BFS_RUN_ENABLED
	while(!m_ReadyForExecutionModules.empty())
	{
		ProgramIntermediateModule* pModuleRef = m_ReadyForExecutionModules.front();
		m_ReadyForExecutionModules.pop();

		ExecuteModule(pModuleRef);
		SendModuleOutputResults(pModuleRef);
	}
#endif
}

