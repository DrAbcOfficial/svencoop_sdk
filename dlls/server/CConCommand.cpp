#include "server/CConCommand.h"
#include "server/CCommand.h"

struct ConCommandContext_t
{
	const CCommand	*command;
	bool			handled;
};

CConCommand::CConCommand(const char *name, CommandCallback_t callback, ConCommandFlags_t flags, const char *helpInfo)
	: CBaseCommand(ConCommandKind::ConCommand, name, callback, flags, helpInfo)
{
}

CConCommand::CConCommand(const char *name, ICommandCallback *callback, ConCommandFlags_t flags, const char *helpInfo)
	: CBaseCommand(ConCommandKind::ConCommand, name, callback, flags, helpInfo)
{
}

void CConCommand::Visit(CConCommandSystem &system, void *context)
{
	CCommand command;

	Dispatch(command);
	((ConCommandContext_t *)context)->handled = true;
}
