#ifndef CLIENT_EFFECTS_H
#define CLIENT_EFFECTS_H

#include "clientobject.h"

#define SRPRIMED_ENTITY_MASK	0x1F

class CSRPrimedFX : public CClientObject
{
public:
	virtual void Spawn(void);

	void TurnOn(int iEntity, float flStartTime);
	void TurnOff(void);
	void EffectThink(void);

	int		m_iEntity;
	float	m_flStartTime;
	float	m_flEndTime;
};

void TE_TurnOnSRPrimedFX(int iEntity, float flStartTime);
void TE_TurnOffSRPrimedFX(int iEntity);

int __MsgFunc_SRPrimed(const char *pszName, int iSize, void *pbuf);
int __MsgFunc_SRPrimedOff(const char *pszName, int iSize, void *pbuf);
int __MsgFunc_CreateBlood(const char *pszName, int iSize, void *pbuf);
int __MsgFunc_ShkFlash(const char *pszName, int iSize, void *pbuf);
int __MsgFunc_SRDetonate(const char *pszName, int iSize, void *pbuf);
int __MsgFunc_ToxicCloud(const char *pszName, int iSize, void *pbuf);

#endif
