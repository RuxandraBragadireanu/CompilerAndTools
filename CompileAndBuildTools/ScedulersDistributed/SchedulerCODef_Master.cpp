#include "../Compiler/ExecutionStrategy_Defines.h"
#if EXECUTION_STRATEGY == USE_MPI_STRATEGY

#include "stdafx.h"
#include "SchedulerCODef.h"
#include <string.h>
#include <assert.h>
#include "SchedulerUtils.h"

namespace SchedulerCO
{

Master::Master(int iNrWorkers)
: m_iNumberOfWorkers(iNrWorkers)
, m_bTerminationInitialized(false)
, m_iProcsRemainsToTerminate(0)
, m_iTotalTasksAssignedToWorkers(0)
{
	m_arrTasksAssignedPerWorker = new int[iNrWorkers + 1];
	memset(m_arrTasksAssignedPerWorker, 0, sizeof(int) * (iNrWorkers + 1));
}

Master::~Master()
{
	if (m_arrTasksAssignedPerWorker)
	{
		delete m_arrTasksAssignedPerWorker;
		m_arrTasksAssignedPerWorker = NULL;
	}
}

void Master::OnTaskGivenToProc(int iProcID)
{
	LOG(("Worker %d is given a task\n", iProcID)); 

	assert(0 <= iProcID && iProcID <= m_iNumberOfWorkers);
	m_arrTasksAssignedPerWorker[iProcID]++;
	m_iTotalTasksAssignedToWorkers++;
}


void Master::OnResultsReceived(char *pDataBuffer, int iDataBufferSize, int iWorkerID)
{
	LOG(("Worker %d finished a task\n", iWorkerID)); 

	assert(0 <= iWorkerID && iWorkerID <= m_iNumberOfWorkers);
	m_arrTasksAssignedPerWorker[iWorkerID]--;
	assert(m_arrTasksAssignedPerWorker[iWorkerID] >= 0);
	m_iTotalTasksAssignedToWorkers--;
	assert(m_iTotalTasksAssignedToWorkers >= 0);
}

void Master::OnProcesorTerminationConfirmed(int iProcID)
{
	m_iProcsRemainsToTerminate--;

	LOG(("New processor finished termination, remains: %d\n", iProcID)); 
}

void Master::StartTermination()
{
	LOG(("Master starting termination...\n")); 

	m_bTerminationInitialized = true;

	// Send message to all workers to terminate their job
	for (int iProcIter = 1; iProcIter <= m_iNumberOfWorkers; iProcIter++)
	{
		// Prepare a dummy message and put into the send list
		TaskDef* pNewTask = new TaskDef();
		pNewTask->pTaskData = new char[1];
		pNewTask->iSizeOfTask = 1;
		pNewTask->iTaskProcDestination = iProcIter;

		char c = 't';
		memcpy(pNewTask->pTaskData, &c, 1);
		m_TasksInSendProgress.push_back(pNewTask);

		// Send the message
		LOG(("Master is sending termination to processor %d\n", iProcIter)); 

		MPI_Isend(pNewTask->pTaskData, pNewTask->iSizeOfTask, MPI_CHAR, iProcIter, TERMINATION_TAG, MPI_COMM_WORLD, &pNewTask->request);
	}
}

int Master::GetProcessorToExecuteTask(int iEstimatedRank, int iEstimatedTime)
{
	//	 TODO OPTIMIZE ME: WHEN SELECTING THE LOWEST OCCUPIED PROCESSOR, USE A HEAP STRUCTURE !!!!
	// For now, I use a O(N) method stupid

	int iMinWorkload = m_arrTasksAssignedPerWorker[1];
	int iMinProc = 1;
	for (int i = 2; i <= m_iNumberOfWorkers; i++)
		if (m_arrTasksAssignedPerWorker[i] < iMinWorkload)
		{
			iMinWorkload = m_arrTasksAssignedPerWorker[i];
			iMinProc = i;
		}

	return iMinProc;
}

void Master::OnTaskCreated(void* pTaskData, int iTaskSize)
{
	if (m_bTerminationInitialized)
	{
		assert(false && "Can't send it");
		printf("Error: TERMINATION INITIATED AND WE WANT CREATE A NEW TASK ???\n");
		return;
	}

	TaskDef* pNewTask = new TaskDef();
	pNewTask->pTaskData = new char[iTaskSize];
	memcpy(pNewTask->pTaskData, pTaskData, iTaskSize);
	
	pNewTask->iSizeOfTask = iTaskSize;
		
	//TODO: 
	int iProcID = GetProcessorToExecuteTask(gOMOGEN_RANK, gOMOGEN_EST_TIME);
	pNewTask->iTaskProcDestination = iProcID;

#ifdef CAN_MASTER_EXECUTE_TASKS
	if (pid == 0)
	{
		m_OwnTasksList.push_back(pTask);
	}
	else
#endif
	{
		LOG(("Master is sending task with size %d to processor %d\n", iTaskSize, iProcID)); 
		OnTaskGivenToProc(iProcID);

		m_TasksInSendProgress.push_back(pNewTask);
		MPI_Isend(pNewTask->pTaskData, pNewTask->iSizeOfTask, MPI_CHAR, iProcID, NEW_TASK_TAG, MPI_COMM_WORLD, &pNewTask->request);
	}
}

void Master::UpdateSendingInProgressTasks()
{
	for (ListOfTasksIter it = m_TasksInSendProgress.begin(); it != m_TasksInSendProgress.end(); )
	{
		// Test if this operation is completed or not
		int flag = 0;
		MPI_Status status;
		MPI_Test(&(*it)->request, &flag, &status);
		if (flag == 1)	// It's completed ?
		{
			LOG(("Master successfully sent message with size %d to processor %d\n", (*it)->iSizeOfTask, (*it)->iTaskProcDestination)); 

			ListOfTasksIter nextIt = it;
			delete (*it);

			nextIt++;
			m_TasksInSendProgress.erase(it);			
			it = nextIt;
		}
		else
		{
			it++;
		}
	}
}

void Master::CheckReceivedMessages()
{
	int flag;
	MPI_Status status;
	MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);

	// Nothing received ?
	if (flag == 0)
		return;	

	// We have a message, let's process it!
	//----------------------------------------
	// Responded to a new task with results ?
	switch(status.MPI_TAG)
	{
		case NEW_TASK_TAG:
			{
				// Use data here
				int receivedSizeInBytes = 0;
				MPI_Get_count(&status, MPI_CHAR, &receivedSizeInBytes);

				char * pData = new char[receivedSizeInBytes];
				MPI_Recv(pData, receivedSizeInBytes, MPI_CHAR, status.MPI_SOURCE, NEW_TASK_TAG, MPI_COMM_WORLD, &status);
				OnResultsReceived(pData, receivedSizeInBytes, status.MPI_SOURCE);
			}
			break;
		case TERMINATION_TAG:
			{
				char data[20];
				MPI_Recv(data, 1, MPI_CHAR, status.MPI_SOURCE, TERMINATION_TAG, MPI_COMM_WORLD, &status);
				OnProcesorTerminationConfirmed(status.MPI_SOURCE);
			}
			break;
		default:
			assert(false);
	}
}

void Master::OnUpdate()
{
	UpdateSendingInProgressTasks();
	CheckReceivedMessages();
}

bool Master::IsAnyTaskInProgress()
{
	int iTotalTasks = m_iProcsRemainsToTerminate + m_TasksInSendProgress.size() + m_iTotalTasksAssignedToWorkers
#ifdef CAN_MASTER_EXECUTE_TASKS
		+ m_OwnTasksList.size()
#endif
		;

	return (iTotalTasks > 0);
}

} // namespace SchedulerCO

#endif // EXECUTION_STRATEGY == USE_MPI_STRATEGY
