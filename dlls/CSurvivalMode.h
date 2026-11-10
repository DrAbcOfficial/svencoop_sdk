#ifndef CSURVIVALMODE_H
#define CSURVIVALMODE_H

#include "CASServerScriptGlobal.h"
#include "CBaseVote.h"
#include "CString.h"

class CSurvivalEndVote : public CBaseVote
{
public:
	void Start( void );
	virtual void VoteBlocked( float time );
	virtual void VoteEnd( bool result, int voters );
	virtual const char *GetName( void ) const;
	virtual float GetVoteLength( void ) const;
	virtual const char *GetVoteMessage( void ) const;
	virtual float GetPercentageNeeded( void ) const;
	virtual const char *GetYesText( void ) const;
	virtual const char *GetNoText( void ) const;
	virtual void AddRef( void ) const;
	virtual void Release( void ) const;
};

class CSurvivalMode : public CASServerScriptGlobal<CSurvivalMode>
{
public:
	CSurvivalMode();
	virtual ~CSurvivalMode();

	virtual void Initialize( void );
	virtual void Shutdown( void );
	virtual void Think( void );
	virtual void PreMapInit( void );
	virtual void PostMapInit( bool success );
	virtual void PostMapActivate( bool success );

	bool IsEnabled( void ) const;
	void Enable( bool activateNow = false );
	void Disable( void );
	void Toggle( void );
	bool IsActive( void ) const;
	void Activate( bool activateNow = false );
	int GetVoteState( void ) const;
	void VoteToggle( void );
	void ResetVoteState( void );
	void EnableMapSupport( void );
	bool MapSupportEnabled( void ) const;
	void SetStartOn( bool value );
	bool GetStartOn( void ) const;
	CString GetNextMap( void ) const;
	void SetNextMap( const CString &mapName );
	void EndRound( void );
	float GetDelayBeforeStart( void ) const;
	void SetDelayBeforeStart( float delay );
	int GetMinPlayersRequired( void ) const;
	void SetMinPlayersRequired( int count );
	bool HasInfiniteRetries( void ) const;
	int GetRetriesAllowed( void ) const;
	int GetRetriesLeft( void ) const;

	void ClientPutInServer( void );
	void ClientDisconnect( void );
	void PlayerKilled( void );
	void VoteBlocked( float time );
	void VoteEnd( bool result, int voters );

private:
	void ResetSettings( void );
	void Reset( void );
	void UpdateCheckpoints( bool active );
	int GetLivingPlayersCount( void ) const;
	void StartEndVote( void );
	const char *GetRestartMapName( void ) const;
	void PerformChangeLevel( const char *mapName );
	void ScheduleActivation( void );
	void ScheduleLevelChange( bool restart );
	bool RetriesDepleted( void ) const;
	void PreEndRoundCheck( void );
	void CheckEndConditions( void );
	void ActivateNow( void );
	bool CheckIfActivationNeeded( bool activateNow );
	void RetryUsed( void );
	void PreChangeLevelCheck( void );

	int   m_iVoteState;
	int   m_iState;
	bool  m_bEnabled;
	bool  m_bStartOn;
	bool  m_bRoundEnding;
	float m_flActivationTime;
	float m_flEndVoteDelay;
	float m_flChangeLevelDelay;
	float m_flUnknownTime;
	float m_flEndRoundCheckTime;
	float m_flEndVoteTime;
	float m_flCountdownTime;
	float m_flChangeLevelTime;

	char m_szPreviousMap[32 + 1];
	char m_szCurrentMap[32 + 1];
	char m_szLastNextMap[32 + 1];
	bool m_bRestartLevel;
	int  m_iRetriesLeft;

	CSurvivalEndVote m_EndVote;
};

#endif
