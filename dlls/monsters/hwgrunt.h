#ifndef HWGRUNT_H
#define HWGRUNT_H

#include "ai/squadmonster.h"

namespace HWGRUNT
{
enum ShootingStates
{
	SHOOTING_IDLE = 0,
	SHOOTING_SPINUP,
	SHOOTING_FIRING,
	SHOOTING_SUPPRESSING,
	SHOOTING_SPINDOWN
};
}

class CHWGrunt : public CSquadMonster
{
public:
	void OnCreate( void );
	void Spawn( void );
	void Precache( void );
	void KeyValue( KeyValueData *data );
	void SetupFriendly( void );
	int Classify( void );
	int ObjectCaps( void );
	int ISoundMask( void );
	float GetPointsForDamage( float damage );
	void RunAI( void );
	void CheckAmmo( void );
	BOOL CheckRangeAttack1( float dot, float distance );
	bool CheckMinigunAttack( CBaseEntity *enemy );
	CBaseEntity *BestVisibleEnemySpecial( void );
	CBaseEntity *FindWeapon( void );
	void SetArmedWeapon( int weapon );
	void SetFiringState( HWGRUNT::ShootingStates state );
	bool FacingShootingIdeal( void );
	void Shoot( Vector target );
	void ExecuteShootTask( void );
	void HandleAnimEvent( MonsterEvent_t *event );
	void SetActivity( Activity activity );
	void SetYawSpeed( void );
	int IgnoreConditions( void );
	void StartTask( Task_t *task );
	void RunTask( Task_t *task );
	Schedule_t *GetSchedule( void );
	Schedule_t *GetScheduleOfType( int type );
	Schedule_t *ScheduleFromName( const char *name );
	void JustSpoke( void );
	bool FOkToSpeak( void );
	void SpeakSentence( void );
	void IdleSound( void );
	void AlertSound( void );
	void AttackSound( void );
	void PainSound( void );
	void DeathSound( void );
	void UseSound( void );
	void UnUseSound( void );
	void GibMonster( void );
	void Killed( entvars_t *attacker, int gib );
	int TakeDamage( entvars_t *inflictor, entvars_t *attacker, float damage, int damageType );
	void TraceAttack( entvars_t *attacker, float damage, Vector direction, TraceResult *trace, int damageType );

private:
	void UpdateGunAngles( const Vector &target );
	void SetShootEnemy( CBaseEntity *enemy );
	bool UpdateShootTarget( bool grounded, CBaseEntity *&enemy, bool canFire );
	void SpinDown( const char *sequence );
	void DropMinigun( float angleDirection );
	bool CheckSidearmAttackFrom( const Vector &source );

	static Schedule_t *m_scheduleList[];

	int m_iBrassShell;
	int m_iQuestion;
	int m_voicePitch;
	int m_iSentence;

	float m_flNextSpinAnimation;
	float m_flSidearmCooldown;
	float m_flSpawnAttackTime;
	float m_flNextShot;

	EHANDLE m_hMinigun;
	Vector	m_vecSuppressionTarget;
	Vector	m_vecGunReference;

	int						m_iArmedWeapon;
	int						m_iUnknown694;
	int						m_iSidearm;
	HWGRUNT::ShootingStates	m_ShootingState;
	int						m_iAmmo;
	int						m_iClipSize;

	Vector	m_vecGunOffset;
	Vector	m_vecGunAngles;
	float	m_flNextFlinch;

	bool m_fStanding;
	bool m_fHasPistolSequence;
	bool m_fDisableMinigunDrop;
	bool m_fPickupAnimationDone;
};

#endif
