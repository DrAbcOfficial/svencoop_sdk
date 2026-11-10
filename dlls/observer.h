#ifndef OBSERVER_H
#define OBSERVER_H

#include "TypedEntityHandle.h"

class CBasePlayer;

class CObserver
{
public:
	CObserver( CBasePlayer *player );
	~CObserver();

	CBasePlayer *GetObserverTarget( void );
	void SetObserverTarget( CBasePlayer *player );
	void ResetObserverTarget( void );
	bool IsObserver( void );
	bool HasCorpse( void );
	CBaseEntity *GetDeadBody( void ) { return m_hDeadBody; }
	bool CanControlObserverMode( void );
	void SetObserverModeControlEnabled( bool enabled );
	bool CanUseObserver( void );
	bool IsObserverCyclic( void );
	static void SetObserverCyclic( bool enabled );

	void StartObserver( const Vector &position, const Vector &angles, bool createBody = true );
	void StopObserver( bool respawn );
	void FindNextPlayer( bool reverse );
	void SetMode( int mode );
	CBaseEntity *FindObserverStartPoint( void );
	void HandleButtons( void );
	void CheckTarget( void );
	void CheckProperties( void );
	void RemoveDeadBody( void );
	void Think( void );

private:
	CEntityHandle<CBasePlayer> m_hPlayer;
	CEntityHandle<CBasePlayer> m_hObserverTarget;
	float                     m_flNextObserverInput;
	CEntityHandle<CBaseEntity> m_hDeadBody;
	bool                      m_fCanControlObserverMode;

	int                       m_iObserverWeapon;
	int                       m_iObserverLastMode;
};

#endif
