#include "CAutoString.h"
#include <string.h>

CAutoString::CAutoString() : m_pszString(NULL)
{
}

CAutoString::CAutoString(const char *text) : m_pszString(NULL)
{
	if (!text)
		return;

	unsigned int length = strlen(text);
	char *destination = new char[length + 1];

	m_pszString = destination;

	for (unsigned int i = 0; i <= length; i++)
	{
		if (text[i] == '\\' && i < length - 1)
		{
			i++;
			*destination++ = text[i] == 'n' ? '\n' : '\\';
		}
		else
		{
			*destination++ = text[i];
		}
	}
}

CAutoString::~CAutoString()
{
	delete[] m_pszString;
}

const char *CAutoString::c_str() const
{
	return m_pszString ? m_pszString : "";
}

bool CAutoString::operator==(const CAutoString &other) const
{
	if (!m_pszString)
		return other.m_pszString == NULL;

	return other.m_pszString && strcmp(m_pszString, other.m_pszString) == 0;
}

size_t std::hash<CAutoString>::operator()(const CAutoString &string) const
{
	const char *text = string.c_str();
	unsigned int length = strlen(text);
	unsigned int hash = 0x811C9DC5;
	unsigned int step = length / 10 + 1;

	for (unsigned int i = 0; i < length; i += step)
		hash = (hash * 16777619) ^ (signed char)text[i];

	return hash;
}
