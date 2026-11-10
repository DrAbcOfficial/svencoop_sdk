#include "hud.h"
#include "cl_util.h"
#include "clientsprite.h"
#include "r_efx.h"
#include "parsemsg.h"

float anglemod(float a);

void CClientSprite::Spawn(void)
{
	m_pszClassname = "client_sprite";
	m_pSprite = NULL;
	m_bKill = false;
}

void CClientSprite::RemoveSprite(void)
{
	if (m_pSprite)
		m_pSprite->die = Time();

	Kill();
}

void CClientSprite::ChangeSprite(const char *pszSprite, Vector vecOrigin, float flScale, int iRenderMode,
	int iRenderFx, int iRenderAmt, Vector vecColor, float flLife, float flFrameRate, int iFlags)
{
	if (m_pSprite && !pszSprite)
		return;

	int iModel = LookupModel(pszSprite);

	if (!iModel)
	{
		if (gEngfuncs.pfnGetCvarFloat("developer") >= 1)
			gEngfuncs.Con_Printf("Could not create sprite with \"%s\", no such file!\n", pszSprite);

		Kill();
		return;
	}

	Vector vecVelocity(0, 0, 0);

	m_pSprite = gEngfuncs.pEfxAPI->R_TempSprite(vecOrigin, vecVelocity, flScale, iModel,
		iRenderMode, iRenderFx, (float)iRenderAmt, flLife + flLife, iFlags);

	if (m_pSprite)
		m_pSprite->entity.curstate.framerate = flFrameRate;

	SetThink((void (CClientObject::*)(void))&CClientSprite::RemoveSprite);
	m_flNextThink = Time() + flLife;
}

tempent_s *CClientSprite::GetSprite(void)
{
	return m_pSprite;
}

void CClientSpriteRamper::Spawn(void)
{
	m_pszClassname = "client_sprite_ramper";
	m_pSprite = NULL;
}

void CClientSpriteRamper::Die(void)
{
	if (m_pSprite)
		m_pSprite->die = Time();

	Kill();
}

void CClientSpriteRamper::Init(int iModel, float flLife, Vector vecOrigin, float flAngle, float flScale, float flAlpha,
	int iRenderMode, int iRenderFx, int r, int g, int b, int iFrame, Vector vecVelocity,
	float flAngleRate, float flScaleRate, float flAlphaRate, float flRedRate, float flGreenRate,
	float flBlueRate, int iFrameRate)
{
	Vector vecZero(0, 0, 0);

	m_pSprite = gEngfuncs.pEfxAPI->R_TempSprite(vecOrigin, vecZero, flScale, iModel,
		iRenderMode, iRenderFx, flAlpha, flLife + flLife,
		FTENT_SPRANIMATE | FTENT_PERSIST | FTENT_SPRANIMATELOOP);

	if (!m_pSprite)
	{
		Die();
		return;
	}

	m_pSprite->entity.angles = Vector(0.0f, 0.0f, flAngle);
	m_pSprite->entity.curstate.velocity = vecVelocity;
	m_pSprite->entity.curstate.rendercolor.r = r;
	m_pSprite->entity.curstate.rendercolor.g = g;
	m_pSprite->entity.curstate.rendercolor.b = b;
	m_pSprite->entity.curstate.frame = iFrame;
	m_pSprite->entity.curstate.framerate = iFrameRate;

	m_flAngleRate = flAngleRate;
	m_flScaleRate = flScaleRate;
	m_flAlphaRate = flAlphaRate;
	m_flRedRate = flRedRate;
	m_flGreenRate = flGreenRate;
	m_flBlueRate = flBlueRate;
	m_flDieTime = Time() + flLife;

	SetThink((void (CClientObject::*)(void))&CClientSpriteRamper::EffectThink);
	m_flNextThink = m_flLastTime = Time();
}

void CClientSpriteRamper::EffectThink(void)
{
	if (Time() > m_flDieTime)
	{
		Die();
		return;
	}

	float flTime = Time();
	float flDelta = flTime - m_flLastTime;

	m_pSprite->entity.curstate.scale += flDelta * m_flScaleRate;

	if (m_pSprite->entity.curstate.scale < 0.001)
	{
		m_pSprite->entity.curstate.scale = 0.001f;
		Die();
		return;
	}

	m_pSprite->entity.curstate.renderamt = (int)(m_flAlphaRate * 255.0f * flDelta + m_pSprite->entity.curstate.renderamt);

	if (m_pSprite->entity.curstate.renderamt < 0)
	{
		m_pSprite->entity.curstate.renderamt = 0;
		Die();
		return;
	}

	if (m_pSprite->entity.curstate.renderamt > 255)
	{
		m_pSprite->entity.curstate.renderamt = 255;
		m_flAlphaRate = 0;
	}

	m_pSprite->entity.curstate.rendercolor.r = (int)(flDelta * m_flRedRate + m_pSprite->entity.curstate.rendercolor.r);
	m_pSprite->entity.curstate.rendercolor.g = (int)(flDelta * m_flGreenRate + m_pSprite->entity.curstate.rendercolor.g);
	m_pSprite->entity.curstate.rendercolor.b = (int)(flDelta * m_flBlueRate + m_pSprite->entity.curstate.rendercolor.b);
	m_pSprite->entity.angles.z = anglemod(flDelta * m_flAngleRate + m_pSprite->entity.angles.z);
	m_flNextThink = m_flLastTime = flTime;
}

void TE_RampSprite(int iModel, float flLife, Vector vecOrigin, float flAngle, float flScale, float flAlpha,
	int iRenderMode, int iRenderFx, int r, int g, int b, int iFrame, Vector vecVelocity,
	float flAngleRate, float flScaleRate, float flAlphaRate, float flRedRate, float flGreenRate,
	float flBlueRate, int iFrameRate)
{
	CClientSpriteRamper *pSprite = new CClientSpriteRamper;

	g_pObjectHandler << pSprite;
	pSprite->Init(iModel, flLife, vecOrigin, flAngle, flScale, flAlpha, iRenderMode, iRenderFx,
		r, g, b, iFrame, vecVelocity, flAngleRate, flScaleRate, flAlphaRate, flRedRate,
		flGreenRate, flBlueRate, iFrameRate);
}

int __MsgFunc_RampSprite(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int iModel = READ_SHORT();
	float flLife = READ_BYTE() * 0.1;
	Vector vecOrigin;

	vecOrigin.x = READ_COORD();
	vecOrigin.y = READ_COORD();
	vecOrigin.z = READ_COORD();

	unsigned short iFlags = READ_SHORT();
	float flAngle = (iFlags & RAMPSPRITE_ANGLE) ? READ_BYTE() * (360.0 / 255.0) : 0;
	float flScale = (iFlags & RAMPSPRITE_SCALE) ? READ_BYTE() * 0.1 : 1;
	float flAlpha = (iFlags & RAMPSPRITE_ALPHA) ? READ_BYTE() * (1.0 / 255.0) : 1;
	int iRenderMode = kRenderTransAdd;
	int iRenderFx = kRenderFxNone;

	if (iFlags & RAMPSPRITE_RENDER)
	{
		int iRender = READ_BYTE();

		iRenderMode = (iRender >> 5) & 7;
		iRenderFx = iRender & 0x1F;
	}

	int r = (iFlags & RAMPSPRITE_RED) ? READ_BYTE() : 255;
	int g = (iFlags & RAMPSPRITE_GREEN) ? READ_BYTE() : 255;
	int b = (iFlags & RAMPSPRITE_BLUE) ? READ_BYTE() : 255;
	int iFrame = (iFlags & RAMPSPRITE_FRAME) ? READ_BYTE() : 0;
	Vector vecVelocity(0, 0, 0);

	if (iFlags & RAMPSPRITE_VELOCITY)
	{
		vecVelocity.x = READ_COORD();
		vecVelocity.y = READ_COORD();
		vecVelocity.z = READ_COORD();
	}

	float flAngleRate = (iFlags & RAMPSPRITE_ANGLERATE) ? READ_BYTE() * 6 : 0;
	float flScaleRate = (iFlags & RAMPSPRITE_SCALERATE) ? (signed char)READ_BYTE() * (20.0 / 255.0) : 0;
	float flAlphaRate = (iFlags & RAMPSPRITE_ALPHARATE) ? (signed char)READ_BYTE() * (20.0 / 255.0) : 0;
	float flRedRate = (iFlags & RAMPSPRITE_REDRATE) ? (signed char)READ_BYTE() * (20.0 / 255.0) : 0;
	float flGreenRate = (iFlags & RAMPSPRITE_GREENRATE) ? (signed char)READ_BYTE() * (20.0 / 255.0) : 0;
	float flBlueRate = (iFlags & RAMPSPRITE_BLUERATE) ? (signed char)READ_BYTE() * (20.0 / 255.0) : 0;
	int iFrameRate = (iFlags & RAMPSPRITE_FRAMERATE) ? READ_BYTE() : 0;

	TE_RampSprite(iModel, flLife, vecOrigin, flAngle, flScale, flAlpha, iRenderMode, iRenderFx,
		r, g, b, iFrame, vecVelocity, flAngleRate, flScaleRate, flAlphaRate, flRedRate,
		flGreenRate, flBlueRate, iFrameRate);
	return 1;
}

void TE_ShieldRic(Vector &vecOrigin)
{
	CClientSpriteRamper *pSprite = new CClientSpriteRamper;

	g_pObjectHandler << pSprite;

	int iModel = pSprite->LookupModel("sprites/muz4.spr");

	pSprite->Init(iModel, 0.3f, vecOrigin, 0, 0.3f, 1, kRenderGlow, kRenderFxNoDissipation,
		100, 100, 255, 0, Vector(0, 0, 0), 0, 4, -1.0f / 0.3f, 0, 0, 0, 0);
}

int __MsgFunc_ShieldRic(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	Vector vecOrigin;

	vecOrigin.x = READ_COORD();
	vecOrigin.y = READ_COORD();
	vecOrigin.z = READ_COORD();
	TE_ShieldRic(vecOrigin);
	return 1;
}
