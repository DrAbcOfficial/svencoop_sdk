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
// menu.cpp
//
// generic menu handler
//
#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

#include <string.h>
#include <stdio.h>

#include "vgui_TeamFortressViewport.h"

#define MAX_MENU_STRING	512
#define MENU_FROM_ANGELSCRIPT	0x80	// ShowMenu "need more" byte: sent by an AngelScript text menu
char g_szMenuString[MAX_MENU_STRING];
char g_szPrelocalisedMenuString[MAX_MENU_STRING];

int KB_ConvertString( char *in, char **ppout );

DECLARE_MESSAGE( m_Menu, ShowMenu );

int CHudMenu :: Init( void )
{
	gHUD.AddHudElem( this );

	HOOK_MESSAGE( ShowMenu );

	InitHUDData();

	return 1;
}

void CHudMenu :: InitHUDData( void )
{
	m_fMenuDisplayed = 0;
	m_bitsValidSlots = 0;
	Reset();
	m_bIsASMenu = false;
}

void CHudMenu :: Reset( void )
{
	g_szPrelocalisedMenuString[0] = 0;
	m_fWaitingForMore = FALSE;
	m_fMenuDisplayed = 0;
	m_iFlags &= ~HUD_ACTIVE;
}

int CHudMenu :: VidInit( void )
{
	return 1;
}


/*=================================
  ParseEscapeToken

  Interprets the given escape token (backslash followed by a letter). The
  first character of the token must be a backslash.  The second character
  specifies the operation to perform:

   \w : White text (this is the default)
   \d : Dim (gray) text
   \y : Yellow text
   \r : Red text
   \g : Green text
   \o : Orange text
   \c : Cyan text
   \m : Magenta text
   \R : Right-align (just for the remainder of the current line)
=================================*/

static int menu_r, menu_g, menu_b, menu_x;
static bool menu_ralign;

static inline const char* ParseEscapeToken( const char* token, int rightEdge )
{
	if ( *token != '\\' )
		return token;

	token++;

	switch ( *token )
	{
	case '\0':
		return token;

	case 'w':
		menu_r = 255;
		menu_g = 255;
		menu_b = 255;
		break;

	case 'd':
		menu_r = 100;
		menu_g = 100;
		menu_b = 100;
		break;

	case 'y':
		menu_r = 255;
		menu_g = 210;
		menu_b = 64;
		break;

	case 'r':
		menu_r = 210;
		menu_g = 24;
		menu_b = 0;
		break;

	case 'g':
		menu_r = 24;
		menu_g = 210;
		menu_b = 0;
		break;

	case 'o':
		menu_r = 250;
		menu_g = 128;
		menu_b = 10;
		break;

	case 'c':
		menu_r = 10;
		menu_g = 250;
		menu_b = 250;
		break;

	case 'm':
		menu_r = 250;
		menu_g = 10;
		menu_b = 250;
		break;

	case 'R':
		menu_x = rightEdge;
		menu_ralign = TRUE;
		break;
	}

	return ++token;
}


int CHudMenu :: Draw( float flTime )
{
	// check for if menu is set to disappear
	if ( m_flShutoffTime > 0 )
	{
		if ( m_flShutoffTime <= gHUD.m_flTime )
		{  // times up, shutoff
			m_fMenuDisplayed = 0;
			m_iFlags &= ~HUD_ACTIVE;
			return 1;
		}
	}

	// don't draw the menu if the scoreboard is being shown
	if ( gViewPort && gViewPort->IsScoreBoardVisible() )
		return 1;

	// draw the menu, along the left-hand side of the screen

	// count the number of newlines
	int nlc = 0;
	int i;
	for ( i = 0; i < MAX_MENU_STRING && g_szMenuString[i] != '\0'; i++ )
	{
		if ( g_szMenuString[i] == '\n' )
			nlc++;
	}

	// center it
	int y = (ScreenHeight/2) - ((nlc/2)* gHUD.m_scrinfo.iCharHeight) - 40;
	int leftEdge = gHUD.GetBorderSize() + 5;
	int rightEdge = ScreenWidth / 2 - gHUD.GetBorderSize() - 5; // make sure it is above the say text

	menu_r		= 255;
	menu_g		= 255;
	menu_b		= 255;
	menu_x		= leftEdge;
	menu_ralign	 = FALSE;

	const char* sptr = g_szMenuString;
	
	while ( *sptr != '\0' )
	{
		if ( *sptr == '\\' )
		{
			sptr = ParseEscapeToken( sptr, rightEdge );
		}
		else if ( *sptr == '\n' )
		{
			menu_ralign	 = FALSE;
			menu_x		 = leftEdge;
			y			+= gHUD.m_scrinfo.iCharHeight;
			
			sptr++;
		}
		else
		{
			char menubuf[ MAX_MENU_STRING ];
			const char *ptr = sptr;
			while ( *sptr != '\0' && *sptr != '\n' && *sptr != '\\')
			{
				sptr++;
			}
			strncpy( menubuf, ptr, V_min( ( sptr - ptr), (int)sizeof( menubuf ) ));
			menubuf[ V_min( ( sptr - ptr), (int)(sizeof( menubuf )-1) ) ] = '\0';
			
			if ( menu_ralign )
			{		
				// IMPORTANT: Right-to-left rendered text does not parse escape tokens!
				menu_x = gHUD.DrawStringReverse( menu_x, y, 0, menubuf, menu_r, menu_g, menu_b, 0 );
			}
			else
			{
				menu_x = gHUD.DrawString( menu_x, y, rightEdge, menubuf, menu_r, menu_g, menu_b, 0 );
			}
		}
	}
	
	return 1;
}

// selects an item from the menu
void CHudMenu :: SelectMenuItem( int menu_item )
{
	// if menu_item is in a valid slot,  send a menuselect command to the server
	if ( (menu_item > 0) && (m_bitsValidSlots & (1 << (menu_item-1))) )
	{
		char szbuf[32];

		UTIL_FormatString( szbuf, "%s %d\n", m_bIsASMenu ? "as_menuselect" : "menuselect", menu_item );
		EngineClientCmd( szbuf );

		// remove the menu
		m_fMenuDisplayed = 0;
		m_iFlags &= ~HUD_ACTIVE;
	}
}


// Message handler for ShowMenu message
// takes four values:
//		short: a bitfield of keys that are valid input
//		char : the duration, in seconds, the menu should stay up. -1 means is stays until something is chosen.
//		byte : a boolean, TRUE if there is more string yet to be received before displaying the menu, FALSE if it's the last string
//		string: menu string to display
// The high bit of the continuation byte selects the AngelScript menu callback
int CHudMenu :: MsgFunc_ShowMenu( const char *pszName, int iSize, void *pbuf )
{
	char *temp = NULL;

	BEGIN_READ( pbuf, iSize );

	m_bitsValidSlots = READ_SHORT();
	int DisplayTime = READ_CHAR();
	int NeedMore = READ_BYTE();

	m_bIsASMenu = (NeedMore & MENU_FROM_ANGELSCRIPT) != 0;
	NeedMore &= ~MENU_FROM_ANGELSCRIPT;

	if ( DisplayTime > 0 )
		m_flShutoffTime = DisplayTime + gHUD.m_flTime;
	else
		m_flShutoffTime = -1;

	if ( m_bitsValidSlots )
	{
		if ( !m_fWaitingForMore ) // this is the start of a new menu
		{
			safe_strcpy( g_szPrelocalisedMenuString, READ_STRING(), MAX_MENU_STRING );
		}
		else
		{  // append to the current menu string
			UTIL_AppendString( g_szPrelocalisedMenuString, READ_STRING(), MAX_MENU_STRING );
		}

		if ( !NeedMore )
		{  // we have the whole string, so we can localise it now
			safe_strcpy( g_szMenuString, gHUD.m_TextMessage.BufferedLocaliseTextString( g_szPrelocalisedMenuString ), MAX_MENU_STRING );

			// Swap in characters
			if ( KB_ConvertString( g_szMenuString, &temp ) )
			{
				safe_strcpy( g_szMenuString, temp, MAX_MENU_STRING );
				free( temp );
			}
		}

		m_fMenuDisplayed = 1;
		m_iFlags |= HUD_ACTIVE;
	}
	else
	{
		m_fMenuDisplayed = 0; // no valid slots means that the menu should be turned off
		m_iFlags &= ~HUD_ACTIVE;
	}

	m_fWaitingForMore = NeedMore != 0;

	return 1;
}
