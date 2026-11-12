#ifndef CASMODULELIST_H
#define CASMODULELIST_H

#include <vector>

class CASModule;

class CASModuleList
{
public:
	CASModuleList();
	~CASModuleList();

	unsigned int Count(void) const;
	int IndexOf(const char *name) const;
	int IndexOf(CASModule *module) const;
	bool Contains(const char *name) const;
	bool Contains(CASModule *module) const;
	CASModule *Get(unsigned int index);
	const CASModule *Get(unsigned int index) const;
	int Add(CASModule *module);
	void Remove(unsigned int index);
	void Remove(CASModule *module);
	void Clear(void);

private:
	std::vector<CASModule *>	m_Modules;
};

#endif
