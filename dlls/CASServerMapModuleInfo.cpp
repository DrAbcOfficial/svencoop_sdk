#include "CASServerMapModuleInfo.h"
#include "CASModule.h"
#include "CASHeaderList.h"
#include "CASScriptBaseClasses.h"
#include "CASCommonServerModuleValidation.h"

CASServerMapModuleInfo g_ServerMapModuleInfo;

CASServerMapModuleInfo::CASServerMapModuleInfo()
    : CASModuleInfo("scripts/maps", ModuleType::MAP, 2, true, "void MapInit()")
{
}

CASServerMapModuleInfo::~CASServerMapModuleInfo()
{
}

CASHeaderList *CASServerMapModuleInfo::GenerateHeaders(void)
{
	CASHeaderList *headers = CASModuleInfo::GenerateHeaders();
	const CASScriptBaseClasses::ScriptBaseClass_t *baseClasses = CASScriptBaseClasses::GetBaseClasses();
	unsigned int count = CASScriptBaseClasses::GetBaseClassesCount();

	for (unsigned int index = 0; index < count; index++)
		headers->AddHeader(baseClasses[index].m_pszName, baseClasses[index].m_pszSource);

	return headers;
}

CASModule *CASServerMapModuleInfo::CreateModule(CASModuleBuilder *builder, asIScriptModule *module)
{
	return new CASModule(module, this);
}

bool CASServerMapModuleInfo::Validate(CASModuleBuilder *builder)
{
	return CASModuleInfo::Validate(builder) && CASCommonServerModuleValidation::CheckForEntityHandles(builder);
}
