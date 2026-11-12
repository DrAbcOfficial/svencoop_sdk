#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "server/server_support.h"
#include "player/player.h"
#include "gamerules/gamerules.h"
#include "world/env_render_individual.h"
#include "world/doors.h"
#include <ctype.h>
#include <limits.h>

#define SF_BREAK_SHOW_HUD_INFO	32

// mp_allowplayerinfo and mp_allowmonsterinfo
#define INFO_ALLIES_ONLY		2

int CountCharacters( const char *text, char character )
{
	if ( !text )
		return 0;

	int count = 0;

	for ( unsigned int index = 0; index < strlen( text ); index++ )
	{
		if ( text[index] == character )
			count++;
	}

	return count;
}

bool CBasePlayer::IsImpostor( const char *name )
{
	if ( !name )
		return false;

	unsigned int length = strlen( name );
	char lowerName[128 + 1];

	if ( length > sizeof( lowerName ) - 1 )
		length = sizeof( lowerName ) - 1;

	UTIL_CopyString( lowerName, name, sizeof( lowerName ) );

	for ( unsigned int index = 0; index < length; index++ )
		lowerName[index] = tolower((unsigned char) lowerName[index]);

	const char *match = strstr( lowerName, "sven" );

	if ( !match )
		match = strstr( lowerName, "seven" );

	if ( !match )
		return false;

	if ( !strstr( match, "king" ) && !strstr( match, "vikng" ) )
		return false;

	const char *authId = GETPLAYERAUTHID( edict() );

	if ( !strcmp( authId, "STEAM_0:0:13732" ) || !strcmp( authId, "STEAM_0:1:211231" )
		|| !strcmp( authId, "4294967295" ) || !strncmp( "VALVE_ID_", authId, 9 ) )
	{
		return false;
	}

	return strncmp( "STEAM_ID_", authId, 9 ) != 0;
}

bool CBasePlayer::IsValidInfoEntity( CBaseEntity *entity )
{
	if ( !entity )
		return false;

	CEnvRenderIndividual *renderer = GetEntityRenderOverride( EntityHandle<CBaseEntity>( entity ) );

	if ( renderer && renderer->pev
		&& renderer->pev->rendermode == kRenderTransTexture && renderer->pev->renderamt == 0.0f )
	{
		return false;
	}

	if ( ( FClassnameIs( entity->pev, "func_breakable" ) || FClassnameIs( entity->pev, "func_pushable" ) )
		&& ( entity->pev->spawnflags & SF_BREAK_SHOW_HUD_INFO ) )
	{
		return true;
	}

	if ( entity->ShowHUDInfo() == 1 )
		return true;

	if ( !entity->IsAlive() )
		return false;

	const char *className = STRING( entity->pev->classname );

	return !strstr( className, "func_" ) && !strstr( className, "item_" )
		&& !strstr( className, "weapon_" ) && !strstr( className, "info_" )
		&& !strstr( className, "env_" ) && !strstr( className, "multisource" )
		&& !strstr( className, "ammo_" ) && !strstr( className, "game_" );
}

void CBasePlayer::UpdateMonsterInfo( void )
{
	if ( m_flNextSBarUpdateTime > gpGlobals->time )
		return;

	CBaseEntity *entity = FindEntityForward( this, 12048.0f );

	if ( !entity || !IsValidInfoEntity( entity ) )
		return;

	if ( !entity->ShowHUDInfo() )
		return;

	if ( !entity->IsBSPModel() && !entity->IsAlive() )
		return;

	if ( entity->pev->health == 0.0f )
		return;

	CBaseMonster *monster = entity->MyMonsterPointer();
	CBasePlayer *targetPlayer = entity->MyPlayerPointer();
	int relationship = g_pGameRules->PlayerRelationship( this, entity );
	int mode = (int) CVAR_GET_FLOAT( targetPlayer ? "mp_allowplayerinfo" : "mp_allowmonsterinfo" );

	if ( !mode )
		return;

	if ( mode == INFO_ALLIES_ONLY && relationship != GR_TEAMMATE && relationship != GR_ALLY )
		return;

	const char *name;

	if ( monster )
	{
		if ( STRING( entity->pev->netname ) )
			name = targetPlayer ? STRING( targetPlayer->pev->netname ) : STRING( monster->m_FormattedName );
		else
		{
			name = strstr( STRING( entity->pev->classname ), "monster_" );

			if ( !name )
				name = STRING( entity->pev->classname );
		}
	}
	else if ( FClassnameIs( entity->pev, "func_breakable" ) || FClassnameIs( entity->pev, "func_pushable" ) )
		name = STRING( entity->pev->netname );
	else
		name = STRING( static_cast<CBaseDoor *>( entity )->m_iszDisplayName );

	int health = (int) entity->pev->health;
	int armor = (int) entity->pev->armorvalue;

	if ( entity->pev->health > 0.0f && entity->pev->health < 1.0f )
		health = 1;

	if ( entity->pev->armorvalue > 0.0f && entity->pev->armorvalue < 1.0f )
		armor = 1;

	char label[24] = {};
	char healthText[24] = {};
	char text[1024];

	if ( health == INT_MIN || ( entity->pev->flags & FL_GODMODE ) )
		UTIL_CopyString( healthText, "Invincible", sizeof( healthText ) );
	else
		UTIL_FormatString( healthText, sizeof( healthText ), "%d", health );

	if ( targetPlayer )
	{
		UTIL_CopyString( label, targetPlayer->m_fIsImpostor ? "IMPOSTER" : "Player", sizeof( label ) );

		if ( relationship == GR_TEAMMATE || relationship == GR_ALLY )
		{
			UTIL_FormatString( text, sizeof( text ), "%s:  %s\nHealth:  %s\nArmor:   %d\nScore:    %d\n",
			                   label, name, healthText, armor, (int) entity->pev->frags );
		}
		else
			UTIL_FormatString( text, sizeof( text ), "%s:  %s\nScore:    %d\n", label, name, (int) entity->pev->frags );
	}
	else if ( monster )
	{
		UTIL_CopyString( label, "Neutral", sizeof( label ) );

		switch ( relationship )
		{
			case GR_NOTTEAMMATE:
			case GR_ENEMY:
				UTIL_CopyString( label, "Enemy", sizeof( label ) );
				break;

			case GR_TEAMMATE:
			case GR_ALLY:
				UTIL_CopyString( label, "Friend", sizeof( label ) );
				break;
		}

		if ( strlen( label ) )
		{
			UTIL_FormatString( text, sizeof( text ), "%s:  %s\nHealth:  %s\nArmor:   %d\nFrags:    %d\n",
			                   label, name, healthText, armor, (int) entity->pev->frags );
		}
		else
		{
			UTIL_FormatString( text, sizeof( text ), "%s\nHealth:  %s\nArmor:   %d\nFrags:    %d\n",
			                   name, healthText, armor, (int) entity->pev->frags );
		}
	}
	else
		UTIL_FormatString( text, sizeof( text ), "%s\nStrength:  %d\n", name, health );

	int lines = CountCharacters( text, '\n' );
	hudtextparms_t parameters = {};

	parameters.x = 0.04f;
	parameters.y = (float) ( 0.62 - (double) lines * 0.0125 );
	parameters.r1 = 54;
	parameters.g1 = 72;
	parameters.b1 = 102;
	parameters.r2 = 72;
	parameters.g2 = 96;
	parameters.b2 = 136;

	switch ( relationship )
	{
		case GR_NOTTEAMMATE:
		case GR_ENEMY:
			parameters.r1 = 171;
			parameters.g1 = 23;
			parameters.b1 = 7;
			parameters.r2 = 207;
			parameters.g2 = 23;
			parameters.b2 = 7;
			break;

		case GR_TEAMMATE:
		case GR_ALLY:
			parameters.r1 = 7;
			parameters.g1 = 171;
			parameters.b1 = 95;
			parameters.r2 = 7;
			parameters.g2 = 207;
			parameters.b2 = 95;
			break;
	}

	parameters.a2 = 255;
	parameters.fadeoutTime = 0.06f;
	parameters.holdTime = 1.75f;
	parameters.channel = 3;

	UTIL_HudMessage( this, parameters, text );

	m_flNextSBarUpdateTime = (float) ( (double) gpGlobals->time + 0.125 );
}
