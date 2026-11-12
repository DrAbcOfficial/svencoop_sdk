#ifndef ROBOTGRUNT_H
#define ROBOTGRUNT_H

#include "ai/squadmonster.h"

class CRobotGrunt : public CSquadMonster
{
public:
	void KeyValue( KeyValueData *pkvd );

	string_t m_iszTargetOnDeath;

	void Spawn( void );
	void Precache( void );
	void SetupFriendly( void );
	int Classify( void );
	int ObjectCaps( void );
	int ISoundMask( void );
	int IRelationship( CBaseEntity *target, bool ignoreNoTarget = false );
	float GetPointsForDamage( float damage );
	void RunAI( void );
	void PrescheduleThink( void );
	CBaseEntity *BestVisibleEnemy( void );
	BOOL FCanCheckAttacks( void );
	BOOL CheckRangeAttack1( float dot, float distance );
	BOOL CheckRangeAttack2( float dot, float distance );
	BOOL CheckMeleeAttack1( float dot, float distance );
	void CheckAmmo( void );
	Vector GetGunPosition( void );
	void HandleAnimEvent( MonsterEvent_t *event );
	void SetActivity( Activity activity );
	void SetYawSpeed( void );
	void StartTask( Task_t *task );
	void RunTask( Task_t *task );
	Schedule_t *GetSchedule( void );
	Schedule_t *GetScheduleOfType( int type );
	Schedule_t *ScheduleFromName( const char *name );
	void IdleSound( void );
	void PainSound( void );
	void DeathSound( void );
	void UseSound( void );
	void UnUseSound( void );
	void GibMonster( void );
	void Killed( entvars_t *attacker, int gib );
	int TakeDamage( entvars_t *inflictor, entvars_t *attacker, float damage, int damageType );
	void TraceAttack( entvars_t *attacker, float damage, Vector direction, TraceResult *trace, int damageType );
	void EXPORT ShockTouch( CBaseEntity *other );
	void EXPORT RobotDeathThink( void );

private:
	void Shoot( void );
	void Shotgun( void );
	void Rocket( void );
	void Sniper( void );
	BOOL CheckRangeAttack1Rocket( float distance, CBaseEntity *enemy );
	void GlowEffect( int active );
	void UpdateGlow( void );
	void UpdateSpot( bool create );
	void KillLaserSpot( void );
	BOOL FOkToSpeak( void );
	void JustSpoke( void );
	void SpeakSentence( void );
	void BreakRobot( const Vector &position, const Vector &sparkPosition );

	static Schedule_t *m_scheduleList[];

	float  m_flNextGrenadeCheck;
	float  m_flNextPainTime;
	Vector m_vecTossVelocity;

	bool m_fThrowGrenade;
	bool m_fStanding;
	bool m_fFirstEncounter;
	int  m_cClipSize;
	int  m_voicePitch;

	int m_iBrassShell;
	int m_iShotgunShell;
	int m_iChromeGibs;
	int m_iComputerGibs;

	float m_flDeathTime;
	float m_flSparkTime;
	float m_flShockEndTime;
	float m_flNextGlow;
	int   m_iSentence;
	float m_flRPGSafetyRadius;
	bool  m_fShockActive;
	bool  m_fHasM16;

	EHANDLE m_hRPGEntity;
	EHANDLE m_hRPGLaserSpot;
	Vector  m_vecLaserAngles;
	Vector  m_vecLaserTargetAngles;
};

#endif
