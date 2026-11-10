#ifndef PICKUPUTILS_H
#define PICKUPUTILS_H

enum COLLECT_TYPE
{
	COLLECT_TOUCH = 0,
	COLLECT_USE_DIRECT,
	COLLECT_USE_INDIRECT
};

COLLECT_TYPE GetUseCollectType( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType );
bool UTIL_HasLineOfSight( CBaseEntity *observer, CBaseEntity *target, bool useOrigin, bool ignoreGlass );
void SetSequenceFromLookup( entvars_t *pev, int sequenceName, int sequence );
void Item_SetDefaultSize( entvars_t *pev, Vector mins, Vector maxs );

#endif
