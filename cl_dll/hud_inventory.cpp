#include "hud.h"
#include "cl_util.h"

int CHudInventoryIcon::Init(void)
{
	m_hCustomSprite = 0;
	m_hSprite = 0;
	Reset();

	gHUD.AddHudElem(this);
	m_iFlags |= HUD_ACTIVE;

	return 1;
}

int CHudInventoryIcon::VidInit(void)
{
	int index = gHUD.GetSpriteIndex("item_inventory");

	m_hSprite = gHUD.GetSprite(index);
	m_rcSprite = gHUD.GetSpriteRect(index);
	m_hCustomSprite = 0;

	return 1;
}

void CHudInventoryIcon::Reset(void)
{
	m_bVisible = false;
	m_iCount = 0;
}

int CHudInventoryIcon::GetWidth(void)
{
	return m_rcSprite.right - m_rcSprite.left;
}

int CHudInventoryIcon::GetHeight(void)
{
	return m_rcSprite.bottom - m_rcSprite.top;
}

int CHudInventoryIcon::GetAlpha(void)
{
	int maxAlpha = gHUD.GetMaxAlpha();
	int defaultAlpha = gHUD.GetDefaultAlpha();

	return (maxAlpha - defaultAlpha) / 2 + gHUD.GetDefaultAlpha();
}

void CHudInventoryIcon::Visible(bool visible)
{
	m_bVisible = visible;
}

void CHudInventoryIcon::SetCount(int count)
{
	m_iCount = count;
}

void CHudInventoryIcon::SetIcon(const char *name)
{
	if (name && *name)
	{
		m_rcCustomSprite.left = 0;
		m_rcCustomSprite.top = 0;
		m_rcCustomSprite.right = GetWidth();
		m_rcCustomSprite.bottom = GetHeight();
		m_hCustomSprite = gHUD.LoadSprite(name, m_rcCustomSprite);
	}
	else
	{
		m_hCustomSprite = -1;
	}
}

int CHudInventoryIcon::Draw(float flTime)
{
	if (gHUD.m_iWeaponBits >= 0 || !m_bVisible)
		return 0;

	int r = 100, g = 130, b = 200;
	int width = GetWidth();
	int height = GetHeight();
	int x = ScreenWidth - width - gHUD.GetBorderSize() - 8;
	int y = (ScreenHeight >> 1) - height;
	HSPRITE sprite = m_hCustomSprite > 0 ? m_hCustomSprite : m_hSprite;
	wrect_t *rect = m_hCustomSprite > 0 ? &m_rcCustomSprite : &m_rcSprite;

	ScaleColors(r, g, b, GetAlpha());
	gHUD.DrawSprite(x, y, sprite, rect, r, g, b, 0, SPR_ADDITIVE);

	if (m_iCount > 1)
	{
		char text[128];
		float scale = (float)(ScreenWidth / 720);

		x = (int)((float)ScreenWidth - ((float)(width + gHUD.GetBorderSize()) + (scale + scale) * 4.0f));
		y = (int)((double)((ScreenHeight >> 1) - height) + (double)height * 1.005);

		if (m_iCount > 99)
			UTIL_FormatString(text, "> %d", 99);
		else
			UTIL_FormatString(text, "x %d", m_iCount);

		gHUD.DrawString(x, y, ScreenWidth - 4, text, r, g, b, 0);
	}

	return 1;
}
