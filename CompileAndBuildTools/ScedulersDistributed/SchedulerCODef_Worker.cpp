#include "../Compiler/ExecutionStrategy_Defines.h"

#if EXECUTION_STRATEGY == USE_MPI_STRATEGY

#include "stdafx.h"
#include "SchedulerCODef.h"
#include <assert.h>

#include "SchedulerUtils.h"

namespace SchedulerCO
{

void Worker::OnUpdate()
{
	// TODO Later: interleave the execution of a task with responding to a new message
	// to avoid the starvation of the producer

	LOG(("Worker %d started execution\n", m_iWorkerID));

	while(true)
	{
		// 1. Check if we have any new message
		int flag;
		MPI_Status status;

		MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
		if (flag != 0) // Received a new message !
		{
			switch(status.MPI_TAG)
			{
				case NEW_TASK_TAG:
					{
						if (m_bTerminationSignaled)
						{
							assert(false);
							printf("Worker: Termination signaled but you still give me tasks???\n");
							break;
						}

						int receivedSizeInBytes = 0;
						MPI_Get_count(&status, MPI_CHAR, &receivedSizeInBytes);

						TaskDef *pNewTask = new TaskDef();
						pNewTask->pTaskData = new char[receivedSizeInBytes];
						pNewTask->iSizeOfTask = receivedSizeInBytes;
						MPI_Recv(pNewTask->pTaskData, receivedSizeInBytes, MPI_CHAR, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &pNewTask->status);
						LOG(("Worker %d received task message of size %d\n", m_iWorkerID, receivedSizeInBytes));

						m_Tasks.push(pNewTask);

						// Try to get all tasks all messages if they exist to avoid master starvation
						continue;
					}
					break;
				case TERMINATION_TAG:
					{
						char c;
						MPI_Recv(&c, 1, MPI_CHAR, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
					
						m_bTerminationSignaled = true;
						LOG(("Worker %d received termination message\n", m_iWorkerID)); 

						MPI_Send(&c, 1, MPI_CHAR, 0, TERMINATION_TAG, MPI_COMM_WORLD);
					}
					break;
				default:
					assert(false && "Unknown message arrived !");
			}
		}

		bool bTasksRemains = true;
		
		// 2. Execute current available messages
		if (m_Tasks.size() > 0)
		{
			TaskDef *pTask = m_Tasks.front();
			m_Tasks.pop();

			LOG(("Worker %d sends task with size %d for execution \n", m_iWorkerID, pTask->iSizeOfTask)); 
			OnExecuteTask(pTask);

			bTasksRemains = true;
		}

		if (m_SendingResultsInProgress.size() > 0)
		{
			UpdateSendingResultsInProgress();
			bTasksRemains = true;
		}

		if(bTasksRemains) // No tasks remained ? Verify if the termination is signaled
		{
			if (m_bTerminationSignaled)
				break;
		}
	}

	LOG(("Worker %d finished execution ! \n", m_iWorkerID));
}

void Worker::UpdateSendingResultsInProgress()
{
	for (ListOfResultsIter it = m_SendingResultsInProgress.begin(); it != m_SendingResultsInProgress.end(); )
	{
		// Test if this operation is completed or not
		int flag = 0;
		MPI_Status status;
		MPI_Test(&(*it)->request, &flag, &status);
		if (flag == 1)	// It's completed ?
		{
			LOG(("Sending back a result with size %d to master\n", (*it)->iSizeOfResultData)); 

			ListOfResultsIter nextIt = it;
			delete (*it);

			nextIt++;
			m_SendingResultsInProgress.erase(it);			
			it = nextIt;
		}
		else
		{
			it++;
		}
	}
}

void Worker::OnSendResults(void *pResultData, int iResultDataSize)
{
	SchedulerCO::TaskResult* result = new SchedulerCO::TaskResult();
	result->pResultData = pResultData;
	result->iSizeOfResultData = iResultDataSize;

	MPI_Isend(result->pResultData, result->iSizeOfResultData, MPI_CHAR, 0, NEW_TASK_TAG, MPI_COMM_WORLD, &result->request);

	m_SendingResultsInProgress.push_back(result);
}

} // namespace SchedulerCO

#endif // EXECUTION_STRATEGY == USE_MPI_STRATEGY
