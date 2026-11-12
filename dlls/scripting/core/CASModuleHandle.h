#ifndef CASMODULEHANDLE_H
#define CASMODULEHANDLE_H

#include "scripting/core/CASBaseClass.h"
#include "scripting/core/CASDirectoryList.h"

class CASBaseManager;
class CASModule;
class CASModuleBuilder;

class CASModuleHandle : public CASBaseClass
{
public:
	~CASModuleHandle();

	const char *GetModuleName(void) const;
	bool Build(bool freeBuilder);
	void FreeBuilder(void);

private:
	friend class CASBaseManager;
	friend class CASServerManager;
	friend class CASScriptEntityPreprocessor;

	CASModuleHandle(ModuleType::Type type, const char *name);

	CASModuleBuilder	*m_pBuilder;
	CASModule			*m_pModule;
	bool				m_bPendingBuild;
};

#endif
