#include "WHILENode.h"

class ProgramAsignment;
class SymbolTable;

enum EForNormalType
{
	E_FORNORMAL_UNDEFINED = -1,
	E_FORNORMAL_S = DTYPE_FORNORMAL_S,
	E_FORNORMAL_T = DTYPE_FORNORMAL_T,
	E_FORNORMAL_ST = DTYPE_FORNORMAL_ST,
};

class ProgramFORNormal : public ProgramWHILE
{
public:
	ProgramFORNormal(int lineNo) : ProgramWHILE(lineNo), mForType(E_FORNORMAL_UNDEFINED)
	{
		// Overwrite the type of the program here
		m_eNodeType = E_NODE_TYPE_FORNORMAL;
		m_pInitExpression = NULL;
		m_pPostExpression = NULL;
	}

	EForNormalType	mForType;
	ProgramAsignment*	m_pInitExpression;
	IExpression*		m_pPostExpression; 

	virtual ProgramBase* Clone();

	virtual bool IsSpatialIteration()  { return mForType == E_FORNORMAL_S; }
	virtual bool IsTemporalIteration() { return mForType == E_FORNORMAL_T; }
	virtual bool IsDiagonalIteration() { return mForType == E_FORNORMAL_ST; }
	virtual void OnBeforeFirstTimeConditionEval();
	virtual void OnAfterChildProgramFinished();

	virtual bool DoSpecificValidationOrSymbolInherit(SymbolTable* pParent);
};
