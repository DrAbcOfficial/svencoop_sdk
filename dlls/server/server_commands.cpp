#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player/player.h"
#include "gamerules/gamerules.h"
#include "server/CServer.h"
#include "gamerules/CVoteManager.h"
#include "server/CAdminList.h"
#include "server/AdminLevel.h"
#include "server/server_support.h"
#include "sound/server_sound.h"
#include "server/server_commands.h"

struct playerban_t
{
	const char  *authId;
	float        expires;
	playerban_t *next;
};

static playerban_t g_PlayerBanList;
static bool g_bCustomServerCommandsRegistered;

bool AddPlayerBan( const char *authId, float seconds )
{
	ALERT( at_console, "Banning Player: %s\n", authId );

	if ( !authId )
		return false;

	playerban_t *last = &g_PlayerBanList;

	while ( last->next )
		last = last->next;

	playerban_t *ban = new playerban_t;

	ban->authId = authId;
	ban->expires = gpGlobals->time + seconds;
	ban->next = NULL;
	last->next = ban;
	return true;
}

bool RemovePlayerBan( const char *authId )
{
	SERVER_COMMAND( UTIL_VarArgs( "removeid %s\n", authId ) );

	playerban_t *previous = &g_PlayerBanList;
	playerban_t *ban = previous->next;

	while ( ban )
	{
		if ( !strcmp( ban->authId, authId ) )
		{
			previous->next = ban->next;
			delete ban;
			break;
		}

		previous = ban;
		ban = ban->next;
	}

	return true;
}

void RemoveAllBans()
{
	playerban_t *ban = g_PlayerBanList.next;

	while ( ban )
	{
		playerban_t *next = ban->next;

		SERVER_COMMAND( UTIL_VarArgs( "removeid %s\n", ban->authId ) );
		delete ban;
		ban = next;
	}

	g_PlayerBanList.next = NULL;
}

bool IsPlayerBanned( const char *authId )
{
	if ( !authId )
		return false;

	for ( playerban_t *ban = g_PlayerBanList.next; ban; ban = ban->next )
	{
		if ( strcmp( ban->authId, authId ) )
			continue;

		if ( ban->expires > gpGlobals->time )
			return true;

		RemovePlayerBan( ban->authId );
		break;
	}

	return false;
}

void ServerCommand_RemoveAllBans()
{
	RemoveAllBans();
	ALERT( at_console, "Removed all voted server bans.\n" );
}

void ServerCommand_RemoveBan()
{
	ALERT( at_console, "%s\n", CMD_ARGV( 1 ) );

	if ( RemovePlayerBan( CMD_ARGV( 1 ) ) )
		ALERT( at_console, "Removed ban for %s\n", CMD_ARGV( 1 ) );
	else
		ALERT( at_console, "Could not remove ban for %s\n", CMD_ARGV( 1 ) );
}

void ServerCommand_ShowBans()
{
	ALERT( at_console, "WON/STEAM Ban Statistics:\n" );

	for ( playerban_t *ban = g_PlayerBanList.next; ban; ban = ban->next )
	{
		ALERT( at_console, "WonID/STEAMID (%s) | Timeleft: %0.1f Minutes\n",
		       ban->authId, ( ban->expires - gpGlobals->time ) / 60.0f );
	}
}

void ServerCommand_AddVoteMap()
{
	if ( CMD_ARGC() < 1 )
	{
		ALERT( at_warning, "Map not specified. Usage: addvotemap [mapname]\n" );
		return;
	}

	const char *map = CMD_ARGV( 1 );

	if ( !IS_MAP_VALID( (char *) map ) )
	{
		ALERT( at_warning, "Invalid map \"%s\" specified.\n", map );
		return;
	}

	CVoteManager &votes = CServer::GetInstance()->GetVoteManager();

	if ( votes.IsMapInVoteList( map ) )
	{
		ALERT( at_warning, "Map \"%s\" already registered.\n", map );
		return;
	}

	if ( !votes.AddMapToVoteList( map ) )
		ALERT( at_aiconsole, "Failed to add \"%s\" from the map vote list.\n", map );
}

void ServerCommand_RemoveVoteMap()
{
	if ( CMD_ARGC() < 1 )
	{
		ALERT( at_warning, "Map not specified. Usage: removevotemap [mapname]\n" );
		return;
	}

	const char *map = CMD_ARGV( 1 );

	if ( !IS_MAP_VALID( (char *) map ) )
	{
		ALERT( at_warning, "Invalid map \"%s\" specified.\n", map );
		return;
	}

	CVoteManager &votes = CServer::GetInstance()->GetVoteManager();

	if ( !votes.IsMapInVoteList( map ) )
	{
		ALERT( at_warning, "Map \"%s\" not registered.\n", map );
		return;
	}

	if ( !votes.RemoveMapFromVoteList( map ) )
	{
		ALERT( at_aiconsole, "Failed to remove \"%s\" from the map vote list.\n", map );
		return;
	}

	ALERT( at_console, "Removed \"%s\" from the map vote list.\n", map );
}

void ServerCommand_ShowVoteMapList()
{
	CVoteManager &votes = CServer::GetInstance()->GetVoteManager();

	ALERT( at_console, "%d map(s) in the vote list:\n", votes.CountMapsInVoteList() );

	for ( unsigned int index = 0;; index++ )
	{
		const char *map = votes.GetMapInVoteList( index );

		if ( !map )
			break;

		if ( *map )
			ALERT( at_console, "- %s\n", map );
	}
}

void ServerCommand_ClearVoteMapList()
{
	CServer::GetInstance()->GetVoteManager().FreeMapVoteList();
	ALERT( at_console, "Cleared map vote list.\n" );
}

void ServerCommand_KickBySteamId()
{
	if ( !g_pGameRules || !g_pGameRules->IsMultiplayer() )
		return;

	if ( CMD_ARGC() < 2 )
	{
		ALERT( at_console, "Usage: kicksteamid \"<steam id>\" (With quotes)\n" );
		return;
	}

	const char *authId = CMD_ARGV( 1 );
	CBasePlayer *player = UTIL_PlayerBySteamId( authId );

	if ( !player )
	{
		ALERT( at_console, "No player with Steam ID '%s'!\n", authId );
		return;
	}

	SERVER_COMMAND( UTIL_VarArgs( "kick \"#%d\"\n", GETPLAYERUSERID( player->edict() ) ) );
}

void ServerCommand_Trigger()
{
	if ( CMD_ARGC() < 2 )
	{
		ALERT( at_console, "No target specified.\n" );
		return;
	}

	USE_TYPE useType = USE_TOGGLE;
	char useName[32] = { 0 };
	int valueIndex = 3;

	UTIL_CopyString( useName, "TOGGLE", sizeof( useName ) );

	const char *argument = CMD_ARGV( 2 );

	if ( argument )
	{
		const char *name = NULL;

		if ( !stricmp( argument, "on" ) )
		{
			useType = USE_ON;
			name = "ON";
		}
		else if ( !stricmp( argument, "off" ) )
		{
			useType = USE_OFF;
			name = "OFF";
		}
		else if ( !stricmp( argument, "set" ) )
		{
			useType = USE_SET;
			name = "SET";
		}
		else if ( !stricmp( argument, "toggle" ) )
		{
			name = "TOGGLE";
		}
		else if ( !stricmp( argument, "kill" ) )
		{
			useType = USE_KILL;
			name = "KILL";
		}

		if ( name )
		{
			UTIL_CopyString( useName, name, sizeof( useName ) );
			valueIndex = 2;
		}
	}

	float value = 0.0f;

	if ( CMD_ARGV( valueIndex ) )
		value = (float) atof( CMD_ARGV( valueIndex ) );

	ALERT( at_console, "Firing target \"%s\" with use type %s [%d] and value %.6f.\n",
	       CMD_ARGV( 1 ), useName, useType, value );
	FireTargets( CMD_ARGV( 1 ), NULL, NULL, useType, value );
}

void ServerCommand_LoadAdminList()
{
	if ( !g_pGameRules || !g_pGameRules->IsMultiplayer() )
		return;

	delete g_pAdminList;
	g_pAdminList = NULL;
	( (CHalfLifeMultiplay *) g_pGameRules )->LoadServerAdmins();

	for ( int index = 1; index <= gpGlobals->maxClients; index++ )
	{
		CBasePlayer *player = (CBasePlayer *) UTIL_PlayerByIndex( index );

		if ( !player )
			continue;

		player->m_iAdminLevel = ADMIN_INIT;
		player->m_iAdminLevel = AdminLevel( player );
		player->SendScoreInfo( NULL );
	}
}

void ServerCommand_RebuildSoundCacheFile()
{
	CServer_SoundEngine::GetSoundEngine()->WriteSoundList( true );
}

void AddCustomServerCommands()
{
	if ( g_bCustomServerCommandsRegistered )
		return;

	g_engfuncs.pfnAddServerCommand( "voteban_remove_all", ServerCommand_RemoveAllBans );
	g_engfuncs.pfnAddServerCommand( "voteban_remove", ServerCommand_RemoveBan );
	g_engfuncs.pfnAddServerCommand( "voteban_show", ServerCommand_ShowBans );
	g_engfuncs.pfnAddServerCommand( "addvotemap", ServerCommand_AddVoteMap );
	g_engfuncs.pfnAddServerCommand( "removevotemap", ServerCommand_RemoveVoteMap );
	g_engfuncs.pfnAddServerCommand( "showvotemaplist", ServerCommand_ShowVoteMapList );
	g_engfuncs.pfnAddServerCommand( "clearvotemaplist", ServerCommand_ClearVoteMapList );
	g_engfuncs.pfnAddServerCommand( "kicksteamid", ServerCommand_KickBySteamId );

	if ( IS_DEDICATED_SERVER() )
		g_engfuncs.pfnAddServerCommand( "trigger", ServerCommand_Trigger );

	g_engfuncs.pfnAddServerCommand( "s_trigger", ServerCommand_Trigger );
	g_engfuncs.pfnAddServerCommand( "reloadadminlist", ServerCommand_LoadAdminList );
	g_engfuncs.pfnAddServerCommand( "rebuildsoundcachefile", ServerCommand_RebuildSoundCacheFile );
	g_bCustomServerCommandsRegistered = true;
}
