#include "extdll.h"
#include "CASModuleList.h"
#include "CASModule.h"

#include <limits.h>

CASModuleList::CASModuleList()
{
}

CASModuleList::~CASModuleList()
{
	Clear();
}

unsigned int CASModuleList::Count(void) const
{
	return m_Modules.size();
}

int CASModuleList::IndexOf(const char *name) const
{
	if (!name || !*name)
		return -1;

	for (unsigned int index = 0; index < Count(); index++)
	{
		if (!stricmp(name, m_Modules[index]->GetModuleName()))
			return index;
	}

	return -1;
}

int CASModuleList::IndexOf(CASModule *module) const
{
	if (!module)
		return -1;

	for (unsigned int index = 0; index < Count(); index++)
	{
		if (m_Modules[index] == module)
			return index;
	}

	return -1;
}

bool CASModuleList::Contains(const char *name) const
{
	return IndexOf(name) != -1;
}

bool CASModuleList::Contains(CASModule *module) const
{
	return module && IndexOf(module) != -1;
}

CASModule *CASModuleList::Get(unsigned int index)
{
	return index < Count() ? m_Modules[index] : NULL;
}

const CASModule *CASModuleList::Get(unsigned int index) const
{
	return index < Count() ? m_Modules[index] : NULL;
}

int CASModuleList::Add(CASModule *module)
{
	if (!module || Contains(module) || Count() == UINT_MAX)
		return -1;

	m_Modules.push_back(module);
	module->AddRef();

	return Count() - 1;
}

void CASModuleList::Remove(unsigned int index)
{
	if (index >= Count())
		return;

	CASModule *module = m_Modules[index];

	m_Modules.erase(m_Modules.begin() + index);
	module->Release();
}

void CASModuleList::Remove(CASModule *module)
{
	if (!module)
		return;

	int index = IndexOf(module);

	if (index != -1)
		Remove((unsigned int)index);
}

void CASModuleList::Clear(void)
{
	for (std::vector<CASModule *>::iterator it = m_Modules.begin(); it != m_Modules.end(); it++)
		(*it)->Release();

	m_Modules.clear();
}
