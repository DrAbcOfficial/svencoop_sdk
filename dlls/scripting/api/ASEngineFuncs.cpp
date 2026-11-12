#include "scripting/api/ASEngineFuncs.h"
#include "util.h"
#include "cbase.h"
#include "scripting/core/CASDocumentation.h"
#include "combat/DamageInfo.h"
#include "monsters/monsters.h"
#include "scripting/api/ASKeyValueBuffer.h"
#include "scripting/core/CASEngineFuncs.h"
#include <new>

void ConstructTraceResult(void *memory)
{
	TraceResult *result = new(memory) TraceResult;

	memset(result, 0, sizeof(*result));
}

void CopyConstructTraceResult(const TraceResult &result, void *memory)
{
	new(memory) TraceResult(result);
}

void DestructTraceResult(void *memory)
{
	((TraceResult *)memory)->~TraceResult();
}

TraceResult &TraceResult_Assign(const TraceResult &result, TraceResult *destination)
{
	*destination = result;
	return *destination;
}

CString GetCvarName(cvar_t *cvar)
{
	return CString(cvar->name);
}

CString GetCvarString(cvar_t *cvar)
{
	return CString(cvar->string);
}

void RegisterSCScriptTraceResult(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("Trace result structure", "TraceResult", sizeof(TraceResult),
		asOBJ_VALUE | asOBJ_APP_CLASS_CDK);
	documentation->RegisterObjectBehaviour("Constructor.", "TraceResult", asBEHAVE_CONSTRUCT,
		"void TraceResult()", asFUNCTION(ConstructTraceResult), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Constructor.", "TraceResult", asBEHAVE_CONSTRUCT,
		"void TraceResult(const TraceResult& in tr)", asFUNCTION(CopyConstructTraceResult), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Destructor.", "TraceResult", asBEHAVE_DESTRUCT,
		"void DestructTraceResult()", asFUNCTION(DestructTraceResult), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Assignment operator", "TraceResult",
		"TraceResult& opAssign(const TraceResult& in tr)", asFUNCTION(TraceResult_Assign), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectProperty("Flag telling whether the trace occured in an all solid area.",
		"TraceResult", "int fAllSolid", asOFFSET(TraceResult, fAllSolid));
	documentation->RegisterObjectProperty("Flag telling whether the trace started in a solid area.",
		"TraceResult", "int fStartSolid", asOFFSET(TraceResult, fStartSolid));
	documentation->RegisterObjectProperty("Flag telling whether the trace ended in an open area.",
		"TraceResult", "int fInOpen", asOFFSET(TraceResult, fInOpen));
	documentation->RegisterObjectProperty("Flag telling whether the trace ended in water",
		"TraceResult", "int fInWater", asOFFSET(TraceResult, fInWater));
	documentation->RegisterObjectProperty("Fraction.", "TraceResult",
		"float flFraction", asOFFSET(TraceResult, flFraction));
	documentation->RegisterObjectProperty("End position.", "TraceResult",
		"Vector vecEndPos", asOFFSET(TraceResult, vecEndPos));
	documentation->RegisterObjectProperty("Plane distance.", "TraceResult",
		"float flPlaneDist", asOFFSET(TraceResult, flPlaneDist));
	documentation->RegisterObjectProperty("Plane normal", "TraceResult",
		"Vector vecPlaneNormal", asOFFSET(TraceResult, vecPlaneNormal));
	documentation->RegisterObjectProperty("Hit entity", "TraceResult",
		"edict_t@ pHit", asOFFSET(TraceResult, pHit));
	documentation->RegisterObjectProperty("Hit group.", "TraceResult",
		"int iHitgroup", asOFFSET(TraceResult, iHitgroup));
}

void RegisterSCScriptDamageInfo(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("Damage info structure", "DamageInfo", 0, asOBJ_REF | asOBJ_NOCOUNT);
	documentation->RegisterObjectProperty("Victim entity.", "DamageInfo",
		"CBaseEntity@ pVictim", asOFFSET(DamageInfo, pVictim));
	documentation->RegisterObjectProperty("Inflictor entity.", "DamageInfo",
		"CBaseEntity@ pInflictor", asOFFSET(DamageInfo, pInflictor));
	documentation->RegisterObjectProperty("Attacker entity.", "DamageInfo",
		"CBaseEntity@ pAttacker", asOFFSET(DamageInfo, pAttacker));
	documentation->RegisterObjectProperty("Damage amount.", "DamageInfo",
		"float flDamage", asOFFSET(DamageInfo, flDamage));
	documentation->RegisterObjectProperty("Damage type.", "DamageInfo",
		"int bitsDamageType", asOFFSET(DamageInfo, bitsDamageType));
}

void RegisterSCScriptCvarFlags(CASDocumentation *documentation)
{
	documentation->RegisterEnum("Console variable flags.", "FCVAR");
	documentation->RegisterEnumValue("Save this cvar.", "FCVAR", "FCVAR_ARCHIVE", FCVAR_ARCHIVE);
	documentation->RegisterEnumValue("Changes the client's info string.", "FCVAR", "FCVAR_USERINFO", FCVAR_USERINFO);
	documentation->RegisterEnumValue("Notifies players when changed.", "FCVAR", "FCVAR_SERVER", FCVAR_SERVER);
	documentation->RegisterEnumValue("Defined by external dll.", "FCVAR", "FCVAR_EXTDLL", FCVAR_EXTDLL);
	documentation->RegisterEnumValue("Defined by the client dll.", "FCVAR", "FCVAR_CLIENTDLL", FCVAR_CLIENTDLL);
	documentation->RegisterEnumValue(
		"It's a server cvar, but we don't send the data since it's a password, etc.",
		"FCVAR", "FCVAR_PROTECTED", FCVAR_PROTECTED);
	documentation->RegisterEnumValue(
		"This cvar cannot be changed by clients connected to a multiplayer server.",
		"FCVAR", "FCVAR_SPONLY", FCVAR_SPONLY);
	documentation->RegisterEnumValue(
		"This cvar's string cannot contain unprintable characters (e.g. used for player name etc)",
		"FCVAR", "FCVAR_PRINTABLEONLY", FCVAR_PRINTABLEONLY);
	documentation->RegisterEnumValue(
		"If this is a FCVAR_SERVER, don't log changes to the log file / console if we are creating a log",
		"FCVAR", "FCVAR_UNLOGGED", FCVAR_UNLOGGED);
}

void RegisterSCScriptCvar(CASDocumentation *documentation)
{
	RegisterSCScriptCvarFlags(documentation);
	documentation->RegisterObjectType("Cvar structure", "Cvar", 0, asOBJ_REF | asOBJ_NOCOUNT);
	documentation->RegisterObjectMethod("Gets the name of the cvar.", "Cvar",
		"string GetName()", asFUNCTION(GetCvarName), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Gets the value of the cvar as a string.", "Cvar",
		"string GetString()", asFUNCTION(GetCvarString), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectProperty("The flags of this cvar.", "Cvar",
		"const int flags", asOFFSET(cvar_t, flags));
	documentation->RegisterObjectProperty("The value of this cvar as a float.", "Cvar",
		"const float value", asOFFSET(cvar_t, value));
}

void RegisterSCScriptEngineMoveType(CASDocumentation *documentation)
{
	documentation->RegisterEnum("MoveToOrigin move type.", "MOVE_TYPE");
	documentation->RegisterEnumValue("Normal move in the direction monster is facing.",
		"MOVE_TYPE", "MOVE_NORMAL", MOVE_NORMAL);
	documentation->RegisterEnumValue("Moves in direction specified, no matter which way monster is facing.",
		"MOVE_TYPE", "MOVE_STRAFE", MOVE_STRAFE);
}

void RegisterSCScriptEngineFuncs( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "Walk move types", "WALKMOVE" );
	documentation->RegisterEnumValue( "normal walkmove", "WALKMOVE", "WALKMOVE_NORMAL", 0 );
	documentation->RegisterEnumValue( "doesn't hit ANY entities, no matter what the solid type",
	                                  "WALKMOVE", "WALKMOVE_WORLDONLY", 1 );
	documentation->RegisterEnumValue( "move, but don't touch triggers", "WALKMOVE", "WALKMOVE_CHECKONLY", 2 );

	documentation->RegisterEnum( "Contents of a spot in the world", "CONTENTS" );
	documentation->RegisterEnumValue( "", "CONTENTS", "CONTENTS_EMPTY", CONTENTS_EMPTY );
	documentation->RegisterEnumValue( "", "CONTENTS", "CONTENTS_SOLID", CONTENTS_SOLID );
	documentation->RegisterEnumValue( "", "CONTENTS", "CONTENTS_WATER", CONTENTS_WATER );
	documentation->RegisterEnumValue( "", "CONTENTS", "CONTENTS_SLIME", CONTENTS_SLIME );
	documentation->RegisterEnumValue( "", "CONTENTS", "CONTENTS_LAVA", CONTENTS_LAVA );
	documentation->RegisterEnumValue( "", "CONTENTS", "CONTENTS_SKY", CONTENTS_SKY );
	documentation->RegisterEnumValue( "", "CONTENTS", "CONTENTS_LADDER", CONTENTS_LADDER );

	RegisterSCScriptTraceResult( documentation );
	RegisterSCScriptDamageInfo( documentation );
	RegisterSCScriptCvar( documentation );

	documentation->RegisterEnum( "Print type", "PRINT_TYPE" );
	documentation->RegisterEnumValue( "Print to the console", "PRINT_TYPE", "print_console", print_console );
	documentation->RegisterEnumValue( "Print to the center of the screen", "PRINT_TYPE", "print_center", print_center );
	documentation->RegisterEnumValue(
	    "Print to chat. Note: does not work. Use CPlayerFuncs::ClientPrint with HUD_PRINTTALK instead.",
	    "PRINT_TYPE", "print_chat", print_chat );

	RegisterASKeyValueBuffer( documentation );
	RegisterSCScriptEngineMoveType( documentation );

	documentation->RegisterObjectType( "Engine functions", "CEngineFuncs", 0, asOBJ_REF | asOBJ_NOCOUNT );

	documentation->RegisterObjectMethod(
	    "Gets the model index of a model.",
	    "CEngineFuncs",
	    "int ModelIndex(const string& in szModelName)",
	    asMETHOD( CASEngineFuncs, ModelIndex ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the number of frames in a model.",
	    "CEngineFuncs",
	    "int ModelFrames(int iModelIndex)",
	    asMETHOD( CASEngineFuncs, ModelFrames ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Changes the current level to the one specified.",
	    "CEngineFuncs",
	    "void ChangeLevel(const string& in szLevelName)",
	    asMETHOD( CASEngineFuncs, ChangeLevel ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Vector to yaw",
	    "CEngineFuncs",
	    "float VecToYaw(const Vector& in vec)",
	    asMETHOD( CASEngineFuncs, VecToYaw ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Vector to angles",
	    "CEngineFuncs",
	    "void VecToAngles(const Vector& in vec, Vector& out angles)",
	    asMETHOD( CASEngineFuncs, VecToAngles ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Move entity to origin.",
	    "CEngineFuncs",
	    "void MoveToOrigin(edict_t@ pEntity, const Vector& in vecGoal, float flDist, int iMoveType)",
	    asMETHOD( CASEngineFuncs, MoveToOrigin ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Change yaw. This updates entvars_t::angles[ 1 ] to approach entvars_t::ideal_yaw, at entvars_t::yaw_speed "
	    "degrees speed.",
	    "CEngineFuncs",
	    "void ChangeYaw(edict_t@ pEntity)",
	    asMETHOD( CASEngineFuncs, ChangeYaw ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Change pitch. This updates entvars_t::angles[ 0 ] to approach entvars_t::idealpitch, at entvars_t::pitch_speed "
	    "degrees speed.",
	    "CEngineFuncs",
	    "void ChangePitch(edict_t@ pEntity)",
	    asMETHOD( CASEngineFuncs, ChangePitch ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Get entity illumination. This is a weighted value between 0 and 255, inclusive.",
	    "CEngineFuncs",
	    "int GetEntityIllum(edict_t@ pEntity)",
	    asMETHOD( CASEngineFuncs, GetEntityIllum ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Find client in Potentially Visible Set.",
	    "CEngineFuncs",
	    "edict_t@ FindClientInPVS(edict_t@ pStartEntity)",
	    asMETHOD( CASEngineFuncs, FindClientInPVS ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Find entities in Potentially Visible Set. Edict::vars::chain is used for this list, and is temporary.",
	    "CEngineFuncs",
	    "edict_t@ EntitiesInPVS(edict_t@ pStartEntity)",
	    asMETHOD( CASEngineFuncs, EntitiesInPVS ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Make vectors. The results are stored in g_Engine::v_forward, v_right, and v_up.",
	    "CEngineFuncs",
	    "void MakeVectors(const Vector& in vec)",
	    asMETHOD( CASEngineFuncs, MakeVectors ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Angle vectors",
	    "CEngineFuncs",
	    "void AngleVectors(const Vector& in vecIn, Vector& out vecForward, Vector& out vecRight, Vector& out vecUp)",
	    asMETHOD( CASEngineFuncs, AngleVectors ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Make entity static. This moves the entity to the client.Its current visual state is copied over, and the entity "
	    "is removed on the server side.Only valid while the level is loading.",
	    "CEngineFuncs",
	    "void MakeStatic(edict_t@ pEntity)",
	    asMETHOD( CASEngineFuncs, MakeStatic ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether entity is on the floor.",
	    "CEngineFuncs",
	    "int EntIsOnFloor(edict_t@ pEntity)",
	    asMETHOD( CASEngineFuncs, EntIsOnFloor ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Drop entity to floor.",
	    "CEngineFuncs",
	    "int DropToFloor(edict_t@ pEntity)",
	    asMETHOD( CASEngineFuncs, DropToFloor ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Make entity walk.\nSee WALKMOVE enum",
	    "CEngineFuncs",
	    "int WalkMove(edict_t@ pEntity, float flYaw, float flDist, int iMove)",
	    asMETHOD( CASEngineFuncs, WalkMove ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Get aim vector for entity.",
	    "CEngineFuncs",
	    "void GetAimVector(edict_t@ pEntity, float flSpeed, Vector& out vecReturn)",
	    asMETHOD( CASEngineFuncs, GetAimVector ), asCALL_THISCALL );

	asDWORD accessMask = documentation->SetDefaultAccessMask( AS_ACCESS_PLUGIN );

	documentation->RegisterObjectMethod(
	    "Issues a command to the server. Plugins only.",
	    "CEngineFuncs",
	    "void ServerCommand(const string& in szCommand)",
	    asMETHOD( CASEngineFuncs, ServerCommand ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Executes all pending server commands. Plugins only.",
	    "CEngineFuncs",
	    "void ServerExecute()",
	    asMETHOD( CASEngineFuncs, ServerExecute ), asCALL_THISCALL );

	documentation->SetDefaultAccessMask( accessMask );

	documentation->RegisterObjectMethod(
	    "Emit a particle effect.",
	    "CEngineFuncs",
	    "void ParticleEffect(const Vector& in vecOrigin, const Vector& in vecDir, float flColor, float flCount)",
	    asMETHOD( CASEngineFuncs, ParticleEffect ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Set a light style.",
	    "CEngineFuncs",
	    "void LightStyle(int iStyle, const string& in szVal)",
	    asMETHOD( CASEngineFuncs, LightStyle ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Get decal index.",
	    "CEngineFuncs",
	    "int DecalIndex(const string& in szName)",
	    asMETHOD( CASEngineFuncs, DecalIndex ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Check if point is in contents.",
	    "CEngineFuncs",
	    "CONTENTS PointContents(const Vector& in vecPoint)",
	    asMETHOD( CASEngineFuncs, PointContents ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Get cvar float value.",
	    "CEngineFuncs",
	    "float CVarGetFloat(const string& in szCvar)",
	    asMETHOD( CASEngineFuncs, CVarGetFloat ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Get cvar string value.",
	    "CEngineFuncs",
	    "string CVarGetString(const string& in szCvar)",
	    asMETHOD( CASEngineFuncs, CVarGetString ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Set cvar float value.",
	    "CEngineFuncs",
	    "void CVarSetFloat(const string& in szCvar, float flValue)",
	    asMETHOD( CASEngineFuncs, CVarSetFloat ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Set cvar string value.",
	    "CEngineFuncs",
	    "void CVarSetString(const string& in szCvar, const string& in szValue)",
	    asMETHOD( CASEngineFuncs, CVarSetString ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets entity variables of entity.",
	    "CEngineFuncs",
	    "entvars_t@ GetVarsOfEnt(edict_t@ pEdict)",
	    asMETHOD( CASEngineFuncs, GetVarsOfEnt ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Get index of edict.",
	    "CEngineFuncs",
	    "int IndexOfEdict(const edict_t@ pEdict)",
	    asMETHOD( CASEngineFuncs, IndexOfEdict ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Get entity by entity index.",
	    "CEngineFuncs",
	    "edict_t@ PEntityOfEntIndex(int iIndex)",
	    asMETHOD( CASEngineFuncs, PEntityOfEntIndex ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Find entity by variables.",
	    "CEngineFuncs",
	    "edict_t@ FindEntityByVars(entvars_t@ pVars)",
	    asMETHOD( CASEngineFuncs, FindEntityByVars ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Get bone position.",
	    "CEngineFuncs",
	    "void GetBonePosition(const edict_t@ pEdict, int iBone, Vector& out vecOrigin, Vector& out vecAngles)",
	    asMETHOD( CASEngineFuncs, GetBonePosition ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Client print",
	    "CEngineFuncs",
	    "void ClientPrintf(CBasePlayer@ pPlayer, PRINT_TYPE printType, const string& in szMessage)",
	    asMETHOD( CASEngineFuncs, ClientPrintf ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Server print",
	    "CEngineFuncs",
	    "void ServerPrint(const string& in szMessage)",
	    asMETHOD( CASEngineFuncs, ServerPrint ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Get command arguments.",
	    "CEngineFuncs",
	    "string Cmd_Args()",
	    asMETHOD( CASEngineFuncs, Cmd_Args ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Get command argument by index.",
	    "CEngineFuncs",
	    "string Cmd_Argv(int iIndex)",
	    asMETHOD( CASEngineFuncs, Cmd_Argv ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Get number of command arguments.",
	    "CEngineFuncs",
	    "int Cmd_Argc()",
	    asMETHOD( CASEngineFuncs, Cmd_Argc ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Get attachment position.",
	    "CEngineFuncs",
	    "void GetAttachment(const edict_t@ pEdict, int iAttachment, Vector& out vecOrigin, Vector& out vecAngles)",
	    asMETHOD( CASEngineFuncs, GetAttachment ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Set view.",
	    "CEngineFuncs",
	    "void SetView(const edict_t@ pEdict, const edict_t@ pViewEntity)",
	    asMETHOD( CASEngineFuncs, SetView ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Get current time.",
	    "CEngineFuncs",
	    "float Time()",
	    asMETHOD( CASEngineFuncs, Time ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets the angles of the given player's crosshairs to the given settings. Set both to 0 to disable. May not have "
	    "any effect in SC.",
	    "CEngineFuncs",
	    "void CrosshairAngle(const edict_t@ pEdict, float flPitch, float flYaw)",
	    asMETHOD( CASEngineFuncs, CrosshairAngle ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Get game directory.",
	    "CEngineFuncs",
	    "string GetGameDir()",
	    asMETHOD( CASEngineFuncs, GetGameDir ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Run player move.",
	    "CEngineFuncs",
	    "void RunPlayerMove(edict_t@ pEdict, const Vector& in vecViewAngles, float flFowardMove, float flSideMove, float "
	    "flUpMove, uint16 iButtons, uint8 iImpulse, uint8 iMsec)",
	    asMETHOD( CASEngineFuncs, RunPlayerMove ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Get number of entities.",
	    "CEngineFuncs",
	    "int NumberOfEntities()",
	    asMETHOD( CASEngineFuncs, NumberOfEntities ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Get info key buffer.",
	    "CEngineFuncs",
	    "KeyValueBuffer@ GetInfoKeyBuffer(edict_t@ pEdict)",
	    asMETHOD( CASEngineFuncs, GetInfoKeyBuffer ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Get physics key buffer.",
	    "CEngineFuncs",
	    "KeyValueBuffer@ GetPhysicsKeyBuffer(edict_t@ pEdict)",
	    asMETHOD( CASEngineFuncs, GetPhysicsKeyBuffer ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether the given map is valid.",
	    "CEngineFuncs",
	    "bool IsMapValid(const string& in szMapName) const",
	    asMETHOD( CASEngineFuncs, IsMapValid ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Project a static decal.",
	    "CEngineFuncs",
	    "void StaticDecal(const Vector& in vecOrigin, int iDecalIndex, int iEntityIndex, int iModelIndex)",
	    asMETHOD( CASEngineFuncs, StaticDecal ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Get player user ID. This is a unique id generated when a player connects. It remains valid as long as the "
	    "player stays connected to the server.",
	    "CEngineFuncs",
	    "int GetPlayerUserId(edict_t@ pEdict)",
	    asMETHOD( CASEngineFuncs, GetPlayerUserId ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this is a dedicated server.",
	    "CEngineFuncs",
	    "bool IsDedicatedServer() const",
	    asMETHOD( CASEngineFuncs, IsDedicatedServer ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets a pointer to a cvar.",
	    "CEngineFuncs",
	    "const Cvar@ CVarGetPointer(const string& in szCvar) const",
	    asMETHOD( CASEngineFuncs, CVarGetPointer ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Get player stats.",
	    "CEngineFuncs",
	    "void GetPlayerStats(const edict_t@ pEdict, int& out iPing, int& out iPacketLoss)",
	    asMETHOD( CASEngineFuncs, GetPlayerStats ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Voice system - Get client listening.",
	    "CEngineFuncs",
	    "bool Voice_GetClientListening(int iReceiver, int iSender)",
	    asMETHOD( CASEngineFuncs, Voice_GetClientListening ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Voice system - Set client listening.",
	    "CEngineFuncs",
	    "bool Voice_SetClientListening(int iReceiver, int iSender, bool fListen)",
	    asMETHOD( CASEngineFuncs, Voice_SetClientListening ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Get the player's Auth ID. (WON/Steam ID)",
	    "CEngineFuncs",
	    "string GetPlayerAuthId(edict_t@ pEdict)",
	    asMETHOD( CASEngineFuncs, GetPlayerAuthId ), asCALL_THISCALL );

	documentation->RegisterGlobalProperty( "Engine functions instance",
	                                       "CEngineFuncs g_EngineFuncs", CASEngineFuncs::GetInstance() );
}
