#ifndef CLIENTSPRITE_H
#define CLIENTSPRITE_H

#include "clientobject.h"

// RampSprite message fields that follow the flags
#define RAMPSPRITE_ANGLE		( 1<<0 )
#define RAMPSPRITE_SCALE		( 1<<1 )
#define RAMPSPRITE_ALPHA		( 1<<2 )
#define RAMPSPRITE_RENDER		( 1<<3 )
#define RAMPSPRITE_RED			( 1<<4 )
#define RAMPSPRITE_GREEN		( 1<<5 )
#define RAMPSPRITE_BLUE			( 1<<6 )
#define RAMPSPRITE_FRAME		( 1<<7 )
#define RAMPSPRITE_VELOCITY		( 1<<8 )
#define RAMPSPRITE_ANGLERATE	( 1<<9 )
#define RAMPSPRITE_SCALERATE	( 1<<10 )
#define RAMPSPRITE_ALPHARATE	( 1<<11 )
#define RAMPSPRITE_REDRATE		( 1<<12 )
#define RAMPSPRITE_GREENRATE	( 1<<13 )
#define RAMPSPRITE_BLUERATE		( 1<<14 )
#define RAMPSPRITE_FRAMERATE	( 1<<15 )

struct tempent_s;

class CClientSprite : public CClientObject
{
public:
	void Spawn(void);
	void RemoveSprite(void);
	void ChangeSprite(const char *pszSprite, Vector vecOrigin, float flScale, int iRenderMode,
		int iRenderFx, int iRenderAmt, Vector vecColor, float flLife, float flFrameRate, int iFlags);
	tempent_s *GetSprite(void);

	tempent_s	*m_pSprite;
};

class CClientSpriteRamper : public CClientObject
{
public:
	void Spawn(void);
	void Die(void);
	void Init(int iModel, float flLife, Vector vecOrigin, float flAngle, float flScale, float flAlpha,
		int iRenderMode, int iRenderFx, int r, int g, int b, int iFrame, Vector vecVelocity,
		float flAngleRate, float flScaleRate, float flAlphaRate, float flRedRate, float flGreenRate,
		float flBlueRate, int iFrameRate);
	void EffectThink(void);

	float		m_flAngleRate;
	float		m_flScaleRate;
	float		m_flAlphaRate;
	float		m_flRedRate;
	float		m_flGreenRate;
	float		m_flBlueRate;
	float		m_flDieTime;
	float		m_flLastTime;
	tempent_s	*m_pSprite;
};

void TE_RampSprite(int iModel, float flLife, Vector vecOrigin, float flAngle, float flScale, float flAlpha,
	int iRenderMode, int iRenderFx, int r, int g, int b, int iFrame, Vector vecVelocity,
	float flAngleRate, float flScaleRate, float flAlphaRate, float flRedRate, float flGreenRate,
	float flBlueRate, int iFrameRate);

void TE_ShieldRic(Vector &vecOrigin);

int __MsgFunc_RampSprite(const char *pszName, int iSize, void *pbuf);
int __MsgFunc_ShieldRic(const char *pszName, int iSize, void *pbuf);

#endif
