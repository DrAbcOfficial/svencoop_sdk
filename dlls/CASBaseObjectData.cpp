#include "CASBaseObjectData.h"
#include "CASMethod.h"
#include "CASModule.h"
#include "CASLog.h"
#include <angelscript.h>

CASBaseObjectData::CASBaseObjectData(asITypeInfo *type, CASModule *module)
    : m_pModule(module), m_pTypeInfo(type)
{
	m_pTypeInfo->AddRef();

	if (m_pModule)
		m_pModule->AddRef();
}

CASBaseObjectData::~CASBaseObjectData()
{
	ClearCache();

	if (m_pModule)
		m_pModule->Release();

	m_pTypeInfo->Release();
}

unsigned int CASBaseObjectData::GetMethodCount(void) const
{
	return m_pTypeInfo->GetMethodCount();
}

CASMethod *CASBaseObjectData::GetMethodByIndex(unsigned int index) const
{
	asIScriptFunction *function = m_pTypeInfo->GetMethodByIndex(index);

	if (!function)
		return NULL;

	return new CASMethod(function, m_pModule);
}

bool CASBaseObjectData::MethodExists(const char *declaration)
{
	return declaration && m_pTypeInfo->GetMethodByDecl(declaration) != NULL;
}

CASMethod *CASBaseObjectData::GetMethod(const char *declaration)
{
	if (!declaration)
		return NULL;

	std::map<std::string, CASMethod *>::iterator entry = m_MethodCache.find(declaration);

	if (entry != m_MethodCache.end())
		return entry->second;

	asIScriptFunction *function = m_pTypeInfo->GetMethodByDecl(declaration);
	CASMethod *method = function ? new CASMethod(function, m_pModule) : NULL;

	std::pair<std::map<std::string, CASMethod *>::iterator, bool> result =
	    m_MethodCache.insert(std::make_pair(declaration, method));

	if (result.first != m_MethodCache.end())
		return method;

	CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
	                             "BaseObjectData::GetMethod: failed to insert method '%s' into method cache!\n", declaration);
	method->Release();
	return NULL;
}

void CASBaseObjectData::ClearCache(void)
{
	for (std::map<std::string, CASMethod *>::iterator entry = m_MethodCache.begin();
	     entry != m_MethodCache.end(); entry++)
	{
		if (entry->second)
			entry->second->Release();
	}

	m_MethodCache.clear();
}
