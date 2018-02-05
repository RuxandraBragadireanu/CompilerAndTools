#ifndef PARSER_PROCESSES_VECTOR
#define PARSER_PROCESSES_VECTOR

#include <string>

enum VectorAccessParsingRes
{
	VAP_NO_ACCESS,
	VAP_WRITE,
	VAP_READ_VP,
	VAP_READ_SP,
};

class ParserProcessesVector
{
public:
	enum TokenType
	{
		TOKEN_ERROR,
		TOKEN_IDENTIFIER,
		TOKEN_STRING,
		TOKEN_NUMBER_INT,
		TOKEN_NUMBER_FLOAT,
		TOKEN_BUFFER_DATA_PAIR, // (address, size)
	};

	union TokenData
	{
		int intData;
		float floatData;
		const char* stringData;
	};
	
	struct ParsedToken
	{
		TokenData data;
		TokenType type;

		ParsedToken()
		{
			Reset();
		}

		void Reset()
		{
			type = TOKEN_ERROR;
		}
	};

	ParserProcessesVector()	: mVectorAccessParsingRes(VAP_NO_ACCESS), m_iSpacesEatenInFront(0), m_iTabsEatenInFront(0)
	{

	}

	// Return true if succeeded to parse and to get data about an vector of processes access 
	bool ParseVectorAccessLine(const char* codeLine, int lineNo);
	bool ParseCommentLine(const char* );

	// Get data parsed
	const char* GetVectorName() { return mVectorName; }
	const char* GetVectorStructItem() { return mVectorStructItem; }
	const ParsedToken& GetVectorIndex() { return mVectorIndex; }
	const ParsedToken& GetValueAssigned() { return mValueAssigned; }
	const ParsedToken& GetSecondValueAssigned();
	int GetVectorAccessParsingRes() { return mVectorAccessParsingRes; }

	int GetNumTabsInFront() { return m_iTabsEatenInFront; }
	int GetNumSpacesInFront() { return m_iSpacesEatenInFront; }

	std::string GetSpacesStringInFront();

private:
	
	// Read the first token in codeLine to the outputToken. Returns the number of chars parsed in codeLine for the outputToken
	int ReadNextToken(const char *codeLine, ParsedToken& outputToken, ParsedToken& secondToken);

	// Statical stuff about parsing
	char mVectorName[512];
	char mVectorStructItem[512];	// "vector.item" , this will contain item if any
	ParsedToken mVectorIndex;
	ParsedToken mValueAssigned;		// @[].item = valueAssigned;

	VectorAccessParsingRes	mVectorAccessParsingRes;

	int m_iSpacesEatenInFront;
	int m_iTabsEatenInFront;
};


#endif