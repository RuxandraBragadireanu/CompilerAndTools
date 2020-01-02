#include "AgapiaToCCode.h"
#include "ExecutionBlackbox.h"
#include "InputTypes.h"

#include "Includes.h"


void IDENTITY(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& b = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();


	// User code: 
		printf("Asta a fost\n");
	

}




void TESTANDGODOWN(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& b = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& bout = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();


	// User code: 
		printf("%d\n",b);
		bout = b;
	

}




void TESTANDGODOWN2(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& b = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& bout = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();


	// User code: 
		bout = 0;
	

}


void InitializeAgapiaToCFunctions()
{
ExecutionBlackbox::Get()->AddAgapiaToCFunction("IDENTITY", &IDENTITY);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("TESTANDGODOWN", &TESTANDGODOWN);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("TESTANDGODOWN2", &TESTANDGODOWN2);
}
