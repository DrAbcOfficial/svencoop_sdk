#ifndef CCFGCOMMAND_H
#define CCFGCOMMAND_H

#include <stddef.h>

class CCfgCommand
{
public:
	enum { MAX_ARGUMENTS = 5 };

	CCfgCommand();
	virtual ~CCfgCommand();
	virtual bool operator==(const char *command);
	virtual bool operator!=(const char *command);

	void ClassifyCommand(void);
	void SetCommand(const char *command, const char *arg0 = NULL,
		const char *arg1 = NULL, const char *arg2 = NULL,
		const char *arg3 = NULL, const char *arg4 = NULL);
	const char *GetCommand(void);
	const char *GetArg(int index);
	void SetArg(const char *argument, unsigned int index);

	CCfgCommand	*m_pNext;
	CCfgCommand	*m_pPrev;
	char		*m_pszCommand;
	char		*m_Arguments[MAX_ARGUMENTS];
	bool		m_bPlayerCommand;
	bool		m_bMapCommand;
	bool		m_bSkillCommand;
	bool		m_bUnknownCommand;
};

#endif
