#ifndef CSTRINGMAP_H
#define CSTRINGMAP_H

#include "CString.h"
#include <utility>

class CStringMap
{
public:
	enum { RESERVE_GROWTH = 4 };

	CStringMap(unsigned int capacity = 0);
	~CStringMap();

	void Reserve(unsigned int capacity);
	bool AddEntry(const char *name, const char *replacement);
	const char *Find(const char *name) const;
	const char *operator[](unsigned int index) const;

	unsigned int					m_uiCapacity;
	unsigned int					m_uiCount;
	std::pair<CString, CString>		*m_pEntries;
};

#endif
