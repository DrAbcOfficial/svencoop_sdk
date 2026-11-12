#ifndef CSPAWNPOINTS_H
#define CSPAWNPOINTS_H

#include "utils/TypedEntityHandle.h"

#define SF_SPAWNPOINT_START_OFF 2
#define SF_SPAWNPOINT_REPEL 4
#define SF_SPAWNPOINT_FILTER_TARGETNAME 8
#define SF_SPAWNPOINT_INVERT_FILTER 16
#define SF_SPAWNPOINT_FIRE_TARGET 32

class CBaseEntity;

bool UTIL_SpawnPointFilterPasses( CBaseEntity *point, CBaseEntity *player );
bool UTIL_IsSpawnPointValid( CBaseEntity *point, CBaseEntity *player );
bool UTIL_IsSpawnPointOccupied( CBaseEntity *point );
Vector GetSpawnSpotPlace( CBaseEntity *point );
void PlayerSpawns( edict_t *point, edict_t *player );

class CSpawnPoints
{
public:
	CSpawnPoints();
	~CSpawnPoints();

	void Add( CBaseEntity *point );
	void Collect( CBaseEntity *player );
	CBaseEntity *GetRandomSpawnPoint( bool avoidOccupied );

	static const char *const m_DeathmatchClassNames[2];
	static const char *const m_CoopClassNames[2];

	// ScriptedSnark: TODO:
	/*
// Secondary player spawn point entities. Only used if no primary types are found, as these are generic points without special spawn point functionality (kept for legacy purposes)
static const char *pSecondaryValidSpawnNames[] = 
{
    "info_player_coop",
    "info_player_start",
};
	*/
private:
	struct SpawnPoint
	{
		CEntityHandle<CBaseEntity> m_hEntity;
		SpawnPoint               *m_pNext;
	};

	SpawnPoint *m_pPoints;
	int         m_iCount;
};

#endif
