#ifndef GONOME_H
#define GONOME_H

class CGonomeSpit : public CBaseEntity
{
public:
	void Spawn( void );
	void Touch( CBaseEntity *other );
	void EXPORT Animate( void );
	static void Shoot( entvars_t *owner, Vector start, Vector velocity );
	int Save( CSave &save );
	int Restore( CRestore &restore );

	static TYPEDESCRIPTION m_SaveData[];

	int m_maxFrame;
};

class CGonome : public CBaseMonster
{
public:
	void AttackSound( void ) {}
	void CheckAndApplyGenericAttacks( void ) { CBaseMonster::CheckAndApplyGenericAttacks(); }
	void Spawn( void );
	void Precache( void );
	int ObjectCaps( void );
	int Classify( void );
	void SetYawSpeed( void );
	int ISoundMask( void );
	void HandleAnimEvent( MonsterEvent_t *event );
	void RunAI( void );
	BOOL CheckRangeAttack1( float dot, float distance );
	BOOL CheckRangeAttack1_Move( float dot, float distance );
	BOOL CheckMeleeAttack1( float dot, float distance );
	BOOL CheckMeleeAttack1_Move( float dot, float distance );
	BOOL CheckMeleeAttack2( float dot, float distance );
	BOOL CheckMeleeAttack2_Move( float dot, float distance );
	void StartTask( Task_t *task );
	void RunTask( Task_t *task );
	Schedule_t *GetSchedule( void );
	Schedule_t *GetScheduleOfType( int type );
	void SetActivity( Activity activity );
	int IgnoreConditions( void );
	BOOL FValidateHintType( short type );
	int TakeDamage( entvars_t *inflictor, entvars_t *attacker, float damage, int damageType );
	float GetPointsForDamage( float damage );
	void IdleSound( void );
	void PainSound( void );
	void DeathSound( void );
	void AlertSound( void );
	void UseSound( void );
	void UnUseSound( void );
	int Save( CSave &save );
	int Restore( CRestore &restore );

	CUSTOM_SCHEDULES;
	static TYPEDESCRIPTION m_SaveData[];

	bool  m_fCanThreatDisplay;
	float m_flLastHurtTime;
	float m_flNextSpitTime;
	float m_flSpitDelayTime;
};

class CGonomeDead : public CBaseMonster
{
public:
	void Spawn( void );
	void Precache( void );
	void KeyValue( KeyValueData *data );

	int m_iPose;
};

#endif
