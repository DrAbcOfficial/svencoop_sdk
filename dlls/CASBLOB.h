#ifndef CASBLOB_H
#define CASBLOB_H

#include "CASBaseClass.h"
#include "CString.h"

class CASFile;

class CASBLOB : public CASRefCountedBaseClass
{
public:
	CASBLOB(unsigned int size = 0, bool canResize = true);
	CASBLOB(const CASBLOB &other);
	~CASBLOB();

	CASBLOB &operator=(const CASBLOB &other);
	void Release(void) const;
	void Copy(const CASBLOB &other);
	bool HasAllocatedBuffer(void) const;
	unsigned int GetSizeInBytes(void) const;
	unsigned int GetReadOffset(void) const;
	bool ReadReachedEnd(void) const;
	bool ReadSizeValid(unsigned int size) const;
	unsigned int GetWriteOffset(void) const;
	bool CanWriteSize(unsigned int size) const;
	unsigned int GetWriteBytesLeft(void) const;
	bool CanResize(void) const;
	void Resize(unsigned int size);
	void Reserve(unsigned int size);
	void ShrinkToFit(void);
	void Clear(void);

	bool ReadData(void *data, unsigned int size);
	signed char ReadInt8(void);
	signed char ReadInt8(bool &success);
	short ReadInt16(void);
	short ReadInt16(bool &success);
	int ReadInt32(void);
	int ReadInt32(bool &success);
	long long ReadInt64(void);
	long long ReadInt64(bool &success);
	float ReadFloat(void);
	float ReadFloat(bool &success);
	double ReadDouble(void);
	double ReadDouble(bool &success);
	CString ReadString(unsigned int size, bool &success);
	CString ReadString(unsigned int size);
	CString ReadString(bool &success);
	CString ReadString(void);

	bool Write(const void *data, unsigned int size);
	bool Write(unsigned char value);
	bool Write(unsigned short value);
	bool Write(unsigned long value);
	bool Write(unsigned long long value);
	bool Write(float value);
	bool Write(double value);
	bool Write(const CString &value, unsigned int size);
	bool Write(const CString &value);
	bool WriteBytes(unsigned int size, int value = 0);
	bool WriteBytesUntil(unsigned int offset, int value = 0);

private:
	friend class CASFile;

	unsigned char	*m_pBuffer;
	unsigned int	m_uiSize;
	unsigned int	m_uiReadOffset;
	unsigned int	m_uiWriteOffset;
	bool			m_bCanResize;
};

#endif
