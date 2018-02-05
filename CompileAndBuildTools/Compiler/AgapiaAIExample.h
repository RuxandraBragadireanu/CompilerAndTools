#ifndef AGAPIA_AI_EXAMPLE_H
#define AGAPIA_AI_EXAMPLE_H

#include <list>
#define max(a,b) (a > b ? a : b)

// Describes a rectangular object 
class ObjectDescription	
{
public:
	int row, column;	// Row and column of the left corner object	
	int rowend, colend;

	// Computes a segment to segment intersection (P and R)
	bool SegmentsIntersect(int xP0, int yP0, int xP1, int yP1, int xR0, int yR0, int xR1, int yR1);

	// Check if this object intersects with the segment specified by S ={(x0,y0), (x1,y1)}
	bool IntersectsWithSegment(int x0, int y0, int x1, int y1);
};

typedef std::list<ObjectDescription>	ListOfObjects;
typedef ListOfObjects::iterator			ListOfObjectsIter;

class MapProblem
{
public:
	// Map dimensions
	int width, height;
	
	// Map of objects
	ListOfObjects	m_ListOfAllObjects;

	static void GenerateRandomFileInput(char *filename, int numObjects, int width, int height);

	// Read the input
	void Read(char* szInputFile);

	// Execute a 2D zone
	void ExecuteZone(int row, int column, int height, int width, int& bestCoverValue, int& bestRow, int& bestColumn);

private:
	// Get the cover value of the (row, column) cell
	int GetCoverValueOf(int row, int column);

};

void ReadMapInput(char *szInputFile, char*& szMapObjects, int& numObjects, int& height, int& width);
void ExecuteZone(void* pObjects, int iNumObjects, int mapHeight, int mapWidth, int row, int column, int height, int width, int& bestCoverValue, int& bestRow, int& bestColumn);


#endif