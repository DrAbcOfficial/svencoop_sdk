#ifndef CASHOOKARGUMENTS_H
#define CASHOOKARGUMENTS_H

namespace HookStopMode
{
enum Type : int
{
	ON_HANDLED = 0,
	MODULE_HANDLED,
	CALL_ALL
};
}

namespace HookFlag
{
enum Type : int
{
	NONE = 0,
	ADMIN_HOOK = 1 << 0
};
}

class CASHookArguments
{
public:
	HookStopMode::Type	m_StopMode;
	HookFlag::Type		m_Flags;
	const char			*m_pszDescription;
};

const char *HookStopModeToString(HookStopMode::Type mode);
HookStopMode::Type StringToHookStopMode(const char *text);
const char *HookFlagToString(HookFlag::Type flag);
HookFlag::Type StringToHookFlag(const char *text);

#endif
