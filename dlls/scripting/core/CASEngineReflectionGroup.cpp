#include "scripting/core/CASEngineReflectionGroup.h"
#include "scripting/core/CASServerManager.h"
#include "scripting/core/CASModule.h"
#include "scripting/core/CASModuleInfo.h"
#include "scripting/core/CASFunction.h"
#include "scripting/core/CASObjectType.h"
#include "scripting/core/CASLog.h"
#include "CString.h"
#include <string.h>

CASEngineReflectionGroup::~CASEngineReflectionGroup()
{
}

CASFunction *CASEngineReflectionGroup::FindGlobalFunction(const CString &name, bool searchByDeclaration)
{
	CASServerManager *manager = CASServerManager::GetInstance();
	CASModule *module = manager->m_pActiveModule;

	if (!module)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CReflection::FindGlobalFunction: active module is null!\n");
		return NULL;
	}

	asIScriptFunction *result = NULL;

	if (searchByDeclaration)
		result = manager->m_pEngine->GetGlobalFunctionByDecl(name.Data());
	else
	{
		unsigned int count = manager->m_pEngine->GetGlobalFunctionCount();

		for (unsigned int index = 0; index < count; index++)
		{
			asIScriptFunction *function = manager->m_pEngine->GetGlobalFunctionByIndex(index);

			if (!strcmp(name.Data(), function->GetName()))
			{
				result = function;
				break;
			}
		}
	}

	if (!result || !(result->GetAccessMask() & (1 << module->m_pModuleInfo->m_ModuleType)))
		return NULL;

	return new CASFunction(result, NULL);
}

unsigned int CASEngineReflectionGroup::GetGlobalFunctionCount(void) const
{
	return CASServerManager::GetInstance()->m_pEngine->GetGlobalFunctionCount();
}

CASFunction *CASEngineReflectionGroup::GetGlobalFunctionByIndex(unsigned int index)
{
	CASServerManager *manager = CASServerManager::GetInstance();
	CASModule *module = manager->m_pActiveModule;

	if (!module)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CReflection::GetGlobalFunctionByIndex: active module is null!\n");
		return NULL;
	}

	asIScriptFunction *result = manager->m_pEngine->GetGlobalFunctionByIndex(index);

	if (!result || !(result->GetAccessMask() & (1 << module->m_pModuleInfo->m_ModuleType)))
		return NULL;

	return new CASFunction(result, NULL);
}

CASObjectType *CASEngineReflectionGroup::FindObjectType(const CString &name, bool searchByDeclaration)
{
	CASServerManager *manager = CASServerManager::GetInstance();
	CASModule *module = manager->m_pActiveModule;

	if (!module)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CReflection::FindObjectType: active module is null!\n");
		return NULL;
	}

	asITypeInfo *result = searchByDeclaration ? manager->m_pEngine->GetTypeInfoByDecl(name.Data()) : manager->m_pEngine->GetTypeInfoByName(name.Data());

	if (!result || !(result->GetAccessMask() & (1 << module->m_pModuleInfo->m_ModuleType)))
		return NULL;

	return new CASObjectType(result, NULL);
}

unsigned int CASEngineReflectionGroup::GetObjectTypeCount(void) const
{
	return CASServerManager::GetInstance()->m_pEngine->GetObjectTypeCount();
}

CASObjectType *CASEngineReflectionGroup::GetObjectTypeByIndex(unsigned int index)
{
	CASServerManager *manager = CASServerManager::GetInstance();
	CASModule *module = manager->m_pActiveModule;

	if (!module)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CReflection::GetObjectTypeByIndex: active module is null!\n");
		return NULL;
	}

	asITypeInfo *result = manager->m_pEngine->GetObjectTypeByIndex(index);

	if (!result || !(result->GetAccessMask() & (1 << module->m_pModuleInfo->m_ModuleType)))
		return NULL;

	return new CASObjectType(result, NULL);
}
