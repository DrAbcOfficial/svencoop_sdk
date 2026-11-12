#ifndef SHOCKROACH_H
#define SHOCKROACH_H

class CShockRoach : public CBaseMonster
{
public:
	void Spawn( void );
	void Precache( void );
	int ObjectCaps( void );
	int Classify( void );
	Vector Center( void );
	Vector BodyTarget( const Vector &source );
	float GetPointsForDamage( float damage );
	float GetDamageAmount( void );
	void SetYawSpeed( void );
	void HandleAnimEvent( MonsterEvent_t *event );
	void PrescheduleThink( void );
	void StartTask( Task_t *task );
	void RunTask( Task_t *task );
	void ChangeSchedule( Schedule_t *schedule );
	Schedule_t *GetScheduleOfType( int type );
	BOOL CheckRangeAttack1( float dot, float distance );
	BOOL CheckRangeAttack2( float dot, float distance );
	void Killed( entvars_t *attacker, int gib );
	void Detonate( entvars_t *attacker, int gib, CBaseEntity *other );
	void EXPORT LeapTouch( CBaseEntity *other );
	void EXPORT DetonateTouch( CBaseEntity *other );
	void EXPORT PickUp( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE type, float value );
	void IdleSound( void );
	void AlertSound( void );
	void PainSound( void );
	void DeathSound( void );

	CUSTOM_SCHEDULES;

	float m_flExpireTime;
	bool  m_fExplosiveLeap;
	bool  m_fDetonated;
};

#endif
