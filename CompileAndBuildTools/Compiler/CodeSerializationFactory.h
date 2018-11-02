#ifndef CODE_SER_FACTORY
#define CODE_SER_FACTORY

#include "InputTypes.h"

class BaseNode;
class InputBlock;
class ProgramBase;
class ProgramIntermediateModule;

namespace Streams
{
	class BytesStreamWriter;
	class BytesStreamReader;
};

class CodeSerializationFactory
{
public:
	// Programs
	//----------------------
	// Module = program with declaration + body
	static int GetSerializedModuleSize(ProgramIntermediateModule* moduleDecl);
	static void SerializeModule(ProgramIntermediateModule* moduleDecl, Streams::BytesStreamWriter& stream);
	static ProgramIntermediateModule* DeserializeModule(Streams::BytesStreamReader& stream);


	static void SerializeProgramBody(ProgramBase* program, Streams::BytesStreamWriter& stream);
	static ProgramBase* DeserializeProgramBody(Streams::BytesStreamReader& stream);
	static int GetSerializedProgramBodySize(ProgramBase* program);
	//------------------------

	// Input blocks 
	//---------------------------------
	//Helpers for input blocks serializations
	static int GetSerializedDataTypeSize_InputBlocks(ProgramIntermediateModule* program);
	static void SerializeAsDataType_InputBlocks(ProgramIntermediateModule* program, Streams::BytesStreamWriter& stream);
	static void DeserializeAsDataType_InputBlocks(InputBlock*& north, InputBlock*& west, 
												  InputBlock*& east, InputBlock*& south, Streams::BytesStreamReader& stream);

	static void SerializeInputBlock(InputBlock* block, Streams::BytesStreamWriter& stream);
	static InputBlock* DeserializeInputBlock(Streams::BytesStreamReader& stream);
	static int GetSerializedInputBlockSize(InputBlock* block);

	static int GetSerializedDataSizeArrayOfBaseProcess(const ArrayOfBaseProcessInputs& arrayOfBaseProcess);
	static void SerializeDataTypeArrayOfBaseProcess(const ArrayOfBaseProcessInputs& arrayOfBaseProcess, Streams::BytesStreamWriter& stream);
	static void DeserializeDataTypeArrayOfBaseProcess(ArrayOfBaseProcessInputs& arrayOfBaseProcess, Streams::BytesStreamReader& stream);
	//--------------------------------
};

#endif