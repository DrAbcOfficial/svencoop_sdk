#ifndef CHASHSTRINGPOOL_H
#define CHASHSTRINGPOOL_H

#include <string>
#include <unordered_set>

// String interning pool. Pointers handed out by Find/Allocate stay valid
// until Clear(); AllocStringLocal relies on that to build engine string_t
// offsets, so elements must never be erased individually.
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
	std::unordered_set<std::string>	m_Strings;
};

#endif
