#include <stdio.h>
#include "TGAReader.h"
#include <string.h>
#include <assert.h>

	TGAReader::STGA::STGA()
	{
		data = (unsigned char*)0;
		width = 0;
		height = 0;
		byteCount = 0;
		imageSize = 0;
	}

	TGAReader::STGA::~STGA() { delete[] data; data = 0; }
	
	
	void TGAReader::STGA::CreateEmptyImageData(int iSize)
	{
		data = new unsigned char[iSize];
		memset(data, 0, iSize * sizeof(unsigned char));
	}

	bool TGAReader::saveTGAHeader(FILE* file, STGA& tgaFile)
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

	bool TGAReader::saveTGA(const char *filename, STGA& tgaFile)
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

	void TGAReader::CopyImageInfoFrom(STGA& dest, STGA& source)
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

	bool TGAReader::loadTGA(const char *filename, STGA& tgaFile)
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
	