#include "hud.h"
#include "cl_util.h"
#include "client_effects.h"
#include "parsemsg.h"
#include "r_efx.h"
#include "dlight.h"
#include "event_api.h"
#include "client_sound.h"
#include "particle_engine.h"
#include "particle_system.h"

void CSRPrimedFX::Spawn(void)
{
	m_pszClassname = "shrifle_primed_effect";
	m_bKill = false;
	m_iEntity = 0;
}

void CSRPrimedFX::TurnOn(int iEntity, float flStartTime)
{
	if (!iEntity)
		return;

	m_flStartTime = flStartTime;
	m_iEntity = iEntity;
	m_flEndTime = flStartTime + 5.0f;
	SetThink((void (CClientObject::*)(void)) &CSRPrimedFX::EffectThink);
	m_flNextThink = Time();
}

void CSRPrimedFX::TurnOff(void)
{
	if (gEngfuncs.GetEntityByIndex(m_iEntity))
	{
		cl_entity_t *pViewModel = gEngfuncs.GetViewModel();

		pViewModel->curstate.renderfx = kRenderFxNone;
		pViewModel->curstate.renderamt = 255;
		pViewModel->curstate.rendercolor.r = 0;
		pViewModel->curstate.rendercolor.g = 0;
		pViewModel->curstate.rendercolor.b = 0;
	}

	Kill();
}

void CSRPrimedFX::EffectThink(void)
{
	if (!m_iEntity || !gEngfuncs.GetEntityByIndex(m_iEntity))
	{
		TurnOff();
		return;
	}

	float flFraction = (Time() - m_flStartTime) / (m_flEndTime - m_flStartTime);

	if (flFraction > 1)
		flFraction = 1;

	cl_entity_t *pViewModel = gEngfuncs.GetViewModel();

	pViewModel->curstate.renderfx = kRenderFxGlowShell;
	pViewModel->curstate.renderamt = (int)(flFraction * 20);
	pViewModel->curstate.rendercolor.b = (int)(flFraction * 220);
	pViewModel->curstate.rendercolor.r = (int)(flFraction * 50);
	pViewModel->curstate.rendercolor.g = (int)(flFraction * 50);
	m_flNextThink = Time();
}

void TE_TurnOnSRPrimedFX(int iEntity, float flStartTime)
{
	CSRPrimedFX *pEffect = new CSRPrimedFX;

	g_pObjectHandler << pEffect;
	pEffect->TurnOn(iEntity, flStartTime);
}

void TE_TurnOffSRPrimedFX(int iEntity)
{
	CClientObject *pObject = NULL;

	while ((pObject = g_pObjectHandler.FindEntityByClassname(pObject, "shrifle_primed_effect")) != NULL)
	{
		CSRPrimedFX *pEffect = (CSRPrimedFX *) pObject;

		if (pEffect->m_iEntity == iEntity)
			pEffect->TurnOff();
	}
}

int __MsgFunc_SRPrimed(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int iEntity = READ_BYTE() & SRPRIMED_ENTITY_MASK;
	float flStartTime = READ_FLOAT();

	TE_TurnOnSRPrimedFX(iEntity, flStartTime);
	return 1;
}

int __MsgFunc_SRPrimedOff(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	TE_TurnOffSRPrimedFX(READ_BYTE() & SRPRIMED_ENTITY_MASK);
	return 1;
}

int __MsgFunc_CreateBlood(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	Vector vecOrigin;

	vecOrigin.x = READ_COORD();
	vecOrigin.y = READ_COORD();
	vecOrigin.z = READ_COORD();

	int iColor = READ_BYTE();
	int iScale = READ_BYTE();
	int iModelIndex = 0;

	gEngfuncs.CL_LoadModel("sprites/bloodspray.spr", &iModelIndex);

	short iSprayModel = iModelIndex;

	iModelIndex = 0;
	gEngfuncs.CL_LoadModel("sprites/blood.spr", &iModelIndex);

	short iBloodModel = iModelIndex;

	gEngfuncs.pEfxAPI->R_BloodSprite(vecOrigin, iColor, iSprayModel, iBloodModel, (float) iScale);
	return 1;
}

int __MsgFunc_ShkFlash(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	Vector vecOrigin;

	vecOrigin.x = READ_COORD();
	vecOrigin.y = READ_COORD();
	vecOrigin.z = READ_COORD();

	const char *pszSample;

	if (READ_BYTE())
	{
		dlight_t *pLight = gEngfuncs.pEfxAPI->CL_AllocDlight(0);

		pLight->origin = vecOrigin;
		pLight->color.r = 85;
		pLight->color.g = 200;
		pLight->color.b = 255;
		pLight->radius = 96;
		pLight->decay = 96;
		pLight->die = gEngfuncs.GetClientTime() + 0.5;

		pLight = gEngfuncs.pEfxAPI->CL_AllocDlight(0);
		pLight->origin = vecOrigin;
		pLight->color.r = 255;
		pLight->color.g = 255;
		pLight->color.b = 255;
		pLight->radius = 48;
		pLight->decay = 32;
		pLight->die = gEngfuncs.GetClientTime() + 1.0;

		pszSample = "weapons/shock_impact.wav";
	}
	else
	{
		dlight_t *pLight = gEngfuncs.pEfxAPI->CL_AllocElight(0);

		pLight->origin = vecOrigin;
		pLight->color.r = 85;
		pLight->color.g = 200;
		pLight->color.b = 255;
		pLight->radius = 24;
		pLight->decay = 24;
		pLight->die = gEngfuncs.GetClientTime() + 0.2;

		pszSample = "shocktrooper/shock_fire.wav";
	}

	CClient_SoundEngine::GetSoundEngine()->PlayFMODSound(SOUNDMSG_ORIGIN, -1, vecOrigin, CHAN_AUTO,
		pszSample, 1, ATTN_NORM, 0, PITCH_NORM, -1, 0);
	return 1;
}

int __MsgFunc_SRDetonate(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	Vector vecOrigin;

	vecOrigin.x = READ_COORD();
	vecOrigin.y = READ_COORD();
	vecOrigin.z = READ_COORD();

	int iCharge = READ_BYTE();
	float flRadius = 2 * iCharge + 32;
	int iBeams = (int)(iCharge / 20.0f) + 3;
	float flMaxWidth = iCharge / 10.0f + 1.0f;

	CClient_SoundEngine::GetSoundEngine()->PlayFMODSound(SOUNDMSG_ORIGIN, -1, vecOrigin, CHAN_AUTO,
		"weapons/shock_discharge2.wav", 1, ATTN_NORM, 0, PITCH_NORM, -1, 0);

	dlight_t *pLight = gEngfuncs.pEfxAPI->CL_AllocDlight(0);

	pLight->origin = vecOrigin;
	pLight->color.r = 85;
	pLight->color.g = 200;
	pLight->color.b = 255;
	pLight->decay = 500;
	pLight->radius = (int)flRadius;
	pLight->die = gEngfuncs.GetClientTime() + 1.0;

	for (int i = 0; i < iBeams; i++)
	{
		float flWidth = gEngfuncs.pfnRandomFloat(1.0f, flMaxWidth);
		Vector vecEnd;

		vecEnd.x = vecOrigin.x + gEngfuncs.pfnRandomFloat(-flRadius, flRadius);
		vecEnd.y = vecOrigin.y + gEngfuncs.pfnRandomFloat(-flRadius, flRadius);
		vecEnd.z = vecOrigin.z + gEngfuncs.pfnRandomFloat(-flRadius, flRadius) * 0.5;

		gEngfuncs.pEfxAPI->R_BeamPoints(vecOrigin, vecEnd,
			gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/lgtning.spr"), 0.25f, flWidth,
			flRadius / 200 + 0.5f, 1.0f, 0, 0, 0, 0.37647059f, 0.7078355f, 1.0f);
	}

	gEngfuncs.pEfxAPI->R_TempSprite(vecOrigin, vec3_origin, flRadius / 40,
		gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/blueflare1.spr"), kRenderGlow,
		kRenderFxNoDissipation, 0.75f, 0.15f, 0);
	gEngfuncs.pEfxAPI->R_StreakSplash(vecOrigin, vec3_origin, 72, 40 * iBeams, flRadius * 4, -400, 400);
	return 1;
}

int __MsgFunc_ToxicCloud(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	Vector vecOrigin;

	vecOrigin.x = READ_COORD();
	vecOrigin.y = READ_COORD();
	vecOrigin.z = READ_COORD();

	CParticleSystem *pSystem = ParticleEngine()->SystemCreate();

	pSystem->m_flDieTime = gEngfuncs.GetClientTime() + 5.0;
	pSystem->m_vecOrigin = vecOrigin;
	pSystem->m_vecDirection = Vector(0, 0, 1);
	pSystem->m_flNextSpawn = gEngfuncs.GetClientTime();
	pSystem->m_flSpawnInterval = 0.1f;
	pSystem->m_iMaxParticles = 50;
	pSystem->SystemStartup(PSYS_TINTED_SMOKE, NULL);

	CClient_SoundEngine::GetSoundEngine()->PlayFMODSound(SOUNDMSG_ORIGIN, -1, vecOrigin, CHAN_AUTO,
		"doors/aliendoor1.wav", 1, ATTN_NORM, 0, PITCH_NORM, -1, 0);
	CClient_SoundEngine::GetSoundEngine()->PlayFMODSound(SOUNDMSG_ORIGIN, -1, vecOrigin, CHAN_AUTO,
		"ambience/disgusting.wav", 1, ATTN_NORM, 0, PITCH_NORM, -1, 0);
	return 1;
}
