#include "AgapiaToCCode.h"
#include "ExecutionBlackbox.h"
#include "InputTypes.h"

#include "Includes.h"


void AUXI(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& b = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	VectorProcessItem& numbers = *((VectorProcessItem*)pNorth->m_InputsInBlock[1]);
	int& wordsnumber = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[2])->m_InputItems[0])->GetValueRef();
	int& b2 = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[2])->m_InputItems[1])->GetValueRef();
	int& indseed = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[3])->m_InputItems[0])->GetValueRef();
	int& nrofprocs2 = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[3])->m_InputItems[1])->GetValueRef();
	BufferDataItem* buf1 = ((BufferDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[3])->m_InputItems[2])->GetValue();
	BufferDataItem* buf2 = ((BufferDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[3])->m_InputItems[3])->GetValue();
	VectorProcessItem& numbersout = *((VectorProcessItem*)pSouth->m_InputsInBlock[0]);
	int& wordsnumberout = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[1])->m_InputItems[0])->GetValueRef();
	int& bout = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[1])->m_InputItems[1])->GetValueRef();
	int& indseedout = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[2])->m_InputItems[0])->GetValueRef();
	int& nrofprocsout = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[2])->m_InputItems[1])->GetValueRef();
	BufferDataItem* buf3 = ((BufferDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[2])->m_InputItems[2])->GetValue();
	BufferDataItem* buf4 = ((BufferDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[2])->m_InputItems[3])->GetValue();


	// User code: 
		int nrValues = GetNumItemsInVectorProcessItem(numbers);
		for (int i=0;i<nrValues;++i)
		{
		SimpleProcessItem* pSPI = (SimpleProcessItem*)GetVectorItemByIndex(numbers, i, 0);
			IntDataItem* pIntData = (IntDataItem*)pSPI->GetItem(0);
			IntDataItem* pIntData2 = (IntDataItem*)pSPI->GetItem(1);
			int aux1 = pIntData->GetValue();
			int aux2 = pIntData2->GetValue();
		SetInputItemToVector(42, &numbersout, i, "nr", aux1);
		SetInputItemToVector(43, &numbersout, i, "proc", aux2);
		}
		wordsnumberout = wordsnumber;
		bout = b2;
		indseedout = indseed;
		nrofprocsout = nrofprocs2;
		char *pData = (char*) buf1->m_pData;
		const int iDataSize = buf1->m_iBufferSize;
		buf3->SetValue(pData, iDataSize);
		char *pData2 = (char*) buf2->m_pData;
		const int iDataSize2 = buf2->m_iBufferSize;
		buf4->SetValue(pData2, iDataSize2);
		std::cout << wordsnumberout << " " << bout << " " << indseedout << " " << nrofprocsout << std::endl;
	

}




void CONTINUE(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& wordsnumber = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& b = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[1])->GetValueRef();
	BufferDataItem* buff = ((BufferDataItem*)((SimpleProcessItem*)pWest->m_InputsInBlock[0])->m_InputItems[0])->GetValue();
	int& bol = ((IntDataItem*)((SimpleProcessItem*)pEast->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	BufferDataItem* buf = ((BufferDataItem*)((SimpleProcessItem*)pEast->m_InputsInBlock[0])->m_InputItems[1])->GetValue();
	int& wordsnum = ((IntDataItem*)((SimpleProcessItem*)pEast->m_InputsInBlock[0])->m_InputItems[2])->GetValueRef();
	int& bol2 = ((IntDataItem*)((SimpleProcessItem*)pEast->m_InputsInBlock[0])->m_InputItems[3])->GetValueRef();


	// User code: 
		const int iDataSize = buff->m_iBufferSize;
		std::cout << wordsnumber << std::endl;
		printf("Continue\n");
		if (iDataSize > wordsnumber)
			b = 0;
		bol = b;
		bol2 = b;
		std::cout << bol << " " << bol2 << std::endl;
		wordsnum = wordsnumber;
		char *pData2 = (char*) buff->m_pData;
		std::cout << pData2 << std::endl;
		const int iDataSize2 = wordsnumber * sizeof(int);
		buf->SetValue(pData2, iDataSize2);
	

}




void GENERATESLAVE(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& nr = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& proc = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[1])->GetValueRef();
	BufferDataItem* buf = ((BufferDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[0])->m_InputItems[0])->GetValue();


	// User code: 
		std::vector<int> generatednumbers;
		int seed = nr;
		for (int i = 0; i < proc; ++i)
		{
			int generatednumber = recurence(seed);
			seed = generatednumber;
			generatednumbers.push_back(generatednumber);
		}
	        printf("Slave\n");
		const int size = generatednumbers.size();
		const int iDataSize = size * sizeof(int);
		int* numb = new int[size];
		for (int i=0;i<size;++i)
			numb[i] = generatednumbers[i];
		char* pDataBegin = (char*) numb;
		buf->SetValue(pDataBegin, iDataSize);
	

}




void GETRESULTNUMBERS(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	VectorProcessItem& numbers = *((VectorProcessItem*)pNorth->m_InputsInBlock[0]);
	BufferDataItem* buf = ((BufferDataItem*)((SimpleProcessItem*)pEast->m_InputsInBlock[0])->m_InputItems[0])->GetValue();


	// User code: 
		int nrValues = GetNumItemsInVectorProcessItem(numbers);
		printf("%d\n",nrValues);
		std::vector<int> generatednumbers;
		printf("Gather\n");
		for (int i = 0; i < nrValues; i++)
		{
		SimpleProcessItem* pSPI = (SimpleProcessItem*)GetVectorItemByIndex(numbers, i, 0);
			BufferDataItem* pIntData = (BufferDataItem*)pSPI->GetItem(0);
			char *pData = pIntData->m_pData;
			const int iDataSize = pIntData->m_iBufferSize;
			int* num = (int*) pData;
			for (int i=0;i<(iDataSize/sizeof(int));++i)
				generatednumbers.push_back(num[i]);
		}
		const int size = generatednumbers.size();
		const int iDataSize = size * sizeof(int);
		int* numb = new int[size];
		for (int i=0;i<size;++i)
			numb[i] = generatednumbers[i];
		char* pDataBegin = (char*) numb;
		buf->SetValue(pDataBegin, iDataSize);
		
	

}




void NEXTSTEP(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& indexseed = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& nrofprocs = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[1])->GetValueRef();
	BufferDataItem* buf1 = ((BufferDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[2])->GetValue();
	BufferDataItem* buf2 = ((BufferDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[3])->GetValue();
	int& bol = ((IntDataItem*)((SimpleProcessItem*)pWest->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	BufferDataItem* buf = ((BufferDataItem*)((SimpleProcessItem*)pWest->m_InputsInBlock[0])->m_InputItems[1])->GetValue();
	int& wordsnum = ((IntDataItem*)((SimpleProcessItem*)pWest->m_InputsInBlock[0])->m_InputItems[2])->GetValueRef();
	int& bol2 = ((IntDataItem*)((SimpleProcessItem*)pWest->m_InputsInBlock[0])->m_InputItems[3])->GetValueRef();
	int& b2 = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	VectorProcessItem& numbers = *((VectorProcessItem*)pSouth->m_InputsInBlock[1]);
	int& wordsnumber = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[2])->m_InputItems[0])->GetValueRef();
	int& b = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[2])->m_InputItems[1])->GetValueRef();
	int& indseed = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[3])->m_InputItems[0])->GetValueRef();
	int& nrofprocs2 = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[3])->m_InputItems[1])->GetValueRef();
	BufferDataItem* buf3 = ((BufferDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[3])->m_InputItems[2])->GetValue();
	BufferDataItem* buf4 = ((BufferDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[3])->m_InputItems[3])->GetValue();
	BufferDataItem* buff = ((BufferDataItem*)((SimpleProcessItem*)pEast->m_InputsInBlock[0])->m_InputItems[0])->GetValue();


	// User code: 
		printf("NextStep\n");
		if (bol == 1)
		{
			char *pData = (char*) buf->m_pData;
			const int iDataSize = buf->m_iBufferSize;
			int* generatednumbers = (int*)pData;
			for (int i=0;i < nrofprocs;++i)
			{
				int n = generatednumbers[indexseed+i];
			SetInputItemToVector(117, &numbers, i, "nr", n);
			SetInputItemToVector(118, &numbers, i, "proc", nrofprocs);
			}
			indexseed += 1;
		}
		char *pData2 = (char*) buf->m_pData;
		const int iDataSize2 = buf->m_iBufferSize;
		buff->SetValue(pData2, iDataSize2);
		wordsnumber = wordsnum;
		indseed = indexseed;
		b = bol2;
		nrofprocs2 = nrofprocs;
		b2 = bol2;
		char *pData = (char*) buf1->m_pData;
		const int iDataSize = buf1->m_iBufferSize;
		buf3->SetValue(pData, iDataSize);
		char *pData3 = (char*) buf2->m_pData;
		const int iDataSize3 = buf2->m_iBufferSize;
		buf4->SetValue(pData3, iDataSize3);
	

}




void READANDGENERATE(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& nrofprocs = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	char** filenamein = ((StringDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[1])->GetValueRef();
	char** filenameout = ((StringDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[2])->GetValueRef();
	int& b = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[3])->GetValueRef();
	int& b2 = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	VectorProcessItem& numbers = *((VectorProcessItem*)pSouth->m_InputsInBlock[1]);
	int& wordsnumber = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[2])->m_InputItems[0])->GetValueRef();
	int& bol = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[2])->m_InputItems[1])->GetValueRef();
	int& indexseed = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[3])->m_InputItems[0])->GetValueRef();
	int& nrofprocs2 = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[3])->m_InputItems[1])->GetValueRef();
	BufferDataItem* buf3 = ((BufferDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[3])->m_InputItems[2])->GetValue();
	BufferDataItem* buf4 = ((BufferDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[3])->m_InputItems[3])->GetValue();


	// User code: 
		std::ifstream fin(*filenamein);
		std::string word;
		int index = 0;
		std::vector<inputWord> input;
		while (fin >> word)
		{
			inputWord aux;
			strcpy(aux.word,word.c_str());
			aux.index = index;
			input.push_back(aux);
			index++;
		}
		const int wordsnum = input.size();
		std::vector<int> generatednumbers;
		srand(time(NULL));
		int seed = std::rand() % (int)pow(2,31);
		for (int i = 0; i < nrofprocs; ++i)
		{
			int generatednumber = recurence(seed);
			seed = generatednumber;
			generatednumbers.push_back(generatednumber);
		}
	
		for (int i=0;i<generatednumbers.size();++i)
		{
			int n = generatednumbers[i];
		SetInputItemToVector(213, &numbers, i, "nr", n);
		SetInputItemToVector(214, &numbers, i, "proc", nrofprocs);
		}
		b2 = b;
		wordsnumber = wordsnum;
		bol = b;
		indexseed = nrofprocs;
		nrofprocs2 = nrofprocs;
		const int iDataSize3 = wordsnum * sizeof(inputWord);
		inputWord* w = new inputWord[wordsnum];
		for (int i = 0; i < input.size(); i++)
			w[i] = input[i];
		char* pDataBegin3 = (char*)w;
		buf3->SetValue(pDataBegin3, iDataSize3);
		const int iDataSize4 = strlen(*filenameout) * sizeof(char);
		char* pDataBegin4 = (*filenameout);
		buf4->SetValue(pDataBegin4, iDataSize4);
	

}




void SORTANDPRINT(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& b2 = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	VectorProcessItem& numbers = *((VectorProcessItem*)pNorth->m_InputsInBlock[1]);
	int& wordsnumber = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[2])->m_InputItems[0])->GetValueRef();
	int& b = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[2])->m_InputItems[1])->GetValueRef();
	int& indseed = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[3])->m_InputItems[0])->GetValueRef();
	int& nrofprocs2 = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[3])->m_InputItems[1])->GetValueRef();
	BufferDataItem* buf2 = ((BufferDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[3])->m_InputItems[2])->GetValue();
	BufferDataItem* buf3 = ((BufferDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[3])->m_InputItems[3])->GetValue();
	BufferDataItem* buf = ((BufferDataItem*)((SimpleProcessItem*)pWest->m_InputsInBlock[0])->m_InputItems[0])->GetValue();


	// User code: 
		char *pData = (char*) buf->m_pData;
		const int iDataSize = buf->m_iBufferSize;
		int* generatednumbers = (int*)pData;
		std::vector<int> numbers2;
		for (int i=0;i<(iDataSize/sizeof(int));++i)
			numbers2.push_back(generatednumbers[i]);
		char *pData2 = (char*) buf2->m_pData;
		const int iDataSize2 = buf2->m_iBufferSize;
		inputWord* w = (inputWord*) pData2;
		std::vector<inputWord> input;
		for (int i=0;i<iDataSize2/sizeof(inputWord);++i)
			input.push_back(w[i]);
		for (int i=0;i<input.size();++i)
			input[i].generatedNumber = numbers2[i];
		std::sort(input.begin(),input.end(),comp);
		char *filenameout = (char*)buf3->m_pData;
		std::ofstream f("key.txt");
		std::ofstream fout(filenameout);
	        int sz = input.size() - 1;
		for (int i = 0; i < sz; ++i)
		{
				fout << input[i].word << " ";
				f << encrypt(input[i].index) << " ";
		}
		fout << input[input.size() - 1].word;
		f << encrypt(input[input.size() - 1].index);
	

}




void TEST(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& b = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& bout = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();


	// User code: 
		std::cout << b << std::endl;
		bout = 0;
	
	

}




void TEST2(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& b = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();


	// User code: 
		std::cout << b << std::endl;
	
	

}


void InitializeAgapiaToCFunctions()
{
ExecutionBlackbox::Get()->AddAgapiaToCFunction("AUXI", &AUXI);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("CONTINUE", &CONTINUE);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("GENERATESLAVE", &GENERATESLAVE);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("GETRESULTNUMBERS", &GETRESULTNUMBERS);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("NEXTSTEP", &NEXTSTEP);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("READANDGENERATE", &READANDGENERATE);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("SORTANDPRINT", &SORTANDPRINT);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("TEST", &TEST);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("TEST2", &TEST2);
}
