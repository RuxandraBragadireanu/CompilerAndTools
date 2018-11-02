#include "AgapiaToCCode.h"
#include "ExecutionBlackbox.h"
#include "InputTypes.h"

#include "Includes.h"


void COMPUTEJOB(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& imageaddress = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& outputaddress = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[1])->GetValueRef();
	int& linestart = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[2])->GetValueRef();
	int& height = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[3])->GetValueRef();
	int& rowstart = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[4])->GetValueRef();
	int& width = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[5])->GetValueRef();
	int& succed = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();


	// User code: 
		struct TaskDesc
		{
			int iLineStart, iHeight;
			int iRowStart, iWidth;
		};
	
		TaskDesc task;
		task.iLineStart = linestart;
		task.iHeight = height;
		task.iRowStart = 0;
		task.iWidth = width;
	
		TGAReader::STGA* pInImage = (TGAReader::STGA*) imageaddress;
		TGAReader::STGA* pOutImage = (TGAReader::STGA*) outputaddress;
	
		static float kThresholdValue = 70.0f;
		static int GDxy[2][3][3] ={ { { -1, 0, 1, },
									{ -2, 0 ,2, },
									{ -1, 0, 1, }
									},
	
									{ { -1, -2, -1, },
									{ 0, 0, 0, },
									{ +1, +2, +1, }
									},
							};
	
		int imageLineBytes = pInImage->byteCount * pInImage->width;
		int pixelBytes = pInImage->byteCount;
		unsigned char *pInputCurrentPos = &pInImage->data[imageLineBytes * task.iLineStart];
		unsigned char *pOutputCurrentPos = &pOutImage->data[imageLineBytes * task.iLineStart];
	
		if (task.iLineStart == 0)
		{
			memcpy(pOutputCurrentPos, pInputCurrentPos, imageLineBytes);
			pInputCurrentPos += imageLineBytes;
			pOutputCurrentPos += imageLineBytes;
			task.iLineStart = 1;
		}
		if (task.iLineStart + task.iHeight == pInImage->height)
		{
			unsigned char* pInputLastLine = &pInImage->data[imageLineBytes * (task.iLineStart + task.iHeight - 1)];
			unsigned char* pOutputLastLine = &pOutImage->data[imageLineBytes * (task.iLineStart + task.iHeight - 1)];
			memcpy(pOutputLastLine, pInputLastLine, imageLineBytes);
	
			task.iHeight--;
		}
	
		for (int i = 0; i < task.iHeight; i++)
		{
			memcpy(pOutputCurrentPos, pInputCurrentPos, pixelBytes);
	
			pInputCurrentPos += pixelBytes; 
			pOutputCurrentPos += pixelBytes;
	
			for (int j = 1; j < task.iWidth - 1; j++, pInputCurrentPos += pixelBytes, pOutputCurrentPos += pixelBytes)
			{
				int Gradients[2] = {0};
				for (int gIdx = 0; gIdx < 2; gIdx++)
					for (int dxI = -1; dxI <= 1; dxI++)
						for (int dxJ = -1; dxJ <= 1; dxJ++)
							Gradients[gIdx] += GDxy[gIdx][dxI+1][dxJ+1]*pInImage->FuncGetGreyValue(pInputCurrentPos - dxI*imageLineBytes - dxJ*pixelBytes);
	
				float fValue = sqrtf(Gradients[0]*Gradients[0] + Gradients[1]*Gradients[1] + 0.0f);
				
				
				if (fValue > kThresholdValue)
					pOutImage->SetGreyValue(pOutputCurrentPos, 255);
				else
					pOutImage->SetGreyValue(pOutputCurrentPos, 0);
			}
	
			memcpy(pOutputCurrentPos, pInputCurrentPos, pixelBytes);
	
			pInputCurrentPos += pixelBytes;
			pOutputCurrentPos += pixelBytes;
		}
	

}




void DISPATCHJOBS(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& nrofprocs = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	char** filenameout = ((StringDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[1])->GetValueRef();
	int& imageaddress = ((IntDataItem*)((SimpleProcessItem*)pWest->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& outputaddress = ((IntDataItem*)((SimpleProcessItem*)pWest->m_InputsInBlock[0])->m_InputItems[1])->GetValueRef();
	VectorProcessItem& tasks = *((VectorProcessItem*)pSouth->m_InputsInBlock[0]);
	int& nrprocs = ((IntDataItem*)((SimpleProcessItem*)pEast->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& imgoutputaddress = ((IntDataItem*)((SimpleProcessItem*)pEast->m_InputsInBlock[0])->m_InputItems[1])->GetValueRef();
	char** imgoutfilename = ((StringDataItem*)((SimpleProcessItem*)pEast->m_InputsInBlock[0])->m_InputItems[2])->GetValueRef();


	// User code: 
		ClearVectorOfProcessItems(&tasks);
		nrprocs = nrofprocs;
		imgoutputaddress = outputaddress;
		*imgoutfilename = *filenameout;
	
		TGAReader::STGA*  pTGAFile = (TGAReader::STGA*) imageaddress;
		int iLinesPerProc = pTGAFile->height / nrofprocs;
		int iLinesToAddOne = pTGAFile->height % nrofprocs;
		int iLineIndex = 0;
		for (int i = 0; i < nrofprocs; i++)
		{
			int linesToThisProc = iLinesPerProc + (iLinesToAddOne > i ? 1 : 0);
	
		SetInputItemToVector(41, &tasks, i, "imageaddress", imageaddress);
		SetInputItemToVector(42, &tasks, i, "outputaddress", outputaddress);
		SetInputItemToVector(43, &tasks, i, "linestart", iLineIndex);
		SetInputItemToVector(44, &tasks, i, "height", linesToThisProc);
		SetInputItemToVector(45, &tasks, i, "rowstart", 0);
		SetInputItemToVector(46, &tasks, i, "width", pTGAFile->width);
	
			iLineIndex += linesToThisProc;
		}
	

}




void READ(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	char** filenamein = ((StringDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& imageaddress = ((IntDataItem*)((SimpleProcessItem*)pEast->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& outputaddress = ((IntDataItem*)((SimpleProcessItem*)pEast->m_InputsInBlock[0])->m_InputItems[1])->GetValueRef();


	// User code: 
		TGAReader::STGA* tgaFile = new TGAReader::STGA();
		TGAReader::loadTGA(*filenamein, *tgaFile);
		imageaddress = (int) tgaFile;
	
		TGAReader::STGA* outFile = new TGAReader::STGA();
		outFile->byteCount = tgaFile->byteCount;
		outFile->width = tgaFile->width;
		outFile->height = tgaFile->height;
		outFile->data = new unsigned char[tgaFile->width * tgaFile->height * tgaFile->byteCount];
		outputaddress = (int) outFile;
	

}




void SAVEFILE(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	VectorProcessItem& succedarray = *((VectorProcessItem*)pNorth->m_InputsInBlock[0]);
	int& nrprocs = ((IntDataItem*)((SimpleProcessItem*)pWest->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& imgoutputaddress = ((IntDataItem*)((SimpleProcessItem*)pWest->m_InputsInBlock[0])->m_InputItems[1])->GetValueRef();
	char** imgoutfilename = ((StringDataItem*)((SimpleProcessItem*)pWest->m_InputsInBlock[0])->m_InputItems[2])->GetValueRef();


	// User code: 
		TGAReader::STGA*  pTGAFile = (TGAReader::STGA*) imgoutputaddress;
		TGAReader::saveTGA(*imgoutfilename, *pTGAFile);
	

}


void InitializeAgapiaToCFunctions()
{
ExecutionBlackbox::Get()->AddAgapiaToCFunction("COMPUTEJOB", &COMPUTEJOB);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("DISPATCHJOBS", &DISPATCHJOBS);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("READ", &READ);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("SAVEFILE", &SAVEFILE);
}
