#ifndef DISCORD_H
#define DISCORD_H

#include <time.h>

#define DISCORD_DYNAMIC_LIB
#include "discord_rpc.h"

struct cvar_s;

enum DiscordPresenceMode
{
	DISCORD_PRESENCE_STATUS = 0,
	DISCORD_PRESENCE_SERVER,
	DISCORD_PRESENCE_JOIN
};

class Discord
{
public:
	enum { MAX_PRESENCE_TEXT = 128 };

	Discord();
	virtual ~Discord();

	static Discord *GetDiscord(void)
	{
		static Discord *pDiscord;

		if (!pDiscord)
			pDiscord = new Discord;

		return pDiscord;
	}

	void DiscordInitCVars(void);
	void GenerateParty(void);
	void SetServername(const char *pszName, int iSize, void *pbuf);
	void DiscordUpdate(void);
	void SetGamestatus(bool status);
	void DiscordExit(void);
	void DiscordInit(void);
	void DiscordUpdateInfo(void);

	static void DiscordDisconnected(int errorCode, const char *message);
	static void DiscordError(int errorCode, const char *message);
	static void DiscordJoinRequest(const DiscordUser *request);
	static void DiscordSpectate(const char *secret);
	static void DiscordReady(const DiscordUser *user);
	static void DiscordJoin(const char *secret);

private:
	bool			m_bGameStatus;
	char			m_szServerName[MAX_PRESENCE_TEXT];
	char			m_szPartyId[MAX_PRESENCE_TEXT];
	clock_t			m_cClock;
	DiscordUser		*m_pUser;
	cvar_s			*cl_discord;
};

#endif
