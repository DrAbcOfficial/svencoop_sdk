#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player/player.h"
#include "scripting/core/CASConCommandSystem.h"
#include "scripting/core/CASBaseConCommand.h"
#include "scripting/core/CASConCommand.h"
#include "scripting/core/CASClientCommand.h"
#include "scripting/core/CASCVar.h"
#include "scripting/core/CASCommand.h"
#include "scripting/core/CASFunction.h"
#include "scripting/core/CASServerManager.h"
#include "scripting/core/CASPluginManager.h"
#include "scripting/core/CASModule.h"
#include "scripting/core/CASModuleInfo.h"
#include "scripting/core/CASScriptInfo.h"
#include "scripting/core/CASLog.h"
#include "scripting/core/CASDocumentation.h"
#include "server/CConCommandSystem.h"
#include "server/ConCommandUtils.h"

static const int COMMAND_ACCESS_DENIED = 3;

namespace
{
struct CommandContext
{
	const CASCommand *m_pCommand;
	int	m_iResult;
};
}

CASConCommandSystem::CASConCommandSystem() : m_StateByte(0), m_bInCVarCallback(false)
{
	CConCommandSystem::GetInstance()->SetUnhandledCommandHandler(this);
}

CASConCommandSystem::~CASConCommandSystem()
{
	ClearQueue();
	ClearCommands();
	CConCommandSystem::GetInstance()->SetUnhandledCommandHandler(NULL);
}

bool CASConCommandSystem::IsValidName(const char *name)
{
	return name && *name && !strchr(name, '.') && !strchr(name, ' ') && !strchr(name, '\t');
}

bool CASConCommandSystem::IsValidNamespace(const char *nameSpace)
{
	return nameSpace && !strchr(nameSpace, ' ') && !strchr(nameSpace, '\t');
}

bool CASConCommandSystem::CommandExists(const char *name) const
{
	return name && *name && m_Commands.find(name) != m_Commands.end();
}

CASBaseConCommand *CASConCommandSystem::GetCommand(const char *name) const
{
	if (!name || !*name)
		return NULL;

	CommandMap_t::const_iterator it = m_Commands.find(name);

	return it != m_Commands.end() ? it->second : NULL;
}

bool CASConCommandSystem::AddCommand(CASBaseConCommand *command)
{
	if (!command)
		return false;

	if (command->m_bAdded)
	{
		CASLog::GetInstance()->Warning(AS_LOG_DEFAULT, "ConCommandManager::AddCommand: Command being added twice, ignoring!\n");
		return true;
	}

	CString name(command->m_Name);

	if (!IsValidName(name.Data()))
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "ConCommandManager::AddCommand: Command '%s' has an invalid name!\n", command->m_Name.Data());
		return false;
	}

	CASModule *module = CASServerManager::GetInstance()->m_pActiveModule;

	if (module && module->m_pModuleInfo->m_ModuleType == ModuleType::PLUGIN)
	{
		CString moduleName(module->GetModuleName());
		CString nameSpace(GetCommandNamespace(moduleName));

		name = nameSpace + '.' + name;
	}

	if (CommandExists(name.Data()))
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "ConCommandManager::AddCommand: Command with name '%s' already exists!\n", name.Data());
		return false;
	}

	CommandMap_t::iterator it = m_Commands.insert(std::make_pair(name, command)).first;

	if (it == m_Commands.end())
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "ConCommandManager::AddCommand: Failed to insert command '%s' into the command map!\n", name.Data());
		return false;
	}

	command->m_FullyQualifiedName = name;
	CASLog::GetInstance()->Log((ASLogLevel_t)2, "Adding console command '%s'\n", name.Data());
	return true;
}

void CASConCommandSystem::RemoveCommand(CASBaseConCommand *command)
{
	if (!command || !command->m_bAdded)
		return;

	CommandMap_t::iterator it = m_Commands.find(command->m_FullyQualifiedName);

	if (it != m_Commands.end() && it->second == command)
	{
		command->m_bAdded = false;
		m_Commands.erase(it);
	}
}

bool CASConCommandSystem::RegisterModule(const char *moduleName, const char *nameSpace)
{
	if (!moduleName || !*moduleName)
		return false;

	CString validNamespace;

	if (IsValidNamespace(nameSpace))
		validNamespace = nameSpace;
	else
		CASLog::GetInstance()->Warning(AS_LOG_DEFAULT, "Module '%s': Console command namespace '%s' contains invalid characters!\n", moduleName, nameSpace);

	if (m_Modules.find(moduleName) != m_Modules.end())
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "Module '%s' already exists!\n", moduleName);
		return false;
	}

	ModuleData_t data = { validNamespace };

	m_Modules.insert(std::make_pair(moduleName, data));
	return true;
}

void CASConCommandSystem::RemoveModule(const char *moduleName)
{
	if (!moduleName || !*moduleName)
		return;

	ModuleMap_t::iterator it = m_Modules.find(moduleName);

	if (it != m_Modules.end())
	{
		RemoveAllCommandsOfModule(moduleName);
		m_Modules.erase(it);
	}
}

int CASConCommandSystem::RemoveAllCommandsOfModule(const char *moduleName)
{
	if (!moduleName || !*moduleName)
		return 0;

	int count = 0;

	for (CommandMap_t::iterator it = m_Commands.begin(); it != m_Commands.end();)
	{
		if (it->second->m_OwningModuleName.ICompare(moduleName))
			it++;
		else
		{
			it->second->m_bAdded = false;
			m_Commands.erase(it++);
			count++;
		}
	}

	return count;
}

const CString &CASConCommandSystem::GetCommandNamespace(const CString &moduleName) const
{
	ModuleMap_t::const_iterator it = m_Modules.find(moduleName);

	return it != m_Modules.end() ? it->second.m_Namespace : CString::EMPTY_STRING;
}

CBasePlayer *CASConCommandSystem::GetCurrentPlayer(void) const
{
	return m_Players.empty() ? NULL : m_Players.top();
}

void CASConCommandSystem::DisplayCommandStats(void) const
{
	ALERT(at_console, "Command statistics:\n%u commands\n", m_Commands.size());
}

void CASConCommandSystem::ClearCommands(void)
{
	// This loop does not advance its iterator
	for (CommandMap_t::iterator it = m_Commands.begin(); it != m_Commands.end();)
		it->second->m_bAdded = false;

	m_Commands.clear();
}

int CASConCommandSystem::Execute(CASBaseConCommand *target, const CASCommand *command,
                                 CBasePlayer *player, bool consoleCommand)
{
	if (!command || command->ArgC() <= 0)
		return 1;

	if (!target)
		return 2;

	int result = ValidateCommandAccess(*command, player, consoleCommand,
	                                   target->m_FullyQualifiedName.Data(), target->m_Kind, target->m_Flags);

	if (result)
		return result;

	m_Players.push(player);

	CommandContext context = { command, 1 };

	target->Visit(*this, &context);
	m_Players.pop();
	return context.m_iResult;
}

int CASConCommandSystem::Execute(CASBaseConCommand *target, const CCommand &command,
                                 CBasePlayer *player, bool consoleCommand)
{
	CASCommand *copy = new CASCommand(command);
	int result = Execute(target, copy, player, consoleCommand);

	copy->Release();
	return result;
}

int CASConCommandSystem::Execute(const CASCommand *command, CBasePlayer *player, bool consoleCommand)
{
	if (!command || command->ArgC() <= 0)
		return 1;

	return Execute(GetCommand(command->Arg(0)), command, player, consoleCommand);
}

int CASConCommandSystem::Execute(const CCommand &command, CBasePlayer *player, bool consoleCommand)
{
	if (command.ArgC() <= 0)
		return 1;

	return Execute(GetCommand(command.Arg(0)), command, player, consoleCommand);
}

void CASConCommandSystem::Visit(CASConCommand &command, void *data)
{
	CommandContext *context = (CommandContext *)data;

	context->m_iResult = command.m_pCallback ? !command.m_pCallback->Call(0, context->m_pCommand) : 1;
}

void CASConCommandSystem::Visit(CASClientCommand &command, void *data)
{
	CommandContext *context = (CommandContext *)data;
	CBasePlayer *player = GetCurrentPlayer();

	if (!player)
	{
		context->m_iResult = 1;
		return;
	}

	CASModule *plugin = CASServerManager::GetInstance()->m_pPluginManager->GetPlugin(command.m_OwningModuleName.Data());

	if (plugin && AdminLevel(player) < plugin->GetScriptInfo()->GetMinimumAdminLevel())
	{
		ClientPrint(player->pev, HUD_PRINTCONSOLE, "You do not have rights needed to use this command\n");
		context->m_iResult = COMMAND_ACCESS_DENIED;
		return;
	}

	context->m_iResult = command.m_pCallback ? !command.m_pCallback->Call(0, context->m_pCommand) : 1;
}

void CASConCommandSystem::Visit(CASCVar &variable, void *data)
{
	CommandContext *context = (CommandContext *)data;
	const CASCommand *command = context->m_pCommand;

	context->m_iResult = 0;

	if (command->ArgC() == 1)
		ALERT(at_console, "\"%s\" is \"%s\"\n", variable.GetName().Data(), variable.GetString().Data());
	else if (command->ArgC() == 2 && variable.GetString() != command->Arg(1))
	{
		if (!variable.SetStringInternal(CString(command->Arg(1)), true))
			context->m_iResult = 1;
	}
}

void CASConCommandSystem::EnqueueCommand(CASCommand *command, CommandQueueCallback_t callback)
{
	if (!command)
		return;

	command->AddRef();

	Command_t queued = { command, callback };

	m_Queue.push_back(queued);
}

void CASConCommandSystem::ExecuteQueue(void)
{
	for (std::vector<Command_t>::iterator it = m_Queue.begin(); it != m_Queue.end(); it++)
	{
		int result = static_cast<IConCommandSystem *>(this)->Execute(*it->m_pCommand, NULL, true);

		if (it->m_pCallback)
			it->m_pCallback((ConCommandExecuteResult::Type)result, it->m_pCommand->Arg(0), it->m_pCommand);
	}

	ClearQueue();
}

void CASConCommandSystem::ClearQueue(void)
{
	for (std::vector<Command_t>::iterator it = m_Queue.begin(); it != m_Queue.end(); it++)
		it->m_pCommand->Release();

	m_Queue.clear();
}

void ServerCommandQueueCallback(ConCommandExecuteResult::Type result, const char *name, const CASCommand *command)
{
	if ((int)result == 1)
		ALERT(at_error, "An error occured while executing command '%s'!\n", name);
	else if ((int)result == 2)
		ALERT(at_error, "Command '%s' does not exist!\n", name);
}

void CASConCommandSystem_ServerCommand(CASConCommandSystem *system, const CString &command)
{
	CCommand arguments(command.Data());

	system->Execute(arguments, NULL, true);
}

void RegisterSCScriptConCommandSystem(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("Script console command manager.", "CConCommandSystem", 0, asOBJ_REF | asOBJ_NOCOUNT);
	documentation->RegisterObjectMethod("If the current command was executed by a player, this returns that player.",
	                                    "CConCommandSystem", "CBasePlayer@ GetCurrentPlayer() const", asMETHOD(CASConCommandSystem, GetCurrentPlayer), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Executes an Angelscript server command.", "CConCommandSystem",
	                                    "void ServerCommand(const string& in szCommand)", asFUNCTION(CASConCommandSystem_ServerCommand), asCALL_CDECL_OBJFIRST);
	documentation->RegisterGlobalProperty("CConCommandSystem global instance", "CConCommandSystem g_ConCommandSystem", CASConCommandSystem::GetInstance());
}

void GetASCVarValues(void *object, const char *&defaultValue, const char *&currentValue)
{
	CASCVar *variable = (CASCVar *)object;

	defaultValue = variable->GetDefaultValue().Data();
	currentValue = variable->GetString().Data();
}

void CASConCommandSystem::FindCommands(const char *prefix, CBasePlayer *player) const
{
	if (!prefix || !*prefix || !player)
		return;

	edict_t *entity = player->pev ? player->edict() : NULL;

	for (CommandMap_t::const_iterator it = m_Commands.begin(); it != m_Commands.end(); it++)
		OutputFoundCommand(entity, prefix, it->second, it->second->m_FullyQualifiedName.Data(),
		                   it->second->m_HelpInfo.Data(), it->second->m_Kind, GetASCVarValues);
}

void RegisterSCScriptCASCommand(CASDocumentation *documentation);
void RegisterSCScriptClientCommand(CASDocumentation *documentation);
void RegisterSCScriptCVar(CASDocumentation *documentation);

void RegisterSCScriptCVarClasses(CASDocumentation *documentation)
{
	{
		CString nameSpace(documentation->GetDefaultNamespace());

		documentation->SetDefaultNamespace("ConCommandKind");
		documentation->RegisterEnum("Console command kind.", "Type");
		documentation->RegisterEnumValue("This console command is a command", "Type", "ConCommand", 1);
		documentation->RegisterEnumValue("This console command is a client command", "Type", "ClientCommand", 2);
		documentation->RegisterEnumValue("This console command is a cvar", "Type", "CVar", 3);
		documentation->SetDefaultNamespace(nameSpace.Data());
		documentation->RegisterTypedef("Type to store console command flags in", "ConCommandFlags_t", "uint32");
	}

	{
		CString nameSpace(documentation->GetDefaultNamespace());

		documentation->SetDefaultNamespace("ConCommandFlag");
		documentation->RegisterEnum("Console command flags", "ConCommandFlag");
		documentation->RegisterEnumValue("No flags to set", "ConCommandFlag", "None", 0);
		documentation->RegisterEnumValue("If set, the invoking player (if any) must have admin access.", "ConCommandFlag", "AdminOnly", 1);
		documentation->RegisterEnumValue("If set, the invoking player (if any) must have cheat access.", "ConCommandFlag", "Cheat", 2);
		documentation->SetDefaultNamespace(nameSpace.Data());
	}

	RegisterSCScriptCASCommand(documentation);
	RegisterSCScriptConCommand(documentation);
	RegisterSCScriptClientCommand(documentation);
	RegisterSCScriptCVar(documentation);
	RegisterSCScriptConCommandSystem(documentation);
}
