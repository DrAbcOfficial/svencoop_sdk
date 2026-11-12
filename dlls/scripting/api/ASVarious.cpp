#include "extdll.h"
#include "util.h"
#include "scripting/api/ASVarious.h"
#include "scripting/api/ASString.h"
#include "scripting/core/CASDocumentation.h"
#include "CString.h"
#include "server/server_support.h"

namespace
{
	const float CONST_WORLD_BOUNDARY = 65536.0f;

	void string_t_DefaultConstruct(unsigned int *destination)
	{
		*destination = 0;
	}

	void string_t_CopyConstruct(const unsigned int &value, unsigned int *destination)
	{
		*destination = value;
	}

	void string_t_ASStringConstruct(const CString &text, unsigned int *destination)
	{
		*destination = AllocStringLocal(text.Data());
	}

	void string_t_IntConstruct(int value, unsigned int *destination)
	{
		*destination = value;
	}

	unsigned int &string_t_Assign_string_t(const unsigned int &value, unsigned int *destination)
	{
		*destination = value;
		return *destination;
	}

	unsigned int &string_t_Assign_CString(const CString &text, unsigned int *destination)
	{
		*destination = AllocStringLocal(text.Data());
		return *destination;
	}

	unsigned int &string_t_Assign_Int(int value, unsigned int *destination)
	{
		*destination = value;
		return *destination;
	}

	CString string_t_ToString(const unsigned int &value)
	{
		return CString(gpGlobals->pStringBase + value);
	}

	int string_t_ToInt(const unsigned int &value)
	{
		return value;
	}
}

void RegisterSCScriptstring_t(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("Opaque handle to a pooled string.", "string_t",
		sizeof(unsigned int), asOBJ_VALUE | asOBJ_POD);
	documentation->RegisterObjectBehaviour("Default Constructor", "string_t", asBEHAVE_CONSTRUCT,
		"void string_t()", asFUNCTION(string_t_DefaultConstruct), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Copy Constructor", "string_t", asBEHAVE_CONSTRUCT,
		"void string_t(const string_t& in str)", asFUNCTION(string_t_CopyConstruct), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Construct a string_t from a string", "string_t", asBEHAVE_CONSTRUCT,
		"void string_t(const string& in str)", asFUNCTION(string_t_ASStringConstruct), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Construct a string_t from an integer", "string_t", asBEHAVE_CONSTRUCT,
		"void string_t(int str)", asFUNCTION(string_t_IntConstruct), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Assignment operator", "string_t",
		"string_t& opAssign(const string_t& in str)", asFUNCTION(string_t_Assign_string_t), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Assignment operator", "string_t",
		"string_t& opAssign(const string& in str)", asFUNCTION(string_t_Assign_CString), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Assignment operator", "string_t",
		"string_t& opAssign(int str)", asFUNCTION(string_t_Assign_Int), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Converts this string_t to a string.", "string_t",
		"string opImplConv() const", asFUNCTION(string_t_ToString), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Converts this string_t to an integer.", "string_t",
		"int opImplConv() const", asFUNCTION(string_t_ToInt), asCALL_CDECL_OBJLAST);
}

void RegisterSCScriptVarious(CASDocumentation *documentation)
{
	documentation->RegisterGlobalProperty("Distance from the world origin to the world boundary in a single axis",
		"const float WORLD_BOUNDARY", (void *)&CONST_WORLD_BOUNDARY);
	RegisterSCScriptForwardDeclareString(documentation);
	documentation->RegisterTypedef("Size type used for denoting sizes", "size_t", "uint32");
}
