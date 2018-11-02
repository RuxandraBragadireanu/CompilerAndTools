#include "stdafx.h"
#include <mpi.h>
#include "SchedulerCODef.h"
#include "SchedulerUtils.h"
#include <iostream>
#include <Windows.h>
#include "TestScheduler.h"

#include "../Compiler/utils.h"

using namespace std;

namespace SchedulerCO
{
class MyWorker : public SchedulerCO::Worker
{
public:
	MyWorker(int iWorkerID) : Worker(iWorkerID) {}
	void OnExecuteTask(SchedulerCO::TaskDef *pTaskDef)
	{
		void *pData = pTaskDef->pTaskData;

		LOG(("OnExecuteTask data size %d\n", pTaskDef->iSizeOfTask));
		Sleep(500);
		
		delete pTaskDef;

		char* buffResult = new char[1];
		OnSendResults(buffResult, 1);
	}
};

class MyMaster : public SchedulerCO::Master
{
public:
	MyMaster(int iNrWorkers) : Master(iNrWorkers){}
#ifdef CAN_MASTER_EXECUTE_TASKS
	void OnExecuteTask(void *pData, int iDataSize)
	{
		LOG("OnExecuteTask address data %d, data size %d", iDataSize);
	}
#endif
};
};

const int iMaxDataSize = 1024*1024;
char buff[iMaxDataSize];

void OnMasterJob(int iNrWorkers)
{
	SchedulerCO::MyMaster master(iNrWorkers);
	

	memset(buff, 123, sizeof(buff));

	// Initial tasks distribution
	for (int i = 0; i < INITIAL_TASKS_NUM; i++)
		master.OnTaskCreated(buff, (iMaxDataSize>>1) + rand() % (iMaxDataSize>>1));

	// Execute the task graph
	while(master.IsAnyTaskInProgress())
		master.OnUpdate();

	// Start and wait for termination to end
	master.StartTermination();
	while(master.IsTerminationFinished())
		master.OnUpdate();

	LOG(("Master finished execution !\n"));
}

void OnWorkerJob(int iRank)
{
	SchedulerCO::MyWorker worker(iRank);
	worker.OnUpdate();
}

int main(int argc, char* argv[]) 
{
	RunTest(argc, argv);
	return 0;
}

int RunTest(int argc, char *argv[])
{
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);

	Timer timer;

	if (rank == 0)
		OnMasterJob(size - 1);
	else
		OnWorkerJob(rank);

	timer.Show();

	MPI_Finalize();
	
	return 0;
}

