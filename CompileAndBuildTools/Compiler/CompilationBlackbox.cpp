#include "CompilationBlackbox.h"
#include "INTERMEDIATENode.h"
#include "CModuleCode.h"
#include "CodeSerializationFactory.h"

#include "calc3_utils.h"
#include <fstream>

// #include <windows.h>

using namespace std;

CompilationBlackbox* CompilationBlackbox::gpInstance = NULL;
CompilationBlackbox* CompilationBlackbox::Get()
{
	if (gpInstance == NULL)
		gpInstance = new CompilationBlackbox();

	return gpInstance;
}

ProgramIntermediateModule* CompilationBlackbox::GetModuleByName(const char *szName)
{
	std::string str(szName);
	ModulesHashIter it = m_ModulesHash.find(str);
	if (it == m_ModulesHash.end())
		return NULL;

	return it->second;
}

void CompilationBlackbox::AddModule(const char *szName, ProgramIntermediateModule* pModule)
{
	std::string str(szName);
	ProgramIntermediateModule* pProgramNode = GetModuleByName(szName);
	if (pProgramNode)
	{
		PrintCompileError(pModule->GetLineNo(), "Module %s already exists !!", szName);
		return;
	}

	m_ModulesHash.insert(std::make_pair(str, pModule));
}

void CompilationBlackbox::ClearIteration()
{
	m_IteratedModules.clear();
}

bool CompilationBlackbox::IsModuleInSet(char* szModuleName)
{
	return (m_IteratedModules.find(szModuleName) != m_IteratedModules.end());
}

void CompilationBlackbox::InsertModuleName(char* szModuleName)
{
	m_IteratedModules.insert(szModuleName);
}

void CompilationBlackbox::AddInputFreeProgram(ProgramBase* pProgram)
{
	m_ListOfInputFreeModules.push_back(pProgram);
}

void CompilationBlackbox::Destroy()
{
	m_ModulesHash.clear();

	delete gpInstance;
	gpInstance = NULL;
}

bool CompilationBlackbox::DoCompileSteps(int executionTypes)
{
	//printf("========== BEGIN STEP 1: Find references to modules ===========\n");
	// Search main module
	ProgramIntermediateModule* pMainModule = GetModuleByName("MAIN");
	if (pMainModule == NULL)
	{
		PrintCompileError(-1, "No MAIN module found !!\n");
		return false;
	}

	// Solve references	
	if (!pMainModule->SolveReferences())
	{
		PrintCompileError(-1, "Can't solve references: maybe you didn't declared one of the modules in both AGAPIA and external file. Report if you see this error");
		return false;
	}

	//printf("========== BEGIN STEP 2: Solve input/output ===========\n");
	ClearIteration();
	if (!pMainModule->SolveInputOutput())
		return false;

	//printf("========== BEGIN STEP 3: Validate statements ===========\n");
	ClearIteration();
	if (!pMainModule->Validate(NULL, false))
		return false;

	//printf("========== BEGIN STEP 4: Write the translated AGAPIA atomic modules to C code ===========\n");

	if (executionTypes == EXECT_GENERATE_CODE)
	{
		WriteCCodeFile();
	}

#ifdef RUN_FROM_VS_DEBUGGER
	WriteCCodeFile();
#endif

	return true;
}

bool CompilationBlackbox::WriteCCodeFile()
{
	// Create the AGAPIAToCCode file in the AGAPIA path workspace - it might be executed from outside
	FILE* f;
	const char* szAgapiaPath = getenv("AGAPIAPATH");
	if (szAgapiaPath == NULL || strlen(szAgapiaPath) == 0)
	{
		printf("ERROR: PLEASE DECLARE THE ENVIRONMENT VARIABLE AGAPIAPATH\n");
		exit(0);
	}

	char buff[1024];
	sprintf_s(buff, 1024, "%s\\Compiler\\AgapiaToCCode.cpp", szAgapiaPath);
	f = fopen(buff, "w");
	fprintf(f, "#include \"AgapiaToCCode.h\"\n");
	fprintf(f, "#include \"ExecutionBlackbox.h\"\n");
	fprintf(f, "#include \"InputTypes.h\"\n\n");
	fprintf(f, "#include \"Includes.h\"\n");
	
	// Write the modules code
	for (ModulesHashIter it = m_ModulesHash.begin(); it != m_ModulesHash.end(); it++)
	{
		ProgramIntermediateModule* pProgram = it->second;
		if (pProgram->IsAtomicProgram())
		{
			if (!pProgram->m_pCCodeObject->WriteCodeToFile(pProgram->m_szModuleName, f))
			{
				assert(false);
				return false;
			}
		}
	}

	// Write the init function
	fprintf(f, "void InitializeAgapiaToCFunctions()\n");
	fprintf(f, "{\n");
	for (ModulesHashIter it = m_ModulesHash.begin(); it != m_ModulesHash.end(); it++)
	{
		ProgramIntermediateModule* pProgram = it->second;
		if (pProgram->IsAtomicProgram())
			fprintf(f, "ExecutionBlackbox::Get()->AddAgapiaToCFunction(\"%s\", &%s);\n", pProgram->m_szModuleName,pProgram->m_szModuleName);
	}
	fprintf(f, "}\n");
	fclose(f);

	return true;
}

void CompilationBlackbox::LoadAST()
{
	// [size | num modules | module 0 | ..... | module n-1]
	ifstream inputFile("temp\\agapia.dat", ios::in | ios::binary);

	if (!inputFile.good())
	{		
		LOG(("LoadABT: can't find temp\\agapia.dat which contans the AST serialized. Try to run your program with \'f\' parameter to build the tree\n"));
		assert(false);
	}

	int numModules = 0;
	int totalSize = 0;

	// Create the buffer
	//---------------------------------------------------
	if (!inputFile.read((char*)&totalSize, sizeof(int)))
	{
		LOG(("Can't read the number of modules or totalSize\n"));
		assert(false);
	}

	char* buff = (char*)alloca(totalSize);
	if (buff == NULL)
	{
		LOG(("LoadABT: cannot allocate this size is too big for stack: %d\n", totalSize));
		assert(false);
	}
	memcpy(buff, &totalSize, sizeof(int));
	//---------------------------------------------------


	// Read entire buffer
	inputFile.read(buff+sizeof(int), totalSize - sizeof(int));

	Streams::BytesStreamReader moduleStream;
	moduleStream.SetWorkingBuffer(buff, totalSize);

	moduleStream.ReadSimpleType(totalSize);
	moduleStream.ReadSimpleType(numModules);

	for (int i = 0; i < numModules; i++)
	{
		ProgramIntermediateModule* program = CodeSerializationFactory::DeserializeModule(moduleStream);
		m_ModulesHash.insert(std::make_pair(program->GetName(), program));
	}
	
	inputFile.close();
}

void CompilationBlackbox::SaveAST()
{
	// [size | num modules | module 0 | ..... | module n-1]
	ofstream outputFile("temp\\agapia.dat", ios::out | ios::binary);

	if (!outputFile.good())
	{
		LOG(("LoadABT: can't create agapi.dat!! Verify your rights. Else, try to run your program with \'f\' parameter to execute it correctly\n"));
		assert(false);
	}	


	// Allocate the buffer and strea for writing all data
	// ------------------------------------------------------------------------------------
	int totalSize = sizeof(m_ModulesHash.size()) + sizeof(int);
	for (ModulesHashIter it = m_ModulesHash.begin(); it != m_ModulesHash.end(); it++)
	{
		const int moduleSize = CodeSerializationFactory::GetSerializedModuleSize(it->second);
		assert(moduleSize > 0);

		totalSize += moduleSize;
	}

	
	char* buff = (char*)alloca(totalSize);
	if (buff == NULL)
	{
		LOG(("SaveABT: cannot allocate this size is too big for stack: %d\n", totalSize));
		assert(false);
	}

	Streams::BytesStreamWriter moduleStream;
	moduleStream.SetWorkingBuffer(buff, totalSize);
	// ------------------------------------------------------------------------------------

	// Serialize data
	//-------------------------------------------------------------------------------------
	moduleStream.WriteSimpleType(totalSize);
	moduleStream.WriteSimpleType(m_ModulesHash.size());
	for (ModulesHashIter it = m_ModulesHash.begin(); it != m_ModulesHash.end(); it++)
	{
		CodeSerializationFactory::SerializeModule(it->second, moduleStream);
	}
	//-------------------------------------------------------------------------------------

	// Save stream
	if (!outputFile.write(moduleStream.GetBufferStart(), moduleStream.m_iAllocatedSize))
	{
		LOG(("Can't write data to output file. Size %d\n", moduleStream.m_iAllocatedSize));
		assert(false);
	}
	outputFile.close();
}


void CompilationBlackbox::CleanupCompileFiles()
{
	// This will delete all the intermediate files created by ModulesAnalyzer tool
	// Even if we run directly from VS or with the user flow using GenerateApp, ModuleAnalyzer will create separate files for atomic modules' code.
	for (ModulesHashIter it = m_ModulesHash.begin(); it != m_ModulesHash.end(); it++)
	{
		if (it->second->IsAtomicProgram())
		{
			char filePath[1024];
			sprintf_s(filePath, "temp\\%s", it->first.c_str());
			int res = remove(filePath);
			/*
			if (res < 0)
			{
				int dwErr = GetLastError();
				dwErr = dwErr;
			}
			*/
		}	
	}

	remove("temp\\generatedModules.txt");
}
