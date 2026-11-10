#ifndef OP4TORCH_H
#define OP4TORCH_H

#include "squadtalkmonster.h"

class CHGruntOp4Torch : public CSquadTalkMonster
{
public:
	void KeyValue( KeyValueData *pkvd );
	void TalkInit( void );
	void Spawn( void );
	void Precache( void );
	void SetupFriendly( void );
	void SetPlayerAlly( bool state );
	int Classify( void );
	int ObjectCaps( void );
	int ISoundMask( void );
	int IRelationship( CBaseEntity *target, bool ignoreNoTarget = false );
	float GetPointsForDamage( float damage );
	void RunAI( void );
	void PrescheduleThink( void );
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
	void AlertSound( void );
	void DeclineFollowing( void );
	void GibMonster( void );
	int TakeDamage( entvars_t *inflictor, entvars_t *attacker, float damage, int damageType );
	void TraceAttack( entvars_t *attacker, float damage, Vector direction, TraceResult *trace, int damageType );
	void EXPORT CanisterExplode( void );
	int Save( CSave &save );
	int Restore( CRestore &restore );
	static TYPEDESCRIPTION m_SaveData[];

private:
	CBaseEntity *Kick( void );
	void Shoot( void );
	void DropWeapon( void );
	void ToggleTorch( int active );
	BOOL FOkToSpeak( void );
	void JustSpoke( void );
	void SpeakSentence( void );

	static Schedule_t *m_scheduleList[];

	float  m_flNextGrenadeCheck;
	float  m_flNextPainTime;
	Vector m_vecTossVelocity;

	bool m_fThrowGrenade;
	bool m_fStanding;
	bool m_fFirstEncounter;
	int  m_cClipSize;
	int  m_iBrassShell;
	int  m_iPlayerDamage;
	int  m_iSentence;

	bool    m_fGunHolstered;
	bool    m_fTorchActive;
	EHANDLE m_hTorchBeam;
	bool    m_fCanisterExploding;

	Vector      m_vecTorchAngles;
	Vector      m_vecTorchSource;
	Vector      m_vecTorchEnd;
	TraceResult m_TorchTrace;
};

#endif
