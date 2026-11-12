#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters/monsters.h"
#include "ai/animation.h"
#include "ai/PathEntities.h"
#include "ai/PathCondition.h"
#include "server/server_support.h"

LINK_ENTITY_TO_CLASS( path_waypoint, CPathWaypoint );

void CPathWaypoint::Spawn( void )
{
	if ( FStringNull( pev->targetname ) )
		ALERT( at_error, "Path_waypoint without a targetname!\n" );

	m_fAlternate = false;

	if ( m_iOccupantLimit > 0 )
	{
		SetThink( &CPathWaypoint::UpdateOccupantCount );
		pev->nextthink = gpGlobals->time + 0.1;
	}
}

void CPathWaypoint::KeyValue( KeyValueData *data )
{
	const char *key = data->szKeyName;
	int *integer = NULL;
	string_t *string = NULL;
	bool *boolean = NULL;

	if ( FStrEq( key, "alternate_target" ) )
		string = &m_iszAlternateTarget;
	else if ( FStrEq( key, "movementtype" ) )
		integer = &m_iMovementType;
	else if ( FStrEq( key, "radius" ) )
		integer = &m_iRadius;
	else if ( FStrEq( key, "useangles" ) )
		integer = &m_iUseAngles;
	else if ( FStrEq( key, "trigger_on_arrival" ) )
		string = &m_iszTriggerOnArrival;
	else if ( FStrEq( key, "arrival_animation" ) )
		string = &m_iszArrivalAnimation;
	else if ( FStrEq( key, "trigger_after_arrival" ) )
		string = &m_iszTriggerAfterArrival;
	else if ( FStrEq( key, "wait_activity" ) )
		integer = &m_iWaitActivity;
	else if ( FStrEq( key, "wait_animation" ) )
		string = &m_iszWaitAnimation;
	else if ( FStrEq( key, "wait_time" ) )
		integer = &m_iWaitTime;
	else if ( FStrEq( key, "wait_master" ) )
		string = &m_iszWaitMaster;
	else if ( FStrEq( key, "departure_animation" ) )
		string = &m_iszDepartureAnimation;
	else if ( FStrEq( key, "trigger_on_departure" ) )
		string = &m_iszTriggerOnDeparture;
	else if ( FStrEq( key, "occupant_radius_check" ) )
		integer = &m_iOccupantRadius;
	else if ( FStrEq( key, "occupant_limit" ) )
		integer = &m_iOccupantLimit;
	else if ( FStrEq( key, "waituntilfull" ) )
		boolean = &m_fWaitUntilFull;
	else if ( FStrEq( key, "overflow_waypoint" ) )
		string = &m_iszOverflowWaypoint;
	else if ( FStrEq( key, "force_complete" ) )
		boolean = &m_fForceComplete;
	else if ( FStrEq( key, "stop_trigger" ) )
		string = &m_iszStopTrigger;
	else if ( FStrEq( key, "restart_delay" ) )
		integer = &m_iRestartDelay;
	else
	{
		CBaseEntity::KeyValue( data );
		return;
	}

	if ( integer )
		*integer = strtol( data->szValue, NULL, 10 );
	else if ( string )
		*string = AllocStringLocal( data->szValue );
	else
		*boolean = strtol( data->szValue, NULL, 10 ) != 0;

	data->fHandled = TRUE;
}

void CPathWaypoint::Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value )
{
	if ( useType == USE_OFF )
		m_fAlternate = false;
	else if ( useType == USE_ON )
		m_fAlternate = true;
	else
		m_fAlternate = !m_fAlternate;
}

void CPathWaypoint::UpdateOccupantCount( void )
{
	if ( m_iOccupantLimit <= 0 )
	{
		SetThink( NULL );
		return;
	}

	m_iOccupantCount = 0;

	CBaseEntity *entity = NULL;

	while ( ( entity = UTIL_FindEntityInSphere( entity, pev->origin, (float) m_iOccupantRadius ) ) != NULL )
	{
		if ( ( entity->pev->flags & FL_MONSTER ) && entity->IsAlive() )
			m_iOccupantCount++;
	}

	pev->nextthink = gpGlobals->time + 1.0f;
}

CPathWaypoint *CPathWaypoint::GetNextWaypoint( void )
{
	string_t target = m_fAlternate ? m_iszAlternateTarget : pev->target;
	CBaseEntity *entity = NULL;

	while ( ( entity = UTIL_FindEntityByTargetname( entity, STRING( target ) ) ) != NULL )
	{
		if ( !FClassnameIs( entity->pev, "path_waypoint" ) )
			continue;

		CPathWaypoint *waypoint = dynamic_cast<CPathWaypoint *>( entity );

		if ( !waypoint )
			continue;

		if ( waypoint->m_iOccupantLimit > 0 && waypoint->m_iOccupantLimit <= waypoint->m_iOccupantCount && waypoint->m_iszOverflowWaypoint != target )
		{
			target = waypoint->m_iszOverflowWaypoint;
			entity = NULL;
			continue;
		}

		return waypoint;
	}

	return NULL;
}

CPathWaypoint *FindPathWaypoint( CBaseMonster *monster, string_t target )
{
	CBaseEntity *entity = NULL;

	while ( ( entity = UTIL_FindEntityByTargetname( entity, STRING( target ) ) ) != NULL )
	{
		if ( !FClassnameIs( entity->pev, "path_waypoint" ) )
			continue;

		CPathWaypoint *waypoint = dynamic_cast<CPathWaypoint *>( entity );

		if ( !waypoint )
			continue;

		if ( waypoint->m_iOccupantLimit > 0 && waypoint->m_iOccupantCount >= waypoint->m_iOccupantLimit && !FStringNull( waypoint->m_iszOverflowWaypoint ) && waypoint->m_iszOverflowWaypoint != target )
		{
			target = waypoint->m_iszOverflowWaypoint;
			entity = NULL;
			continue;
		}

		return waypoint;
	}

	ALERT( at_aiconsole, "Missing path ent named \"%s\" for NPC \"%s\". Delaying path search for 5 seconds.\n", STRING( monster->m_iszPathName ), STRING( monster->pev->classname ) );
	monster->m_flNextPathEvaluation = gpGlobals->time + 5.0;
	return NULL;
}

bool CBaseMonster::FindBestPath( void )
{
	string_t target = m_iszPathName;
	CBaseEntity *entity = NULL;

	while ( ( entity = UTIL_FindEntityByTargetname( entity, STRING( target ) ) ) != NULL )
	{
		if ( FClassnameIs( entity->pev, "path_condition_controller" ) )
		{
			CPathConditionController *controller = dynamic_cast<CPathConditionController *>( entity );

			m_hPathConditionController = controller;
			m_hPathCondition = NULL;
			m_hPathWaypoint = NULL;

			return true;
		}

		if ( FClassnameIs( entity->pev, "path_condition" ) )
		{
			CPathCondition *condition = dynamic_cast<CPathCondition *>( entity );

			m_hPathConditionController = NULL;
			m_hPathCondition = condition;
			m_hPathWaypoint = NULL;

			return true;
		}

		if ( !FClassnameIs( entity->pev, "path_waypoint" ) )
			continue;

		CPathWaypoint *waypoint = dynamic_cast<CPathWaypoint *>( entity );

		if ( !waypoint )
			continue;

		if ( waypoint->m_iOccupantLimit > 0 && waypoint->m_iOccupantCount >= waypoint->m_iOccupantLimit && !FStringNull( waypoint->m_iszOverflowWaypoint ) && waypoint->m_iszOverflowWaypoint != target )
		{
			target = waypoint->m_iszOverflowWaypoint;
			entity = NULL;
			continue;
		}

		m_hPathConditionController = NULL;
		m_hPathCondition = NULL;
		m_hPathWaypoint = waypoint;

		return true;
	}

	ALERT( at_aiconsole, "Missing path ent named \"%s\" for NPC \"%s\". Delaying path search for 5 seconds.\n", STRING( m_iszPathName ), STRING( pev->classname ) );
	m_flNextPathEvaluation = gpGlobals->time + 5.0;

	return false;
}

void CBaseMonster::EvaluatePaths( void )
{
	if ( FStringNull( m_iszPathName ) || m_flNextPathEvaluation > gpGlobals->time )
		return;

	if ( m_iszPathName != m_iszPreviousPathName )
	{
		if ( FindBestPath() )
			m_iszPreviousPathName = m_iszPathName;

		if ( m_hPathConditionController != NULL )
		{
			CPathConditionController *controller = m_hPathConditionController;

			if ( controller )
			{
				m_hPathCondition = controller->FindBestGroup( this );

				CPathCondition *condition = m_hPathCondition;

				if ( condition )
				{
					m_hPathWaypoint = condition->GetStartingPoint();
					m_flNextPathEvaluation = gpGlobals->time + 1.0;
					return;
				}

				m_flNextPathEvaluation = gpGlobals->time + 1.0;
				return;
			}
		}

		if ( m_hPathCondition != NULL )
		{
			CPathCondition *condition = m_hPathCondition;

			if ( condition )
			{
				m_hPathWaypoint = condition->GetStartingPoint();
				m_flNextPathEvaluation = gpGlobals->time + 1.0;
				return;
			}
		}

		if ( m_hPathWaypoint == NULL )
		{
			m_flNextPathEvaluation = gpGlobals->time + 1.0;
			return;
		}

		ChangeSchedule( GetScheduleOfType( SCHED_PATH_WAYPOINT ) );
	}

	m_flNextPathEvaluation = gpGlobals->time + 1.0;
}

bool CBaseMonster::StartPathWaypointTask( Task_t *pTask )
{
	if ( pTask->iTask < TASK_PATH_WAYPOINT_INITIALIZE || pTask->iTask > TASK_PATH_WAYPOINT_DEPARTURE )
		return false;

	CPathWaypoint *waypoint = dynamic_cast<CPathWaypoint *>( (CBaseEntity *) m_hPathWaypoint );

	if ( !waypoint )
	{
		TaskFail();
		return true;
	}

	switch ( pTask->iTask )
	{
		case TASK_PATH_WAYPOINT_INITIALIZE:
			TaskComplete();
			break;

		case TASK_PATH_WAYPOINT_MOVEMENT:
			if ( ( waypoint->pev->origin - pev->origin ).Length2D() <= V_max( waypoint->m_iRadius, 8 ) )
			{
				RouteNew();
				m_movementGoal = MOVEGOAL_NONE;
				m_movementActivity = ACT_IDLE;
				m_moveradius = 0;
				m_hCineBlocker = NULL;
				Forget( bits_MEMORY_MOVE_FAILED | bits_MEMORY_MOVE_RADIUS_ONLY );
				TaskComplete();
				break;
			}

			switch ( waypoint->m_iMovementType )
			{
				case 0:
				case 1:
				{
					Activity activity = waypoint->m_iMovementType == 0 ? ACT_WALK : ACT_RUN;

					if ( LookupActivity( activity ) == ACTIVITY_NOT_AVAILABLE )
						activity = activity == ACT_WALK ? ACT_RUN : ACT_WALK;

					m_movementActivity = activity;

					if ( !MoveToLocation( activity, 0, waypoint->pev->origin, (float) waypoint->m_iRadius ) )
						TaskFail();
				}
				break;

				case 2:
					UTIL_SetOrigin( pev, waypoint->pev->origin );
					pev->velocity = g_vecZero;
					pev->effects |= EF_NOINTERP;
					pev->flags &= ~FL_ONGROUND;
					TaskComplete();
					break;

				case 3:
					pev->ideal_yaw = UTIL_VecToYaw( waypoint->pev->origin - pev->origin );
					break;

				case 4:
					TaskComplete();
					break;
			}
			break;

		case TASK_PATH_WAYPOINT_ANGLES:
			if ( !waypoint->m_iUseAngles )
			{
				TaskComplete();
				break;
			}

			Stop();
			pev->ideal_yaw = UTIL_AngleMod( waypoint->pev->angles.y );
			SetTurnActivity();
			break;

		case TASK_PATH_WAYPOINT_ARRIVAL:
		{
			FireTargets( STRING( waypoint->m_iszTriggerOnArrival ), waypoint, this, USE_TOGGLE, 0 );

			if ( FStringNull( waypoint->m_iszArrivalAnimation ) )
			{
				TaskComplete();
				break;
			}

			Stop();

			int sequence = LookupSequence( STRING( waypoint->m_iszArrivalAnimation ) );

			if ( sequence == ACTIVITY_NOT_AVAILABLE )
			{
				ALERT( at_error, "%s: unknown sequence \"%s\"\n", STRING( waypoint->pev->targetname ), STRING( waypoint->m_iszArrivalAnimation ) );
				TaskComplete();
				break;
			}

			pev->sequence = sequence;
			pev->frame = 0;
			ResetSequenceInfo();
			m_IdealActivity = ACT_RESET;
			m_Activity = ACT_RESET;
			break;
		}

		case TASK_PATH_WAYPOINT_WAIT:
			if ( waypoint->m_iWaitTime <= 0 && FStringNull( waypoint->m_iszWaitMaster ) )
			{
				TaskComplete();
				break;
			}

			Stop();
			m_flWaitFinished = gpGlobals->time + waypoint->m_iWaitTime;
			ALERT( at_aiconsole, "Waiting for \"%d\" seconds.\n", waypoint->m_iWaitTime );

			if ( waypoint->m_iWaitActivity == 0 && !FStringNull( waypoint->m_iszWaitAnimation ) )
			{
				int sequence = LookupSequence( STRING( waypoint->m_iszWaitAnimation ) );

				if ( sequence > ACTIVITY_NOT_AVAILABLE )
				{
					if ( pev->sequence != sequence || !m_fSequenceLoops )
						pev->frame = 0;

					m_Activity = ACT_RESET;
					m_IdealActivity = ACT_RESET;
					pev->sequence = sequence;
					ResetSequenceInfo();
					SetYawSpeed();
				}
				else
				{
					ALERT( at_aiconsole, "%s has no sequence named: %s\n", STRING( pev->classname ), STRING( waypoint->m_iszWaitAnimation ) );
					pev->sequence = 0;
				}
			}
			break;

		case TASK_PATH_WAYPOINT_DEPARTURE:
		{
			if ( FStringNull( waypoint->m_iszDepartureAnimation ) )
			{
				FireTargets( STRING( waypoint->m_iszTriggerOnDeparture ), waypoint, this, USE_TOGGLE, 0 );
				m_hPathWaypoint = waypoint->GetNextWaypoint();

				if ( m_hPathWaypoint != NULL )
					ChangeSchedule( GetScheduleOfType( SCHED_PATH_WAYPOINT ) );
				else
					TaskComplete();

				break;
			}

			int sequence = LookupSequence( STRING( waypoint->m_iszDepartureAnimation ) );

			if ( sequence == ACTIVITY_NOT_AVAILABLE )
			{
				ALERT( at_error, "%s: unknown sequence \"%s\"\n", STRING( waypoint->pev->targetname ), STRING( waypoint->m_iszDepartureAnimation ) );
				break;
			}

			pev->sequence = sequence;
			pev->frame = 0;
			m_Activity = ACT_RESET;
			m_IdealActivity = ACT_RESET;
			ResetSequenceInfo();
			break;
		}
	}

	return true;
}

bool CBaseMonster::RunPathWaypointTask( Task_t *pTask )
{
	if ( pTask->iTask < TASK_PATH_WAYPOINT_INITIALIZE || pTask->iTask > TASK_PATH_WAYPOINT_DEPARTURE )
		return false;

	CPathWaypoint *waypoint = dynamic_cast<CPathWaypoint *>( (CBaseEntity *) m_hPathWaypoint );

	if ( !waypoint )
	{
		TaskFail();
		return true;
	}

	switch ( pTask->iTask )
	{
		case TASK_PATH_WAYPOINT_INITIALIZE:
			TaskComplete();
			break;

		case TASK_PATH_WAYPOINT_MOVEMENT:
			if ( ( waypoint->pev->origin - pev->origin ).Length2D() <= V_max( waypoint->m_iRadius, 8 ) )
			{
				RouteNew();
				m_movementGoal = MOVEGOAL_NONE;
				m_movementActivity = ACT_IDLE;
				m_moveradius = 0;
				m_hCineBlocker = NULL;
				Forget( bits_MEMORY_MOVE_FAILED | bits_MEMORY_MOVE_RADIUS_ONLY );
				TaskComplete();
				break;
			}

			if ( waypoint->m_iOccupantLimit > 0 && waypoint->m_iOccupantCount >= waypoint->m_iOccupantLimit && !FStringNull( waypoint->m_iszOverflowWaypoint ) )
			{
				CPathWaypoint *overflow = FindPathWaypoint( this, waypoint->m_iszOverflowWaypoint );

				if ( !overflow )
					break;

				m_hPathWaypoint = overflow;
				ChangeSchedule( GetScheduleOfType( SCHED_PATH_WAYPOINT ) );
				m_failSchedule = SCHED_PATH_WAYPOINT;
				break;
			}

			switch ( waypoint->m_iMovementType )
			{
				case 0:
				case 1:
					m_movementActivity = waypoint->m_iMovementType == 0 ? ACT_WALK : ACT_RUN;

					if ( LookupActivity( m_movementActivity ) == ACTIVITY_NOT_AVAILABLE )
						m_movementActivity = m_movementActivity == ACT_WALK ? ACT_RUN : ACT_WALK;
					break;

				case 2:
					UTIL_SetOrigin( pev, waypoint->pev->origin );
					pev->velocity = g_vecZero;
					pev->effects |= EF_NOINTERP;
					pev->flags &= ~FL_ONGROUND;
					TaskComplete();
					break;

				case 3:
					pev->ideal_yaw = UTIL_VecToYaw( waypoint->pev->origin - pev->origin );
					ChangeYaw( pev->yaw_speed );

					if ( FacingIdeal() )
						TaskComplete();
					break;

				case 4:
					TaskComplete();
					break;
			}
			break;

		case TASK_PATH_WAYPOINT_ANGLES:
			if ( !waypoint->m_iUseAngles )
			{
				TaskComplete();
				break;
			}

			ChangeYaw( pev->yaw_speed );

			if ( FacingIdeal() )
				TaskComplete();
			break;

		case TASK_PATH_WAYPOINT_ARRIVAL:
			if ( FStringNull( waypoint->m_iszArrivalAnimation ) || m_fSequenceFinished )
			{
				FireTargets( STRING( waypoint->m_iszTriggerAfterArrival ), waypoint, this, USE_TOGGLE, 0 );
				TaskComplete();
			}
			break;

		case TASK_PATH_WAYPOINT_WAIT:
			if ( gpGlobals->time >= m_flWaitFinished && ( FStringNull( waypoint->m_iszWaitMaster ) || UTIL_IsMasterTriggered( waypoint->m_iszWaitMaster, this ) ) )
			{
				TaskComplete();
				break;
			}

			if ( waypoint->m_iWaitActivity == 1 )
			{
				pev->ideal_yaw = UTIL_AngleMod( UTIL_AngleMod( pev->angles.y ) + RANDOM_LONG( -180, 180 ) );
				ChangeYaw( pev->yaw_speed );

				if ( m_afCapability & bits_CAP_TURN_HEAD )
				{
					if ( pev->ideal_yaw > 180 )
						pev->ideal_yaw -= 360;

					if ( pev->ideal_yaw < -180 )
						pev->ideal_yaw += 360;

					SetBoneController( 0, pev->ideal_yaw );
				}

				if ( FStringNull( waypoint->m_iszWaitAnimation ) )
				{
					m_IdealActivity = ACT_IDLE;
					SetTurnActivity();
					break;
				}
			}
			else if ( waypoint->m_iWaitActivity != 0 )
				break;

			if ( !FStringNull( waypoint->m_iszWaitAnimation ) && m_fSequenceFinished )
			{
				if ( m_fSequenceLoops )
				{
					pev->frame = 0;
					ResetSequenceInfo();
				}
				else if ( waypoint->m_iWaitActivity == 0 )
					m_IdealActivity = ACT_IDLE;
			}
			break;

		case TASK_PATH_WAYPOINT_DEPARTURE:
			if ( FStringNull( waypoint->m_iszDepartureAnimation ) || m_fSequenceFinished )
			{
				FireTargets( STRING( waypoint->m_iszTriggerOnDeparture ), waypoint, this, USE_TOGGLE, 0 );
				m_hPathWaypoint = waypoint->GetNextWaypoint();

				if ( m_hPathWaypoint != NULL )
					ChangeSchedule( GetScheduleOfType( SCHED_PATH_WAYPOINT ) );
				else
					TaskComplete();
			}
			break;
	}

	return true;
}
