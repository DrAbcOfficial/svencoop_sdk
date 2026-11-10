#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "env_render_individual.h"

#define SF_RENDER_MASKFX ( 1 << 0 )
#define SF_RENDER_MASKAMT ( 1 << 1 )
#define SF_RENDER_MASKMODE ( 1 << 2 )
#define SF_RENDER_MASKCOLOR ( 1 << 3 )
#define SF_RENDER_MASKALL ( SF_RENDER_MASKFX | SF_RENDER_MASKAMT | SF_RENDER_MASKMODE | SF_RENDER_MASKCOLOR )
#define SF_RENDER_AUTOAPPLY ( 1 << 4 )
#define SF_RENDER_STARTON ( 1 << 5 )
#define SF_RENDER_ACTIVATOR ( 1 << 6 )
#define SF_RENDER_COPY ( 1 << 7 )

LINK_ENTITY_TO_CLASS( env_render_individual, CEnvRenderIndividual );

void CEnvRenderIndividual::Spawn()
{
	if ( ( pev->spawnflags & SF_RENDER_MASKALL ) == SF_RENDER_MASKALL )
	{
		ALERT( at_error, "Error in env_render_individual (%s): All render settings are disabled.\n", STRING( pev->targetname ) );
		pev->flags |= FL_KILLME;
		return;
	}

	if ( ( pev->spawnflags & SF_RENDER_AUTOAPPLY )
		&& ( pev->spawnflags & SF_RENDER_ACTIVATOR ) )
	{
		ALERT( at_error, "Error in env_render_individual (%s): Auto Apply and Affect Activator don't work together.\n", STRING( pev->targetname ) );
		pev->flags |= FL_KILLME;
		return;
	}

	if ( ( pev->spawnflags & SF_RENDER_STARTON )
		&& ( pev->spawnflags & SF_RENDER_ACTIVATOR ) )
	{
		ALERT( at_error, "Error in env_render_individual (%s): Start On and Affect Activator don't work together.\n", STRING( pev->targetname ) );
		pev->flags |= FL_KILLME;
		return;
	}

	m_bActive = ( pev->spawnflags & SF_RENDER_STARTON ) != 0;

	if ( m_bActive )
	{
		SetThink( &CEnvRenderIndividual::AutoApplyThink );
		pev->nextthink = gpGlobals->time;
	}
}

void CEnvRenderIndividual::Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value )
{
	if ( useType == USE_KILL )
	{
		pev->flags |= FL_KILLME;
		return;
	}

	if ( ( pev->spawnflags & SF_RENDER_ACTIVATOR ) && ( !activator || !activator->IsPlayer() ) )
		return;

	m_bActive = useType != USE_OFF;
	HandleEntities( activator );

	if ( m_bActive && ( pev->spawnflags & SF_RENDER_AUTOAPPLY ) )
	{
		SetThink( &CEnvRenderIndividual::AutoApplyThink );
		pev->nextthink = gpGlobals->time + 0.1;
	}
}

void CEnvRenderIndividual::HandleEntityForPlayer( CBasePlayer *player, CBaseEntity *entity )
{
	if ( !player || !entity )
		return;

	if ( m_bActive )
		player->AddEntityRenderOverride( EntityHandle<CBaseEntity>( entity ), this );
	else
		player->RemoveEntityRenderOverride( EntityHandle<CBaseEntity>( entity ) );
}

void CEnvRenderIndividual::HandleEntity( CBaseEntity *activator, CBaseEntity *entity )
{
	if ( pev->spawnflags & SF_RENDER_ACTIVATOR )
	{
		if ( activator && activator->IsPlayer() )
			HandleEntityForPlayer( (CBasePlayer *) activator, entity );

		return;
	}

	CBaseEntity *recipient = NULL;

	while ( ( recipient = UTIL_FindEntityByTargetname( recipient, STRING( pev->netname ) ) ) != NULL )
	{
		if ( recipient->IsPlayer() )
			HandleEntityForPlayer( (CBasePlayer *) recipient, entity );
	}
}

void CEnvRenderIndividual::HandleEntities( CBaseEntity *activator )
{
	CBaseEntity *entity = NULL;

	while ( ( entity = UTIL_FindEntityByTargetname( entity, STRING( pev->target ) ) ) != NULL )
		HandleEntity( activator, entity );
}

void CEnvRenderIndividual::AutoApplyThink()
{
	HandleEntities( NULL );

	if ( m_bActive && ( pev->spawnflags & SF_RENDER_AUTOAPPLY ) )
		pev->nextthink = gpGlobals->time + 0.1;
}

int CEnvRenderIndividual::GetRenderMode( int original )
{
	if ( pev->spawnflags & SF_RENDER_MASKMODE )
		return original;

	if ( !( pev->spawnflags & SF_RENDER_COPY ) )
		return pev->rendermode;

	CBaseEntity *source = m_hCopyFrom;

	if ( !source )
	{
		m_hCopyFrom = UTIL_FindEntityByTargetname( NULL, STRING( pev->message ) );
		source = m_hCopyFrom;
	}

	return source ? source->pev->rendermode : original;
}

int CEnvRenderIndividual::GetRenderAmt( int original )
{
	if ( pev->spawnflags & SF_RENDER_MASKAMT )
		return original;

	if ( !( pev->spawnflags & SF_RENDER_COPY ) )
		return (int) pev->renderamt;

	CBaseEntity *source = m_hCopyFrom;

	if ( !source )
	{
		m_hCopyFrom = UTIL_FindEntityByTargetname( NULL, STRING( pev->message ) );
		source = m_hCopyFrom;
	}

	return source ? (int) source->pev->renderamt : original;
}

int CEnvRenderIndividual::GetRenderFx( int original )
{
	if ( pev->spawnflags & SF_RENDER_MASKFX )
		return original;

	if ( !( pev->spawnflags & SF_RENDER_COPY ) )
		return pev->renderfx;

	CBaseEntity *source = m_hCopyFrom;

	if ( !source )
	{
		m_hCopyFrom = UTIL_FindEntityByTargetname( NULL, STRING( pev->message ) );
		source = m_hCopyFrom;
	}

	return source ? source->pev->renderfx : original;
}

const Vector &CEnvRenderIndividual::GetRenderColor( const Vector &original )
{
	if ( pev->spawnflags & SF_RENDER_MASKCOLOR )
		return original;

	if ( !( pev->spawnflags & SF_RENDER_COPY ) )
		return pev->rendercolor;

	CBaseEntity *source = m_hCopyFrom;

	if ( !source )
	{
		m_hCopyFrom = UTIL_FindEntityByTargetname( NULL, STRING( pev->message ) );
		source = m_hCopyFrom;
	}

	return source ? source->pev->rendercolor : original;
}
