#ifndef STUKABAT_H
#define STUKABAT_H

class CStukabat : public CBaseMonster
{
public:
	CStukabat() : m_SaveVelocity( 0, 0, 0 )
	{
	}

	void Spawn( void );
	void Precache( void );
	int Classify( void );
	float GetPointsForDamage( float damage );
	void SetYawSpeed( void );
	void SetActivity( Activity activity );
	void HandleAnimEvent( MonsterEvent_t *event );
	BOOL CheckRangeAttack1( float dot, float distance );
	BOOL CheckRangeAttack2( float dot, float distance );
	BOOL CheckMeleeAttack1( float dot, float distance );
	Activity GetStoppedActivity( void );
	void Stop( void );
	void MonsterThink( void );
	void Move( float interval = 0.1f );
	void MoveExecute( CBaseEntity *target, const Vector &direction, float interval );
	BOOL ShouldAdvanceRoute( float waypointDistance );
	int CheckLocalMove( const Vector &start, const Vector &end, CBaseEntity *target,
	                    float *distance, Vector *endPosition = NULL, bool walkMoveCheck = false );
	void StartTask( Task_t *task );
	void RunTask( Task_t *task );
	Schedule_t *GetSchedule( void );
	Schedule_t *GetScheduleOfType( int type );
	void EXPORT PainTouch( CBaseEntity *other );
	void SetActiveMode( int mode );
	float CeilingPos( const Vector &position );
	float FloorZ( const Vector &position );
	bool FindPerchPosition( Vector position, Vector &perch );
	bool FindPerchNode( float distance, Vector &perch );
	bool ProbeZ( const Vector &position, const Vector &probe, float *fraction );
	Vector DoProbe( const Vector &probe );
	void Fly( void );
	CUSTOM_SCHEDULES;

	Vector      m_SaveVelocity;
	float       m_stopTime;
	int         m_iActiveMode;
	float       m_flMaxSpeed;
	float       m_flLastTouch;
	bool        m_bTouched;
	const char *m_pFlapSound;
	float       m_flightSpeed;
};

#endif
