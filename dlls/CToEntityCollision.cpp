#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CToEntityCollision.h"

static const float COLLISION_DISABLED_TIME = -1.0f;

CToEntityCollision::CToEntityCollision()
	: m_flNotSolidUntil(0)
{
	SetEnt(NULL);
}

void CToEntityCollision::SetEnt(CBaseEntity *entity)
{
	m_flNotSolidUntil = 0;
	m_hEntity = entity;
}

CBaseEntity *CToEntityCollision::GetEnt(void)
{
	return m_hEntity;
}

bool CToEntityCollision::DoIntersect(CBaseEntity *entity)
{
	CBaseEntity *other = GetEnt();

	return entity && other && entity->Intersects(other);
}

bool CToEntityCollision::CanCollide(void)
{
	if (m_flNotSolidUntil == COLLISION_DISABLED_TIME)
		return false;

	if (GetEnt())
		return gpGlobals->time > m_flNotSolidUntil;

	return true;
}

float CToEntityCollision::GetNotSolidUntil(void)
{
	return m_flNotSolidUntil;
}

void CToEntityCollision::SetNotSolidUntil(float time)
{
	m_flNotSolidUntil = time;
}

void CToEntityCollision::Reset(void)
{
	SetEnt(NULL);
	m_flNotSolidUntil = 0;
}
