#ifndef CASCLIENTCOMMAND_H
#define CASCLIENTCOMMAND_H

#include "scripting/core/CASBaseConCommand.h"

class CASClientCommand : public CASBaseConCommand
{
public:
	CASClientCommand(const CString &name, const CString &helpInfo, asIScriptFunction *callback, ConCommandFlags_t flags);
	virtual void Visit(CASConCommandSystem &system, void *context);
	virtual ~CASClientCommand();
};

#endif
