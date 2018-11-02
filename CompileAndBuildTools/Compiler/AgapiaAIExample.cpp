#include "AgapiaAIExample.h"
#include <stdlib.h>

void ReadMapInput(char *szInputFile, char*& szMapObjects, int& numObjects, int& height, int& width)
{
	//	Width|Height|NrObjects|Object1...N
	//	Where Objecti = row|column|rowend|columnend
	FILE *fInput = fopen(szInputFile, "rb");
	fread(&width, sizeof(int), 1, fInput);
	fread(&height, sizeof(int), 1, fInput);

	fread(&numObjects, sizeof(int), 1, fInput);

	szMapObjects = new char[numObjects * sizeof(ObjectDescription)];
	ObjectDescription* pObjects = (ObjectDescription*) szMapObjects;
	for (int i = 0; i < numObjects; i++, pObjects++)
	{
		ObjectDescription	obj;
		fread(&obj.row, sizeof(int), 1, fInput);
		fread(&obj.column, sizeof(int), 1, fInput);
		fread(&obj.rowend, sizeof(int), 1, fInput);
		fread(&obj.colend, sizeof(int), 1, fInput);

		memcpy(pObjects, &obj, sizeof(ObjectDescription));
	}

	fclose(fInput);
}

void ExecuteZone(void* pObjects, int iNumObjects, int mapHeight, int mapWidth, int row, int column, int height, int width, int& bestCoverValue, int& bestRow, int& bestColumn)
{
	ObjectDescription* objectsList = (ObjectDescription*) pObjects;
	bestCoverValue = bestRow = bestColumn = -1;
	for (int y = row; y < row + height; y++)
		for (int x = column; x < column + width; x++)
		{
			int iCovers = 0;
			for (int i = 0 ; i < mapWidth; i++)
			{
				for (int j = 0; j < mapHeight; j++)
				{
					if (i == x && j == y)	continue;
					bool bGoodPoint = true;		
					int indexObj = 0;
					for (ObjectDescription* pObjIter = objectsList; indexObj < iNumObjects; pObjIter++, indexObj++)
					{
						if (pObjIter->IntersectsWithSegment(x, y, j, i))
						{
							bGoodPoint = false;
							break;
						}
					}					

					if (bGoodPoint)
						iCovers++;
				}
			}

			if (iCovers > bestCoverValue)
			{
				bestCoverValue = iCovers;
				bestRow = y; bestColumn = x;
			}
		}
}

bool ObjectDescription::SegmentsIntersect(int xP0, int yP0, int xP1, int yP1, int xR0, int yR0, int xR1, int yR1)
{	
	int diffX_P = xP1 - xP0;
	int diffY_P = yP1 - yP0;

	int diffX_R = xR1 - xR0;
	int diffY_R = yR1 - yR1;

	int diffX0 = xP0 - xR0;
	int diffY0 = yP0 - yR0;

	int d = (diffY_P*diffX_R - diffY_R*diffX_P);
	if (d == 0)
		return false;

	float t = (diffY_R*diffX0 - diffX_R*diffY0) / float(d);
	float p = (diffY_P*diffX0 - diffX_P*diffY0) / float(d);

	return ( 0.0f <= t && t <= 1.0f && 0.0f <= p && p <= 1.0f);
}

bool ObjectDescription::IntersectsWithSegment(int x0, int y0, int x1, int y1)
{

	int rectangleSegments[4][4] = { { column, row, colend, row-1  },		// Up 
									{ colend, row, colend, rowend },		// Right
									{ column, rowend, colend, rowend },		// Down
									{ column, row, column, rowend },		// Left
								};

	for (int i = 0; i < 4; i++)
		if (SegmentsIntersect(x0, y0, x1, y1, rectangleSegments[i][0], rectangleSegments[i][1], rectangleSegments[i][2], rectangleSegments[i][3]))
			return true;

	return false;
}

int MapProblem::GetCoverValueOf(int x, int y)
{
	int iCovers = 0;
	for (int i = 0 ; i < height; i++)
	{
		for (int j = 0; j < width; j++)
			if (i != x && j != y)
			{
				bool bGoodPoint = true;
				for (ListOfObjectsIter it = m_ListOfAllObjects.begin(); it != m_ListOfAllObjects.end(); it++)
					if (it->IntersectsWithSegment(x, y, j, i))
					{
						bGoodPoint = false;
						break;
					}

				if (bGoodPoint)
					iCovers++;
			}
	}

	return iCovers;
}

void MapProblem::ExecuteZone(int row, int column, int height, int width, int& bestCoverValue, int& bestRow, int& bestColumn)
{
	bestCoverValue = bestRow = bestColumn = -1;
	for (int y = row; y < row + height; y++)
		for (int x = column; x < column + width; x++)
		{
			int iCoverValue = GetCoverValueOf(x, y);
			if (iCoverValue > bestCoverValue)
			{
				bestCoverValue = iCoverValue;
				bestRow = y; bestColumn = x;
			}
		}
}

void MapProblem::Read(char *filename)
{
	//	Width|Height|NrObjects|Object1...N
	//	Where Objecti = row|column|rowend|columnend
	m_ListOfAllObjects.clear();

	FILE *fInput = fopen(filename, "rb");
	fread(&width, sizeof(int), 1, fInput);
	fread(&height, sizeof(int), 1, fInput);

	int nrObjects = 0;
	fread(&nrObjects, sizeof(int), 1, fInput);
	for (int i = 0; i < nrObjects; i++)
	{
		ObjectDescription	obj;
		fread(&obj.row, sizeof(int), 1, fInput);
		fread(&obj.column, sizeof(int), 1, fInput);
		fread(&obj.rowend, sizeof(int), 1, fInput);
		fread(&obj.colend, sizeof(int), 1, fInput);
		
		m_ListOfAllObjects.push_back(obj);
	}
	
	fclose(fInput);
}

void MapProblem::GenerateRandomFileInput(char *filename, int numObjects, int width, int height)
{
	FILE* fOutput = fopen(filename, "wb");
	fwrite(&width, sizeof(int), 1, fOutput);
	fwrite(&height, sizeof(int), 1, fOutput);
	fwrite(&numObjects, sizeof(int), 1, fOutput);

	for (int i = 0; i < numObjects; i++)
	{
		int objwidth = max(2, rand() % (width / 10));
		int objheight = max(2, rand() % (height / 10));
		int row = rand() % height;
		int column = rand() % width;

		fwrite(&row, sizeof(int), 1, fOutput);
		fwrite(&column, sizeof(int), 1, fOutput);
		fwrite(&objheight, sizeof(int), 1, fOutput);
		fwrite(&objwidth, sizeof(int), 1, fOutput);
	}

	fclose(fOutput);
}
