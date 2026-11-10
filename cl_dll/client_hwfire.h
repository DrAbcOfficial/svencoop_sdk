#ifndef CLIENT_HWFIRE_H
#define CLIENT_HWFIRE_H

#include "clientobject.h"

class CClientHWFire : public CClientObject
{
public:
	CClientHWFire();

	virtual void Spawn(void);

	void EffectThink(void);
	void TurnOff(void);

	short	m_iEntity;
	Vector	m_vecOffset;
	short	m_iTarget;
};

void TE_CreateHWGEffect(short iEntity, Vector vecOffset, short iTarget);

#endif
