#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player/player.h"
#include "enginecallback.h"
#include "scripting/api/ASDiagnostics.h"
#include "scripting/api/ASFileSystem.h"
#include "scripting/core/CASServerManager.h"
#include "scripting/core/CASVirtualFileSystem.h"
#include "server/CCommand.h"
#include "server/CConCommand.h"
#include "server/CConCommandSystem.h"
#include "utils/CKeyvaluesWriter.h"
#include "server/server_support.h"
#include <stdlib.h>

void as_globals_Callback( const CCommand &command )
{
	CBasePlayer *player = CConCommandSystem::GetInstance()->GetCurrentPlayer();

	if ( !player )
		return;

	char buffer[256];

	if ( command.ArgC() == 1 )
	{
		UTIL_FormatString( buffer, sizeof( buffer ),
		                   "as_globals: Outputting global variables to default file \"%s\"\n", "asglobals" );
		CLIENT_PRINTF( player->edict(), print_console, buffer );
		ASPrintGlobals( "asglobals" );
	}
	else if ( command.ArgC() == 2 )
	{
		const char *fileName = command.Arg( 1 );

		if ( strstr( fileName, ".." ) || strstr( fileName, "/" ) || strstr( fileName, "\\" ) )
		{
			CLIENT_PRINTF( player->edict(), print_console,
			               "as_globals: Cannot output global variables; file name contains '..', '/' or '\\', which is not allowed\n" );
			return;
		}

		UTIL_FormatString( buffer, sizeof( buffer ), "as_globals: Outputting global variables to file \"%s\"\n", fileName );
		CLIENT_PRINTF( player->edict(), print_console, buffer );
		ASPrintGlobals( fileName );
	}
	else
	{
		UTIL_FormatString( buffer, sizeof( buffer ),
		                   "as_globals usage:\n"
		                   "\"as_globals\" to output global variables to default file \"%s\"\n"
		                   "\"as_globals <filename> to output to file of given name\n",
		                   "asglobals" );
		CLIENT_PRINTF( player->edict(), print_console, buffer );
	}
}

void as_generate_config_Callback( const CCommand &command )
{
	CBasePlayer *player = CConCommandSystem::GetInstance()->GetCurrentPlayer();

	if ( !player )
		return;

	char buffer[256];

	if ( command.ArgC() == 1 )
	{
		UTIL_FormatString( buffer, sizeof( buffer ),
		                   "as_generate_config: Outputting config to default file \"%s\"\n", "asconfig" );
		CLIENT_PRINTF( player->edict(), print_console, buffer );
		ASOutputConfig( "asconfig" );
	}
	else if ( command.ArgC() == 2 )
	{
		const char *fileName = command.Arg( 1 );

		if ( strstr( fileName, ".." ) || strstr( fileName, "/" ) || strstr( fileName, "\\" ) )
		{
			CLIENT_PRINTF( player->edict(), print_console,
			               "as_generate_config: Cannot output config; file name contains '..', '/' or '\\', which is not allowed\n" );
			return;
		}

		UTIL_FormatString( buffer, sizeof( buffer ), "as_generate_config: Outputting config to file \"%s\"\n", fileName );
		CLIENT_PRINTF( player->edict(), print_console, buffer );
		ASOutputConfig( fileName );
	}
	else
	{
		UTIL_FormatString( buffer, sizeof( buffer ),
		                   "as_generate_config usage:\n"
		                   "\"as_generate_config\" to output config to default file \"%s\"\n"
		                   "\"as_generate_config <filename>\" to output to file of given name\n",
		                   "asconfig" );
		CLIENT_PRINTF( player->edict(), print_console, buffer );
	}
}

void as_fs_dumpfilesystem_Callback( const CCommand &command )
{
	bool writeDetails = true;

	if ( command.ArgC() == 3 )
		writeDetails = atoi( command.Arg( 2 ) ) != 0;
	else if ( command.ArgC() != 2 )
	{
		ALERT( at_console, "as_fs_dumpfilesystem usage: as_fs_dumpfilesystem <filename>\n" );
		return;
	}

	char fileName[MAX_PATH + 1];
	int length = UTIL_FormatString( fileName, sizeof( fileName ), "%s.txt", command.Arg( 1 ) );

	if ( (unsigned int) length >= sizeof( fileName ) )
	{
		ALERT( at_console, "Filename too long!\n" );
		return;
	}

	CKeyvaluesWriter writer( fileName );

	if ( writer.m_hFile )
	{
		CWriteFileSystemToFile operation( writer, writeDetails );

		operation.Write( *CASServerManager::GetInstance()->m_pFileSystem );
	}

	if ( writer.m_hFile && !writer.m_bError )
		ALERT( at_console, "Written filesystem to file '%s'\n", writer.m_szFileName );
	else
		ALERT( at_error, "An error occurred while writing filesystem to file '%s'\n", fileName );
}

CConCommand as_fs_dumpfilesystem( "as_fs_dumpfilesystem", as_fs_dumpfilesystem_Callback, ConCommandFlag::AdminOnly,
                                  "Outputs the Angelscript file system and blacklist to a file" );
CConCommand as_globals( "as_globals", as_globals_Callback, ConCommandFlag::AdminOnly,
                        "Outputs a list of global variables that exist in the API to a file. Defaults to \"asglobals\"" );
CConCommand as_generate_config( "as_generate_config", as_generate_config_Callback, ConCommandFlag::AdminOnly,
                                "Outputs a file that contains the entire API as a config. Defaults to \"asconfig\"" );
