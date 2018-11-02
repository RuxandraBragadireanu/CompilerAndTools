#include <stdio.h>
#include <string>
#include <list>
#include "ParserProcessesVector.h"
#include <windows.h>
#include <direct.h>

#define GENERATED_MODULE_FILES_NAME	 "generatedModules.txt"

using namespace std;

enum AnalysisState
{
	E_START,
	E_NEWMODULE,
	E_OPENMODULE,
	E_CLOSEMODULE,
};

#define LINEBUFF_MAX_LENGTH		2048
#define MODULE_NAME_MAXLENGTH	512

typedef list<string> ListOfCodeLines;
typedef ListOfCodeLines::iterator ListOfCodeLinesIter;

FILE *fin = NULL, *fout = NULL;
FILE *genModulesFile = NULL;
int lineNum = 0;

bool IsWhitespaceChar(char c)
{
	if (c == '\n' || c == ' ' || c == '\t')
		return true;

	return false;
}

void PrintError(char *buff)
{
	static char errBuff[1024];
	sprintf_s(errBuff, 1024, "Line %d, Error: %s", lineNum, buff);
	printf(errBuff);
	fclose(fin);
	fclose(fout);
	exit(1);
}

// Test if a buffer is composed only from whitespace
bool IsEmptyBuffer(char* buff)
{
	for (; *buff && (*buff == '\t' || *buff == ' ' || *buff == '\n'); buff++);
	return (*buff == 0);	
}

bool IsBufferContainOnlyChar(char* buff, char Ch)
{
	if (buff[0] != Ch)
		return false;

	return IsEmptyBuffer(buff + 1);
}

bool IsModuleDeclBuffer(char *buff, char *moduleNameOut)
{
	// Find word module
	if (strstr(buff, "module ") != buff)
	{
		printf("didn't find the word \"module\" \n");
		return false;
	}

	// Find the name for the module
	char* pBuffIter = buff + strlen("module ");
	int iPos = 0;
	for (; (*pBuffIter != 0 &&  *pBuffIter != ' ' && *pBuffIter != '\n' && *pBuffIter != '\t' && *pBuffIter != '{'); moduleNameOut[iPos++] = *pBuffIter, pBuffIter++);
	moduleNameOut[iPos] = 0;

	if (iPos == 0)
	{
		printf("No module name!\n");
		return false;
	}

	if (!strstr(pBuffIter, "listen "))
	{
		printf("Not found: listen\n");
		return false;
	}

	if (!strstr(pBuffIter, "read "))
	{
		printf("Not found: read\n");
		return false;
	}

	return true;
}

bool IsEndModuleBuffer(char* buff)
{
	if (buff[0] != '}')
	{	
		return false;
	}

	if (!strstr(buff, "speak "))
	{
		printf("Not found: listen\n");
		return false;
	}

	if (!strstr(buff, "write "))
	{
		printf("Not found: read\n");
		return false;
	}

	return true;
}

// This functions extract the @TARGET from the module specifier 
void ExtractTargetProcessForExecution(char *szModuleDeclLine, int& iExecutionTargetProcess)
{
	iExecutionTargetProcess = -1;
	char*pIter = szModuleDeclLine + strlen(szModuleDeclLine) - 1;
	while(pIter > szModuleDeclLine)
	{
		if (*pIter == '@')
		{
			// We should see what string is there! 
			// TODO: for now we only consider execution on master or on any client
			iExecutionTargetProcess = 0;
			*pIter = '\n';
			*(++pIter) = 0;
			return;
		}

		pIter--;
	}
}

bool IsBetweenCommas(const char *opAddr, const char* strLineBegin)
{
	const char* opIter = opAddr;					
	for (; *opIter != 0 && *opIter != '\n' && *opIter != '\"'; opIter++);
	if (*opIter == '\"')
	{	
		for (opIter = opAddr; opIter >= strLineBegin && *opIter != '\"'; opIter--);
		if (*opIter == '\"')
		{
			return true;
		}						
	}

	return false;
}

bool IsModuloForNumbers(const char *opAddr, const char* strLineBegin)
{
	static const int iNumDelim = 3;
	static char delim[iNumDelim+1] =" \t\n";

	bool leftOk = false;
	bool rightOk = false;

	// right
	char *it = (char*)opAddr + 1;
	bool alreadyFoundSomething = false;
	while(*it != 0)
	{
		// is this char a delim one ?
		bool ignoreThisChar = false;
		if (!alreadyFoundSomething)
		{
			char c = *it;
			for (int i = 0; i < iNumDelim; i++)
				if (c == delim[i])	// Then ignore the character
				{
					ignoreThisChar = true;
					break;
				}
		}

		if (!ignoreThisChar)
		{
			alreadyFoundSomething = true;

			// Other char than a delim, see its type
			if ((isdigit(it[0]) || (it[0] >='a' && it[0] <='z')))
			{
				rightOk= true;
				break;
			}

		}

		it++;
	}

	//left
	it = (char*)opAddr - 1;
	alreadyFoundSomething = false;
	while (it >= strLineBegin)
	{
		bool ignoreThisChar = false;
		// is this char a delim one ?
		if (!alreadyFoundSomething)
		{
			char c = *it;
			for (int i = 0; i < iNumDelim; i++)
				if (c == delim[i])	// Then ignore the character
				{					
					ignoreThisChar = true;	
					break;
				}
		}

		if (!ignoreThisChar)
		{
			alreadyFoundSomething = true;

			// Other char than a delim, see its type
			if (isdigit(it[0]) || (it[0] >='a' && it[0] <='z'))
			{
				leftOk = true;
				break;
			}
		}

		it--;
	}

	if (!leftOk && !rightOk)
	{
		return false;
	}
	else
	{
		return true;
	}
}


void ModifyCodeLineByModuleType(std::string& codeLine, bool isAtomicModule)
{
	// Don't modify code for atomic modules
	if (isAtomicModule)
	{
		return;
	}

	// Change "%" if not under commas with "mod"
	char* strIter = (char*) codeLine.c_str();
	while(*strIter)
	{
		strIter = strstr(strIter, "%");
		// Change the string and restart
		if (strIter == NULL)
			break;

		if (IsBetweenCommas(strIter, codeLine.c_str()))
		{
			strIter++;
			continue;
		}

		if (!IsModuloForNumbers(strIter, codeLine.c_str()))
		{
			strIter++;
			continue;
		}

		codeLine.replace(strIter-codeLine.c_str(), 1, "mod");
	}
}

void AnalyzeCurrentModule(char *moduleName, ListOfCodeLines& listOfCodeLines, char* moduleEndLine, int iExecutionTargetProcess)
{	
	// We have to check if the lines of code contains the following:
	// while_t/s/st. for-same, 
	// composition operators : # , $, %

	// see if the module decl line contains an execution specifier
	bool isAtomicModule = true;

	char* agapiaKeywords[2]={"while_", "for_"};
	for (int i = 0; i < 2; i++)
	{
		char* keyword = agapiaKeywords[i];
		for (ListOfCodeLinesIter it = listOfCodeLines.begin(); it != listOfCodeLines.end(); it++)
			if (strstr(it->c_str(), keyword))
			{
				isAtomicModule = false;
			}
	}

	if (isAtomicModule)
	{
		char *compositionOps[3] = {"#", "$", "%"};
		for (int i = 0; i < 3; i++)
		{
			for (ListOfCodeLinesIter it = listOfCodeLines.begin(); it != listOfCodeLines.end(); it++)
			{
				const char *str = it->c_str();
				do 
				{
					const char *opAddr = strstr(str, compositionOps[i]);
					if (opAddr == NULL)	// Not found on this line, exit
						break;

					// Found , we must check where this operator is included. 
					// 1. Is it between commas ?
					bool isBetweenCommas =  IsBetweenCommas(opAddr, str);
					
					// 2. Is line finished with ";" ? More likely to be a line of code...(TODO better test here)
					str = opAddr + 1;
					if (!isBetweenCommas && !strstr(str, ";"))
					{
						// 3. For '%' we can allow if the right and left side contains identifiers or numbers ?						
						if (strcmp(compositionOps[i], "%") == 0)
						{
							if (!IsModuloForNumbers(opAddr, str))
							{
								isAtomicModule = false;
								break;
							}							
						}
						else
						{
							isAtomicModule = false;
							break;
						}
					}
				} while (true);			

				// TODO - IMPROVE THIS TEST
				// 4. Last check - verify if we have a module like  {  IDENTITY } without composition or keywords
				const char* szCode = str;
				while(IsWhitespaceChar(*szCode) && *szCode) szCode++;

				if (*szCode != 0)
				{
					bool bIsAnyChar = false;

					// All characters should be upper cases. If not, then it seems to be an atomic module
					bool isThereAnyOtherCharThan_UpperLetter = false;
					while (*szCode)
					{
						char c = *szCode;
						if (c == '\n')	// code line finish
						{
							break;
						}

						if (!(c >= 'A' && c <= 'Z'))
						{
							isThereAnyOtherCharThan_UpperLetter = true;
							break;
						}

						szCode++;
					} 

					if (isThereAnyOtherCharThan_UpperLetter == false)
					{
						isAtomicModule = false;
					}
				}
			}
		}		
	}

	// Is atomic module ?
	// Then write the code in a filename as the module name, and write as output to file { @ }
	if (isAtomicModule)
	{
		if (iExecutionTargetProcess == -1)
			fputs("\t@\n", fout);
		else
			fputs("\t@MASTER\n", fout);

		// Create the new file and write the code
		char moduleNameBuff[LINEBUFF_MAX_LENGTH];
		sprintf_s(moduleNameBuff, LINEBUFF_MAX_LENGTH, "temp\\%s", moduleName);
		FILE *f = fopen(moduleNameBuff, "w");
		for (ListOfCodeLinesIter it = listOfCodeLines.begin(); it != listOfCodeLines.end(); it++)
		{
			ModifyCodeLineByModuleType(*it, isAtomicModule);
			fputs(it->c_str(), f);
		}

		fputs(moduleEndLine, fout);
		fprintf(fout, "\n");
		fclose(f);

		fprintf(genModulesFile, "%s\n", moduleName);
	}
	else
	{
		// Write the output lines in fout
		for (ListOfCodeLinesIter it = listOfCodeLines.begin(); it != listOfCodeLines.end(); it++)
		{
			ModifyCodeLineByModuleType(*it, isAtomicModule);
			fputs(it->c_str(), fout);
		}

		fputs(moduleEndLine, fout);
		fprintf(fout, "\n");
	}
}

int main()
{
	ParserProcessesVector parser;

	fin = fopen("agapia.txt", "r");
	if (fin == NULL)
	{
		printf("There is no agapia.txt in the current dir\n");
		return -1;
	}

	_mkdir("temp");

	fout = fopen("temp\\agapia_transf.txt", "w");
	if (fout == NULL)
	{
		int reason = GetLastError();
		printf("Can't run module analyzer because I can't create the file \\temp\\agapia_trans.txt Error code: %d\n", reason);
		return -1;
	}

	genModulesFile = fopen("temp\\" GENERATED_MODULE_FILES_NAME, "w");
	AnalysisState state = E_START;

	char lineBuff[LINEBUFF_MAX_LENGTH];
	char moduleName[MODULE_NAME_MAXLENGTH];
	int iExecutionTargetProcess = -1;
	ListOfCodeLines listOfCodeLines;

	while(!feof(fin))
	{
		lineNum++;
		fgets(lineBuff, 2048, fin);

		if (feof(fin))
			break;

		switch(state)
		{
		case E_START:
			{
				if (IsEmptyBuffer(lineBuff))
					break;

				if (IsModuleDeclBuffer(lineBuff, moduleName))
				{
					state = E_NEWMODULE;

					// Copy the declaration to the other side
					ExtractTargetProcessForExecution(lineBuff, iExecutionTargetProcess);
					fputs(lineBuff, fout);
				}
				else
				{
					PrintError("Wanted a single module declaration but something is wrong on that line\n");
				}
			}
			break;		

		case E_NEWMODULE:
			{
				if (IsBufferContainOnlyChar(lineBuff, '{'))
				{
					state = E_OPENMODULE;
					fputs(lineBuff, fout);
				}
				else
				{
					PrintError("Wanted just \"{\" to open the module but this either miss or is more than this symbol \n");
				}
			}
			break;
		case E_OPENMODULE:
			{				
				if (IsEndModuleBuffer(lineBuff))
				{
					AnalyzeCurrentModule(moduleName, listOfCodeLines, lineBuff, iExecutionTargetProcess);
					state = E_START;

					listOfCodeLines.clear();
				}
				else
				{
					// If it is a comment, don't write it to the other file
					if (parser.ParseCommentLine(lineBuff))
					{

					}
					else if (!parser.ParseVectorAccessLine(lineBuff, lineNum))	// If it is not a vector access, add the entire line
					{
						listOfCodeLines.push_back(string(lineBuff));
					}
					else	// Vector access, translate the code
					{
						// Translate the instruction in the correct format
						if (parser.GetVectorAccessParsingRes() == VAP_WRITE)
						{
							// SetInputItemToVector(int lineNo, VectorProcessItem* pVector, int iVectorIndex, char* structItem, float value);
							std::string strLineCode = parser.GetSpacesStringInFront() + "SetInputItemToVector(" + std::to_string((long long)lineNum) + ", &" + parser.GetVectorName() + ", ";

							if (parser.GetVectorIndex().type == ParserProcessesVector::TOKEN_NUMBER_INT)
							{
								strLineCode += std::to_string((long long)parser.GetVectorIndex().data.intData);
							}
							else if (parser.GetVectorIndex().type == ParserProcessesVector::TOKEN_IDENTIFIER)
							{
								strLineCode += std::string(parser.GetVectorIndex().data.stringData);
							}

							strLineCode += ", \"" + std::string(parser.GetVectorStructItem()) + "\", ";

							if (parser.GetValueAssigned().type == ParserProcessesVector::TOKEN_NUMBER_INT)
							{
								strLineCode += std::to_string((long long)parser.GetValueAssigned().data.intData);
							}
							else if (parser.GetValueAssigned().type == ParserProcessesVector::TOKEN_NUMBER_FLOAT)
							{
								strLineCode += std::to_string((long double)parser.GetValueAssigned().data.floatData);
							}
							else if (parser.GetValueAssigned().type == ParserProcessesVector::TOKEN_IDENTIFIER)
							{
								strLineCode += std::string(parser.GetValueAssigned().data.stringData);
							}
							else if (parser.GetValueAssigned().type == ParserProcessesVector::TOKEN_BUFFER_DATA_PAIR)
							{
								strLineCode += "(char*)" +std::string(parser.GetValueAssigned().data.stringData) + ", ";

								if (parser.GetSecondValueAssigned().type == ParserProcessesVector::TOKEN_IDENTIFIER)
									strLineCode += std::string(parser.GetSecondValueAssigned().data.stringData);
								else 
									strLineCode += std::to_string((long long)parser.GetSecondValueAssigned().data.intData);
							}

							strLineCode += ");\n";
							listOfCodeLines.push_back(strLineCode);
						}
						else if (parser.GetVectorAccessParsingRes() == VAP_READ_VP)
						{
							//	VectorProcessItem* pSPI = (VectorProcessItem*)GetVectorItemByIndex(numbers, i, 0);
							std::string strLineCode = parser.GetSpacesStringInFront() + "VectorProcessItem* ";
							strLineCode			   += parser.GetVectorStructItem();
							strLineCode			   += " = (VectorProcessItem*)GetVectorItemByIndex(";
							strLineCode			   += parser.GetVectorName();
							strLineCode			   += ", ";

							// Put the right index
							const ParserProcessesVector::ParsedToken& vectorIndex = parser.GetVectorIndex();
							if (vectorIndex.type == ParserProcessesVector::TOKEN_IDENTIFIER)
							{
								strLineCode		+= std::string(vectorIndex.data.stringData);
							}
							else if (vectorIndex.type == ParserProcessesVector::TOKEN_NUMBER_INT)
							{
								strLineCode		+= std::to_string((long long)vectorIndex.data.intData);
							}
							else if (vectorIndex.type == ParserProcessesVector::TOKEN_NUMBER_FLOAT)
							{
								strLineCode		+= std::to_string((long double)vectorIndex.data.floatData);
							}

							strLineCode += ", 0);\n";
							listOfCodeLines.push_back(strLineCode);
						}
						else // VAP_READ_SP
						{
							//	SimpleProcessItem* pSPI = (SimpleProcessItem*)GetVectorItemByIndex(numbers, i, 0);
							std::string strLineCode = parser.GetSpacesStringInFront() + "SimpleProcessItem* ";
							strLineCode			   += parser.GetVectorStructItem();
							strLineCode			   += " = (SimpleProcessItem*)GetVectorItemByIndex(";
							strLineCode			   += parser.GetVectorName();
							strLineCode			   += ", ";

							// Put the right index
							const ParserProcessesVector::ParsedToken& vectorIndex = parser.GetVectorIndex();
							if (vectorIndex.type == ParserProcessesVector::TOKEN_IDENTIFIER)
							{
								strLineCode		+= std::string(vectorIndex.data.stringData);
							}
							else if (vectorIndex.type == ParserProcessesVector::TOKEN_NUMBER_INT)
							{
								strLineCode		+= std::to_string((long long)vectorIndex.data.intData);
							}
							else if (vectorIndex.type == ParserProcessesVector::TOKEN_NUMBER_FLOAT)
							{
								strLineCode		+= std::to_string((long double)vectorIndex.data.floatData);
							}

							strLineCode += ", 0);\n";
							listOfCodeLines.push_back(strLineCode);
						}
					}
				}
			}
			break;
		}

	}

	if (state != E_START)
	{
		// Failed to parse the program
		PrintError("FAILED TO EXECUTE THE PROGRAM. SOMETHING IS NOT OPENED/CLOSED CORRECTLY\n");
		return -1;
	}

	fclose(fin);
	fclose(fout);
	fclose(genModulesFile);
	return 0;
}
