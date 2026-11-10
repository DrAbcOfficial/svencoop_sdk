#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

DECLARE_MESSAGE(m_Custom, ToggleElem)
DECLARE_MESSAGE(m_Custom, CustSpr)
DECLARE_MESSAGE(m_Custom, NumDisplay)
DECLARE_MESSAGE(m_Custom, UpdateNum)
DECLARE_MESSAGE(m_Custom, TimeDisplay)
DECLARE_MESSAGE(m_Custom, UpdateTime)

wrect_t READ_RECT(void)
{
	wrect_t rect;

	rect.left = READ_BYTE();
	rect.top = READ_BYTE();
	rect.right = rect.left + READ_SHORT();
	rect.bottom = rect.top + READ_SHORT();

	return rect;
}

RGBA READ_RGBA(void)
{
	RGBA color;

	color.r = READ_BYTE();
	color.g = READ_BYTE();
	color.b = READ_BYTE();
	color.a = READ_BYTE();

	return color;
}

bool CHudBaseElement::Init(void)
{
	m_bVisible = false;
	m_flFade = 0;
	return true;
}

bool CHudBaseElement::Setup(void)
{
	m_flStartTime = 0;
	m_bVisible = !(m_iFlags & HUD_ELEM_HIDDEN);
	return true;
}

bool CHudBaseElement::Draw(float time)
{
	if (!m_bVisible)
		return false;

	if (m_flStartTime == 0)
		m_flStartTime = time;

	if ((m_iFlags & HUD_ELEM_DYNAMIC_ALPHA) && m_flFade > 0)
		gHUD.UpdateFade(m_flFade);

	return true;
}

int CHudBaseElement::GetFinalPosX(void)
{
	return gHUD.CalcXPosition(m_flX, GetWidth(), (m_iFlags & HUD_ELEM_ABSOLUTE_X) != 0,
		(m_iFlags & HUD_ELEM_SCR_CENTER_X) != 0, (m_iFlags & HUD_ELEM_NO_BORDER) != 0);
}

int CHudBaseElement::GetFinalPosY(void)
{
	return gHUD.CalcYPosition(m_flY, GetHeight(), (m_iFlags & HUD_ELEM_ABSOLUTE_Y) != 0,
		(m_iFlags & HUD_ELEM_SCR_CENTER_Y) != 0, (m_iFlags & HUD_ELEM_NO_BORDER) != 0);
}

RGBA CHudBaseElement::GetColor(float time)
{
	float elapsed = time - m_flStartTime;

	if ((m_iFlags & HUD_ELEM_EFFECT_ONCE) && elapsed >= m_flFXTime)
		elapsed = m_flFXTime - m_flFXTime * 0.001;

	RGBA color = gHUD.GetEffectColor(m_iEffect, m_Color, m_Color2, m_flFXTime, elapsed);

	if (m_flFadeIn > 0 && elapsed <= m_flFadeIn)
		color.a *= elapsed / m_flFadeIn;
	else if (m_flHoldTime >= 0 && elapsed > m_flFadeIn + m_flHoldTime)
	{
		float fadeTime = elapsed - (m_flFadeIn + m_flHoldTime);

		if (m_flFadeOut > 0 && fadeTime <= m_flFadeOut)
			color.a *= 1.0f - fadeTime / m_flFadeOut;
		else
		{
			color.a = 0;
			m_bVisible = false;
		}
	}

	if (m_iFlags & HUD_ELEM_DYNAMIC_ALPHA)
		color.a *= gHUD.GetFadeAlpha(m_flFade) / 255.0f;
	else if (m_iFlags & HUD_ELEM_DEFAULT_ALPHA)
		color.a *= gHUD.GetDefaultAlpha() / 255.0f;

	ScaleColors(color);
	return color;
}

int CHudCustom::Init(void)
{
	HOOK_MESSAGE(ToggleElem);
	HOOK_MESSAGE(CustSpr);
	HOOK_MESSAGE(NumDisplay);
	HOOK_MESSAGE(UpdateNum);
	HOOK_MESSAGE(TimeDisplay);
	HOOK_MESSAGE(UpdateTime);

	memset(m_pChannels, 0, sizeof(m_pChannels));
	gHUD.AddHudElem(this);
	return 1;
}

int CHudCustom::VidInit(void)
{
	ClearAll();
	return 1;
}

void CHudCustom::Reset(void)
{
}

void CHudCustom::Shutdown(void)
{
	ClearAll();
}

int CHudCustom::Draw(float time)
{
	if (!(gHUD.m_iHideHUDDisplay & HIDEHUD_CUSTOM_MAPS))
		DrawChannelGroup(0, time);

	if (!(gHUD.m_iHideHUDDisplay & HIDEHUD_CUSTOM_PLUGINS))
		DrawChannelGroup(1, time);

	return 1;
}

void CHudCustom::DrawChannelGroup(int group, float time)
{
	if (!(gHUD.m_pCvarHideCustom->value != group + 1 && gHUD.m_pCvarHideCustom->value <= 2))
		return;

	for (int i = group * HUD_CHANNELS_PER_GROUP; i < (group + 1) * HUD_CHANNELS_PER_GROUP; i++)
	{
		if (m_pChannels[i] && m_pChannels[i]->m_bVisible)
			m_pChannels[i]->Draw(time);
	}
}

CHudBaseElement *CHudCustom::ElementFactory(HUD_ELEMENT type)
{
	if (gHUD.m_pCvarDebug && gHUD.m_pCvarDebug->value > 0 && CVAR_GET_FLOAT("developer") >= 1)
		gEngfuncs.Con_Printf("HUD: Creating new element (type %d)\n", type);

	switch (type)
	{
	case HUD_ELEMENT_SPRITE:
		return new CHudCustomSprite();
	case HUD_ELEMENT_NUMBER:
		return new CHudNumDisplay();
	case HUD_ELEMENT_TIME:
		return new CHudTimeDisplay();
	}

	return NULL;
}

CHudBaseElement *CHudCustom::GetChannel(byte channel)
{
	if (channel >= MAX_HUD_CHANNELS)
	{
		if (CVAR_GET_FLOAT("developer") >= 1)
			gEngfuncs.Con_Printf("HUD: Invalid channel number #%d\n", channel);

		return NULL;
	}

	return m_pChannels[channel];
}

CHudBaseElement *CHudCustom::AssignChannel(byte channel, HUD_ELEMENT type)
{
	CHudBaseElement *element = GetChannel(channel);

	if (channel >= MAX_HUD_CHANNELS)
		return NULL;

	if (element && element->GetType() != type)
	{
		ClearChannel(channel);
		element = NULL;
	}

	if (!element)
	{
		element = ElementFactory(type);
		element->Init();
		m_pChannels[channel] = element;
	}

	if (gHUD.m_pCvarDebug && gHUD.m_pCvarDebug->value > 0 && CVAR_GET_FLOAT("developer") >= 1)
		gEngfuncs.Con_Printf("HUD: Channel #%d assigned (element type %d)\n", channel, type);

	return element;
}

void CHudCustom::ClearChannel(byte channel)
{
	if (channel >= MAX_HUD_CHANNELS || !m_pChannels[channel])
		return;

	if (gHUD.m_pCvarDebug && gHUD.m_pCvarDebug->value > 0 && CVAR_GET_FLOAT("developer") >= 1)
		gEngfuncs.Con_Printf("HUD: Channel #%d cleared\n", channel);

	// Elements have no virtual destructor or owned allocations
	operator delete(m_pChannels[channel]);
	m_pChannels[channel] = NULL;
}

void CHudCustom::ClearAll(void)
{
	for (int i = 0; i < MAX_HUD_CHANNELS; i++)
		ClearChannel(i);
}

int CHudCustom::MsgFunc_ToggleElem(const char *name, int size, void *buffer)
{
	BEGIN_READ(buffer, size);
	CHudBaseElement *element = GetChannel(READ_BYTE());

	if (element)
		element->m_bVisible = READ_BYTE() != 0;

	return 1;
}

int CHudCustom::MsgFunc_CustSpr(const char *name, int size, void *buffer)
{
	BEGIN_READ(buffer, size);
	CHudCustomSprite *element = (CHudCustomSprite *)AssignChannel(READ_BYTE(), HUD_ELEMENT_SPRITE);

	if (!element)
		return 1;

	element->m_iFlags = READ_LONG();
	const char *sprite = READ_STRING();
	element->m_Rect = READ_RECT();
	element->m_flX = READ_FLOAT();
	element->m_flY = READ_FLOAT();
	element->m_Color = READ_RGBA();
	element->m_Color2 = READ_RGBA();
	element->m_iStartFrame = READ_BYTE();
	element->m_iNumFrames = READ_BYTE();
	element->m_flFrameRate = READ_FLOAT();
	element->m_flFadeIn = READ_FLOAT();
	element->m_flFadeOut = READ_FLOAT();
	element->m_flHoldTime = READ_FLOAT();
	element->m_flFXTime = READ_FLOAT();
	element->m_iEffect = READ_BYTE();

	element->m_hSprite = gHUD.LoadSprite(sprite, element->m_Rect);
	element->Setup();
	m_iFlags |= HUD_ACTIVE;
	return 1;
}

int CHudCustom::MsgFunc_NumDisplay(const char *name, int size, void *buffer)
{
	BEGIN_READ(buffer, size);
	CHudNumDisplay *element = (CHudNumDisplay *)AssignChannel(READ_BYTE(), HUD_ELEMENT_NUMBER);

	if (!element)
		return 1;

	element->m_iFlags = READ_LONG();
	float value = READ_FLOAT();

	if ((element->m_iFlags & HUD_ELEM_DYNAMIC_ALPHA) && value != element->m_flValue)
		element->m_flFade = FADE_TIME;

	element->m_flValue = value;
	element->m_iMinDigits = READ_BYTE();
	element->m_iMaxDigits = READ_BYTE();
	element->m_flX = READ_FLOAT();
	element->m_flY = READ_FLOAT();
	element->m_Color = READ_RGBA();
	element->m_Color2 = READ_RGBA();
	const char *sprite = READ_STRING();
	element->m_Rect = READ_RECT();
	element->m_flFadeIn = READ_FLOAT();
	element->m_flFadeOut = READ_FLOAT();
	element->m_flHoldTime = READ_FLOAT();
	element->m_flFXTime = READ_FLOAT();
	element->m_iEffect = READ_BYTE();

	element->m_hSprite = gHUD.LoadSprite(sprite, element->m_Rect);
	element->Setup();
	m_iFlags |= HUD_ACTIVE;
	return 1;
}

int CHudCustom::MsgFunc_UpdateNum(const char *name, int size, void *buffer)
{
	BEGIN_READ(buffer, size);
	CHudBaseElement *base = GetChannel(READ_BYTE());

	if (base && base->GetType() == HUD_ELEMENT_NUMBER)
	{
		CHudNumDisplay *element = (CHudNumDisplay *)base;
		float value = READ_FLOAT();

		if ((element->m_iFlags & HUD_ELEM_DYNAMIC_ALPHA) && value != element->m_flValue)
			element->m_flFade = FADE_TIME;

		element->m_flValue = value;
	}

	return 1;
}

int CHudCustom::MsgFunc_TimeDisplay(const char *name, int size, void *buffer)
{
	BEGIN_READ(buffer, size);
	CHudTimeDisplay *element = (CHudTimeDisplay *)AssignChannel(READ_BYTE(), HUD_ELEMENT_TIME);

	if (!element)
		return 1;

	element->m_iFlags = READ_LONG();
	element->m_flUpdateTime = READ_FLOAT();
	element->m_flValue = READ_FLOAT();

	if (element->m_iFlags & HUD_ELEM_DYNAMIC_ALPHA)
		element->m_flFade = FADE_TIME;

	element->m_flX = READ_FLOAT();
	element->m_flY = READ_FLOAT();
	element->m_Color = READ_RGBA();
	element->m_Color2 = READ_RGBA();
	const char *sprite = READ_STRING();
	element->m_Rect = READ_RECT();
	element->m_flFadeIn = READ_FLOAT();
	element->m_flFadeOut = READ_FLOAT();
	element->m_flHoldTime = READ_FLOAT();
	element->m_flFXTime = READ_FLOAT();
	element->m_iEffect = READ_BYTE();

	element->m_hSprite = gHUD.LoadSprite(sprite, element->m_Rect);
	element->Setup();
	m_iFlags |= HUD_ACTIVE;
	return 1;
}

int CHudCustom::MsgFunc_UpdateTime(const char *name, int size, void *buffer)
{
	BEGIN_READ(buffer, size);
	CHudBaseElement *base = GetChannel(READ_BYTE());

	if (base && base->GetType() == HUD_ELEMENT_TIME)
	{
		CHudTimeDisplay *element = (CHudTimeDisplay *)base;

		element->m_flUpdateTime = READ_FLOAT();
		element->m_flValue = READ_FLOAT();

		if (element->m_iFlags & HUD_ELEM_DYNAMIC_ALPHA)
			element->m_flFade = FADE_TIME;
	}

	return 1;
}
