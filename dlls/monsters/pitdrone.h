#ifndef PITDRONE_H
#define PITDRONE_H

class CSpike : public CBaseEntity
{
public:
	void Spawn( void );
	void Precache( void );
	void Touch( CBaseEntity *other );
	void EXPORT IgniteThink( void );
	void EXPORT FlySpikeThink( void );
	static void Shoot( entvars_t *owner, Vector start, Vector velocity );
};

class CPitdrone : public CSquadMonster
{
public:
	CPitdrone();

	void Spawn( void );
	void Precache( void );
	void KeyValue( KeyValueData *data );
	int ObjectCaps( void );
	int Classify( void );
	int ISoundMask( void );
	void SetYawSpeed( void );
	void SetActivity( Activity activity );
	void HandleAnimEvent( MonsterEvent_t *event );
	void IdleSound( void );
	void PainSound( void );
	void DeathSound( void );
	void AlertSound( void );
	void UseSound( void );
	void UnUseSound( void );
	void RunAI( void );
	void StartTask( Task_t *task );
	void RunTask( Task_t *task );
	BOOL CheckMeleeAttack1( float dot, float distance );
	BOOL CheckMeleeAttack2( float dot, float distance );
	BOOL CheckRangeAttack1( float dot, float distance );
	BOOL FValidateHintType( short hint );
	int IRelationship( CBaseEntity *target, bool ignoreNoTarget = false );
	int IgnoreConditions( void );
	MONSTERSTATE GetIdealState( void );
	Schedule_t *GetSchedule( void );
	Schedule_t *GetScheduleOfType( int type );
	void TraceAttack( entvars_t *attacker, float damage, Vector direction, TraceResult *trace, int damageType );
	int TakeDamage( entvars_t *inflictor, entvars_t *attacker, float damage, int damageType );
	float GetPointsForDamage( float damage );
	void GibMonster( void );
	void SpawnRandomGibs( entvars_t *victim, int count, int human );
	void UpdateSpikeBodygroup( void );
	int Save( CSave &save );
	int Restore( CRestore &restore );

	CUSTOM_SCHEDULES;
	static TYPEDESCRIPTION m_SaveData[];

private:
	float m_flAmmo;
	float m_flLastHurtTime;
	float m_flNextSpitTime;
	float m_flNextSpitCheck;
};

#endif
