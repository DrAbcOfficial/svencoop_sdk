#include "server/CClientCommand.h"
#include "server/CConCommandSystem.h"

CClientCommand::CClientCommand(const char *name, CommandCallback_t callback,
                               ConCommandFlags_t flags, const char *helpInfo)
    : CBaseCommand(ConCommandKind::ClientCommand, name, callback, flags, helpInfo)
{
}

CClientCommand::CClientCommand(const char *name, ICommandCallback *callback,
                               ConCommandFlags_t flags, const char *helpInfo)
    : CBaseCommand(ConCommandKind::ClientCommand, name, callback, flags, helpInfo)
{
}

void CClientCommand::Visit(CConCommandSystem &system, void *context)
{
	system.Visit(*this, context);
}
