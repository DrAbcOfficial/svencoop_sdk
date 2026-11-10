#include "extdll.h"
#include "util.h"
#include "CCustomKeyvalue.h"

CCustomKeyvalue::CCustomKeyvalue()
{
	m_pValue = NULL;
	m_Type = VAR_INVALID;
}

CCustomKeyvalue::CCustomKeyvalue(const void *value, entvartype_s type)
{
	m_pValue = value;
	m_Type = type;
}

bool CCustomKeyvalue::Exists(void) const
{
	return m_pValue != NULL;
}

entvartype_s CCustomKeyvalue::GetType(void) const
{
	return m_Type;
}

CString CCustomKeyvalue::GetString(void) const
{
	if (!m_pValue)
		return CString("");

	char text[MAX_CUSTOM_KEYVALUE_TEXT];

	switch (m_Type)
	{
	case VAR_STRING:
		return CString(STRING(*(const string_t *)m_pValue));

	case VAR_VECTOR:
		{
			const Vector &value = *(const Vector *)m_pValue;

			snprintf(text, sizeof(text), "%f %f %f", value.x, value.y, value.z);
			break;
		}

	case VAR_FLOAT:
		snprintf(text, sizeof(text), "%f", *(const float *)m_pValue);
		break;

	case VAR_INTEGER:
		snprintf(text, sizeof(text), "%i", *(const int *)m_pValue);
		break;

	default:
		return CString("");
	}

	text[sizeof(text) - 1] = 0;
	return CString(text);
}

Vector CCustomKeyvalue::GetVector(void) const
{
	if (!m_pValue)
		return Vector(0, 0, 0);

	switch (m_Type)
	{
	case VAR_STRING:
		{
			Vector value(0, 0, 0);

			UTIL_StringToVector(value, STRING(*(const string_t *)m_pValue), ' ');
			return value;
		}

	case VAR_VECTOR:
		return *(const Vector *)m_pValue;

	default:
		return Vector(0, 0, 0);
	}
}

float CCustomKeyvalue::GetFloat(void) const
{
	if (!m_pValue)
		return 0;

	switch (m_Type)
	{
	case VAR_STRING:
		return atof(STRING(*(const string_t *)m_pValue));

	case VAR_VECTOR:
		return ((const Vector *)m_pValue)->Length();

	case VAR_FLOAT:
		return *(const float *)m_pValue;

	case VAR_INTEGER:
		return *(const int *)m_pValue;

	default:
		return 0;
	}
}

int CCustomKeyvalue::GetInteger(void) const
{
	if (!m_pValue)
		return 0;

	switch (m_Type)
	{
	case VAR_STRING:
		return atoi(STRING(*(const string_t *)m_pValue));

	case VAR_VECTOR:
		return (int)((const Vector *)m_pValue)->Length();

	case VAR_FLOAT:
		return (int)*(const float *)m_pValue;

	case VAR_INTEGER:
		return *(const int *)m_pValue;

	default:
		return 0;
	}
}
