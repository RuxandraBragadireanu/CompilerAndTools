#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include "calc3_defs.h"
#include <stdio.h>
#include "SymbolTable.h"
#include "InputTypes.h"

#include <stack>
#include <set>

enum EGeneralExpressionType
{
	E_EXPR_VALUE,
	E_EXPR_ARRAY_ACCESS,
	E_EXPR_CONSTANT,
	E_EXPR_VARIABLE,
	E_EXPR_BOOLEAN
};

enum EDominantType
{	// -1 if undefined or error
	E_DOM_BOOL = ETYPE_BOOL,
	E_DOM_CHAR = ETYPE_CHAR,
	E_DOM_INT = ETYPE_INT, 
	E_DOM_FLOAT = ETYPE_FLOAT,
	E_DOM_STRING = ETYPE_STRING,
	E_DOM_ARRAY	= ETYPE_GENERAL_ARRAY,
	E_DOM_DATA_BUFFER = ETYPE_DATA_BUFFER,
	E_NUM_TYPES,
};

enum AsignmentCompatibility
{
	OK,	// OK
	WA,	// Warning
	ER,	// Error 
};

class IExpression;

typedef std::pair<IExpression*, int> ExprTreeStackElement;
typedef std::stack<ExprTreeStackElement> ExprTreeStack;


typedef std::set<std::string>			VariablesNameSet;
typedef VariablesNameSet::iterator		VariablesNameSetIter;


class IExpression
{
public:
	EGeneralExpressionType	m_eGeneralType;

	// Dominant type of an expression
	EDominantType			m_eDominantType;

	virtual ~IExpression(){}

	virtual IExpression* Clone() = 0;
	void CopyBase(IExpression* pCloned);

	IExpression(EGeneralExpressionType eType, int lineNo) : m_eGeneralType(eType), mDebugLineNo(lineNo) {}
	void SetDominantType(EDominantType eType) { m_eDominantType = eType; }
	EDominantType GetDominantType() { return m_eDominantType ;}
	EGeneralExpressionType GetGeneralExpressionType() const { return m_eGeneralType ; }

	// Tries to parse the current expression, set its dominant type and check if types are ok, identifiers exits etc - It raises error for each incorrect thing!
	virtual bool ValidateExpression(SymbolTable* pSymbolTable) = 0;

	// Puts in the setOfVariableNames set the variable names inside this expression
	virtual void CollectInternalVariableNames(VariablesNameSet& setOfVariableNames) = 0;

	// Iterators functionality
	virtual char* GetNextInputItem();
	virtual void BeginIteration();
	virtual ExprTreeStack* GetIterationStack() { return NULL; }
	virtual int GetNumberOfChilds() { return 0;}

	virtual int GetValueAsInt() { return 0; }
	virtual void SetValueAsInt(int value) { }

	virtual float GetValueAsFloat() { return 0.0f; }
	virtual char* GetValueAsString() { return NULL ;}
	virtual bool GetValueAsBool() { return false; }
	virtual char GetValueAsChar() { return 0; }

	static int SerializeDataType(const IExpression* expr, Streams::BytesStreamWriter& stream);
	static IExpression* DeserializeDataType(Streams::BytesStreamReader& stream);
	static int GetSerializedDataTypeSize(const IExpression* expr);

	// Decides whether two types that have an assignment operation are compatible or not
	// first type is left, type2 is right
	static AsignmentCompatibility GetAsignmentsTypesCompatibility(EDominantType type1, EDominantType type2);

	// Get the dominant of two types ( used for an value expression )
	static int GetDominantOfTwoTypes(EDominantType type1, EDominantType type2);

	static AsignmentCompatibility gAsignmentCompatible[E_NUM_TYPES][E_NUM_TYPES];
	static int gTypeDominancyTable[E_NUM_TYPES][E_NUM_TYPES];

	int mDebugLineNo;
};

enum EValueExpressionType
{
	E_UNARYMINUS,
	E_OP_PLUS,
	E_OP_MINUS,
	E_OP_MULT,
	E_OP_DIV,
	E_OP_MOD,
	E_OP_INCREMENT,
	E_OP_PINCREMENT,
	E_OP_DECREMENT,
	E_OP_PDECREMENT,
};

enum EBooleanExpressionType
{
	E_BIG_THAN,
	E_BIG_OR_EQUAL_THAN,
	E_SMALL_THAN,
	E_SMALL_OR_EQUAL_THAN,
	E_EQUAL,
	E_AND,
	E_OR,
	E_DIFFERENT
};

class ExpressionArrayAccess : public IExpression
{
public:
	ExpressionArrayAccess(int lineNo) : IExpression(E_EXPR_ARRAY_ACCESS, lineNo), m_pIndexExpression(NULL), m_szArrayName(NULL), m_InputRefItem(NULL)
		, m_pAsArrayOfBools(NULL), m_pAsArrayOfChars(NULL), m_pAsArrayOfFloats(NULL), m_pAsArrayOfStrings(NULL), m_pAsArrayOfInts(NULL) {}
	~ExpressionArrayAccess();

	void SetIndicesAndName(char *szIdentifier, IExpression* pIndexExpression);

	// Should set the right dominant type (bool , int , float, string etc) depending on the symbol table values
	virtual bool ValidateExpression(SymbolTable* pSymbolTable);
	virtual void CollectInternalVariableNames(VariablesNameSet& setOfVariableNames);

	void SetValue(bool bValue);
	void SetValue(char cValue);
	void SetValue(float fValue);
	void SetValue(char* szValue);
	void SetValue(int iValue);

	virtual int GetValueAsInt();
	virtual float GetValueAsFloat();
	virtual char* GetValueAsString();
	virtual bool GetValueAsBool();

	virtual IExpression* Clone();

	// [index]
	IExpression*	m_pIndexExpression;
	char *m_szArrayName;

	BoolVectorDataItem*			m_pAsArrayOfBools;
	FloatVectorDataItem*		m_pAsArrayOfFloats;
	StringVectorDataItem*		m_pAsArrayOfStrings;
	IntVectorDataItem*			m_pAsArrayOfInts;
	CharVectorDataItem*			m_pAsArrayOfChars;

	IDataTypeItem* m_InputRefItem;
};

class ExpressionVariable : public IExpression
{
public:
	ExpressionVariable(int lineNo) : IExpression(E_EXPR_VARIABLE, lineNo), 
		 m_szName(NULL)/*,m_InputRefItem(NULL)*/,m_pAsBoolData(NULL), m_pAsCharData(NULL), m_pAsFloatData(NULL), m_pAsStringData(NULL), m_pAsIntData(NULL) {}
	~ExpressionVariable() {}

	void SetName(char *szIdentifier);

	// Should set the right dominant type (bool , int , float, string etc) depending on the symbol table values
	virtual bool ValidateExpression(SymbolTable* pSymbolTable);
	virtual void CollectInternalVariableNames(VariablesNameSet& setOfVariableNames);

	virtual int GetValueAsInt();
	virtual void SetValueAsInt(int value);

	virtual float GetValueAsFloat();
	virtual char* GetValueAsString();
	virtual bool GetValueAsBool();

	virtual IExpression* Clone();

	BoolDataItem*		m_pAsBoolData;
	CharDataItem*		m_pAsCharData;
	FloatDataItem*		m_pAsFloatData;
	StringDataItem*		m_pAsStringData;
	IntDataItem*		m_pAsIntData;

	char *m_szName;
	//IDataTypeItem* m_InputRefItem;
};

class ExpressionConstant : public IExpression
{
public:
	ExpressionConstant(int lineNo) : IExpression(E_EXPR_CONSTANT, lineNo), m_szVariable(NULL)/*, m_InputRefItem(NULL)*/{}
	virtual ~ExpressionConstant();

#pragma warning(disable:4800)
	void SetValue(int iValue) { m_CharValue = (char)iValue; m_IntValue = iValue; m_FloatValue = (float) iValue; m_BoolValue = (bool) iValue; SetDominantType(E_DOM_INT);}
	void SetValue(float fValue) { m_CharValue = (char)fValue; m_IntValue = (int)fValue; m_FloatValue =  fValue; m_BoolValue = (int) fValue; SetDominantType(E_DOM_FLOAT);}
#pragma warning(default:4800)

	void SetValue(bool bValue) { m_CharValue = (char)bValue; m_IntValue = (int) bValue; m_FloatValue = (float) bValue; m_BoolValue = bValue; SetDominantType(E_DOM_BOOL);}
	void SetValue(char *szValue);
	void SetVariableName(char *szVariableName);

	// Already set the dominant type
	virtual bool ValidateExpression(SymbolTable* pSymbolTable) { return true; }
	virtual void CollectInternalVariableNames(VariablesNameSet& setOfVariableNames);

	virtual int GetValueAsInt() { return m_IntValue ;}
	virtual float GetValueAsFloat() { return m_FloatValue; }
	virtual char* GetValueAsString() { return m_szVariable; }
	virtual bool GetValueAsBool() { return m_BoolValue; }
	virtual char GetValueAsChar() { return m_CharValue; }

	virtual IExpression* Clone();

	//IDataTypeItem* m_InputRefItem;
	int		m_IntValue;
	float	m_FloatValue;
	bool	m_BoolValue;
	char	m_CharValue;
	char*	m_szVariable;	
};

class ExpressionValueNode : public IExpression
{
public:
	ExpressionValueNode(int lineNo) : IExpression(E_EXPR_VALUE, lineNo), m_iChildsCount(0) { m_Childs[0] = m_Childs[1] = NULL; }

	// Give pChild1 as NULL if your expression has only a child
	void SetTypeAndChilds(EValueExpressionType eOpType, IExpression *pChild0, IExpression *pChild1);
	virtual void CollectInternalVariableNames(VariablesNameSet& setOfVariableNames);

	// At this moment we should already know that the types are compatible
	virtual int GetValueAsInt();
	virtual float GetValueAsFloat();
	virtual char* GetValueAsString();
	virtual bool GetValueAsBool();

	virtual bool ValidateExpression(SymbolTable* pSymbolTable);

	virtual IExpression* Clone();

	// Iteration types
	virtual int GetNumberOfChilds() { return (m_Childs[1] == NULL ? 1 : 2); }
	virtual ExprTreeStack* GetIterationStack() { return &m_StackIteration; }
	ExprTreeStack	m_StackIteration;

	IExpression* m_Childs[2];
	int m_iChildsCount;
	EValueExpressionType	m_eType;
};

class ExpressionBooleanNode : public IExpression
{
public:
	ExpressionBooleanNode(int lineNo): IExpression(E_EXPR_BOOLEAN, lineNo){ }
	// Give pChild1 as NULL if your expression has only a child
	void SetTypeAndChilds(EBooleanExpressionType eOpType, IExpression *pChild0, IExpression *pChild1);

	// At this moment we should already know that the types are compatible
	virtual int GetValueAsInt();
	virtual float GetValueAsFloat();
	virtual char* GetValueAsString();
	virtual bool GetValueAsBool();

	virtual bool ValidateExpression(SymbolTable* pSymbolTable);
	virtual void CollectInternalVariableNames(VariablesNameSet& setOfVariableNames);

	virtual IExpression* Clone();

	// Iterator types
	virtual int GetNumberOfChilds() { return (m_Childs[1] == NULL ? 1 : 2); }
	virtual ExprTreeStack* GetIterationStack() { return &m_StackIteration; }
	ExprTreeStack	m_StackIteration;

	// Childs and operation type between them
	IExpression* m_Childs[2];
	EBooleanExpressionType	m_eType;
};


#endif

