#include "hud.h"
#include "cl_util.h"
#include "client_sporetrail.h"
#include "parsemsg.h"
#include "r_efx.h"

void CSporeTrail::Spawn(void)
{
	m_pszClassname = "sporetrail";
	m_bKill = false;

	int iGlowModel = LookupModel("sprites/glow01.spr");

	m_iSpitModel = LookupModel("sprites/pitdronespit.spr");
	m_iExplosionModel = LookupModel("sprites/spore_exp_01.spr");
	m_iExplosionCenterModel = LookupModel("sprites/spore_exp_c_01.spr");

	Vector vecOrigin(0, 0, 0), vecVelocity(0, 0, 0);

	m_pSprite = gEngfuncs.pEfxAPI->R_TempSprite(vecOrigin, vecVelocity, 0.7f,
		iGlowModel, kRenderTransAdd, kRenderFxDistort, 150, 500, 0);

	if (m_pSprite)
	{
		m_pSprite->entity.curstate.framerate = 1;
		m_pSprite->entity.curstate.rendercolor.r = 200;
		m_pSprite->entity.curstate.rendercolor.g = 190;
		m_pSprite->entity.curstate.rendercolor.b = 95;
		m_pSprite->entity.curstate.scale = 0.7f;
		m_pSprite->clientIndex = m_iEntity;
		m_pSprite->flags |= FTENT_PLYRATTACHMENT;
	}

	SetThink((void (CClientObject::*)(void)) &CSporeTrail::EffectThink);
	m_flNextSpray = Time();
	m_flNextThink = m_flNextSpray;
}

void CSporeTrail::EffectThink(void)
{
	if (!m_iEntity)
	{
		TurnOff(false);
		return;
	}

	m_pEntity = gEngfuncs.GetEntityByIndex(m_iEntity);

	cl_entity_t *pPlayer = gEngfuncs.GetLocalPlayer();

	if (!m_pEntity || !pPlayer || m_pEntity->curstate.messagenum < pPlayer->curstate.messagenum)
	{
		TurnOff(false);
		return;
	}

	Vector vecDirection;
	Vector vecVelocity(m_pEntity->curstate.velocity);

	if (vecVelocity.Length() <= 1)
	{
		vecDirection = Vector(0, 0, 1);
	}
	else
	{
		Vector vecForward, vecRight, vecUp;

		AngleVectors(m_pEntity->curstate.angles, vecForward, vecRight, vecUp);
		vecDirection = vecUp - vecForward;
	}

	if (Time() > m_flNextSpray)
	{
		gEngfuncs.pEfxAPI->R_Sprite_Spray(m_pEntity->origin, vecDirection,
			m_iSpitModel, 1, 35, 50);
		m_flNextSpray = Time() + 0.05;
	}

	m_flNextThink = Time();
}

void CSporeTrail::TurnOff(bool bDetonate)
{
	Kill();

	if (m_pSprite)
		m_pSprite->die = Time();

	m_pEntity = gEngfuncs.GetEntityByIndex(m_iEntity);

	if (!m_pEntity || !bDetonate)
		return;

	Vector vecDirection(0, 0, 1);

	gEngfuncs.pEfxAPI->R_Sprite_Spray(m_pEntity->origin, vecDirection,
		m_iSpitModel, 100, 150, 200);

	TEMPENTITY *pExplosion = NULL;
	Vector vecVelocity(0, 0, 0);

	switch (gEngfuncs.pfnRandomLong(0, 1))
	{
	case 0:
		pExplosion = gEngfuncs.pEfxAPI->R_TempSprite(m_pEntity->origin, vecVelocity, 3,
			m_iExplosionModel, kRenderTransAdd, kRenderFxNoDissipation, 255, 1, FTENT_SPRANIMATE);
		break;

	case 1:
		pExplosion = gEngfuncs.pEfxAPI->R_TempSprite(m_pEntity->origin, vecVelocity, 3,
			m_iExplosionCenterModel, kRenderTransAdd, kRenderFxNoDissipation, 255, 1, FTENT_SPRANIMATE);
		break;
	}

	if (pExplosion)
	{
		pExplosion->entity.curstate.rendercolor.r = 200;
		pExplosion->entity.curstate.rendercolor.g = 190;
		pExplosion->entity.curstate.rendercolor.b = 95;
	}

	vecDirection = Vector(0, 0, -1);
	gEngfuncs.pEfxAPI->R_Sprite_Spray(m_pEntity->origin, vecDirection,
		m_iSpitModel, 100, 150, 200);
}

void TE_SporeTrail(int iEntity, bool bEnabled)
{
	if (bEnabled)
	{
		CSporeTrail *pTrail = new CSporeTrail;

		pTrail->m_iEntity = iEntity;
		g_pObjectHandler << pTrail;
		return;
	}

	bool bDetonate = true;
	CClientObject *pObject = NULL;

	while ((pObject = g_pObjectHandler.FindEntityByClassname(pObject, "sporetrail")) != NULL)
	{
		CSporeTrail *pTrail = (CSporeTrail *) pObject;

		if (pTrail->m_iEntity == iEntity)
		{
			pTrail->TurnOff(bDetonate);
			bDetonate = false;
		}
	}
}

int __MsgFunc_SporeTrail(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int iEntity = READ_SHORT();
	bool bEnabled = READ_BYTE() != 0;

	TE_SporeTrail(iEntity, bEnabled);
	return 1;
}

int __MsgFunc_GargSplash(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	Vector vecOrigin, vecDirection;

	vecOrigin.x = READ_COORD();
	vecOrigin.y = READ_COORD();
	vecOrigin.z = READ_COORD();
	vecDirection.x = fabs(READ_COORD());
	vecDirection.y = fabs(READ_COORD());
	vecDirection.z = fabs(READ_COORD());

	gEngfuncs.pEfxAPI->R_StreakSplash(vecOrigin, vecDirection, 6, 20, 50, 0, 400);
	return 1;
}
