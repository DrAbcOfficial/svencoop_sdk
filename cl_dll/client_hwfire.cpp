#include "hud.h"
#include "cl_util.h"
#include "client_hwfire.h"
#include "cl_entity.h"
#include "pm_defs.h"
#include "eventscripts.h"
#include "ev_hldm.h"

#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

// 2.5 degrees in radians, folded in float precision
#define HWFIRE_SPREAD	( 2.5f * (float)M_PI / 180.0f )

CClientHWFire::CClientHWFire() :
	m_vecOffset(0, 0, 0)
{
}

void CClientHWFire::Spawn(void)
{
	m_pszClassname = "hwgrunt_fire";
	m_bKill = false;
	SetThink((void (CClientObject::*)(void)) &CClientHWFire::EffectThink);
	m_flNextThink = Time();
	m_iEntity = 0;
}

void CClientHWFire::EffectThink(void)
{
	m_flNextThink = Time() + 0.05;

	cl_entity_t *pEntity = GetEntity(m_iEntity);
	cl_entity_t *pTarget = GetEntity(m_iTarget);

	if (!pEntity || !pTarget)
	{
		TurnOff();
		return;
	}

	Vector vecForward(0, 0, 0), vecRight(0, 0, 0), vecUp(0, 0, 0);

	AngleVectors(pEntity->angles, vecForward, vecRight, vecUp);

	Vector vecSrc = pEntity->attachment[0] + Vector(0, 0, 42) + vecForward * 15;
	Vector vecDir = ((pTarget->origin - vecSrc) + m_vecOffset).Normalize();

	TE_FireBullet(vecSrc, vecDir, 2, Vector(HWFIRE_SPREAD, HWFIRE_SPREAD, HWFIRE_SPREAD), 2, m_iEntity);
}

void CClientHWFire::TurnOff(void)
{
	m_bKill = true;
	Kill();
}

void TE_CreateHWGEffect(short iEntity, Vector vecOffset, short iTarget)
{
	if (iTarget)
	{
		CClientHWFire *pFire = new CClientHWFire;

		g_pObjectHandler << pFire;
		pFire->m_iEntity = iEntity;
		pFire->m_vecOffset = vecOffset;
		pFire->m_iTarget = iTarget;
		return;
	}

	CClientObject *pObject = NULL;

	while ((pObject = g_pObjectHandler.FindEntityByClassname(pObject, "hwgrunt_fire")) != NULL)
	{
		CClientHWFire *pFire = (CClientHWFire *) pObject;

		if (pFire->m_iEntity == iEntity)
			pFire->TurnOff();
	}
}
