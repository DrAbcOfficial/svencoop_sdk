#include "CASArguments.h"
#include "CASServerManager.h"
#include "CASReflection.h"
#include "CASLog.h"

CASArguments::CASArguments()
	: m_pArguments(NULL), m_uiCount(0)
{
}

CASArguments::CASArguments(const CASArguments &other)
	: m_pArguments(NULL), m_uiCount(0)
{
	Assign(other);
}

CASArguments::CASArguments(asIScriptGeneric *generic, unsigned int firstArgument)
	: m_pArguments(NULL), m_uiCount(0)
{
	SetArguments(generic, firstArgument);
}

CASArguments::CASArguments(asIScriptFunction *function, va_list arguments)
	: m_pArguments(NULL), m_uiCount(0)
{
	SetArguments(function, arguments);
}

CASArguments::~CASArguments()
{
	Clear();
}

void CASArguments::Release(void) const
{
	if (InternalRelease())
		delete this;
}

unsigned int CASArguments::GetCount(void) const
{
	return m_uiCount;
}

bool CASArguments::HasArguments(void) const
{
	return m_uiCount != 0;
}

void CASArguments::Assign(const CASArguments &other)
{
	if (this == &other)
		return;

	Clear();

	m_uiCount = other.m_uiCount;

	if (!m_uiCount)
		return;

	m_pArguments = new CASArgument[m_uiCount];

	const CASArgument *sourceArguments = other.m_pArguments;
	asIScriptEngine *engine = CASServerManager::GetInstance()->m_pEngine;

	for (unsigned int index = 0; index < m_uiCount; index++)
	{
		const CASArgument &source = sourceArguments[index];
		bool isPrimitive = false;
		bool result = CASReflection::SetPrimitiveArgument(m_pArguments[index], source.m_iTypeId,
		                                                  (void *)&source.m_Value, isPrimitive);

		if (!result && !isPrimitive)
			result = CASReflection::SetObjectArgument(engine, m_pArguments[index],
			                                          source.m_iTypeId, (void *)&source.m_Value);

		// The copy path clears successful conversions
		if (result)
		{
			Clear();
			return;
		}
	}
}

CASArguments &CASArguments::operator=(const CASArguments &other)
{
	Assign(other);

	return *this;
}

void CASArguments::Clear(void)
{
	if (!m_pArguments)
		return;

	delete[] m_pArguments;
	m_pArguments = NULL;
	m_uiCount = 0;
}

bool CASArguments::SetArguments(asIScriptGeneric *generic, unsigned int firstArgument)
{
	if (!generic)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
		                             "CASArguments::SetArguments: null arguments pointer!\n");
		return false;
	}

	unsigned int count = generic->GetArgCount();

	if (firstArgument > count)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
		                             "CASArguments::SetArguments: start index is greater than argument count!\n");
		return false;
	}

	count -= firstArgument;

	CASArgument *arguments = count ? new CASArgument[count] : NULL;
	asIScriptEngine *engine = CASServerManager::GetInstance()->m_pEngine;
	bool success = true;

	for (unsigned int index = 0; index < count && success; index++)
	{
		int typeId = generic->GetArgTypeId(firstArgument + index);
		void *argument = generic->GetArgAddress(firstArgument + index);

		success = CASReflection::SetArgument(engine, arguments[index], typeId, argument);
	}

	if (success)
	{
		// Generic capture replaces the array without freeing prior arguments
		m_pArguments = arguments;
		m_uiCount = count;
	}
	else
		delete[] arguments;

	return success;
}

bool CASArguments::SetArguments(asIScriptFunction *function, va_list arguments)
{
	// va_list is a pointer type on the x86 ABIs, but a struct on AArch64,
	// where it cannot be null-checked.
#if defined( __GNUC__ ) && (defined( __i386__ ) || defined( __x86_64__ ))
	if (!function || !arguments)
#else
	if (!function)
#endif
		return false;

	Clear();

	unsigned int count = function->GetParamCount();
	CASArgument *captured = count ? new CASArgument[count] : NULL;
	asIScriptEngine *engine = CASServerManager::GetInstance()->m_pEngine;
	ArgumentValue_t value;
	bool success = true;

	value.qwordValue = 0;

	for (unsigned int index = 0; index < count && success; index++)
	{
		int typeId;
		asDWORD flags;

		if (function->GetParam(index, &typeId, &flags) < 0)
		{
			CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
			                             "CASArguments::SetArguments(va_list): an error occurred while getting function parameter information, aborting!\n");
			success = false;
			break;
		}

		// Capture queries object flags even for primitive parameter types
		asDWORD objectFlags = engine->GetTypeInfoById(typeId)->GetFlags();
		ArgumentType_t argumentType;

		success = CASReflection::GetArgumentFromVarargs(value, typeId, flags, arguments,
		                                                &objectFlags, &argumentType);

		if (success)
			captured[index].Set(typeId, argumentType, value, true);
		else
		{
			const char *functionName = function->GetName();
			const char *nameSpace = function->GetNamespace();

			CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
			                             "CASArguments::SetArguments(va_list): Function '%s::s': failed to set argument %u, aborting!\n",
			                             nameSpace, functionName);
		}
	}

	if (success)
	{
		m_pArguments = captured;
		m_uiCount = count;
	}
	else
		delete[] captured;

	return success;
}