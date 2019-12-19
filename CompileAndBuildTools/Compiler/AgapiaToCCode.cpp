#include "AgapiaToCCode.h"
#include "ExecutionBlackbox.h"
#include "InputTypes.h"

#include "Includes.h"


void COMPUTEALLJOBS(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& nrprocs = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	VectorProcessItem& tasks = *((VectorProcessItem*)pNorth->m_InputsInBlock[1]);


	// User code: 
		
		printf("COMPUTEALLJOBS module called.\n");
		
	
	

}




void COMPUTEFINALRESULT(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	VectorProcessItem& results = *((VectorProcessItem*)pNorth->m_InputsInBlock[0]);


	// User code: 
		
		printf("COMPUTEFINALRESULT module called.\n");
		
		int nrprocs = GetNumItemsInVectorProcessItem(results);
		
		std::vector<int> primes;
		
		for (int i = 0; i < nrprocs; ++i) {
					
		SimpleProcessItem* pResult = (SimpleProcessItem*)GetVectorItemByIndex(results, i, 0);
			BufferDataItem* pDataItem = (BufferDataItem*) pResult->GetItem(0);
			
			char *pData = (char*) pDataItem->m_pData;
			const int iDataSize = pDataItem->m_iBufferSize;
			
			int* aux = (int*) pData;
			int nrElem = iDataSize / sizeof(int);
			
			for (int i = 0; i < nrElem; ++i) {
				
				primes.push_back(aux[i]);
			}
		}
	
		
		std::ofstream fout("result.out");
	
		for (int x: primes) {
			printf("%d, ", x);
			fout << x << ", ";
		}	
	
		fout.close();
	
	

}




void COMPUTEJOB(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	BufferDataItem* primesbuf = ((BufferDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValue();
	int& nrproc = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[1])->GetValueRef();
	int& n = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[2])->GetValueRef();
	int& rank = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[3])->GetValueRef();
	BufferDataItem* buf = ((BufferDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[0])->m_InputItems[0])->GetValue();


	// User code: 
		
		printf("COMPUTEJOB_%d module called.\n", rank);
	
		int sqn = std::floor(sqrt(n)) + 1;
		n = (n - sqn + 1);	
		int t = (n / nrproc) + (rank <= n % nrproc);
		
		int start = sqn + (n / nrproc) * (rank - 1) + std::min(rank - 1, n % nrproc);
		
		char *pData = (char*) primesbuf->m_pData;
		const int iDataSize = primesbuf->m_iBufferSize;
			
		int* primes = (int*) pData;
		int nrElem = iDataSize / sizeof(int);
		
		std::vector<int> more_primes;
		
		if (rank == 1) {
			
			for (int i = 0; i < nrElem; ++i) {
				more_primes.push_back(primes[i]);
			}
		}
	
		for (int i = start, cnt = 0; cnt < t; ++i, ++cnt) {
	
			bool is_prime = true;		
			for (int x = 0; x < nrElem; ++x) {
				
				if (i % primes[x] == 0) {
					
					is_prime = false;
					break;			
				}
			}
	
			if (is_prime) {
				
				more_primes.push_back(i);		
			}
		}
		
		int* more_primes_arr = new int[more_primes.size()];
		std::copy(more_primes.begin(), more_primes.end(), more_primes_arr);
		
		const int jDataSize = more_primes.size() * sizeof(int);
		char* pDataBegin = (char*) more_primes_arr;
		buf->SetValue(pDataBegin, jDataSize);
	
	
	

}




void COMPUTESIEVE(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& nrofprocs = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& n = ((IntDataItem*)((SimpleProcessItem*)pWest->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& nrprocs = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	VectorProcessItem& tasks = *((VectorProcessItem*)pSouth->m_InputsInBlock[1]);


	// User code: 
		
		printf("COMPUTESIEVE module called.\n");
		
		ClearVectorOfProcessItems(&tasks);
		int sqn = std::floor(sqrt(n)) + 1;
	
		std::vector<bool> mark(n+1);
		std::vector<int> lprimes;
	
		lprimes.push_back(2);
	
		for (int i = 3; i <= sqn; i += 2) {
			
			if (!mark[i]) {
				
				lprimes.push_back(i);
				for (int j = i + i; j <= sqn; j += i) {
					
					mark[j] = true;			
				}
			}
		}
		
		int* lprimes_arr = new int[lprimes.size()];
		std::copy(lprimes.begin(), lprimes.end(), lprimes_arr);
		
		const int iDataSize = lprimes.size() * sizeof(int);
		char* pDataBegin = (char*) lprimes_arr;
		
		nrprocs = nrofprocs;
		for (int i = 0; i < nrofprocs; ++i) {
			
			
		SetInputItemToVector(58, &tasks, i, "primes", (char*)pDataBegin, iDataSize);
		SetInputItemToVector(59, &tasks, i, "nrproc", nrofprocs);
		SetInputItemToVector(60, &tasks, i, "n", n);
		SetInputItemToVector(61, &tasks, i, "rank", i);
		}
	
	

}




void I(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& n = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();


	// User code: 
	
	

}




void READ(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& n = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& nn = ((IntDataItem*)((SimpleProcessItem*)pEast->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();


	// User code: 
		
		printf("READ module called.\n");
	
		nn = n;
		
	

}


void InitializeAgapiaToCFunctions()
{
ExecutionBlackbox::Get()->AddAgapiaToCFunction("COMPUTEALLJOBS", &COMPUTEALLJOBS);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("COMPUTEFINALRESULT", &COMPUTEFINALRESULT);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("COMPUTEJOB", &COMPUTEJOB);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("COMPUTESIEVE", &COMPUTESIEVE);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("I", &I);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("READ", &READ);
}
