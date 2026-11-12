#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "server/CCustomEntityHandler.h"
#include "scripting/core/CASObjectInstance.h"
#include "scripting/core/CASMethod.h"
#include "scripting/core/CASServerManager.h"
#include "scripting/core/CASDocumentation.h"
#include "scripting/core/CASLog.h"
#include <angelscript.h>

CCustomEntityHandler::Removal_t *CCustomEntityHandler::m_pToRemoveHead;

CCustomEntityHandler::CCustomEntityHandler(CBaseEntity *entity)
    : m_pEntity(entity), m_pScriptInstance(NULL), m_pThinkFunction(NULL), m_pTouchFunction(NULL),
      m_pBlockedFunction(NULL), m_pUseFunction(NULL), m_pFunctionCacheHead(NULL)
{
}

CCustomEntityHandler::~CCustomEntityHandler()
{
	DeleteFunctions();
	m_pEntity = NULL;

	if (m_pScriptInstance)
		m_pScriptInstance->Release();

	m_pScriptInstance = NULL;
}

bool CCustomEntityHandler::MethodExists(const char *declaration) const
{
	return m_pScriptInstance ? m_pScriptInstance->MethodExists(declaration) : false;
}

void CCustomEntityHandler::SetScriptInstance(CASObjectInstance *instance)
{
	if (m_pScriptInstance)
	{
		CASLog::GetInstance()->Warning(AS_LOG_DEFAULT, "CCustomEntityHandler: freeing existing instance to replace with another!\n");
		CallMethod(true, "void OnDestroy()");
		m_pScriptInstance->Release();
	}

	m_pScriptInstance = instance;

	if (instance)
		instance->AddRef();
}

bool CCustomEntityHandler::CallMethod(bool optional, const char *declaration, ...)
{
	va_list arguments;

	va_start(arguments, declaration);
	bool result = false;

	if (CASServerManager::GetInstance()->m_bScriptLifecycleEntered && m_pScriptInstance)
		result = m_pScriptInstance->CallMethod(optional, declaration, 0, arguments);

	va_end(arguments);

	return result;
}

bool CCustomEntityHandler::CallMethod(void *returnValue, bool optional, const char *declaration, ...)
{
	va_list arguments;

	va_start(arguments, declaration);
	bool result = false;

	if (CASServerManager::GetInstance()->m_bScriptLifecycleEntered && m_pScriptInstance)
		result = m_pScriptInstance->CallMethod(returnValue, optional, declaration, 0, arguments);

	va_end(arguments);

	return result;
}

bool CCustomEntityHandler::CallMethod(const char *declaration, va_list arguments)
{
	if (CASServerManager::GetInstance()->m_bScriptLifecycleEntered && m_pScriptInstance)
		return m_pScriptInstance->CallMethod(declaration, 0, arguments);

	return false;
}

bool CCustomEntityHandler::CallMethod(CASMethod *&function, const char *declaration, va_list arguments)
{
	if (function)
	{
		function->SetThisPointer(m_pScriptInstance->m_pObject);
		function->Call(0, arguments);
		return true;
	}

	if (MethodExists(declaration))
	{
		CallMethod(declaration, arguments);
		return true;
	}

	return false;
}

bool CCustomEntityHandler::CallMethod(CASMethod *&function, const char *declaration, ...)
{
	va_list arguments;

	va_start(arguments, declaration);
	bool result = m_pScriptInstance ? CallMethod(function, declaration, arguments) : false;
	va_end(arguments);

	return result;
}

bool CCustomEntityHandler::CallThink(void)
{
	return m_pScriptInstance ? CallMethod(m_pThinkFunction, "void Think()") : false;
}

bool CCustomEntityHandler::CallTouch(CBaseEntity *other)
{
	return m_pScriptInstance ? CallMethod(m_pTouchFunction, "void Touch(CBaseEntity@)", other) : false;
}

bool CCustomEntityHandler::CallBlocked(CBaseEntity *other)
{
	return m_pScriptInstance ? CallMethod(m_pBlockedFunction, "void Blocked(CBaseEntity@)", other) : false;
}

bool CCustomEntityHandler::CallUse(CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value)
{
	if (!m_pScriptInstance)
		return false;

	return CallMethod(m_pUseFunction, "void Use(CBaseEntity@, CBaseEntity@, USE_TYPE, float)",
	                  activator, caller, useType, value);
}

CASMethod *CCustomEntityHandler::GetFunctionFromCache(asIScriptFunction *function) const
{
	for (CachedFunction_t *entry = m_pFunctionCacheHead; entry; entry = entry->m_pNext)
	{
		if (entry->m_pMethod->GetFunction() == function)
			return entry->m_pMethod;
	}

	return NULL;
}

void CCustomEntityHandler::AddFunctionToCache(CASMethod *function)
{
	if (!function)
		return;

	CachedFunction_t *entry = new CachedFunction_t;

	entry->m_pMethod = function;
	entry->m_pNext = m_pFunctionCacheHead;
	m_pFunctionCacheHead = entry;
}

void CCustomEntityHandler::SetFunction(CASMethod *&destination, asIScriptFunction *function, const char *name)
{
	if (destination)
		destination = NULL;

	if (!function)
		return;

	if (function->GetFuncType() == asFUNC_DELEGATE
	    && function->GetDelegateObjectType() == m_pScriptInstance->m_pTypeInfo)
	{
		asIScriptFunction *delegateFunction = function->GetDelegateFunction();

		destination = GetFunctionFromCache(delegateFunction);

		if (!destination)
		{
			destination = new CASMethod(delegateFunction, CASServerManager::GetInstance()->m_pActiveModule);
			AddFunctionToCache(destination);
		}
	}
	else
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CustomEntityCallbackHandler::Set%sFunction: function must be a delegate of the owning object type!\n", name);
	}

	function->Release();
}

void CCustomEntityHandler::SetThinkFunction(asIScriptFunction *function)
{
	SetFunction(m_pThinkFunction, function, "Think");
}

void CCustomEntityHandler::SetTouchFunction(asIScriptFunction *function)
{
	SetFunction(m_pTouchFunction, function, "Touch");
}

void CCustomEntityHandler::SetBlockedFunction(asIScriptFunction *function)
{
	SetFunction(m_pBlockedFunction, function, "Blocked");
}

void CCustomEntityHandler::SetUseFunction(asIScriptFunction *function)
{
	SetFunction(m_pUseFunction, function, "Use");
}

void CCustomEntityHandler::DeleteFunctions(void)
{
	CachedFunction_t *entry = m_pFunctionCacheHead;

	while (entry)
	{
		CachedFunction_t *next = entry->m_pNext;

		entry->m_pMethod->Release();
		delete entry;
		entry = next;
	}

	m_pFunctionCacheHead = NULL;
}

void CCustomEntityHandler::ScheduleForRemoval(CCustomEntityHandler *handler)
{
	if (!handler)
		return;

	Removal_t *entry = new Removal_t;

	entry->m_pNext = m_pToRemoveHead;
	entry->m_pHandler = handler;
	m_pToRemoveHead = entry;
	handler->m_pEntity = NULL;
}

void CCustomEntityHandler::Clear(void)
{
	Removal_t *entry = m_pToRemoveHead;

	if (entry)
	{
		do
		{
			Removal_t *next = entry->m_pNext;

			delete entry->m_pHandler;
			delete entry;
			entry = next;
		} while (entry);

		m_pToRemoveHead = NULL;
	}
}

void CCustomEntityHandler::Think(void)
{
	Clear();
}

void RegisterSCScriptCustomEntityCallbackHandler(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("Custom entity callback handler. Is used to set callbacks to methods",
	                                  "CustomEntityCallbackHandler", sizeof(CCustomEntityHandler), asOBJ_REF | asOBJ_NOCOUNT);
	documentation->RegisterObjectMethod("Sets the think function for this handler's owning instance.",
	                                    "CustomEntityCallbackHandler", "void SetThinkFunction(ThinkFunction@ pFunction)",
	                                    asMETHOD(CCustomEntityHandler, SetThinkFunction), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Sets the touch function for this handler's owning instance.",
	                                    "CustomEntityCallbackHandler", "void SetTouchFunction(TouchFunction@ pFunction)",
	                                    asMETHOD(CCustomEntityHandler, SetTouchFunction), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Sets the blocked function for this handler's owning instance.",
	                                    "CustomEntityCallbackHandler", "void SetBlockedFunction(BlockedFunction@ pFunction)",
	                                    asMETHOD(CCustomEntityHandler, SetBlockedFunction), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Sets the use function for this handler's owning instance.",
	                                    "CustomEntityCallbackHandler", "void SetUseFunction(UseFunction@ pFunction)",
	                                    asMETHOD(CCustomEntityHandler, SetUseFunction), asCALL_THISCALL);
}
