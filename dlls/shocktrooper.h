#ifndef SHOCKTROOPER_H
#define SHOCKTROOPER_H

class CShocktrooper : public CSquadMonster
{
public:
	void Spawn( void );
	void Precache( void );
	int  Classify( void );
	int  ISoundMask( void );
	int  ObjectCaps( void );
	float GetPointsForDamage( float damage );
	void Revive( void );
	void SetYawSpeed( void );
	BOOL FCanCheckAttacks( void );
	BOOL CheckMeleeAttack1( float dot, float distance );
	BOOL CheckRangeAttack1( float dot, float distance );
	BOOL CheckRangeAttack2( float dot, float distance );
	void CheckAmmo( void );
	void PrescheduleThink( void );
	void RunAI( void );
	void StartTask( Task_t *task );
	void RunTask( Task_t *task );
	void SetActivity( Activity activity );
	void HandleAnimEvent( MonsterEvent_t *event );
	Vector GetGunPosition( void );
	void Shoot( void );
	CBaseEntity *Kick( void );
	Schedule_t *GetSchedule( void );
	Schedule_t *GetScheduleOfType( int type );
	int IRelationship( CBaseEntity *target, bool ignoreNoTarget = false );
	int TakeDamage( entvars_t *inflictor, entvars_t *attacker, float damage, int damageType );
	void TraceAttack( entvars_t *attacker, float damage, Vector direction, TraceResult *trace, int damageType );
	void DeathSound( void );
	void GibMonster( void );
	void PainSound( void );
	void IdleSound( void );
	void AlertSound( void );
	void UseSound( void );
	void UnUseSound( void );
	BOOL FOkToSpeak( void );
	void JustSpoke( void );
	void SpeakSentence( void );
	int Save( CSave &save );
	int Restore( CRestore &restore );

	CUSTOM_SCHEDULES;
	static TYPEDESCRIPTION m_SaveData[];

private:
	void SpawnRandomGibs( entvars_t *pevVictim, int cGibs, int );

	float  m_flNextGrenadeCheck;
	float  m_flNextPainTime;
	float  m_flHealthScale;
	float  m_flNextAmmoRecharge;
	float  m_flMuzzleSprite;

	Vector m_vecTossVelocity;

	bool   m_fThrowGrenade;
	bool   m_fStanding;
	bool   m_fFirstEncounter;
	bool   m_fDroppedShockroach;

	int    m_cClipSize;
	int    m_voicePitch;
	int    m_iSentence;
};

#endif
