#include "Streams.h"
#include <string.h>
#include <assert.h>

#include "GLOBALDebugParams.h"

namespace Streams
{

void BytesStreamBase::Alloc(unsigned int iSize)
{
	m_BufferStart = new char[iSize];
	m_BufferEnd = m_BufferStart + iSize;
	m_iAllocatedSize = iSize;
	Reset();
}

void BytesStreamBase::Dealloc()
{
	if (m_BufferStart)
	{
		delete []m_BufferStart;
		m_BufferStart = NULL;
		m_BufferEnd = NULL;
	}

	Reset();
}

} // namespace Streams