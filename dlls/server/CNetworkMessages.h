#ifndef CNETWORKMESSAGES_H
#define CNETWORKMESSAGES_H

enum
{
	MAX_USER_MESSAGES = 256,
	USER_MESSAGE_VARIABLE_SIZE = -1,
	USER_MESSAGE_UNREGISTERED_INDEX = -2
};

struct UserMessageData_t
{
	const char		*m_pszName;
	int				m_iSize;
	unsigned int	m_uiIndex;
};

class CASDocumentation;

class CNetworkMessages
{
public:
	CNetworkMessages();
	~CNetworkMessages();

	static CNetworkMessages *GetInstance(void);

	const UserMessageData_t *GetUserMessageData(unsigned int index) const;
	unsigned int RegUserMsg_Intercept(const char *name, int size);

private:
	friend void RegisterSCScriptNetworkMessage(CASDocumentation *documentation);

	UserMessageData_t	m_UserMessages[MAX_USER_MESSAGES];
	unsigned int		m_uiFirstMessage;
	unsigned int		m_uiLastMessage;
	unsigned int		m_uiMessageCount;
};

int RegUserMsg_Intercept(const char *name, int size);

#endif
