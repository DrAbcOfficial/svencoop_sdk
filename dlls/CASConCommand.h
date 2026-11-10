#ifndef CASCONCOMMAND_H
#define CASCONCOMMAND_H

#include "CASBaseConCommand.h"

class CASConCommand : public CASBaseConCommand
{
public:
	CASConCommand(const CString &name, const CString &helpInfo, asIScriptFunction *callback, ConCommandFlags_t flags);
	virtual void Visit(CASConCommandSystem &system, void *context);
	virtual ~CASConCommand();
};

#endif
