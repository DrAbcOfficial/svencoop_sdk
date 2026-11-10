#ifndef CADMINLIST_H
#define CADMINLIST_H

#include "AdminLevel.h"

class CAdminList
{
public:
	enum { MAX_AUTHID_LENGTH = 44 };

	struct Entry
	{
		char         m_szAuthID[MAX_AUTHID_LENGTH + 1];
		AdminLevel_t m_AdminLevel;
		Entry       *m_pNext;
	};

	CAdminList();
	~CAdminList();

	void AddEntry( char *authID, AdminLevel_t adminLevel );
	const Entry *Find( const char *authID ) const;
	void Clear( void );

	Entry       *m_pEntries;
	unsigned int m_uiCount;
};

struct cvar_s;

extern cvar_s adminsfile;
extern cvar_s adminsicon;
extern CAdminList *g_pAdminList;

#endif
