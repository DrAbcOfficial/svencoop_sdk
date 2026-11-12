#include "scripting/core/CASCommand.h"
#include "scripting/core/CASClassRegistration.h"

CASCommand::CASCommand(int startArgument)
	: CCommand(startArgument), m_iRefCount(1)
{
}

CASCommand::CASCommand(int count, char **arguments)
	: CCommand(count, arguments), m_iRefCount(1)
{
}

CASCommand::CASCommand(const char *command)
	: CCommand(command), m_iRefCount(1)
{
}

CASCommand::CASCommand(CCfgCommand *command)
	: CCommand(command), m_iRefCount(1)
{
}

CASCommand::CASCommand(const char *command, const char *argument)
	: CCommand(command, argument), m_iRefCount(1)
{
}

CASCommand::CASCommand(const CCommand &command)
	: CCommand(command), m_iRefCount(1)
{
}

CASCommand::~CASCommand()
{
}

CASCommand &CASCommand::operator=(const CCommand &command)
{
	CCommand::operator=(command);
	return *this;
}

void CASCommand::AddRef(void) const
{
	m_iRefCount++;
}

void CASCommand::Release(void) const
{
	if (m_iRefCount-- == 1)
		delete this;
}

int CASCommand::ASFindIntArg(const CString &name, int defaultValue) const
{
	return FindIntArg(name.Data(), defaultValue);
}

CString CASCommand::ASFindArg(const CString &name) const
{
	return CString(FindArg(name.Data()));
}

CString CASCommand::ASArg(int index) const
{
	return CString(Arg(index));
}

CString CASCommand::ASGetArgumentsString(void) const
{
	return CString(GetArgumentsString());
}

CString CASCommand::ASGetCommandString(void) const
{
	return CString(GetCommandString());
}

void RegisterSCScriptCASCommand(CASDocumentation *documentation)
{
 documentation->RegisterObjectType("Command arguments.", "CCommand", 0, asOBJ_REF);
 RegisterSCScriptRefCountedBaseClass<CASCommand>(documentation, "CCommand");
 documentation->RegisterObjectMethod("Gets the number of arguments.", "CCommand", "int ArgC() const", asMETHOD(CASCommand, ArgC), asCALL_THISCALL);
 documentation->RegisterObjectMethod("Gets the entire command as a string.", "CCommand", "string GetCommandString() const", asMETHOD(CASCommand, ASGetCommandString), asCALL_THISCALL);
 documentation->RegisterObjectMethod("Gets all arguments as a single string.", "CCommand", "string GetArgumentsString() const", asMETHOD(CASCommand, ASGetArgumentsString), asCALL_THISCALL);
 documentation->RegisterObjectMethod("Gets the argument by index.", "CCommand", "string Arg(const int iIndex) const", asMETHOD(CASCommand, ASArg), asCALL_THISCALL);
 documentation->RegisterObjectMethod("Gets the argument by index.", "CCommand", "string opIndex(const int iIndex) const", asMETHOD(CASCommand, ASArg), asCALL_THISCALL);
 documentation->RegisterObjectMethod("Find a value for a given argument. If the argument does not exist or has no value, an empty string is returned. Otherwise, returns the value.",
	                                 "CCommand", "string FindArg(const string& in szArgument) const", asMETHOD(CASCommand, ASFindArg), asCALL_THISCALL);
 documentation->RegisterObjectMethod("Find an int value for a given argument. Returns iDefault if no such argument exists, or no value exists for it. Otherwise, returns the value, converted to an int",
	                                 "CCommand", "int FindIntArg(const string& in szArgument, const int iDefault = 0) const", asMETHOD(CASCommand, ASFindIntArg), asCALL_THISCALL);
}
