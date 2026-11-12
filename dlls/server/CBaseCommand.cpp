#include "extdll.h"
#include "enginecallback.h"
#include "server/CBaseCommand.h"

CBaseCommand::CBaseCommand(ConCommandKind::Type kind, const char *name,
	CommandCallback_t callback, ConCommandFlags_t flags, const char *helpInfo)
	: CBaseConCommand(kind, name, flags, helpInfo), m_bInterfaceCallback(false)
{
	m_Callback.m_pFunction = callback;
}

CBaseCommand::CBaseCommand(ConCommandKind::Type kind, const char *name,
	ICommandCallback *callback, ConCommandFlags_t flags, const char *helpInfo)
	: CBaseConCommand(kind, name, flags, helpInfo), m_bInterfaceCallback(true)
{
	m_Callback.m_pInterface = callback;
}

void CBaseCommand::SetInterfaceCallback(ICommandCallback *callback)
{
	m_Callback.m_pInterface = callback;
	m_bInterfaceCallback = true;
}

void CBaseCommand::Dispatch(const CCommand &command)
{
	if (!m_Callback.m_pFunction)
	{
		ALERT(at_error, "Console: Internal error: callback for command '%s' is null\n", GetName());
		return;
	}

	if (m_bInterfaceCallback)
		m_Callback.m_pInterface->CommandCallback(command);
	else
		m_Callback.m_pFunction(command);
}
