#ifndef VOLTIGORE_H
#define VOLTIGORE_H

class CVoltigore : public CSquadMonster
{
public:
	void Spawn( void );
	void Precache( void );
	int Save( CSave &save );
	int Restore( CRestore &restore );

	static TYPEDESCRIPTION m_SaveData[];

	int ObjectCaps( void );
	int Classify( void );
	int IRelationship( CBaseEntity *pTarget, bool fIgnoreNoTarget = false );
	float GetPointsForDamage( float flDamage );
	void SetObjectCollisionBox( void );
	void SetYawSpeed( void );
	int ISoundMask( void );
	BOOL FValidateHintType( short sHint );
	int IgnoreConditions( void );
	void RunAI( void );
	void SetActivity( Activity activity );
	void HandleAnimEvent( MonsterEvent_t *pEvent );
	void StartTask( Task_t *pTask );
	BOOL CheckMeleeAttack1( float flDot, float flDist );
	BOOL CheckMeleeAttack2( float flDot, float flDist );
	BOOL CheckRangeAttack1( float flDot, float flDist );

	int TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );
	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType );
	void IdleSound( void );
	void PainSound( void );
	void AlertSound( void );
	void DeathSound( void );
	void UseSound( void );
	void UnUseSound( void );

	Schedule_t *GetSchedule( void );
	Schedule_t *GetScheduleOfType( int type );
	CUSTOM_SCHEDULES;

	Activity GetDeathActivity( void );
	void CallGibMonster( void );
	void GibMonster( void );
	void UpdateOnRemove( void );
	void Revive( void );
	void BodyBeams( bool fEnable );
	void CreateDeathBeams( void );
	void RemoveDeathBeams( void );
	void SpawnGibs( void );
	void EXPORT VoltigoreDeathThink( void );

private:
	float m_flLastHurtTime;
	float m_flNextSpitTime;

	// unsaved beam and death state
	bool    m_fDeathBeams;
	int     m_iSpitSprite;
	float   m_flDeathTime;
	EHANDLE m_hBodyBeams[4];
	EHANDLE m_hDeathBeams[10];
	EHANDLE m_hGibs[10];
	EHANDLE m_hGlow;
};

class CBabyVoltigore : public CSquadMonster
{
public:
	void Spawn( void );
	void Precache( void );
	int Save( CSave &save );
	int Restore( CRestore &restore );

	static TYPEDESCRIPTION m_SaveData[];

	int ObjectCaps( void );
	int Classify( void );
	int IRelationship( CBaseEntity *pTarget, bool fIgnoreNoTarget = false );
	Vector Center( void );
	Vector BodyTarget( const Vector &source );
	void SetObjectCollisionBox( void );
	void SetYawSpeed( void );
	int ISoundMask( void );
	BOOL FValidateHintType( short sHint );
	int IgnoreConditions( void );
	void RunAI( void );
	void SetActivity( Activity activity );
	void HandleAnimEvent( MonsterEvent_t *pEvent );
	void StartTask( Task_t *pTask );
	void RunTask( Task_t *pTask );
	BOOL CheckMeleeAttack1( float flDot, float flDist );
	BOOL CheckRangeAttack1( float flDot, float flDist );

	int TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );
	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType );
	void PainSound( void );
	void AlertSound( void );
	void DeathSound( void );
	void AttackSound( void );
	void UseSound( void );
	void UnUseSound( void );
	void UpdateSprite( void );

	MONSTERSTATE GetIdealState( void );
	Schedule_t *GetSchedule( void );
	Schedule_t *GetScheduleOfType( int type );
	CUSTOM_SCHEDULES;

private:
	bool  m_fCanThreatDisplay;
	float m_flLastHurtTime;
	float m_flNextSpitTime;

	int m_iSpriteBrightness; // range scheduling writes 255
};

#endif
