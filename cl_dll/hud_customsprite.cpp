#include "hud.h"
#include "cl_util.h"

bool CHudCustomSprite::Init(void)
{
	CHudBaseElement::Init();
	return true;
}

bool CHudCustomSprite::Setup(void)
{
	CHudBaseElement::Setup();
	m_DrawMode = GetDrawMode();
	gHUD.CheckGetSpriteSize(m_hSprite, m_iStartFrame, m_Rect);
	return true;
}

SPR_MODE CHudCustomSprite::GetDrawMode(void)
{
	if (m_iFlags & HUD_SPR_OPAQUE)
		return SPR_NORMAL;

	if (m_iFlags & HUD_SPR_MASKED)
		return SPR_TRANSPARENT;

	return SPR_ADDITIVE;
}

bool CHudCustomSprite::Draw(float time)
{
	if (!CHudBaseElement::Draw(time))
		return false;

	int x = GetFinalPosX();
	int y = GetFinalPosY();
	byte frame = m_iStartFrame;

	if (m_iNumFrames > 1 && m_flFrameRate > 0)
		frame += (int)((time - m_flStartTime) * m_flFrameRate) % m_iNumFrames;

	RGBA color = GetColor(time);

	if (!color.a)
		return false;

	gHUD.DrawSprite(x, y, m_hSprite, &m_Rect, color.r, color.g, color.b, frame, m_DrawMode);
	return true;
}

HUD_ELEMENT CHudCustomSprite::GetType(void)
{
	return HUD_ELEMENT_SPRITE;
}

int CHudCustomSprite::GetWidth(void)
{
	return m_Rect.right - m_Rect.left;
}

int CHudCustomSprite::GetHeight(void)
{
	return m_Rect.bottom - m_Rect.top;
}
