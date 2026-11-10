#include "CASClientCommand.h"
#include "CASConCommandSystem.h"
#include "CASLog.h"
#include "CASClassRegistration.h"

CASClientCommand::CASClientCommand(const CString &name, const CString &helpInfo,
                                   asIScriptFunction *callback, ConCommandFlags_t flags)
    : CASBaseConCommand(ConCommandKind::ClientCommand, name, flags, helpInfo, callback)
{
	if (!callback)
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CClientCommand: callback must be valid!\n");
}

CASClientCommand::~CASClientCommand()
{
}

void CASClientCommand::Visit(CASConCommandSystem &system, void *context)
{
	system.Visit(*this, context);
}

CASClientCommand *CreateClientCommand(const CString &name, const CString &helpInfo, asIScriptFunction *callback, ConCommandFlags_t flags)
{
	return ASNotifyGarbageCollector("CClientCommand", new CASClientCommand(name, helpInfo, callback, flags));
}

void RegisterSCScriptClientCommand(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("Client console command class", "CClientCommand", 0, asOBJ_REF | asOBJ_GC);
	RegisterSCScriptGCRefCountedBaseClass<CASClientCommand>(documentation, "CClientCommand");
	documentation->RegisterObjectMethod("Gets the name of this command.", "CClientCommand",
	                                    "const string& GetName() const", asMETHOD(CASBaseConCommand, GetName), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the fully qualified name of this command. This is the name that the command is referred by when called.", "CClientCommand",
	                                    "const string& GetFullyQualifiedName() const", asMETHOD(CASBaseConCommand, GetFullyQualifiedName), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the help info describing this command.", "CClientCommand",
	                                    "const string& GetHelpInfo() const", asMETHOD(CASBaseConCommand, GetHelpInfo), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the type of this console command.", "CClientCommand",
	                                    "ConCommandKind::Type GetKind() const", asMETHOD(CASBaseConCommand, GetKind), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the name of the module that owns (created) this command.", "CClientCommand",
	                                    "const string& GetOwningModuleName() const", asMETHOD(CASBaseConCommand, GetOwningModuleName), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Whether this command was added to the list of commands.\nMust be added to be usable from the console.", "CClientCommand",
	                                    "bool HasBeenAdded() const", asMETHOD(CASBaseConCommand, HasBeenAdded), asCALL_THISCALL);
	documentation->RegisterFuncDef("Callback for client console commands.", "void ClientCommandCallback(const CCommand@ args)");
	documentation->RegisterObjectBehaviour("Constructor", "CClientCommand", asBEHAVE_FACTORY,
	                                       "CClientCommand@ CClientCommand(const string& in szName, const string& in szHelpInfo, ClientCommandCallback@ pCallback, const ConCommandFlags_t flags = ConCommandFlag::None)",
	                                       asFUNCTION(CreateClientCommand), asCALL_CDECL);
}
