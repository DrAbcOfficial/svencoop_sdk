#include "extdll.h"
#include "hud_iface.h"
#include "client_xenportal.h"
#include "client_electro.h"
#include "clientsprite.h"
#include "cl_entity.h"
#include "pm_defs.h"
#include "r_efx.h"
#include "parsemsg.h"

void AlertMessage(ALERT_TYPE type, char *format, ...);

void CClientXenBeam::Spawn(void)
{
	m_bKill = false;
	m_pszClassname = "client_xenbeam";
}

void CClientXenBeam::KillBeam(void)
{
	if (m_pBeam)
		m_pBeam->die = Time();

	m_pBeam = NULL;
	Kill();
}

void CClientXenBeam::SetBeam(BEAM *pBeam)
{
	m_pBeam = pBeam;
}

void CClientXenBeam::SetAlpha(int iAlpha)
{
	m_iAlpha = iAlpha;

	if (m_pBeam)
		m_pBeam->brightness = (float)iAlpha;
}

void CClientXenBeam::SetColor(Vector vecColor)
{
	m_vecColor = vecColor;

	if (m_pBeam)
	{
		m_pBeam->r = vecColor.x;
		m_pBeam->g = vecColor.y;
		m_pBeam->b = vecColor.z;
	}
}

BEAM *CClientXenBeam::GetBeam(void)
{
	return m_pBeam;
}

Vector CClientXenBeam::GetEndPos(const Vector &vecStart, int iRadius, int &iFailed)
{
	iFailed = 1;

	for (int i = 0; i < 10; i++)
	{
		Vector vecDir = Vector(gEngfuncs.pfnRandomFloat(-1, 1), gEngfuncs.pfnRandomFloat(-1, 1),
			gEngfuncs.pfnRandomFloat(-1, 1)).Normalize();
		pmtrace_t tr;

		TraceLine(vecStart, vecStart + vecDir * (float)iRadius, 1, -1, &tr);

		if (tr.fraction >= 1.0)
			continue;

		Vector vecWallDir;

		do
		{
			vecWallDir = Vector(gEngfuncs.pfnRandomFloat(-1, 1), gEngfuncs.pfnRandomFloat(-1, 1),
				gEngfuncs.pfnRandomFloat(-1, 1));
		}
		while (DotProduct(vecDir, vecWallDir) > 0);

		vecWallDir = vecWallDir.Normalize();

		pmtrace_t trWall;

		TraceLine(vecStart, vecStart + vecWallDir * (float)iRadius, 1, -1, &trWall);

		if (trWall.fraction == 1.0)
			continue;

		if ((vecStart - trWall.endpos).Length() < iRadius * 0.1)
			continue;

		TraceLine(vecStart, trWall.endpos, 1, -1, &trWall);
		iFailed = 0;
		return trWall.endpos;
	}

	return vecStart;
}

CClientXenBeam *CClientXenBeam::CreateXenBeam(const char *pszModel, Vector vecStart, Vector vecEnd,
	float flLife, float flAmplitude, float flWidth, Vector vecColor, int iBrightness,
	float flSpeed, float flFrameRate)
{
	CClientXenBeam *pBeam = new CClientXenBeam;

	g_pObjectHandler << pBeam;
	pBeam->m_iModelIndex = pBeam->LookupModel(pszModel);

	if (!pBeam->m_iModelIndex)
	{
		AlertMessage(at_console, "Could not create xen beam. \"%s\" no such model\n", pszModel);
		pBeam->Kill();
		return NULL;
	}

	pBeam->SetThink((void (CClientObject::*)(void)) &CClientXenBeam::KillBeam);
	pBeam->m_flNextThink = pBeam->Time() + flLife;

	BEAM *pEngineBeam = gEngfuncs.pEfxAPI->R_BeamPoints(vecStart, vecEnd, pBeam->m_iModelIndex,
		flLife, flWidth, flAmplitude, (float)iBrightness, flSpeed, 0, flFrameRate,
		vecColor.x, vecColor.y, vecColor.z);

	if (!pEngineBeam)
	{
		AlertMessage(at_console, "Could not create xen beam. engine function failed\n");
		pBeam->Kill();
		return NULL;
	}

	pBeam->SetBeam(pEngineBeam);
	return pBeam;
}

CClientXenPortal::CClientXenPortal() :
	m_vecBeamColor(0, 0, 0),
	m_vecLightColor(0, 0, 0),
	m_vecSpriteColor(0, 0, 0),
	m_vecFlareColor(0, 0, 0)
{
}

void CClientXenPortal::Spawn(void)
{
	m_pszClassname = "client_xen_portal";
	m_bKill = false;
	SetThink((void (CClientObject::*)(void)) &CClientXenPortal::Start);
	m_flNextThink = Time();
}

void CClientXenPortal::Start(void)
{
	for (int i = 0; i < m_iBeamCount; i++)
	{
		int iFailed = 0;
		Vector vecEnd = CClientXenBeam::GetEndPos(m_vecOrigin, (int)m_flBeamRadius, iFailed);

		if (iFailed)
			continue;

		CClientXenBeam::CreateXenBeam("sprites/lgtning.spr", m_vecOrigin, vecEnd, 1.0f, 0.64f, 1.6f,
			m_vecBeamColor, m_iBeamBrightness, 1.5f, 1.0f);
	}

	CClientDLight *pLight = new CClientDLight;

	g_pObjectHandler << pLight;
	pLight->ChangeDLight(m_vecOrigin, m_flLightRadius, m_vecLightColor, 1.0f, 0, 0, false);

	CClientSprite *pSprite = new CClientSprite;

	g_pObjectHandler << pSprite;
	pSprite->ChangeSprite("sprites/fexplo1.spr", m_vecOrigin, m_flSpriteScale, kRenderGlow,
		kRenderFxNoDissipation, m_iSpriteBrightness, m_vecSpriteColor, 1.0f, m_flSpriteFrameRate,
		FTENT_SPRANIMATE | FTENT_SPRCYCLE | FTENT_PERSIST);

	PlaySound("debris/beamstart7.wav", 1.0f, m_vecOrigin, CHAN_AUTO, PITCH_NORM, ATTN_NORM, 0, -1);

	SetThink((void (CClientObject::*)(void)) &CClientXenPortal::Middle);
	m_flNextThink = Time() + 0.5;
}

void CClientXenPortal::Middle(void)
{
	CClientSprite *pSprite = new CClientSprite;

	g_pObjectHandler << pSprite;
	pSprite->ChangeSprite("sprites/xflare1.spr", m_vecOrigin, m_flFlareScale, kRenderGlow,
		kRenderFxNoDissipation, 128, m_vecFlareColor, (float)m_iFlareLife, m_flFlareFrameRate,
		FTENT_SPRANIMATE | FTENT_PERSIST);

	SetThink((void (CClientObject::*)(void)) &CClientXenPortal::End);
	m_flNextThink = Time() + 0.5;
}

void CClientXenPortal::End(void)
{
	PlaySound("debris/beamstart2.wav", 1.0f, m_vecOrigin, CHAN_AUTO, PITCH_NORM, ATTN_NORM, 0, -1);
	Kill();
}

void TE_CreateXenPortal(void)
{
	CClientXenPortal *pPortal = new CClientXenPortal;

	g_pObjectHandler << pPortal;

	READ_VECTOR(pPortal->m_vecOrigin);
	pPortal->m_iBeamCount = READ_BYTE();
	READ_VECTOR(pPortal->m_vecBeamColor);
	pPortal->m_iBeamBrightness = READ_BYTE();
	pPortal->m_flBeamRadius = READ_COORD();
	READ_VECTOR(pPortal->m_vecLightColor);
	pPortal->m_flLightRadius = READ_COORD();
	READ_VECTOR(pPortal->m_vecSpriteColor);
	pPortal->m_flSpriteScale = READ_BYTE() / 10.0;
	pPortal->m_flSpriteFrameRate = (float)READ_BYTE();
	pPortal->m_iSpriteBrightness = READ_BYTE();
	READ_VECTOR(pPortal->m_vecFlareColor);
	pPortal->m_flFlareScale = READ_BYTE() / 10.0;
	pPortal->m_flFlareFrameRate = (float)READ_BYTE();
	pPortal->m_iFlareLife = READ_BYTE();
}
