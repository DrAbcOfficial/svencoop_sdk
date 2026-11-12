#ifndef ASDIRECTORYENUMERATION_H
#define ASDIRECTORYENUMERATION_H

#include "scripting/core/CASDirectory.h"
#include "scripting/core/CASLog.h"
#include "server/server_support.h"
#include <string.h>

template <class Operation>
bool EnumerateDirectories( Operation &operation, unsigned int &level, const CASDirectory *directory,
                           const char *path, char *suffix, unsigned int size )
{
	if ( !directory )
		return false;

	if ( !UTIL_AppendString( suffix, directory->m_szName, size ) )
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT,
		                              "CASDirectoryList::ClearTempDirectories: Failed to append name '%s'!\n", directory->m_szName );
		return false;
	}

	operation( level, directory, path );

	unsigned int length = strlen( directory->m_szName );

	if ( size < length + 1 )
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT,
		                              "CASDirectoryList::ClearTempDirectories: Failed to append name '%s'!\n", directory->m_szName );
		return false;
	}

	UTIL_AppendString( suffix, "/", size );
	level++;

	for ( const CASDirectory *child = directory->m_pFirstChild; child; child = child->m_pNextSibling )
	{
		if ( !EnumerateDirectories( operation, level, child, path, suffix + length + 1, size - length - 1 ) )
			return false;
	}

	level--;
	*suffix = 0;
	return true;
}

template <class Operation>
bool EnumerateDirectories( Operation &operation, const CASDirectory *directory )
{
	if ( !directory )
		return false;

	char path[CASDirectory::MAX_NAME_LENGTH];
	unsigned int level = 0;

	path[0] = 0;
	return EnumerateDirectories( operation, level, directory, path, path, sizeof( path ) );
}

#endif
