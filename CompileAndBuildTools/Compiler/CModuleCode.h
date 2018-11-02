#ifndef COBJECT_CODE_H
#define COBJECT_CODE_H

#include <string>
#include <stdio.h>

#include "BaseNode.h"

class CModuleCode : public IProgram
{
public:
	CModuleCode(int eTargetType)
		:	IProgram((ENodeType)eTargetType)
	{
	}
	void AppendLineCode(const char* szCodeString);
	bool WriteCodeToFile(char *pModuleName, FILE* f);

public:
	std::string	m_strSourceCode;
};

#endif
