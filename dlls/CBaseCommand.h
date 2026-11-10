#ifndef CBASECOMMAND_H
#define CBASECOMMAND_H

#include "CBaseConCommand.h"
#include "ICommandCallback.h"

class CBaseCommand : public CBaseConCommand
{
public:
	CBaseCommand(ConCommandKind::Type kind, const char *name,
		CommandCallback_t callback, ConCommandFlags_t flags, const char *helpInfo);
	CBaseCommand(ConCommandKind::Type kind, const char *name,
		ICommandCallback *callback, ConCommandFlags_t flags, const char *helpInfo);

	void SetInterfaceCallback(ICommandCallback *callback);
	void Dispatch(const CCommand &command);

private:
	union Callback_t
	{
		CommandCallback_t	m_pFunction;
		ICommandCallback	*m_pInterface;
	};

	Callback_t	m_Callback;
	bool		m_bInterfaceCallback;
};

#endif
