#ifndef TGAREADER_H
#define TGAREADER_H

#include <stdio.h>

class TGAReader
{
public:
	struct STGA
	{
		STGA();
		~STGA();

		typedef unsigned char(TGAReader::STGA::*GetGreyValue)(unsigned char*);
		void destroy() { delete[] data; data = 0; }

		int width;
		int height;
		unsigned char byteCount;
		unsigned char* data;
		int imageSize;
		
		// Header of TGA
		unsigned char type[4];
		unsigned char info[6];

		GetGreyValue pGreyGetGreyValueFunc;

		inline unsigned char FuncGetGreyValue(unsigned char* pBegin)
		{
			return (this->*pGreyGetGreyValueFunc)(pBegin);
		}

		inline unsigned char static S_FuncGetGreyValue(unsigned char* pBegin)
		{
			return ((*pBegin) + (*(pBegin + 1)) + (*(pBegin + 2))) / 3;
		}

		inline unsigned char GetGreyValue3Bytes(unsigned char *pBegin)
		{
			return ((*pBegin) + (*(pBegin + 1)) + (*(pBegin + 2))) / 3;

		}

		inline unsigned char GetGreyValue4Bytes(unsigned char *pBegin)
		{
			return ((*pBegin) + (*(pBegin + 1)) + (*(pBegin + 2))) / 3;
		}

		inline void SetGreyValue(unsigned char* pBegin, int val)
		{
			*pBegin = val;
			*(pBegin + 1) = val;
			*(pBegin + 2) = val;
		}

		inline static void S_SetGreyValue(unsigned char* pBegin, int val)
		{
			*pBegin = val;
			*(pBegin + 1) = val;
			*(pBegin + 2) = val;
		}

		inline void SetPixelColor3(unsigned char* pBegin, unsigned char red, unsigned char green, unsigned char blue)
		{
			*pBegin = red;
			*(pBegin + 1) = green;
			*(pBegin + 2) = blue;
		}

		void CreateEmptyImageData(int iSize);
	};

	static bool saveTGAHeader(FILE* file, STGA& tgaFile);
	static bool saveTGA(const char *filename, STGA& tgaFile);
	static void CopyImageInfoFrom(STGA& dest, STGA& source);
	static bool loadTGA(const char *filename, STGA& tgaFile);
};

#endif