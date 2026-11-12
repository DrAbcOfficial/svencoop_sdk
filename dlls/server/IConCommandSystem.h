#ifndef ICONCOMMANDSYSTEM_H
#define ICONCOMMANDSYSTEM_H

class CBasePlayer;
class CCommand;

class IConCommandSystem
{
public:
	virtual ~IConCommandSystem() {}
	virtual int Execute(const CCommand &command, CBasePlayer *player, bool consoleCommand) = 0;
};

#endif
