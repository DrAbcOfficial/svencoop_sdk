#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "observer.h"
#include "trigger_cyclicobserver.h"

LINK_ENTITY_TO_CLASS( trigger_cyclicobserver, CTriggerCyclicObserver );

CTriggerCyclicObserver::CTriggerCyclicObserver() : m_Mode( 0 )
{
}

CTriggerCyclicObserver::~CTriggerCyclicObserver()
{
}

void CTriggerCyclicObserver::KeyValue( KeyValueData *data )
{
	if ( !stricmp( data->szKeyName, "m_Mode" ) )
	{
		m_Mode = atoi( data->szValue );
		data->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( data );
}

void CTriggerCyclicObserver::Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value )
{
	bool enabled = CVAR_GET_FLOAT( "mp_observer_cyclic" ) == 0;

	if ( m_Mode == 0 )
		enabled = false;
	else if ( m_Mode == 1 )
		enabled = true;

	CObserver::SetObserverCyclic( enabled );
	SUB_UseTargets( activator, USE_TOGGLE, 0 );
}
