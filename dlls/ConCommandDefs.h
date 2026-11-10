#ifndef CONCOMMANDDEFS_H
#define CONCOMMANDDEFS_H

namespace ConCommandKind
{
	enum Type
	{
		None = 0,
		ConCommand,
		ClientCommand,
		CVar
	};
}

typedef unsigned int ConCommandFlags_t;

namespace ConCommandFlag
{
	enum ConCommandFlag
	{
		None = 0,
		AdminOnly = 1 << 0,
		Cheat = 1 << 1
	};
}

namespace ConCommandExecuteResult
{
	enum Type : int;
}

class CCommand;
class CBasePlayer;

ConCommandExecuteResult::Type ValidateCommandAccess(const CCommand &command, CBasePlayer *player,
	bool consoleCommand, const char *name, ConCommandKind::Type kind, ConCommandFlags_t flags);
bool CheatsAllowed(CBasePlayer *player, const char *name, bool requireAlive, bool silent);

#endif
