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
//  ammohistory.cpp
//


#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

#include <string.h>
#include <stdio.h>

#include "ammohistory.h"

HistoryResource gHR;

#define AMMO_PICKUP_GAP (iHistoryGap + 5)
#define AMMO_PICKUP_PICK_HEIGHT (32 + iHistoryGap * 2)
#define AMMO_PICKUP_HEIGHT_MAX (ScreenHeight - gHUD.GetBorderSize() * 2 - 75)

void HistoryResource::AddToHistoryInt(int iType, int iId, int iCount)
{
	if ( AMMO_PICKUP_GAP * iCurrentHistorySlot + AMMO_PICKUP_PICK_HEIGHT > AMMO_PICKUP_HEIGHT_MAX
		|| iCurrentHistorySlot >= MAX_HISTORY )
	{
		iCurrentHistorySlot = 0;
	}

	HIST_ITEM *slot = &rgAmmoHistory[iCurrentHistorySlot++];

	slot->type = iType;
	slot->iId = iId;
	slot->iCount = iCount;
	slot->DisplayTime = gHUD.m_flTime + CVAR_GET_FLOAT("hud_drawhistory_time");
}

void HistoryResource::AddToHistory(int iType, int iId, int iCount)
{
	if ( iType != HISTSLOT_AMMO || iCount )
		AddToHistoryInt(iType, iId, iCount);
}

void HistoryResource::AddToHistory(int iType, const char *szName, int iCount)
{
	if ( iType != HISTSLOT_ITEM )
		return;

	int sprite = gHUD.GetSpriteIndex(szName);

	if ( sprite != -1 )
		AddToHistoryInt(iType, sprite, iCount);
}

void HistoryResource :: CheckClearHistory( void )
{
	for ( int i = 0; i < MAX_HISTORY; i++ )
	{
		if ( rgAmmoHistory[i].type )
			return;
	}

	iCurrentHistorySlot = 0;
}

int HistoryResource :: DrawAmmoHistory( float flTime )
{
	for ( int i = 0; i < MAX_HISTORY; i++ )
	{
		HIST_ITEM *slot = &rgAmmoHistory[i];

		if ( !slot->type )
			continue;

		float duration = CVAR_GET_FLOAT("hud_drawhistory_time");
		slot->DisplayTime = V_min(slot->DisplayTime, gHUD.m_flTime + duration);

			if ( slot->DisplayTime <= flTime )
			{  // pic drawing time has expired
				memset(slot, 0, sizeof(*slot) );
				CheckClearHistory();
			continue;
		}

		int alpha = gHUD.GetMaxAlpha() * (slot->DisplayTime - flTime) / duration;
		int ypos = ScreenHeight - 2 * (32 + iHistoryGap) - AMMO_PICKUP_GAP * i - gHUD.GetBorderSize();
		int xpos = ScreenWidth - gHUD.GetBorderSize();

				int r, g, b;
				UnpackRGB(r,g,b, RGB_BLUEISH);

		if ( slot->type == HISTSLOT_AMMO )
		{
			wrect_t rect;
			HSPRITE *spr = gWR.GetAmmoPicFromWeapon(slot->iId, rect);
				ScaleColors(r, g, b, alpha);
			xpos -= 26;
				if ( spr && *spr )
				gHUD.DrawSprite(xpos, ypos, *spr, &rect, r, g, b, 0, SPR_ADDITIVE);

			gHUD.DrawNumberString( xpos - 10, ypos, xpos - 100, slot->iCount, r, g, b );
			}
			else if ( slot->type == HISTSLOT_WEAP )
			{
				WEAPON *weap = gWR.GetWeapon(slot->iId );

				if ( !weap )
					return 1;

			if ( gWR.UsesAmmo(weap) && !gWR.HasAmmoLeft( weap ) )
					UnpackRGB(r,g,b, RGB_REDISH);

			ScaleColors(r, g, b, alpha);
			xpos -= weap->rcInactive.right - weap->rcInactive.left + 2;
			gHUD.DrawSprite(xpos, ypos, weap->hInactive, &weap->rcInactive, r, g, b, 0, SPR_ADDITIVE);
			}
			else if ( slot->type == HISTSLOT_ITEM && slot->iId )
		{
			wrect_t rect = gHUD.GetSpriteRect(slot->iId);
				ScaleColors(r, g, b, alpha);
			xpos -= rect.right - rect.left + 12;
			gHUD.DrawSprite(xpos, ypos, gHUD.GetSprite(slot->iId), &rect, r, g, b, 0, SPR_ADDITIVE);
		}
	}


	return 1;
}