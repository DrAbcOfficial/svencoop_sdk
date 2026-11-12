#ifndef CASSCRIPTDEBUGGER_H
#define CASSCRIPTDEBUGGER_H

#include "scripting/core/CASServerScriptGlobal.h"

class CASDocumentation;

class CASScriptDebugger : public CASServerScriptGlobal<CASScriptDebugger>
{
public:
	CASScriptDebugger();
	virtual ~CASScriptDebugger();

	unsigned int GetCurrentStackLevel(void) const;
	void OutputLocals(void);
	void OutputGlobals(void);
	void OutputMembers(void);
	void OutputStatistics(void);
};

void RegisterSCScriptDebugger(CASDocumentation *documentation);

#endif
