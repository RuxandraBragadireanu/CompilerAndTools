#ifndef BASE_NODE_H
#define BASE_NODE_H

#include "InputTypes.h"
#include "SymbolTable.h"
#include <set>
#include <string>

enum ECompositionOperator
{
	E_COMP_UNDEFINED,
	E_COMP_VERTICAL,
	E_COMP_HORIZONTAL,
	E_COMP_DIAGONAL,
};

enum ENodeType
{
	E_NODE_TYPE_IF,
	E_NODE_TYPE_FOREACH,
	E_NODE_TYPE_WHILE,
	E_NODE_TYPE_FORNORMAL,
	E_NODE_TYPE_INTERMEDIATE,
	E_NODE_TYPE_MODULE_REF,
	E_NODE_TYPE_IDENTITY,			// Programs with NIL as body code, that only eats/produces inputs (used to match the interfaces)
	E_NODE_TYPE_DECLARATION,
	E_NODE_TYPE_ASIGNMENT,
	E_NODE_TYPE_C_CODE_ZONE_ALL,	// A C task which can be executed on all procs
	E_NODE_TYPE_C_CODE_ZONE_MASTER,	// A C task which can be executed only on master
};

class IExpression;

class IProgram
{
public:
	virtual ENodeType	GetType() { return m_eNodeType; }
	IProgram(ENodeType eType) : m_eNodeType(eType) {}

	ENodeType		m_eNodeType;
};

class ProgramBase : public IProgram
{
public:
	void SetInputBlocks(InputBlock* pNorth, InputBlock* pWest) { m_pInputNorth = pNorth; m_pInputNorth->SetParent(this); m_pInputWest = pWest; m_pInputWest->SetParent(this); }
	void SetOutputBlocks(InputBlock* pSouth, InputBlock* pEast) { m_pOutputSouth = pSouth;  m_pOutputSouth->SetParent(this); m_pOutputEast = pEast; m_pOutputEast->SetParent(this); }

	virtual ProgramBase* Clone() = 0;
	void CopyBase(ProgramBase* pCloned);
	virtual bool SolveReferences() = 0;
	virtual bool SolveInputOutput() = 0;

	// Inherit in this program symbol table, the given one
	virtual bool DoBaseSymbolTableInheritance(SymbolTable *pParent);

	// Inherit the symbol table of a program
	// This is currently used in IF and WHILE programs such that the program inherits the symbol table of the child program,
	// and we can use in the condition, inputs of the child program
	// MUST BE CALLED BEFORE INHERITS THE BASE SYMBOL TABLE !! (global values are prioritar to the child ones)
	virtual bool DoSymbolTableInheritInputBlocks(InputBlock* pNorthBlock, InputBlock* pWestBlock, bool useLastVar=false);
	
	virtual bool Validate(SymbolTable *pParent, bool bAtRuntimeSpawn) = 0;

	ProgramBase(ENodeType eType, int lineNo)
		: IProgram(eType), m_iNumberOfInputsRemainedToReceive(0), m_bHasNoInput(false)
		, m_pInputNorth(NULL), m_pOutputSouth(NULL), m_pInputWest(NULL), m_pOutputEast(NULL)
		, m_pProgramParent(NULL)
		, m_bIsProgramFinished(false)
		, mDebugLineNo(lineNo)
		{ m_SymbolTable = new SymbolTable(this); 	
		}

	virtual ~ProgramBase() { delete m_SymbolTable;}
	

	// Return true if the parameter specified program must inherit the symbol table from parent
	static bool GetShouldGiveParentTableTo(ProgramBase *pProgram);

	SymbolTable*	m_SymbolTable;

	// Inputs / Outputs blocks definition and functionality
	int GetNorthInputProcCount()	{ return m_pInputNorth->GetProcessesCount(); }
	int GetWestInputProcCount()		{ return m_pInputWest->GetProcessesCount(); }
	int GetEastOutputProcCount()	{ return m_pOutputEast->GetProcessesCount(); }
	int GetSouthInputProcCount()	{ return m_pOutputSouth->GetProcessesCount(); }
	InputBlock *m_pInputNorth, *m_pInputWest;
	InputBlock *m_pOutputSouth, *m_pOutputEast;

	// Called when an input is received by the node
	// Certain program types are interested in inputs received moments
	// This should be called to create input linkage and all kinds of stuff depending by node
	// pOriginalInput  - the input received
	// pInputNodeReceiver - the node that received this input through the links
	// eDir - direction where this input came from
	virtual bool OnInputReceived(BaseProcessInput* pOriginalInput, BaseProcessInput* pInputNodeReceiver, EInputDirection eDir) { return true; }
	// This should be called AFTER the module has been linked and input assigned
	virtual void OnAfterInputReceived() { }

	// This is used in the input listeners - observers logic
	// Called when the szName variable is ready to be queried and this program is a listener for that variable
	virtual void OnInputItemReady(char* szName) { }

	// Returns the module name - if it has one
	// TODO: write a naming convention for automatic modules
	virtual const char* GetName() { return "";}
	int GetLineNo() const { return mDebugLineNo; }

	virtual bool IsAtomicProgram() { return false; }

	// This will be called when a child program of this will finish the execution.
	// A pointer to the child will he sent as a parameter
	virtual void OnChildProgramFinished(ProgramBase* pChildFinished) = 0;

	// Sends the message to the parent program that this one finished.
	// This one exist only on here, abstractize some tests
	virtual void SendProgramFinished(ProgramBase* pChildFinished);

	// Checks if this module has finished already or not.
	// Return false if not valid finished called
	virtual bool CheckFinishedCondition();

	// Number of inputs to be received - only if this is an atomic module
	int m_iNumberOfInputsRemainedToReceive;

	// True if this module has no input - only possible for user defined once, but cleaner to put it here
	bool m_bHasNoInput;

	// True is this program (and all its child finished execution)
	bool m_bIsProgramFinished;

	// The module that incorporates this one
	ProgramBase* m_pProgramParent;

	// Line number of this module to help for error messages
	int mDebugLineNo;
};



#endif
