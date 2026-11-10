/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
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
// battery.cpp
//
// implementation of CHudBattery class
//

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

#include <string.h>
#include <stdio.h>

DECLARE_MESSAGE(m_Battery, Battery)

int CHudBattery::Init(void)
{
	m_iBat = 0;
	m_fFade = 0;
	m_iFlags = 0;

	HOOK_MESSAGE(Battery);

	gHUD.AddHudElem(this);

	return 1;
};


int CHudBattery::VidInit(void)
{
	int HUD_suit_empty = gHUD.GetSpriteIndex( "suit_empty" );
	int HUD_suit_full = gHUD.GetSpriteIndex( "suit_full" );

	m_hSprite1 = m_hSprite2 = 0;  // delaying get sprite handles until we know the sprites are loaded
	m_prc1 = &gHUD.GetSpriteRect( HUD_suit_empty );
	m_prc2 = &gHUD.GetSpriteRect( HUD_suit_full );
	m_iHeight = m_prc2->bottom - m_prc1->top;
	m_fFade = 0;
	return 1;
};

int CHudBattery:: MsgFunc_Battery(const char *pszName,  int iSize, void *pbuf )
{
	m_iFlags |= HUD_ACTIVE;
	
	BEGIN_READ( pbuf, iSize );
	int x = READ_SHORT();

	if ( x != m_iBat )
	{
		m_fFade = FADE_TIME;
		m_iBat = x;
	}

	return 1;
}


int CHudBattery::Draw(float flTime)
{
	if ( gHUD.m_iHideHUDDisplay & HIDEHUD_ARMOR)
		return 1;

	wrect_t rc = *m_prc2;
	rc.top  += m_iHeight * ((float)(100- V_min(100,m_iBat)) * 0.01);

	if (!(gHUD.m_iWeaponBits & (1<< WEAPON_SUIT) ))
		return 1;

	gHUD.UpdateFade(m_fFade);
	int a = gHUD.GetFadeAlpha(m_fFade);
	int r, g, b;

	UnpackRGB(r, g, b, RGB_BLUEISH);

	ScaleColors(r, g, b, a );

	int iOffset = gHUD.GetSpriteIndex("suit_empty");
	int HUD_suit_full = gHUD.GetSpriteIndex("suit_full");

	// make sure we have the right sprite handles
	if ( !m_hSprite1 )
		m_hSprite1 = gHUD.GetSprite(iOffset);
	if ( !m_hSprite2 )
		m_hSprite2 = gHUD.GetSprite(HUD_suit_full);

	wrect_t &suit = gHUD.GetSpriteRect(iOffset);
	int width = suit.right - suit.left;
	int offset = (suit.bottom - suit.top - gHUD.m_iFontHeight) >> 1;
	int y = gHUD.GetLineY(1);
	int healthWidth = gHUD.m_Health.GetWidth() + gHUD.GetSpaceWidth();
	int x = gHUD.GetWidth() / 6;

	if (x > 240)
		x = 240;
	else
		x = V_max(x, healthWidth);

	x += gHUD.GetBorderSize();
	gHUD.DrawSprite(x, y - offset, m_hSprite1, m_prc1, r, g, b, 0, SPR_ADDITIVE);

	if (rc.bottom > rc.top)
		gHUD.DrawSprite(x, y - offset + rc.top - m_prc2->top, m_hSprite2, &rc, r, g, b, 0, SPR_ADDITIVE);

	gHUD.DrawNumber(x + width + 4, y, m_iBat, r, g, b, 3, 4, 0);

	return 1;
}
