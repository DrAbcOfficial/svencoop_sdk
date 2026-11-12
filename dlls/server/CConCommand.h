#ifndef CCONCOMMAND_H
#define CCONCOMMAND_H

#include "server/CBaseCommand.h"

class CConCommand : public CBaseCommand
{
public:
	CConCommand(const char *name, CommandCallback_t callback, ConCommandFlags_t flags, const char *helpInfo);
	CConCommand(const char *name, ICommandCallback *callback, ConCommandFlags_t flags, const char *helpInfo);

	void Visit(CConCommandSystem &system, void *context);
};

#endif
