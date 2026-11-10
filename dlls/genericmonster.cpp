/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/
//=========================================================
// Generic Monster - purely for scripted sequence work.
//=========================================================
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"scriptevent.h"
#include	"schedule.h"
#include "soundent.h"

// For holograms, make them not solid so the player can walk through them
#define	SF_GENERICMONSTER_NOTSOLID					4
#define	SF_GENERICMONSTER_HEADCONTROLLER			8

//=========================================================
// Monster's Anim Events Go Here
//=========================================================

class CGenericMonster : public CBaseMonster
{
public:
	void Spawn( void );
	void Precache( void );
	void SetYawSpeed( void );
	int  Classify ( void );
	void HandleAnimEvent( MonsterEvent_t *pEvent );
	int ISoundMask ( void );
	void KeyValue( KeyValueData *data );

private:
	bool m_fDisableAI;
};
LINK_ENTITY_TO_CLASS( monster_generic, CGenericMonster );
LINK_ENTITY_TO_CLASS( monster_op4loader, CGenericMonster );

void CGenericMonster::KeyValue( KeyValueData *data )
{
	if ( FStrEq( data->szKeyName, "disableai" ) )
	{
		m_fDisableAI = atoi( data->szValue ) != 0;
		data->fHandled = TRUE;
	}
	else
		CBaseMonster::KeyValue( data );
}


//=========================================================
// Classify - indicates this monster's place in the 
// relationship table.
//=========================================================
int	CGenericMonster :: Classify ( void )
{
	if ( FClassnameIs( pev, "monster_op4loader" ) )
		return GetClassification( CLASS_NONE );

	return GetClassification( CLASS_PLAYER_ALLY );
}

//=========================================================
// SetYawSpeed - allows each sequence to have a different
// turn rate associated with it.
//=========================================================
void CGenericMonster :: SetYawSpeed ( void )
{
	int ys;

	switch ( m_Activity )
	{
	case ACT_IDLE:
	default:
		ys = 90;
	}

	pev->yaw_speed = ys;
}

//=========================================================
// HandleAnimEvent - catches the monster-specific messages
// that occur when tagged animation frames are played.
//=========================================================
void CGenericMonster :: HandleAnimEvent( MonsterEvent_t *pEvent )
{
	if ( pEvent->event == SCRIPT_EVENT_SOUND_VOICE && FClassnameIs( pev, "monster_op4loader" ) )
	{
		EMIT_SOUND( ENT(pev), CHAN_BODY, pEvent->options, 1, 1.25 );
		return;
	}

	CBaseMonster::HandleAnimEvent( pEvent );
}

//=========================================================
// ISoundMask - generic monster can't hear.
//=========================================================
int CGenericMonster :: ISoundMask ( void )
{
	return	bits_SOUND_NONE;
}

//=========================================================
// Spawn
//=========================================================
void CGenericMonster :: Spawn()
{
	Precache();

	pev->solid			= SOLID_SLIDEBOX;
	pev->movetype		= MOVETYPE_STEP;
	m_bloodColor		= BLOOD_COLOR_RED;

	if ( !pev->health )
		pev->health			= 8;

	m_flFieldOfView		= 0.5;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState		= MONSTERSTATE_NONE;

	if ( FClassnameIs( pev, "monster_op4loader" ) && !SetupModel() )
	{
		if ( !pev->health )
			pev->health = 50000;

		pev->takedamage = DAMAGE_NO;
		m_bloodColor = DONT_BLEED;
		SET_MODEL( ENT(pev), "models/loader.mdl" );
	}
	else
	{
		SET_MODEL( ENT(pev), STRING(pev->model) );
	}

/*
	if ( FStrEq( STRING(pev->model), "models/player.mdl" ) )
		UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);
	else
		UTIL_SetSize(pev, VEC_HULL_MIN, VEC_HULL_MAX);
*/

	if ( FStrEq( STRING(pev->model), "models/player.mdl" ) || FStrEq( STRING(pev->model), "models/holo.mdl" ) )
		UTIL_SetSize(pev, VEC_HULL_MIN, VEC_HULL_MAX);
	else
		UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);

	if ( !m_FormattedName )
		m_FormattedName = MAKE_STRING( "Monster" );

	if ( !m_fDisableAI )
		MonsterInit();

	if ( pev->spawnflags & SF_GENERICMONSTER_NOTSOLID )
	{
		pev->solid = SOLID_NOT;
		pev->takedamage = DAMAGE_NO;
	}

	if ( pev->spawnflags & SF_GENERICMONSTER_HEADCONTROLLER )
		m_afCapability |= bits_CAP_TURN_HEAD;

	if ( m_fPlayerAlly && !( pev->spawnflags & SF_MONSTER_PREDISASTER ) )
	{
		SetUse( &CBaseMonster::CallFollowerPlayerUse );
		m_fCanPlayerFollow = true;
	}
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CGenericMonster :: Precache()
{
	if ( FClassnameIs( pev, "monster_op4loader" ) && !SetupModel() )
	{
		PRECACHE_MODEL( "models/loader.mdl" );
		return;
	}

	PRECACHE_MODEL( (char *)STRING(pev->model) );
}	

//=========================================================
// AI Schedules Specific to this monster
//=========================================================
