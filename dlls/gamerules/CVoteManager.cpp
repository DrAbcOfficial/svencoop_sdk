#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player/player.h"
#include "gamerules/gamerules.h"
#include "gamerules/game.h"
#include "gamerules/CBaseVote.h"
#include "gamerules/CVoteManager.h"
#include "server/CServer.h"
#include "gamerules/CClassicMode.h"
#include "gamerules/CSurvivalMode.h"
#include "server/AdminLevel.h"
#include "server/server_support.h"
#include "server/user_messages.h"

CVoteManager::CVoteManager() : m_pCustomVote( NULL )
{
	memset( &m_MapVoteList, 0, sizeof( m_MapVoteList ) );
	InitVoteVars();
}

CVoteManager::~CVoteManager()
{
}

const char *CVoteManager::GetVoteActionName( VOTEINPUT input ) const
{
	if ( m_pCustomVote )
		return input == VOTE_YES ? m_pCustomVote->GetYesText() : m_pCustomVote->GetNoText();

	return input == VOTE_YES ? "Yes" : "No";
}

const char *CVoteManager::GetVoteTypeName( VOTETYPE type ) const
{
	switch ( type )
	{
		case VOTE_KILL:
			return "Vote Kill";
		case VOTE_KICK:
			return "Vote Kick";
		case VOTE_BAN:
			return "Vote Ban";
		case VOTE_CUSTOM:
			return m_pCustomVote ? m_pCustomVote->GetName() : "Custom";
		default:
			return "Invalid";
	}
}

float CVoteManager::PercentageNeeded( VOTETYPE type ) const
{
	switch ( type )
	{
		case VOTE_KICK:
			return mp_votekickrequired.value;
		case VOTE_BAN:
			return mp_votebanrequired.value;
		case VOTE_MAP:
			return mp_votemaprequired.value;
		case VOTE_CLASSICMODE:
			return mp_voteclassicmoderequired.value;
		case VOTE_SURVIVALMODE:
			return mp_votesurvivalmoderequired.value;
		case VOTE_CUSTOM:
			return m_pCustomVote->GetPercentageNeeded();
		default:
			return mp_votekillrequired.value;
	}
}

void CVoteManager::InitVoteVars()
{
	m_flNextVoteTime = gpGlobals->time;
	m_flVoteEndTime = gpGlobals->time;
	m_iVoteType = VOTE_NONE;
	m_iVotesNo = 0;
	m_iVotesYes = 0;
	m_iVoters = 0;

	if ( m_pCustomVote )
		m_pCustomVote->Release();

	m_pCustomVote = NULL;
	memset( m_szVotedAuthId, 0, sizeof( m_szVotedAuthId ) );
	memset( m_szVotedPlayerName, 0, sizeof( m_szVotedPlayerName ) );
	m_hVotedPlayer = NULL;
	memset( m_szVotedMap, 0, sizeof( m_szVotedMap ) );

	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBasePlayer *player = (CBasePlayer *) UTIL_PlayerByIndex( i );

		if ( player )
			player->ResetVote();
	}
}

void CVoteManager::MapInit()
{
	InitVoteVars();
}

bool CVoteManager::FActiveVote() const
{
	return m_iVoteType != VOTE_NONE;
}

bool CVoteManager::CanStartNewVote( CBasePlayer *player, VOTETYPE type ) const
{
	if ( FActiveVote() || m_flNextVoteTime > gpGlobals->time )
		return false;

	if ( player->m_flNextVoteTime != 0.0f && player->m_flNextVoteTime > gpGlobals->time )
		return false;

	if ( player->CanStartNextVote( VOTE_NONE ) || !( CVAR_GET_FLOAT( "sv_cheats" ) < 1.0f ) )
		return true;

	return CVAR_GET_FLOAT( "sv_cheats" ) == 0.0f && CServer::GetInstance()->GetPlayerCount() > 1;
}

float CVoteManager::TimeVoteEnds() const
{
	return m_flVoteEndTime;
}

void CVoteManager::SetVotedMap( const char *name )
{
	if ( name )
		UTIL_CopyString( m_szVotedMap, name, sizeof( m_szVotedMap ) );
}

void CVoteManager::SetVotedPlayer( CBasePlayer *player )
{
	m_hVotedPlayer = NULL;

	if ( !player )
		return;

	m_hVotedPlayer = player;
	UTIL_CopyString( m_szVotedAuthId, GETPLAYERAUTHID( player->edict() ), sizeof( m_szVotedAuthId ) );
	UTIL_CopyString( m_szVotedPlayerName, STRING( player->pev->netname ), sizeof( m_szVotedPlayerName ) );
}

void CVoteManager::ClearCustomVote( CBaseVote *vote )
{
	if ( vote && m_pCustomVote == vote )
		InitVoteVars();
}

void CVoteManager::PlayerVoted( CBasePlayer *player, VOTEINPUT input )
{
	if ( !player )
		return;

	if ( player->HasVoted() )
	{
		ClientPrint( player->pev, HUD_PRINTCENTER, "Already voted %s.\n", GetVoteActionName( player->LastVoteInput() ) );
		return;
	}

	player->Vote( input, GetVoteActionName( input ) );

	if ( player->LastVoteInput() == VOTE_YES )
		m_iVotesYes++;
	else if ( player->LastVoteInput() == VOTE_NO )
		m_iVotesNo++;

	m_iVoters++;
}

void CVoteManager::Think()
{
	if ( FActiveVote() && gpGlobals->time > m_flVoteEndTime )
		EndVote();
}

void CVoteManager::ShowVoteMenu( CBasePlayer *player, const char *text, VOTETYPE type, const char *yes, const char *no )
{
	if ( !player || !text )
		return;

	MESSAGE_BEGIN( MSG_ONE, gmsgVoteMenu, NULL, player->edict() );
	WRITE_BYTE( type );
	WRITE_STRING( text );
	WRITE_STRING( yes );
	WRITE_STRING( no );
	MESSAGE_END();
	ClientPrint( player->pev, HUD_PRINTCONSOLE, "Type 'voteyes' or 'voteno' in console to vote now.\n" );
}

void CVoteManager::ShowVoteMenu_All( const char *text, VOTETYPE type, const char *yes, const char *no )
{
	if ( !text || type == VOTE_NONE )
		return;

	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBasePlayer *player = (CBasePlayer *) UTIL_PlayerByIndex( i );

		if ( player )
			ShowVoteMenu( player, text, type, yes, no );
	}
}

void CVoteManager::StartCustomVote( CBaseVote *vote )
{
	if ( !vote )
		return;

	if ( FActiveVote() || (float) ( m_flVoteEndTime + 1.0 ) > gpGlobals->time )
	{
		vote->VoteBlocked( (float) ( m_flVoteEndTime + 1.0 ) );
		return;
	}

	InitVoteVars();
	m_pCustomVote = vote;
	vote->AddRef();
	float length = vote->GetVoteLength();

	m_flVoteEndTime = length + gpGlobals->time;
	m_flNextVoteTime = (float) ( m_flVoteEndTime + 2.0 );
	UTIL_LogPrintf( "Started custom vote: %s\n", vote->GetName() );

	const char *no = vote->GetNoText();
	const char *yes = vote->GetYesText();
	const char *message = vote->GetVoteMessage();

	ShowVoteMenu_All( message, VOTE_CUSTOM, yes, no );
	m_iVoteType = VOTE_CUSTOM;
}

bool CVoteManager::IsMapInVoteList( const char *name ) const
{
	if ( !name )
		return false;

	for ( const MapVote *map = m_MapVoteList.next; map; map = map->next )
	{
		if ( !strcmp( map->name, name ) )
			return true;
	}

	return false;
}

bool CVoteManager::IsMapAllowedForVote( const char *name ) const
{
	if ( !name )
		return false;

	return m_MapVoteList.next ? IsMapInVoteList( name ) : IS_MAP_VALID( (char *) name ) != 0;
}

bool CVoteManager::AddMapToVoteList( const char *name )
{
	if ( !name || !IS_MAP_VALID( (char *) name ) )
		return false;

	if ( IsMapInVoteList( name ) )
		return true;

	MapVote *last = &m_MapVoteList;

	while ( last->next )
		last = last->next;

	last->next = new MapVote;
	UTIL_CopyString( last->next->name, name, sizeof( last->next->name ) );
	last->next->next = NULL;
	return true;
}

int CVoteManager::CountMapsInVoteList() const
{
	int count = 0;

	for ( const MapVote *map = m_MapVoteList.next; map; map = map->next )
		count++;

	return count;
}

const char *CVoteManager::GetMapInVoteList( unsigned int index ) const
{
	const MapVote *map = m_MapVoteList.next;

	while ( map && index )
	{
		map = map->next;
		index--;
	}

	return map ? map->name : NULL;
}

bool CVoteManager::RemoveMapFromVoteList( const char *name )
{
	if ( !name )
		return false;

	MapVote *previous = &m_MapVoteList;

	while ( previous->next )
	{
		MapVote *map = previous->next;

		if ( !strcmp( map->name, name ) )
		{
			previous->next = map->next;
			delete map;
			break;
		}

		previous = map;
	}

	return true;
}

void CVoteManager::FreeMapVoteList()
{
	MapVote *map = m_MapVoteList.next;

	while ( map )
	{
		MapVote *next = map->next;

		delete map;
		map = next;
	}

	m_MapVoteList.next = NULL;
}

CBasePlayer *CVoteManager::VotedPlayer()
{
	return m_hVotedPlayer;
}

bool CVoteManager::PerformVoteAction( VOTETYPE type )
{
	switch ( type )
	{
		case VOTE_KILL:
		case VOTE_KICK:
		case VOTE_BAN:
		{
			const char *log = UTIL_IsPlayerConnected( VotedPlayer() )
			                      ? PlayerLog( VotedPlayer()->edict() )
			                      : PlayerLogString( m_szVotedPlayerName, NULL, m_szVotedAuthId );
			const char *action = type == VOTE_KILL ? "kill" : type == VOTE_KICK ? "kick"
			                                                                    : "ban";

			if ( type == VOTE_KILL )
				log = PlayerLog( VotedPlayer()->edict() );

			UTIL_LogPrintf( "Vote to %s \"%s\" has been passed, performing action.\n", action, log );

			if ( type == VOTE_KILL )
				UTIL_ClientPrintAll( HUD_PRINTNOTIFY, UTIL_VarArgs( "Vote killing \"%s\".\n", m_szVotedPlayerName ) );
			else
				UTIL_ClientPrintAll( HUD_PRINTNOTIFY, UTIL_VarArgs( type == VOTE_KICK
				                                                        ? "Vote kicking \"%s\" for %0.1f minutes.\n"
				                                                        : "Vote banning \"%s\" for %0.1f minutes.\n",
				                                                    m_szVotedPlayerName,
				                                                    type == VOTE_KICK ? mp_kickbantime.value : mp_bantime.value ) );

			bool connected = UTIL_IsPlayerConnected( VotedPlayer() );

			if ( connected )
				CServer::GetInstance()->KillPlayer( VotedPlayer() );

			if ( type == VOTE_KILL )
				return connected;

			if ( type == VOTE_KICK )
			return CServer::GetInstance()->KickPlayer( VotedPlayer(), m_szVotedAuthId );

			return CServer::GetInstance()->BanPlayer( VotedPlayer(), m_szVotedAuthId, -1.0f );
		}
		case VOTE_MAP:
			if ( !m_szVotedMap[0] )
				return false;

			UTIL_LogPrintf( "Vote for map change to \"%s\" has been passed, performing action.\n", m_szVotedMap );
			UTIL_ClientPrintAll( HUD_PRINTCENTER, UTIL_VarArgs( "Changing map to \"%s\" by vote...\n", m_szVotedMap ) );
			CVAR_SET_STRING( "mp_nextmap", m_szVotedMap );
			CVAR_SET_STRING( "mp_nextmap_cycle", m_szVotedMap );
			static_cast<CHalfLifeMultiplay *>( g_pGameRules )->ChangeLevel();
			UTIL_ClientPrintAll( HUD_PRINTNOTIFY, UTIL_VarArgs( "Vote map changing to \"%s\".\n", m_szVotedMap ) );
			return true;

		case VOTE_CLASSICMODE:
		case VOTE_SURVIVALMODE:
		{
			const char *action = ( type == VOTE_CLASSICMODE ? CClassicMode::GetInstance()->IsEnabled() : CSurvivalMode::GetInstance()->IsEnabled() ) ? "disable" : "enable";
			const char *mode = type == VOTE_CLASSICMODE ? "classic" : "survival";

			UTIL_LogPrintf( "Vote to %s %s mode has been passed, performing action.\n", action, mode );
			UTIL_ClientPrintAll( HUD_PRINTNOTIFY, UTIL_VarArgs( type == VOTE_CLASSICMODE ? "Vote to %s classic mode passed.\n" : "Vote to %s survival mode passed.", action ) );

			if ( type == VOTE_CLASSICMODE )
				CClassicMode::GetInstance()->Toggle();
			else
				CSurvivalMode::GetInstance()->VoteToggle();

			return true;
		}
		default:
			ALERT( at_console, "CHalfLifeMultiplay::PerformVoteAction(): Invalid vote type passed as argument.\n" );
			return false;
	}
}

void CVoteManager::EndVote()
{
	float percentage = ( (float) m_iVotesYes / (float) m_iVoters ) * 100.0f;

	ALERT( at_console, "Yes: %d, No: %d, Voted: %d\n Percent: %0.1f\n Needed: %0.1f\n",
	       m_iVotesYes, m_iVotesNo, m_iVoters, percentage, PercentageNeeded( m_iVoteType ) );

	if ( m_pCustomVote && m_iVoteType == VOTE_CUSTOM )
	{
		m_pCustomVote->VoteEnd( percentage >= PercentageNeeded( VOTE_CUSTOM ), m_iVoters );
	}
	else
	{
		bool failed = percentage < PercentageNeeded( m_iVoteType );
		const char *status = failed ? "failed" : "succeeded";

		if ( m_szVotedMap[0] )
		{
			UTIL_LogPrintf( "Vote for map change to \"%s\" finished, %s. (%0.3f%% reached, needed %0.3f%%.)\n",
			                m_szVotedMap, failed ? "but failed" : "succeeded", percentage, PercentageNeeded( m_iVoteType ) );
			UTIL_ClientPrintAll( HUD_PRINTNOTIFY, UTIL_VarArgs( "Vote for map change to \"%s\" %s.\n", m_szVotedMap, status ) );
		}
		else if ( m_szVotedAuthId[0] )
		{
			const char *log = UTIL_IsPlayerConnected( VotedPlayer() )
			                      ? PlayerLog( VotedPlayer()->edict() )
			                      : PlayerLogString( m_szVotedPlayerName, NULL, m_szVotedAuthId );

			UTIL_LogPrintf( "Vote against player \"%s\" finished, %s. (%0.3f%% reached, needed %0.3f%%.)\n",
			                log, failed ? "but failed" : "succeeded", percentage, PercentageNeeded( m_iVoteType ) );
			UTIL_ClientPrintAll( HUD_PRINTNOTIFY, UTIL_VarArgs( "Vote against player \"%s\" %s.\n", m_szVotedPlayerName, status ) );
		}
		else if ( m_iVoteType != VOTE_NONE )
		{
			UTIL_LogPrintf( "Vote to change classic/survival mode finished, %s. (%0.3f%% reached, needed %0.3f%%.)\n",
			                failed ? "but failed" : "succeeded", percentage, PercentageNeeded( m_iVoteType ) );
			UTIL_ClientPrintAll( HUD_PRINTNOTIFY, UTIL_VarArgs( "Vote to change classic/survival mode %s.\n", status ) );
		}
		else
		{
			UTIL_ClientPrintAll( HUD_PRINTNOTIFY, failed ? "Vote failed.\n" : "Vote succeeded.\n" );
		}

		if ( !failed && !PerformVoteAction( m_iVoteType ) )
			UTIL_LogPrintf( "Failed to perform vote action!\n" );
	}

	InitVoteVars();
	m_iVoteType = VOTE_NONE;
	MESSAGE_BEGIN( MSG_ALL, gmsgEndVote );
	MESSAGE_END();
}

void CVoteManager::StartVote( VOTETYPE type, CBaseEntity *initiator, const char *argument )
{
	if ( !initiator || type == VOTE_NONE || !argument || !initiator->IsPlayer() )
		return;

	CBasePlayer *player = (CBasePlayer *) initiator;

	if ( !CanStartNewVote( player, type ) )
	{
		float remaining = V_max( m_flNextVoteTime - gpGlobals->time, player->TimeToStartNextVote() );

		remaining = remaining >= 0.0f ? V_max( m_flNextVoteTime - gpGlobals->time, player->TimeToStartNextVote() ) : 0.0f;
		ClientPrint( player->pev, HUD_PRINTCENTER, UTIL_VarArgs( "Can't start vote yet, wait %.1f seconds.", remaining ) );
		ClientPrint( player->pev, HUD_PRINTCONSOLE, UTIL_VarArgs( "You must wait %.1f seconds before\nstarting a new vote.\n", remaining ) );
		return;
	}

	if ( player->m_flNextMiscCommandTime > gpGlobals->time )
	{
		float remaining = player->m_flNextMiscCommandTime - gpGlobals->time;

		ClientPrint( player->pev, HUD_PRINTCENTER, UTIL_VarArgs( "Can't start vote yet, try again in %.1f seconds.", remaining ) );
		ClientPrint( player->pev, HUD_PRINTCONSOLE, UTIL_VarArgs( "You must wait %.1f seconds before\ntrying a new vote.\n", remaining ) );
		return;
	}

	player->m_flNextMiscCommandTime = gpGlobals->time + 4.0f;
	InitVoteVars();

	const char *typeName = GetVoteTypeName( type );

	SetVotedPlayer( UTIL_CBasePlayerByName( argument, true ) );

	float required = PercentageNeeded( type );
	float needed = required >= -1.0f ? required : -1.0f;

	needed = required <= 100.0f ? needed : 100.0f;

	if ( needed < 0.0f )
	{
		ClientPrint( player->pev, HUD_PRINTCENTER, "%s is disabled on this server.", typeName );
		UTIL_LogPrintf( "\"%s\" tried to use disabled %s, denied.\n", PlayerLog( player->edict() ), typeName );
		return;
	}

	if ( type >= VOTE_KILL && type <= VOTE_BAN )
	{
		if ( !VotedPlayer() )
			return;

		const char *reason = NULL;

		if ( !UTIL_IsPlayerConnected( VotedPlayer() ) || !VotedPlayer()->IsNetClient() )
			reason = "non-connected player";
		else if ( type == VOTE_KILL && ( VotedPlayer() )->IsObserver() )
			reason = "observer";
		else if ( type == VOTE_KILL && !VotedPlayer()->IsAlive() )
			reason = "dead player";
		else if ( type != VOTE_KILL && AdminLevel( VotedPlayer() ) > AdminLevel( player ) )
			reason = "player with higher administrator level";

		if ( reason )
		{
			const char *message = !strcmp( reason, "observer" )               ? "You cannot %s observers.\n"
			                      : !strcmp( reason, "dead player" )          ? "You cannot %s dead players.\n"
			                      : !strcmp( reason, "non-connected player" ) ? "You cannot %s non-connected players.\n"
			                                                                  : "You cannot %s administrators.\n";

			ClientPrint( player->pev, HUD_PRINTNOTIFY, message, typeName );
			if ( !strcmp( reason, "player with higher administrator level" ) )
				UTIL_LogPrintf( "\"%s\" tried to %s player \"%s\" with higher administrator level, denied.\n",
				                PlayerLog( player->edict() ), typeName, PlayerLog( VotedPlayer()->edict() ) );
			else
				UTIL_LogPrintf( "\"%s\" tried to %s %s \"%s\", denied.\n", PlayerLog( player->edict() ), typeName,
				                reason, PlayerLog( VotedPlayer()->edict() ) );
			return;
		}
	}

	const char *menu = NULL;
	const char *subject = NULL;

	switch ( type )
	{
		case VOTE_KILL:
		case VOTE_KICK:
		case VOTE_BAN:
		{
			const char *action = type == VOTE_KILL ? "kill" : type == VOTE_KICK ? "kick"
			                                                                    : "ban";

			UTIL_LogPrintf( type == VOTE_KILL   ? "\"%s\" started a kill vote against \"%s\"\n"
			                : type == VOTE_KICK ? "\"%s\" started a kick vote against \"%s\".\n"
			                                    : "\"%s\" started a ban vote against \"%s\".\n",
			                PlayerLog( player->edict() ), PlayerLog( VotedPlayer()->edict() ) );
			UTIL_ClientPrintAll( HUD_PRINTNOTIFY, UTIL_VarArgs( "Vote to %s \"%s\" started by \"%s\".\n", action, argument, STRING( player->pev->netname ) ) );
			subject = STRING( VotedPlayer()->pev->netname );
			menu = type == VOTE_KILL   ? "Would you like to kill \"%s\"? (%.0f percent needed.)"
			       : type == VOTE_KICK ? "Would you like to kick \"%s\" from the server? (%.0f percent needed.)"
			                           : "Do you wish to ban \"%s\" from the server? (%.0f percent needed.)";
			break;
		}
		case VOTE_MAP:
			SetVotedMap( argument );

			if ( !IsMapAllowedForVote( m_szVotedMap ) )
			{
				ClientPrint( player->pev, HUD_PRINTCONSOLE, UTIL_VarArgs( "Map \"%s\" not allowed for voting.\n", m_szVotedMap ) );
				return;
			}

			UTIL_LogPrintf( "\"%s\" started a map vote for \"%s\".\n", PlayerLog( player->edict() ), m_szVotedMap );
			UTIL_ClientPrintAll( HUD_PRINTNOTIFY, "Vote for map \"%s\" started by \"%s\".\n", argument, STRING( player->pev->netname ) );
			subject = m_szVotedMap;
			menu = "Would you like to change the map to \"%s\"? (%.0f percent needed.)";
			break;

		case VOTE_CLASSICMODE:
			if ( !CClassicMode::GetInstance()->m_bMapSupport )
			{
				ClientPrint( player->pev, HUD_PRINTCONSOLE, "Classic mode is not supported on this map.\n" );
				return;
			}

			if ( CClassicMode::GetInstance()->m_iNextState != -1 )
			{
				ClientPrint( player->pev, HUD_PRINTCONSOLE, "Classic mode voting not allowed.\n" );
				return;
			}

			// Fall through to the shared mode-vote presentation
		case VOTE_SURVIVALMODE:
		{
			if ( type == VOTE_SURVIVALMODE && !CSurvivalMode::GetInstance()->MapSupportEnabled() )
			{
				ClientPrint( player->pev, HUD_PRINTCONSOLE, "Survival mode is not supported on this map.\n" );
				return;
			}

			bool enabled = type == VOTE_CLASSICMODE ? CClassicMode::GetInstance()->IsEnabled() : CSurvivalMode::GetInstance()->IsEnabled();
			const char *mode = type == VOTE_CLASSICMODE ? "classic" : "survival";

			subject = enabled ? "disable" : "enable";
			UTIL_LogPrintf( "\"%s\" started a vote to %s %s mode.\n", PlayerLog( player->edict() ), subject, mode );
			UTIL_ClientPrintAll( HUD_PRINTNOTIFY, UTIL_VarArgs( "%s %s mode started by \"%s\".\n", enabled ? "Disable" : "Enable", mode, STRING( player->pev->netname ) ) );
			menu = type == VOTE_CLASSICMODE ? "Would you like to %s classic mode? (%.0f percent needed.)" : "Would you like to %s survival mode? (%.0f percent needed.)";
			break;
		}
		default:
			ClientPrint( player->pev, HUD_PRINTCONSOLE, "Vote type invalid.\n" );
			return;
	}

	ShowVoteMenu_All( UTIL_VarArgs( menu, subject, needed ), type, "", "" );
	m_flVoteEndTime = gpGlobals->time + mp_votetimecheck.value;
	m_flNextVoteTime = m_flVoteEndTime + mp_votetimebetween.value;
	m_iVoteType = type;
	player->VoteStarted( type );
}

bool CVoteManager::ClientCommand( CBasePlayer *player, const char *command )
{
	if ( !stricmp( command, "vote" ) )
	{
		if ( CVAR_GET_FLOAT( "mp_voteallow" ) == 0.0f )
			ClientPrint( player->pev, HUD_PRINTCENTER, "Voting is not allowed on this server." );
		else
			ClientPrint( player->pev, HUD_PRINTCONSOLE, "Usage:\nvotekill [playername]\nvotekick [playername]\nvoteban [playername]\nvotemap [mapname]\nvoteclassicmode\nvotesurvivalmode\n" );

		return true;
	}

	VOTETYPE type = VOTE_NONE;
	const char *usage = NULL;

	if ( !strcmp( command, "votekill" ) )
	{
		type = VOTE_KILL;
		usage = "Usage: votekill [playername]";
	}
	else if ( !strcmp( command, "votekick" ) )
	{
		type = VOTE_KICK;
		usage = "Usage: votekick [playername]";
	}
	else if ( !strcmp( command, "voteban" ) )
	{
		type = VOTE_BAN;
		usage = "Usage: voteban [playername]";
	}
	else if ( FStrEq( command, "votemap" ) )
	{
		type = VOTE_MAP;
		usage = "Usage: votemap [mapname]";
	}
	else if ( FStrEq( command, "voteclassicmode" ) )
		type = VOTE_CLASSICMODE;
	else if ( FStrEq( command, "votesurvivalmode" ) )
		type = VOTE_SURVIVALMODE;

	if ( type != VOTE_NONE )
	{
		if ( CVAR_GET_FLOAT( "mp_voteallow" ) == 0.0f )
			ClientPrint( player->pev, HUD_PRINTCENTER, "Voting is not allowed on this server." );
		else if ( type == VOTE_SURVIVALMODE && CVAR_GET_FLOAT( "mp_survival_voteallow" ) == 0.0f )
			ClientPrint( player->pev, HUD_PRINTCENTER, "Survival mode voting not allowed" );
		else if ( !usage || CMD_ARGC() > 1 )
			StartVote( type, player, usage ? CMD_ARGV( 1 ) : "" );
		else
			ClientPrint( player->pev, HUD_PRINTCONSOLE, usage );

		return true;
	}

	if ( FStrEq( command, "voteyes" ) )
		PlayerVoted( player, VOTE_YES );
	else if ( FStrEq( command, "voteno" ) )
		PlayerVoted( player, VOTE_NO );
	else if ( FStrEq( command, "print_voteinfo" ) )
	{
		float remaining = m_flVoteEndTime - gpGlobals->time;

		ClientPrint( player->pev, HUD_PRINTCONSOLE, UTIL_VarArgs( "Vote info:\n Active %s\nSeconds Left %f\n Yes: %d No: %d\n", FActiveVote() ? "Yes" : "No", remaining > 0.0f ? remaining : 0.0f, m_iVotesYes, m_iVotesNo ) );
	}
	else
		return false;

	return true;
}
