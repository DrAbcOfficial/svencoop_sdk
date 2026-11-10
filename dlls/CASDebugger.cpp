#include "extdll.h"
#include "util.h"
#include "CASDebugger.h"
#include "CASServerManager.h"
#include <sstream>

static const unsigned int DEBUGGER_OUTPUT_CHUNK_SIZE = 512;

CASDebugger::CASDebugger()
{
}

CASDebugger::~CASDebugger()
{
}

void CASDebugger::TakeCommands(asIScriptContext *context)
{
}

void CASDebugger::Output(const std::string &text)
{
	for (unsigned int offset = 0; offset < text.size(); offset += DEBUGGER_OUTPUT_CHUNK_SIZE)
	{
		void (*alert)(ALERT_TYPE, char *, ...) = g_engfuncs.pfnAlertMessage;
		std::string chunk = text.substr(offset, DEBUGGER_OUTPUT_CHUNK_SIZE);

		alert(at_console, "%s", chunk.c_str());
	}
}

void CASDebugger::ListMemberProperties(asIScriptContext *context)
{
	void *object = context->GetThisPointer();

	if (!object)
		return;

	asIScriptEngine *engine = context->GetEngine();
	asITypeInfo *type = engine->GetTypeInfoById(context->GetThisTypeId());
	std::stringstream stream;

	stream << type->GetName() << " this = ";

	engine = context->GetEngine();
	int typeId = context->GetThisTypeId();

	stream << ToString(object, typeId, true, engine) << std::endl;
	Output(stream.str());
}

std::string CASDebugger::ToString(void *value, asUINT typeId, bool expandMembers, asIScriptEngine *engine)
{
	std::stringstream stream;

	if (typeId == asTYPEID_VOID)
		return "<void>";
	else if (typeId == asTYPEID_BOOL)
		return *(bool *)value ? "true" : "false";
	else if (typeId == asTYPEID_INT8)
		stream << (int)*(signed char *)value;
	else if (typeId == asTYPEID_INT16)
		stream << (int)*(signed short *)value;
	else if (typeId == asTYPEID_INT32)
		stream << *(int *)value;
	else if (typeId == asTYPEID_INT64)
		stream << *(asINT64 *)value;
	else if (typeId == asTYPEID_UINT8)
		stream << (unsigned int)*(unsigned char *)value;
	else if (typeId == asTYPEID_UINT16)
		stream << (unsigned int)*(unsigned short *)value;
	else if (typeId == asTYPEID_UINT32)
		stream << *(unsigned int *)value;
	else if (typeId == asTYPEID_UINT64)
		stream << *(asQWORD *)value;
	else if (typeId == asTYPEID_FLOAT)
		stream << *(float *)value;
	else if (typeId == asTYPEID_DOUBLE)
		stream << *(double *)value;
	else if (!(typeId & asTYPEID_MASK_OBJECT))
	{
		stream << *(asUINT *)value;

		asITypeInfo *type = engine->GetTypeInfoById(typeId);

		if (type)
		{
			for (int index = type->GetEnumValueCount(); index-- > 0;)
			{
				int enumValue;
				const char *name = type->GetEnumValueByIndex(index, &enumValue);

				if (enumValue == *(int *)value)
				{
					stream << ", " << name;
					break;
				}
			}
		}
	}
	else if (typeId & asTYPEID_SCRIPTOBJECT)
	{
		if (typeId & asTYPEID_OBJHANDLE)
			value = *(void **)value;

		asIScriptObject *object = (asIScriptObject *)value;

		if (object && expandMembers)
		{
			asITypeInfo *type = object->GetObjectType();
			std::map<const asITypeInfo *, ToStringCallback>::iterator callback = m_toStringCallbacks.find(type);

			if (callback != m_toStringCallbacks.end())
				stream << callback->second(object, expandMembers, this);

			for (asUINT index = 0; index < object->GetPropertyCount(); ++index)
			{
				stream << std::endl
				       << "  " << type->GetPropertyDeclaration(index) << " = "
				       << ToString(object->GetAddressOfProperty(index), object->GetPropertyTypeId(index), false, engine);
			}
		}
	}
	else
	{
		if (typeId & asTYPEID_OBJHANDLE)
			value = *(void **)value;

		asITypeInfo *type = engine->GetTypeInfoById(typeId);

		if (value)
		{
			std::map<const asITypeInfo *, ToStringCallback>::iterator callback = m_toStringCallbacks.find(type);

			if (callback == m_toStringCallbacks.end() && (type->GetFlags() & asOBJ_TEMPLATE))
			{
				asITypeInfo *templateType = CASServerManager::GetInstance()->GetTypeInfoByName(type->GetName());

				callback = m_toStringCallbacks.find(templateType);
			}

			if (callback != m_toStringCallbacks.end())
				stream << callback->second(value, expandMembers, this);
			else
				stream << "{...}";
		}
		else
			stream << "null";
	}

	return stream.str();
}
