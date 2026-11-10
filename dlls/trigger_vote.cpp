#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "trigger_vote.h"
#include "CServer.h"
#include "CVoteManager.h"

LINK_ENTITY_TO_CLASS( trigger_vote, CTriggerVote );

CTriggerVote::CTriggerVote()
    : m_iszYesString( 0 ), m_iszNoString( 0 )
{
}

void CTriggerVote::KeyValue( KeyValueData *data )
{
	if ( !stricmp( data->szKeyName, "m_iszYesString" ) )
	{
		m_iszYesString = AllocStringLocal( data->szValue );
		data->fHandled = TRUE;
	}
	else if ( !stricmp( data->szKeyName, "m_iszNoString" ) )
	{
		m_iszNoString = AllocStringLocal( data->szValue );
		data->fHandled = TRUE;
	}
	else
		CBaseDelay::KeyValue( data );
}

void CTriggerVote::Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value )
{
	Vote();
}

void CTriggerVote::OnDestroy( void )
{
	CServer::GetInstance()->GetVoteManager().ClearCustomVote( this );
}

void CTriggerVote::Vote( void )
{
	SetThink( NULL );

	CServer::GetInstance()->GetVoteManager().StartCustomVote( this );
}

void CTriggerVote::VoteBlocked( float time )
{
	SetThink( &CTriggerVote::Vote );
	pev->nextthink = time;
}

void CTriggerVote::VoteEnd( bool result, int voters )
{
	string_t target;

	if ( voters == 0 )
		target = pev->noise;
	else
		target = result ? pev->target : pev->netname;

	FireTargets( STRING( target ), this, this, USE_ON, 0 );
}

const char *CTriggerVote::GetName() const
{
	return STRING( pev->targetname );
}

float CTriggerVote::GetVoteLength() const
{
	return pev->frags;
}

const char *CTriggerVote::GetVoteMessage() const
{
	return STRING( pev->message );
}

float CTriggerVote::GetPercentageNeeded() const
{
	return pev->health;
}

const char *CTriggerVote::GetYesText() const
{
	return m_iszYesString ? STRING( m_iszYesString ) : "Yes";
}

const char *CTriggerVote::GetNoText() const
{
	return m_iszNoString ? STRING( m_iszNoString ) : "No";
}

void CTriggerVote::AddRef() const
{
}

void CTriggerVote::Release() const
{
}
