#include "extdll.h"
#include "util.h"
#include "cdll_dll.h"
#include "server_support.h"
#include "iengineserver.h"
#include "ModelUtils.h"
#include "ServerDLL.h"

IEngineServer *g_pEngineServer;
CServerDLL *CServerDLL::s_pServerDLL;
char g_szChangeLevelName[64];

CServerDLL::CServerDLL()
{
	s_pServerDLL = this;
	m_pfnEngineFactory = NULL;
}

void CServerDLL::Init(CreateInterfaceFn engineFactory)
{
	m_pfnEngineFactory = engineFactory;

	if (!m_pfnEngineFactory)
		UTIL_HostError("Could not get Svengine factory.\n", -1);

	g_pEngineServer = (IEngineServer *)m_pfnEngineFactory(IENGINESERVER_INTERFACE_VERSION, NULL);

	if (!g_pEngineServer)
		UTIL_HostError("Unable to initialize Svengine server interface.\n", -1);
}

void CServerDLL::EndFrame(void)
{
	::EndFrame();
}

void CServerDLL::NotifyClients(const char *text)
{
	UTIL_ClientPrintAll(HUD_PRINTTALK, text);
}

void CServerDLL::OnChangeLevel(const char *mapName)
{
	memset(g_szChangeLevelName, 0, sizeof(g_szChangeLevelName));

	if (!mapName || !strlen(mapName))
		return;

	UTIL_CopyString(g_szChangeLevelName, mapName, sizeof(g_szChangeLevelName));
	UTIL_LogPrintf("Changing to level \"%s\".\n", g_szChangeLevelName);
}

EXPOSE_SINGLE_INTERFACE(CServerDLL, IServerDLL, SERVERDLL_INTERFACE_VERSION);
