#ifndef CASMODULEHOOKMANAGER_H
#define CASMODULEHOOKMANAGER_H

#include "scripting/core/CASBaseClass.h"
#include "scripting/core/CASHook.h"
#include <stdarg.h>
#include <vector>

class asIScriptGeneric;
class CASFunction;
class CASModule;

class CASModuleHookManager : public CASBaseClass
{
public:
	CASModuleHookManager(CASModule *module);
	~CASModuleHookManager();

	bool IsValidHookCode(unsigned int code) const;
	bool HookExists(unsigned int code) const;
	HookReturnCode_t CallHook(const CASHook &hook, int argumentCount, va_list arguments);
	HookReturnCode_t CallHook(const CASHook &hook, int argumentCount, ...);
	bool RegisterHook(unsigned int code, asIScriptGeneric *generic);
	void RemoveHook(unsigned int code);
	void RemoveHook(unsigned int code, asIScriptGeneric *generic);
	void RemoveAllHooks(void);

private:
	struct ScriptHook_t
	{
		const CASHook                  *m_pHook;
		std::vector<CASFunction *>      m_Functions;
	};

	ScriptHook_t *FindScriptHook(unsigned int code);
	const ScriptHook_t *FindScriptHook(unsigned int code) const;
	std::vector<CASFunction *>::iterator FindHook(ScriptHook_t &hook, asIScriptFunction *function);
	std::vector<CASFunction *>::const_iterator FindHook(const ScriptHook_t &hook,
	                                                    asIScriptFunction *function) const;
	void RemoveHook(ScriptHook_t &hook);
	bool ValidateHookFunction(unsigned int code, int typeId, void *object,
	                          const char *operation, asIScriptFunction *&function) const;

	CASModule          *m_pModule;
	ScriptHook_t       *m_pHooks;
	unsigned int        m_uiHookCount;
};

void RegisterSCScriptModuleHookManager(CASDocumentation *documentation);

#endif
