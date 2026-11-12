#include "extdll.h"
#include "enginecallback.h"
#include "util.h"
#include "scripting/api/ASString.h"
#include "scripting/core/CASServerManager.h"
#include "scripting/core/CASLog.h"
#include "CString.h"
#include "server/server_support.h"
#include <stdlib.h>
#include <string.h>

unsigned int UTIL_TrimFloatTrailingZeroes(char *text)
{
	if (!text)
		return 0;

	char *decimal = strrchr(text, '.');
	unsigned int length = strlen(text);

	if (decimal && length)
	{
		char *end = text + length - 1;

		while (*end == '0')
		{
			*end-- = 0;

			if (!length--)
				break;
		}

		if (*end == '.')
		{
			*end = 0;
			length--;
		}
	}

	return length;
}

bool ASSPrintf(char *destination, unsigned int size, const char *format,
               unsigned int firstArgument, asIScriptGeneric *generic)
{
	if (!destination || !size || !format || !generic || firstArgument > (unsigned int)generic->GetArgCount())
		return false;

	memset(destination, 0, size);

	if (!*format)
		return true;

	char *output = destination;
	char *end = destination + size;
	unsigned int count = generic->GetArgCount() - firstArgument;
	asIScriptEngine *engine = CASServerManager::GetInstance()->m_pEngine;

	for (const char *cursor = format; *cursor; cursor++)
	{
		if (*cursor != '%')
		{
			*output++ = *cursor;

			if (output == end)
			{
				CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "ASSPrintf: could not fit data in buffer\n");
				return false;
			}

			continue;
		}

		cursor++;

		if (*cursor == '%')
		{
			// The escaped-percent branch has no capacity check
			*output++ = '%';
			continue;
		}

		char *next;
		unsigned int index = strtoul(cursor, &next, 10);

		if (!index || index > count)
		{
			CASLog::GetInstance()->Warning(AS_LOG_DEFAULT, "ASSPrintf: format parameter index is out of range!\n");
			return false;
		}

		index += firstArgument - 1;

		int typeId = generic->GetArgTypeId(index);
		char buffer[256];
		void *argument = generic->GetArgAddress(index);

		buffer[0] = 0;

		switch (typeId)
		{
		case asTYPEID_VOID:
			UTIL_FormatString(buffer, sizeof(buffer), "%p", (void *)NULL);
			break;

		case asTYPEID_BOOL:
			UTIL_CopyString(buffer, *(bool *)argument ? "true" : "false", sizeof(buffer));
			break;

		case asTYPEID_INT8:
			UTIL_FormatString(buffer, sizeof(buffer), "%d", *(signed char *)argument);
			break;

		case asTYPEID_INT16:
			UTIL_FormatString(buffer, sizeof(buffer), "%d", *(short *)argument);
			break;

		case asTYPEID_INT32:
			UTIL_FormatString(buffer, sizeof(buffer), "%d", *(int *)argument);
			break;

		case asTYPEID_INT64:
			UTIL_FormatString(buffer, sizeof(buffer), "%lld", *(long long *)argument);
			break;

		case asTYPEID_UINT8:
			UTIL_FormatString(buffer, sizeof(buffer), "%u", *(unsigned char *)argument);
			break;

		case asTYPEID_UINT16:
			UTIL_FormatString(buffer, sizeof(buffer), "%u", *(unsigned short *)argument);
			break;

		case asTYPEID_UINT32:
			UTIL_FormatString(buffer, sizeof(buffer), "%u", *(unsigned int *)argument);
			break;

		case asTYPEID_UINT64:
			UTIL_FormatString(buffer, sizeof(buffer), "%llu", *(unsigned long long *)argument);
			break;

		case asTYPEID_FLOAT:
		case asTYPEID_DOUBLE:
			UTIL_FormatString(buffer, sizeof(buffer), "%f", typeId == asTYPEID_FLOAT ? *(float *)argument : *(double *)argument);
			UTIL_TrimFloatTrailingZeroes(buffer);
			break;

		default:
		{
			asITypeInfo *type = engine->GetTypeInfoById(typeId);

			if (type)
			{
				const char *string = NULL;
				unsigned int length = 0;

				if (!strcmp(type->GetName(), "string"))
				{
					string = ((CString *)argument)->Data();
					length = ((CString *)argument)->Length();
				}
				else if (!strcmp(type->GetName(), "string_t"))
				{
					string = STRING(*(string_t *)argument);
					length = strlen(string);
				}

				if (string)
				{
					if (length >= (unsigned int)(end - output))
					{
						CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "ASSPrintf: could not fit data in buffer\n");
						return false;
					}

					UTIL_AppendString(output, string, end - output);
					output += length;
				}
				else
					UTIL_FormatString(buffer, sizeof(buffer), "%p", argument);
			}
			else if (argument)
				UTIL_FormatString(buffer, sizeof(buffer), "%d", *(int *)argument);
			else
				UTIL_FormatString(buffer, sizeof(buffer), "%p", (void *)NULL);
		}
		break;
		}

		unsigned int length = strlen(buffer);

		if (length >= (unsigned int)(end - output))
		{
			CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "ASSPrintf: could not fit data in buffer\n");
			return false;
		}

		UTIL_AppendString(output, buffer, end - output);
		output += length;
		cursor = next - 1;
	}

	*output = 0;
	return true;
}
