#ifndef CCUSTOMRESPAWNDELAY_H
#define CCUSTOMRESPAWNDELAY_H

#include "extdll.h"

class CBaseEntity;

class CCustomRespawnDelay
{
public:
	static const float DEFAULT_RESPAWN_DELAY;

	CCustomRespawnDelay();

	void OnCreate(const char *cvarName);
	bool KeyValue(CBaseEntity *entity, KeyValueData *data);
	float GetRespawnTime(float defaultTime, bool useDefaultTime);
	bool ItemCanRespawn() const;

private:
	enum
	{
		NO_RESPAWN_SPAWNFLAG = 0x400
	};

	const char	*m_pszCVarName;
	float		 m_flCustomRespawnTime;
};

#endif
