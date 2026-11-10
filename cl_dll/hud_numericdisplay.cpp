#include "hud.h"
#include "cl_util.h"

bool CHudNumDisplay::Init(void)
{
	CHudBaseElement::Init();
	return true;
}

int CHudNumDisplay::TransformFlags(int hudFlags)
{
	int flags = 0;

	if (hudFlags & HUD_NUM_LEADING_ZEROS)
		flags |= DN_LEADINGZEROS;

	if (hudFlags & HUD_NUM_NEGATIVE_NUMBERS)
		flags |= DN_NEGATIVE;

	if (hudFlags & HUD_NUM_PLUS_SIGN)
		flags |= DN_PLUSSIGN;

	return flags;
}

bool CHudNumDisplay::Setup(void)
{
	CHudBaseElement::Setup();
	m_iDrawFlags = TransformFlags(m_iFlags);
	gHUD.CheckGetSpriteSize(m_hSprite, 0, m_Rect);
	return true;
}

bool CHudNumDisplay::Draw(float time)
{
	if (!CHudBaseElement::Draw(time))
		return false;

	if ((m_iFlags & HUD_NUM_DONT_DRAW_ZERO) && m_flValue == 0)
		return false;

	int x = GetFinalPosX();
	int y = GetFinalPosY();
	RGBA color = GetColor(time);

	if (!color.a)
		return false;

	int iconOffset = (m_Rect.bottom - m_Rect.top - gHUD.m_iFontHeight) >> 1;

	if (m_iFlags & HUD_NUM_RIGHT_ALIGN)
	{
		if (m_iFlags & HUD_NUM_SEPARATOR)
			x += gHUD.DrawSeparator(x, y, color.r, color.g, color.b, color.a) + gHUD.GetSpaceWidth();

		x = gHUD.DrawNumber(x, y, (int)m_flValue, color.r, color.g, color.b,
			m_iMinDigits, m_iMaxDigits, m_iDrawFlags);

		if (m_hSprite >= 0)
			gHUD.DrawSprite(x + 4, y - iconOffset, m_hSprite, &m_Rect, color.r, color.g, color.b);
	}
	else
	{
		if (m_hSprite >= 0)
			x = gHUD.DrawSprite(x, y - iconOffset, m_hSprite, &m_Rect, color.r, color.g, color.b) + 4;

		x = gHUD.DrawNumber(x, y, (int)m_flValue, color.r, color.g, color.b,
			m_iMinDigits, m_iMaxDigits, m_iDrawFlags);

		if (m_iFlags & HUD_NUM_SEPARATOR)
			gHUD.DrawSeparator(x + gHUD.GetSpaceWidth(), y, color.r, color.g, color.b, color.a);
	}

	return true;
}

HUD_ELEMENT CHudNumDisplay::GetType(void)
{
	return HUD_ELEMENT_NUMBER;
}

int CHudNumDisplay::GetWidth(void)
{
	int width = m_hSprite >= 0 ? m_Rect.right - m_Rect.left + 4 : 0;

	width += gHUD.GetNumberWidth((int)m_flValue, m_iMinDigits, m_iMaxDigits, m_iDrawFlags);

	if (m_iFlags & HUD_NUM_SEPARATOR)
		width += gHUD.GetSpaceWidth() + gHUD.GetSeparatorWidth();

	return width;
}

int CHudNumDisplay::GetHeight(void)
{
	return gHUD.m_iFontHeight;
}

bool CHudTimeDisplay::Init(void)
{
	CHudBaseElement::Init();
	m_iColon = gHUD.GetSpriteIndex("colon");
	m_iDot = gHUD.GetSpriteIndex("dot");

	wrect_t &rect = gHUD.GetSpriteRect(m_iColon);

	m_iColonWidth = rect.right - rect.left;
	m_iColonHeight = rect.bottom - rect.top;
	return true;
}

bool CHudTimeDisplay::Setup(void)
{
	CHudBaseElement::Setup();

	if ((m_iFlags & (HUD_TIME_HOURS | HUD_TIME_SECONDS)) == (HUD_TIME_HOURS | HUD_TIME_SECONDS))
		m_iFlags |= HUD_TIME_MINUTES;

	if ((m_iFlags & (HUD_TIME_MINUTES | HUD_TIME_MILLISECONDS)) == (HUD_TIME_MINUTES | HUD_TIME_MILLISECONDS))
		m_iFlags |= HUD_TIME_SECONDS;

	gHUD.CheckGetSpriteSize(m_hSprite, 0, m_Rect);
	return true;
}

void CHudTimeDisplay::UpdateTime(float time)
{
	float value = m_flValue;

	if (!(m_iFlags & HUD_TIME_FREEZE))
		value += (time - m_flUpdateTime) * ((m_iFlags & HUD_TIME_COUNT_DOWN) ? -1.0f : 1.0f);

	m_iHours = (int)value / 3600;
	m_iMinutes = (int)(value / 60.0f) % 60;
	m_iSeconds = (int)value % 60;
	m_iMilliseconds = (int)(value * 1000.0f) % 1000;
}

bool CHudTimeDisplay::Draw(float time)
{
	if (!CHudBaseElement::Draw(time))
		return false;

	UpdateTime(time);

	int x = GetFinalPosX();
	int y = GetFinalPosY();
	RGBA color = GetColor(time);

	if (!color.a)
		return false;

	int iconOffset = (m_Rect.bottom - m_Rect.top - gHUD.m_iFontHeight) >> 1;
	int colonOffset = (m_iColonHeight - gHUD.m_iFontHeight) >> 1;
	bool first = true;

	if (!(m_iFlags & HUD_TIME_RIGHT_ALIGN) && m_hSprite >= 0)
		x = gHUD.DrawSprite(x, y - iconOffset, m_hSprite, &m_Rect, color.r, color.g, color.b) + 4;

	if (((m_iFlags & HUD_TIME_HOURS) && m_iHours > 0) || (m_iFlags & HUD_TIME_ZERO_HOURS))
	{
		x = gHUD.DrawNumber(x, y, m_iHours, color.r, color.g, color.b, 2, 4, 0);
		first = false;
	}

	if (m_iFlags & HUD_TIME_MINUTES)
	{
		if (!first)
			x = gHUD.DrawSprite(x, y - colonOffset, gHUD.GetSprite(m_iColon),
				&gHUD.GetSpriteRect(m_iColon), color.r, color.g, color.b);

		x = gHUD.DrawNumber(x, y, m_iMinutes, color.r, color.g, color.b, 2, 2, first ? 0 : DN_LEADINGZEROS);
		first = false;
	}

	if (m_iFlags & HUD_TIME_SECONDS)
	{
		if (!first)
			x = gHUD.DrawSprite(x, y - colonOffset, gHUD.GetSprite(m_iColon),
					&gHUD.GetSpriteRect(m_iColon), color.r, color.g, color.b);

		x = gHUD.DrawNumber(x, y, m_iSeconds, color.r, color.g, color.b, 2, 2, first ? 0 : DN_LEADINGZEROS);
		first = false;
	}

	if (m_iFlags & HUD_TIME_MILLISECONDS)
	{
		if (!first)
			x = gHUD.DrawSprite(x, y - colonOffset, gHUD.GetSprite(m_iDot),
					&gHUD.GetSpriteRect(m_iDot), color.r, color.g, color.b);

		x = gHUD.DrawNumber(x, y, m_iMilliseconds, color.r, color.g, color.b, 3, 3, first ? 0 : DN_LEADINGZEROS);
	}

	if ((m_iFlags & HUD_TIME_RIGHT_ALIGN) && m_hSprite >= 0)
		gHUD.DrawSprite(x + 4, y - iconOffset, m_hSprite, &m_Rect, color.r, color.g, color.b);

	return true;
}

HUD_ELEMENT CHudTimeDisplay::GetType(void)
{
	return HUD_ELEMENT_TIME;
}

int CHudTimeDisplay::GetWidth(void)
{
	int width = m_hSprite >= 0 ? m_Rect.right - m_Rect.left + 4 : 0;
	bool first = true;

	if (((m_iFlags & HUD_TIME_HOURS) && m_iHours > 0) || (m_iFlags & HUD_TIME_ZERO_HOURS))
	{
		width += gHUD.GetNumberWidth(m_iHours, 2, 4, 0);
		first = false;
	}

	if (m_iFlags & HUD_TIME_MINUTES)
	{
		if (!first)
			width += m_iColonWidth;

		width += gHUD.GetNumberWidth(m_iMinutes, 2, 2, 0);
		first = false;
	}

	if (m_iFlags & HUD_TIME_SECONDS)
	{
		if (!first)
			width += m_iColonWidth;

		width += gHUD.GetNumberWidth(m_iSeconds, 2, 2, 0);
		first = false;
	}

	if (m_iFlags & HUD_TIME_MILLISECONDS)
	{
		if (!first)
			width += m_iColonWidth;

		width += gHUD.GetNumberWidth(m_iMilliseconds, 3, 3, 0);
	}

	return width;
}

int CHudTimeDisplay::GetHeight(void)
{
	return gHUD.m_iFontHeight;
}
