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
//  hud_msg.cpp
//

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "r_efx.h"
#include "clientobject.h"
#include "particle_engine.h"
#include "client_sound.h"
#include "client_electro.h"

#include "particleman.h"
extern IParticleMan *g_pParticleMan;

float g_iFogColor[3];
float g_iStartDist;
float g_iEndDist;

#define MAX_CLIENTS 32

// SDK PlayCDTrack range: -1 stops playback, 1-30 play media/Half-Life00-29
// READ_BYTE only returns -1 after a bad read
#define CDAUDIO_TRACK_STOP	-1
#define CDAUDIO_MAX_TRACK	30

#if defined( _TFC )
void ClearEventList( void );
#endif

/// USER-DEFINED SERVER MESSAGE HANDLERS

int CHud :: MsgFunc_ResetHUD(const char *pszName, int iSize, void *pbuf )
{
	// clear all hud data
	HUDLIST *pList = m_pHudList;

	while ( pList )
	{
		if ( pList->p )
			pList->p->Reset();
		pList = pList->pNext;
	}

	// reset sensitivity
	m_flMouseSensitivity = 0;

	// reset concussion effect
	m_flConcussionScale = 0;
	m_flConcussionFrequency = 1;
	m_flConcussionFadeTime = 0;

	return 1;
}

void CAM_ToFirstPerson(void);

void CHud :: MsgFunc_ViewMode( const char *pszName, int iSize, void *pbuf )
{
	CAM_ToFirstPerson();
}

void CHud :: MsgFunc_InitHUD( const char *pszName, int iSize, void *pbuf )
{
	g_pObjectHandler.CleanList();
	DisableElectricEffect( gEngfuncs.GetViewModel() );

	// prepare all hud data
	HUDLIST *pList = m_pHudList;

	while (pList)
	{
		if ( pList->p )
			pList->p->InitHUDData();
		pList = pList->pNext;
	}

#if defined( _TFC )
	ClearEventList();

	// catch up on any building events that are going on
	gEngfuncs.pfnServerCmd("sendevents");
#endif

	ParticleEngine()->m_bInitialized = false;
	CClient_SoundEngine::GetSoundEngine()->MapStarted();

	g_iFogColor[0] = g_iFogColor[1] = g_iFogColor[2] = 0;
	g_iStartDist = g_iEndDist = 0;

	if ( g_pParticleMan )
		 g_pParticleMan->ResetParticles();

	char szColor[64];

	UTIL_FormatString( szColor, "con_color \"%d %d %d\"", 180, 220, 255 );
	gEngfuncs.pfnClientCmd( szColor );
}

void CHud::MsgFunc_CdAudio(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int iTrack = READ_BYTE();

	if (iTrack < CDAUDIO_TRACK_STOP || !iTrack || iTrack > CDAUDIO_MAX_TRACK)
		return;

	if (iTrack <= 0)
	{
		gEngfuncs.pfnClientCmd("mp3 stop");
		return;
	}

	char szTrack[260];
	char szCommand[388];

	UTIL_FormatString(szTrack, "media/Half-Life%02d", iTrack - 1);
	UTIL_FormatString(szCommand, "mp3 play %s", szTrack);
	gEngfuncs.pfnClientCmd(szCommand);
}


int CHud :: MsgFunc_GameMode(const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	m_Teamplay = READ_BYTE();

	return 1;
}


int CHud :: MsgFunc_Damage(const char *pszName, int iSize, void *pbuf )
{
	int		armor, blood;
	Vector	from;
	int		i;
	float	count;
	
	BEGIN_READ( pbuf, iSize );
	armor = READ_BYTE();
	blood = READ_BYTE();

	for (i=0 ; i<3 ; i++)
		from[i] = READ_COORD();

	count = (blood * 0.5) + (armor * 0.5);

	if (count < 10)
		count = 10;

	// TODO: kick viewangles,  show damage visually

	return 1;
}

int CHud :: MsgFunc_Concuss( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	m_flConcussionScale = READ_FLOAT();
	m_flConcussionFrequency = READ_FLOAT();
	m_flConcussionFadeTime = READ_FLOAT();
	return 1;
}

int CHud::MsgFunc_VModelPos(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	m_bViewModelPos = READ_BYTE() == 1;

	if (m_bViewModelPos)
	{
		m_vecViewModelPos.x = READ_COORD();
		m_vecViewModelPos.y = READ_COORD();
		m_vecViewModelPos.z = READ_COORD();
	}

	return 1;
}

int CHud::MsgFunc_ClassicMode(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	m_bClassicMode = READ_BYTE() != 0;

	return m_bClassicMode;
}

int CHud::MsgFunc_Fog(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	READ_SHORT();

	int enabled = READ_BYTE();

	READ_COORD();
	READ_COORD();
	READ_COORD();
	READ_SHORT();

	float red = READ_BYTE();
	float green = READ_BYTE();
	float blue = READ_BYTE();
	int start = READ_SHORT();
	int end = READ_SHORT();

	if (enabled)
	{
		g_iFogColor[0] = red;
		g_iFogColor[1] = green;
		g_iFogColor[2] = blue;
		g_iStartDist = start;
		g_iEndDist = end;
	}
	else
	{
		g_iFogColor[0] = g_iFogColor[1] = g_iFogColor[2] = 0;
		g_iStartDist = g_iEndDist = -1;
	}

	return 1;
}
