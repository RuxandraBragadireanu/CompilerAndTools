#include "ParserProcessesVector.h"
#include <cctype>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

using namespace std;

static char sBufferString[4096];
static int sBufferIndex = 0;
static inline void AddToBuffString(char  c)
{
	sBufferString[sBufferIndex++] = c;
}

static inline const char* GetBuffStringAddr() { return sBufferString; }
static inline void ResetBuffString() { sBufferIndex = 0;}
static inline int GetIntFromBuffString()
{
	sBufferString[sBufferIndex] = 0;
	return atoi(sBufferString);
}

static inline float GetFloatFromBuffString()
{
	sBufferString[sBufferIndex] = 0;
	return (float)atof(sBufferString);
}

#define EAT_CHAR_FROM_CODELINE(codeString)		{ codeLine++;	\
												  nrCharsEaten++; }

ParserProcessesVector::ParsedToken secondToken;

const ParserProcessesVector::ParsedToken& ParserProcessesVector::GetSecondValueAssigned()
{
	return secondToken;
}

int ParserProcessesVector::ReadNextToken(const char *codeLine, ParsedToken& outputToken, ParsedToken& secondToken)
{
	ResetBuffString();

	outputToken.Reset();
	int nrCharsEaten = 0;

	// Read all whitespace first
	while(*codeLine != '\0' && (*codeLine == ' ' || *codeLine == '\t'))
	{
		EAT_CHAR_FROM_CODELINE(codeLine);
	}

	if (*codeLine == '\0')
	{
		return nrCharsEaten;
	}

	if (*codeLine == '"')	// Possible string
	{
		EAT_CHAR_FROM_CODELINE(codeLine);

		while(*codeLine != '\0')
		{
			if (*codeLine == '"')	// End of the string	
			{
				EAT_CHAR_FROM_CODELINE(codeLine);

				outputToken.data.stringData = GetBuffStringAddr();
				outputToken.type = TOKEN_STRING;
			}

			AddToBuffString(*codeLine);
		}

		AddToBuffString(0);
	}
	else if (isdigit(*codeLine))	// Possible number
	{
		bool dotHappened = false;
		bool isError = false;
		int isFinishedCode = 0;	// 0 not finished, 1 finished with float, 2 finished by other char

		do 
		{
			if (isdigit(*codeLine))	// ok
			{
				AddToBuffString(*codeLine);
			}
			else if (*codeLine == '.')	// dot
			{
				if (dotHappened)
				{
					isError = true;
					break;
				}

				dotHappened = true;
				AddToBuffString(*codeLine);
			}
			else if (*codeLine == 'f')
			{
				isFinishedCode = 1;
			}
			else
			{
				isFinishedCode = 2;
			}

			if (isFinishedCode > 0)	// finished with float or other char
			{
				EAT_CHAR_FROM_CODELINE(codeLine);
			}

			*codeLine++;
		}while(*codeLine != 0);

		AddToBuffString(0);

		// Set up output token
		if (isFinishedCode == 1) // Float read
		{
			outputToken.data.floatData = GetFloatFromBuffString();
			outputToken.type = TOKEN_NUMBER_FLOAT;
		}
		else // Int data
		{
			outputToken.data.intData = GetIntFromBuffString();
			outputToken.type = TOKEN_NUMBER_INT;
		}
	}
	else if (isalpha(*codeLine))	// Possible identifier
	{
		while(isalpha(*codeLine) || isdigit(*codeLine))
		{
			AddToBuffString(*codeLine);
			EAT_CHAR_FROM_CODELINE(codeLine);

			// Catch correctly struct.item or struct->item
			if (*codeLine == '.' && *(codeLine+1) == '\0')
			{
				AddToBuffString(*codeLine);
				EAT_CHAR_FROM_CODELINE(codeLine);
			}
			else if (*codeLine == '-' && *(codeLine+1) == '>')
			{
				for (int i = 0; i < 2; i++)
				{
					AddToBuffString(*codeLine);
					EAT_CHAR_FROM_CODELINE(codeLine);
				}
			}
		}

		AddToBuffString(0);

		outputToken.data.stringData = GetBuffStringAddr();
		outputToken.type = TOKEN_IDENTIFIER;
	}
	else if (*codeLine == '<')
	{
		const char *endPos = strstr(codeLine, ">");
		const char *comma = strstr(codeLine, ",");

		if (endPos == NULL || comma == NULL)
		{
			return false;
		}

		const char *afterOpen = codeLine + 1;		
		if (!ReadNextToken(afterOpen, outputToken, secondToken) || outputToken.type != TOKEN_IDENTIFIER)
		{
			printf("Analyzer tool error: when reading %s. expected to be find an identifier before comma\n", codeLine);
			return false;
		}

		char *copyIdentifier = _strdup(outputToken.data.stringData);
		outputToken.data.stringData = copyIdentifier;

		// Get the token after comma
		const char* afterComma = comma + 1;
		if (!ReadNextToken(afterComma, secondToken, secondToken) || secondToken.type != TOKEN_IDENTIFIER && secondToken.type != TOKEN_NUMBER_INT)
		{
			printf("Analyzer tool error: when reading %s. expected to be find an identifier or integer after comma\n", codeLine);
			return false;
		}

		if (secondToken.type == TOKEN_NUMBER_INT)
		{
			secondToken.data.intData = GetIntFromBuffString();
		}
		
		// Eat chars and setup the output !
		outputToken.type = TOKEN_BUFFER_DATA_PAIR;
		int lenOfBufferPair = endPos - codeLine + 1;
		for (int i = 0; i < lenOfBufferPair; i++)
			EAT_CHAR_FROM_CODELINE(codeLine);
	}
	else							// Error
	{
	}

	return nrCharsEaten;
}

bool ParserProcessesVector::ParseVectorAccessLine(const char* codeLine, int lineNo)
{
	mVectorAccessParsingRes = VAP_NO_ACCESS;

	// remove all whitespace from codeLine
	m_iSpacesEatenInFront = 0;
	m_iTabsEatenInFront = 0;
	while(*codeLine != 0 && (*codeLine == ' ' || *codeLine == '\t'))
	{
		codeLine++;

		if (*codeLine == ' ')		m_iSpacesEatenInFront++;
		else if (*codeLine == '\t') m_iTabsEatenInFront++;
	}

	// Search for the "@[" and try to figure out the accessed index
	const char* strAcess = "@[";
	const int accessLen = strlen(strAcess);
	const char *strVecAccess = strstr(codeLine, strAcess);
	if (strVecAccess == NULL)
		return false;

	const char* strIndexParanthesisClose = strstr(strVecAccess, "]");
	if (strIndexParanthesisClose == NULL)
	{
		printf("Line %d, Error when parsing the vector acess: couldn't find ] to close the index", lineNo);
		return false;
	}

	int charsEaten = ReadNextToken(strVecAccess+accessLen, mVectorIndex, secondToken);
	
	// Verify if succeeded to eat correctly the vector accessed index
	//-----------------------------------------------------------------------
	int nrCharsInIndex = strIndexParanthesisClose - (strVecAccess + accessLen);
	if (charsEaten != nrCharsInIndex || mVectorIndex.type == TOKEN_ERROR || mVectorIndex.type == TOKEN_STRING || mVectorIndex.type == TOKEN_NUMBER_FLOAT)
	{
		printf("Line %d, Error when parsing the vector access: couldn't parse the index of the array accessed between [ ]", lineNo);
		return false;
	}

	// If string type, we need to store it somewhere in memory
	if (mVectorIndex.type == TOKEN_IDENTIFIER)
	{
		mVectorIndex.data.stringData = _strdup(mVectorIndex.data.stringData);
	}
	//-----------------------------------------------------------------------


	// Find and store the vector name ! in front of @
	//-----------------------------------------------------------------------
	const char *it = strVecAccess;
	while(it >= codeLine && *it != ' ' && *it != '\t' && *it != '\n' && *it != 0)
		it--;

	it++;
	ParsedToken arrayNameToken;
	if (!ReadNextToken(it, arrayNameToken, secondToken) || arrayNameToken.type != TOKEN_IDENTIFIER || (it > strVecAccess))
	{
		printf("Line %d, Error when parsing the vector access: can't retrieve the vector accessed name", lineNo);
	}

	strncpy(mVectorName, arrayNameToken.data.stringData, sizeof(mVectorName));
	//-----------------------------------------------------------------------

	const char* equalPos = strstr(codeLine, "=");
	if (equalPos == NULL)
	{
		printf("Line %d, Error when parsing the vector access: seems like an array of processes access is not used in an assignment. Use it only on assignments", lineNo);
		return false;
	}

	equalPos++;

	if (equalPos > strVecAccess) 	// name@[index].item = value;
	{
		ParsedToken structItemName;
		if (*(strIndexParanthesisClose+1) != '.' || !ReadNextToken(strIndexParanthesisClose + 2, structItemName, secondToken) || structItemName.type != TOKEN_IDENTIFIER)
		{
			printf("Line %d, Error when parsing the vector access: Can't read correctly the accessed item in the vector component: n@[i].item", lineNo);
			return false;
		}
		
		strncpy(mVectorStructItem, structItemName.data.stringData, sizeof(mVectorStructItem));
		if (!ReadNextToken(equalPos, mValueAssigned, secondToken) || mValueAssigned.type == TOKEN_ERROR)
		{
			printf("Line %d, Error when parsing the vector access: Can't read correctly the value item: n@[i].item = value", lineNo);
			return false;
		}

		mVectorAccessParsingRes = VAP_WRITE;
	}
	else	// SimpleProcessItem*/VectorProcessItem* identifier = a@[index];
	{
		const char* simplePI = "SimpleProcessItem*";
		const char* vectorPI = "VectorProcessItem*";

		ParsedToken variableName;

		if ((codeLine = strstr(codeLine, simplePI)) != NULL)
		{
			if (!ReadNextToken(codeLine + strlen(simplePI), variableName, secondToken) || variableName.type != TOKEN_IDENTIFIER)
			{
				printf("Line %d, Error when parsing the vector access: can't read the identifier in: SimpleProcessItem* identifier = vectorName@[index]; OR VectorProcessItem* identifier = vectorName@[index];", lineNo);
				return false;
			}
			strncpy(mVectorStructItem, variableName.data.stringData, sizeof(mVectorStructItem));
			mVectorAccessParsingRes = VAP_READ_SP;
		}
		else if ((codeLine = strstr(codeLine, vectorPI)) != NULL)
		{
			if (!ReadNextToken(codeLine + strlen(vectorPI), variableName, secondToken) || variableName.type != TOKEN_IDENTIFIER)
			{
				printf("Line %d, Error when parsing the vector access: can't read the identifier in: VectorProcessItem* identifier = vectorName@[index]; OR VectorProcessItem* identifier = vectorName@[index];", lineNo);
				return false;
			}

			strncpy(mVectorStructItem, variableName.data.stringData, sizeof(mVectorStructItem));
			mVectorAccessParsingRes = VAP_READ_VP;
		}
		else
		{
			printf("Line %d, Error when parsing the vector access: Line must be: SimpleProcessItem* identifier = vectorName@[index]; OR VectorProcessItem* identifier = vectorName@[index];", lineNo);
			return false;
		}
	}

	return true;
}


std::string ParserProcessesVector::GetSpacesStringInFront()
{
	std::string my;
	for (int i = 0;i < m_iTabsEatenInFront; i++)
		my += '\t';

	for (int i = 0;i < m_iSpacesEatenInFront; i++)
		my += ' ';		

	return my;
}


bool ParserProcessesVector::ParseCommentLine(const char* codeLine)
{
	while(*codeLine != 0 && (*codeLine == ' ' || *codeLine == '\t' || *codeLine == '\n'))
		codeLine++;

	return strstr(codeLine, "//") == codeLine;
}