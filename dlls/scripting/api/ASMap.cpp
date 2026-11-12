#include "scripting/api/ASMap.h"
#include "scripting/core/CASDocumentation.h"
#include "scripting/core/CASMap.h"

void RegisterSCScriptMap( CASDocumentation *documentation )
{
	documentation->RegisterObjectType( "Map state variables", "CMap", 0, asOBJ_REF | asOBJ_NOCOUNT );
	documentation->RegisterObjectMethod(
	    "Get if player models are forced",
	    "CMap",
	    "bool HasForcedPlayerModels() const",
	    asMETHOD( CASMap, HasForcedPlayerModels ),
	    asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Reload the standard map skill file (_skl.cfg).",
	    "CMap",
	    "bool LoadMapSkillFile()",
	    asMETHOD( CASMap, LoadMapSkillFile ),
	    asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Reload the global skill file.",
	    "CMap",
	    "bool LoadGlobalSkillFile()",
	    asMETHOD( CASMap, LoadGlobalSkillFile ),
	    asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Load a custom skill file.",
	    "CMap",
	    "bool LoadSkillFile(const string& in szFileName)",
	    asMETHOD( CASMap, LoadSkillFile ),
	    asCALL_THISCALL );
	documentation->RegisterGlobalProperty(
	    "Map state variables instance", "CMap g_Map", CASMap::GetInstance() );
}
