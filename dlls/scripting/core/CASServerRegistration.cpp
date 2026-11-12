#include "scripting/core/CASServerRegistration.h"
#include "scripting/core/CASServerManager.h"
#include "scripting/core/CASDebugger.h"
#include "scripting/api/ASDebugEntities.h"
#include "scripting/api/ASMath.h"
#include "scripting/api/ASEngine.h"
#include "scripting/api/ASDateTime.h"
#include "scripting/api/ASNetworkMessage.h"

void RegisterSCScriptEdict(CASDocumentation *documentation);
void RegisterSCScriptEntityDependencies(CASDocumentation *documentation);
void RegisterSCScriptEngineFuncs(CASDocumentation *documentation);
void RegisterSCScriptModelFuncs(CASDocumentation *documentation);
void RegisterSCScriptEntities(CASDocumentation *documentation);
void RegisterSCScriptUtility(CASDocumentation *documentation);
void RegisterSCScriptGame(CASDocumentation *documentation);
void RegisterSCScriptSound(CASDocumentation *documentation);
void RegisterSCScriptEntityFuncs(CASDocumentation *documentation);
void RegisterSCScriptPlayerFuncs(CASDocumentation *documentation);
void RegisterSCScriptWeaponFuncs(CASDocumentation *documentation);
void RegisterSCScriptGlobalState(CASDocumentation *documentation);
void RegisterSCScriptAngelscript(CASDocumentation *documentation);
void RegisterSCScriptMapCycle(CASDocumentation *documentation);
void RegisterSCScriptMap(CASDocumentation *documentation);
void RegisterSCScriptPersistence(CASDocumentation *documentation);
void RegisterSCScriptPluginManager(CASDocumentation *documentation);
void RegisterSCScriptStartInventory(CASDocumentation *documentation);
void RegisterSCScriptTextMenus(CASDocumentation *documentation);
void RegisterSCScriptVote(CASDocumentation *documentation);
void RegisterSCScriptAdmin(CASDocumentation *documentation);
void RegisterSCScriptWaypointManager(CASDocumentation *documentation);
void RegisterSCScriptScriptLog(CASDocumentation *documentation);
void RegisterSCScriptCVarClasses(CASDocumentation *documentation);
void RegisterSCScriptClientSayParameters(CASDocumentation *documentation);
void RegisterSCScriptClassicMode(CASDocumentation *documentation);
void RegisterSCScriptSurvivalMode(CASDocumentation *documentation);

namespace
{
struct EntityToStringCallback_t
{
	const char					*name;
	CDebugger::ToStringCallback	callback;
};

const EntityToStringCallback_t entityCallbacks[] =
{
	{ "CBaseEntity", SCScriptEntityToString },
	{ "CBaseDelay", SCScriptEntityToString },
	{ "CBaseAnimating", SCScriptEntityToString },
	{ "CBaseToggle", SCScriptEntityToString },
	{ "CBasePlayerItem", SCScriptEntityToString },
	{ "CBasePlayerWeapon", SCScriptEntityToString },
	{ "CBasePlayerAmmo", SCScriptEntityToString },
	{ "CItemInventory", SCScriptEntityToString },
	{ "CItem", SCScriptEntityToString },
	{ "CGrenade", SCScriptEntityToString },
	{ "CBaseMonster", SCScriptEntityToString },
	{ "CCineMonster", SCScriptEntityToString },
	{ "CBasePlayer", SCScriptPlayerEntityToString },
	{ "CSprite", SCScriptEntityToString },
	{ "CPathTrack", SCScriptEntityToString },
	{ "CBeam", SCScriptEntityToString },
	{ "CLaser", SCScriptEntityToString },
	{ "CBaseTank", SCScriptEntityToString },
	{ "CPathCondition", SCScriptEntityToString },
	{ "CBaseButton", SCScriptEntityToString },
	{ "CBaseDoor", SCScriptEntityToString },
	{ "CGib", SCScriptEntityToString }
};
}

CASServerRegistration::CASServerRegistration()
{
}

CASServerRegistration::~CASServerRegistration()
{
}

void CASServerRegistration::RegisterToStringCallbacks(asIScriptEngine *engine)
{
	CASBaseRegistration::RegisterToStringCallbacks(engine);

	CASDebugger *debugger = CASServerManager::GetInstance()->m_pDebugger;

	for (unsigned int index = 0; index < sizeof(entityCallbacks) / sizeof(entityCallbacks[0]); index++)
	{
		debugger->RegisterToStringCallback(
		    CASServerManager::GetInstance()->GetTypeInfoByName(entityCallbacks[index].name),
		    entityCallbacks[index].callback);
	}
}

void CASServerRegistration::RegisterInterfaces(CASDocumentation *documentation)
{
	CASBaseRegistration::RegisterInterfaces(documentation);
	RegisterSCScriptMath(documentation);
	RegisterSCScriptEdict(documentation);
	RegisterSCScriptEntityDependencies(documentation);
	RegisterSCScriptEngineFuncs(documentation);
	RegisterSCScriptModelFuncs(documentation);
	RegisterSCScriptEntities(documentation);
	RegisterSCScriptUtility(documentation);
	RegisterSCScriptGame(documentation);
	RegisterSCScriptSound(documentation);
	RegisterSCScriptEngine(documentation);
	RegisterSCScriptEntityFuncs(documentation);
	RegisterSCScriptPlayerFuncs(documentation);
	RegisterSCScriptWeaponFuncs(documentation);
	RegisterSCScriptGlobalState(documentation);
	RegisterSCScriptAngelscript(documentation);
	RegisterSCScriptMapCycle(documentation);
	RegisterSCScriptMap(documentation);
	RegisterSCScriptPersistence(documentation);
	RegisterSCScriptPluginManager(documentation);
	RegisterSCScriptStartInventory(documentation);
	RegisterSCScriptTextMenus(documentation);
	RegisterSCScriptVote(documentation);
	RegisterSCScriptDateTime(documentation);
	RegisterSCScriptAdmin(documentation);
	RegisterSCScriptWaypointManager(documentation);
	RegisterSCScriptNetworkMessage(documentation);
	RegisterSCScriptScriptLog(documentation);
	RegisterSCScriptCVarClasses(documentation);
	RegisterSCScriptClientSayParameters(documentation);
	RegisterSCScriptClassicMode(documentation);
	RegisterSCScriptSurvivalMode(documentation);
}
