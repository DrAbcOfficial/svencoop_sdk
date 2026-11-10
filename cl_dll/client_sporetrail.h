#ifndef CLIENT_SPORETRAIL_H
#define CLIENT_SPORETRAIL_H

#include "clientobject.h"

struct cl_entity_s;
struct tempent_s;

class CSporeTrail : public CClientObject
{
public:
	virtual void Spawn(void);

	void EffectThink(void);
	void TurnOff(bool bDetonate);

	cl_entity_s	*m_pEntity;
	tempent_s	*m_pSprite;
	int			m_iEntity;
	short		m_iSpitModel;
	short		m_iExplosionModel;
	short		m_iExplosionCenterModel;
	float		m_flNextSpray;
};

void TE_SporeTrail(int iEntity, bool bEnabled);
int __MsgFunc_SporeTrail(const char *pszName, int iSize, void *pbuf);
int __MsgFunc_GargSplash(const char *pszName, int iSize, void *pbuf);

#endif
