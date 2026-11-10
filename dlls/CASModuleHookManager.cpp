#include "extdll.h"
#include "CASModuleHookManager.h"
#include "CASHookManager.h"
#include "CASModule.h"
#include "CASModuleInfo.h"
#include "CASFunction.h"
#include "CASLog.h"
#include "CASServerManager.h"
#include "CASDocumentation.h"
#include "server_support.h"

static const HookStopMode::Type HOOK_STOP_ON_HANDLED = (HookStopMode::Type) 0;
static const ASLogLevel_t HOOK_REGISTRATION_LOG_LEVEL = (ASLogLevel_t) 2;
static const unsigned int HOOK_FUNCTION_DECLARATION_LENGTH = 512;

CASModuleHookManager::CASModuleHookManager(CASModule *module)
	: m_pModule(module), m_pHooks(NULL), m_uiHookCount(CASHookManager::GetInstance()->m_uiHookCount)
{
	m_pHooks = new ScriptHook_t[m_uiHookCount];

	ModuleType::Type moduleType = module->m_pModuleInfo->m_ModuleType;

	for (unsigned int code = 0; code < m_uiHookCount; code++)
		m_pHooks[code].m_pHook = CASHookManager::GetInstance()->GetHook(moduleType, code);
}

CASModuleHookManager::~CASModuleHookManager()
{
	RemoveAllHooks();

	delete[] m_pHooks;
	m_pHooks = NULL;
}

bool CASModuleHookManager::RegisterHook(unsigned int code, asIScriptGeneric *generic)
{
	asIScriptFunction *function = NULL;
	void *object = generic->GetArgAddress(1);
	int typeId = generic->GetArgTypeId(1);

	if (!ValidateHookFunction(code, typeId, object, "RegisterHook", function))
		return false;

	ScriptHook_t *hook = FindScriptHook(code);

	if (FindHook(*hook, function) != hook->m_Functions.end())
	{
		CASLog::GetInstance()->Log(HOOK_REGISTRATION_LOG_LEVEL,
		                           "CModuleHookManager::RegisterHook: Cannot register duplicate hook: module '%s', hook '%s', function '%s::%s'\n",
		                           m_pModule->GetModuleName(), hook->m_pHook->m_pszName, function->GetNamespace(), function->GetName());
		return false;
	}

	hook->m_Functions.push_back(new CASFunction(function, m_pModule));

	asIScriptFunction *delegate = function->GetDelegateFunction();

	if (delegate)
	{
		asITypeInfo *type = function->GetDelegateObjectType();

		CASLog::GetInstance()->Log(HOOK_REGISTRATION_LOG_LEVEL,
		                           "Registering hook: module '%s', hook '%s', method '%s::%s::%s'\n",
		                           m_pModule->GetModuleName(), hook->m_pHook->m_pszName,
		                           type->GetNamespace(), type->GetName(), delegate->GetName());
	}
	else
	{
		CASLog::GetInstance()->Log(HOOK_REGISTRATION_LOG_LEVEL,
		                           "Registering hook: module '%s', hook '%s', function '%s::%s'\n",
		                           m_pModule->GetModuleName(), hook->m_pHook->m_pszName, function->GetNamespace(), function->GetName());
	}

	return true;
}

bool CASModuleHookManager::IsValidHookCode(unsigned int code) const
{
	return m_pHooks && code < CASHookManager::GetInstance()->m_uiHookCount &&
	       m_pHooks[code].m_pHook != NULL;
}

bool CASModuleHookManager::HookExists(unsigned int code) const
{
	return IsValidHookCode(code) && !FindScriptHook(code)->m_Functions.empty();
}

HookReturnCode_t CASModuleHookManager::CallHook(const CASHook &hook, int argumentCount, va_list arguments)
{
	if (!HookExists(hook.m_uiCode))
		return HOOK_CONTINUE;

	ScriptHook_t *scriptHook = FindScriptHook(hook.m_uiCode);
	std::vector<CASFunction *>::iterator it = scriptHook->m_Functions.end();
	HookReturnCode_t returnCode = HOOK_CONTINUE;

	while (it != scriptHook->m_Functions.begin())
	{
		CASFunction *function = *--it;
		HookReturnCode_t functionReturn = HOOK_CONTINUE;

		if (!function)
			continue;

		function->Call(&functionReturn, argumentCount, arguments);

		if (functionReturn == HOOK_HANDLED)
		{
			returnCode = HOOK_HANDLED;

			if (hook.m_StopMode == HOOK_STOP_ON_HANDLED)
				break;
		}
	}

	return returnCode;
}

HookReturnCode_t CASModuleHookManager::CallHook(const CASHook &hook, int argumentCount, ...)
{
	va_list arguments;

	va_start(arguments, argumentCount);
	HookReturnCode_t returnCode = CallHook(hook, argumentCount, arguments);
	va_end(arguments);

	return returnCode;
}

CASModuleHookManager::ScriptHook_t *CASModuleHookManager::FindScriptHook(unsigned int code)
{
	return &m_pHooks[code];
}

const CASModuleHookManager::ScriptHook_t *CASModuleHookManager::FindScriptHook(unsigned int code) const
{
	return &m_pHooks[code];
}

std::vector<CASFunction *>::iterator CASModuleHookManager::FindHook(ScriptHook_t &hook,
                                                                    asIScriptFunction *function)
{
	if (function)
	{
		for (std::vector<CASFunction *>::iterator it = hook.m_Functions.begin(); it != hook.m_Functions.end(); it++)
		{
			if ((*it)->GetFunction() == function)
				return it;
		}
	}

	return hook.m_Functions.end();
}

std::vector<CASFunction *>::const_iterator CASModuleHookManager::FindHook(const ScriptHook_t &hook,
                                                                          asIScriptFunction *function) const
{
	return const_cast<CASModuleHookManager *>(this)->FindHook(const_cast<ScriptHook_t &>(hook), function);
}

void CASModuleHookManager::RemoveHook(ScriptHook_t &hook)
{
	for (std::vector<CASFunction *>::iterator it = hook.m_Functions.begin(); it != hook.m_Functions.end(); it++)
		(*it)->Release();

	hook.m_Functions.clear();
}

void CASModuleHookManager::RemoveHook(unsigned int code)
{
	if (!IsValidHookCode(code))
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
		                             "CModuleHookManager::RemoveHook: Hook code is invalid!\n");
		return;
	}

	RemoveHook(*FindScriptHook(code));
}

void CASModuleHookManager::RemoveHook(unsigned int code, asIScriptGeneric *generic)
{
	asIScriptFunction *function = NULL;
	void *object = generic->GetArgAddress(1);
	int typeId = generic->GetArgTypeId(1);

	if (!ValidateHookFunction(code, typeId, object, "RemoveHook", function))
		return;

	ScriptHook_t *hook = FindScriptHook(code);
	std::vector<CASFunction *>::iterator entry = FindHook(*hook, function);

	// Generic removal does not release the erased wrapper
	if (entry != hook->m_Functions.end())
		hook->m_Functions.erase(entry);
}

void CASModuleHookManager::RemoveAllHooks(void)
{
	if (!m_pHooks)
		return;

	for (unsigned int code = 0; code < m_uiHookCount; code++)
		RemoveHook(m_pHooks[code]);
}

bool CASModuleHookManager::ValidateHookFunction(unsigned int code, int typeId, void *object,
                                                const char *operation, asIScriptFunction *&function) const
{
	function = NULL;

	if (!IsValidHookCode(code))
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
		                             "CModuleHookManager::%s: Hook code is invalid!\n", operation);
		return false;
	}

	asITypeInfo *type = CASServerManager::GetInstance()->m_pEngine->GetTypeInfoById(typeId);

	if (!type)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
		                             "CModuleHookManager::%s: unknown type!\n", operation);
		return false;
	}

	if (!(type->GetFlags() & asOBJ_FUNCDEF))
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
		                             "CModuleHookManager::%s: Object is not a function or delegate!\n", operation);
		return false;
	}

	if (object && (typeId & asTYPEID_OBJHANDLE))
		object = *(void **)object;

	if (!object)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
		                             "CModuleHookManager::%s: Object is null!\n", operation);
		return false;
	}

	asIScriptFunction *candidate = (asIScriptFunction *)object;
	asIScriptFunction *hookFunction = FindScriptHook(code)->m_pHook->m_pFunctionType;

	if (hookFunction->IsCompatibleWithTypeId(candidate->GetTypeId()))
	{
		function = candidate;
		return true;
	}

	asIScriptFunction *delegate = candidate->GetDelegateFunction();

	if (delegate)
	{
		asITypeInfo *objectType = candidate->GetDelegateObjectType();

		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
		                             "CModuleHookManager::%s: Method '%s::%s::%s' is incompatible with hook '%s'!\n",
		                             operation, objectType->GetNamespace(), objectType->GetName(), delegate->GetName(), hookFunction->GetName());
	}
	else
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
		                             "CModuleHookManager::%s: Function '%s::%s' is incompatible with hook '%s'!\n",
		                             operation, candidate->GetNamespace(), candidate->GetName(), hookFunction->GetName());
	}

	return false;
}

void CASModuleHookManager_RegisterHookGeneric(asIScriptGeneric *generic)
{
	unsigned int code = generic->GetArgDWord(0);
	CASModuleHookManager *manager = (CASModuleHookManager *)generic->GetObject();
	bool result = manager->RegisterHook(code, generic);

	generic->SetReturnByte(result);
}

void CASModuleHookManager_RemoveHookGeneric(asIScriptGeneric *generic)
{
	unsigned int code = generic->GetArgDWord(0);
	CASModuleHookManager *manager = (CASModuleHookManager *)generic->GetObject();

	manager->RemoveHook(code, generic);
}

CASModuleHookManager *ModuleHookManager_GetActiveHookManager( void )
{
	CASModule *module = CASServerManager::GetInstance()->m_pActiveModule;

	if ( !module )
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT,
		                              "Attempted to get current module while outside of a script!\n" );

		return NULL;
	}

	return module->m_pHookManager;
}

void RegisterSCScriptModuleHookManager(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("Module hook manager", "CModuleHookManager", 0, asOBJ_REF | asOBJ_NOCOUNT);
	documentation->RegisterObjectMethod("Registers a hook. Pass in a hook function or delegate.",
	                                    "CModuleHookManager", "bool RegisterHook(uint uiHookCode, ?& in pHookFunction)",
	                                    asFUNCTION(CASModuleHookManager_RegisterHookGeneric), asCALL_GENERIC);
	documentation->RegisterObjectMethod("Removes all functions hooked into the given hook.",
	                                    "CModuleHookManager", "void RemoveHook(uint uiHookCode)",
	                                    asMETHODPR(CASModuleHookManager, RemoveHook, (unsigned int), void), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Removes a hook. Pass in the hook function or delegate to remove.",
	                                    "CModuleHookManager", "void RemoveHook(uint uiHookCode, ?& in pHookFunction)",
	                                    asFUNCTION(CASModuleHookManager_RemoveHookGeneric), asCALL_GENERIC);

	asIScriptEngine *engine = documentation->m_pEngine;
	asDWORD accessMask = engine->SetDefaultAccessMask(0);
	unsigned int functionIndex = engine->GetFuncdefCount();

	for (unsigned int code = 0; code < CASHookManager::GetInstance()->m_uiHookCount; code++)
	{
		CASHook *hook = CASHookManager::GetInstance()->m_pHooks[code];
		char declaration[HOOK_FUNCTION_DECLARATION_LENGTH];
		int length = UTIL_FormatString(declaration, sizeof(declaration), "%s %sHook(%s)",
		                               CASHook::HOOK_RETURN_TYPE, hook->m_pszName, hook->m_pszArguments);

		if (length < 0 || (unsigned int)length >= sizeof(declaration))
		{
			CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "Failed to register hook %s!\n", hook->m_pszName);
			continue;
		}

		engine->SetDefaultAccessMask(hook->m_uiModuleTypes);
		documentation->RegisterFuncDef(hook->m_pszDescription, documentation->AllocString(declaration));
		hook->m_pFunctionType = engine->GetFuncdefByIndex(functionIndex++)->GetFuncdefSignature();
	}

	engine->SetDefaultAccessMask(accessMask);

	documentation->RegisterGlobalFunction("Gets the current script's hook manager.",
	                                      "CModuleHookManager@ get_g_Hooks()", asFUNCTION( ModuleHookManager_GetActiveHookManager ), asCALL_CDECL);
}
