#include "extdll.h"
#include "util.h"
#include "CCommand.h"
#include "CCfgCommand.h"
#include "server_support.h"
#include "server_parse.h"

const char *GetArg_EngArgV(void *context, int index)
{
	return CMD_ARGV(*(int *)context + index);
}

const char *GetArg_ArgV(void *context, int index)
{
	return ((const char **)context)[index];
}

const char *GetArg_CCommand(void *context, int index)
{
	return ((CCommand *)context)->Arg(index);
}

CCommand::CCommand(int startArgument)
{
	Reset();
	Initialize(CMD_ARGC() - startArgument, &startArgument, GetArg_EngArgV);
}

CCommand::CCommand(int count, char **arguments)
{
	Reset();
	Initialize(count, arguments, GetArg_ArgV);
}

CCommand::CCommand(CCfgCommand *command)
{
	Reset();

	const char *arguments[CCfgCommand::MAX_ARGUMENTS + 1];

	arguments[0] = command->GetArg(0);

	if (!arguments[0])
		return;

	int count = 1;

	for (; count <= CCfgCommand::MAX_ARGUMENTS; count++)
	{
		const char *argument = command->GetArg(count);

		if (!argument || !*argument)
			break;

		arguments[count] = argument;
	}

	Initialize(count, arguments, GetArg_ArgV);
}

CCommand::CCommand(const char *command)
{
	Initialize(command);
}

CCommand::CCommand(const char *command, const char *argument)
{
	const char *arguments[] = { command, argument };

	Reset();
	Initialize(ARRAYSIZE(arguments), arguments, GetArg_ArgV);
}

CCommand::CCommand(const CCommand &command)
{
	*this = command;
}

CCommand::~CCommand()
{
}

CCommand &CCommand::operator=(const CCommand &command)
{
	if (this != &command)
	{
		Reset();
		Initialize(command.ArgC(), (void *)&command, GetArg_CCommand);
	}

	return *this;
}

void CCommand::Reset(void)
{
	m_iArgCount = 0;
	m_iArg0Length = 0;
	m_szCommand[0] = 0;
}

bool CCommand::Initialize(int count, void *context, const char *(*getArgument)(void *, int))
{
	m_iArgCount = count;
	char *command = m_szCommand;
	char *arguments = m_szArguments;

	for (int i = 0; i < m_iArgCount; i++)
	{
		const char *argument = getArgument(context, i);
		unsigned int length = strlen(argument);
		bool quoted = strchr(argument, ' ') != NULL;

		if (quoted && sizeof(m_szCommand) - (command - m_szCommand) > 1)
			*command++ = '"';

		if (length >= sizeof(m_szCommand) - (command - m_szCommand))
		{
			m_iArgCount = 0;
		}
		else
		{
			strncpy(command, argument, length + 1);
			command += length;
		}

		if (quoted && sizeof(m_szCommand) - (command - m_szCommand) > 1)
			*command++ = '"';

		if (i < m_iArgCount - 1 && sizeof(m_szCommand) - (command - m_szCommand) > 1)
			*command++ = ' ';

		if (!m_iArgCount || command <= m_szCommand)
		{
			m_iArgCount = 0;
			return false;
		}

		m_Arguments[i] = arguments;
		strncpy(arguments, argument, length + 1);
		arguments += length + 1;

		if (!i)
			m_iArg0Length = length;
	}

	return m_iArgCount > 0;
}

bool CCommand::Initialize(const char *command)
{
	Reset();

	if (!UTIL_CopyString(m_szCommand, command, sizeof(m_szCommand)))
		return false;

	char *input = (char *)command;
	char *arguments = m_szArguments;

	while (*input)
	{
		while (*input && (unsigned char)*input <= ' ' && *input != '\n')
			input++;

		if (!*input || *input == '\n')
			break;

		input = COM_Parse(input);

		if (!input)
			break;

		unsigned int length = strlen(com_token);

		if (m_iArgCount < MAX_ARGUMENTS)
		{
			if (length >= sizeof(m_szArguments) - (arguments - m_szArguments))
			{
				m_iArgCount = 0;
				return false;
			}

			m_Arguments[m_iArgCount++] = arguments;
			strncpy(arguments, com_token, length + 1);
			arguments += length + 1;
		}

		if (m_iArgCount == 1)
			m_iArg0Length = length;
	}

	return m_iArgCount > 0;
}

int CCommand::ArgC(void) const
{
	return m_iArgCount;
}

const char *const *CCommand::ArgV(void) const
{
	return m_Arguments;
}

const char *CCommand::Arg(int index) const
{
	return index >= 0 && index < m_iArgCount ? m_Arguments[index] : "";
}

const char *CCommand::operator[](int index) const
{
	return Arg(index);
}

const char *CCommand::GetCommandString(void) const
{
	return m_iArgCount ? m_szCommand : "";
}

const char *CCommand::GetArgumentsString(void) const
{
	return m_iArg0Length ? m_szCommand + m_iArg0Length : "";
}

const char *CCommand::FindArg(const char *name) const
{
	for (int i = 0; i < m_iArgCount; i++)
	{
		if (!strcmp(name, Arg(i)))
			return Arg(i + 1);
	}

	return NULL;
}

int CCommand::FindIntArg(const char *name, int defaultValue) const
{
	const char *argument = FindArg(name);

	return argument ? atoi(argument) : defaultValue;
}
