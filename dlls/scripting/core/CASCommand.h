#ifndef CASCOMMAND_H
#define CASCOMMAND_H

#include "server/CCommand.h"
#include "CString.h"

class CASDocumentation;

class CASCommand : public CCommand
{
public:
	CASCommand(int startArgument = 0);
	CASCommand(int count, char **arguments);
	CASCommand(const char *command);
	CASCommand(CCfgCommand *command);
	CASCommand(const char *command, const char *argument);
	CASCommand(const CCommand &command);
	~CASCommand();

	CASCommand &operator=(const CCommand &command);
	void AddRef(void) const;
	void Release(void) const;

	int ASFindIntArg(const CString &name, int defaultValue = 0) const;
	CString ASFindArg(const CString &name) const;
	CString ASArg(int index) const;
	CString ASGetArgumentsString(void) const;
	CString ASGetCommandString(void) const;

private:
	mutable int	m_iRefCount;
};

#endif
