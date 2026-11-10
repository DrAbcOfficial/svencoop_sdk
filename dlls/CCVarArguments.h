#ifndef CCVARARGUMENTS_H
#define CCVARARGUMENTS_H

#include "ConCommandDefs.h"
#include "ICVarCallback.h"

struct CCVarArguments
{
	ConCommandFlags_t	m_Flags;
	const char *m_pszHelpInfo;
	const char *m_pszDefaultValue;
	union
	{
		CVarCallback_t	m_pFunctionCallback;
		ICVarCallback *m_pInterfaceCallback;
	};
	bool m_bInterfaceCallback;
};

#endif
