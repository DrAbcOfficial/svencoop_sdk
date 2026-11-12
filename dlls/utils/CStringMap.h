#ifndef CSTRINGMAP_H
#define CSTRINGMAP_H

#include "CString.h"
#include <utility>
#include <vector>

// Linear key -> replacement map. Entries keep insertion order, duplicate
// keys are allowed and Find returns the first case-insensitive match.
class CStringMap
{
public:
	enum { RESERVE_GROWTH = 4 };

	CStringMap(unsigned int capacity = 0);

	void Reserve(unsigned int capacity);
	bool AddEntry(const char *name, const char *replacement);
	const char *Find(const char *name) const;
	const char *operator[](unsigned int index) const;
	unsigned int Count() const { return static_cast<unsigned int>(m_Entries.size()); }

	std::vector<std::pair<CString, CString>>	m_Entries;
};

#endif
