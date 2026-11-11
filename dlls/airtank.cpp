/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "PickupUtils.h"

#define SF_AIRTANK_NO_DAMAGE		1
#define SF_AIRTANK_START_EMPTY	2

#define SF_ITEM_TOUCH_ONLY		128
#define SF_ITEM_USE_WITHOUT_LOS	512

#define AIRTANK_TOUCH_DELAY	3

class CAirtank : public CGrenade
{
	void Spawn( void );
	void Precache( void );
	int ObjectCaps( void );
	void EXPORT TankThink( void );
	void EXPORT TankTouch( CBaseEntity *pOther );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void GiveAir( CBaseEntity *pOther );
	int	 BloodColor( void ) { return DONT_BLEED; };
	void Killed( entvars_t *pevAttacker, int iGib );
	void KeyValue( KeyValueData *pkvd );

	virtual int		Save( CSave &save ); 
	virtual int		Restore( CRestore &restore );
	
	static	TYPEDESCRIPTION m_SaveData[];

	float		m_flLastTouch;
	int	 m_state;
	float		m_flAirTimeGiven;
	float		m_flRefillDelay;
	string_t	m_iszSoundOnConsume;
	string_t	m_iszSoundOnIsEmpty;
	string_t	m_iszSoundOnRefill;
	string_t	m_iszTargetOnConsume;
	string_t	m_iszTargetOnIsEmpty;
	string_t	m_iszTargetOnRefill;
	string_t	m_iszTargetOnDestroy;
};


LINK_ENTITY_TO_CLASS( item_airtank, CAirtank );
TYPEDESCRIPTION	CAirtank::m_SaveData[] = 
{
	DEFINE_FIELD( CAirtank, m_flLastTouch, FIELD_TIME ),
	DEFINE_FIELD( CAirtank, m_state, FIELD_INTEGER ),
	DEFINE_FIELD( CAirtank, m_flAirTimeGiven, FIELD_FLOAT ),
	DEFINE_FIELD( CAirtank, m_flRefillDelay, FIELD_FLOAT ),
	DEFINE_FIELD( CAirtank, m_iszSoundOnConsume, FIELD_STRING ),
	DEFINE_FIELD( CAirtank, m_iszSoundOnIsEmpty, FIELD_STRING ),
	DEFINE_FIELD( CAirtank, m_iszSoundOnRefill, FIELD_STRING ),
	DEFINE_FIELD( CAirtank, m_iszTargetOnConsume, FIELD_STRING ),
	DEFINE_FIELD( CAirtank, m_iszTargetOnIsEmpty, FIELD_STRING ),
	DEFINE_FIELD( CAirtank, m_iszTargetOnRefill, FIELD_STRING ),
	DEFINE_FIELD( CAirtank, m_iszTargetOnDestroy, FIELD_STRING ),
};

IMPLEMENT_SAVERESTORE( CAirtank, CGrenade );


int CAirtank::ObjectCaps( void )
{
	if ( pev->spawnflags & SF_ITEM_TOUCH_ONLY )
		return FCAP_ACROSS_TRANSITION;

	return FCAP_ACROSS_TRANSITION | FCAP_IMPULSE_USE;
}

void CAirtank::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "air_time_given" ) )
	{
		m_flAirTimeGiven = V_max( 0.0f, (float) atof( pkvd->szValue ) );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "refill_delay" ) )
	{
		m_flRefillDelay = V_max( -1.0f, (float) atof( pkvd->szValue ) );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "target_on_consume" ) )
	{
		if ( *pkvd->szValue )
			m_iszTargetOnConsume = ALLOC_STRING( pkvd->szValue );

		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "target_on_is_empty" ) )
	{
		if ( *pkvd->szValue )
			m_iszTargetOnIsEmpty = ALLOC_STRING( pkvd->szValue );

		pkvd->fHandled = TRUE;
	}
	else if ( !stricmp( pkvd->szKeyName, "target_on_refill" ) )
	{
		if ( *pkvd->szValue )
			m_iszTargetOnRefill = ALLOC_STRING( pkvd->szValue );

		pkvd->fHandled = TRUE;
	}
	else if ( !stricmp( pkvd->szKeyName, "target_on_destroy" ) )
	{
		if ( *pkvd->szValue )
			m_iszTargetOnDestroy = ALLOC_STRING( pkvd->szValue );

		pkvd->fHandled = TRUE;
	}
	else if ( !stricmp( pkvd->szKeyName, "sound_on_consume" ) )
	{
		if ( *pkvd->szValue )
			m_iszSoundOnConsume = ALLOC_STRING( pkvd->szValue );

		pkvd->fHandled = TRUE;
	}
	else if ( !stricmp( pkvd->szKeyName, "sound_on_is_empty" ) )
	{
		if ( *pkvd->szValue )
			m_iszSoundOnIsEmpty = ALLOC_STRING( pkvd->szValue );

		pkvd->fHandled = TRUE;
	}
	else if ( !stricmp( pkvd->szKeyName, "sound_on_refill" ) )
	{
		if ( *pkvd->szValue )
			m_iszSoundOnRefill = ALLOC_STRING( pkvd->szValue );

		pkvd->fHandled = TRUE;
	}
	else
	{
		CGrenade::KeyValue( pkvd );
	}
}


void CAirtank :: Spawn( void )
{
	Precache( );
	// motor
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;

	if ( SetupModel() )
		SET_MODEL( ENT(pev), STRING( pev->model ) );
	else
		SET_MODEL(ENT(pev), "models/w_oxygen.mdl");

	UTIL_SetSize(pev, Vector( -16, -16, 0), Vector(16, 16, 36));
	UTIL_SetOrigin( pev, pev->origin );

	SetTouch( &CAirtank::TankTouch );
	SetThink( &CAirtank::TankThink );
	SetUse( &CBaseEntity::Use );

	pev->flags |= FL_MONSTER;
	pev->takedamage		= ( pev->spawnflags & SF_AIRTANK_NO_DAMAGE ) ? DAMAGE_NO : DAMAGE_YES;

	m_flLastTouch = 0;

	if ( pev->spawnflags & SF_AIRTANK_START_EMPTY )
		m_state = -1;
	else
		m_state = 1;

	if ( pev->health <= 0 )
		pev->health = 20;

	if ( pev->dmg <= 0 )
		pev->dmg = 50;

	if ( m_flAirTimeGiven == 0 )
		m_flAirTimeGiven = 12;

	if ( m_flRefillDelay == 0 )
		m_flRefillDelay = 30;
}

void CAirtank::Precache( void )
{
	CBaseEntity::Precache();

	if ( !SetupModel() )
		PRECACHE_MODEL("models/w_oxygen.mdl");

	if ( m_iszSoundOnConsume )
		PRECACHE_SOUND( STRING( m_iszSoundOnConsume ) );
	else
		PRECACHE_SOUND("doors/aliendoor3.wav");

	if ( m_iszSoundOnIsEmpty )
		PRECACHE_SOUND( STRING( m_iszSoundOnIsEmpty ) );
	else
		PRECACHE_SOUND("player/pl_swim2.wav");

	if ( m_iszSoundOnRefill )
		PRECACHE_SOUND( STRING( m_iszSoundOnRefill ) );
	else
		PRECACHE_SOUND("items/airtank1.wav");
}


void CAirtank :: Killed( entvars_t *pevAttacker, int iGib )
{
	if ( pevAttacker )
		pev->owner = ENT( pevAttacker );
	else
		pev->owner = NULL;

	// UNDONE: this should make a big bubble cloud, not an explosion

	if ( !( pev->spawnflags & SF_AIRTANK_NO_DAMAGE ) )
		Explode( pev->origin, Vector( 0, 0, -1 ) );

	if ( m_iszTargetOnDestroy )
		FireTargets( STRING( m_iszTargetOnDestroy ), this, this, USE_TOGGLE, 0 );
}


void CAirtank::TankThink( void )
{
	if ( m_state == -1 || m_state == 1 )
		return;

	// Fire trigger
	m_state = 1;
	SUB_UseTargets( this, USE_TOGGLE, 0 );

	if ( m_iszTargetOnRefill )
		FireTargets( STRING( m_iszTargetOnRefill ), this, this, USE_TOGGLE, 0 );

	EMIT_SOUND( ENT(pev), CHAN_BODY, m_iszSoundOnRefill ? STRING( m_iszSoundOnRefill ) : "items/airtank1.wav", 1.0, ATTN_NORM );
}


void CAirtank::TankTouch( CBaseEntity *pOther )
{
	if ( m_flLastTouch >= 0 && gpGlobals->time - m_flLastTouch < AIRTANK_TOUCH_DELAY )
		return;

	if ( !pOther || !pOther->IsPlayer() )
		return;

	if ( pev->spawnflags & SF_ITEM_USE_ONLY )
		return;

	m_flLastTouch = gpGlobals->time;
	GiveAir( pOther );
}


void CAirtank::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	switch ( useType )
	{
	case USE_OFF:
		m_state = -1;
		pev->nextthink = gpGlobals->time;
		break;

	case USE_ON:
		m_state = 0;
		pev->nextthink = gpGlobals->time;
		break;

	case USE_SET:
		if ( !pActivator || !pActivator->IsPlayer() || value < 1 )
			return;

		if ( pev->spawnflags & SF_ITEM_TOUCH_ONLY )
			return;

		if ( !( pev->spawnflags & SF_ITEM_USE_WITHOUT_LOS ) && !UTIL_HasLineOfSight( pActivator, this, false, false ) )
			return;

		GiveAir( pActivator );
		break;

	case USE_TOGGLE:
		if ( pActivator && pActivator->IsPlayer() )
			GiveAir( pActivator );
		break;

	case USE_KILL:
		if ( value < 1 || pev->takedamage == DAMAGE_NO )
			UTIL_Remove( this );
		else if ( pActivator )
			Killed( pActivator->pev, GIB_NORMAL );
		else
			Killed( NULL, GIB_NORMAL );
		break;
	}
}


void CAirtank::GiveAir( CBaseEntity *pOther )
{
	if ( !pOther || !pOther->IsPlayer() )
		return;

	if ( m_state <= 0 )
	{
		if ( m_iszTargetOnIsEmpty )
			FireTargets( STRING( m_iszTargetOnIsEmpty ), pOther, this, USE_TOGGLE, 0 );

		// "no oxygen" sound
		EMIT_SOUND( ENT(pev), CHAN_BODY, m_iszSoundOnIsEmpty ? STRING( m_iszSoundOnIsEmpty ) : "player/pl_swim2.wav", 1.0, ATTN_NORM );
		return;
	}

	// give player more seconds of air
	pOther->pev->air_finished = V_max( 0.0f, gpGlobals->time + m_flAirTimeGiven );

	// suit recharge sound
	SUB_UseTargets( this, USE_TOGGLE, 1 );

	if ( m_iszTargetOnConsume )
		FireTargets( STRING( m_iszTargetOnConsume ), pOther, this, USE_TOGGLE, 0 );

	EMIT_SOUND( ENT(pev), CHAN_BODY, m_iszSoundOnConsume ? STRING( m_iszSoundOnConsume ) : "doors/aliendoor3.wav", 1.0, ATTN_NORM );

	// recharge airtank in the configured time, a negative delay never refills
	pev->nextthink = gpGlobals->time + m_flRefillDelay;

	if ( m_flRefillDelay >= 0 )
		m_state = 0;
	else
		m_state = -1;
}
