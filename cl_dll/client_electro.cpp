#include "extdll.h"
#include "hud_iface.h"
#include "client_electro.h"
#include "client_sound.h"
#include "cl_entity.h"
#include "dlight.h"
#include "parsemsg.h"

void AlertMessage(ALERT_TYPE type, char *format, ...);

void CClientDLight::Spawn(void)
{
	m_pszClassname = "client_dlight";
	m_pLight = NULL;
	m_bKill = false;
}

void CClientDLight::ChangeDLight(Vector vecOrigin, float flRadius, Vector vecColor,
	float flLife, float flDecay, float flMinLight, bool bDark)
{
	if (!m_pLight)
		m_pLight = gEngfuncs.pEfxAPI->CL_AllocDlight(0);

	if (!m_pLight)
	{
		AlertMessage(at_console, "Error, could not allocate new dlight\n");
		Kill();
		return;
	}

	m_pLight->origin = vecOrigin;
	m_pLight->radius = flRadius;
	m_pLight->color.r = (int) vecColor.x;
	m_pLight->color.g = (int) vecColor.y;
	m_pLight->color.b = (int) vecColor.z;
	m_pLight->die = Time() + flLife * 2;
	m_pLight->decay = flDecay;
	m_pLight->minlight = flMinLight;
	m_pLight->dark = bDark;
	SetThink((void (CClientObject::*)(void)) &CClientDLight::RemoveLight);
	m_flNextThink = Time() + flLife;
}

void CClientDLight::RemoveLight(void)
{
	if (m_pLight)
		m_pLight->die = Time();

	Kill();
}

dlight_t *CClientDLight::GetDLight(void)
{
	return m_pLight;
}

void CElectroEffect::Spawn(void)
{
	m_pszClassname = "electro_effect";
	m_bKill = false;
	SetThink((void (CClientObject::*)(void)) &CElectroEffect::EffectThink);
	m_flNextThink = Time();
	m_iEntity = 0;
	m_pLight = new CClientDLight;
	g_pObjectHandler << m_pLight;
	m_pLight->ChangeDLight(Vector(0, 0, 0), 0, Vector(0, 0, 0), 512, 0, 0, false);
	m_flNextSound = (float)(Time() + 1.0);
	m_bPlayStartSound = true;
}

void CElectroEffect::Remove(void)
{
	TurnOff();
}

void CElectroEffect::TurnOff(void)
{
	m_pLight->RemoveLight();

	cl_entity_t *pEntity = gEngfuncs.GetEntityByIndex(m_iEntity);

	if (pEntity)
	{
		pEntity->curstate.renderfx = kRenderFxNone;
		pEntity->curstate.rendermode = kRenderNormal;
		pEntity->curstate.renderamt = 0;
		pEntity->curstate.rendercolor.r = 0;
		pEntity->curstate.rendercolor.g = 0;
		pEntity->curstate.rendercolor.b = 0;

		StopSound(CHAN_ITEM, "weapons/cbe/cbe_idle1.wav", pEntity->index);
		PlaySound("weapons/cbe/cbe_off.wav", 1, pEntity->origin, CHAN_ITEM, PITCH_NORM, ATTN_NORM, 0,
			pEntity->index);
	}

	m_iEntity = 0;
	Kill();
}

void CElectroEffect::EffectThink(void)
{
	if (!m_iEntity)
	{
		TurnOff();
		return;
	}

	cl_entity_t *pEntity = gEngfuncs.GetEntityByIndex(m_iEntity);

	if (!pEntity)
	{
		TurnOff();
		return;
	}

	if (m_bPlayStartSound)
	{
		PlaySound("weapons/cbe/cbe_on.wav", 1, pEntity->origin, CHAN_ITEM, PITCH_NORM, ATTN_NORM, 0,
			pEntity->index);
		m_bPlayStartSound = false;
	}

	if (Time() >= m_flNextSound)
	{
		StopSound(CHAN_ITEM, "weapons/cbe/cbe_idle1.wav", pEntity->index);
		PlaySound("weapons/cbe/cbe_idle1.wav", 0.7f, pEntity->origin, CHAN_ITEM, PITCH_NORM, ATTN_NORM, 0,
			pEntity->index);
		m_flNextSound = Time() + 2;
	}

	pEntity->curstate.renderfx = kRenderFxGlowShell;
	pEntity->curstate.rendercolor.r = 80;
	pEntity->curstate.rendercolor.g = 100;
	pEntity->curstate.rendercolor.b = 220;
	m_pLight->ChangeDLight(pEntity->origin, 128, Vector(80, 100, 220), 256, 0, 0, false);
	m_flNextThink = Time();
}

void TE_TurnOnElectroFX(int iEntity)
{
	CElectroEffect *pEffect = new CElectroEffect;

	g_pObjectHandler << pEffect;
	pEffect->m_iEntity = iEntity;
}

void TE_TurnOffElectroFX(int iEntity)
{
	CClientObject *pObject = NULL;

	while ((pObject = g_pObjectHandler.FindEntityByClassname(pObject, "electro_effect")) != NULL)
	{
		CElectroEffect *pEffect = (CElectroEffect *) pObject;

		if (pEffect->m_iEntity == iEntity)
			pEffect->TurnOff();
	}
}

bool TE_CheckElectroFX(int iEntity)
{
	CClientObject *pObject = NULL;

	while ((pObject = g_pObjectHandler.FindEntityByClassname(pObject, "electro_effect")) != NULL)
	{
		if (((CElectroEffect *) pObject)->m_iEntity == iEntity)
			return true;
	}

	return false;
}

void DisableElectricEffect(cl_entity_t *pEntity)
{
	pEntity->curstate.renderfx = kRenderFxNone;
	pEntity->curstate.rendermode = kRenderNormal;
	pEntity->curstate.renderamt = 0;
	pEntity->curstate.rendercolor.r = 0;
	pEntity->curstate.rendercolor.g = 0;
	pEntity->curstate.rendercolor.b = 0;
	pEntity->curstate.effects &= ~EF_BRIGHTLIGHT;
}

void ToggleElectricEffect(bool bEnable, int iEntity)
{
	cl_entity_t *pViewModel = gEngfuncs.GetViewModel();
	cl_entity_t *pPlayer = gEngfuncs.GetLocalPlayer();

	if (bEnable)
	{
		if (pPlayer->index == iEntity)
		{
			pViewModel->curstate.effects |= EF_BRIGHTLIGHT;
			pViewModel->curstate.renderfx = kRenderFxGlowShell;
			pViewModel->curstate.rendermode = kRenderNormal;
			pViewModel->curstate.renderamt = 0;
			pViewModel->curstate.rendercolor.r = 50;
			pViewModel->curstate.rendercolor.g = 75;
			pViewModel->curstate.rendercolor.b = 220;
		}

		TE_TurnOnElectroFX(iEntity);
	}
	else
	{
		if (pPlayer->index == iEntity)
			DisableElectricEffect(pViewModel);

		TE_TurnOffElectroFX(iEntity);
	}
}

int __MsgFunc_CbElec(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int iFlags = READ_BYTE();

	ToggleElectricEffect((iFlags & CBELEC_ON) != 0, iFlags & CBELEC_ENTITY_MASK);
	return 1;
}
