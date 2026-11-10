#include "ASAngelscript.h"
#include "CASDocumentation.h"

CAngelscript::CAngelscript()
{
}

CAngelscript::~CAngelscript()
{
}

CString CAngelscript::GetAngelscriptDescription(void) const
{
	return CString("Angelscript v2.36.1 for Sven Co-op, API Version 1.26");
}

CString CAngelscript::GetAngelscriptVersionString(void) const
{
	return CString("2.36.1");
}

unsigned int CAngelscript::GetAngelscriptVersion(void) const
{
	return 23601;
}

unsigned int CAngelscript::GetMajorVersion(void) const
{
	return 1;
}

unsigned int CAngelscript::GetMinorVersion(void) const
{
	return 26;
}

void RegisterSCScriptAngelscript(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("Angelscript debugging functions", "CAngelscript", 0, asOBJ_REF | asOBJ_NOCOUNT);
	documentation->RegisterObjectMethod("Returns a brief description of Angelscript", "CAngelscript", "string GetAngelscriptDescription() const", asMETHOD(CAngelscript, GetAngelscriptDescription), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the current version of Angelscript as a string", "CAngelscript", "string GetAngelscriptVersionString() const", asMETHOD(CAngelscript, GetAngelscriptVersionString), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the current version of Angelscript", "CAngelscript", "uint GetAngelscriptVersion() const", asMETHOD(CAngelscript, GetAngelscriptVersion), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the major version of the Angelscript Sven Co-op implementation", "CAngelscript", "uint GetMajorVersion() const", asMETHOD(CAngelscript, GetMajorVersion), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the minor version of the Angelscript Sven Co-op implementation", "CAngelscript", "uint GetMinorVersion() const", asMETHOD(CAngelscript, GetMinorVersion), asCALL_THISCALL);
	documentation->RegisterGlobalProperty("Angelscript debugging functions instance", "CAngelscript g_Angelscript", CAngelscript::GetInstance());
}
