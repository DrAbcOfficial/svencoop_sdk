#include "extdll.h"
#include "server/CBaseConCommand.h"

CBaseConCommand *CBaseConCommand::m_pHead = NULL;

CBaseConCommand::CBaseConCommand(ConCommandKind::Type kind, const char *name,
	ConCommandFlags_t flags, const char *helpInfo)
	: m_Kind(kind), m_Flags(flags), m_pszName(name),
	m_pszHelpInfo(helpInfo ? helpInfo : ""), m_pNext(m_pHead)
{
	m_pHead = this;
}

CBaseConCommand::~CBaseConCommand()
{
}
