#include "extdll.h"
#include "scripting/core/CASObjectType.h"
#include "scripting/core/CASObjectInstance.h"
#include "scripting/core/CASModule.h"
#include "scripting/core/CASServerManager.h"
#include <angelscript.h>

CASObjectType::CASObjectType(asITypeInfo *type, CASModule *module)
    : CASBaseObjectData(type, module)
{
}

CASObjectType::~CASObjectType()
{
}

void CASObjectType::Release(void) const
{
	if (InternalRelease())
		delete this;
}

bool CASObjectType::Equals(const CASObjectType *type) const
{
	return type && m_pTypeInfo == type->m_pTypeInfo;
}

bool CASObjectType::IsCompatible(const CASObjectType *type) const
{
	return type && (type == this || m_pTypeInfo->DerivesFrom(type->m_pTypeInfo)
	                || type->m_pTypeInfo->DerivesFrom(m_pTypeInfo));
}

bool CASObjectType::DerivesFrom(const char *name) const
{
	if (!name || !*name)
		return false;

	CASObjectType *type;

	if (m_pModule)
		type = m_pModule->GetObjectType(name);
	else
	{
		CASServerManager *manager = CASServerManager::GetInstance();
		asITypeInfo *typeInfo = manager->GetTypeInfoByName(name);

		if (!typeInfo)
			return false;

		type = new CASObjectType(typeInfo, NULL);
	}

	if (!type)
		return false;

	bool derives = m_pTypeInfo->DerivesFrom(type->m_pTypeInfo);

	type->Release();
	return derives;
}

bool CASObjectType::DerivesFrom(const CASObjectType *type) const
{
	return type && (type == this || m_pTypeInfo->DerivesFrom(type->m_pTypeInfo));
}

bool CASObjectType::HasDefaultConstructor(void) const
{
	for (unsigned int index = 0; index < m_pTypeInfo->GetFactoryCount(); index++)
	{
		if (!m_pTypeInfo->GetFactoryByIndex(index)->GetParamCount())
			return true;
	}

	return false;
}

CASObjectInstance *CASObjectType::CreateInstance(void)
{
	if (!HasDefaultConstructor())
		return NULL;

	return new CASObjectInstance(m_pTypeInfo,
	                             CASServerManager::GetInstance()->m_pEngine->CreateScriptObject(m_pTypeInfo), m_pModule);
}
