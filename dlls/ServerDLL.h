#ifndef SERVERDLL_H
#define SERVERDLL_H

#include "iserverdll.h"

class CServerDLL : public IServerDLL
{
public:
	CServerDLL();

	void Init(CreateInterfaceFn engineFactory);
	void EndFrame(void);
	void NotifyClients(const char *text);
	void OnChangeLevel(const char *mapName);

	static CServerDLL	*s_pServerDLL;

private:
	CreateInterfaceFn	m_pfnEngineFactory;
};

extern char g_szChangeLevelName[64];

void EndFrame(void);

#endif
