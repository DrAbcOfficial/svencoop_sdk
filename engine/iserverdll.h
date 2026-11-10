//
#ifndef ISERVERDLL_H
#define ISERVERDLL_H
#ifdef _WIN32
#pragma once
#endif

#include "interface.h"

class IServerDLL : public IBaseInterface
{
public:
	virtual ~IServerDLL() {}

	virtual void Init(CreateInterfaceFn _EngineServerIfaceFn) = 0;
	virtual void EndFrame(void) = 0;
	virtual void NotifyClients(const char* _Text) = 0;
	virtual void OnChangeLevel(const char* _MapName) = 0;
};

extern IServerDLL* g_pServerDLL;

#define SERVERDLL_INTERFACE_VERSION "SCServerDLL003"

#endif // ISERVERDLL_H
