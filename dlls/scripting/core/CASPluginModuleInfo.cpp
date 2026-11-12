#include "scripting/core/CASPluginModuleInfo.h"
#include "scripting/core/CASModule.h"
#include "scripting/core/CASHeaderList.h"
#include "scripting/core/CASScriptBaseClasses.h"
#include "scripting/core/CASCommonServerModuleValidation.h"

CASPluginModuleInfo g_PluginModuleInfo;

CASPluginModuleInfo::CASPluginModuleInfo()
    : CASModuleInfo("scripts/plugins", ModuleType::PLUGIN, 3, false, "void PluginInit()")
{
}

CASPluginModuleInfo::~CASPluginModuleInfo()
{
}

CASHeaderList *CASPluginModuleInfo::GenerateHeaders(void)
{
	CASHeaderList *headers = CASModuleInfo::GenerateHeaders();
	const CASScriptBaseClasses::ScriptBaseClass_t *baseClasses = CASScriptBaseClasses::GetBaseClasses();
	unsigned int count = CASScriptBaseClasses::GetBaseClassesCount();

	for (unsigned int index = 0; index < count; index++)
		headers->AddHeader(baseClasses[index].m_pszName, baseClasses[index].m_pszSource);

	return headers;
}

CASModule *CASPluginModuleInfo::CreateModule(CASModuleBuilder *builder, asIScriptModule *module)
{
	return new CASModule(module, this);
}

bool CASPluginModuleInfo::Validate(CASModuleBuilder *builder)
{
	return CASModuleInfo::Validate(builder) && CASCommonServerModuleValidation::CheckForEntityHandles(builder);
}
