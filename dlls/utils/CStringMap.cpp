#include "extdll.h"
#include "utils/CStringMap.h"

CStringMap::CStringMap(unsigned int capacity)
{
	Reserve(capacity);
}

void CStringMap::Reserve(unsigned int capacity)
{
	if (capacity <= m_Entries.capacity())
		return;

	m_Entries.reserve(capacity + RESERVE_GROWTH);
}

bool CStringMap::AddEntry(const char *name, const char *replacement)
{
	if (!name || !replacement)
		return false;

	Reserve(Count() + 1);

	m_Entries.emplace_back(name, replacement);
	return true;
}

const char *CStringMap::Find(const char *name) const
{
	if (!name)
		return NULL;

	for (const std::pair<CString, CString> &entry : m_Entries)
	{
		if (!stricmp(name, entry.first.Data()))
			return entry.second.Data();
	}

	return NULL;
}

const char *CStringMap::operator[](unsigned int index) const
{
	return m_Entries[index].second.Data();
}
