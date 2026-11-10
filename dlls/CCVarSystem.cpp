#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "gamerules.h"
#include "server_support.h"
#include "CCVarSystem.h"
#include "CASCommand.h"
#include "CASConCommandSystem.h"

#include <ctype.h>

#define MAX_CVAR_NAME_LENGTH 60

static const int COMMAND_KIND_MISMATCH = 2;

extern int g_iSkillLevel;

bool SvenForceRespawn = false;
float g_plCountBalance = 1.0f;

CCVarSystem::CCVarSystem()
{
}

CCVarSystem *CCVarSystem::GetInstance( void )
{
	static CCVarSystem *instance;

	if ( !instance )
		instance = new CCVarSystem;

	return instance;
}

int CCVarSystem::SetCVar( const char *name, const char *value, SetType_t type )
{
	int result = SET_RESULT_ERROR;

	if ( type & SET_ENGINE_CVAR )
	{
		result = SetEngineCVar( name, value );

		if ( result == 0 || result == SET_RESULT_NO_NAME || result == SET_RESULT_OK )
			return result;
	}

	if ( type & SET_AS_CVAR )
		return SetASCVar( name, value );

	return result;
}

int CCVarSystem::SetEngineCVar( const char *name, const char *value )
{
	static const char *allowedNames[] = {
		"mp_allowmonsterinfo", "mp_allowplayerinfo", "mp_ammo_droprules",
		"mp_ammo_respawndelay", "mp_banana", "mp_barnacle_paralyze",
		"mp_disable_autoclimb", "mp_disable_medkit_points", "mp_disable_pcbalancing",
		"mp_disable_player_rappel", "mp_disablegaussjump", "mp_dropweapons",
		"mp_falldamage", "mp_flashlight", "mp_footsteps", "mp_forcerespawn",
		"mp_fraglimit", "mp_grapple_mode", "mp_hevsuit_voice", "mp_item_respawndelay",
		"mp_monsterpoints", "mp_multiplespawn", "mp_no_akimbo_uzis", "mp_noblastgibs",
		"mp_nomedkit", "mp_npckill", "mp_observer_cyclic", "mp_observer_mode",
		"mp_pcbalancing_factorlist", "mp_respawndelay", "mp_suitpower",
		"mp_survival_nextmap", "mp_survival_startdelay", "mp_survival_starton",
		"mp_survival_supported", "mp_survival_voteallow", "mp_telefrag", "mp_timelimit",
		"mp_weapon_droprules", "mp_weapon_respawndelay", "mp_weaponstay",
		"npc_dropweapons", "sv_accelerate", "sv_ai_enemy_detection_mode",
		"sv_airaccelerate", "sv_friction", "sv_gravity", "sv_maxspeed", "sv_maxvelocity",
		"sv_wateraccelerate", "sv_waterfriction", "angelscript_compiler_treat_warnings_as_errors"
	};

	if ( !name || !*name )
		return SET_RESULT_NO_NAME;

	unsigned int length = strlen( name );

	if ( length > MAX_CVAR_NAME_LENGTH )
		return SET_RESULT_NAME_TOO_LONG;

	char normalized[64];

	UTIL_CopyString( normalized, name, sizeof( normalized ) );

	for ( unsigned int i = 0; i < length; i++ )
		normalized[i] = tolower( normalized[i] );

	bool skillCvar = !strncmp( normalized, "sk_", 3 );

	if ( !skillCvar )
	{
		bool allowed = !strcmp( normalized, "npc_dropweapons" );

		if ( !strncmp( normalized, "sv_", 3 ) || !strncmp( normalized, "mp_", 3 ) || !strcmp( normalized, "skill" ) )
		{
			for ( unsigned int i = 0; i < ARRAYSIZE( allowedNames ); i++ )
			{
				if ( !strcmp( normalized, allowedNames[i] ) )
				{
					allowed = true;
					break;
				}
			}
		}

		if ( !allowed )
			return SET_RESULT_DISALLOWED;
	}

	if ( !CVAR_GET_POINTER( normalized ) )
	{
		const char *last = normalized + length - 1;

		if ( ( isdigit( *last ) && atoi( last ) == g_iSkillLevel ) || *last == '#' )
		{
			char baseName[256] = { 0 };

			UTIL_CopyString( baseName, normalized, length );

			if ( CVAR_GET_POINTER( baseName ) )
				CVAR_SET_STRING( baseName, value );
		}
	}

	if ( skillCvar && !CVAR_GET_POINTER( normalized ) )
		return SET_RESULT_UNKNOWN_SKILL;

	CVAR_SET_STRING( normalized, value );

	if ( !strcmp( normalized, "mp_barnacle_paralyze" ) )
		g_sBarnacleParalyze = atoi( value ) != 0;
	else if ( !strcmp( normalized, "mp_disable_autoclimb" ) )
		g_sDisableAutoclimb = atoi( value ) != 0;
	else if ( !strcmp( normalized, "mp_flashlight" ) )
	{
		if ( atoi( value ) == 0 )
		{
			for ( int i = 1; i <= gpGlobals->maxClients; i++ )
			{
				CBaseEntity *entity = UTIL_PlayerByIndex( i );

				if ( entity && entity->IsPlayer() )
				{
					CBasePlayer *player = (CBasePlayer *) entity;

					if ( player->FlashlightIsOn() )
						player->FlashlightTurnOff();
				}
			}
		}
	}
	else if ( !strcmp( normalized, "mp_forcerespawn" ) )
		SvenForceRespawn = atoi( value ) != 0;
	else if ( !strcmp( normalized, "mp_pcbalancing_factorlist" ) )
		g_plCountBalance = CBasePlayer::PlCountBalanceCalc();

	return SET_RESULT_OK;
}

int CCVarSystem::SetASCVar( const char *name, const char *value )
{
	CASCommand *command = new CASCommand( name, value );
	int result = CASConCommandSystem::GetInstance()->Execute( command, NULL, true );

	command->Release();

	if ( result == 0 )
		return SET_RESULT_OK;

	return result == COMMAND_KIND_MISMATCH ? SET_RESULT_DISALLOWED : SET_RESULT_ERROR;
}
