#include "extdll.h"
#include "server_support.h"
#include "entvars_types.h"
#include <ctype.h>

const char *EntVarStr(entvartype_s type)
{
	switch (type)
	{
	case VAR_INVALID:
		return "INVALID";
	case VAR_STRING:
		return "STRING";
	case VAR_VECTOR:
		return "VECTOR";
	case VAR_FLOAT:
		return "FLOAT";
	case VAR_INTEGER:
		return "INTEGER";
	case VAR_EDICT:
		return "EDICT";
	case VAR_BYTE4:
		return "BYTE[4]";
	case VAR_BYTE2:
		return "BYTE[2]";
	case VAR_CUSTOM:
		return "CUSTOM";
	default:
		return "UNDEFINED";
	}
}

bool UTIL_IsStringInt(const char *text)
{
	int length = strlen(text);
	int position = text[0] == '-' || text[0] == '+' ? 1 : 0;

	for (; position < length; position++)
	{
		if (!isalnum((unsigned char)text[position]) || isalpha((unsigned char)text[position]))
			return false;
	}

	return length > 0;
}

bool UTIL_IsStringFloat(const char *text)
{
	int length = strlen(text);
	int position = text[0] == '-' || text[0] == '+' ? 1 : 0;
	bool hasPoint = false;

	for (; position < length; position++)
	{
		if (!isalnum((unsigned char)text[position]) || isalpha((unsigned char)text[position]))
		{
			if (hasPoint || text[position] != '.')
				return false;

			hasPoint = true;
		}
	}

	return hasPoint;
}

bool UTIL_IsString3DVec(const char *text)
{
	int length = strlen(text);
	int components = 0;
	bool hasSign = false;
	bool hasPoint = false;
	bool hasDigit = false;

	for (int position = 0; position < length; position++)
	{
		if (!isalnum((unsigned char)text[position]) || isalpha((unsigned char)text[position]))
		{
			switch (text[position])
			{
			case '-':
				if (hasSign)
					return false;

				hasSign = true;
				break;

			case '.':
				if (hasPoint)
					return false;

				hasPoint = true;
				break;

			case ' ':
				if (hasDigit)
				{
					components++;

					if (components > ENTVAR_VECTOR_COMPONENTS)
						return false;

					hasSign = false;
					hasDigit = false;
					hasPoint = false;
				}
				else if (hasSign && hasPoint)
				{
					return false;
				}
				break;

			default:
				return false;
			}
		}
		else
		{
			hasDigit = true;
		}
	}

	if (!hasDigit)
		return false;

	int count = components + 1;

	if (text[length - 1] == ' ')
		count = components;

	return count == ENTVAR_VECTOR_COMPONENTS;
}

entvartype_s UTIL_FigureVarType(const char *text)
{
	if (UTIL_IsStringInt(text))
		return VAR_INTEGER;

	if (UTIL_IsStringFloat(text))
		return VAR_FLOAT;

	if (UTIL_IsString3DVec(text))
		return VAR_VECTOR;

	return VAR_STRING;
}

bool UTIL_StringToVectorND(float *values, int count, const char *text, char separator)
{
	char buffer[MAX_ENTVAR_VALUE_TEXT];

	UTIL_CopyString(buffer, text, sizeof(buffer));
	buffer[sizeof(buffer) - 1] = 0;

	char *value = buffer;
	char *cursor = buffer;
	int component = 0;

	if (count > 0)
	{
		do
		{
			values[component] = (float)atof(value);

			char character = *cursor;

			if (!character)
				break;

			for (;;)
			{
				cursor++;

				if (character == separator)
					break;

				character = *cursor;

				if (!character)
					goto parsed;
			}

			component++;
			value = cursor;
		}
		while (component < count);
	}

parsed:
	bool complete = component == count - 1;

	for (component++; component < count; component++)
		values[component] = 0;

	return complete;
}

bool UTIL_StringToVector(float *values, const char *text, char separator)
{
	return UTIL_StringToVectorND(values, ENTVAR_VECTOR_COMPONENTS, text, separator);
}
