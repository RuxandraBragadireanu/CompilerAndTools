#ifndef BUFFERED_DATA_TYPES
#define BUFFERED_DATA_TYPES

#include <vector>

class BaseProcessInput;

// This is used as a item buffered in a buffered block
class BufferedDataItem
{
public:
	BufferedDataItem() : index(-1), mBufferedData(NULL){}
	// This is where the input should go in the destination after buffering.
	// Could be an index or a pointer
	int index;

	// This is a pointer to the data buffered
	BaseProcessInput* mBufferedData;
};

typedef std::vector<BufferedDataItem>	ArrayOfBufferedItems;
typedef ArrayOfBufferedItems::iterator  ArrayOfBufferedItemsIter;


// Contains an actual copy of an input block
// Inside this, the items different than NULL are considered to be buffered
class BufferedInputBlock
{
public:
	// You must know the number of items in advance before instancing this type
	BufferedInputBlock(int nrItems)
	{
		m_bufferedItems.reserve(nrItems);
		for (int i = 0; i < nrItems; i++)
		{
			BufferedDataItem bd;
			bd.index = i;
			m_bufferedItems.push_back(bd);
		}
	}

	ArrayOfBufferedItems	m_bufferedItems;
};

#endif
