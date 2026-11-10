#ifndef CCONCOMMANDSYSTEM_H
#define CCONCOMMANDSYSTEM_H

#include "IConCommandSystem.h"
#include "CASExtensionList.h"
#include <map>
#include <stack>

class CBaseConCommand;
class CConCommand;
class CClientCommand;
class CCVar;
class CString;

class CConCommandSystem : public IConCommandSystem
{
public:
	CConCommandSystem();
	virtual ~CConCommandSystem();
	static CConCommandSystem *GetInstance(void);
	virtual int Execute(const CCommand &command, CBasePlayer *player, bool consoleCommand);

	int Execute(CBaseConCommand *target, const CCommand &command, CBasePlayer *player, bool consoleCommand);
	void Initialize(void);
	void Shutdown(void);
	CBasePlayer *GetCurrentPlayer(void) const;
	void SetUnhandledCommandHandler(IConCommandSystem *handler);
	void FindCommands(const char *prefix, CBasePlayer *player) const;
	void Visit(CConCommand &command, void *context);
	void Visit(CClientCommand &command, void *context);
	void Visit(CCVar &variable, void *context);
	void CVarChanged(const CCVar &variable, const CString &oldValue, float oldFloat);

private:
	typedef std::map<CString, CBaseConCommand *, CStringCaseInsensitiveLess> CommandMap_t;

	int							m_iState;
	CommandMap_t				m_Commands;
	std::stack<CBasePlayer *>	m_Players;
	IConCommandSystem *m_pUnhandledCommandHandler;
};

#endif
