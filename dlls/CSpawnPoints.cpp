#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CSpawnPoints.h"

const char *const CSpawnPoints::m_DeathmatchClassNames[] =
{
	"info_player_deathmatch",
	"info_player_dm2"
};

const char *const CSpawnPoints::m_CoopClassNames[] =
{
	"info_player_coop",
	"info_player_start"
};

CSpawnPoints::CSpawnPoints()
	: m_pPoints( NULL ), m_iCount( 0 )
{
}

CSpawnPoints::~CSpawnPoints()
{
	if ( !m_pPoints || !m_iCount )
		return;

	while ( m_pPoints )
	{
		SpawnPoint *next = m_pPoints->m_pNext;

		delete m_pPoints;
		m_pPoints = next;
	}

	m_iCount = 0;
}

void CSpawnPoints::Add( CBaseEntity *point )
{
	SpawnPoint **next = &m_pPoints;

	while ( *next )
	{
		if ( (CBaseEntity *) ( *next )->m_hEntity == point )
			return;

		next = &( *next )->m_pNext;
	}

	SpawnPoint *entry = new SpawnPoint;

	entry->m_hEntity = point;
	entry->m_pNext = NULL;
	*next = entry;
	m_iCount++;
}

void CSpawnPoints::Collect( CBaseEntity *player )
{
	int found = 0;
	CBaseEntity *point = NULL;

	for ( int index = 0; index < ARRAYSIZE( m_DeathmatchClassNames ); index++ )
	{
		while ( ( point = UTIL_FindEntityByClassname( point, m_DeathmatchClassNames[index] ) ) != NULL )
		{
			found++;

			if ( UTIL_IsSpawnPointValid( point, player ) )
				Add( point );
		}
	}

	if ( found > 0 )
		return;

	for ( int index = 0; index < ARRAYSIZE( m_CoopClassNames ); index++ )
	{
		while ( ( point = UTIL_FindEntityByClassname( point, m_CoopClassNames[index] ) ) != NULL )
		{
			found++;

			if ( UTIL_IsSpawnPointValid( point, player ) )
				Add( point );
		}
	}

	if ( found > 0 )
		return;

	if ( gpGlobals->startspot && STRING( gpGlobals->startspot )[0] )
	{
		point = UTIL_FindEntityByTargetname( NULL, STRING( gpGlobals->startspot ) );

		if ( UTIL_IsSpawnPointValid( point, player ) )
			Add( point );
	}

	if ( m_iCount <= 0 )
	{
		edict_t *world = INDEXENT( 0 );

		if ( !world )
			world = INDEXENT( 0 );

		Add( world && !world->free ? (CBaseEntity *) world->pvPrivateData : NULL );
	}
}

CBaseEntity *CSpawnPoints::GetRandomSpawnPoint( bool avoidOccupied )
{
	if ( m_iCount == 0 )
		return NULL;

	int first = RANDOM_LONG( 0, m_iCount - 1 );
	SpawnPoint *point = m_pPoints;

	for ( int index = 0; point && index < first; index++ )
		point = point->m_pNext;

	if ( !point )
		return NULL;

	int current = first;
	int checked = 0;

	while ( avoidOccupied && UTIL_IsSpawnPointOccupied( point->m_hEntity ) )
	{
		checked++;

		if ( checked >= m_iCount )
			return NULL;

		if ( current >= m_iCount - 1 )
		{
			current = 0;
			point = m_pPoints;
		}
		else
		{
			current++;
			point = point->m_pNext;
		}

		if ( !point )
			return NULL;
	}

	return point->m_hEntity;
}

bool UTIL_SpawnPointFilterPasses( CBaseEntity *point, CBaseEntity *player )
{
	if ( !point || !player )
		return false;

	if ( !( point->pev->spawnflags & SF_SPAWNPOINT_FILTER_TARGETNAME ) )
		return true;

	char *filter = (char *) STRING( point->pev->message );
	const char *name = STRING( player->pev->targetname );

	if ( !filter[0] && !name[0] )
		return true;

	bool matches = false;
	char *token = strtok( filter, ";" );

	while ( token )
	{
		if ( !strcmp( name, token ) )
		{
			matches = true;
			break;
		}

		token = strtok( NULL, ";" );
	}

	if ( point->pev->spawnflags & SF_SPAWNPOINT_INVERT_FILTER )
		matches = !matches;

	return matches;
}

bool UTIL_IsSpawnPointValid( CBaseEntity *point, CBaseEntity *player )
{
	if ( !point || !player )
		return false;

	if ( !point->IsAlive() && !FClassnameIs( point->pev, "info_player_start" ) )
		return false;

	return UTIL_SpawnPointFilterPasses( point, player );
}

bool UTIL_IsSpawnPointOccupied( CBaseEntity *point )
{
	if ( !point )
		return true;

	Vector mins = point->pev->origin + Vector( -16, -16, 0 );
	Vector maxs = point->pev->origin + Vector( 16, 16, 72 );

	mins = mins - Vector( 8, 8, 8 );
	maxs = maxs + Vector( 8, 8, 8 );

	CBaseEntity *entities[8];
	int count = UTIL_EntitiesInBox( entities, ARRAYSIZE( entities ), mins, maxs, FL_CLIENT | FL_MONSTER );

	for ( int index = 0; index < count; index++ )
	{
		if ( entities[index]->pev->solid != SOLID_NOT )
			return true;
	}

	return false;
}
