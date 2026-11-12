#ifndef CASASREFLECTION_H
#define CASASREFLECTION_H

#include "scripting/core/CASServerScriptGlobal.h"
#include "scripting/core/CASEngineReflectionGroup.h"
#include "scripting/core/CASModuleReflectionGroup.h"

class CASDocumentation;

class CASASReflection : public CASServerScriptGlobal<CASASReflection>
{
public:
	CASASReflection();
	virtual ~CASASReflection();

private:
	friend void RegisterSCScriptReflection(CASDocumentation *documentation);

	CASEngineReflectionGroup m_EngineGroup;
	CASModuleReflectionGroup m_ModuleGroup;
};

#endif
