#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "gamerules/CCustomRespawnDelay.h"
#include <limits.h>

const float CCustomRespawnDelay::DEFAULT_RESPAWN_DELAY = -1.0f;

namespace
{
const float USE_DEFAULT_RESPAWN_TIME = -2.0f;
const float NO_RESPAWN_TIME = (float)INT_MAX;
} // namespace

CCustomRespawnDelay::CCustomRespawnDelay()
	: m_pszCVarName(NULL), m_flCustomRespawnTime(DEFAULT_RESPAWN_DELAY)
{
}

void CCustomRespawnDelay::OnCreate(const char *cvarName)
{
	m_pszCVarName = cvarName;
}

bool CCustomRespawnDelay::KeyValue(CBaseEntity *entity, KeyValueData *data)
{
	if (!entity || stricmp("m_flCustomRespawnTime", data->szKeyName))
		return false;

	m_flCustomRespawnTime = atof(data->szValue);

	if (m_flCustomRespawnTime < 0.0f)
		entity->pev->spawnflags |= NO_RESPAWN_SPAWNFLAG;

	data->fHandled = TRUE;
	return true;
}

float CCustomRespawnDelay::GetRespawnTime(float defaultTime, bool useDefaultTime)
{
	if (m_flCustomRespawnTime != DEFAULT_RESPAWN_DELAY)
		return m_flCustomRespawnTime;

	if (useDefaultTime)
		return defaultTime;

	float value = CVAR_GET_FLOAT(m_pszCVarName);

	if (value == USE_DEFAULT_RESPAWN_TIME)
		return defaultTime;

	if (value == DEFAULT_RESPAWN_DELAY || value < USE_DEFAULT_RESPAWN_TIME)
		return NO_RESPAWN_TIME;

	return value;
}

bool CCustomRespawnDelay::ItemCanRespawn() const
{
	float value = CVAR_GET_FLOAT(m_pszCVarName);

	return m_flCustomRespawnTime != DEFAULT_RESPAWN_DELAY ||
	       (value != DEFAULT_RESPAWN_DELAY && value >= USE_DEFAULT_RESPAWN_TIME);
}
