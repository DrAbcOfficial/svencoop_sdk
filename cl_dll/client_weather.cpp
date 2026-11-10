#include "hud.h"
#include "cl_util.h"
#include "client_weather.h"
#include "cl_entity.h"
#include "pm_defs.h"
#include "r_efx.h"

#include <math.h>

#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

void ClientWeatherSpriteHitCallback(tempent_s *pTemp, pmtrace_s *pTrace)
{
	if (!pTemp || !pTemp->entity.curstate.iuser2)
		return;

	Vector vecVelocity(0, 0, 0);

	TEMPENTITY *pSplash = gEngfuncs.pEfxAPI->R_TempSprite(pTemp->entity.origin, vecVelocity,
		pTemp->entity.curstate.fuser2, pTemp->entity.curstate.iuser2,
		pTemp->entity.curstate.rendermode, pTemp->entity.curstate.renderfx,
		(float)pTemp->entity.curstate.renderamt, 1.0f, FTENT_SPRANIMATE);

	if (!pSplash)
		return;

	pSplash->entity.curstate.rendercolor.r = pTemp->entity.curstate.rendercolor.r;
	pSplash->entity.curstate.rendercolor.g = pTemp->entity.curstate.rendercolor.g;
	pSplash->entity.curstate.rendercolor.b = pTemp->entity.curstate.rendercolor.b;
	pSplash->bounceFactor = 0;
	pSplash->entity.curstate.gravity = 0;

	if (pTemp->entity.curstate.iuser3 > 0)
	{
		pSplash->entity.curstate.effects |= EF_SPRITE_CUSTOM_VP;
		pSplash->entity.curstate.sequence = pTemp->entity.curstate.iuser3 - 1;
	}
}

void CClientWeather::Spawn(void)
{
}

void CClientWeather::Init(void)
{
	m_flFrequency = 1.0f / V_min(V_max(m_flFrequency, 1.0f), 200.0f);

	m_vecHalfSize = m_vecMaxs - m_vecMins;
	m_flVolume = m_vecHalfSize.x * m_vecHalfSize.y * m_vecHalfSize.z;
	m_vecHalfSize = m_vecHalfSize * 0.5;
	m_vecCenter = m_vecMins + m_vecHalfSize;

	m_pszClassname = "weather_effect";
	m_bKill = false;
	TurnOn();
}

void CClientWeather::TurnOn(void)
{
	SetThink((void (CClientObject::*)(void)) &CClientWeather::WeatherThink);
}

void CClientWeather::TurnOff(void)
{
	SetThink(NULL);
}

void CClientWeather::WeatherThink(void)
{
	SetThink((void (CClientObject::*)(void)) &CClientWeather::WeatherThink);
	m_flNextThink = Time() + m_flFrequency;

	Vector vecAngles;

	vecAngles.x = m_vecAngles.x + gEngfuncs.pfnRandomFloat(-m_flAngleVariance, m_flAngleVariance);
	vecAngles.y = m_vecAngles.y + gEngfuncs.pfnRandomFloat(-m_flAngleVariance, m_flAngleVariance);
	vecAngles.z = m_vecAngles.z + gEngfuncs.pfnRandomFloat(-m_flAngleVariance, m_flAngleVariance);

	Vector vecDirection(0, 0, 0);

	AngleVectors(vecAngles, vecDirection, NULL, NULL);
	vecDirection = vecDirection.Normalize();

	// Spawn on the face of the volume that the sprites travel away from
	Vector vecOffset;

	vecOffset.x = gEngfuncs.pfnRandomFloat(-m_vecHalfSize.x, m_vecHalfSize.x);
	vecOffset.y = gEngfuncs.pfnRandomFloat(-m_vecHalfSize.y, m_vecHalfSize.y);
	vecOffset.z = gEngfuncs.pfnRandomFloat(-m_vecHalfSize.z, m_vecHalfSize.z);

	if (acos(DotProduct(vecDirection, Vector(0, 0, 1))) <= M_PI / 4)
		vecOffset.z = -m_vecHalfSize.z;
	else if (acos(DotProduct(vecDirection, Vector(0, 0, -1))) <= M_PI / 4)
		vecOffset.z = m_vecHalfSize.z;
	else if (acos(DotProduct(vecDirection, Vector(0, 1, 0))) <= M_PI / 4)
		vecOffset.y = -m_vecHalfSize.y;
	else if (acos(DotProduct(vecDirection, Vector(0, -1, 0))) <= M_PI / 4)
		vecOffset.y = m_vecHalfSize.y;
	else if (acos(DotProduct(vecDirection, Vector(1, 0, 0))) <= M_PI / 4)
		vecOffset.x = -m_vecHalfSize.x;
	else if (acos(DotProduct(vecDirection, Vector(-1, 0, 0))) <= M_PI / 4)
		vecOffset.x = m_vecHalfSize.x;

	Vector vecOrigin = m_vecCenter + vecOffset;
	float flSpeed = m_flSpeed + gEngfuncs.pfnRandomFloat(-m_flSpeedVariance, m_flSpeedVariance);
	Vector vecVelocity = vecDirection * flSpeed;
	Vector vecTravel(m_vecHalfSize.x * 2 * vecDirection.x, m_vecHalfSize.y * 2 * vecDirection.y,
		m_vecHalfSize.z * 2 * vecDirection.z);

	TEMPENTITY *pSprite = gEngfuncs.pEfxAPI->R_TempSprite(vecOrigin, vecVelocity, m_flScale,
		m_iModel, m_iRenderMode, m_iRenderFx, m_flRenderAmt, vecTravel.Length() / flSpeed,
		FTENT_SPRCYCLE | FTENT_COLLIDEALL | FTENT_COLLIDEKILL | FTENT_SPRANIMATELOOP);

	if (!pSprite)
		return;

	pSprite->entity.curstate.rendercolor.r = (int)m_vecRenderColor.x;
	pSprite->entity.curstate.rendercolor.g = (int)m_vecRenderColor.y;
	pSprite->entity.curstate.rendercolor.b = (int)m_vecRenderColor.z;
	pSprite->bounceFactor = 0;
	pSprite->entity.curstate.gravity = 0;
	pSprite->entity.curstate.velocity = vecDirection * flSpeed;

	if (m_iVPType > 0)
	{
		pSprite->entity.curstate.effects |= EF_SPRITE_CUSTOM_VP;
		pSprite->entity.curstate.sequence = m_iVPType - 1;
	}

	if (m_iSplashModel)
	{
		pSprite->entity.curstate.iuser2 = m_iSplashModel;
		pSprite->entity.curstate.fuser2 = m_flSplashScale;
		pSprite->entity.curstate.iuser3 = m_iSplashVPType;
		pSprite->hitcallback = ClientWeatherSpriteHitCallback;
	}
}

void TE_CreateWeatherEffect(int iWeatherID, Vector vecMins, Vector vecMaxs, Vector vecAngles,
	int iModel, float flScale, int iVPType, int iSplashModel, float flSplashScale, int iSplashVPType,
	int iRenderMode, float flRenderAmt, Vector vecRenderColor, int iRenderFx, float flFrequency,
	float flSpeed, float flAngleVariance, float flSpeedVariance)
{
	// Only looks up an existing effect: nothing creates CClientWeather objects any more
	CClientObject *pObject = NULL;

	while ((pObject = g_pObjectHandler.FindEntityByClassname(pObject, "weather_effect")) != NULL)
	{
		if (((CClientWeather *) pObject)->m_iWeatherID == iWeatherID)
			break;
	}
}
