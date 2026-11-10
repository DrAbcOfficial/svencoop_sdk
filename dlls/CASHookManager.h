#ifndef CASHOOKMANAGER_H
#define CASHOOKMANAGER_H

#include "CASDirectoryList.h"

class CASDocumentation;
class CASHook;
class CASModuleHookManager;

class CASHookManager
{
public:
	CASHookManager();
	~CASHookManager();

	static CASHookManager *GetInstance(void);
	bool RegisterHooks(void);
	void UnregisterHooks(void);
	const CASHook *GetHook(ModuleType::Type moduleType, unsigned int code) const;
	const CASHook *GetHook(unsigned int code) const;
	void RegisterSCScriptHooks(CASDocumentation *documentation);

private:
	friend void RegisterSCScriptModuleHookManager(CASDocumentation *documentation);

	friend class CASModuleHookManager;

	CASHook           **m_pHooks;
	unsigned int        m_uiHookCount;
	bool                m_bRegistered;
};

#endif
