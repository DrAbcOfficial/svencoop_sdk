#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "AdminLevel.h"
#include "ConCommandDefs.h"
#include "server_support.h"

#define CHEATS_ADMIN_ONLY		2
#define CHEATS_ANY_ADMIN		255
#define CHEATS_MAX_PLAYERS		3

bool CheatsAllowed( CBasePlayer *player, const char *name, bool requireAlive, bool silent )
{
	if ( !player || !name || strlen( name ) == 0 )
		return false;

	if ( !player->IsAlive() && requireAlive )
	{
		if ( !silent )
			ClientPrint( player->pev, HUD_PRINTCENTER, "%s N/A: You're dead", name );

		return false;
	}

	if ( player->IsObserver() && requireAlive )
	{
		if ( !silent )
			ClientPrint( player->pev, HUD_PRINTCENTER, "%s N/A: You're observing", name );

		return false;
	}

	int cheats = (int) CVAR_GET_POINTER( "sv_cheats" )->value;
	bool allowed = true;

	if ( cheats >= CHEATS_ADMIN_ONLY )
	{
		if ( AdminLevel( player ) < ADMIN_YES || ( cheats < CHEATS_ANY_ADMIN && AdminLevel( player ) < cheats - 1 ) )
		{
			allowed = false;

			if ( !silent )
				ClientPrint( player->pev, HUD_PRINTCENTER, "%s N/A: Cheats restricted", name );
		}
	}
	else if ( cheats < 1 )
	{
		allowed = false;

		if ( !silent )
			ClientPrint( player->pev, HUD_PRINTCENTER, "%s N/A: Cheats disabled", name );
	}
	else if ( gpGlobals->maxClients > CHEATS_MAX_PLAYERS )
	{
		allowed = false;

		if ( !silent )
		{
			char message[128];

			UTIL_FormatString( message, sizeof( message ), "%s N/A: Player limit above %d", name, CHEATS_MAX_PLAYERS );
			ClientPrint( player->pev, HUD_PRINTCENTER, message );
		}
	}

	if ( !allowed && !silent )
	{
		const char *playerName = STRING( player->pev ? player->pev->netname : 0 );

		UTIL_LogPrintf( "Cheat command \"%s\" from \"%s\" blocked.\n", name, playerName );
	}

	return allowed;
}
