#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "teleportdestination.h"

LINK_ENTITY_TO_CLASS( info_teleport_destination, CTeleportDestination );

void CTeleportDestination::Spawn( void )
{
	m_flNextTeleportTime = 0;
	CPointEntity::Spawn();
}

void CTeleportDestination::KeyValue( KeyValueData *data )
{
	if ( FStrEq( data->szKeyName, "teleport_cooldown" ) )
	{
		double delay = atof( data->szValue );

		m_bHasTeleportDelay = true;
		m_flTeleportDelay = delay;
		data->fHandled = TRUE;
	}
	else
	{
		CBaseEntity::KeyValue( data );
	}
}
