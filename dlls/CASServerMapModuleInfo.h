#ifndef CASSERVERMAPMODULEINFO_H
#define CASSERVERMAPMODULEINFO_H

#include "CASModuleInfo.h"

class CASServerMapModuleInfo : public CASModuleInfo
{
public:
	CASServerMapModuleInfo();
	virtual ~CASServerMapModuleInfo();

	virtual CASHeaderList *GenerateHeaders(void);
	virtual CASModule *CreateModule(CASModuleBuilder *builder, asIScriptModule *module);
	virtual bool Validate(CASModuleBuilder *builder);
};

extern CASServerMapModuleInfo g_ServerMapModuleInfo;

#endif
