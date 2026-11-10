#include "extdll.h"
#include "util.h"
#include "CHashStringPool.h"

CHashStringPool::CHashStringPool() : m_Strings(256)
{
}

CHashStringPool::~CHashStringPool()
{
}

void CHashStringPool::Clear(void)
{
	m_Strings.clear();
}

const char *CHashStringPool::Find(const char *text) const
{
	std::unordered_set<CAutoString>::const_iterator found = m_Strings.find(CAutoString(text));

	return found != m_Strings.end() ? found->c_str() : "";
}

const char *CHashStringPool::Allocate(const char *text, bool *allocated)
{
	std::pair<std::unordered_set<CAutoString>::iterator, bool> result = m_Strings.emplace(text);

	if (allocated)
		*allocated = result.second;

	return result.first != m_Strings.end() ? result.first->c_str() : "";
}

void CHashStringPool::DebugPrint(void) const
{
	unsigned int memory = 0;

	for (std::unordered_set<CAutoString>::const_iterator it = m_Strings.begin(); it != m_Strings.end(); it++)
		memory += strlen(it->c_str()) + 1;

	ALERT(at_console, "CHashStringPool: current status:\nNumber of strings allocated: %u\nMemory in use: %u bytes\n",
		(unsigned int)m_Strings.size(), memory);
}
