/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
// hud_redraw.cpp
//
#include <math.h>
#include "hud.h"
#include "cl_util.h"

#include "vgui_TeamFortressViewport.h"
#include "vgui_ScorePanel.h"
#include "vgui_CameraPanel.h"
#include "client_media.h"
#include "vgui_MediaSelect.h"
#include "IClient.h"

#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

void CHud::CheckGetSpriteSize(HSPRITE sprite, int frame, wrect_t &rect)
{
	if (rect.right - rect.left <= 0)
		rect.right = SPR_Width(sprite, frame);

	if (rect.bottom - rect.top <= 0)
		rect.bottom = SPR_Height(sprite, frame);
}

void ScaleColors(RGBA &color)
{
	float scale = color.a / 255.0f;

	color.r *= scale;
	color.g *= scale;
	color.b *= scale;
}

RGBA ColorGrad(const RGBA &first, const RGBA &second, float fraction, bool reverse)
{
	float blend = reverse ? (float)(1.0 - fraction) : fraction;
	RGBA result;

	result.r = (second.r - first.r) * blend + first.r;
	result.g = (second.g - first.g) * blend + first.g;
	result.b = (second.b - first.b) * blend + first.b;
	result.a = (second.a - first.a) * blend + first.a;
	return result;
}

RGBA CHud::GetEffectColor(int effect, const RGBA &first, const RGBA &second, float period, float time)
{
	static const RGBA transparent = {0, 0, 0, 0};

	switch (effect)
	{
	case HUD_EFFECT_RAMP_UP:
	case HUD_EFFECT_RAMP_DOWN:
		return ColorGrad(first, second, (float)fmod(time, period) / period, effect == HUD_EFFECT_RAMP_DOWN);
	case HUD_EFFECT_TRIANGLE:
		return GetEffectColor((float)fmod(time, period) > period * 0.5 ? HUD_EFFECT_RAMP_DOWN : HUD_EFFECT_RAMP_UP,
			first, second, period * 0.5, time);
	case HUD_EFFECT_COSINE_UP:
	case HUD_EFFECT_COSINE_DOWN:
	{
		float phase = fmod((double)((float)fmod(time, period) / period) * M_PI - M_PI / 2, M_PI);
		float wave = sin(phase);

		// Both sine modes use the same direction
		return ColorGrad(first, second, (wave + 1.0) * 0.5, false);
	}
	case HUD_EFFECT_COSINE:
		return GetEffectColor((float)fmod(time, period) > period * 0.5 ? HUD_EFFECT_COSINE_DOWN : HUD_EFFECT_COSINE_UP,
			first, second, period * 0.5, time);
	case HUD_EFFECT_TOGGLE:
		return (float)fmod(time, period) > period * 0.5 ? second : first;
	case HUD_EFFECT_SINE_PULSE:
	{
		float phase = (float)fmod(time, period) / period;
		float wave = sin((double)(phase + phase) * M_PI);

		return ColorGrad(transparent, wave > 0 ? first : second, fabs(wave), false);
	}
	}

	return first;
}

int CHud::CalcPosition(float position, int size, int screenSize, bool absolute, bool centered, bool ignoreBorder)
{
	int border = ignoreBorder ? 0 : GetBorderSize();
	int available = screenSize - 2 * border;

	if (absolute)
	{
		if (centered)
			return (int)((float)((screenSize - size) >> 1) + position);

		return (int)((float)(position >= 0 ? border : screenSize - border - size) + position);
	}

	int result;

	if (centered)
		result = (int)((float)((screenSize - size) >> 1) + available * position);
	else if (position < 0)
		result = (int)((position + 1.0) * available + border - size);
	else
		result = (int)(available * position + border);

	if (result + size > screenSize - border)
		return screenSize - border - size;

	if (result < border)
		return border;

	return result;
}

int CHud::CalcXPosition(float position, int width, bool absolute, bool centered, bool ignoreBorder)
{
	return CalcPosition(position, width, ScreenWidth, absolute, centered, ignoreBorder);
}

int CHud::CalcYPosition(float position, int height, bool absolute, bool centered, bool ignoreBorder)
{
	return CalcPosition(position, height, ScreenHeight, absolute, centered, ignoreBorder);
}

int CHud::CalcXPositionLegacy(float position, int width)
{
	bool centered = position == -1;

	return CalcPosition(centered ? 0 : position, width, ScreenWidth, false, centered, false);
}

int CHud::CalcYPositionLegacy(float position, int height)
{
	bool centered = position == -1;

	return CalcPosition(centered ? 0 : position, height, ScreenHeight, false, centered, false);
}

HSPRITE CHud::LoadSprite(const char *name, wrect_t &rect)
{
	if (!name || !*name)
		return -1;

	if (strstr(name, ".spr"))
	{
		char path[260];

		snprintf(path, sizeof(path), "sprites/%s", name);
		return SPR_Load(path);
	}

	int index = GetSpriteIndex(name);

	if (!rect.left && !rect.top && !rect.right && !rect.bottom)
		rect = GetSpriteRect(index);

	return GetSprite(index);
}

int CHud::DrawSprite( int x, int y, HSPRITE sprite, const wrect_t *rect, int r, int g, int b, int frame, SPR_MODE mode )
{
	SPR_Set( sprite, r, g, b );
	if ( mode == SPR_NORMAL )
		SPR_Draw( frame, x, y, rect );
	else if ( mode == SPR_TRANSPARENT )
		SPR_DrawHoles( frame, x, y, rect );
	else
		SPR_DrawAdditive( frame, x, y, rect );

	DRAW_DEBUG_CROSS( x, y );
	if ( !rect )
		return x;

	DRAW_DEBUG_RECT( x, y, rect->right - rect->left, rect->bottom - rect->top );
	return x + rect->right - rect->left;
}

int CHud::GetDigitWidth( void )
{
	wrect_t &rect = GetSpriteRect( m_HUD_number_0 );
	return rect.right - rect.left;
}

int CHud::GetNumDigits( int number, int flags )
{
	if ( !number )
		return !(flags & DN_HIDEZERO);

	unsigned int value = number < 0 ? 0u - (unsigned int)number : (unsigned int)number;
	int digits = 0;
	while ( value )
	{
		digits++;
		value /= 10;
	}
	return digits;
}

bool CHud::ShowSign( int number, int flags )
{
	return ((flags & DN_NEGATIVE) && number < 0) || ((flags & DN_PLUSSIGN) && number != 0);
}

int CHud::GetSpaceWidthForDigits( int digits, int minDigits, int maxDigits, bool sign )
{
	if ( digits < minDigits )
		return (minDigits - digits) * GetDigitWidth();

	return 0;
}

int CHud::GetWidthForDigits( int digits, int minDigits, int maxDigits, bool sign )
{
	int width = (digits > maxDigits ? maxDigits : V_max( digits, minDigits )) * GetDigitWidth();
	wrect_t &rect = GetSpriteRect( m_HUD_plus );
	int signWidth = rect.right - rect.left + 4;

	if ( digits > maxDigits )
		width += signWidth;
	if ( sign )
		width += signWidth;

	return width;
}

int CHud::GetNumberWidth( int number, int minDigits, int maxDigits, int flags )
{
	int digits = GetNumDigits( number, flags );

	return GetWidthForDigits( digits, minDigits, maxDigits, ShowSign( number, flags ) );
}

int CHud::DrawNumber( int x, int y, int number, int r, int g, int b, int minDigits, int maxDigits, int flags )
{
	DRAW_DEBUG_CROSS( x, y );

	unsigned int magnitude = number < 0 ? 0u - (unsigned int)number : (unsigned int)number;
	unsigned int value = number < 0 && !(flags & DN_NEGATIVE) ? 0 : magnitude;
	unsigned int limit = (unsigned int)pow( 10.0, maxDigits ) - 1;
	value = V_min( value, limit );

	int digits[10];
	int count = 0;
	do
	{
		digits[count++] = value % 10;
		value /= 10;
	} while ( value );

	int padding = (flags & DN_LEADINGZEROS) ? V_max( minDigits - count, 0 ) : 0;
	bool sign = ShowSign( number, flags );

	x += GetSpaceWidthForDigits( count + padding, minDigits, maxDigits, sign );

	wrect_t &plus = GetSpriteRect( m_HUD_plus );
	int signY = y - ((plus.bottom - plus.top - m_iFontHeight) >> 1);
	if ( sign )
	{
		int index = number > 0 ? m_HUD_plus : m_HUD_minus;
		x = DrawSprite( x + 4, signY, GetSprite( index ), &GetSpriteRect( index ), r, g, b );
	}

	while ( padding-- > 0 )
		x = DrawSprite( x, y, GetSprite( m_HUD_number_0 ), &GetSpriteRect( m_HUD_number_0 ), r, g, b );

	while ( count )
	{
		int index = m_HUD_number_0 + digits[--count];
		x = DrawSprite( x, y, GetSprite( index ), &GetSpriteRect( index ), r, g, b );
	}

	if ( magnitude > limit )
		x = DrawSprite( x + 4, signY, GetSprite( m_HUD_plus ), &plus, r, g, b );

	return x;
}

int CHud::GetSpaceWidth( void )
{
	return GetDigitWidth() >> 1;
}

int CHud::GetBorderSize( void )
{
	return V_min( V_max( (int)m_pCvarBorderSize->value, 0 ), 100 );
}

int CHud::GetWidth( void )
{
	return ScreenWidth - 2 * GetBorderSize();
}

int CHud::GetHeight( void )
{
	return ScreenHeight - 2 * GetBorderSize();
}

int CHud::GetSeparatorWidth( void )
{
	return GetDigitWidth() / 10;
}

int CHud::DrawSeparator( int x, int y, int r, int g, int b, int a )
{
	DRAW_DEBUG_CROSS( x, y );
	int width = GetSeparatorWidth();
	FillRGBA( x, y, width, m_iFontHeight, r, g, b, a );
	return width;
}

int CHud::GetLineSeparatorHeight( void )
{
	return m_iFontHeight >> 1;
}

int CHud::GetLineY( int line )
{
	return ScreenHeight - GetBorderSize() - (line - 1) * GetLineSeparatorHeight() - line * m_iFontHeight;
}

int CHud::GetDefaultAlpha( void )
{
	return V_min( V_max( (int)m_pCvarDefaultAlpha->value, 0 ), 255 );
}

int CHud::GetMaxAlpha( void )
{
	return V_min( V_max( (int)m_pCvarMaxAlpha->value, GetDefaultAlpha() ), 255 );
}

void CHud::UpdateFade( float &fade )
{
	if ( fade != 0 )
	{
		if ( fade > FADE_TIME )
			fade = FADE_TIME;

		fade -= m_flTimeDelta * 20;
		if ( fade <= 0 )
			fade = 0;
	}
}

byte CHud::GetFadeAlpha( float fade )
{
	int alpha = GetDefaultAlpha();
	if ( fade > 0 )
		return alpha + (GetMaxAlpha() - alpha) * (fade / FADE_TIME);

	return alpha;
}

void DRAW_DEBUG_LINE_V( int x, int y )
{
	if ( gHUD.m_pCvarDebug->value >= 1 )
		FillRGBA( x, y - 4, 1, 9, 0, 255, 0, 255 );
}

void DRAW_DEBUG_LINE_H( int x, int y )
{
	if ( gHUD.m_pCvarDebug->value >= 1 )
		FillRGBA( x - 4, y, 9, 1, 0, 255, 0, 255 );
}

void DRAW_DEBUG_CROSS( int x, int y )
{
	if ( gHUD.m_pCvarDebug->value >= 1 )
	{
		DRAW_DEBUG_LINE_V( x, y );
		DRAW_DEBUG_LINE_H( x, y );
	}
}

void DRAW_DEBUG_RECT( int x, int y, int width, int height )
{
	if ( gHUD.m_pCvarDebug->value >= 2 && width > 0 && height > 0 )
	{
		FillRGBA( x, y, 1, height, 0, 255, 0, 255 );
		FillRGBA( x + width - 1, y, 1, height, 0, 255, 0, 255 );
		FillRGBA( x, y, width, 1, 0, 255, 0, 255 );
		FillRGBA( x, y + height - 1, width, 1, 0, 255, 0, 255 );
	}
}

extern int g_iVisibleMouse;

float HUD_GetFOV( void );

extern cvar_t *sensitivity;

// Think
void CHud::Think(void)
{
	int newfov = HUD_GetFOV();
	HUDLIST *pList = m_pHudList;

	while (pList)
	{
		if (pList->p->m_iFlags & HUD_ACTIVE)
			pList->p->Think();
		pList = pList->pNext;
	}

	if ( newfov == 0 )
	{
		m_iFOV = V_min( default_fov->value, 110 );
	}
	else
	{
		m_iFOV = V_min( newfov, 180 );
	}

	// the clients fov is actually set in the client data update section of the hud

	// Set a new sensitivity
	if ( m_iFOV == default_fov->value )
	{  
		// reset to saved sensitivity
		m_flMouseSensitivity = 0;
	}
	else
	{  
		// set a new sensitivity that is proportional to the change from the FOV default
		m_flMouseSensitivity = sensitivity->value * ((double)newfov / V_max( default_fov->value, 0.1 )) * CVAR_GET_FLOAT("zoom_sensitivity_ratio");
	}

	// think about default fov
	if ( m_iFOV == 0 )
	{  // only let players adjust up in fov,  and only if they are not overriden by something else
		m_iFOV = V_min( default_fov->value, 110 );
	}

}

void CHud::DrawElements(void)
{
	if (ScreenWidth < 640 || m_pCvarDraw->value == 0 || (m_iHideHUDDisplay & HIDEHUD_ALL))
		return;

	int border = GetBorderSize();

	DRAW_DEBUG_RECT(border, border, ScreenWidth - 2 * border, ScreenHeight - 2 * border);

	for (HUDLIST *list = m_pHudList; list; list = list->pNext)
	{
		if (list->p->m_iFlags & (m_iIntermission ? HUD_INTERMISSION : HUD_ACTIVE))
			list->p->Draw(m_flTime);
	}
}

// Redraw
// step through the local data,  placing the appropriate graphics & text as appropriate
// returns 1 if they've changed, 0 otherwise
int CHud :: Redraw( float flTime, int intermission )
{
	MediaPlayer.UpdateMedia();
	MediaPlayer.DrawHUDIcon();
	gViewPort->m_pMediaSelect->UpdateTime();

	static int m_flShotTime = 0;
	int oldWidth = ScreenWidth;
	int oldHeight = ScreenHeight;

	GetScreenInfo(&m_scrinfo);

	if (gViewPort && (oldWidth != ScreenWidth || oldHeight != ScreenHeight))
	{
		gViewPort->setSize(ScreenWidth, ScreenHeight);
		gViewPort->GetScoreBoard()->setSize(
			(int)(ScreenWidth - 2 * (ScreenWidth / 640.0f * 80)),
			(int)(ScreenHeight - 2 * (ScreenHeight / 480.0f * 40)));
	}

	m_fOldTime = m_flTime;	// save time of previous redraw
	m_flTime = flTime;
	m_flTimeDelta = (double)m_flTime - m_fOldTime;

	// Clock was reset, reset delta
	if ( m_flTimeDelta < 0 )
		m_flTimeDelta = 0;

	// Bring up the scoreboard during intermission
	if (gViewPort)
	{
		if ( m_iIntermission && !intermission )
		{
			// Have to do this here so the scoreboard goes away
			m_iIntermission = 0;
			gViewPort->HideCommandMenu();
			gViewPort->HideScoreBoard();
		}
		else if ( !m_iIntermission && intermission )
		{
			gViewPort->HideCommandMenu();
			gViewPort->ShowScoreBoard();

			// Take a screenshot if the client's got the cvar set
			if ( CVAR_GET_FLOAT( "hud_takesshots" ) != 0 )
				m_flShotTime = (int)(flTime + 1.0);	// Take a screenshot in a second
		}
	}

	if (m_flShotTime && flTime > (float)m_flShotTime)
	{
		gEngfuncs.pfnClientCmd("snapshot\n");
		m_flShotTime = 0;
	}

	m_iIntermission = intermission;

	DrawElements();

	if (gViewPort && gViewPort->m_pCameraPanel
		&& gViewPort->m_pCurrentMenu == gViewPort->m_pCameraPanel
		&& gViewPort->m_pCameraPanel->isVisible()
		&& gViewPort->m_pCameraPanel->HasOverrideCursor() )
			{
		gViewPort->m_pCameraPanel->DrawOverrideCursor(flTime);
	}

	/*
	if ( g_iVisibleMouse )
	{
		void IN_GetMousePos( int *mx, int *my );
		int mx, my;

		IN_GetMousePos( &mx, &my );

		if (m_hsprCursor == 0)
		{
			char sz[256];
			sprintf( sz, "sprites/cursor.spr" );
			m_hsprCursor = SPR_Load( sz );
		}

		SPR_Set(m_hsprCursor, 250, 250, 250 );

		// Draw the logo at 20 fps
		SPR_DrawAdditive( 0, mx, my, NULL );
	}
	*/

	return 1;
}

void ScaleColors( int &r, int &g, int &b, int a )
{
	float x = (float)a / 255;
	r = (int)(r * x);
	g = (int)(g * x);
	b = (int)(b * x);
}

int CHud::DrawString(int x, int y, int maxX, char *string, int r, int g, int b, int font)
{
	DRAW_DEBUG_CROSS(x, y);

	for (char *text = string; *text && *text != '\n'; ++text)
	{
		int next = x + m_scrinfo.charWidths[*text];

		if (next > maxX)
			break;

		g_pEngineClient->DrawCharacterOpq(x, y, *text, r, g, b, font);
		x = next;
	}

	return x;
}

int CHud::DrawNumberString(int x, int y, int minX, int number, int r, int g, int b)
{
	char string[32];

	UTIL_FormatString(string, "%d", number);
	return DrawStringReverse(x, y, minX, string, r, g, b, 0);
}

// draws a string from right to left (right-aligned)
int CHud::DrawStringReverse(int x, int y, int minX, char *string, int r, int g, int b, int font)
{
	DRAW_DEBUG_CROSS(x, y);

	for (int i = strlen(string) - 1; i >= 0; --i)
	{
		char ch = string[i];
		int next = x - m_scrinfo.charWidths[ch];

		if (next < minX)
			break;

		x = next;
		g_pEngineClient->DrawCharacterOpq(x, y, ch, r, g, b, font);
	}

	return x;
}

int CHud :: DrawHudNumber( int x, int y, int iFlags, int iNumber, int r, int g, int b)
{
	int iWidth = GetSpriteRect(m_HUD_number_0).right - GetSpriteRect(m_HUD_number_0).left;
	int k;
	
	if (iNumber > 0)
	{
		// SPR_Draw 100's
		if (iNumber >= 100)
		{
			 k = iNumber/100;
			SPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b );
			SPR_DrawAdditive( 0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_3DIGITS))
		{
			//SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		// SPR_Draw 10's
		if (iNumber >= 10)
		{
			k = (iNumber % 100)/10;
			SPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b );
			SPR_DrawAdditive( 0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_3DIGITS | DHN_2DIGITS))
		{
			//SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		// SPR_Draw ones
		k = iNumber % 10;
		SPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b );
		SPR_DrawAdditive(0,  x, y, &GetSpriteRect(m_HUD_number_0 + k));
		x += iWidth;
	} 
	else if (iFlags & DHN_DRAWZERO) 
	{
		SPR_Set(GetSprite(m_HUD_number_0), r, g, b );

		// SPR_Draw 100's
		if (iFlags & (DHN_3DIGITS))
		{
			//SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		if (iFlags & (DHN_3DIGITS | DHN_2DIGITS))
		{
			//SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		// SPR_Draw ones
		
		SPR_DrawAdditive( 0,  x, y, &GetSpriteRect(m_HUD_number_0));
		x += iWidth;
	}

	return x;
}


int CHud::GetNumWidth( int iNumber, int iFlags )
{
	if (iFlags & (DHN_3DIGITS))
		return 3;

	if (iFlags & (DHN_2DIGITS))
		return 2;

	if (iNumber <= 0)
	{
		if (iFlags & (DHN_DRAWZERO))
			return 1;
		else
			return 0;
	}

	if (iNumber < 10)
		return 1;

	if (iNumber < 100)
		return 2;

	return 3;

}	


