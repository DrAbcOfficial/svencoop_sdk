#ifndef CBASECONCOMMAND_H
#define CBASECONCOMMAND_H

#include "server/ConCommandDefs.h"

class CConCommandSystem;

class CBaseConCommand
{
public:
	CBaseConCommand(ConCommandKind::Type kind, const char *name,
		ConCommandFlags_t flags, const char *helpInfo);

	virtual void Visit(CConCommandSystem &system, void *context) = 0;
	virtual ~CBaseConCommand();

	ConCommandKind::Type GetKind(void) const { return m_Kind; }
	ConCommandFlags_t GetFlags(void) const { return m_Flags; }
	const char *GetName(void) const { return m_pszName; }
	const char *GetHelpInfo(void) const { return m_pszHelpInfo; }
	CBaseConCommand *GetNext(void) const { return m_pNext; }
	static CBaseConCommand *GetHead(void) { return m_pHead; }

private:
	static CBaseConCommand	*m_pHead;

	ConCommandKind::Type		m_Kind;
	ConCommandFlags_t		m_Flags;
	const char				*m_pszName;
	const char				*m_pszHelpInfo;
	CBaseConCommand			*m_pNext;
};

#endif
