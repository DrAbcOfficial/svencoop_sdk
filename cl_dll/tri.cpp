//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

// Triangle rendering, if any

#include "hud.h"
#include "cl_util.h"
#include "SvenVersion.h"

// Triangle rendering apis are in gEngfuncs.pTriAPI

#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "triangleapi.h"
#include "Exports.h"

#include "particleman.h"
#include "tri.h"
#include "particle_engine.h"
#include "client_sound.h"
#include "Discord.h"
#include "portal_manager.h"
#include "SDL2/SDL_opengl.h"
#include "net_api.h"
#include "in_defs.h"
#include "IClient.h"
#include "client_support.h"
extern IParticleMan *g_pParticleMan;
extern cvar_t *cl_showstats;

/*
=================
HUD_DrawNormalTriangles

Non-transparent triangles-- add them here
=================
*/
void DLLEXPORT HUD_DrawNormalTriangles( void )
{
	P_DrawPortals();
}

#if defined( _TFC )
void RunEventList( void );
#endif

/*
=================
HUD_DrawTransparentTriangles

Render any triangles with transparent rendermode needs here
=================
*/
void DLLEXPORT HUD_DrawTransparentTriangles( void )
{
	glDisable(GL_FOG);
	ParticleEngine()->EngineThink();
	CClient_SoundEngine::GetSoundEngine()->Update();
	Discord::GetDiscord()->DiscordUpdateInfo();

//	RecClDrawTransparentTriangles();

#if defined( _TFC )
	RunEventList();
#endif

	if ( g_pParticleMan )
		 g_pParticleMan->Update();
}

/*
=================
HUD_DrawOrthoTriangles

=================
*/
void HUD_DrawOrthoTriangles( void )
{
	float xScale = ScreenWidth / 720.0f;
	float yScale = ScreenHeight / 480.0f;

	if (cl_showstats && cl_showstats->value >= 1)
	{
		int y = gHUD.GetHeight() / 10 + gHUD.GetBorderSize();
		float labelX = gHUD.GetBorderSize();
		float valueX = xScale * 110 + labelX;
		float lineHeight = yScale * 14;

		char label[128 + 1];
		net_status_t status;
		char clientAddress[128 + 1];
		char serverVersion[33];
		char serverBuild[129];
		char serverAddress[128 + 1];
		char mapName[128];

		memset(label, 0, sizeof(label));
		memset(&status, 0, sizeof(status));
		gEngfuncs.pNetAPI->Status(&status);

		UTIL_CopyString(clientAddress, "Not connected", sizeof(clientAddress));

		if (status.connected > 0)
			g_pEngineClient->NET_AdrToString(status.local_address, 0, clientAddress, sizeof(clientAddress));

		UTIL_CopyString(serverVersion, "Unknown", sizeof(serverVersion));
		UTIL_CopyString(serverBuild, "Unknown", sizeof(serverBuild));
		UTIL_CopyString(serverVersion, gHUD.m_szServerVersion, sizeof(serverVersion));
		UTIL_CopyString(serverBuild, gHUD.m_szServerBuild, sizeof(serverBuild));
		UTIL_CopyString(serverAddress, "Unknown", sizeof(serverAddress));

		if (status.connected > 0)
			g_pEngineClient->NET_AdrToString(status.remote_address, 0, serverAddress, sizeof(serverAddress));

		if (!stricmp("loopback", serverAddress))
		{
			UTIL_CopyString(label, serverAddress, sizeof(label));
			UTIL_CopyString(serverAddress, clientAddress, sizeof(serverAddress));
			UTIL_CopyString(clientAddress, label, sizeof(clientAddress));
			memset(label, 0, sizeof(label));
		}

		memset(mapName, 0, sizeof(mapName));
		StripMapFromPFN(gEngfuncs.pfnGetLevelName(), mapName);

		cl_entity_t *player = gEngfuncs.GetLocalPlayer();
		Vector origin = player->origin;
		Vector angles = player->angles;
		Vector velocity = origin - Vector(player->ph[(player->current_position - 1) % HISTORY_MAX].origin);

		int left = (int)labelX;
		int right = (int)valueX;

		UTIL_CopyString(label, "Client version:", sizeof(label));
		gHUD.DrawString(left, y, ScreenWidth, label, 160, 160, 255, 0);
		gHUD.DrawString(right, y, ScreenWidth, SVEN_GAME_VERSION_STRING, 255, 160, 160, 0);
		y = (int)(y + lineHeight);
		memset(label, 0, sizeof(label));

		UTIL_CopyString(label, "Client build:", sizeof(label));
		gHUD.DrawString(left, y, ScreenWidth, label, 160, 160, 255, 0);
		gHUD.DrawString(right, y, ScreenWidth, SVEN_BUILD_STRING, 255, 160, 160, 0);
		y = (int)(y + lineHeight);
		memset(label, 0, sizeof(label));

		UTIL_CopyString(label, "Client address:", sizeof(label));
		gHUD.DrawString(left, y, ScreenWidth, label, 160, 160, 255, 0);
		gHUD.DrawString(right, y, ScreenWidth, clientAddress, 255, 160, 160, 0);
		y = (int)(y + lineHeight);
		y = (int)(y + lineHeight);
		memset(label, 0, sizeof(label));

		UTIL_CopyString(label, "Server version:", sizeof(label));
		gHUD.DrawString(left, y, ScreenWidth, label, 160, 160, 255, 0);
		gHUD.DrawString(right, y, ScreenWidth, serverVersion, 255, 160, 160, 0);
		y = (int)(y + lineHeight);
		memset(label, 0, sizeof(label));

		UTIL_CopyString(label, "Server build:", sizeof(label));
		gHUD.DrawString(left, y, ScreenWidth, label, 160, 160, 255, 0);
		gHUD.DrawString(right, y, ScreenWidth, serverBuild, 255, 160, 160, 0);
		y = (int)(y + lineHeight);
		memset(label, 0, sizeof(label));

		UTIL_CopyString(label, "Server address:", sizeof(label));
		gHUD.DrawString(left, y, ScreenWidth, label, 160, 160, 255, 0);
		gHUD.DrawString(right, y, ScreenWidth, serverAddress, 255, 160, 160, 0);
		y = (int)(y + lineHeight);
		y = (int)(y + lineHeight);
		memset(label, 0, sizeof(label));

		UTIL_CopyString(label, "Map:", sizeof(label));
		gHUD.DrawString(left, y, ScreenWidth, label, 160, 160, 255, 0);
		gHUD.DrawString(right, y, ScreenWidth, mapName, 255, 160, 160, 0);
		memset(label, 0, sizeof(label));

		if (cl_showstats->value >= 2)
		{
			CClient_SoundEngine *soundEngine = CClient_SoundEngine::GetSoundEngine();
			unsigned int soundMemory;

			y = (int)(y + lineHeight);
			y = (int)(y + lineHeight);

			UTIL_CopyString(label, "Active sounds:", sizeof(label));
			gHUD.DrawString(left, y, ScreenWidth, label, 160, 160, 255, 0);
			UTIL_FormatString(label, "%d", soundEngine->m_iActiveSounds);
			gHUD.DrawString(right, y, ScreenWidth, label, 255, 160, 160, 0);
			y = (int)(y + lineHeight);
			memset(label, 0, sizeof(label));

			soundEngine->m_pSystem->getMemoryInfo(FMOD_MEMBITS_ALL, FMOD_EVENT_MEMBITS_ALL, &soundMemory, NULL);
			soundMemory >>= 20;

			UTIL_CopyString(label, "Sound memory:", sizeof(label));
			gHUD.DrawString(left, y, ScreenWidth, label, 160, 160, 255, 0);
			UTIL_FormatString(label, "%d MB", soundMemory);
			gHUD.DrawString(right, y, ScreenWidth, label, 255, 160, 160, 0);
			y = (int)(y + lineHeight);
			memset(label, 0, sizeof(label));

			UTIL_CopyString(label, "Active particles:", sizeof(label));
			gHUD.DrawString(left, y, ScreenWidth, label, 160, 160, 255, 0);
			UTIL_FormatString(label, "%d", ParticleEngine()->m_iParticles);
			gHUD.DrawString(right, y, ScreenWidth, label, 255, 160, 160, 0);
			y = (int)(y + lineHeight);
			y = (int)(y + lineHeight);
			memset(label, 0, sizeof(label));

			UTIL_CopyString(label, "Origin:", sizeof(label));
			gHUD.DrawString(left, y, ScreenWidth, label, 160, 160, 255, 0);
			UTIL_FormatString(label, "X:%0.0f  Y:%0.0f  Z:%0.0f", origin.x, origin.y, origin.z);
			gHUD.DrawString(right, y, ScreenWidth, label, 255, 160, 160, 0);
			y = (int)(y + lineHeight);
			memset(label, 0, sizeof(label));

			angles[PITCH] *= 9.0;

			UTIL_CopyString(label, "Angles:", sizeof(label));
			gHUD.DrawString(left, y, ScreenWidth, label, 160, 160, 255, 0);
			UTIL_FormatString(label, "P:%0.0f  Y:%0.0f  R:%0.0f", angles[PITCH], angles[YAW], angles[ROLL]);
			gHUD.DrawString(right, y, ScreenWidth, label, 255, 160, 160, 0);
			y = (int)(y + lineHeight);
			memset(label, 0, sizeof(label));

			UTIL_CopyString(label, "Velocity:", sizeof(label));
			gHUD.DrawString(left, y, ScreenWidth, label, 160, 160, 255, 0);
			UTIL_FormatString(label, "X:%0.0f  Y:%0.0f  Z:%0.0f", velocity.x, velocity.y, velocity.z);
			gHUD.DrawString(right, y, ScreenWidth, label, 255, 160, 160, 0);
		}
	}
}
