#include "extdll.h"
#include "ASUtility.h"
#include "CASUtility.h"
#include "CASDocumentation.h"

void RegisterSCScriptUtility( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "Ignore monsters options.\nFlag for certain trace functions.", "IGNORE_MONSTERS" );
	documentation->RegisterEnumValue( "Ignore monsters", "IGNORE_MONSTERS", "ignore_monsters", 1 );
	documentation->RegisterEnumValue( "Don't ignore monsters", "IGNORE_MONSTERS", "dont_ignore_monsters", 0 );
	documentation->RegisterEnumValue( "Object doing the trace is a missile", "IGNORE_MONSTERS", "missile", 2 );
	documentation->RegisterEnum( "Ignore glass options.\nFlag for certain trace functions.", "IGNORE_GLASS" );
	documentation->RegisterEnumValue( "Ignore glass", "IGNORE_GLASS", "ignore_glass", 1 );
	documentation->RegisterEnumValue( "Don't ignore glass", "IGNORE_GLASS", "dont_ignore_glass", 0 );
	documentation->RegisterEnum( "Ignore glass options.\nFlag for certain trace functions.", "HULL_NUMBER" );
	documentation->RegisterEnumValue( "Point hull. No size.", "HULL_NUMBER", "point_hull", 0 );
	documentation->RegisterEnumValue( "Human sized hull.", "HULL_NUMBER", "human_hull", 1 );
	documentation->RegisterEnumValue( "Large hull.", "HULL_NUMBER", "large_hull", 2 );
	documentation->RegisterEnumValue( "Head hull.", "HULL_NUMBER", "head_hull", 3 );
	documentation->RegisterInterface( "Listener for CountPlayersInBrushVolume.", "PlayerInVolumeListener" );
	documentation->RegisterInterfaceMethod( "Called whenever a player is detected.", "PlayerInVolumeListener",
	                                        "void PlayerDetected(const bool fInVolume, CBasePlayer@ pPlayer)" );

	documentation->RegisterObjectType( "Utility methods", "CUtility", 0, asOBJ_REF | asOBJ_NOCOUNT );

	documentation->RegisterObjectMethod(
	    "Creates a blood stream. Use BLOOD_COLOR enum for the color.",
	    "CUtility",
	    "void BloodStream(const Vector& in vecOrigin, const Vector& in vecDirection, int iColor, int iAmount)",
	    asMETHOD( CASUtility, BloodStream ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Creates blood drips. Use BLOOD_COLOR enum for the color.",
	    "CUtility",
	    "void BloodDrips(const Vector& in vecOrigin, const Vector& in vecDirection, int iColor, int iAmount)",
	    asMETHOD( CASUtility, BloodDrips ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns a random blood direction vector",
	    "CUtility",
	    "Vector RandomBloodVector()",
	    asMETHOD( CASUtility, RandomBloodVector ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether blood of the given type should be shown.",
	    "CUtility",
	    "bool ShouldShowBlood(int iBloodColor)",
	    asMETHOD( CASUtility, ShouldShowBlood ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Applies blood decal to the trace hit location",
	    "CUtility",
	    "void BloodDecalTrace(TraceResult& in trace, int iBloodColor)",
	    asMETHOD( CASUtility, BloodDecalTrace ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Applies decal to the trace hit location",
	    "CUtility",
	    "void DecalTrace(TraceResult& in trace, int iDecalNumber)",
	    asMETHOD( CASUtility, DecalTrace ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Applies player decal to the trace hit location",
	    "CUtility",
	    "void PlayerDecalTrace(TraceResult& in trace, int iPlayerNum, int iDecalNumber, const bool bIsCustom)",
	    asMETHOD( CASUtility, PlayerDecalTrace ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Applies gunshot decal to the trace hit location",
	    "CUtility",
	    "void GunshotDecalTrace(TraceResult& in trace, int iDecalNumber)",
	    asMETHOD( CASUtility, GunshotDecalTrace ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Creates sparks at the given location",
	    "CUtility",
	    "void Sparks(const Vector& in vecPosition)",
	    asMETHOD( CASUtility, Sparks ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Creates a ricochet at the given location",
	    "CUtility",
	    "void Ricochet(const Vector& in vecPosition, float flScale)",
	    asMETHOD( CASUtility, Ricochet ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Determines the Z level at which a water surface level is, given a position to start searching from and a range to search.",
	    "CUtility",
	    "float WaterLevel(const Vector& in vecPosition, float minz, float maxz)",
	    asMETHOD( CASUtility, WaterLevel ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Creates a box filled with bubbles",
	    "CUtility",
	    "void Bubbles(const Vector& in vecMins, const Vector& in vecMaxs, int iCount)",
	    asMETHOD( CASUtility, Bubbles ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Creates a trail of bubbles",
	    "CUtility",
	    "void BubbleTrail(const Vector& in vecFrom, const Vector& in vecTo, int iCount)",
	    asMETHOD( CASUtility, BubbleTrail ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Converts the given string to a vector",
	    "CUtility",
	    "void StringToVector(Vector& out vecVector, const string& in szString, const char delimiter = char(' '))",
	    asMETHOD( CASUtility, StringToVector ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Converts the given string to an RGBA (alpha component is optional)",
	    "CUtility",
	    "void StringToRGBA(RGBA& out rgba, const string& in szString, const char delimiter = char(' '))",
	    asMETHOD( CASUtility, StringToRGBA ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether the given string is an integer",
	    "CUtility",
	    "bool IsStringInt(const string& in szString)",
	    asMETHOD( CASUtility, IsStringInt ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether the given string is a float",
	    "CUtility",
	    "bool IsStringFloat(const string& in szString)",
	    asMETHOD( CASUtility, IsStringFloat ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether the given float is a whole number, and returns the rounded number",
	    "CUtility",
	    "bool IsWholeNumber(const float flNum, int& out iRounded)",
	    asMETHOD( CASUtility, IsWholeNumber ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether the given string is a 3D vector",
	    "CUtility",
	    "bool IsString3DVec(const string& in szString)",
	    asMETHOD( CASUtility, IsString3DVec ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Calculates a trace along the given line, storing the results in ptr.",
	    "CUtility",
	    "void TraceLine(const Vector& in vecStart, const Vector& in vecEnd, IGNORE_MONSTERS igmon,edict_t@ pEntIgnore, TraceResult& out ptr)",
	    asMETHODPR( CASUtility, TraceLine, (const Vector &, const Vector &, IGNORE_MONSTERS, edict_t *, TraceResult &), void ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Calculates a trace along the given line, storing the results in ptr.",
	    "CUtility",
	    "void TraceLine(const Vector& in vecStart, const Vector& in vecEnd, IGNORE_MONSTERS igmon,IGNORE_GLASS ignoreGlass, edict_t@ pEntIgnore, TraceResult& out ptr)",
	    asMETHODPR( CASUtility, TraceLine, (const Vector &, const Vector &, IGNORE_MONSTERS, IGNORE_GLASS, edict_t *, TraceResult &), void ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Calculates a trace along the given line, storing the results in ptr, using the specified hull type.",
	    "CUtility",
	    "void TraceHull(const Vector& in vecStart, const Vector& in vecEnd, IGNORE_MONSTERS igmon,HULL_NUMBER hullNumber, edict_t@ pEntIgnore, TraceResult& out ptr)",
	    asMETHOD( CASUtility, TraceHull ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Trace toss",
	    "CUtility",
	    "void TraceToss(edict_t@ pEntity, edict_t@ pEntityToIgnore, TraceResult& out traceResult)",
	    asMETHOD( CASUtility, TraceToss ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Trace monster hull. Returns true if the trace was entirely in a solid object, or hit something.",
	    "CUtility",
	    "bool TraceMonsterHull(edict_t@ pEntity, const Vector& in vecStart, const Vector& in vecEnd, IGNORE_MONSTERS igmon, edict_t@ pEntityToIgnore, TraceResult& out ptr)",
	    asMETHOD( CASUtility, TraceMonsterHull ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "TraceModel",
	    "CUtility",
	    "void TraceModel(const Vector& in vecStart, const Vector& in vecEnd, int iHullNumber, edict_t@ pEntityToIgnore, TraceResult& out ptr)",
	    asMETHOD( CASUtility, TraceModel ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Trace texture",
	    "CUtility",
	    "string TraceTexture(edict_t@ pEntity, const Vector& in vecStart, const Vector& in vecEnd)",
	    asMETHOD( CASUtility, TraceTexture ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Convience function that finds the entity that the given entity is currently looking at.",
	    "CUtility",
	    "CBaseEntity@ FindEntityForward(CBaseEntity@ pLooker, float flMaxDistance)",
	    asMETHODPR( CASUtility, FindEntityForward, (CBaseEntity *, float), CBaseEntity * ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Convience function that finds the entity that the given entity is currently looking at. Uses a maximum distance of 12048 units.",
	    "CUtility",
	    "CBaseEntity@ FindEntityForward(CBaseEntity@ pLooker)",
	    asMETHODPR( CASUtility, FindEntityForward, (CBaseEntity *), CBaseEntity * ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the global trace data stored in g_Engine as a TraceResult object.",
	    "CUtility",
	    "TraceResult GetGlobalTrace()",
	    asMETHOD( CASUtility, GetGlobalTrace ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether the given player is in the given volume.",
	    "CUtility",
	    "bool IsPlayerInVolume(CBasePlayer@ pPlayer, CBaseEntity@ pEntityVolume)",
	    asMETHOD( CASUtility, IsPlayerInVolume ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether a vote is active. This only covers the votes started using the in-game vote menu, trigger_vote and the Vote class.",
	    "CUtility",
	    "bool VoteActive() const",
	    asMETHOD( CASUtility, VoteActive ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets circular gaussian spread.",
	    "CUtility",
	    "void GetCircularGaussianSpread(float& out x, float& out y) const",
	    asMETHOD( CASUtility, GetCircularGaussianSpread ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Counts the number of players inside and outside a brush volume.",
	    "CUtility",
	    "int CountPlayersInBrushVolume(const bool fIgnoreDeadPlayers, CBaseEntity@ pBrushVolume,int& out iOutPlayersInsideVolume, int& out iOutPlayersOutsideVolume, PlayerInVolumeListener@ pListener)",
	    asMETHOD( CASUtility, CountPlayersInBrushVolume ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Finds the hull intersection from a traceline in a given set of bounds. The bounds are relative to the input traceline's endpoint.\nThe given entity is the entity performing the traceline.",
	    "CUtility",
	    "void FindHullIntersection(const Vector& in vecSrc, TraceResult& in inTr, TraceResult& out outTr,const Vector& in vecMins, const Vector& in vecMaxs, edict_t@ pEntity, float flDistance = 1e6f)",
	    asMETHOD( CASUtility, FindHullIntersection ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Builds a generic entity log string.",
	    "CUtility",
	    "string BuildEntityLogString(const string& in szName, const string& in szUserID = NULL, const string& in szAuthID = NULL, const string& in szTeam = NULL) const",
	    asMETHOD( CASUtility, BuildEntityLogString ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Builds a generic player log string.",
	    "CUtility",
	    "string BuildPlayerLogString(const string& in szName, const string& in szUserID = NULL, const string& in szAuthID = NULL) const",
	    asMETHOD( CASUtility, BuildPlayerLogString ),
	    asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Get a player log string.",
	    "CUtility",
	    "string GetPlayerLog(edict_t@ pPlayerEdict) const",
	    asMETHOD( CASUtility, GetPlayerLog ),
	    asCALL_THISCALL );

	documentation->RegisterGlobalProperty( "Utility methods instance", "CUtility g_Utility", CASUtility::GetInstance() );
}
