#include "AgapiaToCCode.h"
#include "ExecutionBlackbox.h"
#include "InputTypes.h"

#include "Includes.h"


void PRIMETEST(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& n = ((IntDataItem*)((SimpleProcessItem*)pWest->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();


	// User code: 
		int nrIters = (int)sqrtf((float)n);
		bool prime = true;
		for (int i = 2; i <= nrIters; i++)
			if (n % i == 0)
			{
				prime = false;
				break;
			}
		printf("Is prime= %d\n", (prime == true ? 1 : 0));
	

}




void READ(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& n = ((IntDataItem*)((SimpleProcessItem*)pEast->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();


	// User code: 
		scanf("%d", &n);
	

}


void InitializeAgapiaToCFunctions()
{
ExecutionBlackbox::Get()->AddAgapiaToCFunction("PRIMETEST", &PRIMETEST);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("READ", &READ);
}
