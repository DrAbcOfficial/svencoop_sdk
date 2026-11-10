#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "game.h"
#include "server_support.h"

void CBasePlayer::VoteStarted( VOTETYPE type )
{
	// Keep this SSE operand order for when both inputs are NaNs
	volatile float deadline = mp_playervotedelay.value;

	deadline += gpGlobals->time;
	m_flNextVoteTime = deadline;
}

bool CBasePlayer::CanStartNextVote( VOTETYPE type )
{
	return TimeToStartNextVote() <= 0.0f;
}

void CBasePlayer::Vote( VOTEINPUT input, const char *text )
{
	if ( HasVoted() )
		return;

	m_iLastVoteInput = input;
	ClientPrint( pev, HUD_PRINTCENTER, "Voted %s", text );
	UTIL_LogPrintf( "\"%s\" voted \"%s\"\n", PlayerLog( edict() ), text );
	m_bHasVoted = true;
}

bool CBasePlayer::HasVoted()
{
	return m_bHasVoted;
}

void CBasePlayer::ResetVote()
{
	m_iLastVoteInput = VOTE_NO;
	m_bHasVoted = false;
}

VOTEINPUT CBasePlayer::LastVoteInput()
{
	return m_iLastVoteInput;
}

void CBasePlayer::InitVote()
{
	ResetVote();
	m_flNextVoteTime = 0.0f;
}

float CBasePlayer::TimeToStartNextVote()
{
	float remaining = m_flNextVoteTime - gpGlobals->time;

	return 0.0f > remaining ? 0.0f : remaining;
}
