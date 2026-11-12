#include "scripting/core/CASBaseCallable.h"
#include "scripting/core/CASModule.h"
#include "scripting/core/CASLog.h"
#include "scripting/core/CASServerManager.h"
#include "scripting/core/CASReflection.h"
#include "scripting/core/CASReturnValue.h"
#include "server/server_support.h"
#include <string.h>

static const unsigned int FUNCTION_FORMAT_RESERVED_SPACE = 2;
static const unsigned int MAX_CALL_RECURSION = 16;

unsigned int CASBaseCallable::m_uiInCallCount;

CASBaseCallable::CASBaseCallable(asIScriptFunction *function, CASModule *module)
	: m_pFunction(function), m_pModule(module), m_pContext(NULL)
{
	m_pFunction->AddRef();

	if (m_pModule)
		m_pModule->AddRef();
}

CASBaseCallable::~CASBaseCallable()
{
	if (m_pContext)
		ReleaseContext(NULL);

	if (m_pFunction)
		m_pFunction->Release();

	m_pFunction = NULL;

	if (m_pModule)
		m_pModule->Release();

	m_pModule = NULL;
}

unsigned int CASBaseCallable::ComputeReservedSpace(const char *returnType, const char *arguments)
{
	return (unsigned int)(strlen(returnType) + strlen(arguments)) + FUNCTION_FORMAT_RESERVED_SPACE;
}

bool CASBaseCallable::FormatFunction(char *buffer, unsigned int size, const char *returnType,
	const char *name, const char *arguments)
{
	if (!buffer)
		return false;

	if (strlen(name) + ComputeReservedSpace(returnType, arguments) > size)
		return false;

	int length = UTIL_FormatString(buffer, size, "%s %s(%s)", returnType, name, arguments);

	return length >= 0 && (unsigned int)length < size;
}

void CASBaseCallable::Release(void) const
{
	if (InternalRelease())
		delete this;
}

asIScriptFunction *CASBaseCallable::GetFunction(void) const
{
	return m_pFunction;
}

CASModule *CASBaseCallable::GetModule(void) const
{
	return m_pModule;
}

asIScriptContext *CASBaseCallable::GetContext(void) const
{
	return m_pContext;
}

bool CASBaseCallable::Equals(const CASBaseCallable *other) const
{
	return other && m_pFunction == other->m_pFunction;
}

bool CASBaseCallable::IsCompatible(const CASBaseCallable *other) const
{
	return other && m_pFunction->IsCompatibleWithTypeId(other->m_pFunction->GetTypeId());
}

void CASBaseCallable::Prepare(void)
{
	if (!m_pContext)
		return;

	int result = m_pContext->Prepare(m_pFunction);

	if (result < 0)
		CASLog::GetInstance()->Warning(AS_LOG_DEFAULT,
			"CASBaseCallable::Prepare: error code %d received while attempting to prepare for execution\n", result);
}

void CASBaseCallable::Unprepare(void)
{
	if (!m_pContext)
		return;

	int result = m_pContext->Unprepare();

	if (result < 0)
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
			"CASBaseCallable::Unprepare: error while unpreparing, got error code '%d'!\n", result);
}

asIScriptContext *CASBaseCallable::AcquireContext(void)
{
	asIScriptContext *previousContext = m_pContext;

	m_pContext = CASServerManager::GetInstance()->m_pEngine->RequestContext();
	m_pContext->AddRef();

	return previousContext;
}

void CASBaseCallable::ReleaseContext(asIScriptContext *previousContext)
{
	m_pContext->Release();
	CASServerManager::GetInstance()->m_pEngine->ReturnContext(m_pContext);
	m_pContext = previousContext;
}

bool CASBaseCallable::Execute(bool unprepare)
{
	if (!m_pContext)
		return false;

	bool success = false;

	if (m_pModule && m_pModule->m_bValid)
	{
		if (m_uiInCallCount >= MAX_CALL_RECURSION)
		{
			CASLog::GetInstance()->Warning(AS_LOG_DEFAULT,
				"CASBaseCallable::Execute: maximum recursion count '%u' reached!\n", MAX_CALL_RECURSION);
		}
		else
		{
			CASServerManager::GetInstance()->Update();

			if (PreExecute())
			{
				CASModule *previousModule = CASServerManager::GetInstance()->m_pActiveModule;

				CASServerManager::GetInstance()->m_pActiveModule = m_pModule;
				m_uiInCallCount++;

				int result = m_pContext->Execute();

				m_uiInCallCount--;
				PostExecute();

				switch (result)
				{
					case asEXECUTION_FINISHED:
						success = true;
						break;

					case asCONTEXT_NOT_PREPARED:
						CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "Context not prepared for execution\n");
						break;

					case asEXECUTION_SUSPENDED:
						CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "Script execution suspended\n");
						break;

					case asEXECUTION_ABORTED:
						CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "Script execution aborted\n");
						break;

					case asEXECUTION_EXCEPTION:
					{
						int column;
						const char *section;
						int line = m_pContext->GetExceptionLineNumber(&column, &section);

						CASLog::GetInstance()->Log(AS_LOG_DEFAULT,
							"Not allowed: in section '%s', at (%d, %d):", section ? section : "unknown section", line, column);

						asIScriptFunction *function = m_pContext->GetExceptionFunction();

						if (function)
						{
							CASLog::GetInstance()->Log(AS_LOG_DEFAULT, " Function ");

							const char *nameSpace = function->GetNamespace();

							if (nameSpace && *nameSpace)
								CASLog::GetInstance()->Log(AS_LOG_DEFAULT, "%s::%s:\n", function->GetNamespace(), function->GetName());
							else
								CASLog::GetInstance()->Log(AS_LOG_DEFAULT, "%s:\n", function->GetName());
						}

						CASLog::GetInstance()->Log(AS_LOG_DEFAULT, "Message: %s\n", m_pContext->GetExceptionString());
						break;
					}

					default:
					{
						const char *state;

						switch (result)
						{
							case asEXECUTION_PREPARED:
								state = "Prepared";
								break;

							case asEXECUTION_UNINITIALIZED:
								state = "Uninitialized";
								break;

							case asEXECUTION_ACTIVE:
								state = "Active";
								break;

							case asEXECUTION_ERROR:
								state = "Error";
								break;

							default:
								state = "Unknown";
								break;
						}

						CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "An unknown error '%s' occured\n", state);
						break;
					}
				}

				CASServerManager::GetInstance()->m_pActiveModule = previousModule;
			}
		}
	}

	if (unprepare)
		Unprepare();

	return success;
}

bool CASBaseCallable::SetArgByte(unsigned int index, char value)
{
	return m_pContext->SetArgByte(index, value) >= 0;
}

bool CASBaseCallable::SetArgWord(unsigned int index, short value)
{
	return m_pContext->SetArgWord(index, value) >= 0;
}

bool CASBaseCallable::SetArgDWord(unsigned int index, long value)
{
	return m_pContext->SetArgDWord(index, value) >= 0;
}

bool CASBaseCallable::SetArgQWord(unsigned int index, long long value)
{
	return m_pContext->SetArgQWord(index, value) >= 0;
}

bool CASBaseCallable::SetArgFloat(unsigned int index, float value)
{
	return m_pContext->SetArgFloat(index, value) >= 0;
}

bool CASBaseCallable::SetArgDouble(unsigned int index, double value)
{
	return m_pContext->SetArgDouble(index, value) >= 0;
}

bool CASBaseCallable::SetArgAddress(unsigned int index, void *value)
{
	return m_pContext->SetArgAddress(index, value) >= 0;
}

bool CASBaseCallable::SetArgObject(unsigned int index, void *value)
{
	return m_pContext->SetArgObject(index, value) >= 0;
}

void *CASBaseCallable::GetAddressofArg(unsigned int index)
{
	return m_pContext->GetAddressOfArg(index);
}

asBYTE CASBaseCallable::GetReturnByte(void)
{
	return m_pContext->GetReturnByte();
}

asWORD CASBaseCallable::GetReturnWord(void)
{
	return m_pContext->GetReturnWord();
}

asDWORD CASBaseCallable::GetReturnDWord(void)
{
	return m_pContext->GetReturnDWord();
}

asQWORD CASBaseCallable::GetReturnQWord(void)
{
	return m_pContext->GetReturnQWord();
}

float CASBaseCallable::GetReturnFloat(void)
{
	return m_pContext->GetReturnFloat();
}

double CASBaseCallable::GetReturnDouble(void)
{
	return m_pContext->GetReturnDouble();
}

void *CASBaseCallable::GetReturnAddress(void)
{
	return m_pContext->GetReturnAddress();
}

void *CASBaseCallable::GetReturnObject(void)
{
	return m_pContext->GetReturnObject();
}

bool CASBaseCallable::CallInternal(bool unprepare, int argumentCount, va_list arguments)
{
	Prepare();

	if (CASReflection::SetArguments(m_pFunction, m_pContext, arguments))
	{
		bool success = Execute(unprepare);

		if (!success)
			CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
				"CASBaseCallable::Call: Execution of function '%s::%s' failed!\n", m_pFunction->GetNamespace(), m_pFunction->GetName());

		return success;
	}

	CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
		"CASBaseCallable::Call: Failed to set parameters for function call to '%s::%s'!\n", m_pFunction->GetNamespace(), m_pFunction->GetName());
	return false;
}

bool CASBaseCallable::CallInternal(bool unprepare, CASArguments &arguments)
{
	Prepare();

	if (CASReflection::SetArguments(m_pFunction, m_pContext, arguments))
	{
		bool success = Execute(unprepare);

		if (!success)
			CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
				"CASBaseCallable::Call: Execution of function '%s::%s' failed!\n", m_pFunction->GetNamespace(), m_pFunction->GetName());

		return success;
	}

	CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
		"CASBaseCallable::Call: Failed to set parameters for function call to '%s::%s'!\n", m_pFunction->GetNamespace(), m_pFunction->GetName());
	return false;
}

bool CASBaseCallable::Call(void *returnValue, int argumentCount, va_list arguments)
{
	asIScriptContext *previousContext = AcquireContext();
	bool success = CallInternal(false, argumentCount, arguments);

	if (success)
	{
		asDWORD flags;
		int typeId = m_pFunction->GetReturnTypeId(&flags);

		success = CASReflection::GetReturnValue(returnValue, typeId, flags, m_pContext, m_pModule);
	}

	Unprepare();
	ReleaseContext(previousContext);

	return success;
}

bool CASBaseCallable::Call(int argumentCount, va_list arguments)
{
	asIScriptContext *previousContext = AcquireContext();
	bool success = CallInternal(true, argumentCount, arguments);

	ReleaseContext(previousContext);

	return success;
}

bool CASBaseCallable::Call(void *returnValue, int argumentCount, ...)
{
	va_list arguments;

	va_start(arguments, argumentCount);
	bool success = Call(returnValue, argumentCount, arguments);
	va_end(arguments);

	return success;
}

bool CASBaseCallable::Call(int argumentCount, ...)
{
	va_list arguments;

	va_start(arguments, argumentCount);
	bool success = Call(argumentCount, arguments);
	va_end(arguments);

	return success;
}

bool CASBaseCallable::Call(CASArguments &arguments, CASReturnValue *returnValue)
{
	asIScriptContext *previousContext = AcquireContext();
	bool success = CallInternal(false, arguments);

	if (success && returnValue)
	{
		asDWORD flags;

		success = CASReflection::GetReturnValue(returnValue->m_ReturnValue, m_pFunction, m_pContext, &flags);

		if (success)
			returnValue->m_uiFlags = flags;
		else
			CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CASBaseCallable::Call: Could not get return value!\n");
	}

	Unprepare();
	ReleaseContext(previousContext);

	return success;
}

CASReturnValue *CASBaseCallable::Call(CASArguments &arguments, bool unused)
{
	CASReturnValue *returnValue = new CASReturnValue;

	if (!Call(arguments, returnValue))
	{
		delete returnValue;
		return NULL;
	}

	return returnValue;
}

bool CASBaseCallable::PreExecute(void)
{
	return true;
}

void CASBaseCallable::PostExecute(void)
{
}
