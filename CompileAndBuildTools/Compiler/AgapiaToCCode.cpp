#include "AgapiaToCCode.h"
#include "ExecutionBlackbox.h"
#include "InputTypes.h"

#include "Includes.h"


void CONSUMEBUFFER(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	BufferDataItem* buf = ((BufferDataItem*)((SimpleProcessItem*)pWest->m_InputsInBlock[0])->m_InputItems[0])->GetValue();


	// User code: 
		char *pData = (char*) buf->m_pData;
		const int iDataSize = buf->m_iBufferSize;
	
		double* numbers = (double*) pData;
		for (int i = 0; i < 10; i++)
			printf("%d ", (int)numbers[i]);
	
	

}




void FILLBUFFER(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	BufferDataItem* buf = ((BufferDataItem*)((SimpleProcessItem*)pEast->m_InputsInBlock[0])->m_InputItems[0])->GetValue();


	// User code: 
		const int iDataSize = 10 * sizeof(double);
		double* numbers = new double[10];
		for (int i = 0; i < 10; i++)
			numbers[i] = (double)i;
		char* pDataBegin = (char*) numbers;
	
		buf->SetValue(pDataBegin, iDataSize);
	

}


void InitializeAgapiaToCFunctions()
{
ExecutionBlackbox::Get()->AddAgapiaToCFunction("CONSUMEBUFFER", &CONSUMEBUFFER);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("FILLBUFFER", &FILLBUFFER);
}
