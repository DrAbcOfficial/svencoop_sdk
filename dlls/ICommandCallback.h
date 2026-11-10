#ifndef ICOMMANDCALLBACK_H
#define ICOMMANDCALLBACK_H

class CCommand;

class ICommandCallback
{
public:
	virtual ~ICommandCallback() {}
	virtual void CommandCallback(const CCommand &command) = 0;
};

typedef void (*CommandCallback_t)(const CCommand &command);

#endif
