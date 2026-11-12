#include "utils/CLinkedStringPool.h"
#include <stddef.h>
#include <string.h>

CLinkedStringPool::CLinkedStringPool()
	: m_pStrings(NULL)
{
}

CLinkedStringPool::~CLinkedStringPool()
{
	Clear();
}

const char *CLinkedStringPool::AllocateString(const char *text)
{
	if (!text)
		return NULL;

	unsigned int length = strlen(text);
	String_t *entry = new String_t;

	entry->m_pNext = m_pStrings;
	m_pStrings = entry;
	entry->m_pszString = new char[length + 1];

	char *destination = entry->m_pszString;

	for (unsigned int i = 0; i <= length; i++)
	{
		if (text[i] == '\\' && i < length)
		{
			i++;
			*destination++ = text[i] == 'n' ? '\n' : '\\';
		}
		else
		{
			*destination++ = text[i];
		}
	}

	return entry->m_pszString;
}

void CLinkedStringPool::Clear(void)
{
	String_t *entry = m_pStrings;

	while (entry)
	{
		String_t *next = entry->m_pNext;

		delete[] entry->m_pszString;
		delete entry;
		entry = next;
	}

	m_pStrings = NULL;
}
