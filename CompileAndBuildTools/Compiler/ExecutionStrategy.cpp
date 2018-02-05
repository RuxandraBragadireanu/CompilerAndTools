#include "ExecutionStrategy.h"
#include "CompilationBlackbox.h"
#include "ExecutionBlackbox.h"


void ExecutionStrategy::DoMainModuleInputInitialization()
{
	// Inform the execution blackbox that the input free modules are ready to execution
	// Dont run the main module here!
	const ListOfPrograms& refListOfInputFreeModules = CompilationBlackbox::Get()->GetListOfFreeModules();
	for (ListOfProgramsIterConst it = refListOfInputFreeModules.begin(); it != refListOfInputFreeModules.end(); it++)
	{
		ProgramIntermediateModule* pProgram = (ProgramIntermediateModule*) (*it);
		//if (pProgram->m_bIsUserDefined && strcmp(pProgram->m_szModuleName, "MAIN"))
		ExecutionBlackbox::Get()->OnModuleReadyForExecution(pProgram);
	}

	// Run main - TODO HERE
	ProgramIntermediateModule* pMainModule = CompilationBlackbox::Get()->GetModuleByName("MAIN");
	//ExecutionBlackbox::Get()->OnModuleReadyForExecution(pMainModule);
	ExecutionBlackbox::ParseMainInput();
	//if (pMainModule->IsAtomicProgram())
}