#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "animation.h"
#include "trigger_changemodel.h"

LINK_ENTITY_TO_CLASS( trigger_changemodel, CTriggerChangeModel );

bool IsValidChangeModelTarget( CBaseEntity *entity )
{
	if ( !entity || !entity->pev )
		return false;

	edict_t *edict = entity->edict();

	return edict && !edict->free && !FNullEnt( edict );
}

void CTriggerChangeModel::OnCreate( void )
{
	m_iBodygroup = 0;
	m_iValue = 0;
}

void CTriggerChangeModel::Spawn( void )
{
	Precache();
}

void CTriggerChangeModel::Precache( void )
{
	UTIL_PrecacheModel( this, STRING( pev->model ) );
}

void CTriggerChangeModel::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "bodygroup" ) )
	{
		m_iBodygroup = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "value" ) )
	{
		m_iValue = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

void CTriggerChangeModel::ChangeModel( CBaseEntity *entity )
{
	UTIL_SetModel( entity, STRING( pev->model ) );
	entity->pev->skin = pev->skin;
	SetBodygroup( GET_MODEL_PTR( entity->edict() ), entity->pev, m_iBodygroup, m_iValue );
}

void CTriggerChangeModel::Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value )
{
	if ( FStrEq( STRING( pev->target ), "!activator" ) && IsValidChangeModelTarget( activator ) )
	{
		ChangeModel( activator );
		return;
	}

	if ( FStrEq( STRING( pev->target ), "!caller" ) && IsValidChangeModelTarget( caller ) )
	{
		ChangeModel( caller );
		return;
	}

	CBaseEntity *entity = NULL;

	for ( ;; )
	{
		edict_t *start = entity && entity->pev ? entity->edict() : NULL;
		edict_t *found = FIND_ENTITY_BY_TARGETNAME( start, STRING( pev->target ) );

		if ( !found || found->free || FNullEnt( found ) || found->free )
			break;

		entity = CBaseEntity::Instance( found );

		if ( !entity )
			break;

		if ( IsValidChangeModelTarget( entity ) )
			ChangeModel( entity );
	}
}
