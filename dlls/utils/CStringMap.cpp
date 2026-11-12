#include "extdll.h"
#include "utils/CStringMap.h"

CStringMap::CStringMap(unsigned int capacity)
{
	m_uiCapacity = 0;
	m_uiCount = 0;
	m_pEntries = NULL;
	Reserve(capacity);
}

CStringMap::~CStringMap()
{
	if (m_pEntries)
	{
		delete[] m_pEntries;
		m_pEntries = NULL;
		m_uiCapacity = 0;
		m_uiCount = 0;
	}
}

void CStringMap::Reserve(unsigned int capacity)
{
	if (capacity <= m_uiCapacity)
		return;

	std::pair<CString, CString> *entries = new std::pair<CString, CString>[capacity + RESERVE_GROWTH];

	if (m_pEntries)
	{
		for (unsigned int i = 0; i < m_uiCount; i++)
		{
			entries[i].first = m_pEntries[i].first;
			entries[i].second = m_pEntries[i].second;
		}

		delete[] m_pEntries;
	}

	m_pEntries = entries;
	m_uiCapacity = capacity + RESERVE_GROWTH;
}

bool CStringMap::AddEntry(const char *name, const char *replacement)
{
	if (!name || !replacement)
		return false;

	Reserve(m_uiCount + 1);

	std::pair<CString, CString> &entry = m_pEntries[m_uiCount++];

	entry.first = name;
	entry.second = replacement;
	return true;
}

const char *CStringMap::Find(const char *name) const
{
	if (!name)
		return NULL;

	for (unsigned int i = 0; i < m_uiCount; i++)
	{
		if (!stricmp(name, m_pEntries[i].first.Data()))
			return m_pEntries[i].second.Data();
	}

	return NULL;
}

const char *CStringMap::operator[](unsigned int index) const
{
	return m_pEntries[index].second.Data();
}
