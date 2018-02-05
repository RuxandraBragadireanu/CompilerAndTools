#include <iostream>
#include "ExecutionBlackbox.h"

#include <Windows.h>
#include "Profiler.h"
#include "calc3_utils.h"
#include <direct.h>

extern "C" 	FILE *yyin;
extern int yyparse (void);

int gExecutionType		= EXECT_GENERATE_EXE;	// Type of the execution
int gUseSerializedAST	= true;				// This is true when when the AST is serialized

Timer gTotalGlobalWatch;

int main(int argc, char* argv[]) 
{

#ifdef DISTRIBUTED_FROM_VS
	const char* pathToSlnDir = GetPathToSlnDir();
	std::wstring wstr(pathToSlnDir, pathToSlnDir+strlen(pathToSlnDir));
	SetCurrentDirectory(wstr.c_str());
#endif

	gTotalGlobalWatch.Start();

	// Parse the parameters
	if (argc > 1)
	{
		for (int i = 1; i < argc; i++)
		{
			if (argv[i][0] == 'g')
			{
				gExecutionType = EXECT_GENERATE_CODE;
			}
			else if (argv[i][0] == 'f')
			{
				gUseSerializedAST = false;
			}
		}
	}

	int error = _mkdir(TEMP_DIR_NAME);
	error = error;

	// Pre init the execution strategy
	ExecutionBlackbox::PreInitStrategy(argc, argv);

	WCHAR dirPathBuff[1024];
	GetCurrentDirectory(1024, dirPathBuff);
	std::wcout<<dirPathBuff<<std::endl;

	// On debug we need to analyze and transform again. This is because we generally debug directly from Compiler solution.
#ifdef RUN_FROM_VS_DEBUGGER_NEW
	if (gExecutionType == EXECT_GENERATE_CODE)
	{
		RunDebugModulesAnalyzer();
	}
#endif

	if (gExecutionType != EXECT_GENERATE_CODE && gUseSerializedAST)
	{
		// We already have the serialized tree, don't need to parse the file again
		ABSTFactory::RunCompilation();
	}
	else
	{
		// Parse the file then execute RunCompilation
		FILE *f = fopen(GetCompleteFilePathToSlnDirTemp("agapia_transf.txt"), "r");
		if (f == NULL)
		{
			std::cout<<"Error: Input file agapia_transf.txt couldn't be opened "<<std::endl;
		}

		yyin = f;

		yyparse();
	}

	std::cout<<"Total execution time: "<<std::endl;
	gTotalGlobalWatch.Show();

	return 0;
}

