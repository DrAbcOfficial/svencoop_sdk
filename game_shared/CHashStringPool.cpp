#include "extdll.h"
#include "util.h"
#include "CHashStringPool.h"

namespace
{
	// Reproduces the escape handling of the old CAutoString constructor:
	// "\\n" becomes a newline, "\\\\" collapses to "\\".
	std::string UnescapeAllocateString(const char *text)
	{
		if (!text)
			return {};

		std::string result;
		const std::size_t length = strlen(text);

		result.reserve(length + 1);

		for (std::size_t i = 0; i <= length; i++)
		{
			if (text[i] == '\\' && i < length - 1)
			{
				i++;
				result += text[i] == 'n' ? '\n' : '\\';
			}
			else
			{
				result += text[i];
			}
		}

		return result;
	}
}

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
	std::unordered_set<std::string>::const_iterator found = m_Strings.find(UnescapeAllocateString(text));

	return found != m_Strings.end() ? found->c_str() : "";
}

const char *CHashStringPool::Allocate(const char *text, bool *allocated)
{
	std::pair<std::unordered_set<std::string>::iterator, bool> result = m_Strings.emplace(UnescapeAllocateString(text));

	if (allocated)
		*allocated = result.second;

	return result.first != m_Strings.end() ? result.first->c_str() : "";
}

void CHashStringPool::DebugPrint(void) const
{
	unsigned int memory = 0;

	for (std::unordered_set<std::string>::const_iterator it = m_Strings.begin(); it != m_Strings.end(); it++)
		memory += it->size() + 1;

	ALERT(at_console, "CHashStringPool: current status:\nNumber of strings allocated: %u\nMemory in use: %u bytes\n",
		(unsigned int)m_Strings.size(), memory);
}
