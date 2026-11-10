#include "CString.h"
#include <string.h>

CString::CString()
{
	Construct();
}

CString::CString(const char *text)
{
	Construct();
	*this = text;
}

CString::CString(const CString &text)
{
	Construct();
	*this = text;
}

CString::~CString()
{
	if (GetDynamicAllocation() && m_pszString)
		delete[] m_pszString;
}

void CString::Construct(void)
{
	m_szBuffer[0] = 0;
	m_pszString = m_szBuffer;
	m_uiLength = 0;
	m_uiCapacity = sizeof(m_szBuffer);
	m_bStatic = false;
	m_uiTokenizeIndex = 0;
}

unsigned int CString::GetDynamicAllocation(void) const
{
	if (m_pszString == m_szBuffer || m_bStatic)
		return 0;

	return m_uiCapacity;
}

void CString::Resize(unsigned int length, bool preserve)
{
	if (length + 1 == m_uiCapacity || m_bStatic)
		return;

	char *buffer = new char[length + 1];

	if (preserve && m_pszString)
	{
		m_pszString[m_uiLength] = 0;
		strncpy(buffer, m_pszString, length + 1);
		buffer[length] = 0;
	}

	if (m_pszString != m_szBuffer)
		delete[] m_pszString;

	m_pszString = buffer;
	m_uiCapacity = length + 1;
}

void CString::Reserve(unsigned int length, bool preserve)
{
	if (length >= m_uiCapacity)
		Resize(length, preserve);
}

void CString::Assign(const char *text, unsigned int length)
{
	if (!text)
		text = "";

	if (text == m_pszString)
		return;

	Reserve(length, false);
	strcpy(m_pszString, text);
	m_uiLength = length;
}

CString &CString::operator=(const char *text)
{
	if (text)
		Assign(text, strlen(text));

	return *this;
}

CString &CString::operator=(const CString &text)
{
	Assign(text.m_pszString, text.m_uiLength);
	return *this;
}

const char *CString::Data(void) const
{
	return m_pszString;
}

unsigned int CString::Length(void) const
{
	return m_uiLength;
}
