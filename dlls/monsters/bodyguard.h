#ifndef BODYGUARD_H
#define BODYGUARD_H

#include "ai/talkmonster.h"

class CBodyGuard : public CTalkMonster
{
public:
	void Spawn( void );
	void Precache( void );
	void SetupFriendly( void );
	int Classify( void );
	int ObjectCaps( void );
	int ISoundMask( void );
	void SetPlayerAlly( bool state );
	float GetPointsForDamage( float damage );
	void TalkInit( void );
	void DeclineFollowing( void );
	void AlertSound( void );
	void PainSound( void );
	void DeathSound( void );
	void CheckAmmo( void );
	BOOL FCanCheckAttacks( void );
	BOOL CheckRangeAttack1( float dot, float distance );
	void SetActivity( Activity activity );
	void SetGaitActivity( Activity activity );
	void SetYawSpeed( void );
	float ChangeYawNoDelay( int yawSpeed );
	bool FacingShootingIdeal( void );
	void HandleAnimEvent( MonsterEvent_t *event );
	void FirePistol( void );
	void FireDeagle( void );
	void FireMp5( void );
	void FireShotgun( void );
	void FireUziAkimbo( void );
	void FireSniperRifle( void );
	void FireMinigun( void );
	void StartTask( Task_t *task );
	void RunTask( Task_t *task );
	CBaseEntity *BestVisibleEnemySpecial( void );
	Schedule_t *GetSchedule( void );
	Schedule_t *GetScheduleOfType( int type );
	void TraceAttack( entvars_t *attacker, float damage, Vector direction, TraceResult *trace, int damageType );
	int TakeDamage( entvars_t *inflictor, entvars_t *attacker, float damage, int damageType );
	void Killed( entvars_t *attacker, int gib );
	int Save( CSave &save );
	int Restore( CRestore &restore );
	static TYPEDESCRIPTION m_SaveData[];
	CUSTOM_SCHEDULES;

private:
	float	m_painTime;
	int		m_iShell;
	int		m_iShotgunShell;
	int		m_cClipSize;

	bool	m_fMinigunStarted;
	float	m_flNextSpinSound;
	float	m_flNextMinigunAttack;
	float	m_flSpinupEnd;
};

#endif
