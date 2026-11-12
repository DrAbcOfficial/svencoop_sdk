#ifndef CASENTITYFUNCS_H
#define CASENTITYFUNCS_H

#include "scripting/core/CASServerScriptGlobal.h"
#include "CString.h"
#include "weapons/weapons.h"
#include "world/func_break.h"

class CBaseEntity;
class CGib;
class CScriptArray;
class CScriptDictionary;

class CASEntityFuncs : public CASServerScriptGlobal<CASEntityFuncs>
{
public:
	virtual ~CASEntityFuncs();

	CBaseEntity *CreateEntity( const CString &className, CScriptDictionary *dictionary, bool spawn );
	CBaseEntity *CreateEntity( const CString &className, CScriptDictionary *dictionary, bool spawn,
	                           edict_t *( *factory )( int ) );
	bool InitializeEntity( const CString &className, CBaseEntity *entity, CScriptDictionary *dictionary );
	CBaseEntity *Create( const CString &className, const Vector &origin, const Vector &angles,
	                     bool createAndDontSpawn, edict_t *owner );
	void CreateDecal( const CString &decalName, const Vector &origin, const CString &targetName );
	CSprite *CreateSprite( const CString &spriteName, const Vector &origin, bool animated, float frameRate );
	CBeam *CreateBeam( const CString &spriteName, int width );
	void CreateExplosion( const Vector &center, const Vector &angles, edict_t *owner, int magnitude, bool doDamage );
	CBaseEntity *CreateDisplacerPortal( const Vector &origin, const Vector &velocity, edict_t *owner,
	                                    float damage, float radius );
	CBaseEntity *CreateRPGRocket( const Vector &origin, const Vector &angles, edict_t *owner );
	CBaseEntity *CreateRPGRocket( const Vector &origin, const Vector &angles, const Vector &velocity, edict_t *owner );
	CGib *CreateGib( const Vector &origin, const Vector &angles );
	CGrenade *ShootTimed( entvars_t *owner, const Vector &start, const Vector &velocity, float time );
	CGrenade *ShootContact( entvars_t *owner, const Vector &start, const Vector &velocity );
	CGrenade *ShootMortar( entvars_t *owner, const Vector &start, const Vector &velocity );
	CGrenade *ShootBananaCluster( entvars_t *owner, const Vector &start, const Vector &velocity );
	void UseSatchelCharges( entvars_t *owner, CGrenade::SATCHELCODE code );
	void SpawnHeadGib( entvars_t *victim );
	void SpawnRandomGibs( entvars_t *victim, int count, int human );
	void SpawnStickyGibs( entvars_t *victim, const Vector &origin, int count );
	void SetOrigin( CBaseEntity *entity, const Vector &origin );
	void SetModel( CBaseEntity *entity, const CString &model );
	void SetSize( entvars_t *vars, const Vector &mins, const Vector &maxs );
	CBaseEntity *FindEntityInSphere( CBaseEntity *start, const Vector &center, float radius,
	                                 const CString &value, const CString &key );
	CBaseEntity *FindEntityByString( CBaseEntity *start, const CString &key, const CString &value );
	CBaseEntity *FindEntityByClassname( CBaseEntity *start, const CString &name );
	CBaseEntity *FindEntityByTargetname( CBaseEntity *start, const CString &name );
	CBaseEntity *FindEntityGeneric( const CString &name, const Vector &source, float radius );
	int MonstersInSphere( CScriptArray *array, const Vector &center, float radius );
	int EntitiesInBox( CScriptArray *array, const Vector &mins, const Vector &maxs, int flagMask );
	int TargetsInBox( CScriptArray *array, const Vector &mins, const Vector &maxs );
	int BrushEntsInBox( CScriptArray *array, const Vector &mins, const Vector &maxs );
	int DispatchSpawn( edict_t *entity );
	bool DispatchKeyValue( edict_t *entity, const CString &key, const CString &value );
	bool EntvarsKeyvalue( edict_t *entity, const CString &key, const CString &value );
	void DispatchObjectCollisionBox( edict_t *entity );
	CBaseEntity *Instance( edict_t *entity );
	CBaseEntity *Instance( entvars_t *vars );
	CBaseEntity *Instance( int index );
	void FireTargets( const CString &targetName, CBaseEntity *activator, CBaseEntity *caller,
	                  USE_TYPE useType, float value, float delay );
	bool IsMasterTriggered( const CString &master, CBaseEntity *activator );
	void Remove( CBaseEntity *entity );
	bool IsValidEntity( edict_t *entity );
	int EntIndex( edict_t *entity );
	edict_t *IndexEnt( int index );
	void EjectBrass( const Vector &origin, const Vector &velocity, float rotation, int model, int soundType );
	void *CastToScriptClass( CBaseEntity *entity );
	CBaseEntity *RandomTargetname( const CString &targetName );
	void PrecacheMaterialSounds( Materials material );

private:
	friend class CASBaseScriptGlobal<IASServerScriptGlobal, CASEntityFuncs>;

	CASEntityFuncs();
};

#endif
