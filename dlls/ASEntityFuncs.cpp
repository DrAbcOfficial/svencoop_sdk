#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "ASEntityFuncs.h"
#include "CASEntityFuncs.h"
#include "CASDocumentation.h"

void RegisterSCScriptEntityFuncs( CASDocumentation *documentation )
{
	documentation->RegisterObjectType( "Global entity functions", "CEntityFuncs", 0, asOBJ_REF | asOBJ_NOCOUNT );

	documentation->RegisterObjectMethod(
	    "Create an entity by class name. Pass keyvalues in using the dictionary, if any. Keyvalues must be strings.\nThe "
	    "class name can be either one of the entities included with the game or a custom entity.",
	    "CEntityFuncs",
	    "CBaseEntity@ CreateEntity(const string& in szClassName, dictionary@ pDictionary = null, bool fSpawn = true)",
	    asMETHODPR( CASEntityFuncs, CreateEntity, (const CString &, CScriptDictionary *, bool), CBaseEntity * ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Creates an entity by class name.",
	    "CEntityFuncs",
	    "CBaseEntity@ Create(const string& in szClassname, const Vector& in vecOrigin, const Vector& in vecAngles, bool "
	    "fCreateAndDontSpawn, edict_t@ entOwner = null)",
	    asMETHOD( CASEntityFuncs, Create ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Creates a decal with the specified name.\nIf given a name, has to be triggered to be placed, otherwise, is placed immediately.",
	    "CEntityFuncs",
	    "void CreateDecal(const string& in szDecalName, const Vector& in vecOrigin, const string& in szTargetName = \"\")",
	    asMETHOD( CASEntityFuncs, CreateDecal ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Creates a sprite. flFrameRate must be set to a valid value to animate the sprite.",
	    "CEntityFuncs",
	    "CSprite@ CreateSprite(const string& in szSpriteName, const Vector& in vecOrigin, bool fAnimated, float flFrameRate = 10.0f)",
	    asMETHOD( CASEntityFuncs, CreateSprite ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Creates a beam entity with the given sprite and width set.",
	    "CEntityFuncs",
	    "CBeam@ CreateBeam(const string& in szSpriteName, int width)",
	    asMETHOD( CASEntityFuncs, CreateBeam ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Creates an explosion.\nIf fDoDamage is true, also deals damage to entities within its effect radius, which is "
	    "determined by the magnitude.",
	    "CEntityFuncs",
	    "void CreateExplosion(const Vector& in vecCenter, const Vector& in vecAngles, edict_t@ pOwner, int iMagnitude, bool fDoDamage)",
	    asMETHOD( CASEntityFuncs, CreateExplosion ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Creates a displacer portal.",
	    "CEntityFuncs",
	    "CBaseEntity@ CreateDisplacerPortal(const Vector& in vecOrigin, const Vector& in vecVelocity, edict_t@ pOwner, "
	    "float flDamage, float flRadius)",
	    asMETHOD( CASEntityFuncs, CreateDisplacerPortal ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Creates an rpg rocket.",
	    "CEntityFuncs",
	    "CBaseEntity@ CreateRPGRocket(const Vector& in vecOrigin, const Vector& in vecAngles, edict_t@ pOwner)",
	    asMETHODPR( CASEntityFuncs, CreateRPGRocket, (const Vector &, const Vector &, edict_t *), CBaseEntity * ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Creates an rpg rocket with custom velocity.",
	    "CEntityFuncs",
	    "CBaseEntity@ CreateRPGRocket(const Vector& in vecOrigin, const Vector& in vecAngles, const Vector& in vecVelocity, edict_t@ pOwner)",
	    asMETHODPR( CASEntityFuncs, CreateRPGRocket, (const Vector &, const Vector &, const Vector &, edict_t *), CBaseEntity * ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Creates a server side gib.",
	    "CEntityFuncs",
	    "CGib@ CreateGib( const Vector& in vecOrigin, const Vector& in vecAngles)",
	    asMETHOD( CASEntityFuncs, CreateGib ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Spawns a timed grenade.",
	    "CEntityFuncs",
	    "CGrenade@ ShootTimed(entvars_t@ pevOwner, const Vector& in vecStart, const Vector& in vecVelocity, float flTime)",
	    asMETHOD( CASEntityFuncs, ShootTimed ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Spawns a contact grenade.",
	    "CEntityFuncs",
	    "CGrenade@ ShootContact(entvars_t@ pevOwner, const Vector& in vecStart, const Vector& in vecVelocity)",
	    asMETHOD( CASEntityFuncs, ShootContact ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Spawns a mortar grenade.",
	    "CEntityFuncs",
	    "CGrenade@ ShootMortar(entvars_t@ pevOwner, const Vector& in vecStart, const Vector& in vecVelocity)",
	    asMETHOD( CASEntityFuncs, ShootMortar ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Spawn a banana bomb.",
	    "CEntityFuncs",
	    "CGrenade@ ShootBananaCluster(entvars_t@ pevOwner, const Vector& in vecStart, const Vector& in vecVelocity)",
	    asMETHOD( CASEntityFuncs, ShootBananaCluster ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Triggers all satchel charges owned by pevOwner to either be released from ownership, or detonated.",
	    "CEntityFuncs",
	    "void UseSatchelCharges(entvars_t@ pevOwner, SATCHELCODE code)",
	    asMETHOD( CASEntityFuncs, UseSatchelCharges ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Spawns a head gib for the target victim.",
	    "CEntityFuncs",
	    "void SpawnHeadGib(entvars_t@ pevVictim)",
	    asMETHOD( CASEntityFuncs, SpawnHeadGib ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Spawns random gibs for the target victim.\niHuman specifies whether the gibs are human or alien.",
	    "CEntityFuncs",
	    "void SpawnRandomGibs(entvars_t@ pevVictim, int cGibs, int iHuman)",
	    asMETHOD( CASEntityFuncs, SpawnRandomGibs ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Spawns sticky gibs for the target victim.\ncGibs specifies how many gibs there are.",
	    "CEntityFuncs",
	    "void SpawnStickyGibs(entvars_t@ pevVictim, const Vector& in vecOrigin, int cGibs)",
	    asMETHOD( CASEntityFuncs, SpawnStickyGibs ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Set the origin of an entity",
	    "CEntityFuncs",
	    "void SetOrigin(CBaseEntity@ entity, const Vector& in vecOrigin)",
	    asMETHOD( CASEntityFuncs, SetOrigin ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Set the model of an entity",
	    "CEntityFuncs",
	    "void SetModel(CBaseEntity@ entity, const string& in szFileName)",
	    asMETHOD( CASEntityFuncs, SetModel ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Set the size of an entity",
	    "CEntityFuncs",
	    "void SetSize(entvars_t@ pev, const Vector& in vecMin, const Vector& in vecMax)",
	    asMETHOD( CASEntityFuncs, SetSize ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Finds an entity in a sphere",
	    "CEntityFuncs",
	    "CBaseEntity@ FindEntityInSphere(CBaseEntity@ pStartEntity, const Vector& in vecCenter, float flRadius,const "
	    "string& in szValue = \"\", const string& in szKeyword = \"targetname\")",
	    asMETHOD( CASEntityFuncs, FindEntityInSphere ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Finds an entity based on a key value pair",
	    "CEntityFuncs",
	    "CBaseEntity@ FindEntityByString(CBaseEntity@ pStartEntity,const string& in szKeyword, const string& in szValue)",
	    asMETHOD( CASEntityFuncs, FindEntityByString ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Finds an entity by class name",
	    "CEntityFuncs",
	    "CBaseEntity@ FindEntityByClassname(CBaseEntity@ startEntity, const string& in name)",
	    asMETHOD( CASEntityFuncs, FindEntityByClassname ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Finds an entity by target name",
	    "CEntityFuncs",
	    "CBaseEntity@ FindEntityByTargetname(CBaseEntity@ startEntity, const string& in name)",
	    asMETHOD( CASEntityFuncs, FindEntityByTargetname ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Finds an entity by target name in a sphere",
	    "CEntityFuncs",
	    "CBaseEntity@ FindEntityGeneric(const string& in szName, const Vector& in vecSrc, float flRadius)",
	    asMETHOD( CASEntityFuncs, FindEntityGeneric ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Finds monsters in a sphere",
	    "CEntityFuncs",
	    "int MonstersInSphere(array<CBaseEntity@>@ pArray, const Vector& in vecCenter, float flRadius)",
	    asMETHOD( CASEntityFuncs, MonstersInSphere ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Finds entities in a box",
	    "CEntityFuncs",
	    "int EntitiesInBox(array<CBaseEntity@>@ pArray, const Vector& in mins, const Vector& in maxs, int flagMask)",
	    asMETHOD( CASEntityFuncs, EntitiesInBox ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Finds targets in a box",
	    "CEntityFuncs",
	    "int TargetsInBox(array<CBaseEntity@>@ pArray, const Vector& in mins, const Vector& in maxs)",
	    asMETHOD( CASEntityFuncs, TargetsInBox ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Finds brush entities in a box",
	    "CEntityFuncs",
	    "int BrushEntsInBox(array<CBaseEntity@>@ pArray, const Vector& in mins, const Vector& in maxs )",
	    asMETHOD( CASEntityFuncs, BrushEntsInBox ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Dispatches the spawning of an entity",
	    "CEntityFuncs",
	    "int DispatchSpawn(edict_t@ entity)",
	    asMETHOD( CASEntityFuncs, DispatchSpawn ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Dispatches the passing of a key value pair to an entity",
	    "CEntityFuncs",
	    "bool DispatchKeyValue(edict_t@ entity, const string& in szKeyName, const string& in szValue)",
	    asMETHOD( CASEntityFuncs, DispatchKeyValue ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Handles the initialization of keyvalues located in the entity's entvars_t object.",
	    "CEntityFuncs",
	    "bool EntvarsKeyvalue(edict_t@ entity, const string& in szKeyName, const string& in szValue)",
	    asMETHOD( CASEntityFuncs, EntvarsKeyvalue ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Dispatches the initialization of the collision box of an entity",
	    "CEntityFuncs",
	    "void DispatchObjectCollisionBox(edict_t@ entity)",
	    asMETHOD( CASEntityFuncs, DispatchObjectCollisionBox ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the entity instance of an edict instance",
	    "CEntityFuncs",
	    "CBaseEntity@ Instance(edict_t@ edict)",
	    asMETHODPR( CASEntityFuncs, Instance, (edict_t *), CBaseEntity * ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the entity instance of an entvars instance",
	    "CEntityFuncs",
	    "CBaseEntity@ Instance(entvars_t@ vars)",
	    asMETHODPR( CASEntityFuncs, Instance, (entvars_t *), CBaseEntity * ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the entity instance based on an edict number",
	    "CEntityFuncs",
	    "CBaseEntity@ Instance(int iEdictNum)",
	    asMETHODPR( CASEntityFuncs, Instance, (int), CBaseEntity * ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Triggers targets. If flDelay is nonzero, causes a temporary entity to be spawned to trigger the target at the "
	    "given time.\nThe temporary entity becomes the caller, not the entity passed in.",
	    "CEntityFuncs",
	    "void FireTargets(const string& in szTargetName, CBaseEntity@ pActivator, CBaseEntity@ pCaller, USE_TYPE useType,"
	    " float flValue = 0.0f, float flDelay = 0.0f)",
	    asMETHOD( CASEntityFuncs, FireTargets ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether the master with the given name would be triggered if activated by the given entity.",
	    "CEntityFuncs",
	    "bool IsMasterTriggered(const string& in szMaster, CBaseEntity@ pActivator)",
	    asMETHOD( CASEntityFuncs, IsMasterTriggered ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Removes the given entity before the next frame starts.",
	    "CEntityFuncs",
	    "void Remove(CBaseEntity@ pEntity)",
	    asMETHOD( CASEntityFuncs, Remove ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this is a valid entity.",
	    "CEntityFuncs",
	    "bool IsValidEntity(edict_t@ pEntity)",
	    asMETHOD( CASEntityFuncs, IsValidEntity ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the index for the given edict.",
	    "CEntityFuncs",
	    "int EntIndex(edict_t@ pEdict)",
	    asMETHOD( CASEntityFuncs, EntIndex ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the edict for the given index.",
	    "CEntityFuncs",
	    "edict_t@ IndexEnt(int iEdictNum)",
	    asMETHOD( CASEntityFuncs, IndexEnt ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "tosses a brass shell from passed origin at passed velocity.",
	    "CEntityFuncs",
	    "void EjectBrass(const Vector& in vecOrigin, const Vector& in vecVelocity, float flRotation, int iModel, TE_BOUNCE soundtype)",
	    asMETHOD( CASEntityFuncs, EjectBrass ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Casts an CBaseEntity@ representing a custom entity to the ScriptClassBaseClass type",
	    "CEntityFuncs",
	    "ScriptClassInterface@ CastToScriptClass(CBaseEntity@ pEntity)",
	    asMETHOD( CASEntityFuncs, CastToScriptClass ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns a randomly selected entity that has the given target name.",
	    "CEntityFuncs",
	    "CBaseEntity@ RandomTargetname(const string& in szTargetname)",
	    asMETHOD( CASEntityFuncs, RandomTargetname ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Precaches the sounds used by a particular material",
	    "CEntityFuncs",
	    "void PrecacheMaterialSounds(const Materials material)",
	    asMETHOD( CASEntityFuncs, PrecacheMaterialSounds ), asCALL_THISCALL );

	documentation->RegisterGlobalProperty( "Global entity functions instance",
	                                       "CEntityFuncs g_EntityFuncs", CASEntityFuncs::GetInstance() );
}
