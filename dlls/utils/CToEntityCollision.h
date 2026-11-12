#ifndef CTOENTITYCOLLISION_H
#define CTOENTITYCOLLISION_H

#include "utils/EntityHandle.h"

class CBaseEntity;

class CToEntityCollision
{
public:
	CToEntityCollision();

	void SetEnt(CBaseEntity *entity);
	CBaseEntity *GetEnt(void);
	bool DoIntersect(CBaseEntity *entity);
	bool CanCollide(void);
	float GetNotSolidUntil(void);
	void SetNotSolidUntil(float time);
	void Reset(void);

private:
	float						m_flNotSolidUntil;
	EntityHandle<CBaseEntity>	m_hEntity;
};

#endif
