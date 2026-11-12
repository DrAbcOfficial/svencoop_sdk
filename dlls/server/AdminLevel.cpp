#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player/player.h"
#include "server/AdminLevel.h"
#include "server/CAdminList.h"

AdminLevel_t AdminLevel( CBasePlayer *player )
{
	edict_t *entity = player->pev ? ENT( player->pev ) : NULL;

	if ( !entity || entity->free || !OFFSET( entity ) )
		return ADMIN_NO;

	if ( player->m_iAdminLevel != ADMIN_INIT )
		return (AdminLevel_t) player->m_iAdminLevel;

	const char *authID = GETPLAYERAUTHID( player->pev ? ENT( player->pev ) : NULL );

	if ( !authID || !*authID )
		return ADMIN_NO;

	if ( !strcmp( authID, "VALVE_ID_LOOPBACK" ) )
		return ADMIN_OWNER;

	if ( !IS_DEDICATED_SERVER() && ENTINDEX( player->pev ? ENT( player->pev ) : NULL ) == 1 )
		return ADMIN_OWNER;

	if ( !g_pAdminList || !g_pAdminList->Find( authID ) )
		return ADMIN_NO;

	const CAdminList::Entry *entry = g_pAdminList->Find( authID );

	return entry ? entry->m_AdminLevel : ADMIN_NO;
}

AdminLevel_t StringToAdminLevel(const char *level)
{
	if (!level || !*level || !stricmp("ADMIN_NO", level))
		return ADMIN_NO;

	if (!stricmp("ADMIN_INIT", level))
		return ADMIN_INIT;

	if (!stricmp("ADMIN_YES", level))
		return ADMIN_YES;

	if (!stricmp("ADMIN_OWNER", level))
		return ADMIN_OWNER;

	return ADMIN_NO;
}

const char *AdminLevelToString( AdminLevel_t level )
{
	switch ( level )
	{
		case ADMIN_INIT:
			return "ADMIN_INIT";

		case ADMIN_NO:
			return "ADMIN_NO";

		case ADMIN_YES:
			return "ADMIN_YES";

		case ADMIN_OWNER:
			return "ADMIN_OWNER";
	}

	return "";
}
