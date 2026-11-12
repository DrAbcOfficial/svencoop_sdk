#ifndef CMEMORY_H
#define CMEMORY_H

#include <stddef.h>

template<class SizeType>
class CMemory
{
public:
	CMemory()
	{
		m_pData = NULL;
		m_uiSize = 0;
		m_bOwnsData = false;
	}

	explicit CMemory(SizeType size)
	{
		m_pData = size ? new char[size] : NULL;
		m_uiSize = size;
		m_bOwnsData = true;
	}

	CMemory(const char *data, SizeType size)
	{
		m_pData = NULL;
		m_uiSize = 0;
		m_bOwnsData = false;

		if (data)
		{
			m_pData = const_cast<char *>(data);
			m_uiSize = size;
		}
	}

	CMemory(char *data, SizeType size, bool ownsData)
	{
		m_pData = data;
		m_uiSize = data ? size : 0;
		m_bOwnsData = data && ownsData;
	}

	~CMemory()
	{
		if (m_bOwnsData)
			delete[] m_pData;
	}

	void Swap(CMemory &memory)
	{
		if (this == &memory)
			return;

		char *data = m_pData;
		SizeType size = m_uiSize;
		bool ownsData = m_bOwnsData;

		m_pData = memory.m_pData;
		m_uiSize = memory.m_uiSize;
		m_bOwnsData = memory.m_bOwnsData;
		memory.m_pData = data;
		memory.m_uiSize = size;
		memory.m_bOwnsData = ownsData;
	}

	char *Data(void) const { return m_pData; }
	SizeType Size(void) const { return m_uiSize; }

private:
	char		*m_pData;
	SizeType	m_uiSize;
	bool		m_bOwnsData;
};

#endif
