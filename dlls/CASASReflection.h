#ifndef CASASREFLECTION_H
#define CASASREFLECTION_H

#include "CASServerScriptGlobal.h"
#include "CASEngineReflectionGroup.h"
#include "CASModuleReflectionGroup.h"

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
