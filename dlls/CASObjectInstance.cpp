#include "extdll.h"
#include "CASObjectInstance.h"
#include "CASMethod.h"
#include "CASServerManager.h"
#include "CASLog.h"
#include <angelscript.h>

CASObjectInstance::CASObjectInstance(asITypeInfo *type, void *object, CASModule *module)
    : CASBaseObjectData(type, module), m_pObject(object)
{
}

CASObjectInstance::~CASObjectInstance()
{
	FreeInstance();
}

void CASObjectInstance::Release(void) const
{
	if (InternalRelease())
		delete this;
}

void CASObjectInstance::FreeInstance(void)
{
	if (!m_pObject)
		return;

	CASServerManager::GetInstance()->m_pEngine->ReleaseScriptObject(m_pObject, m_pTypeInfo);
	m_pObject = NULL;
}

bool CASObjectInstance::CallMethod(void *returnValue, bool optional, const char *declaration, int argumentCount, va_list arguments)
{
	CASMethod *method = GetMethod(declaration);

	if (method)
	{
		method->SetThisPointer(m_pObject);
		return method->Call(returnValue, argumentCount, arguments);
	}

	if (!optional)
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
		                             "CASObjectInstance::CallMethod: method '%s' does not exist!\n", declaration);

	return false;
}

bool CASObjectInstance::CallMethod(void *returnValue, bool optional, const char *declaration, int argumentCount, ...)
{
	va_list arguments;

	va_start(arguments, argumentCount);
	bool success = CallMethod(returnValue, optional, declaration, argumentCount, arguments);
	va_end(arguments);

	return success;
}

bool CASObjectInstance::CallMethod(void *returnValue, const char *declaration, int argumentCount, va_list arguments)
{
	return CallMethod(returnValue, false, declaration, argumentCount, arguments);
}

bool CASObjectInstance::CallMethod(void *returnValue, const char *declaration, int argumentCount, ...)
{
	va_list arguments;

	va_start(arguments, argumentCount);
	bool success = CallMethod(returnValue, false, declaration, argumentCount, arguments);
	va_end(arguments);

	return success;
}

bool CASObjectInstance::CallMethod(bool optional, const char *declaration, int argumentCount, va_list arguments)
{
	CASMethod *method = GetMethod(declaration);

	if (method)
	{
		method->SetThisPointer(m_pObject);
		return method->Call(argumentCount, arguments);
	}

	if (!optional)
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
		                             "CASObjectInstance::CallMethod: method '%s' does not exist!\n", declaration);

	return false;
}

bool CASObjectInstance::CallMethod(bool optional, const char *declaration, int argumentCount, ...)
{
	va_list arguments;

	va_start(arguments, argumentCount);
	bool success = CallMethod(optional, declaration, argumentCount, arguments);
	va_end(arguments);

	return success;
}

bool CASObjectInstance::CallMethod(const char *declaration, int argumentCount, va_list arguments)
{
	return CallMethod(false, declaration, argumentCount, arguments);
}

bool CASObjectInstance::CallMethod(const char *declaration, int argumentCount, ...)
{
	va_list arguments;

	va_start(arguments, argumentCount);
	bool success = CallMethod(false, declaration, argumentCount, arguments);
	va_end(arguments);

	return success;
}
