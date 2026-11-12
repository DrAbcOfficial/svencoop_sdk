#include "scripting/core/CASBLOB.h"
#include <sstream>
#include <string.h>

CASBLOB::CASBLOB(unsigned int size, bool canResize)
	: m_pBuffer(NULL), m_uiSize(0), m_uiReadOffset(0), m_uiWriteOffset(0), m_bCanResize(true)
{
	Resize(size);
	m_bCanResize = canResize;
}

CASBLOB::CASBLOB(const CASBLOB &other)
	: m_pBuffer(NULL), m_uiSize(0), m_uiReadOffset(0), m_uiWriteOffset(0), m_bCanResize(true)
{
	Copy(other);
}

CASBLOB::~CASBLOB()
{
	Clear();
}

CASBLOB &CASBLOB::operator=(const CASBLOB &other)
{
	Copy(other);
	return *this;
}

void CASBLOB::Release(void) const
{
	if (InternalRelease())
		delete this;
}

void CASBLOB::Copy(const CASBLOB &other)
{
	if (this == &other)
		return;

	Clear();

	if (other.m_pBuffer)
		Write(other.m_pBuffer, other.m_uiWriteOffset);

	m_bCanResize = other.m_bCanResize;
}

bool CASBLOB::HasAllocatedBuffer(void) const
{
	return m_pBuffer != NULL;
}

unsigned int CASBLOB::GetSizeInBytes(void) const
{
	return m_uiSize;
}

unsigned int CASBLOB::GetReadOffset(void) const
{
	return m_uiReadOffset;
}

bool CASBLOB::ReadReachedEnd(void) const
{
	return m_uiReadOffset >= m_uiWriteOffset;
}

bool CASBLOB::ReadSizeValid(unsigned int size) const
{
	return size + m_uiReadOffset <= m_uiWriteOffset;
}

unsigned int CASBLOB::GetWriteOffset(void) const
{
	return m_uiWriteOffset;
}

bool CASBLOB::CanWriteSize(unsigned int size) const
{
	return size + m_uiWriteOffset <= m_uiSize || m_bCanResize;
}

unsigned int CASBLOB::GetWriteBytesLeft(void) const
{
	return m_uiSize - m_uiWriteOffset;
}

bool CASBLOB::CanResize(void) const
{
	return m_bCanResize;
}

void CASBLOB::Resize(unsigned int size)
{
	if (size == m_uiSize)
		return;

	unsigned char *buffer = NULL;

	if (size)
	{
		buffer = new unsigned char[size];

		if (m_pBuffer)
			memmove(buffer, m_pBuffer, size < m_uiWriteOffset ? size : m_uiWriteOffset);

		if (m_uiWriteOffset < size)
			memset(buffer + m_uiWriteOffset, 0, size - m_uiWriteOffset);
	}

	delete[] m_pBuffer;
	m_pBuffer = buffer;
	m_uiSize = size;

	if (m_uiWriteOffset > size)
		m_uiWriteOffset = size;

	if (size > m_uiReadOffset)
		m_uiReadOffset = size;
}

void CASBLOB::Reserve(unsigned int size)
{
	if (m_uiSize < size)
		Resize(size);
}

void CASBLOB::ShrinkToFit(void)
{
	if (m_uiWriteOffset > m_uiSize)
		Resize(m_uiWriteOffset);
}

void CASBLOB::Clear(void)
{
	if (!m_pBuffer)
		return;

	delete[] m_pBuffer;
	m_pBuffer = NULL;
	m_uiSize = 0;
	m_uiWriteOffset = 0;
	m_uiReadOffset = 0;
	m_bCanResize = true;
}

bool CASBLOB::ReadData(void *data, unsigned int size)
{
	if (!data || !size || m_uiReadOffset >= m_uiWriteOffset ||
		m_uiReadOffset + size > m_uiWriteOffset)
		return false;

	memmove(data, m_pBuffer + m_uiReadOffset, size);
	m_uiReadOffset += size;
	return true;
}

signed char CASBLOB::ReadInt8(void)
{
	bool success;

	return ReadInt8(success);
}

signed char CASBLOB::ReadInt8(bool &success)
{
	signed char value = 0;

	success = ReadData(&value, sizeof(value));
	return value;
}

short CASBLOB::ReadInt16(void)
{
	bool success;

	return ReadInt16(success);
}

short CASBLOB::ReadInt16(bool &success)
{
	short value = 0;

	success = ReadData(&value, sizeof(value));
	return value;
}

int CASBLOB::ReadInt32(void)
{
	bool success;

	return ReadInt32(success);
}

int CASBLOB::ReadInt32(bool &success)
{
	int value = 0;

	success = ReadData(&value, sizeof(value));
	return value;
}

long long CASBLOB::ReadInt64(void)
{
	bool success;

	return ReadInt64(success);
}

long long CASBLOB::ReadInt64(bool &success)
{
	long long value = 0;

	success = ReadData(&value, sizeof(value));
	return value;
}

float CASBLOB::ReadFloat(void)
{
	bool success;

	return ReadFloat(success);
}

float CASBLOB::ReadFloat(bool &success)
{
	float value = 0;

	success = ReadData(&value, sizeof(value));
	return value;
}

double CASBLOB::ReadDouble(void)
{
	bool success;

	return ReadDouble(success);
}

double CASBLOB::ReadDouble(bool &success)
{
	double value = 0;

	success = ReadData(&value, sizeof(value));
	return value;
}

CString CASBLOB::ReadString(unsigned int size, bool &success)
{
	if (!size)
	{
		success = true;
		return CString("");
	}

	if (!ReadSizeValid(size))
	{
		success = false;
		return CString("");
	}

	char *buffer = new char[size];

	memset(buffer, 0, size);
	success = ReadData(buffer, size);
	return CString(buffer, true);
}

CString CASBLOB::ReadString(unsigned int size)
{
	bool success;

	return ReadString(size, success);
}

CString CASBLOB::ReadString(bool &success)
{
	if (ReadReachedEnd())
	{
		success = false;
		return CString("");
	}

	std::ostringstream stream;
	bool reading = true;

	while (m_uiReadOffset < m_uiWriteOffset && reading)
	{
		unsigned char value = m_pBuffer[m_uiReadOffset++];

		if (value)
			stream.put(value);
		else
			reading = false;
	}

	success = true;
	return CString(stream.str().c_str());
}

CString CASBLOB::ReadString(void)
{
	bool success;

	return ReadString(success);
}

bool CASBLOB::Write(const void *data, unsigned int size)
{
	if (!data || !size || !CanWriteSize(size))
		return false;

	Reserve(m_uiWriteOffset + size);
	memmove(m_pBuffer + m_uiWriteOffset, data, size);
	m_uiWriteOffset += size;
	return true;
}

bool CASBLOB::Write(unsigned char value)
{
	return Write(&value, sizeof(value));
}

bool CASBLOB::Write(unsigned short value)
{
	return Write(&value, sizeof(value));
}

bool CASBLOB::Write(unsigned long value)
{
	return Write(&value, sizeof(value));
}

bool CASBLOB::Write(unsigned long long value)
{
	return Write(&value, sizeof(value));
}

bool CASBLOB::Write(float value)
{
	return Write(&value, sizeof(value));
}

bool CASBLOB::Write(double value)
{
	return Write(&value, sizeof(value));
}

bool CASBLOB::Write(const CString &value, unsigned int size)
{
	if (!CanWriteSize(size) || value.Length() >= size)
		return false;

	if (value.Data())
		Write((unsigned char)*value.Data());

	WriteBytes(size - value.Length(), 0);
	return true;
}

bool CASBLOB::Write(const CString &value)
{
	if (!CanWriteSize(value.Length()))
		return false;

	if (value.Data())
		Write((unsigned char)*value.Data());

	return true;
}

bool CASBLOB::WriteBytes(unsigned int size, int value)
{
	if (!size)
		return true;

	if (!CanWriteSize(size))
		return false;

	Reserve(m_uiWriteOffset + size);
	memset(m_pBuffer + m_uiWriteOffset, value, size);
	m_uiWriteOffset += size;
	return true;
}

bool CASBLOB::WriteBytesUntil(unsigned int offset, int value)
{
	unsigned int size = offset - m_uiWriteOffset;

	if (!size)
		return true;

	if (offset > m_uiSize && !m_bCanResize)
		return false;

	Reserve(offset);
	memset(m_pBuffer + m_uiWriteOffset, value, size);
	m_uiWriteOffset += size;
	return true;
}
