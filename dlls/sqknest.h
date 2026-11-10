#ifndef SQKNEST_H
#define SQKNEST_H

class CSqkNest : public CBaseMonster
{
public:
	CSqkNest() : m_vecSpawnAngles( 0, 0, 0 )
	{
	}

	void Spawn( void );
	void Precache( void );
	int Classify( void );
	int ObjectCaps( void );
	float GetPointsForDamage( float damage );
	void SetObjectCollisionBox( void );
	void HandleAnimEvent( MonsterEvent_t *event );
	BOOL CheckRangeAttack1( float dot, float distance );
	void Killed( entvars_t *attacker, int gib );
	void MonsterThink( void );
	void StartTask( Task_t *task );
	void RunTask( Task_t *task );
	Schedule_t *GetScheduleOfType( int type );
	void EXPORT Burst( void );
	void EXPORT SpawnSnark( void );
	void EXPORT ThrowTouch( CBaseEntity *other );
	void EXPORT BlindWalkBlocked( CBaseEntity *other );
	void StartBurst( void );
	void ThrownWakeUp( void );
	void NotifySnarkCount( int change );
	CUSTOM_SCHEDULES;

	static const char *pStepSounds[];

	int     m_iGibModelIndex;
	int     m_iSnarkCount;
	Vector  m_vecSpawnAngles;
	EHANDLE m_hOwner;
};

#endif
