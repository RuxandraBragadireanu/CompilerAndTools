#ifndef MAIN_INPUT_PARSER_H
#define MAIN_INPUT_PARSER_H

#include "InputTypes.h"
#include <stdio.h>

class MainInputParser
{
public:
	static void ParseInputFile();

private:
	static void ParseSimpleDataType(FILE*& f, IDataTypeItem* pDataType, DataTypes eDataType);
	static void ParseVectorDataType(FILE*& f, IDataTypeItem* pDataType, DataTypes eVectorDataType);
	static void ParseVectorOfProcessesType(FILE*& f, VectorProcessItem* pDataType, const ArrayOfBaseProcessInputs& pElementType);
};

#endif