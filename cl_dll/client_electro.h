#ifndef CLIENT_ELECTRO_H
#define CLIENT_ELECTRO_H

#include "clientobject.h"

// CbElec message byte
#define CBELEC_ON			0x40
#define CBELEC_ENTITY_MASK	0x1F

struct cl_entity_s;
struct dlight_s;

class CClientDLight : public CClientObject
{
public:
	virtual void Spawn(void);

	void ChangeDLight(Vector vecOrigin, float flRadius, Vector vecColor,
		float flLife, float flDecay, float flMinLight, bool bDark);
	void RemoveLight(void);
	dlight_s *GetDLight(void);

	dlight_s	*m_pLight;
};

class CElectroEffect : public CClientObject
{
public:
	virtual void Spawn(void);
	virtual void Remove(void);

	void EffectThink(void);
	void TurnOff(void);

	CClientDLight	*m_pLight;
	int				m_iEntity;
	float			m_flNextSound;
	bool			m_bPlayStartSound;
};

void TE_TurnOnElectroFX(int iEntity);
void TE_TurnOffElectroFX(int iEntity);
bool TE_CheckElectroFX(int iEntity);
void DisableElectricEffect(cl_entity_s *pEntity);
void ToggleElectricEffect(bool bEnable, int iEntity);
int __MsgFunc_CbElec(const char *pszName, int iSize, void *pbuf);

#endif
