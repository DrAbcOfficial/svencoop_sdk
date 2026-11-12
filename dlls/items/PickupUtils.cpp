#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "ai/animation.h"
#include "items/PickupUtils.h"

COLLECT_TYPE GetUseCollectType( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType )
{
	if ( activator && activator->IsPlayer() && activator == caller && useType == USE_SET )
		return COLLECT_USE_DIRECT;

	return COLLECT_USE_INDIRECT;
}

bool UTIL_HasLineOfSight( CBaseEntity *observer, CBaseEntity *target, bool useOrigin, bool ignoreGlass )
{
	if ( !observer || !target )
		return false;

	Vector offset = useOrigin ? g_vecZero : Vector( 0, 0, 8 );
	Vector start = observer->pev->origin;
	Vector end = target->pev->origin + offset;
	edict_t *ignore = observer->edict();
	int traceFlags = ignoreGlass ? 0x100 : 0;
	TraceResult trace = {};

	g_engfuncs.pfnTraceLine( start, end, traceFlags, ignore, &trace );

	if ( ( trace.fInOpen || trace.fInWater ) && trace.flFraction == 1.0f )
		return true;

	Vector corners[8] = {
		target->pev->absmax + offset,
		Vector( target->pev->absmax.x, target->pev->absmax.y, target->pev->absmin.z ) + offset,
		Vector( target->pev->absmax.x, target->pev->absmin.y, target->pev->absmin.z ) + offset,
		Vector( target->pev->absmax.x, target->pev->absmin.y, target->pev->absmax.z ) + offset,
		target->pev->absmin + offset,
		Vector( target->pev->absmin.x, target->pev->absmin.y, target->pev->absmax.z ) + offset,
		Vector( target->pev->absmin.x, target->pev->absmax.y, target->pev->absmax.z ) + offset,
		Vector( target->pev->absmin.x, target->pev->absmax.y, target->pev->absmin.z ) + offset
	};

	for ( int i = 0; i < 8; i++ )
	{
		g_engfuncs.pfnTraceLine( observer->pev->origin, corners[i], traceFlags, ignore, &trace );

		if ( ( trace.fInOpen || trace.fInWater ) && trace.flFraction == 1.0f )
			return true;
	}

	return false;
}

void SetSequenceFromLookup( entvars_t *pev, int sequenceName, int sequence )
{
	if ( sequenceName && sequence <= 0 )
		sequence = LookupSequence( GET_MODEL_PTR( ENT( pev ) ), STRING( sequenceName ) );

	pev->frame = 0;

	if ( sequence >= 0 )
	{
		if ( strcmp( STRING( sequenceName ), "idle" ) )
			ALERT( at_aiconsole, "Sequence \"%s\" found in model \"%s\" at index %d.\n",
			       STRING( sequenceName ), STRING( pev->model ), sequence );

		pev->sequence = sequence;
		pev->framerate = 1.0f;
	}
	else
	{
		if ( strcmp( STRING( sequenceName ), "idle" ) )
			ALERT( at_error, "Sequence \"%s\" not found in model \"%s\".\n",
			       STRING( sequenceName ), STRING( pev->model ) );

		pev->sequence = 0;
		pev->framerate = 0;
	}
}

void Item_SetDefaultSize( entvars_t *pev, Vector mins, Vector maxs )
{
	CBaseEntity *entity = CBaseEntity::Instance( pev );
	bool customMins = pev->vuser1 != g_vecZero;
	bool customMaxs = pev->vuser2 != g_vecZero;

	UTIL_SetSize( pev, customMins ? pev->vuser1 : mins, customMaxs ? pev->vuser2 : maxs );

	if ( customMins || customMaxs )
		entity->m_fCustomModel = true;
}
