#include "TGAReader.h"
#include <iostream>
#include "Timer.h"
#include <thread>

using namespace std;

#define NUM_TASKS	1

TGAReader::STGA* inFile = NULL;
TGAReader::STGA* outFile = NULL;

struct Task
{
	int				linestart;
	int				height;

	Task()
		: linestart(0)
		, height(0) {}
};

Task tasks[NUM_TASKS];

void Read(char *filenamein)
{
	inFile = new TGAReader::STGA();
	TGAReader::loadTGA(filenamein, *inFile);
	//printf("File received to read from: %s\n", *filenamein);

	// Create and initialize the output image
	outFile = new TGAReader::STGA();
	outFile->byteCount = inFile->byteCount;
	outFile->width = inFile->width;
	outFile->height = inFile->height;
	outFile->data = new unsigned char[inFile->width * inFile->height * inFile->byteCount];
}

void CreateTasks()
{
	const int numTasks = NUM_TASKS;

	int iLinesPerProc	= inFile->height / numTasks;
	int iLinesToAddOne	= inFile->height % numTasks;
	int iLineIndex		= 0;
	for (int i = 0; i < numTasks; i++)
	{
		int linesToThisProc = iLinesPerProc + (iLinesToAddOne > i ? 1 : 0);

		// Create the simple process item with data for this process
		tasks[i].linestart	=	iLineIndex;
		tasks[i].height		=	linesToThisProc;
		
		iLineIndex += linesToThisProc;
	}
}

void SolveTasks()
{
	static float kThresholdValue = 70.0f;
	// GDxy[0] - first gradient, GDxy[1] second one
	static int GDxy[2][3][3] ={ { { -1, 0, 1, },
								{ -2, 0 ,2, },
								{ -1, 0, 1, }
								},

								{ { -1, -2, -1,},
								{ 0, 0, 0, },
								{ +1, +2, +1, }
								},
	};


	for (int i = 0; i < NUM_TASKS; i++)
	{
		int imageLineBytes = inFile->byteCount * inFile->width;
		int pixelBytes = inFile->byteCount;
		unsigned char *pInputCurrentPos = &inFile->data[imageLineBytes * tasks[i].linestart];
		unsigned char *pOutputCurrentPos = &outFile->data[imageLineBytes * tasks[i].linestart];

		// Don't notify the first and last row, and first and last column
		// Maybe it was better to do this at tasks distribution......
		if (tasks[i].linestart == 0)
		{
			// Copy first row and advance the pointers
			memcpy(pOutputCurrentPos, pInputCurrentPos, imageLineBytes);
			pInputCurrentPos += imageLineBytes;
			pOutputCurrentPos += imageLineBytes;
		}

		if (tasks[i].linestart + tasks[i].height == inFile->height)
		{
			// Copy last row
			unsigned char* pInputLastLine = &inFile->data[imageLineBytes * (tasks[i].linestart + tasks[i].height - 1)];
			unsigned char* pOutputLastLine = &outFile->data[imageLineBytes * (tasks[i].linestart + tasks[i].height - 1)];
			memcpy(pOutputLastLine, pInputLastLine, imageLineBytes);
		}

		Timer tm;
		tm.StartCounter();
		for (int k = 1; k < tasks[i].height-1; k++)
		{
			// Don't do nothing with the first column pixel
			//VerifyOutFileWrite(pOutputCurrentPos, pixelBytes);
			memcpy(pOutputCurrentPos, pInputCurrentPos, pixelBytes);

			pInputCurrentPos += pixelBytes; 
			pOutputCurrentPos += pixelBytes;

			for (int j = 1; j < inFile->width - 1; j++, pInputCurrentPos += pixelBytes, pOutputCurrentPos += pixelBytes)
			{
				// Compute the gradients 
				int Gradients[2] = {0};
				for (int gIdx = 0; gIdx < 2; gIdx++)
					for (int dxI = -1; dxI <= 1; dxI++)
						for (int dxJ = -1; dxJ <= 1; dxJ++)
							Gradients[gIdx] += GDxy[gIdx][dxI+1][dxJ+1]*inFile->FuncGetGreyValue(pInputCurrentPos - dxI*imageLineBytes - dxJ*pixelBytes);

				float fValue = sqrtf(Gradients[0]*Gradients[0] + Gradients[1]*Gradients[1] + 0.0f);

				//VerifyOutFileWrite(pOutputCurrentPos, pixelBytes);

				if (fValue > kThresholdValue)
					outFile->SetGreyValue(pOutputCurrentPos, 255);
				else
					outFile->SetGreyValue(pOutputCurrentPos, 0);
			}

			// Don't do nothing with the last column pixel
			//VerifyOutFileWrite(pOutputCurrentPos, pixelBytes);
			memcpy(pOutputCurrentPos, pInputCurrentPos, pixelBytes);

			pInputCurrentPos += pixelBytes;
			pOutputCurrentPos += pixelBytes;
		}
		cout<<tm.GetCounter()<<endl;
	}

	TGAReader::CopyImageInfoFrom(*outFile, *inFile);
	TGAReader::saveTGA("output.tga", *outFile);
}

int main(int argc, char* argv[])
{
	if (argc <= 1)
	{
		cout<<"Incorrect input format: " << " first parameter must be the name of the file. i.e: SobelIter images.tga" << endl;
		return -1;
	}

	Timer timer;
	timer.StartCounter();

	Read(argv[1]);
	//cout<<" Read time: " << timer.GetCounter() <<endl;
	CreateTasks();
	//cout<<" Create time: " << timer.GetCounter()<<endl;
	SolveTasks();
	//cout<<" Solve tasks: " << timer.GetCounter()<<endl;

	cout<<" Total time to execute: "<< 	timer.GetCounter() << endl;
	timer.Reset();
	return 0;
}

