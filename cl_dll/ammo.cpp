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
// Ammo.cpp
//
// implementation of CHudAmmo class
//

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "pm_shared.h"
#include "weaponinfo.h"

#include <string.h>
#include <stdio.h>

#include "ammohistory.h"
#include "vgui_TeamFortressViewport.h"

WEAPON *gpActiveSel;	// NULL means off, 1 means just the menu bar, otherwise
						// this points to the active weapon menu item

client_sprite_t *GetSpriteList(client_sprite_t *pList, const char *psz, int iCount);

WeaponsResource gWR;

int g_weaponselect = 0;

void WeaponsResource :: LoadAllWeaponSprites( void )
{
	for (int i = 0; i < MAX_WEAPONS; i++)
	{
		if ( rgWeapons[i].iId )
			LoadWeaponSprites( &rgWeapons[i] );
	}
}

int WeaponsResource :: CountAmmo( int iId ) 
{ 
	if ( (unsigned int)iId >= MAX_AMMO_TYPES )
		return 0;

	return riAmmo[iId];
}

bool WeaponsResource :: CheckWeaponSlot( WEAPON *p )
{
	if ( (unsigned int)p->iSlot > MAX_WEAPON_SLOTS )
	{
		gEngfuncs.Con_Printf("Weapon '%s' slot index is out of range!(%d)\n", p->szName, p->iSlot);
		return false;
	}

	if ( (unsigned int)p->iSlotPos > MAX_WEAPON_POSITIONS )
	{
		gEngfuncs.Con_Printf("Weapon '%s' slot position index is out of range!(%d)\n", p->szName, p->iSlotPos);
		return false;
	}

	return true;
}

bool WeaponsResource :: UsesAmmo( WEAPON *p )
{
	return p && p->iMax1 != -1 && (p->iAmmoType != -1 || p->iAmmo2Type != -1);
}

bool WeaponsResource :: HasAmmoLeft( WEAPON *p )
{
	if ( !p )
		return false;

	return (p->iAmmoType != -1 && (CountAmmo(p->iAmmoType) > 0 || p->iClip > 0))
		|| (p->iAmmo2Type != -1 && (CountAmmo(p->iAmmo2Type) > 0 || p->iClip2 > 0));
}

bool WeaponsResource :: CanBeSelected( WEAPON *p )
{
	return p && (!UsesAmmo(p) || (p->iFlags & WEAPON_FLAGS_SELECTONEMPTY) || HasAmmoLeft(p));
}


void WeaponsResource :: LoadWeaponSprites( WEAPON *pWeapon )
{
	int i;

	char sz[128];

	if ( !pWeapon )
		return;

	memset( &pWeapon->rcActive, 0, sizeof(wrect_t) );
	memset( &pWeapon->rcInactive, 0, sizeof(wrect_t) );
	memset( &pWeapon->rcAmmo, 0, sizeof(wrect_t) );
	memset( &pWeapon->rcAmmo2, 0, sizeof(wrect_t) );
	pWeapon->hInactive = 0;
	pWeapon->hActive = 0;
	pWeapon->hAmmo = 0;
	pWeapon->hAmmo2 = 0;

	if ( pWeapon->szCustomDirectory[0] )
		UTIL_FormatString(sz, "sprites/%s/%s.txt", pWeapon->szCustomDirectory, pWeapon->szSpriteName);
	else
		UTIL_FormatString(sz, "sprites/%s.txt", pWeapon->szSpriteName);
	client_sprite_t *pList = SPR_GetList(sz, &i);

	if (!pList)
		return;

	client_sprite_t *p;
	
	p = GetSpriteList( pList, "crosshair", i );
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hCrosshair = SPR_Load(sz);
		pWeapon->rcCrosshair = p->rc;
	}
	else
		pWeapon->hCrosshair = 0;

	p = GetSpriteList(pList, "autoaim", i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hAutoaim = SPR_Load(sz);
		pWeapon->rcAutoaim = p->rc;
	}
	else
		pWeapon->hAutoaim = 0;

	p = GetSpriteList( pList, "zoom", i );
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hZoomedCrosshair = SPR_Load(sz);
		pWeapon->rcZoomedCrosshair = p->rc;
	}
	else
	{
		pWeapon->hZoomedCrosshair = pWeapon->hCrosshair; //default to non-zoomed crosshair
		pWeapon->rcZoomedCrosshair = pWeapon->rcCrosshair;
	}

	p = GetSpriteList(pList, "zoom_autoaim", i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hZoomedAutoaim = SPR_Load(sz);
		pWeapon->rcZoomedAutoaim = p->rc;
	}
	else
	{
		pWeapon->hZoomedAutoaim = pWeapon->hZoomedCrosshair;  //default to zoomed crosshair
		pWeapon->rcZoomedAutoaim = pWeapon->rcZoomedCrosshair;
	}

	p = GetSpriteList(pList, "weapon", i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hInactive = SPR_Load(sz);
		pWeapon->rcInactive = p->rc;

		gHR.iHistoryGap = V_max( gHR.iHistoryGap, pWeapon->rcActive.bottom - pWeapon->rcActive.top );
	}
	else
		pWeapon->hInactive = 0;

	p = GetSpriteList(pList, "weapon_s", i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hActive = SPR_Load(sz);
		pWeapon->rcActive = p->rc;
	}
	else
		pWeapon->hActive = 0;

	p = GetSpriteList(pList, "ammo", i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hAmmo = SPR_Load(sz);
		pWeapon->rcAmmo = p->rc;

		gHR.iHistoryGap = V_max( gHR.iHistoryGap, pWeapon->rcActive.bottom - pWeapon->rcActive.top );
	}
	else
		pWeapon->hAmmo = 0;

	p = GetSpriteList(pList, "ammo2", i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hAmmo2 = SPR_Load(sz);
		pWeapon->rcAmmo2 = p->rc;

		gHR.iHistoryGap = V_max( gHR.iHistoryGap, pWeapon->rcActive.bottom - pWeapon->rcActive.top );
	}
	else
		pWeapon->hAmmo2 = 0;

}

// Returns the first weapon for a given slot.
WEAPON *WeaponsResource :: GetFirstPos( int iSlot )
{
	WEAPON *pret = NULL;

	for (int i = 0; i < MAX_WEAPON_POSITIONS; i++)
	{
		if ( rgSlots[iSlot][i] && CanBeSelected( rgSlots[iSlot][i] ) )
		{
			pret = rgSlots[iSlot][i];
			break;
		}
	}

	return pret;
}


WEAPON* WeaponsResource :: GetNextActivePos( int iSlot, int iSlotPos )
{
	if ( iSlotPos >= MAX_WEAPON_POSITIONS || iSlot >= MAX_WEAPON_SLOTS )
		return NULL;

	WEAPON *p = gWR.rgSlots[ iSlot ][ iSlotPos+1 ];
	
	if ( !p || !gWR.CanBeSelected(p) )
		return GetNextActivePos( iSlot, iSlotPos + 1 );

	return p;
}

DECLARE_MESSAGE(m_Ammo, CurWeapon );	// Current weapon and clip
DECLARE_MESSAGE(m_Ammo, WeaponList);	// new weapon type
DECLARE_MESSAGE(m_Ammo, CustWeapon);
DECLARE_MESSAGE(m_Ammo, WeaponSpr);
DECLARE_MESSAGE(m_Ammo, AmmoX);			// update known ammo type's count
DECLARE_MESSAGE(m_Ammo, AmmoPickup);	// flashes an ammo pickup record
DECLARE_MESSAGE(m_Ammo, WeapPickup);    // flashes a weapon pickup record
DECLARE_MESSAGE(m_Ammo, HideHUD);	// hides the weapon, ammo, and crosshair displays temporarily
DECLARE_MESSAGE(m_Ammo, ItemPickup);

DECLARE_COMMAND(m_Ammo, Slot1);
DECLARE_COMMAND(m_Ammo, Slot2);
DECLARE_COMMAND(m_Ammo, Slot3);
DECLARE_COMMAND(m_Ammo, Slot4);
DECLARE_COMMAND(m_Ammo, Slot5);
DECLARE_COMMAND(m_Ammo, Slot6);
DECLARE_COMMAND(m_Ammo, Slot7);
DECLARE_COMMAND(m_Ammo, Slot8);
DECLARE_COMMAND(m_Ammo, Slot9);
DECLARE_COMMAND(m_Ammo, Slot10);
DECLARE_COMMAND(m_Ammo, Close);
DECLARE_COMMAND(m_Ammo, NextWeapon);
DECLARE_COMMAND(m_Ammo, PrevWeapon);

// width of ammo fonts
#define AMMO_SMALL_WIDTH 10
#define AMMO_LARGE_WIDTH 20

#define HISTORY_DRAW_TIME	"5"

int CHudAmmo::Init(void)
{
	gHUD.AddHudElem(this);

	HOOK_MESSAGE(CurWeapon);
	HOOK_MESSAGE(WeaponList);
	HOOK_MESSAGE(CustWeapon);
	HOOK_MESSAGE(WeaponSpr);
	HOOK_MESSAGE(AmmoPickup);
	HOOK_MESSAGE(WeapPickup);
	HOOK_MESSAGE(ItemPickup);
	HOOK_MESSAGE(HideHUD);
	HOOK_MESSAGE(AmmoX);

	HOOK_COMMAND("slot1", Slot1);
	HOOK_COMMAND("slot2", Slot2);
	HOOK_COMMAND("slot3", Slot3);
	HOOK_COMMAND("slot4", Slot4);
	HOOK_COMMAND("slot5", Slot5);
	HOOK_COMMAND("slot6", Slot6);
	HOOK_COMMAND("slot7", Slot7);
	HOOK_COMMAND("slot8", Slot8);
	HOOK_COMMAND("slot9", Slot9);
	HOOK_COMMAND("slot10", Slot10);
	HOOK_COMMAND("cancelselect", Close);
	HOOK_COMMAND("invnext", NextWeapon);
	HOOK_COMMAND("invprev", PrevWeapon);

	CVAR_CREATE( "hud_weaponautoswitch", "1", FCVAR_ARCHIVE );

	CVAR_CREATE( "hud_drawhistory_time", HISTORY_DRAW_TIME, FCVAR_ARCHIVE );
	CVAR_CREATE( "hud_fastswitch", "0", FCVAR_ARCHIVE );		// controls whether or not weapons can be selected in one keypress
	m_pCvarSelectionTimeout = CVAR_CREATE( "hud_selection_timeout", "1.5", FCVAR_ARCHIVE | FCVAR_CLIENTDLL );
	m_pCvarSelectionFadeout = CVAR_CREATE( "hud_selection_fadeout", "0.5", FCVAR_ARCHIVE | FCVAR_CLIENTDLL ); //!!!

	gWR.Init();
	gHR.Init();
	Reset();

	return 1;
};

void CHudAmmo::Reset(void)
{
	m_fFade = 0;
	m_iFlags |= HUD_ACTIVE; //!!!
	m_iBucketCount = 0;
	m_pWeapon = NULL; //!!!

	gpActiveSel = NULL;
	gHUD.m_iHideHUDDisplay = 0;

	gWR.Reset();
	gHR.Reset();
}

int CHudAmmo::VidInit(void)
{
	// Load sprites for buckets (top row of weapon menu)
	m_HUD_bucket0 = gHUD.GetSpriteIndex( "bucket1" );
	m_HUD_selection = gHUD.GetSpriteIndex( "selection" );

	m_iBucketWidth = gHUD.GetSpriteRect(m_HUD_bucket0).right - gHUD.GetSpriteRect(m_HUD_bucket0).left;
	m_iBucketHeight = gHUD.GetSpriteRect(m_HUD_bucket0).bottom - gHUD.GetSpriteRect(m_HUD_bucket0).top;

	gHR.iHistoryGap = V_max( gHR.iHistoryGap, gHUD.GetSpriteRect(m_HUD_bucket0).bottom - gHUD.GetSpriteRect(m_HUD_bucket0).top);

	// If we've already loaded weapons, let's get new sprites
	gWR.LoadAllWeaponSprites();

	m_flLastFastSwitch = 0;

	return 1;
}

//
// Think:
//  Used for selection of weapon menu item.
//
void CHudAmmo::CloseSelection(bool bSelected)
{
	gpActiveSel = NULL;
	PlaySound(bSelected ? "common/wpn_select.wav" : "common/wpn_hudoff.wav", 1);
}

void CHudAmmo::Think(void)
{
	if ( gpActiveSel && ( gHUD.m_iHideHUDDisplay & HIDEHUD_WEAPONSELECT) )
		CloseSelection(false);

	if (!gpActiveSel)
		return;

	// has the player selected one?
	if (gHUD.m_iKeyBits & IN_ATTACK)
	{
		if (gpActiveSel != (WEAPON *)1)
		{
			ServerCmd(gpActiveSel->szName);
			g_weaponselect = gpActiveSel->iId;
		}

		gHUD.m_iKeyBits &= ~IN_ATTACK;
		CloseSelection(true);
	}

}

void CHudAmmo::UpdateWeaponList(weapon_data_t *pWeaponData)
{
	int iMaxSlot = 0;

	for ( int i = MAX_WEAPONS - 1; i > 0; i-- )
	{
		WEAPON *p = gWR.GetWeapon(i);

		if ( pWeaponData[p->iId].m_iId )
			gWR.PickupWeapon(p);
		else
			gWR.DropWeapon(p);

		if ( p->iSlot != 9 && p->iSlot > iMaxSlot )
			iMaxSlot = p->iSlot;
	}

	m_iBucketCount = V_max(6, V_min(iMaxSlot + 1, MAX_WEAPON_SLOTS));
}

// Helper function to return a Ammo pointer from id
//

HSPRITE* WeaponsResource :: GetAmmoPicFromWeapon( int iAmmoId, wrect_t& rect )
{
	for ( int i = 0; i < MAX_WEAPONS; i++ )
	{
		if ( rgWeapons[i].iAmmoType == iAmmoId )
		{
			rect = rgWeapons[i].rcAmmo;
			return &rgWeapons[i].hAmmo;
		}
		else if ( rgWeapons[i].iAmmo2Type == iAmmoId )
		{
			rect = rgWeapons[i].rcAmmo2;
			return &rgWeapons[i].hAmmo2;
		}
	}

	return NULL;
}


// Menu Selection Code

void WeaponsResource :: SelectSlot( int iSlot, int fAdvance, int iDirection )
{
	if ( gHUD.m_Menu.m_fMenuDisplayed && (fAdvance == FALSE) && (iDirection == 1) )	
	{ // menu is overriding slot use commands
		gHUD.m_Menu.SelectMenuItem( iSlot + 1 );  // slots are one off the key numbers
		return;
	}

	if ( iSlot > MAX_WEAPON_SLOTS )
		return;

	if ( gHUD.m_iHideHUDDisplay & HIDEHUD_WEAPONSELECT )
		gEngfuncs.Con_NPrintf(1, "You must drop this weapon before selecting another weapon.\n\n");

	if ( gHUD.m_iHideHUDDisplay & ( HIDEHUD_WEAPONS | HIDEHUD_WEAPONSELECT ) )
		return;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
		return;

	if ( !HasWeapons())
		return;

	WEAPON *p = NULL;
	bool fastSwitch = CVAR_GET_FLOAT( "hud_fastswitch" ) != 0;

	if ( (gpActiveSel == NULL) || (gpActiveSel == (WEAPON *)1) || (iSlot != gpActiveSel->iSlot) )
	{
		PlaySound( "common/wpn_hudon.wav", 1 );
		p = GetFirstPos( iSlot );

		if ( p && fastSwitch ) // check for fast weapon switch mode
		{
			// if fast weapon switch is on, then weapons can be selected in a single keypress
			// but only if there is only one item in the bucket
			WEAPON *p2 = GetNextActivePos( p->iSlot, p->iSlotPos );
			if ( !p2 )
			{	// only one active item in bucket, so change directly to weapon
				ServerCmd( p->szName );
				g_weaponselect = p->iId;
				return;
			}
		}
	}
	else
	{
		PlaySound("common/wpn_moveselect.wav", 1);
		if ( gpActiveSel )
			p = GetNextActivePos( gpActiveSel->iSlot, gpActiveSel->iSlotPos );
		if ( !p )
			p = GetFirstPos( iSlot );
	}

	
	if ( !p )  // no selection found
	{
		// just display the weapon list, unless fastswitch is on just ignore it
		if ( !fastSwitch )
			gpActiveSel = (WEAPON *)1;
		else
			gpActiveSel = NULL;
	}
	else 
		gpActiveSel = p;
}

//------------------------------------------------------------------------
// Message Handlers
//------------------------------------------------------------------------

//
// AmmoX  -- Update the count of a known type of ammo
// 
int CHudAmmo::MsgFunc_AmmoX(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );

	int iIndex = READ_BYTE();
	int iCount = READ_LONG();

	gWR.SetAmmo( iIndex, abs(iCount) );

	for ( int i = 0; i < MAX_WEAPONS; i++ )
	{
		WEAPON *p = gWR.GetWeapon(i);

		if ( p->bState & WEAPON_STATE_CURRENT )
		{
			if ( p->iAmmoType == iIndex || p->iAmmo2Type == iIndex )
				m_fFade = 100;
			break;
		}
	}

	return 1;
}

int CHudAmmo::MsgFunc_AmmoPickup( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	int iIndex = READ_BYTE();
	int iCount = READ_LONG();

	// Add ammo to the history
	gHR.AddToHistory( HISTSLOT_AMMO, iIndex, abs(iCount) );

	return 1;
}

int CHudAmmo::MsgFunc_WeapPickup( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	int iIndex = READ_SHORT();

	// Add the weapon to the history
	gHR.AddToHistory( HISTSLOT_WEAP, iIndex );

	return 1;
}

int CHudAmmo::MsgFunc_ItemPickup( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	const char *szName = READ_STRING();

	// Add the weapon to the history
	gHR.AddToHistory( HISTSLOT_ITEM, szName );

	return 1;
}


int CHudAmmo::MsgFunc_HideHUD( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	
	gHUD.m_iHideHUDDisplay = READ_SHORT();

	if ( gHUD.m_iHideHUDDisplay & ( HIDEHUD_WEAPONS | HIDEHUD_CROSSHAIR ) )
	{
		gpActiveSel = NULL;
		gHUD.m_Crosshair.ClearSprite();
	}
	else
	{
		if ( m_pWeapon )
			gHUD.m_Crosshair.SetSprite( m_pWeapon->hCrosshair, m_pWeapon->rcCrosshair, 255, 255, 255 );
	}

	return 1;
}

// 
//  CurWeapon: Update hud state with the current weapon and clip count. Ammo
//  counts are updated with AmmoX. Server assures that the Weapon ammo type 
//  numbers match a real ammo type.
//
int CHudAmmo::MsgFunc_CurWeapon(const char *pszName, int iSize, void *pbuf )
{

	BEGIN_READ( pbuf, iSize );

	int iState = READ_BYTE();
	int iId = READ_SHORT();
	int iClip = READ_LONG();
	int iClip2 = READ_LONG();

	// detect if we're also on target
	if ( iId == -1 )
	{
		gHUD.m_Crosshair.ClearSprite();
		m_pWeapon = NULL;

		if ( g_iUser1 != OBS_IN_EYE )
			gpActiveSel = NULL;
			return 1;
	}

	WEAPON *pWeapon = gWR.GetWeapon( iId );

	if ( !pWeapon )
		return 0;

	if ( iClip < -1 )
		pWeapon->iClip = 0;
	else
		pWeapon->iClip = iClip;

	if ( iClip2 < -1 )
		pWeapon->iClip2 = 0;
	else
		pWeapon->iClip2 = iClip2;

	pWeapon->bState = iState;


	if ( !(iState & WEAPON_STATE_CURRENT) )	// we're not the current weapon, so update no more
		return 1;

	m_pWeapon = pWeapon;

	if ( !(gHUD.m_iHideHUDDisplay & (HIDEHUD_WEAPONS | HIDEHUD_CROSSHAIR)) )
	{
		bool fOnTarget = (iState & WEAPON_STATE_ONTARGET) != 0;

		if ( gHUD.m_iFOV >= 50 )
	{ // normal crosshairs
		if (fOnTarget && pWeapon->hAutoaim)
				gHUD.m_Crosshair.SetSprite(pWeapon->hAutoaim, pWeapon->rcAutoaim, 255, 255, 255);
		else
				gHUD.m_Crosshair.SetSprite(pWeapon->hCrosshair, pWeapon->rcCrosshair, 255, 255, 255);
	}
	else
	{ // zoomed crosshairs
		if (fOnTarget && pWeapon->hZoomedAutoaim)
				gHUD.m_Crosshair.SetSprite(pWeapon->hZoomedAutoaim, pWeapon->rcZoomedAutoaim, 255, 255, 255);
		else
				gHUD.m_Crosshair.SetSprite(pWeapon->hZoomedCrosshair, pWeapon->rcZoomedCrosshair, 255, 255, 255);

	}
	}

	m_fFade = 100; //!!!
	m_iFlags |= HUD_ACTIVE;

	return 1;
}

// WeaponList -- Tells the hud about a new weapon type.
//
int CHudAmmo::MsgFunc_WeaponList(const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	
	WEAPON Weapon = {};

	safe_strcpy( Weapon.szName, READ_STRING(), sizeof(Weapon.szName) );
	Weapon.iAmmoType = (int)READ_CHAR();	
	
	Weapon.iMax1 = READ_LONG();
	if (Weapon.iMax1 == 255)
		Weapon.iMax1 = -1;

	Weapon.iAmmo2Type = READ_CHAR();
	Weapon.iMax2 = READ_LONG();
	if (Weapon.iMax2 == 255)
		Weapon.iMax2 = -1;

	Weapon.iSlot = READ_CHAR();
	Weapon.iSlotPos = READ_CHAR();
	Weapon.iId = READ_SHORT();
	Weapon.iFlags = READ_BYTE();
	Weapon.iClip = 0;
	safe_strcpy( Weapon.szSpriteName, Weapon.szName, sizeof(Weapon.szSpriteName) );

	gWR.AddWeapon( &Weapon );

	return 1;

}

int CHudAmmo::MsgFunc_CustWeapon(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	WEAPON *pWeapon = gWR.GetWeapon(READ_SHORT());

	if ( pWeapon )
	{
		safe_strcpy(pWeapon->szCustomDirectory, READ_STRING(), sizeof(pWeapon->szCustomDirectory));
		gWR.LoadWeaponSprites(pWeapon);
	}

	return 1;
}

int CHudAmmo::MsgFunc_WeaponSpr(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	WEAPON *pWeapon = gWR.GetWeapon(READ_SHORT());

	if ( pWeapon )
	{
		safe_strcpy(pWeapon->szSpriteName, READ_STRING(), sizeof(pWeapon->szSpriteName));
		gWR.LoadWeaponSprites(pWeapon);
	}

	return 1;
}

//------------------------------------------------------------------------
// Command Handlers
//------------------------------------------------------------------------
// Slot button pressed
void CHudAmmo::SlotInput( int iSlot )
{
	if ( gViewPort && gViewPort->SlotInput( iSlot ) )
		return;

	m_flSelectionTime = gHUD.m_flTime;
	gWR.SelectSlot(iSlot, FALSE, 1);
}

void CHudAmmo::UserCmd_Slot1(void)
{
	SlotInput( 0 );
}

void CHudAmmo::UserCmd_Slot2(void)
{
	SlotInput( 1 );
}

void CHudAmmo::UserCmd_Slot3(void)
{
	SlotInput( 2 );
}

void CHudAmmo::UserCmd_Slot4(void)
{
	SlotInput( 3 );
}

void CHudAmmo::UserCmd_Slot5(void)
{
	SlotInput( 4 );
}

void CHudAmmo::UserCmd_Slot6(void)
{
	SlotInput( 5 );
}

void CHudAmmo::UserCmd_Slot7(void)
{
	SlotInput( 6 );
}

void CHudAmmo::UserCmd_Slot8(void)
{
	SlotInput( 7 );
}

void CHudAmmo::UserCmd_Slot9(void)
{
	SlotInput( 8 );
}

void CHudAmmo::UserCmd_Slot10(void)
{
	SlotInput( 9 );
}

void CHudAmmo::UserCmd_Close(void)
{
	if (gpActiveSel)
		CloseSelection(false);
	else
		EngineClientCmd("escape");
}


// Selects the next item in the weapon menu
void CHudAmmo::UserCmd_NextWeapon(void)
{
	if ( gHUD.m_iHideHUDDisplay & HIDEHUD_WEAPONSELECT )
		gEngfuncs.Con_NPrintf(1, "You must drop this weapon before selecting another weapon.\n\n");

	if (gHUD.m_iHideHUDDisplay & (HIDEHUD_WEAPONS | HIDEHUD_WEAPONSELECT) )
		return;

	m_flSelectionTime = gHUD.m_flTime;
	bool fastSwitch = CVAR_GET_FLOAT("hud_fastswitch") != 0;

	if ( !gpActiveSel || gpActiveSel == (WEAPON*)1 )
		gpActiveSel = m_pWeapon;

	int pos = 0;
	int slot = 0;
	if ( gpActiveSel )
	{
		pos = gpActiveSel->iSlotPos + 1;
		slot = gpActiveSel->iSlot;
	}

	for ( int loop = 0; loop <= 1; loop++ )
	{
		for ( ; slot < MAX_WEAPON_SLOTS; slot++ )
		{
			for ( ; pos < MAX_WEAPON_POSITIONS; pos++ )
			{
				WEAPON *wsp = gWR.GetWeaponSlot( slot, pos );

				if ( wsp && gWR.CanBeSelected(wsp) )
				{
					if ( fastSwitch )
					{
						if ( gHUD.m_flTime - m_flLastFastSwitch > 0.25f )
						{
							m_flLastFastSwitch = gHUD.m_flTime;
							ServerCmd(wsp->szName);
							g_weaponselect = wsp->iId;
							CloseSelection(true);
						}
						gpActiveSel = NULL;
					}
					else
						gpActiveSel = wsp;
					return;
				}
			}

			pos = 0;
		}

		slot = 0;  // start looking from the first slot again
	}

	gpActiveSel = NULL;
}

// Selects the previous item in the menu
void CHudAmmo::UserCmd_PrevWeapon(void)
{
	if ( gHUD.m_iHideHUDDisplay & HIDEHUD_WEAPONSELECT )
		gEngfuncs.Con_NPrintf(1, "You must drop this weapon before selecting another weapon.\n\n");

	if (gHUD.m_iHideHUDDisplay & (HIDEHUD_WEAPONS | HIDEHUD_WEAPONSELECT) )
		return;

	m_flSelectionTime = gHUD.m_flTime;
	bool fastSwitch = CVAR_GET_FLOAT("hud_fastswitch") != 0;

	if ( !gpActiveSel || gpActiveSel == (WEAPON*)1 )
		gpActiveSel = m_pWeapon;

	int pos = MAX_WEAPON_POSITIONS-1;
	int slot = MAX_WEAPON_SLOTS-1;
	if ( gpActiveSel )
	{
		pos = gpActiveSel->iSlotPos - 1;
		slot = gpActiveSel->iSlot;
	}
	
	for ( int loop = 0; loop <= 1; loop++ )
	{
		for ( ; slot >= 0; slot-- )
		{
			for ( ; pos >= 0; pos-- )
			{
				WEAPON *wsp = gWR.GetWeaponSlot( slot, pos );

				if ( wsp && gWR.CanBeSelected(wsp) )
				{
					if ( fastSwitch )
					{
						if ( gHUD.m_flTime - m_flLastFastSwitch > 0.25f )
						{
							m_flLastFastSwitch = gHUD.m_flTime;
							ServerCmd(wsp->szName);
							g_weaponselect = wsp->iId;
							CloseSelection(true);
						}
						gpActiveSel = NULL;
					}
					else
						gpActiveSel = wsp;
					return;
				}
			}

			pos = MAX_WEAPON_POSITIONS-1;
		}
		
		slot = MAX_WEAPON_SLOTS-1;
	}

	gpActiveSel = NULL;
}



//-------------------------------------------------------------------------
// Drawing code
//-------------------------------------------------------------------------

int CHudAmmo::Draw(float flTime)
{
	if (!(gHUD.m_iWeaponBits & (1<< WEAPON_SUIT)) || (gHUD.m_iHideHUDDisplay & HIDEHUD_WEAPONS) )
		return 1;

	// Draw Weapon Menu
	DrawWList(flTime);

	// Draw ammo pickup history
	gHR.DrawAmmoHistory( flTime );

	if ( gHUD.m_iHideHUDDisplay & HIDEHUD_AMMO )
		return 1;

	WEAPON *pw = m_pWeapon; // shorthand

	// SPR_Draw Ammo
	if ( !(m_iFlags & HUD_ACTIVE) || !pw || (pw->iAmmoType < 0 && pw->iAmmo2Type < 0))
		return 0;

	gHUD.UpdateFade(m_fFade);
	int a = gHUD.GetFadeAlpha(m_fFade );
	int r, g, b;

	UnpackRGB(r,g,b, RGB_BLUEISH);

	ScaleColors(r, g, b, a );

	if ( pw->iAmmoType > 0)
	{
		int iIconWidth = ScreenWidth - (pw->rcAmmo.right - pw->rcAmmo.left) - gHUD.GetBorderSize();
		int y = gHUD.GetLineY(1);
		int iOffset = (pw->rcAmmo.bottom - pw->rcAmmo.top - gHUD.m_iFontHeight) >> 1;

		gHUD.DrawSprite(iIconWidth, y - iOffset, pw->hAmmo, &pw->rcAmmo, r, g, b, 0, SPR_ADDITIVE);
		iIconWidth -= 4 + gHUD.GetNumberWidth(gWR.CountAmmo(pw->iAmmoType), 3, 4, 0);
		gHUD.DrawNumber(iIconWidth, y, gWR.CountAmmo(pw->iAmmoType), r, g, b, 3, 4, 0);

		if ( pw->iClip >= 0 )
		{
			iIconWidth -= gHUD.GetSeparatorWidth() + gHUD.GetSpaceWidth();
			gHUD.DrawSeparator(iIconWidth, y, r, g, b, a);
			iIconWidth -= gHUD.GetSpaceWidth() + gHUD.GetNumberWidth(pw->iClip, 3, 4, 0);
			gHUD.DrawNumber(iIconWidth, y, pw->iClip, r, g, b, 3, 4, 0);


		}
	}

	if ( pw->iFlags & WEAPON_FLAGS_ALTERNATE )
	{
		if ( pw->bState & WEAPON_STATE_ALTERNATE )
		{
			int iIconWidth = ScreenWidth - (pw->rcAmmo2.right - pw->rcAmmo2.left) - gHUD.GetBorderSize();
			int y = gHUD.GetLineY(2);
			int iOffset = (pw->rcAmmo.bottom - pw->rcAmmo.top - gHUD.m_iFontHeight) >> 1;

			gHUD.DrawSprite(iIconWidth, y - iOffset, pw->hAmmo, &pw->rcAmmo, r, g, b, 0, SPR_ADDITIVE);
			iIconWidth -= 4 + gHUD.GetNumberWidth(pw->iClip2, 3, 4, 0);
			gHUD.DrawNumber(iIconWidth, y, pw->iClip2, r, g, b, 3, 4, 0);
		}
	}
	else if ( (pw->iAmmo2Type > 0 && gWR.CountAmmo(pw->iAmmo2Type)) || pw->iClip2 > 0)
	{
		int iIconWidth = ScreenWidth - (pw->rcAmmo2.right - pw->rcAmmo2.left) - gHUD.GetBorderSize();
		int y = gHUD.GetLineY(2);
		int iOffset = (pw->rcAmmo.bottom - pw->rcAmmo.top - gHUD.m_iFontHeight) >> 1;

		gHUD.DrawSprite(iIconWidth, y - iOffset, pw->hAmmo2, &pw->rcAmmo2, r, g, b, 0, SPR_ADDITIVE);
		iIconWidth -= 4 + gHUD.GetNumberWidth(gWR.CountAmmo(pw->iAmmo2Type), 2, 4, 0);
		gHUD.DrawNumber(iIconWidth, y, gWR.CountAmmo(pw->iAmmo2Type), r, g, b, 2, 4, 0);

		if ( pw->iClip2 >= 0 )
		{
			iIconWidth -= gHUD.GetSeparatorWidth() + gHUD.GetSpaceWidth();
			gHUD.DrawSeparator(iIconWidth, y, r, g, b, a);
			iIconWidth -= gHUD.GetSpaceWidth() + gHUD.GetNumberWidth(pw->iClip2, 2, 4, 0);
			gHUD.DrawNumber(iIconWidth, y, pw->iClip2, r, g, b, 2, 4, 0);
		}
	}
	return 1;
}

// Draws the ammo bar on the hud
//
int DrawBar(int x, int y, int width, int height, float f, float fade)
{
	int r, g, b;

	if (f != 0 )
	{
		int w = 2;
	if (f > 0 )
		{
			w = V_max(2, (int)(width * V_min(f, 1.0f)));
		UnpackRGB(r, g, b, RGB_GREENISH);
		}
		else
			UnpackRGB(r, g, b, RGB_REDISH);
		FillRGBA(x, y, w, height, r, g, b, gHUD.GetMaxAlpha() * fade);
		x += w;
		width -= w;
	}

	if ( f > 0 )
		UnpackRGB(r, g, b, RGB_BLUEISH);
	else
		UnpackRGB(r, g, b, RGB_DARKREDISH);

	FillRGBA(x, y, width, height, r, g, b, gHUD.GetDefaultAlpha() * fade);

	return x + width;
}

// Reserve ammo, or the clip as a negative count when there is no reserve
int GetPrimaryAmmoCount(WEAPON *p)
{
	if ( p->iAmmoType == -1 )
		return 0;

	int count = gWR.CountAmmo(p->iAmmoType);

	if ( count )
		return count;

	if ( p->iClip != -1 )
		return -p->iClip;

	return 0;
}

int GetSecondaryAmmoCount(WEAPON *p)
{
	if ( p->iAmmo2Type == -1 )
		return 0;

	int count = gWR.CountAmmo(p->iAmmo2Type);

	if ( count )
		return count;

	if ( p->iClip2 != -1 )
		return -p->iClip2;

	return 0;
}

void DrawAmmoBar(WEAPON *p, int x, int y, int width, int height, float fade)
{
	int primary = GetPrimaryAmmoCount(p);
	int secondary = GetSecondaryAmmoCount(p);

	if ( primary || secondary )
	{
		x = DrawBar(x, y, width, height, (float)primary / p->iMax1, fade);


		// Do we have secondary ammo too?

		if ( secondary )
			DrawBar(x + 5, y, width, height, (float)secondary / p->iMax2, fade);
		}
	}

int CHudAmmo::DrawWList(float flTime)
{
	if ( !gpActiveSel )
		return 0;

	float fade = 1;
	float timeout = m_pCvarSelectionTimeout->value;

	if ( timeout > 0 && flTime - m_flSelectionTime > timeout )
	{
		float duration = m_pCvarSelectionFadeout->value;
		float elapsed = flTime - m_flSelectionTime - timeout;

		if ( duration <= 0 || elapsed >= duration )
		{
			gpActiveSel = NULL;
			return 0;
		}

		fade = (1 - elapsed / duration) * 0.5f + 0.2f;
	}

	int iActiveSlot;

	if ( gpActiveSel == (WEAPON *)1 )
		iActiveSlot = -1;
	else
		iActiveSlot = gpActiveSel->iSlot;

	int x = gHUD.GetBorderSize();
	int y = gHUD.GetBorderSize(); //!!!


	// Ensure that there are available choices in the active slot
	if ( iActiveSlot > 0 && !gWR.GetFirstPos( iActiveSlot ) )
		{
			gpActiveSel = (WEAPON *)1;
			iActiveSlot = -1;
		}

	for ( int slot = 0; slot < m_iBucketCount; slot++ )
	{
		int r,g,b;
		int alpha = gHUD.GetDefaultAlpha();
		int iWidth = m_iBucketWidth;

		if ( slot == iActiveSlot )
		{
			alpha = GetBrightAlpha();
			WEAPON *p = gWR.GetFirstPos(slot);
			if ( p )
				iWidth = p->rcActive.right - p->rcActive.left;
		}

		UnpackRGB( r,g,b, RGB_BLUEISH);
						ScaleColors(r, g, b, alpha * fade);
		gHUD.DrawSprite(x, y, gHUD.GetSprite(m_HUD_bucket0 + slot),
			&gHUD.GetSpriteRect(m_HUD_bucket0 + slot), r, g, b, 0, SPR_ADDITIVE);
		x += iWidth + 5;

		}

	x = gHUD.GetBorderSize();

	for ( int slot = 0; slot < m_iBucketCount; slot++ )
	{
		int iWidth = m_iBucketWidth;
		y = gHUD.GetBorderSize() + m_iBucketHeight + 1;

		if ( slot == iActiveSlot )
		{
			WEAPON *first = gWR.GetFirstPos(slot);

			if ( first )
				iWidth = first->rcActive.right - first->rcActive.left;
		}

		for ( int iPos = 0; iPos < MAX_WEAPON_POSITIONS; iPos++ )
			{
				WEAPON *p = gWR.GetWeaponSlot(slot, iPos );

				if ( !p || !p->iId )
					continue;

			int r, g, b;
			int alpha = gHUD.GetDefaultAlpha();
			UnpackRGB(r, g, b, RGB_BLUEISH);

			if ( !gWR.CanBeSelected(p) )
				{
					UnpackRGB(r,g,b, RGB_REDISH);
				alpha = GetDimAlpha();
			}

			if ( slot == iActiveSlot )
			{
				if ( gpActiveSel == p )
				{
					UnpackRGB(r,g,b, RGB_BLUEISH);
					ScaleColors(r, g, b, gHUD.GetMaxAlpha() * fade);
					gHUD.DrawSprite( x, y, p->hActive, &p->rcActive, r, g, b, 0, SPR_ADDITIVE);
					gHUD.DrawSprite(x, y, gHUD.GetSprite(m_HUD_selection),
						&gHUD.GetSpriteRect(m_HUD_selection), r, g, b, 0, SPR_ADDITIVE);
				}
				else
				{
					ScaleColors(r, g, b, alpha * fade);
					gHUD.DrawSprite(x, y, p->hInactive, &p->rcInactive, r, g, b, 0, SPR_ADDITIVE);
				}

				DrawAmmoBar(p, x + 10, y + 1, 20, 4, fade);

				y += p->rcActive.bottom - p->rcActive.top + 5;
			}
			else
			{
				FillRGBA(x, y, m_iBucketWidth, m_iBucketHeight, r, g, b, alpha * fade);
				y += m_iBucketHeight + 5;
		}
	}

		x += iWidth + 5;
	}

	return 1;

}

int CHudAmmo::GetDimAlpha(void)
{
	return gHUD.GetDefaultAlpha() / 2;
}

int CHudAmmo::GetBrightAlpha(void)
{
	return (gHUD.GetMaxAlpha() - gHUD.GetDefaultAlpha()) / 2 + gHUD.GetDefaultAlpha();
}

/* =================================
	GetSpriteList

Finds and returns the matching 
sprite name 'psz' at the 640 resolution
in the given sprite list 'pList'
iCount is the number of items in the pList
================================= */
client_sprite_t *GetSpriteList(client_sprite_t *pList, const char *psz, int iCount)
{
	if (!pList)
		return NULL;

	int i = iCount;
	client_sprite_t *p = pList;

	while(i--)
	{
		if ((!strcmp(psz, p->szName)) && (p->iRes == HUD_SPRITE_RES))
			return p;
		p++;
	}

	return NULL;
}
