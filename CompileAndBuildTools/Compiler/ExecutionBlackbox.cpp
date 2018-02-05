#include "ExecutionBlackbox.h"
#include "AgapiaToCCode.h"
#include "MainInputParser.h"
#include "CompilationBlackbox.h"

#include <stdio.h>
#include <map>
#include <string>

#include "ExecutionStrategy_Defines.h"


#if EXECUTION_STRATEGY == USE_SERIAL_STRATEGY
#include "ExecutionStrategyConcrete_Serial.h"
#elif EXECUTION_STRATEGY == USE_MPI_STRATEGY
#include <mpi.h>
#include "ExecutionStrategyConcrete_MPI.h"
#endif

ExecutionBlackbox* ExecutionBlackbox::gInstance = NULL;
MainInputParser* ExecutionBlackbox::m_pInputParserTool = NULL;

void ExecutionBlackbox::PreInitStrategy(int argc, char *argv[])
{
#if EXECUTION_STRATEGY == USE_SERIAL_STRATEGY
		// Nothing
#elif EXECUTION_STRATEGY == USE_MPI_STRATEGY
		MPI_Init(&argc, &argv);
#else
		assert(false && "Not defined ");
		gInstance->m_pStrategy = NULL;
#endif
}

void ExecutionBlackbox::Sync()
{
#if EXECUTION_STRATEGY == USE_MPI_STRATEGY
	MPI_Barrier(MPI_COMM_WORLD);
#endif
}

void ExecutionBlackbox::Initialize()
{
	if (gInstance == NULL)
	{
		gInstance = new ExecutionBlackbox();

		// Create the strategy here

		// Define factory strategy
#if EXECUTION_STRATEGY == USE_SERIAL_STRATEGY
		gInstance->m_pStrategy = new ExecutionStrategyConcrete_Serial();
#elif EXECUTION_STRATEGY == USE_MPI_STRATEGY
		int rank, size;
		MPI_Comm_rank(MPI_COMM_WORLD,&rank);
		MPI_Comm_size(MPI_COMM_WORLD,&size);
		
		// Master ?
		if (rank == 0)
		{
			gInstance->m_pStrategy = new ExecutionStrategyConcrete_MPI_Master(size - 1);
		}
		else
		{
			gInstance->m_pStrategy = new ExecutionStrategyConcrete_MPI_Worker(rank);
		}
#else
		assert(false && "Not defined ");
		gInstance->m_pStrategy = NULL;
#endif

		InitializeAgapiaToCFunctions();

		m_pInputParserTool = new MainInputParser();
	}
}

void ExecutionBlackbox::DoWork()
{	
	if (gInstance->m_pStrategy == NULL)
	{
		printf("gInstance->m_pStrategy is NULL !!!\n");
	}

	gInstance->m_pStrategy->DoWork();
}

void ExecutionBlackbox::Destroy()
{
	if (m_pInputParserTool)
		delete m_pInputParserTool;

	if (gInstance)
	{
		delete gInstance->m_pStrategy;
		delete gInstance;
	}
}

void ExecutionBlackbox::ParseMainInput()
{
	m_pInputParserTool->ParseInputFile();
}

void ExecutionBlackbox::OnModuleReadyForExecution(ProgramIntermediateModule* pProgram)
{
	m_pStrategy->OnModuleReadyForExecution(pProgram);
}

void ExecutionBlackbox::AddAgapiaToCFunction(const char *szName, AgapiaToCFunction func)
{
	std::string str(szName);
	std::pair<std::string, AgapiaToCFunction> p = std::make_pair(str, func);
	m_AgapiaToCFunctions.insert(p);
}

AgapiaToCFunction ExecutionBlackbox::GetAgapiaToCFunction(const char* szName)
{
	std::string str(szName);
	MapOfAgapiaToCFunctions::iterator it = m_AgapiaToCFunctions.find(str);
	if (it == m_AgapiaToCFunctions.end())
		return NULL;
	else
		return it->second;
}
