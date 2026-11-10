#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "basemonster.h"
#include "PathCondition.h"
#include "CASWaypointManager.h"
#include "CASFunction.h"
#include "CASDocumentation.h"
#include "ASBaseEntity.h"

LINK_ENTITY_TO_CLASS( path_condition, CPathCondition );
LINK_ENTITY_TO_CLASS( path_condition_controller, CPathConditionController );

void CPathCondition::KeyValue( KeyValueData *data )
{
	if ( FStrEq( data->szKeyName, "conditions_reference" ) )
	{
		m_pConditions = CASWaypointManager::GetInstance()->GetConditions( data->szValue );
		data->fHandled = TRUE;
		return;
	}

	CBaseEntity::KeyValue( data );
}

void CPathCondition::Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value )
{
	CBaseEntity::Use( activator, caller, useType, value );
}

bool CPathCondition::IsValid( CBaseMonster *monster )
{
	if ( !monster || !m_fEnabled )
		return false;

	if ( monster->m_MonsterState == MONSTERSTATE_SCRIPT )
		return false;

	if ( m_flMinDistance > 0.0f || m_flMaxDistance > 0.0f )
	{
		float distance = ( monster->Center() - pev->origin ).Length();

		if ( m_flMinDistance > 0.0f && m_flMinDistance > distance )
			return false;

		if ( m_flMaxDistance > 0.0f && distance > m_flMaxDistance )
			return false;
	}

	if ( m_pConditions )
	{
		bool start = false;

		m_pConditions->m_pStartFunction->Call( &start, 0, this, monster );
	}

	return m_fEnabled;
}

CPathWaypoint *CPathCondition::GetStartingPoint( void )
{
	return NULL;
}

void CPathConditionController::Spawn( void )
{
	m_fEnabled = !( pev->spawnflags & 1 );

	if ( FStringNull( pev->targetname ) )
	{
		ALERT( at_error, "Path_condition_controller without a targetname!\n" );
		return;
	}

	SetThink( &CPathConditionController::Initialize );
	pev->nextthink = gpGlobals->time + 1.0f;
}

void CPathConditionController::KeyValue( KeyValueData *data )
{
	if ( FStrEq( data->szKeyName, "movementtype" ) || FStrEq( data->szKeyName, "grouplist" ) )
	{
		data->fHandled = TRUE;
		return;
	}

	CBaseEntity::KeyValue( data );
}

void CPathConditionController::Initialize( void )
{
	ClearAll();

	if ( FStringNull( pev->targetname ) )
	{
		SetThink( &CBaseEntity::SUB_Remove );
	}
}

void CPathConditionController::ClearAll( void )
{
}

CPathCondition *CPathConditionController::FindBestGroup( CBaseMonster *monster )
{
	PathConditionGroup_t *group = m_pGroups;

	while ( group )
		group = group->m_pNext;

	m_pGroups = NULL;

	return NULL;
}

void CPathConditionController::Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value )
{
	if ( useType == USE_OFF )
		m_fEnabled = false;
	else if ( useType == USE_ON )
		m_fEnabled = true;
	else
		m_fEnabled = !m_fEnabled;
}

void RegisterSCScriptPathConditionEntity( CASDocumentation *documentation )
{
	RegisterSCScriptBaseEntity<CPathCondition>( documentation, "CPathCondition", true );
}
