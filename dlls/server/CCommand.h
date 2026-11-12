#ifndef CCOMMAND_H
#define CCOMMAND_H

class CCfgCommand;

class CCommand
{
public:
	enum
	{
		MAX_ARGUMENTS = 80,
		MAX_COMMAND_LENGTH = 512
	};

	CCommand(int startArgument = 0);
	CCommand(int count, char **arguments);
	CCommand(CCfgCommand *command);
	CCommand(const char *command);
	CCommand(const char *command, const char *argument);
	CCommand(const CCommand &command);
	~CCommand();

	CCommand &operator=(const CCommand &command);
	void Reset(void);
	bool Initialize(const char *command);
	int ArgC(void) const;
	const char *const *ArgV(void) const;
	const char *Arg(int index) const;
	const char *operator[](int index) const;
	const char *GetCommandString(void) const;
	const char *GetArgumentsString(void) const;
	const char *FindArg(const char *name) const;
	int FindIntArg(const char *name, int defaultValue = 0) const;

private:
	bool Initialize(int count, void *context, const char *(*getArgument)(void *, int));

	int			m_iArgCount;
	int			m_iArg0Length;
	char		m_szCommand[MAX_COMMAND_LENGTH];
	char		m_szArguments[MAX_COMMAND_LENGTH];
	const char	*m_Arguments[MAX_ARGUMENTS];
};

#endif
