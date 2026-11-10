#ifndef ASBASEENTITY_H
#define ASBASEENTITY_H

#include "ASEntityHeaderSupport.h"
#include "CString.h"

class CScriptDictValue;

void RegisterSCScriptEntityInternal( CASDocumentation *documentation, const char *name );

template <class T>
bool BaseEntity_KeyValue( const CString &key, const CString &value, T *entity )
{
	KeyValueData keyValue = { entity->pev ? STRING( entity->pev->classname ) : "", key.Data(), value.Data(), FALSE };

	entity->T::KeyValue( &keyValue );
	return keyValue.fHandled != FALSE;
}

template <class T>
void BaseEntity_Precache( T *entity )
{
	entity->T::Precache();
}

template <class T>
void BaseEntity_PreSpawn( T *entity )
{
	entity->T::PreSpawn();
}

template <class T>
void BaseEntity_Spawn( T *entity )
{
	entity->T::Spawn();
}

template <class T>
void BaseEntity_PostSpawn( T *entity )
{
	entity->T::PostSpawn();
}

template <class T>
void BaseEntity_Think( T *entity )
{
	entity->T::Think();
}

template <class T>
void BaseEntity_Touch( CBaseEntity *other, T *entity )
{
	entity->T::Touch( other );
}

template <class T>
void BaseEntity_Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value, T *entity )
{
	entity->T::Use( activator, caller, useType, value );
}

template <class T>
void BaseEntity_Blocked( CBaseEntity *other, T *entity )
{
	entity->T::Blocked( other );
}

template <class T>
void BaseEntity_UpdateOnRemove( T *entity )
{
	entity->T::UpdateOnRemove();
}

template <class T>
int BaseEntity_ObjectCaps( T *entity )
{
	return entity->T::ObjectCaps();
}

template <class T>
void BaseEntity_TraceAttack( entvars_t *attacker, float damage, const Vector &direction,
                             TraceResult &trace, int damageType, T *entity )
{
	entity->T::TraceAttack( attacker, damage, direction, &trace, damageType );
}

template <class T>
int BaseEntity_TakeDamage( entvars_t *inflictor, entvars_t *attacker, float damage, int damageType, T *entity )
{
	return entity->T::TakeDamage( inflictor, attacker, damage, damageType );
}

// Cap arguments are integers even where the script declarations say float
template <class T>
bool BaseEntity_TakeHealth( float health, int damageType, int healthCap, T *entity )
{
	return entity->T::TakeHealth( health, damageType, healthCap );
}

template <class T>
bool BaseEntity_TakeArmor( float armor, int damageType, int armorCap, T *entity )
{
	return entity->T::TakeArmor( armor, damageType, armorCap );
}

template <class T>
void BaseEntity_Killed( entvars_t *attacker, int gib, T *entity )
{
	entity->T::Killed( attacker, gib );
}

template <class T>
int BaseEntity_BloodColor( T *entity )
{
	return entity->T::BloodColor();
}

template <class T>
void BaseEntity_TraceBleed( float damage, const Vector &direction, TraceResult *trace, int damageType, T *entity )
{
	entity->T::TraceBleed( damage, direction, trace, damageType );
}

template <class T>
float BaseEntity_GetPointsForDamage( float damage, T *entity )
{
	return entity->T::GetPointsForDamage( damage );
}

template <class T>
bool BaseEntity_IsRevivable( T *entity )
{
	return entity->T::IsRevivable();
}

template <class T>
void BaseEntity_BeginRevive( float timeUntilRevive, T *entity )
{
	entity->T::BeginRevive( timeUntilRevive );
}

template <class T>
void BaseEntity_EndRevive( float timeUntilRevive, T *entity )
{
	entity->T::EndRevive( timeUntilRevive );
}

template <class T>
void RegisterSCScriptBaseBaseClassEntity( CASDocumentation *documentation, const char *description, const char *name )
{
	documentation->RegisterObjectType( description, name, sizeof( T ), asOBJ_REF | asOBJ_NOCOUNT );

	RegisterASScriptEntityHeaderCasts<CBaseEntity, T>( documentation, "BaseEntity", name );

	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of KeyValue", name,
	    "bool KeyValue(const string& in szKeyName, const string& in szValue)",
	    asFUNCTION( BaseEntity_KeyValue<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "Calls the base class implementation of Precache", name,
	                                     "void Precache()", asFUNCTION( BaseEntity_Precache<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "Calls the base class implementation of PreSpawn", name,
	                                     "void PreSpawn()", asFUNCTION( BaseEntity_PreSpawn<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "Calls the base class implementation of Spawn", name,
	                                     "void Spawn()", asFUNCTION( BaseEntity_Spawn<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "Calls the base class implementation of PostSpawn", name,
	                                     "void PostSpawn()", asFUNCTION( BaseEntity_PostSpawn<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "Calls the base class implementation of Think", name,
	                                     "void Think()", asFUNCTION( BaseEntity_Think<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "Calls the base class implementation of Touch", name,
	                                     "void Touch(CBaseEntity@ pOther)", asFUNCTION( BaseEntity_Touch<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of Use", name,
	    "void Use(CBaseEntity@ pActivator, CBaseEntity@ pCaller, USE_TYPE useType, float flValue = 0.0f)",
	    asFUNCTION( BaseEntity_Use<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "Calls the base class implementation of Blocked", name,
	                                     "void Blocked(CBaseEntity@ pOther)", asFUNCTION( BaseEntity_Blocked<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "Calls the base class implementation of UpdateOnRemove", name,
	                                     "void UpdateOnRemove()", asFUNCTION( BaseEntity_UpdateOnRemove<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "Calls the base class implementation of ObjectCaps", name,
	                                     "int ObjectCaps()", asFUNCTION( BaseEntity_ObjectCaps<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of TraceAttack", name,
	    "void TraceAttack(entvars_t@ pevAttacker, float flDamage, const Vector& in vecDir, TraceResult& in ptr, int bitsDamageType)",
	    asFUNCTION( BaseEntity_TraceAttack<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of TakeDamage", name,
	    "int TakeDamage(entvars_t@ pevInflictor, entvars_t@ pevAttacker, float flDamage, int bitsDamageType)",
	    asFUNCTION( BaseEntity_TakeDamage<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of TakeHealth", name,
	    "bool TakeHealth(float flHealth, int bitsDamageType, float health_cap = 0)",
	    asFUNCTION( BaseEntity_TakeHealth<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of TakeArmor", name,
	    "bool TakeArmor(float flArmor, int bitsDamageType, float armor_cap = 0)",
	    asFUNCTION( BaseEntity_TakeArmor<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "Calls the base class implementation of Killed", name,
	                                     "void Killed(entvars_t@ pevAttacker, int iGib)", asFUNCTION( BaseEntity_Killed<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "Calls the base class implementation of BloodColor", name,
	                                     "int BloodColor()", asFUNCTION( BaseEntity_BloodColor<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of TraceBleed", name,
	    "void TraceBleed(float flDamage, const Vector& in vecDir, TraceResult& in traceResult, int bitsDamageType)",
	    asFUNCTION( BaseEntity_TraceBleed<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of GetPointsForDamage", name,
	    "float GetPointsForDamage(float flDamage)", asFUNCTION( BaseEntity_GetPointsForDamage<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "Calls the base class implementation of IsRevivable", name,
	                                     "bool IsRevivable()", asFUNCTION( BaseEntity_IsRevivable<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "Calls the base class implementation of BeginRevive", name,
	                                     "void BeginRevive(float flTimeUntilRevive)", asFUNCTION( BaseEntity_BeginRevive<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "Calls the base class implementation of EndRevive", name,
	                                     "void EndRevive(float flTimeUntilRevive)", asFUNCTION( BaseEntity_EndRevive<T> ), asCALL_CDECL_OBJLAST );
}

template <class T>
CCustomKeyvalues *Entity_GetCustomKeyvalues( T *entity )
{
	return entity->m_pCustomKeyvalues;
}

template <class T>
void RegisterSCScriptBaseEntity( CASDocumentation *documentation, const char *name, bool registerUseTargets )
{
	RegisterASScriptEntityHeaderCasts<CBaseEntity, T>( documentation, "CBaseEntity", name );
	RegisterSCScriptEntityInternal( documentation, name );

	documentation->RegisterObjectMethod(
	    "Precaches the entity. Do not call directly.", name,
	    "void Precache()",
	    asMETHOD( T, Precache ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets this entity's objects caps. See the FCAP enum.", name,
	    "int ObjectCaps()",
	    asMETHOD( T, ObjectCaps ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets up the object collision box.", name,
	    "void SetObjectCollisionBox()",
	    asMETHOD( T, SetObjectCollisionBox ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets this entity's classification. Returns the given value if none is set. See the CLASSIFICATION enum.", name,
	    "int GetClassification(int cl)",
	    asMETHOD( T, GetClassification ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets this entity's classification override. Can override the entity's player ally setting.", name,
	    "void SetClassification(int cl)",
	    asMETHOD( T, SetClassification ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets this entity's classification override to the classification override set on the given entity. Can override "
	    "the entity's player ally setting.",
	    name,
	    "void SetClassificationFromEntity(CBaseEntity@ pEntity)",
	    asMETHOD( T, SetClassificationFromEntity ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Clears this entity's classification override.", name,
	    "void ClearClassification()",
	    asMETHOD( T, ClearClassification ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the relationship between this entity and the given entity. See the RELATIONSHIP enum.", name,
	    "int IRelationship(CBaseEntity@ other, bool ignoreNoTarget = false)",
	    asMETHOD( T, IRelationship ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the relationship between this entity and the given entity classification. See the RELATIONSHIP enum.", name,
	    "int IRelationshipByClass(CLASS iClass)",
	    asMETHOD( T, IRelationshipByClass ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets this entity's classification.", name,
	    "int Classify()",
	    asMETHOD( T, Classify ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Causes this entity to take damage. Returns 1 if it took damage, 0 otherwise.", name,
	    "int TakeDamage(entvars_t@ pevInflictor, entvars_t@ pevAttacker, float flDamage, int bitsDamageType)",
	    asMETHOD( T, TakeDamage ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Takes health from this entity. Note: flHealth is added to entity health. Use negative values to subtract.If "
	    "health_cap is non-zero, won't add more than health_cap. Returns true if it took damage, false otherwise.",
	    name,
	    "bool TakeHealth(float flHealth, int bitsDamageType, int health_cap = 0)",
	    asMETHOD( T, TakeHealth ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Takes armor from this entity. Note: flArmor is added to entity armor. Use negative values to subtract.If "
	    "armor_cap is non-zero, won't add more than armor_cap. Returns 1 if it took damage, 0 otherwise.",
	    name,
	    "bool TakeArmor(float flArmor, int bitsDamageType, int armor_cap = 0)",
	    asMETHOD( T, TakeArmor ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Tells this entity it's been killed. See the GIB enum for possible values.", name,
	    "void Killed(entvars_t@ pevAttacker, int iGibbed)",
	    asMETHOD( T, Killed ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns the blood color. See the BLOOD_COLOR enum.", name,
	    "int BloodColor()",
	    asMETHOD( T, BloodColor ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this entity is triggered by the given entity.", name,
	    "bool IsTriggered(CBaseEntity@ pActivator)",
	    asMETHOD( T, IsTriggered ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns this entity as its monster entity, if it is a monster.", name,
	    "CBaseMonster@ MyMonsterPointer()",
	    asMETHOD( T, MyMonsterPointer ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets this entity's custom keyvalues.", name,
	    "CustomKeyvalues@ GetCustomKeyvalues()",
	    asFUNCTION( Entity_GetCustomKeyvalues<T> ), asCALL_CDECL_OBJLAST );

	documentation->RegisterObjectMethod(
	    "Returns true if you should be blocked by the entity, false otherwise. Also deals blocking damage to the entity, "
	    "if applicable.",
	    name,
	    "bool BlockedByEntity(CBaseEntity@ pOther, float flDamage)",
	    asMETHOD( T, BlockedByEntity ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Think function. Delay-removes this entity.", name,
	    "void SUB_Remove()",
	    asMETHOD( T, SUB_Remove ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Think function. Explicitly does nothing.", name,
	    "void SUB_DoNothing()",
	    asMETHOD( T, SUB_DoNothing ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Think function. Starts fading this entity.", name,
	    "void SUB_StartFadeOut()",
	    asMETHOD( T, SUB_StartFadeOut ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Think function. Fades this entity.", name,
	    "void SUB_FadeOut()",
	    asMETHOD( T, SUB_FadeOut ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Think function. Calls use toggle on this entity.", name,
	    "void SUB_CallUseToggle()",
	    asMETHOD( T, SUB_CallUseToggle ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this entity should toggle.", name,
	    "bool ShouldToggle(USE_TYPE useType, const bool currentState)",
	    asMETHOD( T, ShouldToggle ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Fires bullets. Must call CMath::MakeAimVectors with an angle vector first.", name,
	    "void FireBullets(uint cShots, Vector vecSrc, Vector vecDirShooting, Vector vecSpread,float flDistance, Bullet "
	    "iBulletType, int iTracerFreq = 4, int iDamage = 0, entvars_t@ pevAttacker = null, FireBulletsDrawMode fDraw = "
	    "FBDM_DRAW)",
	    asMETHOD( T, FireBullets ), asCALL_THISCALL );

	if ( registerUseTargets )
	{
		documentation->RegisterObjectMethod(
		    "Calls use on targets.", name,
		    "void SUB_UseTargets(CBaseEntity@ pActivator, USE_TYPE useType, float flValue)",
		    asMETHOD( T, SUB_UseTargets ), asCALL_THISCALL );
	}

	documentation->RegisterObjectMethod(
	    "Returns whether this entity intersects with the given entity.", name,
	    "bool Intersects(CBaseEntity@ pOther)",
	    asMETHOD( T, Intersects ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Makes this entity dormant.", name,
	    "void MakeDormant()",
	    asMETHOD( T, MakeDormant ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this entity is dormant.", name,
	    "bool IsDormant()",
	    asMETHOD( T, IsDormant ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this entity is locked by its master.", name,
	    "bool IsLockedByMaster()",
	    asMETHOD( T, IsLockedByMaster ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns this entity's edict.", name,
	    "edict_t@ edict()",
	    asMETHOD( T, edict ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns this entity's index.", name,
	    "int entindex()",
	    asMETHOD( T, entindex ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets up the model.", name,
	    "bool SetupModel()",
	    asMETHOD( T, SetupModel ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the toggle state.\nSee the TOGGLE_STATE enum.", name,
	    "int GetToggleState()",
	    asMETHOD( T, GetToggleState ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Adds points to this entity.", name,
	    "void AddPoints(int score, const bool bAllowNegativeScore)",
	    asMETHOD( T, AddPoints ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Adds points to this entity's team.", name,
	    "void AddPointsToTeam(int score, const bool bAllowNegativeScore)",
	    asMETHOD( T, AddPointsToTeam ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Adds the given player item to this entity.", name,
	    "AddPlayerItemResult AddPlayerItem(CBasePlayerItem@ pItem)",
	    asMETHOD( T, AddPlayerItem ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Removes the given player item to this entity.", name,
	    "bool RemovePlayerItem(CBasePlayerItem@ pItem)",
	    asMETHOD( T, RemovePlayerItem ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets this entity's trigger delay.", name,
	    "float GetDelay()",
	    asMETHOD( T, GetDelay ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this entity is moving.", name,
	    "bool IsMoving()",
	    asMETHOD( T, IsMoving ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Resets this entity.", name,
	    "void OverrideReset()",
	    asMETHOD( T, OverrideReset ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns the damage decal\nSee decal_e.", name,
	    "int DamageDecal(int bitsDamageType)",
	    asMETHOD( T, DamageDecal ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets this entity's toggle state.", name,
	    "void SetToggleState(int state)",
	    asMETHOD( T, SetToggleState ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Starts sneaking.", name,
	    "void StartSneaking()",
	    asMETHOD( T, StartSneaking ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Stops sneaking.", name,
	    "void StopSneaking()",
	    asMETHOD( T, StopSneaking ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Reacts to controls set on the pev.", name,
	    "bool OnControls(entvars_t@ pev)",
	    asMETHOD( T, OnControls ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this entity is sneaking.", name,
	    "bool IsSneaking()",
	    asMETHOD( T, IsSneaking ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this entity is alive.", name,
	    "bool IsAlive()",
	    asMETHOD( T, IsAlive ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this entity is a BSP model.", name,
	    "bool IsBSPModel()",
	    asMETHOD( T, IsBSPModel ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this entity reflects gauss shots.", name,
	    "bool ReflectGauss()",
	    asMETHOD( T, ReflectGauss ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this entity is in the world.", name,
	    "bool IsInWorld()",
	    asMETHOD( T, IsInWorld ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this entity is a monster.", name,
	    "bool IsMonster()",
	    asMETHOD( T, IsMonster ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this entity is a player.", name,
	    "bool IsPlayer()",
	    asMETHOD( T, IsPlayer ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this is a net client.", name,
	    "bool IsNetClient()",
	    asMETHOD( T, IsNetClient ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this is a point entity.", name,
	    "bool IsPointEnt()",
	    asMETHOD( T, IsPointEnt ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this is a breakable brush entity.", name,
	    "bool IsBreakable()",
	    asMETHOD( T, IsBreakable ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this is a machine.", name,
	    "bool IsMachine()",
	    asMETHOD( T, IsMachine ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Removes this entity in critical situations.", name,
	    "bool CriticalRemove()",
	    asMETHOD( T, CriticalRemove ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the next target to trigger.", name,
	    "CBaseEntity@ GetNextTarget()",
	    asMETHOD( T, GetNextTarget ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Makes this entity think. Do not call directly.", name,
	    "void Think()",
	    asMETHOD( T, Think ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Triggers this entity's touch function.", name,
	    "void Touch(CBaseEntity@ pOther)",
	    asMETHOD( T, Touch ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Trigger's this entity's use function.", name,
	    "void Use(CBaseEntity@ pActivator, CBaseEntity@ pCaller, USE_TYPE useType, float flValue = 0.0f)",
	    asMETHOD( T, Use ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Triggers this entity's blocked function.", name,
	    "void Blocked(CBaseEntity@ pOther)",
	    asMETHOD( T, Blocked ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Updates this entity when removed.", name,
	    "void UpdateOnRemove()",
	    asMETHOD( T, UpdateOnRemove ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Respawns this entity.", name,
	    "CBaseEntity@ Respawn()",
	    asMETHOD( T, Respawn ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Makes this entity become prone.", name,
	    "bool FBecomeProne(CBaseEntity@ pOther)",
	    asMETHOD( T, FBecomeProne ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns this entity's center.", name,
	    "Vector Center()",
	    asMETHOD( T, Center ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns this entity's eye position.", name,
	    "Vector EyePosition()",
	    asMETHOD( T, EyePosition ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns this entity's ear position.", name,
	    "Vector EarPosition()",
	    asMETHOD( T, EarPosition ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns this entity's body target for shooting.", name,
	    "Vector BodyTarget(const Vector& in posSrc)",
	    asMETHOD( T, BodyTarget ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns this entity's illumination.", name,
	    "int Illumination()",
	    asMETHOD( T, Illumination ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this entity is visible to the given entity.", name,
	    "bool FVisible(CBaseEntity@ pEntity, bool fIgnoreGlass)",
	    asMETHODPR( T, FVisible, (CBaseEntity *, bool), bool ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this entity is visible from the given origin.", name,
	    "bool FVisible(const Vector& in vecOrigin)",
	    asMETHODPR( T, FVisible, (const Vector &), BOOL ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this entity is facing the given entity, within the given view field.", name,
	    "bool IsFacing(entvars_t@ pevTest, float flDotProduct = VIEW_FIELD_ULTRA_NARROW)",
	    asMETHOD( T, IsFacing ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns the points received for damage done.", name,
	    "float GetPointsForDamage(float flDamage)",
	    asMETHOD( T, GetPointsForDamage ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Adds damage points to the inflicting entity.", name,
	    "void GetDamagePoints(entvars_t@ pevAttacker, entvars_t@ pevInflictor, float flDamage)",
	    asMETHOD( T, GetDamagePoints ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Entity constructor.", name,
	    "void OnCreate()",
	    asMETHOD( T, OnCreate ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Entity destructor.", name,
	    "void OnDestroy()",
	    asMETHOD( T, OnDestroy ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Entity callback after trigger_setorigin has moved the entity.", name,
	    "void OnSetOriginByMap()",
	    asMETHOD( T, OnSetOriginByMap ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this entity is revivable at this time.", name,
	    "bool IsRevivable()",
	    asMETHOD( T, IsRevivable ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Prepares this entity for revival. flTimeUntilRevive is the time until the revive action completes.", name,
	    "void BeginRevive(float flTimeUntilRevive)",
	    asMETHOD( T, BeginRevive ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Called when this entity should be revived. flTimeUntilRevive is the time until the revival should start.", name,
	    "void EndRevive(float flTimeUntilRevive)",
	    asMETHOD( T, EndRevive ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Add a entity flag mask (EFLAG)", name,
	    "void AddEntityFlag(int FlagMask)",
	    asMETHOD( T, AddEntityFlag ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Remove a entity flag mask (EFLAG)", name,
	    "void RemoveEntityFlag(int FlagMask)",
	    asMETHOD( T, RemoveEntityFlag ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Check if it has such entity flag mask (EFLAG) set", name,
	    "bool HasEntityFlag(int FlagMask) const",
	    asMETHOD( T, HasEntityFlag ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Clear all entity flag masks (EFLAG)", name,
	    "void ClearEntityFlags()",
	    asMETHOD( T, ClearEntityFlags ), asCALL_THISCALL );

	asDWORD accessMask = documentation->SetDefaultAccessMask( AS_ACCESS_MAP );

	documentation->RegisterObjectMethod(
	    "Gets the user data on this entity. Do not keep references to this data, the object might be removed from this "
	    "entity.\nOnly map scripts can use this.",
	    name,
	    "dictionary@ GetUserData()",
	    asMETHODPR( T, GetUserData, (), CScriptDictionary * ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the user data stored under the specified key on this entity.\nDo not keep references to this data, the "
	    "object might be removed from this entity.\nOnly map scripts can use this.",
	    name,
	    "dictionaryValue& GetUserData(const string& in szKey)",
	    asMETHODPR( T, GetUserData, (const CString &), CScriptDictValue & ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Clears the user data on this entity. WARNING: clears all data.\nOnly map scripts can use this.", name,
	    "void ClearUserData()",
	    asMETHODPR( T, ClearUserData, (), void ), asCALL_THISCALL );

	documentation->SetDefaultAccessMask( accessMask );
}

template <class T>
void RegisterSCScriptBaseDelayEntity( CASDocumentation *documentation, const char *name )
{
	RegisterASScriptEntityHeaderCasts<CBaseDelay, T>( documentation, "CBaseDelay", name );
	RegisterSCScriptBaseEntity<T>( documentation, name, false );

	documentation->RegisterObjectMethod(
	    "Causes this entity to trigger its targets with an optional delay.", name,
	    "void SUB_UseTargets(CBaseEntity@ pActivator, USE_TYPE useType, float flValue)",
	    asMETHOD( T, SUB_UseTargets ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Think function. Used internally.", name, "void DelayThink()",
	    asMETHOD( T, DelayThink ), asCALL_THISCALL );

	documentation->RegisterObjectProperty( "Delay before fire.", name,
	                                       "float m_flDelay", asOFFSET( T, m_flDelay ) );
	documentation->RegisterObjectProperty( "The name of the kill target, if any.", name,
	                                       "string_t m_iszKillTarget", asOFFSET( T, m_iszKillTarget ) );
}

#endif
