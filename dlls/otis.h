#ifndef OTIS_H
#define OTIS_H


#include "talkmonster.h"

class COtis : public CTalkMonster
{
public:
	void Spawn( void );
	void Precache( void );
	void KeyValue( KeyValueData *data );
	int Classify( void );
	int ISoundMask( void );
	int ObjectCaps( void );
	void SetPlayerAlly( bool state );
	void SetupFriendly( void );
	void SetYawSpeed( void );

	void CheckAmmo( void );
	BOOL CheckRangeAttack1( float dot, float distance );
	void OtisFirePistol( void );
	void HandleAnimEvent( MonsterEvent_t *event );
	void RunAI( void );
	void RunTask( Task_t *task );
	Schedule_t *GetSchedule( void );
	Schedule_t *GetScheduleOfType( int type );

	void TalkInit( void );
	void DeclineFollowing( void );
	void AlertSound( void );
	void PainSound( void );
	void DeathSound( void );

	int TakeDamage( entvars_t *inflictor, entvars_t *attacker, float damage, int damageType );
	void TraceAttack( entvars_t *attacker, float damage, Vector direction, TraceResult *trace, int damageType );
	void Killed( entvars_t *attacker, int gib );
	float GetPointsForDamage( float damage );

	int Save( CSave &save );
	int Restore( CRestore &restore );
	static TYPEDESCRIPTION m_SaveData[];

	bool  m_fGunDrawn;
	float m_painTime;
	int   m_head;
	int   m_iShell;
	int   m_cClipSize;

	CUSTOM_SCHEDULES;
};


#endif
