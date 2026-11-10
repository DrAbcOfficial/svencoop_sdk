#ifndef PATHENTITIES_H
#define PATHENTITIES_H

class CPathWaypoint : public CBaseEntity
{
public:
	void Spawn( void );
	void KeyValue( KeyValueData *data );
	int ObjectCaps( void ) { return 0; }
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );
	void EXPORT UpdateOccupantCount( void );
	CPathWaypoint *GetNextWaypoint( void );

	bool		m_fAlternate;
	string_t	m_iszAlternateTarget;
	int			m_iMovementType;
	int			m_iRadius;
	int			m_iUseAngles;

	string_t	m_iszTriggerOnArrival;
	string_t	m_iszArrivalAnimation;
	string_t	m_iszTriggerAfterArrival;

	int			m_iWaitActivity;
	string_t	m_iszWaitAnimation;
	int			m_iWaitTime;
	string_t	m_iszWaitMaster;
	string_t	m_iszDepartureAnimation;
	string_t	m_iszTriggerOnDeparture;

	int			m_iOccupantRadius;
	int			m_iOccupantLimit;
	int			m_iOccupantCount;
	bool		m_fWaitUntilFull;
	string_t	m_iszOverflowWaypoint;
	bool		m_fForceComplete;

	string_t	m_iszStopTrigger;
	int			m_iRestartDelay;
};

#endif
