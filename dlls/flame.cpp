#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "flame.h"

void CFlame::Precache( void )
{
	CBaseEntity::Precache();
	PRECACHE_MODEL( "sprites/flamethrower/fthrow.spr" );
}

void CFlame::Spawn( void )
{
	pev->classname = MAKE_STRING( "flame" );
	Precache();

	SetTouch( &CFlame::FlameTouch );
	pev->solid = SOLID_TRIGGER;
	pev->movetype = MOVETYPE_FLY;
	SetThink( &CFlame::FlameThink );
	pev->nextthink = gpGlobals->time;
	pev->dmgtime = gpGlobals->time + 0.75;
}

void CFlame::FlameThink( void )
{
	if ( gpGlobals->time > pev->dmgtime )
	{
		UTIL_Remove( this );
		return;
	}

	CBaseEntity *entity = NULL;

	while ( ( entity = UTIL_FindEntityInSphere( entity, pev->origin, 8 ) ) != NULL )
	{
		if ( m_flLastDamageTime + 0.1 > gpGlobals->time )
			break;

		DoDamage( entity );
	}

	pev->nextthink = gpGlobals->time;
}

void CFlame::FlameTouch( CBaseEntity *other )
{
	DoDamage( other );

	if ( other->IsBSPModel() )
		pev->velocity = g_vecZero;
}

bool CFlame::DoDamage( CBaseEntity *other )
{
	if ( m_flLastDamageTime + 0.1 > gpGlobals->time )
		return true;

	if ( FClassnameIs( other->pev, STRING( pev->classname ) ) || other->edict() == pev->owner )
		return false;

	TraceResult trace;

	UTIL_TraceLine( pev->origin, other->pev->origin, dont_ignore_monsters, ignore_glass, edict(), &trace );

	entvars_t *attacker = pev->owner ? VARS( pev->owner ) : pev;

	ClearMultiDamage();
	other->TraceAttack( attacker, pev->dmg, pev->velocity.Normalize(), &trace, DMG_BURN | DMG_SLOWBURN );
	ApplyMultiDamage( pev, attacker );

	if ( gMultiDamage.amount > 0 )
		m_flLastDamageTime = gpGlobals->time;

	return true;
}

CFlame *CFlame::Create( const Vector &origin, const Vector &velocity, float damage,
                        CBaseEntity *owner, float frameRate )
{
	CFlame *flame = GetClassPtr( (CFlame *) NULL );

	flame->pev->origin = origin;
	flame->pev->velocity = velocity;
	flame->pev->dmg = damage;
	flame->pev->framerate = frameRate;

	if ( owner )
		flame->pev->owner = owner->edict();

	flame->Spawn();
	return flame;
}
