#include "AgapiaToCCode.h"
#include "ExecutionBlackbox.h"
#include "InputTypes.h"

#include "Includes.h"


void COMPUTEJOB(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	BufferDataItem* imagebuffer = ((BufferDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValue();
	int& linestart = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[1])->GetValueRef();
	int& height = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[2])->GetValueRef();
	int& colstart = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[3])->GetValueRef();
	int& width = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[4])->GetValueRef();
	BufferDataItem* imageoutbuffer = ((BufferDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[0])->m_InputItems[0])->GetValue();
	int& outwidth = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[0])->m_InputItems[1])->GetValueRef();
	int& outheight = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[0])->m_InputItems[2])->GetValueRef();
	int& outbpp = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[0])->m_InputItems[3])->GetValueRef();


	// User code: 
		printf("Job received with (ls: %d, h: %d, rs: %d, w: %d)\n", linestart, height, colstart, width);
		struct TaskDesc
		{
			int iLineStart, iHeight;
			int iColStart, iWidth;
		};
		
		TaskDesc task;
		task.iLineStart = linestart;
		task.iHeight = height;
		task.iColStart = colstart;
		task.iWidth = width;
		
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
		
	
		const int pixelBytes = 3;
		const int imageLineBytes = pixelBytes * task.iWidth;
		
		unsigned char *pInputCurrentPos = (unsigned char*) imagebuffer->m_pData;
		unsigned char *pOutputCurrentPos = new unsigned char[imagebuffer->m_iBufferSize];
		imageoutbuffer->m_pData = (char*)pOutputCurrentPos;
		imageoutbuffer->m_iBufferSize = imagebuffer->m_iBufferSize;
		
		memcpy(pOutputCurrentPos, pInputCurrentPos, imageLineBytes);
		pOutputCurrentPos += imageLineBytes;
		pInputCurrentPos += imageLineBytes;
	
		memcpy(pOutputCurrentPos + (imageLineBytes * task.iHeight), pInputCurrentPos + (imageLineBytes * task.iHeight), imageLineBytes);
		
		int beginRow = task.iLineStart; 
		int endRow = beginRow + task.iHeight - 1;
		for (int i = beginRow; i <= endRow; i++)
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
							Gradients[gIdx] += GDxy[gIdx][dxI+1][dxJ+1]*TGAReader::STGA::S_FuncGetGreyValue(pInputCurrentPos - dxI*imageLineBytes - dxJ*pixelBytes);
		
				float fValue = sqrtf(Gradients[0]*Gradients[0] + Gradients[1]*Gradients[1] + 0.0f);			
				if (fValue > kThresholdValue)
					TGAReader::STGA::S_SetGreyValue(pOutputCurrentPos, 255);
				else
					TGAReader::STGA::S_SetGreyValue(pOutputCurrentPos, 0);
			}
		
			memcpy(pOutputCurrentPos, pInputCurrentPos, pixelBytes);
		
			pInputCurrentPos += pixelBytes;
			pOutputCurrentPos += pixelBytes;
		}
		
		outwidth = width;
		outheight = height;
		outbpp = pixelBytes;
	

}




void DISPATCHJOBS(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	int& nrofprocs = ((IntDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& imageaddress = ((IntDataItem*)((SimpleProcessItem*)pWest->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& nrprocs = ((IntDataItem*)((SimpleProcessItem*)pSouth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	VectorProcessItem& tasks = *((VectorProcessItem*)pSouth->m_InputsInBlock[1]);


	// User code: 
		ClearVectorOfProcessItems(&tasks);
		nrprocs = nrofprocs;
		
		TGAReader::STGA*  pTGAFile = (TGAReader::STGA*) imageaddress;
		const int pixelBytes = pTGAFile->byteCount;
		const int imageLineBytes = pixelBytes * pTGAFile->width;
		
		int iLinesPerProc = pTGAFile->height / nrofprocs;
		int iLinesToAddOne = pTGAFile->height % nrofprocs;
		int iLineIndex = 0;
		for (int i = 0; i < nrofprocs; i++)
		{
			int linesToThisProc = iLinesPerProc + (iLinesToAddOne > i ? 1 : 0);
		
			unsigned int iDataSize = imageLineBytes*(linesToThisProc + 2 - 1*(i == 0 || i == (nrofprocs-1)));
			unsigned char* pDataBegin = (unsigned char*) &pTGAFile->data[imageLineBytes * (iLineIndex - 1 * (i > 0))];	
	
			int lineStart = iLineIndex + 1*(i == 0);
			int numRows = linesToThisProc - 1*(i == 0 || i == (nrofprocs-1));	
		
		SetInputItemToVector(45, &tasks, i, "objects", (char*)pDataBegin, iDataSize);
		SetInputItemToVector(46, &tasks, i, "rowstart", lineStart);
		SetInputItemToVector(47, &tasks, i, "numrows", numRows);
		SetInputItemToVector(48, &tasks, i, "colstart", 0);
		SetInputItemToVector(49, &tasks, i, "width", pTGAFile->width);
			
			iLineIndex += linesToThisProc;
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
	char** filenamein = ((StringDataItem*)((SimpleProcessItem*)pNorth->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();
	int& imageaddress = ((IntDataItem*)((SimpleProcessItem*)pEast->m_InputsInBlock[0])->m_InputItems[0])->GetValueRef();


	// User code: 
		TGAReader::STGA* tgaFile = new TGAReader::STGA();
		TGAReader::loadTGA(*filenamein, *tgaFile);
	
		imageaddress = (int) tgaFile;
	

}




void SAVEFILE(InputBlock* pNorth, InputBlock* pWest, InputBlock* pSouth, InputBlock* pEast)
{
	// Local variables declaration: 
	VectorProcessItem& results = *((VectorProcessItem*)pNorth->m_InputsInBlock[0]);


	// User code: 
		int nrprocs = GetNumItemsInVectorProcessItem(results);
	
		int totalHeight = 0;
		int width = 0;
		int bpp = 0;
		for (int i = 0; i < nrprocs; i++)
		{
		SimpleProcessItem* pSBPP = (SimpleProcessItem*)GetVectorItemByIndex(results, i, 0);
			bpp = ((IntDataItem*)pSBPP->GetItem(3))->GetValue();
			width = ((IntDataItem*)pSBPP->GetItem(1))->GetValue();
			totalHeight += ((IntDataItem*)pSBPP->GetItem(2))->GetValue();
		}
		
		TGAReader::STGA outHeader;
		outHeader.width = width;
		outHeader.height = totalHeight;
		outHeader.byteCount = bpp;
		
		FILE *file = fopen("output.tga", "wb");
		TGAReader::saveTGAHeader(file, outHeader);
		
		int iBytesPerLine = outHeader.byteCount * outHeader.width;
		for (int i = 0; i < nrprocs; i++)
		{
		SimpleProcessItem* pResult = (SimpleProcessItem*)GetVectorItemByIndex(results, i, 0);
			BufferDataItem* pDataItem = (BufferDataItem*) pResult->GetItem(0);
			
			int iBeginOffset = iBytesPerLine * (i > 0);
			int iReceivedLines = pDataItem->m_iBufferSize / iBytesPerLine;
			unsigned int iSize = iBytesPerLine * (iReceivedLines - 1 - 1*(i > 0 && i < nrprocs-1));
	
	
			fwrite(pDataItem->m_pData + iBeginOffset, sizeof(pDataItem->m_pData[0]), iSize, file);
		}	
		fclose(file);
	

}


void InitializeAgapiaToCFunctions()
{
ExecutionBlackbox::Get()->AddAgapiaToCFunction("COMPUTEJOB", &COMPUTEJOB);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("DISPATCHJOBS", &DISPATCHJOBS);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("I", &I);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("READ", &READ);
ExecutionBlackbox::Get()->AddAgapiaToCFunction("SAVEFILE", &SAVEFILE);
}
