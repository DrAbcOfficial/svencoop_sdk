#include "scripting/core/CASConCommand.h"
#include "scripting/core/CASConCommandSystem.h"
#include "scripting/core/CASLog.h"
#include "scripting/core/CASClassRegistration.h"

CASConCommand::CASConCommand(const CString &name, const CString &helpInfo,
                             asIScriptFunction *callback, ConCommandFlags_t flags)
    : CASBaseConCommand(ConCommandKind::ConCommand, name, flags, helpInfo, callback)
{
	if (!callback)
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CConCommand: callback must be valid!\n");
}

CASConCommand::~CASConCommand()
{
}

void CASConCommand::Visit(CASConCommandSystem &system, void *context)
{
	system.Visit(*this, context);
}

CASConCommand *CreateConCommand(const CString &name, const CString &helpInfo, asIScriptFunction *callback, ConCommandFlags_t flags)
{
	return ASNotifyGarbageCollector("CConCommand", new CASConCommand(name, helpInfo, callback, flags));
}

void RegisterSCScriptConCommand(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("Console command class", "CConCommand", 0, asOBJ_REF | asOBJ_GC);
	RegisterSCScriptGCRefCountedBaseClass<CASConCommand>(documentation, "CConCommand");
	documentation->RegisterObjectMethod("Gets the name of this command.", "CConCommand",
	                                    "const string& GetName() const", asMETHOD(CASBaseConCommand, GetName), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the fully qualified name of this command. This is the name that the command is referred by when called.", "CConCommand",
	                                    "const string& GetFullyQualifiedName() const", asMETHOD(CASBaseConCommand, GetFullyQualifiedName), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the help info describing this command.", "CConCommand",
	                                    "const string& GetHelpInfo() const", asMETHOD(CASBaseConCommand, GetHelpInfo), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the type of this console command.", "CConCommand",
	                                    "ConCommandKind::Type GetKind() const", asMETHOD(CASBaseConCommand, GetKind), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the name of the module that owns (created) this command.", "CConCommand",
	                                    "const string& GetOwningModuleName() const", asMETHOD(CASBaseConCommand, GetOwningModuleName), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Whether this command was added to the list of commands.\nMust be added to be usable from the console.", "CConCommand",
	                                    "bool HasBeenAdded() const", asMETHOD(CASBaseConCommand, HasBeenAdded), asCALL_THISCALL);
	documentation->RegisterFuncDef("Callback for server console commands.", "void ConCommandCallback(const CCommand@ args)");
	documentation->RegisterObjectBehaviour("Constructor", "CConCommand", asBEHAVE_FACTORY,
	                                       "CConCommand@ CConCommand(const string& in szName, const string& in szHelpInfo, ConCommandCallback@ pCallback, const ConCommandFlags_t flags = ConCommandFlag::None)",
	                                       asFUNCTION(CreateConCommand), asCALL_CDECL);
}
