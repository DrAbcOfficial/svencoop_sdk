#ifndef CASMODULE_H
#define CASMODULE_H

#include "CASBaseClass.h"
#include "CString.h"
#include <map>
#include <stdarg.h>
#include <string>
#include <vector>

class CString;
class CASVirtualFileSystem;
class CASFile;
class asIScriptModule;
class CASModuleInfo;
class CASScriptInfo;
class CASDocumentation;
class CASScheduler;
class CASModuleHookManager;
class CASFunction;
class CASMethod;
class CASObjectType;
class CASObjectInstance;

class CASModule : public CASAtomicRefCountedBaseClass
{
public:
	CASModule(asIScriptModule *module, CASModuleInfo *moduleInfo);
	virtual ~CASModule();
	virtual void Invalidate(void);
	virtual void Think(void);

	void Release(void) const;
	const char *GetModuleName(void) const;
	CASScriptInfo *GetScriptInfo(void) const;
	bool FunctionExists(const char *declaration) const;
	bool ClassExists(const char *name) const;
	bool MethodExists(const char *className, const char *methodDeclaration) const;

	CASFunction *GetFunction(const char *declaration);
	CASObjectType *GetObjectType(const char *declaration);
	CASObjectInstance *CreateObjectInstance(const char *declaration);
	CASMethod *GetMethod(const char *classDeclaration, const char *methodDeclaration);
	void ClearFunctionCache(void);
	void PrintGlobals(void *file);

	bool CallFunction(void *returnValue, bool optional, const char *declaration,
		int argumentCount, va_list arguments);
	bool CallFunction(void *returnValue, bool optional, const char *declaration,
		int argumentCount, ...);
	bool CallFunction(bool optional, const char *declaration, int argumentCount, va_list arguments);
	bool CallFunction(bool optional, const char *declaration, int argumentCount, ...);
	bool CallFunction(void *returnValue, const char *declaration, int argumentCount, va_list arguments);
	bool CallFunction(void *returnValue, const char *declaration, int argumentCount, ...);
	bool CallFunction(const char *declaration, int argumentCount, va_list arguments);
	bool CallFunction(const char *declaration, int argumentCount, ...);

	bool CallMethod(void *returnValue, bool optional, void *object,
		const char *classDeclaration, const char *methodDeclaration, int argumentCount, va_list arguments);
	bool CallMethod(void *returnValue, bool optional, void *object,
		const char *classDeclaration, const char *methodDeclaration, int argumentCount, ...);
	bool CallMethod(bool optional, void *object, const char *classDeclaration,
		const char *methodDeclaration, int argumentCount, va_list arguments);
	bool CallMethod(bool optional, void *object, const char *classDeclaration,
		const char *methodDeclaration, int argumentCount, ...);
	bool CallMethod(void *returnValue, void *object, const char *classDeclaration,
		const char *methodDeclaration, int argumentCount, va_list arguments);
	bool CallMethod(void *returnValue, void *object, const char *classDeclaration,
		const char *methodDeclaration, int argumentCount, ...);
	bool CallMethod(void *object, const char *classDeclaration, const char *methodDeclaration,
		int argumentCount, va_list arguments);
	bool CallMethod(void *object, const char *classDeclaration, const char *methodDeclaration,
		int argumentCount, ...);

private:
	friend CASFile *CASVirtualFileSystem_OpenFile(CASVirtualFileSystem *, const CString &, unsigned char);
	friend void CASVirtualFileSystem_RemoveFile(CASVirtualFileSystem *, const CString &);

	friend class CASEngineReflectionGroup;
	friend class CASModuleReflectionGroup;

	friend CASModuleHookManager *ModuleHookManager_GetActiveHookManager( void );
	friend CASScheduler *Scheduler_GetActiveScheduler( void );

	friend void RegisterSCScriptModuleHookManager(CASDocumentation *documentation);
	friend void RegisterSCScriptScheduler(CASDocumentation *documentation);

	friend class CASBaseCallable;
	friend class CASBaseManager;
	friend class CASServerManager;
	friend class CASPluginManager;
	friend class CASConCommandSystem;
	friend class CASPlayerFuncs;
	friend class CASEntityLoader;
	friend class CASScheduler;
	friend class CASModuleHookManager;
	friend class CASCustomEntityFuncs;

	asIScriptModule                        *m_pModule;
	CASModuleInfo                          *m_pModuleInfo;
	CASScriptInfo                          *m_pScriptInfo;
	CASScheduler                           *m_pScheduler;
	CASModuleHookManager                   *m_pHookManager;
	std::map<std::string, CASFunction *>    m_FunctionCache;

	bool									m_bValid;
	bool									m_bHasCustomEntities;
	bool									m_bDelayedRemoval;
	bool									m_bDelayedReload;
	bool									m_bDisallowedForNextMap;

	std::vector<CString>					m_IncludedMaps;
	std::vector<CString>					m_ExcludedMaps;
};

void RegisterSCScriptModule(CASDocumentation *documentation);

#endif
