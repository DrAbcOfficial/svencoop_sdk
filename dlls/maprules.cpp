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

//	-------------------------------------------
//
//	maprules.cpp
//
//	This module contains entities for implementing/changing game
//	rules dynamically within each map (.BSP)
//
//	-------------------------------------------

#include "extdll.h"
#include "eiface.h"
#include "util.h"
#include "gamerules.h"
#include "maprules.h"
#include "cbase.h"
#include "player.h"
#include "CStartInventory.h"

class CRuleEntity : public CBaseDelay
{
public:
	void	Spawn( void );
	void	KeyValue( KeyValueData *pkvd );
	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	void	SetMaster( int iszMaster ) { m_iszMaster = iszMaster; }

protected:
	BOOL	CanFireForActivator( CBaseEntity *pActivator );

private:
	string_t	m_iszMaster;
};

TYPEDESCRIPTION	CRuleEntity::m_SaveData[] = 
{
	DEFINE_FIELD( CRuleEntity, m_iszMaster, FIELD_STRING),
};

IMPLEMENT_SAVERESTORE( CRuleEntity, CBaseEntity );


void CRuleEntity::Spawn( void )
{
	pev->solid			= SOLID_NOT;
	pev->movetype		= MOVETYPE_NONE;
	pev->effects		= EF_NODRAW;
}


void CRuleEntity::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "master"))
	{
		SetMaster( ALLOC_STRING(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseDelay::KeyValue( pkvd );
}

BOOL CRuleEntity::CanFireForActivator( CBaseEntity *pActivator )
{
	if ( m_iszMaster )
	{
		if ( UTIL_IsMasterTriggered( m_iszMaster, pActivator ) )
			return TRUE;
		else
			return FALSE;
	}
	
	return TRUE;
}

// 
// CRulePointEntity -- base class for all rule "point" entities (not brushes)
//
class CRulePointEntity : public CRuleEntity
{
public:
	void		Spawn( void );
};

void CRulePointEntity::Spawn( void )
{
	CRuleEntity::Spawn();
	pev->frame			= 0;
	pev->model			= 0;
}

class CGameSlotCounter : public CRulePointEntity
{
public:
	void Spawn( void ) override;
	void Think( void ) override;

private:
	void HitValue( void );
};

class CGamePlayerCounter : public CRulePointEntity
{
public:
	void Spawn( void ) override;
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value ) override;
};

LINK_ENTITY_TO_CLASS( game_slot_counter, CGameSlotCounter );
LINK_ENTITY_TO_CLASS( game_player_counter, CGamePlayerCounter );

void CGameSlotCounter::Spawn( void )
{
	CRulePointEntity::Spawn();
	SetThink( NULL );
	pev->nextthink = gpGlobals->time;
	pev->targetname = 0;
}

void CGameSlotCounter::HitValue( void )
{
	CBaseEntity *entity = NULL;

	while ( ( entity = UTIL_FindEntityByTargetname( entity, STRING( pev->target ) ) ) != NULL )
		entity->Use( NULL, NULL, USE_ON, 0 );
}

void CGameSlotCounter::Think( void )
{
	if ( (int) pev->health == gpGlobals->maxClients )
		HitValue();

	SetThink( NULL );
	UTIL_Remove( this );
}

void CGamePlayerCounter::Spawn( void )
{
	CRulePointEntity::Spawn();
}

int CountConnectedPlayers( void )
{
	int count = 0;

	for ( int index = 1; index <= gpGlobals->maxClients; ++index )
	{
		CBaseEntity *entity = UTIL_PlayerByIndex( index );

		if ( !entity || !entity->IsPlayer() )
			continue;

		CBasePlayer *player = dynamic_cast<CBasePlayer *>( entity );

		if ( player && player->IsConnected() )
			++count;
	}

	return count;
}

void CGamePlayerCounter::Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value )
{
	pev->dmg = CountConnectedPlayers();

	if ( pev->dmg == pev->frags )
	{
		CBaseEntity *entity = NULL;

		while ( ( entity = UTIL_FindEntityByTargetname( entity, STRING( pev->target ) ) ) != NULL )
			entity->Use( NULL, NULL, USE_ON, 0 );
	}

	if ( pev->dmg == pev->health )
	{
		CBaseEntity *entity = NULL;

		while ( ( entity = UTIL_FindEntityByTargetname( entity, STRING( pev->netname ) ) ) != NULL )
			entity->Use( NULL, NULL, USE_ON, 0 );
	}
}

// 
// CRuleBrushEntity -- base class for all rule "brush" entities (not brushes)
// Default behavior is to set up like a trigger, invisible, but keep the model for volume testing
//
class CRuleBrushEntity : public CRuleEntity
{
public:
	void		Spawn( void );

private:
};

void CRuleBrushEntity::Spawn( void )
{
	SET_MODEL( edict(), STRING(pev->model) );
	CRuleEntity::Spawn();
}


// CGameScore / game_score	-- award points to player / team 
//	Points +/- total
//	Flag: Allow negative scores					SF_SCORE_NEGATIVE
//	Flag: Award points to team in teamplay		SF_SCORE_TEAM

#define SF_SCORE_NEGATIVE			0x0001
#define SF_SCORE_TEAM				0x0002

class CGameScore : public CRulePointEntity
{
public:
	void	Spawn( void );
	void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void	KeyValue( KeyValueData *pkvd );

	inline	int		Points( void ) { return pev->frags; }
	inline	BOOL	AllowNegativeScore( void ) { return pev->spawnflags & SF_SCORE_NEGATIVE; }
	inline	BOOL	AwardToTeam( void ) { return pev->spawnflags & SF_SCORE_TEAM; }

	inline	void	SetPoints( int points ) { pev->frags = points; }

private:
};

LINK_ENTITY_TO_CLASS( game_score, CGameScore );


void CGameScore::Spawn( void )
{
	CRulePointEntity::Spawn();
}


void CGameScore::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "points"))
	{
		SetPoints( atoi(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else
		CRulePointEntity::KeyValue( pkvd );
}



void CGameScore::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !CanFireForActivator( pActivator ) )
		return;

	// Only players can use this
	if ( pActivator->IsPlayer() )
	{
		if ( AwardToTeam() )
		{
			pActivator->AddPointsToTeam( Points(), AllowNegativeScore() );
		}
		else
		{
			pActivator->AddPoints( Points(), AllowNegativeScore() );
		}
	}
}


// CGameEnd / game_end	-- Ends the game in MP

class CGameEnd : public CRulePointEntity
{
public:
	void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
private:
};

LINK_ENTITY_TO_CLASS( game_end, CGameEnd );


void CGameEnd::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !CanFireForActivator( pActivator ) )
		return;

	g_pGameRules->EndMultiplayerGame();
}


//
// CGameText / game_text	-- NON-Localized HUD Message (use env_message to display a titles.txt message)
//	Flag: All players					SF_ENVTEXT_ALLPLAYERS
//


#define SF_ENVTEXT_ALLPLAYERS			0x0001
#define SF_ENVTEXT_NO_CONSOLE_ECHO		0x0002


class CGameText : public CRulePointEntity
{
public:
	void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void	KeyValue( KeyValueData *pkvd );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	inline	BOOL	MessageToAll( void ) { return (pev->spawnflags & SF_ENVTEXT_ALLPLAYERS); }
	inline	void	MessageSet( const char *pMessage ) { pev->message = ALLOC_STRING(pMessage); }
	inline	const char *MessageGet( void )	{ return STRING(pev->message); }

private:

	hudtextparms_t	m_textParms;
};

LINK_ENTITY_TO_CLASS( game_text, CGameText );

// Save parms as a block.  Will break save/restore if the structure changes, but this entity didn't ship with Half-Life, so
// it can't impact saved Half-Life games.
TYPEDESCRIPTION	CGameText::m_SaveData[] = 
{
	DEFINE_ARRAY( CGameText, m_textParms, FIELD_CHARACTER, sizeof(hudtextparms_t) ),
};

IMPLEMENT_SAVERESTORE( CGameText, CRulePointEntity );


void CGameText::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "channel"))
	{
		m_textParms.channel = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "x"))
	{
		m_textParms.x = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "y"))
	{
		m_textParms.y = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "effect"))
	{
		m_textParms.effect = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "color"))
	{
		int color[4];
		UTIL_StringToIntArray( color, 4, pkvd->szValue );
		m_textParms.r1 = color[0];
		m_textParms.g1 = color[1];
		m_textParms.b1 = color[2];
		m_textParms.a1 = color[3];
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "color2"))
	{
		int color[4];
		UTIL_StringToIntArray( color, 4, pkvd->szValue );
		m_textParms.r2 = color[0];
		m_textParms.g2 = color[1];
		m_textParms.b2 = color[2];
		m_textParms.a2 = color[3];
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "fadein"))
	{
		m_textParms.fadeinTime = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "fadeout"))
	{
		m_textParms.fadeoutTime = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "holdtime"))
	{
		m_textParms.holdTime = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "fxtime"))
	{
		m_textParms.fxTime = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CRulePointEntity::KeyValue( pkvd );
}


void CGameText::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !CanFireForActivator( pActivator ) )
	{
		SUB_UseTargets( pActivator, USE_TOGGLE, 0 );
		return;
	}

	if ( MessageToAll() )
	{
		UTIL_HudMessageAll( m_textParms, MessageGet() );

		if ( !( pev->spawnflags & SF_ENVTEXT_NO_CONSOLE_ECHO ) )
			UTIL_ClientPrintAll( HUD_PRINTCONSOLE, "HUD-MSG: \"%s\"\n", MessageGet() );

		ALERT( at_aiconsole, "HUD-MSG to all: \"%s\"\n", MessageGet() );
	}
	else if ( !pActivator || FNullEnt( pActivator->edict() ) )
	{
		ALERT( at_console, "Game_text \"%s\" got no activator for activator-only message.\n", STRING( pev->targetname ) );
	}
	else if ( pActivator->IsNetClient() )
	{
		UTIL_HudMessage( pActivator, m_textParms, MessageGet() );

		if ( !( pev->spawnflags & SF_ENVTEXT_NO_CONSOLE_ECHO ) )
			ClientPrint( pActivator->pev, HUD_PRINTCONSOLE, "HUD-MSG: \"%s\"\n", MessageGet() );

		ALERT( at_aiconsole, "HUD-MSG to \"%s\": \"%s\"\n", STRING( pActivator->pev->netname ), MessageGet() );
	}

	SUB_UseTargets( pActivator, USE_TOGGLE, 0 );
}


//
// CGameTeamMaster / game_team_master	-- "Masters" like multisource, but based on the team of the activator
// Only allows mastered entity to fire if the team matches my team
//
// team index (pulled from server team list "mp_teamlist"
// Flag: Remove on Fire
// Flag: Any team until set?		-- Any team can use this until the team is set (otherwise no teams can use it)
//

#define SF_TEAMMASTER_FIREONCE			0x0001
#define SF_TEAMMASTER_ANYTEAM			0x0002

class CGameTeamMaster : public CRulePointEntity
{
public:
	void		KeyValue( KeyValueData *pkvd );
	void		Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	int			ObjectCaps( void ) { return CRulePointEntity:: ObjectCaps() | FCAP_MASTER; }

	BOOL		IsTriggered( CBaseEntity *pActivator );
	const char	*TeamID( void );
	inline BOOL RemoveOnFire( void ) { return (pev->spawnflags & SF_TEAMMASTER_FIREONCE) ? TRUE : FALSE; }
	inline BOOL AnyTeam( void ) { return (pev->spawnflags & SF_TEAMMASTER_ANYTEAM) ? TRUE : FALSE; }

private:
	BOOL		TeamMatch( CBaseEntity *pActivator );

	int			m_teamIndex;
	USE_TYPE	triggerType;
};

LINK_ENTITY_TO_CLASS( game_team_master, CGameTeamMaster );

void CGameTeamMaster::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "teamindex"))
	{
		m_teamIndex = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "triggerstate"))
	{
		int type = atoi( pkvd->szValue );
		switch( type )
		{
		case 0:
			triggerType = USE_OFF;
			break;
		case 2:
			triggerType = USE_TOGGLE;
			break;
		default:
			triggerType = USE_ON;
			break;
		}
		pkvd->fHandled = TRUE;
	}
	else
		CRulePointEntity::KeyValue( pkvd );
}


void CGameTeamMaster::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !CanFireForActivator( pActivator ) )
		return;

	if ( useType == USE_SET )
	{
		if ( value < 0 )
		{
			m_teamIndex = -1;
		}
		else
		{
			m_teamIndex = g_pGameRules->GetTeamIndex( pActivator->TeamID() );
		}
		return;
	}

	if ( TeamMatch( pActivator ) )
	{
		SUB_UseTargets( pActivator, triggerType, value );
		if ( RemoveOnFire() )
			UTIL_Remove( this );
	}
}


BOOL CGameTeamMaster::IsTriggered( CBaseEntity *pActivator )
{
	return TeamMatch( pActivator );
}


const char *CGameTeamMaster::TeamID( void )
{
	if ( m_teamIndex < 0 )		// Currently set to "no team"
		return "";

	return g_pGameRules->GetIndexedTeamName( m_teamIndex );		// UNDONE: Fill this in with the team from the "teamlist"
}


BOOL CGameTeamMaster::TeamMatch( CBaseEntity *pActivator )
{
	if ( m_teamIndex < 0 && AnyTeam() )
		return TRUE;

	if ( !pActivator )
		return FALSE;

	return UTIL_TeamsMatch( pActivator->TeamID(), TeamID() );
}


//
// CGameTeamSet / game_team_set	-- Changes the team of the entity it targets to the activator's team
// Flag: Fire once
// Flag: Clear team				-- Sets the team to "NONE" instead of activator

#define SF_TEAMSET_FIREONCE			0x0001
#define SF_TEAMSET_CLEARTEAM		0x0002

class CGameTeamSet : public CRulePointEntity
{
public:
	void		Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	inline BOOL RemoveOnFire( void ) { return (pev->spawnflags & SF_TEAMSET_FIREONCE) ? TRUE : FALSE; }
	inline BOOL ShouldClearTeam( void ) { return (pev->spawnflags & SF_TEAMSET_CLEARTEAM) ? TRUE : FALSE; }

private:
};

LINK_ENTITY_TO_CLASS( game_team_set, CGameTeamSet );


void CGameTeamSet::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !CanFireForActivator( pActivator ) )
		return;

	if ( ShouldClearTeam() )
	{
		SUB_UseTargets( pActivator, USE_SET, -1 );
	}
	else
	{
		SUB_UseTargets( pActivator, USE_SET, 0 );
	}

	if ( RemoveOnFire() )
	{
		UTIL_Remove( this );
	}
}


//
// CGamePlayerZone / game_player_zone -- players in the zone fire my target when I'm fired
//
// Needs master?
class CGamePlayerZone : public CRuleBrushEntity
{
public:
	void		KeyValue( KeyValueData *pkvd );
	void		Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

private:
	string_t	m_iszInTarget;
	string_t	m_iszOutTarget;
	string_t	m_iszInCount;
	string_t	m_iszOutCount;
};

LINK_ENTITY_TO_CLASS( game_zone_player, CGamePlayerZone );
TYPEDESCRIPTION	CGamePlayerZone::m_SaveData[] = 
{
	DEFINE_FIELD( CGamePlayerZone, m_iszInTarget, FIELD_STRING ),
	DEFINE_FIELD( CGamePlayerZone, m_iszOutTarget, FIELD_STRING ),
	DEFINE_FIELD( CGamePlayerZone, m_iszInCount, FIELD_STRING ),
	DEFINE_FIELD( CGamePlayerZone, m_iszOutCount, FIELD_STRING ),
};

IMPLEMENT_SAVERESTORE( CGamePlayerZone, CRuleBrushEntity );

void CGamePlayerZone::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "intarget"))
	{
		m_iszInTarget = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "outtarget"))
	{
		m_iszOutTarget = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "incount"))
	{
		m_iszInCount = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "outcount"))
	{
		m_iszOutCount = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CRuleBrushEntity::KeyValue( pkvd );
}

class CGameZonePlayerVolumeListener : public IPlayerInVolumeListener
{
public:
	CGameZonePlayerVolumeListener( string_t inTarget, string_t outTarget, CBaseEntity *activator, USE_TYPE useType, float value )
		: m_iszInTarget( inTarget ), m_iszOutTarget( outTarget ), m_pActivator( activator ), m_UseType( useType ), m_flValue( value )
	{
	}

	virtual ~CGameZonePlayerVolumeListener() {}

	virtual void PlayerDetected( bool inVolume, CBasePlayer *player )
	{
		string_t target = inVolume ? m_iszInTarget : m_iszOutTarget;

		if ( target )
			FireTargets( STRING( target ), player, m_pActivator, m_UseType, m_flValue );
	}

private:
	string_t     m_iszInTarget;
	string_t     m_iszOutTarget;
	CBaseEntity *m_pActivator;
	USE_TYPE     m_UseType;
	float        m_flValue;
};

void CGamePlayerZone::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	int playersInCount = 0;
	int playersOutCount = 0;

	if ( !CanFireForActivator( pActivator ) )
		return;

	CGameZonePlayerVolumeListener listener( m_iszInTarget, m_iszOutTarget, pActivator, useType, value );

	UTIL_CountPlayersInBrushVolume( false, this, playersInCount, playersOutCount, &listener );

	if ( m_iszInCount )
	{
		FireTargets( STRING(m_iszInCount), pActivator, this, USE_SET, playersInCount );
	}

	if ( m_iszOutCount )
	{
		FireTargets( STRING(m_iszOutCount), pActivator, this, USE_SET, playersOutCount );
	}
}



//
// CGamePlayerHurt / game_player_hurt	-- Damages the player who fires it
// Flag: Fire once

#define SF_PKILL_FIREONCE			0x0001
class CGamePlayerHurt : public CRulePointEntity
{
public:
	void		Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	inline BOOL RemoveOnFire( void ) { return (pev->spawnflags & SF_PKILL_FIREONCE) ? TRUE : FALSE; }

private:
};

LINK_ENTITY_TO_CLASS( game_player_hurt, CGamePlayerHurt );


void CGamePlayerHurt::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !CanFireForActivator( pActivator ) )
		return;

	if ( pActivator->IsPlayer() )
	{
		if ( pev->dmg < 0 )
			pActivator->TakeHealth( -pev->dmg, DMG_GENERIC );
		else
			pActivator->TakeDamage( pev, pev, pev->dmg, DMG_GENERIC );
	}
	
	SUB_UseTargets( pActivator, useType, value );

	if ( RemoveOnFire() )
	{
		UTIL_Remove( this );
	}
}



//
// CGameCounter / game_counter	-- Counts events and fires target
// Flag: Fire once
// Flag: Reset on Fire

#define SF_GAMECOUNT_FIREONCE			0x0001
#define SF_GAMECOUNT_RESET				0x0002

class CGameCounter : public CRulePointEntity
{
public:
	void		Spawn( void );
	void		Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	inline BOOL RemoveOnFire( void ) { return (pev->spawnflags & SF_GAMECOUNT_FIREONCE) ? TRUE : FALSE; }
	inline BOOL ResetOnFire( void ) { return (pev->spawnflags & SF_GAMECOUNT_RESET) ? TRUE : FALSE; }

	inline void CountUp( void ) { pev->frags++; }
	inline void CountDown( void ) { pev->frags--; }
	inline void ResetCount( void ) { pev->frags = pev->dmg; }
	inline int  CountValue( void ) { return pev->frags; }
	inline int	LimitValue( void ) { return pev->health; }
	
	inline BOOL HitLimit( void ) { return CountValue() == LimitValue(); }

private:

	inline void SetCountValue( int value ) { pev->frags = value; }
	inline void SetInitialValue( int value ) { pev->dmg = value; }
};

LINK_ENTITY_TO_CLASS( game_counter, CGameCounter );

void CGameCounter::Spawn( void )
{
	// Save off the initial count
	SetInitialValue( CountValue() );
	CRulePointEntity::Spawn();
}


void CGameCounter::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !CanFireForActivator( pActivator ) )
		return;

	switch( useType )
	{
	case USE_ON:
	case USE_TOGGLE:
		CountUp();
		break;
	
	case USE_OFF:
		CountDown();
		break;

	case USE_SET:
		SetCountValue( (int)value );
		break;
	}
	
	if ( HitLimit() )
	{
		SUB_UseTargets( pActivator, USE_TOGGLE, 0 );
		if ( RemoveOnFire() )
		{
			UTIL_Remove( this );
		}
		
		if ( ResetOnFire() )
		{
			ResetCount();
		}
	}
}



//
// CGameCounterSet / game_counter_set	-- Sets the counter's value
// Flag: Fire once

#define SF_GAMECOUNTSET_FIREONCE			0x0001

class CGameCounterSet : public CRulePointEntity
{
public:
	void		Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	inline BOOL RemoveOnFire( void ) { return (pev->spawnflags & SF_GAMECOUNTSET_FIREONCE) ? TRUE : FALSE; }

private:
};

LINK_ENTITY_TO_CLASS( game_counter_set, CGameCounterSet );


void CGameCounterSet::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !CanFireForActivator( pActivator ) )
		return;

	SUB_UseTargets( pActivator, USE_SET, pev->frags );

	if ( RemoveOnFire() )
	{
		UTIL_Remove( this );
	}
}


//
// CGamePlayerEquip / game_playerequip	-- Sets the default player equipment
// Flag: USE Only

#define SF_PLAYEREQUIP_USEONLY			0x0001
#define SF_PLAYEREQUIP_TARGET_FILTER	0x0002
#define SF_PLAYEREQUIP_ACTIVATOR_ONLY	0x0004
#define SF_PLAYEREQUIP_NON_EXCLUSIVE	0x0008
#define MAX_EQUIP		32

class CGamePlayerEquip : public CRulePointEntity
{
public:
	bool EquipOnSpawn( CBasePlayer *player );
	void ReEquipPlayers( void );
	void KeyValue( KeyValueData *pkvd );
	void Touch( CBaseEntity *pOther );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	inline BOOL UseOnly( void ) { return ( pev->spawnflags & SF_PLAYEREQUIP_USEONLY ) ? TRUE : FALSE; }

private:
	void EquipPlayer( CBaseEntity *pPlayer, bool reEquip );

	string_t	m_weaponNames[MAX_EQUIP];
	int			m_weaponCount[MAX_EQUIP];

	int m_iEquipMode;
	int m_iInventoryMode;
};

LINK_ENTITY_TO_CLASS( game_player_equip, CGamePlayerEquip );


void CGamePlayerEquip::KeyValue( KeyValueData *pkvd )
{
	CRulePointEntity::KeyValue( pkvd );

	if ( pkvd->fHandled )
		return;

	if ( !stricmp( pkvd->szKeyName, "equipmode" ) )
	{
		m_iEquipMode = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
		return;
	}

	if ( !stricmp( pkvd->szKeyName, "inventorymode" ) )
	{
		m_iInventoryMode = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
		return;
	}

	if ( !pkvd->fHandled )
	{
		for ( int i = 0; i < MAX_EQUIP; i++ )
		{
			if ( !m_weaponNames[i] )
			{
				char tmp[128];

				UTIL_StripToken( pkvd->szKeyName, tmp, sizeof( tmp ) );

				m_weaponNames[i] = ALLOC_STRING(tmp);
				m_weaponCount[i] = atoi(pkvd->szValue);
				m_weaponCount[i] = V_max(1,m_weaponCount[i]);
				pkvd->fHandled = TRUE;
				break;
			}
		}
	}
}

bool GamePlayerEquipOnSpawn( CBaseEntity *entity, CBasePlayer *player )
{
	return ( (CGamePlayerEquip *) entity )->EquipOnSpawn( player );
}

bool CGamePlayerEquip::EquipOnSpawn( CBasePlayer *player )
{
	if ( !CanFireForActivator( player ) || UseOnly() || m_iEquipMode != 0 )
		return true;

	Touch( player );

	return ( pev->spawnflags & SF_PLAYEREQUIP_NON_EXCLUSIVE ) != 0;
}

void CGamePlayerEquip::Touch( CBaseEntity *pOther )
{
	if ( !CanFireForActivator( pOther ) )
		return;

	if ( UseOnly() )
		return;

	EquipPlayer( pOther, false );
}

void CGamePlayerEquip::EquipPlayer( CBaseEntity *entity, bool reEquip )
{
	if ( m_iEquipMode != 0 )
		return;

	CBasePlayer *pPlayer;

	if ( entity->IsPlayer() )
		pPlayer = (CBasePlayer *) entity;
	else
		pPlayer = NULL;

	if ( !pPlayer )
		return;

	if ( ( pev->spawnflags & SF_PLAYEREQUIP_TARGET_FILTER ) && pev->target )
	{
		if ( !pPlayer->pev->targetname || !FStrEq( STRING( pev->target ), STRING( pPlayer->pev->targetname ) ) )
			return;
	}

	if ( reEquip )
	{
		pPlayer->RemoveAllItems( false, true );
		pPlayer->SetItemPickupTimes( gpGlobals->time );
	}

	for ( int i = 0; i < MAX_EQUIP && m_weaponNames[i]; ++i )
	{
		for ( int count = 0; count < m_weaponCount[i]; ++count )
			pPlayer->GiveNamedItem( STRING(m_weaponNames[i]) );
		}
	}

void CGamePlayerEquip::ReEquipPlayers( void )
{
	if ( m_iEquipMode != 0 )
		return;

	for ( int index = 1; index <= gpGlobals->maxClients; ++index )
	{
		CBasePlayer *player = (CBasePlayer *) UTIL_PlayerByIndex( index );

		if ( !player )
			continue;

		if ( ( pev->spawnflags & SF_PLAYEREQUIP_TARGET_FILTER ) && pev->target )
		{
			if ( !player->pev->targetname || !FStrEq( STRING( pev->target ), STRING( player->pev->targetname ) ) )
				continue;
		}

		player->RemoveAllItems( false, true );
		player->SetItemPickupTimes( gpGlobals->time );

		for ( int item = 0; item < MAX_EQUIP && m_weaponNames[item]; ++item )
		{
			for ( int count = 0; count < m_weaponCount[item]; ++count )
				player->GiveNamedItem( STRING( m_weaponNames[item] ) );
		}
	}
}

void CGamePlayerEquip::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( m_iEquipMode == 0 )
	{
		if ( !pActivator )
			return;

		if ( pev->spawnflags & SF_PLAYEREQUIP_ACTIVATOR_ONLY )
		{
			if ( pActivator->IsPlayer() )
				EquipPlayer( pActivator, true );
			else
				ReEquipPlayers();
}
		else
			EquipPlayer( pActivator, false );

		return;
	}

	if ( m_iEquipMode != 1 )
		return;

	for ( int index = 0; index < MAX_EQUIP && m_weaponNames[index]; ++index )
	{
		const char *name = STRING( m_weaponNames[index] );
		int count = m_weaponCount[index];

		unsigned long amount;

		if ( count < 0 )
			amount = 0UL - (unsigned long) count;
		else
			amount = (unsigned long) count;

		switch ( m_iInventoryMode )
		{
			case 0:
				if ( count > 0 )
					CStartInventory::GetInstance()->Set( name, count );
				break;

			case 1:
				CStartInventory::GetInstance()->Add( name, amount, count >= 0 );
				break;

			case 2:
				CStartInventory::GetInstance()->Add( name, amount, count < 0 );
				break;

			case 3:
				CStartInventory::GetInstance()->Remove( name );
				break;

			case 4:
				CStartInventory::GetInstance()->Restock( name, amount );
				break;

			case 5:
				CStartInventory::GetInstance()->Limit( name, amount );
				break;

			default:
				ALERT( at_error, "game_player_equip(%s): unknown inventory mode %d!\n", STRING( pev->targetname ), m_iInventoryMode );
				break;
		}
	}
}

//
// CGamePlayerTeam / game_player_team	-- Changes the team of the player who fired it
// Flag: Fire once
// Flag: Kill Player
// Flag: Gib Player

#define SF_PTEAM_FIREONCE			0x0001
#define SF_PTEAM_KILL    			0x0002
#define SF_PTEAM_GIB     			0x0004

class CGamePlayerTeam : public CRulePointEntity
{
public:
	void		Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

private:

	inline BOOL RemoveOnFire( void ) { return (pev->spawnflags & SF_PTEAM_FIREONCE) ? TRUE : FALSE; }
	inline BOOL ShouldKillPlayer( void ) { return (pev->spawnflags & SF_PTEAM_KILL) ? TRUE : FALSE; }
	inline BOOL ShouldGibPlayer( void ) { return (pev->spawnflags & SF_PTEAM_GIB) ? TRUE : FALSE; }
	
	const char *TargetTeamName( const char *pszTargetName );
};

LINK_ENTITY_TO_CLASS( game_player_team, CGamePlayerTeam );


const char *CGamePlayerTeam::TargetTeamName( const char *pszTargetName )
{
	CBaseEntity *pTeamEntity = NULL;

	while ((pTeamEntity = UTIL_FindEntityByTargetname( pTeamEntity, pszTargetName )) != NULL)
	{
		if ( FClassnameIs( pTeamEntity->pev, "game_team_master" ) )
			return pTeamEntity->TeamID();
	}

	return NULL;
}


void CGamePlayerTeam::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !CanFireForActivator( pActivator ) )
		return;

	if ( pActivator->IsPlayer() )
	{
		const char *pszTargetTeam = TargetTeamName( STRING(pev->target) );
		if ( pszTargetTeam )
		{
			CBasePlayer *pPlayer = (CBasePlayer *)pActivator;
			g_pGameRules->ChangePlayerTeam( pPlayer, pszTargetTeam, ShouldKillPlayer(), ShouldGibPlayer() );
		}
	}
	
	if ( RemoveOnFire() )
	{
		UTIL_Remove( this );
	}
}


class CGamePlayerRespawnZone : public CRuleBrushEntity
{
public:
	void KeyValue( KeyValueData *data );
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );
	int Save( CSave &save );
	int Restore( CRestore &restore );

	static TYPEDESCRIPTION m_SaveData[];

private:
	int m_iZoneType;
};

LINK_ENTITY_TO_CLASS( player_respawn_zone, CGamePlayerRespawnZone );

TYPEDESCRIPTION CGamePlayerRespawnZone::m_SaveData[] =
{
	DEFINE_FIELD( CGamePlayerRespawnZone, m_iZoneType, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE( CGamePlayerRespawnZone, CRuleBrushEntity );

void CGamePlayerRespawnZone::KeyValue( KeyValueData *data )
{
	if ( FStrEq( data->szKeyName, "zonetype" ) )
	{
		m_iZoneType = atoi( data->szValue );
		data->fHandled = TRUE;
	}
	else
	{
		CRuleBrushEntity::KeyValue( data );
	}
}

void CGamePlayerRespawnZone::Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value )
{
	if ( !CanFireForActivator( activator ) )
		return;

	for ( int index = 1; index <= gpGlobals->maxClients; ++index )
	{
		CBaseEntity *entity = UTIL_PlayerByIndex( index );

		if ( !entity )
			continue;

		TraceResult trace;
		int hull = ( entity->pev->flags & FL_DUCKING ) ? head_hull : human_hull;

		trace.vecEndPos = g_vecZero;
		trace.vecPlaneNormal = g_vecZero;

		UTIL_TraceModel( entity->pev->origin, entity->pev->origin, hull, edict(), &trace );

		int respawnDelay = (int) CVAR_GET_FLOAT( "mp_respawndelay" );

		if ( trace.fStartSolid ? m_iZoneType == 0 : m_iZoneType != 0 )
			continue;

		CBasePlayer *player = (CBasePlayer *) entity;
		EntityHandle<CBaseEntity> point = g_pGameRules->GetPlayerSpawnSpot( player );

		if ( respawnDelay > 0 )
			player->m_flRespawnDelayTime -= (float) respawnDelay;

		if ( !point )
			ALERT( at_console, "Error: player_respawn_zone: could not find any spawnspot in map!" );
	}
}
