#include "AgapiaToCCode.h"
#include "ExecutionBlackbox.h"
#include "InputTypes.h"

#include "Includes.h"


void READ(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	VectorProcessItem& numbers = *((VectorProcessItem*)pSouth->m_InputsInBlock[0]);
	int& n = ((IntDataItem*)((SimpleProcessItem*)pEast->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();


	// User code: 
	    scanf("%d", &n);
	    for (int i = 0; i < n; i++)
		{
			int nr;
			scanf("%d", &nr);
		SetInputItemToVector(15, &numbers, i, "nr", nr);
		}
	

}




void SUM0(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& nr = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& n = ((IntDataItem*)((SimpleProcessItem*)pWest->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& resnew = ((IntDataItem*)((SimpleProcessItem*)pEast->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();


	// User code: 
		resnew = nr; 
	

}




void SUMI(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& nr = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& res = ((IntDataItem*)((SimpleProcessItem*)pWest->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& resnew = ((IntDataItem*)((SimpleProcessItem*)pEast->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();


	// User code: 
		resnew = res + nr;
	

}




void SUMRESULT(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& res = ((IntDataItem*)((SimpleProcessItem*)pWest->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();


	// User code: 
		printf("Sum result is %d\n", res);
	

}


void InitializeAgapiaToCFunctions()
{
ExecutionBlackbox::Get()->AddAgapiaToCFunction("READ", &READ);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("SUM0", &SUM0);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("SUMI", &SUMI);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("SUMRESULT", &SUMRESULT);
}
