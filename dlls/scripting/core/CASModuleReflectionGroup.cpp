#include "scripting/core/CASModuleReflectionGroup.h"
#include "scripting/core/CASServerManager.h"
#include "scripting/core/CASModule.h"
#include "scripting/core/CASModuleInfo.h"
#include "scripting/core/CASFunction.h"
#include "scripting/core/CASObjectType.h"
#include "scripting/core/CASLog.h"
#include "CString.h"
#include <string.h>

CASModuleReflectionGroup::~CASModuleReflectionGroup()
{
}

CASFunction *CASModuleReflectionGroup::FindGlobalFunction(const CString &name, bool searchByDeclaration)
{
	CASServerManager *manager = CASServerManager::GetInstance();
	CASModule *module = manager->m_pActiveModule;

	if (!module)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CReflection::FindGlobalFunction: active module is null!\n");
		return NULL;
	}

	asIScriptFunction *result = searchByDeclaration ? module->m_pModule->GetFunctionByDecl(name.Data()) : module->m_pModule->GetFunctionByName(name.Data());

	if (!result || !(result->GetAccessMask() & (1 << module->m_pModuleInfo->m_ModuleType)))
		return NULL;

	return new CASFunction(result, module);
}

unsigned int CASModuleReflectionGroup::GetGlobalFunctionCount(void) const
{
	CASServerManager *manager = CASServerManager::GetInstance();
	CASModule *module = manager->m_pActiveModule;

	if (!module)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CReflection::GetGlobalFunctionCount: active module is null!\n");
		return 0;
	}

	return module->m_pModule->GetFunctionCount();
}

CASFunction *CASModuleReflectionGroup::GetGlobalFunctionByIndex(unsigned int index)
{
	CASServerManager *manager = CASServerManager::GetInstance();
	CASModule *module = manager->m_pActiveModule;

	if (!module)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CReflection::GetGlobalFunctionByIndex: active module is null!\n");
		return NULL;
	}

	asIScriptFunction *result = module->m_pModule->GetFunctionByIndex(index);

	if (!result || !(result->GetAccessMask() & (1 << module->m_pModuleInfo->m_ModuleType)))
		return NULL;

	return new CASFunction(result, module);
}

CASObjectType *CASModuleReflectionGroup::FindObjectType(const CString &name, bool searchByDeclaration)
{
	CASServerManager *manager = CASServerManager::GetInstance();
	CASModule *module = manager->m_pActiveModule;

	if (!module)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CReflection::FindClassType: active module is null!\n");
		return NULL;
	}

	asITypeInfo *result = searchByDeclaration ? module->m_pModule->GetTypeInfoByDecl(name.Data()) : module->m_pModule->GetTypeInfoByName(name.Data());

	if (!result || !(result->GetAccessMask() & (1 << module->m_pModuleInfo->m_ModuleType)))
		return NULL;

	return new CASObjectType(result, module);
}

unsigned int CASModuleReflectionGroup::GetObjectTypeCount(void) const
{
	CASServerManager *manager = CASServerManager::GetInstance();
	CASModule *module = manager->m_pActiveModule;

	if (!module)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CReflection::GetObjectTypeCount: active module is null!\n");
		return 0;
	}

	return module->m_pModule->GetObjectTypeCount();
}

CASObjectType *CASModuleReflectionGroup::GetObjectTypeByIndex(unsigned int index)
{
	CASServerManager *manager = CASServerManager::GetInstance();
	CASModule *module = manager->m_pActiveModule;

	if (!module)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CReflection::GetObjectTypeByIndex: active module is null!\n");
		return NULL;
	}

	asITypeInfo *result = module->m_pModule->GetObjectTypeByIndex(index);

	if (!result || !(result->GetAccessMask() & (1 << module->m_pModuleInfo->m_ModuleType)))
		return NULL;

	return new CASObjectType(result, module);
}
