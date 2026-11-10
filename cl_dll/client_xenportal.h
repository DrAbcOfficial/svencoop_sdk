#ifndef CLIENT_XENPORTAL_H
#define CLIENT_XENPORTAL_H

#include "clientobject.h"

struct beam_s;

class CClientXenBeam : public CClientObject
{
public:
	virtual void Spawn(void);

	void KillBeam(void);
	void SetBeam(beam_s *pBeam);
	void SetAlpha(int iAlpha);
	void SetColor(Vector vecColor);
	beam_s *GetBeam(void);

	static Vector GetEndPos(const Vector &vecStart, int iRadius, int &iFailed);
	static CClientXenBeam *CreateXenBeam(const char *pszModel, Vector vecStart, Vector vecEnd,
		float flLife, float flAmplitude, float flWidth, Vector vecColor, int iBrightness,
		float flSpeed, float flFrameRate);

	beam_s	*m_pBeam;
};

class CClientXenPortal : public CClientObject
{
public:
	CClientXenPortal();

	virtual void Spawn(void);

	void Start(void);
	void Middle(void);
	void End(void);

	int		m_iBeamCount;
	Vector	m_vecBeamColor;
	int		m_iBeamBrightness;
	float	m_flBeamRadius;
	Vector	m_vecLightColor;
	float	m_flLightRadius;
	Vector	m_vecSpriteColor;
	float	m_flSpriteScale;
	float	m_flSpriteFrameRate;
	int		m_iSpriteBrightness;
	Vector	m_vecFlareColor;
	float	m_flFlareScale;
	float	m_flFlareFrameRate;
	int		m_iFlareLife;
};

void TE_CreateXenPortal(void);

#endif
