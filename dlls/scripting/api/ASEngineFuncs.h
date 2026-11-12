#ifndef ASENGINEFUNCS_H
#define ASENGINEFUNCS_H

#include "extdll.h"
#include "CString.h"

class CASDocumentation;

void ConstructTraceResult(void *memory);
void CopyConstructTraceResult(const TraceResult &result, void *memory);
void DestructTraceResult(void *memory);
TraceResult &TraceResult_Assign(const TraceResult &result, TraceResult *destination);
CString GetCvarName(cvar_t *cvar);
CString GetCvarString(cvar_t *cvar);

void RegisterSCScriptTraceResult(CASDocumentation *documentation);
void RegisterSCScriptDamageInfo(CASDocumentation *documentation);
void RegisterSCScriptCvarFlags(CASDocumentation *documentation);
void RegisterSCScriptCvar(CASDocumentation *documentation);
void RegisterSCScriptEngineMoveType(CASDocumentation *documentation);
void RegisterSCScriptEngineFuncs(CASDocumentation *documentation);

#endif
