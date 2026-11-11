#ifndef CHASHSTRINGPOOL_H
#define CHASHSTRINGPOOL_H

#include "CAutoString.h"
#include <cstddef>
#include <unordered_set>

class CHashStringPool
{
public:
	CHashStringPool();
	~CHashStringPool();

	void Clear(void);
	const char *Find(const char *text) const;
	const char *Allocate(const char *text, bool *allocated = NULL);
	void DebugPrint(void) const;

private:
	std::unordered_set<CAutoString>	m_Strings;
};

#endif
