#include "hud.h"
#include "cl_util.h"
#include "camera.h"

extern bool cam_thirdperson_clipped;

int CHudCrosshair::Init(void)
{
	m_hSprite = 0;
	gHUD.AddHudElem(this);
	m_iFlags |= HUD_ACTIVE;
	m_flScreenX = 0;
	m_flScreenY = 0;
	return 1;
}

int CHudCrosshair::VidInit(void)
{
	return 1;
}

void CHudCrosshair::Reset(void)
{
	ClearSprite();
}

int CHudCrosshair::Draw(float flTime)
{
	if (m_hSprite && (int)g_pCrosshair->value > 0)
	{
		int x = ScreenWidth >> 1;
		int y = ScreenHeight >> 1;

		if (cam_thirdperson && !cam_thirdperson_clipped)
		{
			x = (int)m_flScreenX;
			y = (int)m_flScreenY;
		}

		x -= (m_rcSprite.right - m_rcSprite.left) >> 1;
		y -= (m_rcSprite.bottom - m_rcSprite.top) >> 1;
		SPR_Set(m_hSprite, m_iR, m_iG, m_iB);
		SPR_DrawHoles(0, x, y, &m_rcSprite);
		DRAW_DEBUG_CROSS(ScreenWidth >> 1, ScreenHeight >> 1);
	}

	return 1;
}

void CHudCrosshair::SetPosition(float x, float y)
{
	m_flScreenX = (x + 1.0f) * ScreenWidth * 0.5f;
	m_flScreenY = (1.0f - y) * ScreenHeight * 0.5f;
}

void CHudCrosshair::SetSprite(HSPRITE hSprite, wrect_t rect, int r, int g, int b)
{
	m_hSprite = hSprite;
	m_rcSprite = rect;
	m_iR = r;
	m_iG = g;
	m_iB = b;
}

void CHudCrosshair::ClearSprite(void)
{
	wrect_t rect = {0, 0, 0, 0};

	SetSprite(0, rect, 0, 0, 0);
}
