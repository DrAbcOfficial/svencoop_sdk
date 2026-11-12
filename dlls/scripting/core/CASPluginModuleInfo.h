#ifndef CASPLUGINMODULEINFO_H
#define CASPLUGINMODULEINFO_H

#include "scripting/core/CASModuleInfo.h"

class CASPluginModuleInfo : public CASModuleInfo
{
public:
	CASPluginModuleInfo();
	virtual ~CASPluginModuleInfo();

	virtual CASHeaderList *GenerateHeaders(void);
	virtual CASModule *CreateModule(CASModuleBuilder *builder, asIScriptModule *module);
	virtual bool Validate(CASModuleBuilder *builder);
};

extern CASPluginModuleInfo g_PluginModuleInfo;

#endif
