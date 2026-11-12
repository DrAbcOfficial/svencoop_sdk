#include "extdll.h"
#include "server/ConCommandDefs.h"
#include "server/CCommand.h"
#include "server/AdminLevel.h"

static const ConCommandExecuteResult::Type COMMAND_ACCESS_ALLOWED = (ConCommandExecuteResult::Type) 0;
static const ConCommandExecuteResult::Type COMMAND_NAME_MISMATCH = (ConCommandExecuteResult::Type) 1;
static const ConCommandExecuteResult::Type COMMAND_KIND_MISMATCH = (ConCommandExecuteResult::Type) 2;
static const ConCommandExecuteResult::Type COMMAND_ACCESS_DENIED = (ConCommandExecuteResult::Type) 3;

ConCommandExecuteResult::Type ValidateCommandAccess(const CCommand &command, CBasePlayer *player,
	bool consoleCommand, const char *name, ConCommandKind::Type kind, ConCommandFlags_t flags)
{
	if (stricmp(name, command.Arg(0)))
		return COMMAND_NAME_MISMATCH;

	bool isConsoleKind = kind == ConCommandKind::ConCommand || kind == ConCommandKind::CVar;

	if (isConsoleKind != consoleCommand)
		return COMMAND_KIND_MISMATCH;

	if (!player)
		return COMMAND_ACCESS_ALLOWED;

	if ((flags & ConCommandFlag::AdminOnly) && AdminLevel(player) < ADMIN_YES)
		return COMMAND_ACCESS_DENIED;

	if ((flags & ConCommandFlag::Cheat) && !CheatsAllowed(player, name, false, true))
		return COMMAND_ACCESS_DENIED;

	return COMMAND_ACCESS_ALLOWED;
}
