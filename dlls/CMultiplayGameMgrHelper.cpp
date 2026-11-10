#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "CMultiplayGameMgrHelper.h"

CMultiplayGameMgrHelper g_GameMgrHelper;

bool CMultiplayGameMgrHelper::CanPlayerHearPlayer( CBasePlayer *listener, CBasePlayer *talker )
{
	if ( !listener || !talker )
		return false;

	int talkerIndex = ENTINDEX( talker->pev ? talker->pev->pContainingEntity : NULL );
	int listenerIndex = ENTINDEX( listener->pev ? listener->pev->pContainingEntity : NULL );

	return !IsPlayerBanned( listenerIndex, talkerIndex );
}

bool CMultiplayGameMgrHelper::IsPlayerBanned( int listener, int talker )
{
	for ( BanMap_t::iterator entry = m_Bans.begin(); entry != m_Bans.end(); entry++ )
	{
		if ( entry->first == listener && entry->second == talker )
			return true;
	}

	return false;
}

void CMultiplayGameMgrHelper::SetPlayerBanned( int listener, int talker, bool banned )
{
	if ( banned )
	{
		if ( !IsPlayerBanned( listener, talker ) )
			m_Bans.insert( std::make_pair( listener, talker ) );

		return;
	}

	for ( BanMap_t::iterator entry = m_Bans.begin(); entry != m_Bans.end(); )
	{
		BanMap_t::iterator current = entry++;

		if ( current->first == listener && current->second == talker )
			m_Bans.erase( current );
	}
}

void CMultiplayGameMgrHelper::ClearBansforListener( int listener )
{
	for ( BanMap_t::iterator entry = m_Bans.begin(); entry != m_Bans.end(); )
	{
		BanMap_t::iterator current = entry++;

		if ( current->first == listener )
			m_Bans.erase( current );
	}
}

void CMultiplayGameMgrHelper::ClearBansforTalker( int talker )
{
	for ( BanMap_t::iterator entry = m_Bans.begin(); entry != m_Bans.end(); )
	{
		BanMap_t::iterator current = entry++;

		if ( current->second == talker )
			m_Bans.erase( current );
	}
}

void CMultiplayGameMgrHelper::ClearBans( void )
{
	m_Bans.clear();
}

void CMultiplayGameMgrHelper::PrintBanList( entvars_s *client )
{
	ClientPrint( client, HUD_PRINTCONSOLE, "VoiceBans: \n" );

	for ( BanMap_t::iterator entry = m_Bans.begin(); entry != m_Bans.end(); entry++ )
	{
		char text[512];

		snprintf( text, 510, "[L: %i T: %i]\n", entry->first, entry->second );
		text[511] = 0;
		ClientPrint( client, HUD_PRINTCONSOLE, text );
	}

	for ( int listenerIndex = 1; listenerIndex <= gpGlobals->maxClients; listenerIndex++ )
	{
		CBaseEntity *listener = UTIL_PlayerByIndex( listenerIndex );

		if ( !listener )
			continue;

		for ( int talkerIndex = 1; talkerIndex <= gpGlobals->maxClients; talkerIndex++ )
		{
			CBaseEntity *talker = UTIL_PlayerByIndex( talkerIndex );

			if ( !talker )
				continue;

			int talkerEntityIndex = ENTINDEX( talker->pev ? talker->pev->pContainingEntity : NULL );
			int listenerEntityIndex = ENTINDEX( listener->pev ? listener->pev->pContainingEntity : NULL );

			if ( !IsPlayerBanned( listenerEntityIndex, talkerEntityIndex ) )
				continue;

			ClientPrint( client, HUD_PRINTCONSOLE, "%s can NOT hear %s\n",
			    STRING( listener->pev->netname ), STRING( talker->pev->netname ) );
		}
	}
}
