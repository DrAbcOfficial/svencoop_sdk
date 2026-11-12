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
// rat - environmental monster
//=========================================================

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters/monsters.h"
#include	"ai/schedule.h"
#include "ai/animation.h"

//=========================================================
// Monster's Anim Events Go Here
//=========================================================

class CRat : public CBaseMonster
{
public:
	int ObjectCaps( void );

	void Spawn( void );
	void Precache( void );
	void SetYawSpeed( void );
	void SetActivity( Activity activity );
	int  Classify ( void );
};
LINK_ENTITY_TO_CLASS( monster_rat, CRat );

//=========================================================
// Classify - indicates this monster's place in the 
// relationship table.
//=========================================================
int	CRat :: Classify ( void )
{
	return GetClassification( CLASS_INSECT );
}

//=========================================================
// SetYawSpeed - allows each sequence to have a different
// turn rate associated with it.
//=========================================================
void CRat :: SetYawSpeed ( void )
{
	pev->yaw_speed = 270;
}

//=========================================================
// Spawn
//=========================================================
void CRat :: Spawn()
{
	Precache( );

	if ( !SetupModel() )
		SET_MODEL(ENT(pev), "models/bigrat.mdl");
	UTIL_SetSize( pev, Vector( 0, 0, 0 ), Vector( 0, 0, 0 ) );

	pev->solid			= SOLID_SLIDEBOX;
	pev->movetype		= MOVETYPE_STEP;
	m_bloodColor		= BLOOD_COLOR_RED;
	if ( !pev->health )
		pev->health			= 8;
	pev->view_ofs		= Vector ( 0, 0, 6 );// position of the eyes relative to monster's origin.
	m_flFieldOfView		= 0.5;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState		= MONSTERSTATE_NONE;

	if ( !m_FormattedName )
		m_FormattedName = MAKE_STRING( "Rat" );

	MonsterInit();

	if ( m_fPlayerAlly && !( pev->spawnflags & SF_MONSTER_PREDISASTER ) )
	{
		SetUse( &CBaseMonster::CallFollowerPlayerUse );
		m_fCanPlayerFollow = true;
	}
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CRat :: Precache()
{
	PRECACHE_MODEL("models/bigrat.mdl");
}	

//=========================================================
// AI Schedules Specific to this monster
//=========================================================

void CRat::SetActivity( Activity activity )
{
	int sequence = LookupActivity( activity == ACT_RUN ? ACT_WALK : activity );

	m_Activity = activity;
	m_IdealActivity = activity;

	if ( sequence < 0 )
	{
		ALERT( at_console, "%s has no sequence for act:%d\n", STRING( pev->classname ), activity );
		pev->sequence = 0;
		return;
	}

	pev->sequence = sequence;

	void *model = GET_MODEL_PTR( edict() );

	GetSequenceInfo( model, pev, &m_flFrameRate, &m_flGroundSpeed );
	m_fSequenceLoops = ( GetSequenceFlags() & STUDIO_LOOPING ) != 0;

	if ( pev->sequence != sequence || !m_fSequenceLoops )
		pev->frame = 0;

	pev->animtime = gpGlobals->time;
	m_fSequenceFinished = FALSE;
	m_flLastEventCheck = gpGlobals->time;
	pev->framerate = m_Activity == ACT_WALK || m_Activity == ACT_RUN ? 10.0f : 1.0f;
	SetYawSpeed();
}

int CRat::ObjectCaps( void )
{
	return IsPlayerAlly() ? FCAP_IMPULSE_USE : FCAP_ACROSS_TRANSITION;
}
