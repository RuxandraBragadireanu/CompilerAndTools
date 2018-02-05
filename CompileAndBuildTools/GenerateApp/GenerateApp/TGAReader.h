#ifndef TGAREADER_H
#define TGAREADER_H

#include <stdio.h>

class TGAReader
{
public:
	struct STGA
	{
		STGA()
		{
			data = (unsigned char*)0;
			width = 0;
			height = 0;
			byteCount = 0;
			imageSize = 0;
		}

		~STGA() { delete[] data; data = 0; }

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

		void CreateEmptyImageData(int iSize)
		{
			data = new unsigned char[iSize];
			memset(data, 0, iSize * sizeof(unsigned char));
		}
	};

	static bool saveTGAHeader(FILE* file, STGA& tgaFile)
	{
		memset(tgaFile.type, 0, sizeof(tgaFile.type));
		tgaFile.type[1] = 0; tgaFile.type[2] = 2;
		fwrite(&tgaFile.type, sizeof(char), 3, file);
		//fwrite(&tgaFile.type, sizeof(char), 12-3, file);
		fseek(file, 12, SEEK_SET);

		tgaFile.info[0] = tgaFile.width & 0xFF;  tgaFile.info[1] = (tgaFile.width>>8) & 0xFF;
		tgaFile.info[2] = tgaFile.height & 0xFF;  tgaFile.info[3] = (tgaFile.height>>8) & 0xFF;
		tgaFile.info[4] = tgaFile.byteCount * 8;
		fwrite(&tgaFile.info, sizeof(char), 6, file);

		if (tgaFile.byteCount != 3 && tgaFile.byteCount != 4) 
		{
			fclose(file);
			assert(false);
			return false;
		}

		return true;
	}

	static bool saveTGA(const char *filename, STGA& tgaFile)
	{
		FILE *file = fopen(filename, "wb");
		if (!file)
			return false;

		if (!saveTGAHeader(file, tgaFile))
			return false;

		tgaFile.imageSize = /*tgaFile.width * */ tgaFile.height * tgaFile.width * tgaFile.byteCount;
		fwrite(tgaFile.data, sizeof(unsigned char), tgaFile.imageSize, file);

		fclose(file);
		return true;
	}

	static void CopyImageInfoFrom(STGA& dest, STGA& source)
	{
		dest.width = source.width;
		dest.height = source.height;
		dest.byteCount = source.byteCount;

		dest.pGreyGetGreyValueFunc = source.pGreyGetGreyValueFunc;

		dest.imageSize = /*tgaFile.width * */ dest.height * dest.width * dest.byteCount;

		//allocate memory for image data
		dest.data = new unsigned char[dest.imageSize];

		// Header of TGA
		memcpy(dest.type, source.type, sizeof(dest.type));
		memcpy(dest.info, source.info, sizeof(dest.info));
	}

	static bool loadTGA(const char *filename, STGA& tgaFile)
	{
		FILE *file;

		file = fopen(filename, "rb");

		if (!file)
			return false;

		fread (&tgaFile.type, sizeof (char), 3, file);
		fseek (file, 12, SEEK_SET);
		fread (&tgaFile.info, sizeof (char), 6, file);

		//image type either 2 (color) or 3 (greyscale)
		if (tgaFile.type[1] != 0 || (tgaFile.type[2] != 2 && tgaFile.type[2] != 3))
		{
			fclose(file);
			return false;
		}

		tgaFile.width = tgaFile.info[0] + tgaFile.info[1] * 256;
		tgaFile.height = tgaFile.info[2] + tgaFile.info[3] * 256;
		tgaFile.byteCount = tgaFile.info[4] / 8;

		if (tgaFile.byteCount == 3) 
			tgaFile.pGreyGetGreyValueFunc = &TGAReader::STGA::GetGreyValue3Bytes;
		else
			tgaFile.pGreyGetGreyValueFunc = &TGAReader::STGA::GetGreyValue4Bytes;

		if (tgaFile.byteCount != 3 && tgaFile.byteCount != 4) {
			fclose(file);
			return false;
		}

		long imageSize = /*tgaFile.width * */ tgaFile.height * tgaFile.width * tgaFile.byteCount;

		//allocate memory for image data
		tgaFile.data = new unsigned char[imageSize];

		//read in image data
		fread(tgaFile.data, sizeof(unsigned char), imageSize, file);

		//close file
		fclose(file);

		return true;
	}
};

#endif