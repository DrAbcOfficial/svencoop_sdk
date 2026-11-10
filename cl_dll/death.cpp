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
#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"

int __MsgFunc_DeathMsg( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	int killer = READ_BYTE();
	int victim = READ_BYTE();

	// The weapon name is not needed by the scoreboard highlight
	READ_STRING();

	if ( gViewPort )
		gViewPort->DeathMsg( killer, victim );

	return 1;
}

float g_ColorDefault[3] = {180.0f / 255.0f, 220.0f / 255.0f, 1.0f};
float g_ColorPlayer[3] = {0.6f, 0.75f, 1.0f};
float g_ColorServer[3] = {1.0f, 0.67f, 0.25f};
float g_ColorBlue[3] = {0.25f, 0.25f, 1.0f};
float g_ColorRed[3] = {1.0f, 0.25f, 0.25f};
float g_ColorYellow[3] = {1.0f, 1.0f, 0.25f};
float g_ColorGreen[3] = {0.15f, 1.0f, 0.4f};

float *GetClientColor( int clientIndex )
{
	if (clientIndex == 0)
		return g_ColorServer;

	if (clientIndex < 0)
		return g_ColorDefault;

	switch ( g_PlayerExtraInfo[clientIndex].teamnumber )
	{
	case 1:	return g_ColorBlue;
	case 2: return g_ColorRed;
	case 3: return g_ColorYellow;
	case 4: return g_ColorGreen;
	default	: return g_ColorPlayer;
	}
}
