#pragma once
#ifndef STREAMS_H
#define STREAMS_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

namespace Streams
{
	static int GetStringSizeOnStream(char* str)
	{
		return strlen(str) + 1;
	}

	class BytesStreamBase
	{
	public:
		BytesStreamBase() : m_BufferStart(NULL), m_BufferEnd(NULL), m_iHeaderPos(0), m_iAllocatedSize(0),m_BufferPos(NULL){}
		// Moves the cursor to 0
		inline void Reset()			{ m_iHeaderPos = 0; m_BufferPos = m_BufferStart; }
		inline size_t GetAllocatedSize() { return m_iAllocatedSize; }
		inline char*  GetBufferStart() { return m_BufferStart; }

		// Allocate a specified size internally
		void Alloc(unsigned int iSize);

		// Deallocate the buffer
		void Dealloc();

		char* GetHeaderAddress() { return m_BufferPos; }

		// Used for using an existing buffer
		void SetWorkingBuffer(char* data, int iBuffSize) 
		{ 
			m_BufferStart = data; 
			m_BufferPos = m_BufferStart;
			m_BufferEnd = m_BufferStart + iBuffSize;
			m_iAllocatedSize = iBuffSize;
			m_iHeaderPos = 0;
		}

	//protected:
		// Where is the stream right now
		int m_iHeaderPos;

		// How much is allocated now
		int m_iAllocatedSize; 

		// Internal buffer where this is stored (start)
		char* m_BufferStart;

		// Current header position
		char* m_BufferPos;

		// The end header. Should not get over it ever. It's used for testing streams write
		char* m_BufferEnd;

		// Last buffer pos written
		//#ifdef DEBUG_STREAMS_USED
	};
	class BytesStreamWriter : public BytesStreamBase
	{
	public:
		// Various write types
		template <typename TYPE>
		inline void WriteSimpleType(const TYPE& val)
		{
			#ifdef DEBUG_STREAMS_WRITE
			assert((m_BufferPos + sizeof(val)) <= m_BufferEnd && "Buffer overflow !");
			#endif

			memcpy(m_BufferPos, &val, sizeof(val));
			m_BufferPos += sizeof(val);

		}

		inline void WriteByteArray(const char* data, unsigned int iNumBytes)
		{
			#ifdef DEBUG_STREAMS_WRITE
			assert((m_BufferPos + iNumBytes) <= m_BufferEnd && "Buffer overflow !");
			#endif

			memcpy(m_BufferPos, data, iNumBytes);
			m_BufferPos += iNumBytes;			
		}

		inline void WriteString(const char* data)
		{
			char lastCharWritten = 0;
			do 
			{
				lastCharWritten = *data;
				data++;

#ifdef DEBUG_STREAMS_WRITE
				assert((m_BufferPos + sizeof(char)) <= m_BufferEnd && "Buffer overflow !");
#endif
				*m_BufferPos = lastCharWritten;
				m_BufferPos++;
			}while(lastCharWritten != '\0');
		}

	private:
		
	};

	class BytesStreamReader : public BytesStreamBase
	{
	public:

		// Various read types
		template <typename TYPE>
		inline void ReadSimpleType(TYPE& data)
		{
			#ifdef DEBUG_STREAMS_WRITE
			assert((m_BufferPos + sizeof(data))<= m_BufferEnd && "Buffer overflow !");
			#endif

			memcpy(&data, m_BufferPos, sizeof(data));
			m_BufferPos += sizeof(data);
		}

		inline void ReadByteArray(char* data, unsigned int iNumBytes)
		{
			#ifdef DEBUG_STREAMS_WRITE
			assert((m_BufferPos + iNumBytes)<= m_BufferEnd && "Buffer overflow !");
			#endif

			memcpy(data, m_BufferPos, iNumBytes);
			m_BufferPos += iNumBytes;
		}

		// Returns the number of bytes read
		inline int ReadString(char* allocatedData, int maxChars)
		{
			int numCharsRead = 0;

			char lastCharRead = 0;
			do
			{
				numCharsRead++;

#ifdef DEBUG_STREAMS_WRITE
				assert(numCharsRead <= maxChars && "Too many chars read than was expected");							
				assert((m_BufferPos + sizeof(char)) <= m_BufferEnd && "Buffer overflow !");
#endif
				lastCharRead = *m_BufferPos;
				*allocatedData = lastCharRead;
				m_BufferPos++;
				allocatedData++;
			}
			while(lastCharRead != '\0');

			return numCharsRead;
		}
	};

};

#endif