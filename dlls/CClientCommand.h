#ifndef CCLIENTCOMMAND_H
#define CCLIENTCOMMAND_H

#include "CBaseCommand.h"

class CClientCommand : public CBaseCommand
{
public:
	CClientCommand(const char *name, CommandCallback_t callback, ConCommandFlags_t flags, const char *helpInfo);
	CClientCommand(const char *name, ICommandCallback *callback, ConCommandFlags_t flags, const char *helpInfo);

	virtual void Visit(CConCommandSystem &system, void *context);
};

#endif
