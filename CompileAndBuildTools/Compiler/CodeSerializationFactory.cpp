#include "CodeSerializationFactory.h"
#include "BaseNode.h"
#include "INTERMEDIATENode.h"
#include "ReferenceNode.h"
#include "DECLARATIONNode.h"
#include "FOREachNode.h"
#include "FORNormalNode.h"
#include "IFNode.h"
#include "WHILENode.h"
#include "ASIGNMENTNode.h"
#include "calc3_utils.h"

#include "InputTypes.h"
#include "Streams.h"

#ifdef RUN_FROM_VS_DEBUGGER
#define CSF_INIT_SERIALIZE_CHECK \
	char* totalSizeAddressToWrite = stream.m_BufferPos; \
	stream.WriteSimpleType(0);

#define CSF_END_SERIALIZE_CHECK	\
	const int totalSize = stream.m_BufferPos - totalSizeAddressToWrite;	\
	assert(expectedSize == totalSize && "A different data size than expected was written for expression serialization");	\
	memcpy(totalSizeAddressToWrite, &totalSize, sizeof(int));

#define CSF_INIT_DESERIALIZE_CHECK	\
	const char* beginDesAddr = stream.m_BufferPos;	\
	int expectedSize = 0;	\
	stream.ReadSimpleType(expectedSize);	

#define CSF_END_DESERIALIZE_CHECK  \
	assert(result != NULL && "Result variable was not set at the end of deserialization process");	\
	const int dataRead = stream.m_BufferPos - beginDesAddr;	\
	assert(dataRead == expectedSize && "Didn't read data as expected on deserialization process");

#else
#define CSF_INIT_SERIALIZE_CHECK
#define CSF_END_SERIALIZE_CHECK
#define CSF_INIT_DESERIALIZE_CHECK
#define CSF_END_DESERIALIZE_CHECK
#endif

// ------------------------------ Programs' body --------------------------------------------
int CodeSerializationFactory::GetSerializedModuleSize(ProgramIntermediateModule* moduleDecl)
{
	// [Size | name | input blocks | operator | body].
	// If program has a defined operator body = [child0 | child1]

	int totalSize = sizeof(int);
	totalSize += Streams::GetStringSizeOnStream(moduleDecl->m_szModuleName);
	totalSize += GetSerializedDataTypeSize_InputBlocks(moduleDecl);
	totalSize += sizeof(moduleDecl->m_operator);

	switch (moduleDecl->m_operator)
	{
	case E_COMP_DIAGONAL:
	case E_COMP_HORIZONTAL:
	case E_COMP_VERTICAL:
		{
			totalSize += GetSerializedProgramBodySize(moduleDecl->m_pChilds[0]);
			totalSize += GetSerializedProgramBodySize(moduleDecl->m_pChilds[1]);
		}
		break;
	case E_COMP_UNDEFINED:
		{
			totalSize += GetSerializedProgramBodySize(moduleDecl->m_pChilds[0]);
		}
		break;
	default:
		break;
	}

	return totalSize;
}

void CodeSerializationFactory::SerializeModule(ProgramIntermediateModule* moduleDecl, Streams::BytesStreamWriter& stream)
{
	// [Size | name | input blocks | operator | body].
	// If program has a defined operator body = [child0 | child1]
	const int expectedSize = GetSerializedModuleSize(moduleDecl);
	CSF_INIT_SERIALIZE_CHECK

	stream.WriteString(moduleDecl->m_szModuleName);
	SerializeAsDataType_InputBlocks(moduleDecl, stream);

	stream.WriteSimpleType(moduleDecl->m_operator);

	// As an optimization, I decided to include directly the two child if there is a composition operator defined.
	switch (moduleDecl->m_operator)
	{
	case E_COMP_DIAGONAL:
	case E_COMP_HORIZONTAL:
	case E_COMP_VERTICAL:
		{
			SerializeProgramBody(moduleDecl->m_pChilds[0], stream);
			SerializeProgramBody(moduleDecl->m_pChilds[1], stream);
		}
		break;
	case E_COMP_UNDEFINED:
		{
			if (moduleDecl->m_pChilds[0])
			{
				SerializeProgramBody(moduleDecl->m_pChilds[0], stream);
			}
		}
		break;
	default:
		{
			LOG(("Operator not defined for serialization %d. Run resulted executable with \'f\' option to create AST.\n", moduleDecl->m_operator));
			assert(false);
		}
		break;
	}

	CSF_END_SERIALIZE_CHECK
}

ProgramIntermediateModule* CodeSerializationFactory::DeserializeModule(Streams::BytesStreamReader& stream)
{
	// [Size | name | input blocks | operator | body].
	// If program has a defined operator body = [child0 | child1]
	CSF_INIT_DESERIALIZE_CHECK

	char buff[1024];
	stream.ReadString(buff, 1024);

	InputBlock* north = NULL;
	InputBlock* south = NULL;
	InputBlock* east = NULL;
	InputBlock* west = NULL;
	DeserializeAsDataType_InputBlocks(north, west, east, south, stream);	
	ProgramInput	moduleInput  = { north, west };
	ProgramOutput	moduleOutput = { south, east };

	ECompositionOperator compOperator = E_COMP_UNDEFINED;
	stream.ReadSimpleType(compOperator);

	// As an optimization, I decided to include directly the two child if there is a composition operator defined.
	ProgramBase* body = NULL;
	switch (compOperator)
	{
	case E_COMP_DIAGONAL:
	case E_COMP_HORIZONTAL:
	case E_COMP_VERTICAL:
		{
			ProgramBase* child0 = DeserializeProgramBody(stream);
			ProgramBase* child1 = DeserializeProgramBody(stream);

			body = static_cast<ProgramIntermediateModule*>(ABSTFactory::CreateIntermediateProgram(child0, child1, compOperator));
		}
		break;
	case E_COMP_UNDEFINED:
		{
			body = static_cast<ProgramBase*>(DeserializeProgramBody(stream));
		}
		break;
	default:
		{
			LOG(("Operator not defined for serialization %d. Run resulted executable with \'f\' option to create AST.\n", compOperator));
			assert(false);
		}
		break;
	}

	ProgramIntermediateModule* result = static_cast<ProgramIntermediateModule*>(ABSTFactory::CreateAgapiaModule(buff, &moduleInput, body, &moduleOutput));
	CSF_END_DESERIALIZE_CHECK	


	return result;
}

int CodeSerializationFactory::GetSerializedProgramBodySize(ProgramBase* program)
{
	// [Size | Type | Specific info]
	const int headerSize = sizeof(int) + sizeof(program->GetType());
	int totalSize = headerSize;
	
	switch(program->GetType())
	{
	case E_NODE_TYPE_IF:
		{
			// [condition | Num childs | Child 1 | Child2]
			ProgramIF* ifProgram = static_cast<ProgramIF*>(program);
			totalSize += IExpression::GetSerializedDataTypeSize(ifProgram->m_pCondition);

			totalSize += sizeof(int);
			totalSize += GetSerializedProgramBodySize(ifProgram->m_pIfProgram);
			totalSize += (ifProgram->m_pElseProgram ? GetSerializedProgramBodySize(ifProgram->m_pElseProgram) : 0);
		}
		break;
	case E_NODE_TYPE_FOREACH:
		{
			// [type | num iterations | child program ]
			ProgramFOREACH* foreProgram = static_cast<ProgramFOREACH*>(program);
			totalSize += sizeof(foreProgram->m_eType);
			totalSize += IExpression::GetSerializedDataTypeSize(foreProgram->m_pValueToGo);
			totalSize += GetSerializedProgramBodySize(foreProgram->m_pBaseChildProgram);
		}
		break;
	case E_NODE_TYPE_WHILE:
		{
			// [type | condition | child program ]
			ProgramWHILE* whileProgram = static_cast<ProgramWHILE*>(program);
			totalSize += sizeof(whileProgram->m_eWHILEType);
			totalSize += IExpression::GetSerializedDataTypeSize(whileProgram->m_pCondition);
			totalSize += GetSerializedProgramBodySize(whileProgram->m_pBaseProgram);
		}
		break;
	case E_NODE_TYPE_FORNORMAL:
		{
			ProgramFORNormal* fornProgram = static_cast<ProgramFORNormal*>(program);
			totalSize += sizeof(fornProgram->mForType);
			totalSize += GetSerializedProgramBodySize(fornProgram->m_pInitExpression);
			totalSize += IExpression::GetSerializedDataTypeSize(fornProgram->m_pCondition);
			totalSize += IExpression::GetSerializedDataTypeSize(fornProgram->m_pPostExpression);
			totalSize += GetSerializedProgramBodySize(fornProgram->m_pBaseProgram);
		}
		break;
	case E_NODE_TYPE_INTERMEDIATE:
		{
			// [Operation | Program left | Program right]
			ProgramIntermediateModule* opProgram = static_cast<ProgramIntermediateModule*>(program);
			
			totalSize += sizeof(opProgram->m_operator);
			totalSize += GetSerializedProgramBodySize(opProgram->m_pChilds[0]);
			totalSize += GetSerializedProgramBodySize(opProgram->m_pChilds[1]);
		}
		break;
	case E_NODE_TYPE_MODULE_REF:
		{
			ProgramReference* refProgram = static_cast<ProgramReference*>(program);
			totalSize += Streams::GetStringSizeOnStream(refProgram->m_strModuleReference);
		}
		break;
	case E_NODE_TYPE_IDENTITY:			// Programs with NIL as body code, that only eats/produces inputs (used to match the interfaces)
		{
			ProgramAsignment* assigProg = static_cast<ProgramAsignment*>(program);
			totalSize += IExpression::GetSerializedDataTypeSize(assigProg->m_ChildExpressions[0]);
			totalSize += IExpression::GetSerializedDataTypeSize(assigProg->m_ChildExpressions[1]);
		}
		break;
	case E_NODE_TYPE_DECLARATION:
		{
			assert("Not implemented yet serialization of E_NODE_TYPE_DECLARATION");
		}
		break;			
	case E_NODE_TYPE_ASIGNMENT:
		{
			assert("Not implemented yet serialization of E_NODE_TYPE_ASIGNMENT");
		}
		break;
	case E_NODE_TYPE_C_CODE_ZONE_ALL:	// A C task which can be executed on all procs
		{
			// 0
		}
		break;
	case E_NODE_TYPE_C_CODE_ZONE_MASTER:	// A C task which can be executed only on master
		{
			// 0
		}
		break;
	default:
		{
			assert("Unknown program type to serialize !!");
		}
		break;
	}

	return totalSize;
}

void CodeSerializationFactory::SerializeProgramBody(ProgramBase* program, Streams::BytesStreamWriter& stream)
{
	// [Size | Type | Specific info]
	const int expectedSize = GetSerializedProgramBodySize(program);
	CSF_INIT_SERIALIZE_CHECK

	stream.WriteSimpleType(program->GetType());

	switch(program->GetType())
	{
	case E_NODE_TYPE_IF:
		{
			// [condition | Num childs | Child 1 | Child2]
			ProgramIF* ifProgram = static_cast<ProgramIF*>(program);
			IExpression::SerializeDataType(ifProgram->m_pCondition, stream);

			const int numChilds = (ifProgram->m_pElseProgram ? 2 : 1);
			stream.WriteSimpleType(numChilds);
			SerializeProgramBody(ifProgram->m_pIfProgram, stream);
			if (ifProgram->m_pElseProgram)
			{
				SerializeProgramBody(ifProgram->m_pElseProgram, stream);
			}
		}
		break;
	case E_NODE_TYPE_FOREACH:
		{
			// [type | num iterations | child program ]
			ProgramFOREACH* foreProgram = static_cast<ProgramFOREACH*>(program);
			stream.WriteSimpleType(foreProgram->m_eType);
			IExpression::SerializeDataType(foreProgram->m_pValueToGo, stream);
			SerializeProgramBody(foreProgram->m_pBaseChildProgram, stream);
		}
		break;
	case E_NODE_TYPE_WHILE:
		{
			// [type | condition | child program ]
			ProgramWHILE* whileProgram = static_cast<ProgramWHILE*>(program);
			stream.WriteSimpleType(whileProgram->m_eWHILEType);
			IExpression::SerializeDataType(whileProgram->m_pCondition, stream);
			SerializeProgramBody(whileProgram->m_pBaseProgram, stream);
		}
		break;
	case E_NODE_TYPE_FORNORMAL:
		{
			// [type | init expr | cond | post expr | base prog ]
			ProgramFORNormal* fornProgram = static_cast<ProgramFORNormal*>(program);
			stream.WriteSimpleType(fornProgram->mForType);
			SerializeProgramBody(fornProgram->m_pInitExpression, stream);
			IExpression::SerializeDataType(fornProgram->m_pCondition, stream);
			IExpression::SerializeDataType(fornProgram->m_pPostExpression, stream);
			SerializeProgramBody(fornProgram->m_pBaseProgram, stream);
		}
		break;
	case E_NODE_TYPE_INTERMEDIATE:
		{
			// [Operation | Program left | Program right]
			ProgramIntermediateModule* opProgram = static_cast<ProgramIntermediateModule*>(program);

			stream.WriteSimpleType(opProgram->m_operator);
			SerializeProgramBody(opProgram->m_pChilds[0], stream);
			SerializeProgramBody(opProgram->m_pChilds[1], stream);
		}
		break;
	case E_NODE_TYPE_MODULE_REF:
		{
			ProgramReference* refProgram = static_cast<ProgramReference*>(program);
			stream.WriteString(refProgram->m_strModuleReference);
		}
		break;
	case E_NODE_TYPE_IDENTITY:			// Programs with NIL as body code, that only eats/produces inputs (used to match the interfaces)
		{
			// 0
		}
		break;
	case E_NODE_TYPE_DECLARATION:
		{
			assert("Not implemented yet serialization of E_NODE_TYPE_DECLARATION");
		}
		break;			
	case E_NODE_TYPE_ASIGNMENT:
		{
			ProgramAsignment* assigProg = static_cast<ProgramAsignment*>(program);
			IExpression::SerializeDataType(assigProg->m_ChildExpressions[0], stream);
			IExpression::SerializeDataType(assigProg->m_ChildExpressions[1], stream);
		}
		break;
	case E_NODE_TYPE_C_CODE_ZONE_ALL:	// A C task which can be executed on all procs
		{
			// 0
		}
		break;
	case E_NODE_TYPE_C_CODE_ZONE_MASTER:	// A C task which can be executed only on master
		{
			// 0
		}
		break;
	default:
		{
			assert("Unknown program type to serialize !!");
		}
		break;
	}

	CSF_END_SERIALIZE_CHECK
}

ProgramBase* CodeSerializationFactory::DeserializeProgramBody(Streams::BytesStreamReader& stream)
{
	// [Size | Type | Specific info]
	CSF_INIT_DESERIALIZE_CHECK

	ENodeType nodeType = E_NODE_TYPE_INTERMEDIATE;
	stream.ReadSimpleType(nodeType);

	ProgramBase* result = NULL;

	switch(nodeType)
	{
	case E_NODE_TYPE_IF:
		{
			// [condition | Num childs | Child 1 | Child2]
			IExpression* condition = IExpression::DeserializeDataType(stream);

			int numChilds = 0;
			stream.ReadSimpleType(numChilds);
			ProgramBase* ifBranch = DeserializeProgramBody(stream);
			ProgramBase* elseBranch = (numChilds == 2 ? DeserializeProgramBody(stream) : NULL);

			result = static_cast<ProgramBase*>(ABSTFactory::CreateIFProgram(condition, ifBranch, elseBranch));
		}
		break;
	case E_NODE_TYPE_FOREACH:
		{
			EForEachType type = E_FOREACH_UNDEFINED;
			stream.ReadSimpleType(type);
			IExpression* valueToGo = IExpression::DeserializeDataType(stream);
			ProgramBase* childProg = DeserializeProgramBody(stream);

			result = static_cast<ProgramBase*>(ABSTFactory::CreateFOREACHProgram(type, valueToGo, childProg));
		}
		break;
	case E_NODE_TYPE_WHILE:
		{
			EWHILEType type = E_WHILE_UNDEFINED;
			stream.ReadSimpleType(type);
			IExpression* condition = IExpression::DeserializeDataType(stream);
			ProgramBase* childProg = DeserializeProgramBody(stream);

			result = static_cast<ProgramBase*>(ABSTFactory::CreateWHILEProgram(type, condition, childProg));
		}
		break;
	case E_NODE_TYPE_FORNORMAL:
		{
			// [type | init expr | cond | post expr | base prog ]
			EForNormalType fornType = E_FORNORMAL_UNDEFINED;
			stream.ReadSimpleType(fornType);
			ProgramBase* initExpression		 = DeserializeProgramBody(stream);
			IExpression* condition			 = IExpression::DeserializeDataType(stream);
			IExpression* postExpr			 = IExpression::DeserializeDataType(stream);
			ProgramBase* baseProg			 = DeserializeProgramBody(stream);

			result = static_cast<ProgramBase*>(ABSTFactory::CreateFORNormalProgram(fornType, initExpression, condition, postExpr, baseProg));
		}
		break;
	case E_NODE_TYPE_INTERMEDIATE:
		{
			// [Operation | Program left | Program right]
			ECompositionOperator op = E_COMP_UNDEFINED;
			stream.ReadSimpleType(op);
			ProgramBase* leftChild = DeserializeProgramBody(stream);
			ProgramBase* rightChild = DeserializeProgramBody(stream);

			result = static_cast<ProgramBase*>(ABSTFactory::CreateIntermediateProgram(leftChild, rightChild, op));
		}
		break;
	case E_NODE_TYPE_MODULE_REF:
		{
			char buff[1024];
			stream.ReadString(buff, 1024);

			result = static_cast<ProgramBase*>(ABSTFactory::CreateModuleRef(buff));
		}
		break;
	case E_NODE_TYPE_IDENTITY:			// Programs with NIL as body code, that only eats/produces inputs (used to match the interfaces)
		{
			// 0
			result = static_cast<ProgramBase*>(ABSTFactory::CreateIdentityProgram());
		}
		break;
	case E_NODE_TYPE_DECLARATION:
		{
			assert("Not implemented yet serialization of E_NODE_TYPE_DECLARATION");
		}
		break;			
	case E_NODE_TYPE_ASIGNMENT:
		{
			IExpression* leftExpr = IExpression::DeserializeDataType(stream);
			IExpression* rightExpr = IExpression::DeserializeDataType(stream);

			result = static_cast<ProgramBase*>(ABSTFactory::CreateAsignmentProgram(leftExpr, rightExpr));
		}
		break;
	case E_NODE_TYPE_C_CODE_ZONE_ALL:	// A C task which can be executed on all procs
		{
			result = static_cast<ProgramBase*>(ABSTFactory::CreateCCodeProgramType(E_CCODE_FORALL));
		}
		break;
	case E_NODE_TYPE_C_CODE_ZONE_MASTER:	// A C task which can be executed only on master
		{
			result = static_cast<ProgramBase*>(ABSTFactory::CreateCCodeProgramType(E_CCODE_FORMASTER));
		}
		break;
	default:
		{
			assert("Unknown program type to serialize !!");
		}
		break;
	}

	CSF_END_DESERIALIZE_CHECK

	return result;
}

//----------------------------------------------------------------------------------------------------------



// --------------------------------------- Input blocks -----------------------------------
int CodeSerializationFactory::GetSerializedDataSizeArrayOfBaseProcess(const ArrayOfBaseProcessInputs& arrayOfBaseProcess)
{
	// [size | num items | type + size of each item ]
	int totalSize = sizeof(arrayOfBaseProcess.size()) + sizeof(int);
	for (ArrayOfBaseProcessInputsConstIter processIt = arrayOfBaseProcess.begin(); processIt != arrayOfBaseProcess.end(); processIt++)
	{
		BaseProcessInput* baseProcessInput = *processIt;
		totalSize += sizeof(baseProcessInput->GetType());
		totalSize += (*processIt)->GetSerializedDataTypeSize();
	}

	return totalSize;
}

void CodeSerializationFactory::SerializeDataTypeArrayOfBaseProcess(const ArrayOfBaseProcessInputs& arrayOfBaseProcess, Streams::BytesStreamWriter& stream)
{
	// [size | num items | type + size of each item ]
	const int expectedSize = GetSerializedDataSizeArrayOfBaseProcess(arrayOfBaseProcess);
	CSF_INIT_SERIALIZE_CHECK

	stream.WriteSimpleType(arrayOfBaseProcess.size());
	for (ArrayOfBaseProcessInputsConstIter processIt = arrayOfBaseProcess.begin(); processIt != arrayOfBaseProcess.end(); processIt++)
	{
		BaseProcessInput* baseProcessInput = *processIt;
		stream.WriteSimpleType(baseProcessInput->GetType());
		(*processIt)->SerializeAsDataType(stream);
	}

	const ArrayOfBaseProcessInputs* result = &arrayOfBaseProcess; // Dummy decl, just for def END_SERIALIZE_CHECK to compile
	CSF_END_SERIALIZE_CHECK
}

void CodeSerializationFactory::DeserializeDataTypeArrayOfBaseProcess(ArrayOfBaseProcessInputs& arrayOfBaseProcess, Streams::BytesStreamReader& stream)
{
	// [size | num items | type + size of each item ]

	CSF_INIT_DESERIALIZE_CHECK

	int numItems = 0;
	stream.ReadSimpleType(numItems);

	for (int i = 0; i < numItems; i++)
	{
		// Create by type !
		EInputTypes inputType;
		stream.ReadSimpleType(inputType);

		BaseProcessInput* processInput = NULL;
		if (inputType == E_INPUT_ARRAY)
		{
			processInput = new VectorProcessItem();
		}
		else
		{
			processInput = new SimpleProcessItem();
		}

		processInput->DeserializeAsDataType(stream);
		arrayOfBaseProcess.push_back(processInput);
	}

	const ArrayOfBaseProcessInputs* result = &arrayOfBaseProcess; // Dummy decl, just for def END_SERIALIZE_CHECK to compile
	CSF_END_DESERIALIZE_CHECK
}

int CodeSerializationFactory::GetSerializedInputBlockSize(InputBlock* block)
{
	// [size | IsNorthBlock -> int | array of base processes]
	int totalSize = sizeof(int) + sizeof(int);
	totalSize += GetSerializedDataSizeArrayOfBaseProcess(block->m_InputsInBlock);
	return totalSize;
}

void CodeSerializationFactory::SerializeInputBlock(InputBlock* block, Streams::BytesStreamWriter& stream)
{
	// [isnorthBlock (int) | Array of processes]
	const int expectedSize = GetSerializedInputBlockSize(block);
	CSF_INIT_SERIALIZE_CHECK

	const int isNorthBlock = static_cast<int>(block->m_bIsNorthBlock);
	stream.WriteSimpleType(isNorthBlock);
	SerializeDataTypeArrayOfBaseProcess(block->m_InputsInBlock, stream);

	CSF_END_SERIALIZE_CHECK
}

InputBlock* CodeSerializationFactory::DeserializeInputBlock(Streams::BytesStreamReader& stream)
{
	// [isnorthBlock (int) | Array of processes]
	CSF_INIT_DESERIALIZE_CHECK

	InputBlock* result = new InputBlock();

	int isNorthBlock = 0;
	stream.ReadSimpleType(isNorthBlock);
	result->m_bIsNorthBlock = isNorthBlock == 1 ? true : false;

	ArrayOfBaseProcessInputs arrayOfBaseProcesses;
	DeserializeDataTypeArrayOfBaseProcess(arrayOfBaseProcesses, stream);

	for (ArrayOfBaseProcessInputsConstIter it = arrayOfBaseProcesses.begin(); it != arrayOfBaseProcesses.end(); it++)
	{
		result->AddInput(*it);
	}

	CSF_END_DESERIALIZE_CHECK

	return result;
}

int CodeSerializationFactory::GetSerializedDataTypeSize_InputBlocks(ProgramIntermediateModule* program)
{
	int totalSize = 0;
	totalSize += GetSerializedInputBlockSize(program->m_pInputNorth);
	totalSize += GetSerializedInputBlockSize(program->m_pInputWest);
	totalSize += GetSerializedInputBlockSize(program->m_pOutputEast);
	totalSize += GetSerializedInputBlockSize(program->m_pOutputSouth);

	return  totalSize;
}

void CodeSerializationFactory::SerializeAsDataType_InputBlocks(ProgramIntermediateModule* program, Streams::BytesStreamWriter& stream)
{
	SerializeInputBlock(program->m_pInputNorth, stream);
	SerializeInputBlock(program->m_pInputWest, stream);
	SerializeInputBlock(program->m_pOutputEast, stream);
	SerializeInputBlock(program->m_pOutputSouth, stream);
}

void CodeSerializationFactory::DeserializeAsDataType_InputBlocks(InputBlock*& north, InputBlock*& west, 
																 InputBlock*& east, InputBlock*& south, 
																 Streams::BytesStreamReader& stream)
{
	north = DeserializeInputBlock(stream);
	west  = DeserializeInputBlock(stream);
	east  = DeserializeInputBlock(stream);
	south = DeserializeInputBlock(stream);
}

// -----------------------------------------------------------------------------------------

