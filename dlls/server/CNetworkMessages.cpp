#include "extdll.h"
#include "enginecallback.h"
#include "server/CNetworkMessages.h"

CNetworkMessages::CNetworkMessages()
	: m_uiFirstMessage(0), m_uiLastMessage(0), m_uiMessageCount(0)
{
	for (unsigned int i = 0; i < MAX_USER_MESSAGES; i++)
	{
		m_UserMessages[i].m_pszName = NULL;
		m_UserMessages[i].m_iSize = 0;
		m_UserMessages[i].m_uiIndex = USER_MESSAGE_UNREGISTERED_INDEX;
	}
}

CNetworkMessages::~CNetworkMessages()
{
}

CNetworkMessages *CNetworkMessages::GetInstance(void)
{
	static CNetworkMessages *pInstance = NULL;

	if (!pInstance)
		pInstance = new CNetworkMessages;

	return pInstance;
}

const UserMessageData_t *CNetworkMessages::GetUserMessageData(unsigned int index) const
{
	if (index < m_uiFirstMessage || index > m_uiLastMessage)
		return NULL;

	return &m_UserMessages[index];
}

unsigned int CNetworkMessages::RegUserMsg_Intercept(const char *name, int size)
{
	unsigned int index = (*g_engfuncs.pfnRegUserMsg)(name, size);

	if (index >= MAX_USER_MESSAGES)
	{
		ALERT(at_warning, "RegUserMsg_Intercept: message index is larger than or equal to array size!\n");
		return index;
	}

	m_UserMessages[index].m_pszName = name;
	m_UserMessages[index].m_iSize = size;
	m_UserMessages[index].m_uiIndex = index;

	if (!m_uiFirstMessage)
		m_uiFirstMessage = index;

	m_uiLastMessage = index;
	m_uiMessageCount++;

	return index;
}

int RegUserMsg_Intercept(const char *name, int size)
{
	return CNetworkMessages::GetInstance()->RegUserMsg_Intercept(name, size);
}
