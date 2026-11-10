#ifndef ASSCRIPTLOG_H
#define ASSCRIPTLOG_H

#include "CASServerScriptGlobal.h"

class CASDocumentation;
class asIScriptGeneric;

class CASScriptLog : public CASServerScriptGlobal<CASScriptLog>
{
public:
	CASScriptLog();
	virtual ~CASScriptLog();

	bool PrintF(asIScriptGeneric *generic);
};

void CASScriptLog_PrintF(asIScriptGeneric *generic);
void RegisterSCScriptScriptLog(CASDocumentation *documentation);

#endif
