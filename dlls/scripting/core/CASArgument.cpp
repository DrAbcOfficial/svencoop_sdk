#include "scripting/core/CASArgument.h"
#include "scripting/core/CASServerManager.h"
#include "scripting/core/CASLog.h"

static const int ARGUMENT_INVALID_TYPE_ID = -1;

CASArgument::CASArgument()
	: m_iTypeId(ARGUMENT_INVALID_TYPE_ID), m_ArgumentType(ARGUMENT_NONE)
{
	m_Value.qwordValue = 0;
}

CASArgument::CASArgument(const CASArgument &other)
	: m_iTypeId(ARGUMENT_INVALID_TYPE_ID), m_ArgumentType(ARGUMENT_NONE)
{
	m_Value.qwordValue = 0;

	Set(other);
}

CASArgument::~CASArgument()
{
	Reset();
}

CASArgument &CASArgument::operator=(const CASArgument &other)
{
	if (this != &other)
		Set(other);

	return *this;
}

void CASArgument::Reset(void)
{
	if (m_ArgumentType & (ARGUMENT_NONE | ARGUMENT_VOID))
		return;

	if (!(m_iTypeId >= asTYPEID_BOOL && m_iTypeId <= asTYPEID_DOUBLE) &&
	    (m_iTypeId <= asTYPEID_DOUBLE || (m_iTypeId & asTYPEID_MASK_OBJECT)))
	{
		asIScriptEngine *engine = CASServerManager::GetInstance()->m_pEngine;
		asITypeInfo *type = engine->GetTypeInfoById(m_iTypeId);

		if (type)
			engine->ReleaseScriptObject(m_Value.pointerValue, type);
		else
			CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
			                             "CASArgument::Reset: failed to get object type!\n");
	}

	m_iTypeId = ARGUMENT_INVALID_TYPE_ID;
	m_ArgumentType = ARGUMENT_NONE;
	m_Value.qwordValue = 0;
}

bool CASArgument::Set(int typeId, ArgumentType_t argumentType, const ArgumentValue_t &value, bool copyObject)
{
	Reset();

	if (argumentType == ARGUMENT_NONE)
		return true;

	m_iTypeId = typeId;
	m_ArgumentType = argumentType;

	if (copyObject && (argumentType == ARGUMENT_OBJECT || argumentType == ARGUMENT_HANDLE))
	{
		asIScriptEngine *engine = CASServerManager::GetInstance()->m_pEngine;
		asITypeInfo *type = engine->GetTypeInfoById(typeId);

		if (!type)
		{
			CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
			                             "CASArgument::Set: failed to get object type!\n");
			return false;
		}

		if (argumentType == ARGUMENT_OBJECT)
			m_Value.pointerValue = engine->CreateScriptObjectCopy(value.pointerValue, type);
		else
		{
			engine->AddRefScriptObject(value.pointerValue, type);
			m_Value = value;
		}
	}
	else if (argumentType != ARGUMENT_VOID)
		m_Value = value;

	return true;
}

bool CASArgument::Set(const CASArgument &other)
{
	return Set(other.m_iTypeId, other.m_ArgumentType, other.m_Value, true);
}

void *CASArgument::GetArgumentAsPointer(void) const
{
	if (m_ArgumentType & (ARGUMENT_NONE | ARGUMENT_VOID))
		return NULL;

	switch (m_ArgumentType)
	{
	case ARGUMENT_PRIMITIVE:
	case ARGUMENT_ENUM:
		return (void *)&m_Value;

	case ARGUMENT_OBJECT:
	case ARGUMENT_HANDLE:
		return m_Value.pointerValue;

	default:
		return NULL;
	}
}