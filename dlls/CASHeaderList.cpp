#include "extdll.h"
#include "util.h"
#include "server_support.h"
#include "CASHeaderList.h"

CASHeaderList::CASHeaderList() : m_pHead(NULL), m_pTail(NULL), m_uiCount(0)
{
}

CASHeaderList::~CASHeaderList()
{
	Clear();
}

void CASHeaderList::Release(void) const
{
	if (InternalRelease())
		delete this;
}

bool CASHeaderList::HasHeader(const char *name) const
{
	if (!name)
		return false;

	for (Header_t *pHeader = m_pHead; pHeader; pHeader = pHeader->m_pNext)
	{
		if (!strcmp(name, pHeader->m_pszName))
			return true;
	}

	return false;
}

void CASHeaderList::AddHeader(const char *name, const char *source)
{
	if (!name || !source)
		return;

	Header_t *pHeader = new Header_t;
	unsigned int nameSize = strlen(name) + 1;

	pHeader->m_pNext = NULL;
	pHeader->m_pszName = new char[nameSize];
	UTIL_CopyString(pHeader->m_pszName, name, nameSize);

	unsigned int sourceSize = strlen(source) + 1;

	pHeader->m_pszSource = new char[sourceSize];
	UTIL_CopyString(pHeader->m_pszSource, source, sourceSize);

	if (m_pTail)
		m_pTail->m_pNext = pHeader;
	else
		m_pHead = pHeader;

	m_pTail = pHeader;
	m_uiCount++;
}

void CASHeaderList::Clear(void)
{
	if (!m_pHead)
		return;

	Header_t *pHeader = m_pHead;

	do
	{
		Header_t *pNext = pHeader->m_pNext;

		delete[] pHeader->m_pszName;
		delete[] pHeader->m_pszSource;
		delete pHeader;
		pHeader = pNext;
	}
	while (pHeader);

	m_pHead = NULL;
	m_pTail = NULL;
	m_uiCount = 0;
}
