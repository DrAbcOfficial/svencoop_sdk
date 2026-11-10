#ifndef HUD_CROSSHAIR_H
#define HUD_CROSSHAIR_H

class CHudCrosshair : public CHudBase
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	void Reset(void);
	void SetPosition(float x, float y);
	void SetSprite(HSPRITE hSprite, wrect_t rect, int r, int g, int b);
	void ClearSprite(void);

private:
	HSPRITE m_hSprite;
	wrect_t m_rcSprite;
	int m_iR;
	int m_iG;
	int m_iB;
	float m_flScreenX;
	float m_flScreenY;
};

extern cvar_t *g_pCrosshair;

#endif
