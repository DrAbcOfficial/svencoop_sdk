#ifndef CVOTEMANAGER_H
#define CVOTEMANAGER_H

#include "VoteDefs.h"
#include "TypedEntityHandle.h"

class CBaseVote;
class CBasePlayer;

class CVoteManager
{
public:
	CVoteManager();
	~CVoteManager();

	const char *GetVoteActionName( VOTEINPUT input ) const;
	const char *GetVoteTypeName( VOTETYPE type ) const;
	float PercentageNeeded( VOTETYPE type ) const;
	void InitVoteVars();
	void MapInit();
	bool FActiveVote() const;
	bool CanStartNewVote( CBasePlayer *player, VOTETYPE type ) const;
	float TimeVoteEnds() const;
	void SetVotedMap( const char *name );
	void SetVotedPlayer( CBasePlayer *player );
	void ClearCustomVote( CBaseVote *vote );
	void PlayerVoted( CBasePlayer *player, VOTEINPUT input );
	bool PerformVoteAction( VOTETYPE type );
	void EndVote();
	void Think();
	void ShowVoteMenu( CBasePlayer *player, const char *text, VOTETYPE type, const char *yes, const char *no );
	void ShowVoteMenu_All( const char *text, VOTETYPE type, const char *yes, const char *no );
	void StartCustomVote( CBaseVote *vote );
	bool IsMapInVoteList( const char *name ) const;
	bool IsMapAllowedForVote( const char *name ) const;
	bool AddMapToVoteList( const char *name );
	int CountMapsInVoteList() const;
	const char *GetMapInVoteList( unsigned int index ) const;
	bool RemoveMapFromVoteList( const char *name );
	void FreeMapVoteList();
	void StartVote( VOTETYPE type, CBaseEntity *initiator, const char *argument );
	bool ClientCommand( CBasePlayer *player, const char *command );

private:
	CBasePlayer *VotedPlayer();

	struct MapVote
	{
		char     name[64];
		MapVote *next;
	};

	float m_flNextVoteTime;
	float m_flVoteEndTime;

	int      m_iVotesNo;
	int      m_iVotesYes;
	int      m_iVoters;
	VOTETYPE m_iVoteType;

	CEntityHandle<CBasePlayer> m_hVotedPlayer;
	char                      m_szVotedAuthId[44];
	char                      m_szVotedPlayerName[32];
	char                      m_szVotedMap[64];

	CBaseVote *m_pCustomVote;
	MapVote    m_MapVoteList;
};

#endif
