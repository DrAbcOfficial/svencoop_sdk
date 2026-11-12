#ifndef CASDEBUGGER_H
#define CASDEBUGGER_H

#include "../../../external/angelscript/sdk/add_on/debugger/debugger.h"

class CASDebugger : public CDebugger
{
public:
	CASDebugger();
	virtual ~CASDebugger();

	virtual void TakeCommands(asIScriptContext *context);
	virtual void Output(const std::string &text);
	virtual void ListMemberProperties(asIScriptContext *context);
	virtual std::string ToString(void *value, asUINT typeId, bool expandMembers, asIScriptEngine *engine);
};

#endif
