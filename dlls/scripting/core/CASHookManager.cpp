#include "extdll.h"
#include "scripting/core/CASHookManager.h"
#include "scripting/core/CASHook.h"
#include "scripting/core/CASLog.h"
#include "scripting/core/CASDocumentation.h"
#include "server/server_support.h"

CASHookManager::CASHookManager()
	: m_pHooks(NULL), m_uiHookCount(0), m_bRegistered(false)
{
}

CASHookManager::~CASHookManager()
{
}

CASHookManager *CASHookManager::GetInstance(void)
{
	static CASHookManager *instance;

	if (!instance)
		instance = new CASHookManager;

	return instance;
}

bool CASHookManager::RegisterHooks(void)
{
	if (!m_bRegistered)
	{
		for (CASHook *hook = CASHook::m_pHead; hook; hook = hook->m_pNext)
			m_uiHookCount++;

		m_pHooks = new CASHook *[m_uiHookCount];
		memset(m_pHooks, 0, m_uiHookCount * sizeof(*m_pHooks));

		unsigned int code = 0;

		for (CASHook *hook = CASHook::m_pHead; hook; hook = hook->m_pNext)
		{
			m_pHooks[code] = hook;
			hook->m_uiCode = code++;
		}

		m_bRegistered = true;
	}

	return true;
}

void CASHookManager::UnregisterHooks(void)
{
	if (!m_bRegistered)
		return;

	delete[] m_pHooks;
	m_pHooks = NULL;
	m_uiHookCount = 0;
	m_bRegistered = false;
}

void CASHookManager::RegisterSCScriptHooks(CASDocumentation *documentation)
{
	if (!m_bRegistered)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
		                             "CASHookManager::RegisterSCScriptHooks: Attempted to register hook codes without having registered hooks first!\n");
		return;
	}

	asDWORD accessMask = documentation->SetDefaultAccessMask(0);
	char declaration[256];

	for (unsigned int code = 0; code < m_uiHookCount; code++)
	{
		CASHook *hook = m_pHooks[code];

		if (UTIL_FormatString(declaration, sizeof(declaration), "Hooks::%s", hook->m_pszNamespace))
		{
			documentation->SetDefaultNamespace(documentation->AllocString(declaration));

			if (UTIL_FormatString(declaration, sizeof(declaration), "const uint32 %s", hook->m_pszName))
			{
				documentation->SetDefaultAccessMask(hook->m_uiModuleTypes);

				const char *propertyDeclaration = documentation->AllocString(declaration);

				documentation->RegisterGlobalProperty(hook->m_pszDescription, propertyDeclaration, &hook->m_uiCode);
			}
		}
		else
			CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "Failed to register hook '%s'\n", hook->m_pszName);
	}

	documentation->SetDefaultNamespace("");
	documentation->SetDefaultAccessMask(accessMask);
}

const CASHook *CASHookManager::GetHook(ModuleType::Type moduleType, unsigned int code) const
{
	if ((unsigned int) moduleType > ModuleType::PLUGIN)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
		                             "CASHookManager::GetHook: unsupported module type %d!\n", moduleType);
		return NULL;
	}

	CASHook *hook = m_pHooks[code];

	return (hook->m_uiModuleTypes & (1 << moduleType)) ? hook : NULL;
}

const CASHook *CASHookManager::GetHook(unsigned int code) const
{
	if (code < m_uiHookCount)
		return m_pHooks[code];

	CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
	                             "CASHookManager::GetHook: attempted to get hook with invalid id %u!\n", code);
	return NULL;
}
