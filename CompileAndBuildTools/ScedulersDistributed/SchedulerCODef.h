#pragma once
#ifndef SCHEDULER_CO_DEF_H
#define SCHEDULER_CO_DEF_H

#include "../Compiler/ExecutionStrategy_Defines.h"

#if EXECUTION_STRATEGY == USE_MPI_STRATEGY

#include <list>
#include <mpi.h>
#include <queue>

//#define CAN_MASTER_EXECUTE_TASKS

#include "SimulationParams.h"

#define NEW_TASK_TAG		1
#define TERMINATION_TAG		2

namespace SchedulerCO
{
	static const int gOMOGEN_RANK = 0, gOMOGEN_EST_TIME = 0;
	class TaskDef
	{
	public:
		TaskDef():
			request(-1),
			flag(0),
			pTaskData(NULL),
			iSizeOfTask(0)
			{}

		virtual ~TaskDef()
		{
			if (pTaskData)
			{
				delete [] pTaskData;
				pTaskData = NULL;
			}
		}

		// MPI specific data
		MPI_Request request;
		MPI_Status  status;
		int			flag;
		
		// Task to do related data
		void *pTaskData;
		int iSizeOfTask;

		// Destionation of this task
		int iTaskProcDestination;
	};

	class TaskResult
	{
	public:
		TaskResult()
			:	pResultData(NULL),
				iSizeOfResultData(0) {}
		
		virtual ~TaskResult()
		{
			if (pResultData)
			{
				delete [] pResultData;
				pResultData = NULL;
			}
		}

		// MPI specific data
		MPI_Request request;
		MPI_Status  status;
		int			flag;
		
		// Task to do related data
		void *pResultData;
		int iSizeOfResultData;
	};

	typedef std::list<TaskResult*>	ListOfResults;
	typedef ListOfResults::iterator	ListOfResultsIter;

	typedef std::list<TaskDef*>		ListOfTasks;
	typedef ListOfTasks::iterator	ListOfTasksIter;

	typedef std::queue<TaskDef*>	QueueOfTasks;
	
	// -------------------------------- Master class ------------------------------------
	class Master
	{
	public:
		Master(int iNrWorkers);
		virtual ~Master();

		// Called when a new task is available. Sends a task according to strategy
		void OnTaskCreated(void *pTaskData, int iTaskSize);

		// Main function that polls for tasks sending / receiving
		void OnUpdate();

		bool IsAnyTaskInProgress();

		// Termination PUBLIC implementation 
		// ------------------------------------------------
		void StartTermination();
		bool IsTerminationFinished() { return (m_iProcsRemainsToTerminate == 0); }
		// ------------------------------------------------

	protected:
		// Update the tasks being send asyncronous (in m_TasksInSendProgress)
		void UpdateSendingInProgressTasks();

		// Check results from workers
		void CheckReceivedMessages();

		// Called when a processor confirms its termination
		void OnProcesorTerminationConfirmed(int iProcID);

		// Termination implementation 
		// ------------------------------------------------
		// Called when we want to finish the termination.
		// After this we can't continue any other tasks send
		bool m_bTerminationInitialized;
		// Number of procs terminated 
		int m_iProcsRemainsToTerminate;
		// -------------------------------------------------

		// Strategy for scheduling data and functionality
		//---------------------------------------------------
		// Detect which processor should execute a new task ?
		int GetProcessorToExecuteTask(int gOMOGEN_RANK, int gOMOGEN_EST_TIME);
		void OnTaskGivenToProc(int iProcID);
		virtual void OnResultsReceived(char *pDataBuffer, int iDataBufferSize, int iWorkerID);

		// Tasks assigned per workers data structures
		int	m_iNumberOfWorkers;
		int *m_arrTasksAssignedPerWorker;
		int m_iTotalTasksAssignedToWorkers;
		//---------------------------------------------------
		
#ifdef CAN_MASTER_EXECUTE_TASKS
		// Takes tasks from own list and execute
		void UpdateOwnTasks();

		// How to execute a task
		virtual void ExecuteTask(void *pTaskData, int iTaskDataSize) = 0;
#endif

	private:
		// List of all tasks which sending are in progress
		ListOfTasks m_TasksInSendProgress;
		//ListOfTasks m_TasksToReceiveResults;	

#ifdef CAN_MASTER_EXECUTE_TASKS
		ListOfTasks m_OwnTasksList;
#endif
	};


	// -------------------------------- Worker class ------------------------------------
	class Worker
	{
	public:
		Worker(int iWorkerID) : m_bTerminationSignaled(false), m_iWorkerID(iWorkerID) {}
		// The main update function. Listens for tasks input and executes the current ones

		void OnUpdate();
		void UpdateSendingResultsInProgress();
		void OnSendResults(void *pResultData, int iResultDataSize);

	protected:
		// How to execute a task
		virtual void OnExecuteTask(TaskDef *pTaskData) = 0;

		ListOfResults		m_SendingResultsInProgress;
	private:
		QueueOfTasks		m_Tasks;
		bool				m_bTerminationSignaled;
		int					m_iWorkerID;		
	};
};

#endif // EXECUTION_STRATEGY == USE_MPI_STRATEGY

#endif //SCHEDULER_CO_DEF_H