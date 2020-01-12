#include "AgapiaToCCode.h"
#include "ExecutionBlackbox.h"
#include "InputTypes.h"

#include "Includes.h"


void SHOWRESULTS(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& b = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	VectorProcessItem& results = *((VectorProcessItem*)pWest->m_InputsInBlock[0]);


	// User code: 
		int numItemsInWest = GetNumItemsInVectorProcessItem(results);
	   	printf("Results:\n");
	   	for (int i = 0; i < numItemsInWest; i++)
	   	{
			  SimpleProcessItem* pSBPP = (SimpleProcessItem*)GetVectorItemByIndex(results, i, 0);
	   		int val = ((IntDataItem*)pSBPP->GetItem(0))->GetValue();
	   		printf("%d\n", val);
	   	}
		
		printf("Finished Identity\n");
	

}




void TESTANDGODOWN(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& b = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& bout = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();


	// User code: 
		bout = b;
	

}




void TESTANDGODOWN2(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& b = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& bout = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& x = ((IntDataItem*)((SimpleProcessItem*)pEast->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();


	// User code: 
		bout = b - 1;
		x = bout * bout;
	

}


void InitializeAgapiaToCFunctions()
{
ExecutionBlackbox::Get()->AddAgapiaToCFunction("SHOWRESULTS", &SHOWRESULTS);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("TESTANDGODOWN", &TESTANDGODOWN);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("TESTANDGODOWN2", &TESTANDGODOWN2);
}
