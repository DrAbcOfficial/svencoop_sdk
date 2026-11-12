#include "extdll.h"
#include "util.h"
#include "server/server_support.h"
#include "cbase.h"
#include "gamerules/gamerules.h"
#include "server/server_filesystem.h"
#include "server/CAdminList.h"

#define ADMIN_LIST_LINE_LENGTH 128

cvar_t adminsfile = { "adminsfile", "admins.txt", FCVAR_SERVER | FCVAR_UNLOGGED };
cvar_t adminsicon = { "sv_admin_icon", "gfx/icons/admin.tga", FCVAR_SERVER | FCVAR_UNLOGGED };

CAdminList *g_pAdminList = NULL;

CAdminList::CAdminList()
	: m_pEntries( NULL ), m_uiCount( 0 )
{
}

CAdminList::~CAdminList()
{
	Clear();
}

void CAdminList::AddEntry( char *authID, AdminLevel_t adminLevel )
{
	unsigned int length = strlen( authID );

	if ( !length )
		return;

	if ( length > MAX_AUTHID_LENGTH )
		authID[MAX_AUTHID_LENGTH] = 0;
	else
		authID[length] = 0;

	Entry *entry = m_pEntries;
	Entry *previous = NULL;

	while ( entry )
	{
		if ( !stricmp( entry->m_szAuthID, authID ) )
			return;

		previous = entry;
		entry = entry->m_pNext;
	}

	entry = new Entry;
	memset( entry->m_szAuthID, 0, sizeof( entry->m_szAuthID ) );
	UTIL_CopyString( entry->m_szAuthID, authID, sizeof( entry->m_szAuthID ) );
	entry->m_AdminLevel = adminLevel;
	entry->m_pNext = NULL;

	if ( previous )
		previous->m_pNext = entry;
	else
		m_pEntries = entry;

	m_uiCount++;
}

const CAdminList::Entry *CAdminList::Find( const char *authID ) const
{
	if ( !m_uiCount || !m_pEntries )
		return NULL;

	for ( Entry *entry = m_pEntries; entry; entry = entry->m_pNext )
	{
		if ( !stricmp( entry->m_szAuthID, authID ) )
			return entry;
	}

	return NULL;
}

void CAdminList::Clear( void )
{
	if ( m_pEntries && m_uiCount )
	{
		Entry *entry = m_pEntries;

		do
		{
			Entry *next = entry->m_pNext;

			delete entry;
			entry = next;
		} while ( entry );

		m_pEntries = NULL;
		m_uiCount = 0;
	}
}

void CHalfLifeMultiplay::LoadServerAdmins( void )
{
	cvar_t *adminsFile = CVAR_GET_POINTER( "adminsfile" );
	const char *fileName = adminsFile->string;

	if ( !fileName || !strlen( fileName ) )
	{
		ALERT( at_warning, "No server administrator list file defined.\n" );
		return;
	}

	FileHandle_t file = g_pFileSystem->Open( fileName, "r", "GAMECONFIG" );

	if ( !file )
	{
		ALERT( at_warning, "Server administrator list file '%s' not found.\n", fileName );
		return;
	}

	g_pAdminList = new CAdminList;

	int administrators = 0;
	int owners = 0;
	char line[ADMIN_LIST_LINE_LENGTH + 1];

	memset( line, 0, sizeof( line ) );

	while ( !g_pFileSystem->EndOfFile( file ) )
	{
		if ( !g_pFileSystem->ReadLine( line, ADMIN_LIST_LINE_LENGTH, file ) ||
		     !line[0] || line[0] == '\r' || line[0] == '\n' ||
		     line[0] == '\t' || line[0] == ' ' || line[0] == '#' || line[0] == '/' )
			continue;

		const char delimiters[] = { '\r', '\n', '\t', ' ', '#', '/' };

		for ( unsigned int i = 0; i < sizeof( delimiters ); i++ )
		{
			char *delimiter = strchr( line, delimiters[i] );

			if ( delimiter )
				*delimiter = 0;
		}

		bool owner = line[0] == '*';
		char *authID = owner ? line + 1 : line;

		if ( *authID )
		{
			authID[CAdminList::MAX_AUTHID_LENGTH] = 0;

			if ( owner )
			{
				ALERT( at_aiconsole, "Added server owner: %s\n", authID );
				owners++;
			}
			else
			{
				ALERT( at_aiconsole, "Added server administrator: %s\n", authID );
				administrators++;
			}

			g_pAdminList->AddEntry( authID, owner ? ADMIN_OWNER : ADMIN_YES );
		}

		memset( line, 0, sizeof( line ) );
	}

	ALERT( at_console, "%d server administrator(s) and %d owner(s) read.\n", administrators, owners );
	g_pFileSystem->Close( file );
}
