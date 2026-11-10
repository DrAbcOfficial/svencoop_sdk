#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "trigger_respawn.h"

#define SF_RESPAWN_TARGETED 1
#define SF_RESPAWN_OBSERVERS 2
#define SF_RESPAWN_DEAD_ONLY 4

LINK_ENTITY_TO_CLASS( trigger_respawn, CTriggerRespawn );

void CTriggerRespawn::RespawnTarget( CBaseEntity *entity, bool allowLiving, bool respawnObserver, int respawnDelay )
{
	if ( !entity )
		return;

	if ( entity->IsPlayer() )
	{
		CBasePlayer *player = (CBasePlayer *) entity;

		UTIL_RespawnPlayer( player, allowLiving, respawnObserver );

		if ( respawnDelay > 0 )
			player->m_flRespawnDelayTime -= (float) respawnDelay;
	}
	else
	{
		entity->Spawn();
	}
}

void CTriggerRespawn::Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value )
{
	int respawnDelay = (int) CVAR_GET_FLOAT( "mp_respawndelay" );
	bool allowLiving = ( pev->spawnflags & SF_RESPAWN_DEAD_ONLY ) == 0;
	bool respawnObserver = ( pev->spawnflags & SF_RESPAWN_OBSERVERS ) != 0;

	if ( pev->spawnflags & SF_RESPAWN_TARGETED )
	{
		CBaseEntity *entity = NULL;

		if ( FStrEq( STRING( pev->target ), "!activator" ) )
			entity = activator;
		else if ( FStrEq( STRING( pev->target ), "!caller" ) )
			entity = caller;
		else
		{
			while ( ( entity = UTIL_FindEntityByTargetname( entity, STRING( pev->target ) ) ) != NULL )
				RespawnTarget( entity, allowLiving, respawnObserver, respawnDelay );

			return;
		}

		if ( !entity || !entity->IsPlayer() )
			return;

		RespawnTarget( entity, allowLiving, respawnObserver, respawnDelay );
		return;
	}

	for ( int index = 1; index <= gpGlobals->maxClients; index++ )
	{
		CBaseEntity *entity = UTIL_PlayerByIndex( index );

		if ( entity )
			RespawnTarget( entity, allowLiving, respawnObserver, respawnDelay );
	}

	FireTargets( STRING( pev->target ), activator, this, useType, value );
}
