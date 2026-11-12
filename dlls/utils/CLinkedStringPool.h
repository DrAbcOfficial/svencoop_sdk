#ifndef CLINKEDSTRINGPOOL_H
#define CLINKEDSTRINGPOOL_H

class CLinkedStringPool
{
public:
	CLinkedStringPool();
	~CLinkedStringPool();

	const char *AllocateString(const char *text);
	void Clear(void);

private:
	struct String_t
	{
		String_t	*m_pNext;
		char		*m_pszString;
	};

	String_t *m_pStrings;
};

#endif
