#include "extdll.h"
#include "hud_iface.h"
#include "Discord.h"
#include "client_support.h"
#include "IClient.h"
#include "cl_entity.h"
#include "net_api.h"
#include "parsemsg.h"
#include <limits.h>
#include <stdint.h>
#include <stdarg.h>

void AlertMessage(ALERT_TYPE type, char *format, ...);

static const char DISCORD_APP_ID[] = "396515353498484736";
static const char STEAM_APP_ID[] = "225840";
static const char DISCORD_DEFAULT_MODE[] = "2";
static const float DISCORD_CALLBACK_DELAY = 1.0f * CLOCKS_PER_SEC;
static const float DISCORD_CALLBACK_DOUBLE_WINDOW = 6.0f * CLOCKS_PER_SEC;
static const int DISCORD_AUTO_REGISTER = 1;

Discord::Discord()
{
	memset(m_szServerName, 0, sizeof(m_szServerName));
	memset(m_szPartyId, 0, sizeof(m_szPartyId));
	cl_discord = NULL;
	m_bGameStatus = false;
	m_cClock = clock();
	m_pUser = NULL;
	GenerateParty();
}

Discord::~Discord()
{
	DiscordExit();
}

void Discord::DiscordInitCVars(void)
{
	cl_discord = gEngfuncs.pfnRegisterVariable("cl_discord", (char *)DISCORD_DEFAULT_MODE,
		FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
}

void Discord::GenerateParty(void)
{
	if (m_pUser)
		UTIL_FormatString(m_szPartyId, "%s-%d", m_pUser->userId,
			gEngfuncs.pfnRandomLong(0, INT_MAX));
	else
		UTIL_FormatString(m_szPartyId, "invalid_discord-%d",
			gEngfuncs.pfnRandomLong(0, INT_MAX));
}

void Discord::SetServername(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	UTIL_CopyString(m_szServerName, READ_STRING(), sizeof(m_szServerName));

	if (!stricmp(m_szServerName, "Half-Life") || !strlen(m_szServerName))
		UTIL_CopyString(m_szServerName, "Unknown server", sizeof(m_szServerName));

	if (!m_bGameStatus)
		SetGamestatus(true);
	else
		DiscordUpdateInfo();
}

void Discord::DiscordUpdate(void)
{
	if ((float)(clock() - m_cClock) >= DISCORD_CALLBACK_DELAY)
	{
		Discord_RunCallbacks();

		if ((float)(clock() - m_cClock) <= DISCORD_CALLBACK_DOUBLE_WINDOW)
			Discord_RunCallbacks();
	}
}

void Discord::SetGamestatus(bool status)
{
	m_bGameStatus = status;

	if (!m_bGameStatus)
		GenerateParty();

	DiscordUpdateInfo();
}

void Discord::DiscordExit(void)
{
	AlertMessage(at_console, "[Discord] Exiting\n");
	Discord_Shutdown();
}

void Discord::DiscordInit(void)
{
	DiscordEventHandlers handlers = {};

	handlers.ready = DiscordReady;
	handlers.disconnected = DiscordDisconnected;
	handlers.errored = DiscordError;
	handlers.joinGame = DiscordJoin;
	Discord_Initialize(DISCORD_APP_ID, &handlers, DISCORD_AUTO_REGISTER, STEAM_APP_ID);
}

void Discord::DiscordUpdateInfo(void)
{
	int mode = cl_discord ? (int)cl_discord->value : DISCORD_PRESENCE_STATUS;

	if (mode < DISCORD_PRESENCE_STATUS)
		mode = DISCORD_PRESENCE_STATUS;

	DiscordRichPresence presence = {};
	char state[MAX_PRESENCE_TEXT] = {};
	char mapName[MAX_PRESENCE_TEXT] = {};
	char address[MAX_PRESENCE_TEXT] = {};

	presence.largeImageKey = "svenlogo";

	if (m_bGameStatus)
	{
		UTIL_CopyString(state, "In game", sizeof(state));
		presence.state = state;

		if (mode >= DISCORD_PRESENCE_SERVER)
		{
			presence.details = m_szServerName;
			StripMapFromPFN(gEngfuncs.pfnGetLevelName(), mapName);
			presence.largeImageText = mapName;

			int players = 0;

			for (int i = 1; i < gEngfuncs.GetMaxClients(); i++)
			{
				cl_entity_t *entity = gEngfuncs.GetEntityByIndex(i);

				if (entity && entity->player)
					players++;
			}

			presence.partySize = players;
			presence.partyMax = gEngfuncs.GetMaxClients();
		}

		if (mode >= DISCORD_PRESENCE_JOIN)
		{
			netadr_t serverAddress;

			g_pEngineClient->GetServerAddress(&serverAddress);

			if (serverAddress.type != NA_UNUSED)
			{
				UTIL_FormatString(address, "%s", gEngfuncs.pNetAPI->AdrToString(&serverAddress));
				presence.partyId = m_szPartyId;
				presence.joinSecret = address;
			}
		}
	}
	else
	{
		UTIL_CopyString(state, "In menu", sizeof(state));
		presence.state = state;
	}

	Discord_UpdatePresence(&presence);
}

void Discord::DiscordDisconnected(int errorCode, const char *message)
{
	AlertMessage(at_error, "[Discord] Disconnected (%d : %s)\n", errorCode, message);
}

void Discord::DiscordError(int errorCode, const char *message)
{
	AlertMessage(at_error, "[Discord] Error (%d : %s)\n", errorCode, message);
}

void Discord::DiscordJoinRequest(const DiscordUser *request)
{
	Discord_Respond(request->userId, DISCORD_REPLY_YES);
}

void Discord::DiscordSpectate(const char *secret)
{
}

void Discord::DiscordReady(const DiscordUser *user)
{
	DiscordUser *discordUser = new DiscordUser();
	Discord *discord = GetDiscord();

	discord->m_pUser = discordUser;
	discord->m_pUser->avatar = user->avatar;
	discord->m_pUser->discriminator = user->discriminator;
	discord->m_pUser->userId = user->userId;
	discord->m_pUser->username = user->username;
	discord->GenerateParty();
	AlertMessage(at_console, "[Discord] Ready!\n");
}

void Discord::DiscordJoin(const char *secret)
{
	char command[MAX_PRESENCE_TEXT] = {};

	AlertMessage(at_console, "[Discord] Join: %s\n", secret);
	UTIL_FormatString(command, "connect %s", secret);
	gEngfuncs.pfnClientCmd(command);
}
