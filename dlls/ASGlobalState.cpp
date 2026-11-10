#include "extdll.h"
#include "util.h"
#include "saverestore.h"
#include "ASGlobalState.h"
#include "CASDocumentation.h"
#include "CString.h"

CString GlobalEntity_GetName(globalentity_t *entity)
{
	return CString(entity->name);
}

CString GlobalEntity_GetLevelName(globalentity_t *entity)
{
	return CString(entity->levelName);
}

// Script wrapper calls always operate on the global store
void GlobalState_EntityAdd(const CString &globalName, const CString &mapName, GLOBALESTATE state, CGlobalState *)
{
	gGlobalState.EntityAdd(MAKE_STRING(globalName.Data()), MAKE_STRING(mapName.Data()), state);
}

void GlobalState_EntitySetState(const CString &globalName, GLOBALESTATE state, CGlobalState *)
{
	gGlobalState.EntitySetState(MAKE_STRING(globalName.Data()), state);
}

void GlobalState_EntityUpdate(const CString &globalName, const CString &mapName, CGlobalState *)
{
	gGlobalState.EntityUpdate(MAKE_STRING(globalName.Data()), MAKE_STRING(mapName.Data()));
}

const globalentity_t *GlobalState_EntityFromTable(const CString &globalName, CGlobalState *)
{
	return gGlobalState.EntityFromTable(MAKE_STRING(globalName.Data()));
}

GLOBALESTATE GlobalState_EntityGetState(const CString &globalName, CGlobalState *)
{
	return gGlobalState.EntityGetState(MAKE_STRING(globalName.Data()));
}

bool GlobalState_EntityInTable(const CString &globalName, CGlobalState *)
{
	return gGlobalState.EntityInTable(MAKE_STRING(globalName.Data())) != 0;
}

void RegisterSCScriptGlobalState(CASDocumentation *documentation)
{
	documentation->RegisterEnum("Global state values", "GLOBALESTATE");
	documentation->RegisterEnumValue("Global state when off", "GLOBALESTATE", "GLOBAL_OFF", GLOBAL_OFF);
	documentation->RegisterEnumValue("Global state when on", "GLOBALESTATE", "GLOBAL_ON", GLOBAL_ON);
	documentation->RegisterEnumValue("Global state when removed", "GLOBALESTATE", "GLOBAL_DEAD", GLOBAL_DEAD);
	documentation->RegisterObjectType("Global state entity", "GlobalEntity", 0, asOBJ_REF | asOBJ_NOCOUNT);
	documentation->RegisterObjectMethod("Gets the name of this global state", "GlobalEntity", "string name() const", asFUNCTION(GlobalEntity_GetName), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Name of the level where this global state was last updated", "GlobalEntity", "string levelName() const", asFUNCTION(GlobalEntity_GetLevelName), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectProperty("Global state of this entity", "GlobalEntity", "GLOBALESTATE state", offsetof(globalentity_t, state));
	documentation->RegisterObjectProperty("Next global state entity in the list", "GlobalEntity", "GlobalEntity@ next", offsetof(globalentity_t, pNext));
	documentation->RegisterObjectType("Global state manager", "CGlobalState", 0, asOBJ_REF | asOBJ_NOCOUNT);
	documentation->RegisterObjectMethod("Clears all global states", "CGlobalState", "void ClearStates()", asMETHOD(CGlobalState, ClearStates), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Adds a new global state object", "CGlobalState", "void EntityAdd(const string& in globalName, const string& in mapName , GLOBALESTATE state)", asFUNCTION(GlobalState_EntityAdd), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Sets the state of an existing global state object", "CGlobalState", "void EntitySetState(const string& in globalName, GLOBALESTATE state)", asFUNCTION(GlobalState_EntitySetState), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Updates the level name of the specified global state object", "CGlobalState", "void EntityUpdate(const string& in globalName, const string& in mapName)", asFUNCTION(GlobalState_EntityUpdate), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Finds a global state object by name", "CGlobalState", "const GlobalEntity@ EntityFromTable(const string& in globalName) const", asFUNCTION(GlobalState_EntityFromTable), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Gets the state of the specified global state object", "CGlobalState", "const GLOBALESTATE EntityGetState(const string& in globalName) const", asFUNCTION(GlobalState_EntityGetState), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Returns whether the global state object is in the table of objects", "CGlobalState", "bool EntityInTable(const string& in globalName) const", asFUNCTION(GlobalState_EntityInTable), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Dumps all global state objects to the console", "CGlobalState", "void DumpGlobals() const", asMETHOD(CGlobalState, DumpGlobals), asCALL_THISCALL);
	documentation->RegisterGlobalProperty("Global state manager instance", "CGlobalState g_GlobalState", &gGlobalState);
}
