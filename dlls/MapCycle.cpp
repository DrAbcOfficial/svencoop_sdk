#include "extdll.h"
#include "util.h"
#include "MapCycle.h"
#include "server_support.h"
#include "server_parse.h"
#include <sys/stat.h>

int COM_TokenWaiting( char *buffer );

mapcycle_t g_mapcycle;

void DestroyMapCycle( mapcycle_t *cycle )
{
	mapcycle_item_t *start = cycle->items;
	if ( start )
	{
		mapcycle_item_t *p = start->next;
		while ( p != start )
		{
			mapcycle_item_t *next = p->next;
			delete p;
			p = next;
		}

		delete cycle->items;
	}

	memset( cycle->filename, 0, sizeof( cycle->filename ) );
	cycle->modified = 0;
	cycle->items = NULL;
	cycle->next_item = NULL;
	cycle->count = 0;
	cycle->position = 0;
	cycle->shuffled = false;
	cycle->parted = 0;
}

bool ReloadMapCycleFile( char *filename, mapcycle_t *cycle, bool shuffle )
{
	int length;
	char *aFileList = (char*)LOAD_FILE_FOR_ME( filename, &length );
	char fullPath[MAX_PATH + 1];

	memset( fullPath, 0, sizeof( fullPath ) );
	GET_GAME_DIR( fullPath );
	UTIL_AppendString( fullPath, "/", sizeof( fullPath ) );
	UTIL_AppendString( fullPath, filename, sizeof( fullPath ) );

	if ( aFileList )
	{
		if ( length )
	{
			char *pFileList = aFileList;

			for ( ;; )
			{
				char szBuffer[1024];
			memset( szBuffer, 0, sizeof( szBuffer ) );

			pFileList = COM_Parse( pFileList );
			if ( !strlen( com_token ) )
				break;

				if ( strlen( com_token ) > MAX_MAPCYCLE_NAME )
					ALERT( at_warning, "Map cycle reader: Map \"%s\" too long, max is %u characters. Name will be cut off!\n", com_token, MAX_MAPCYCLE_NAME );

				char szMap[MAX_MAPCYCLE_NAME + 1];

				memset( szMap, 0, sizeof( szMap ) );
				UTIL_CopyString( szMap, com_token, sizeof( szMap ) );

				bool hasbuffer = false;

			// Any more tokens on this line?
			if ( COM_TokenWaiting( pFileList ) )
			{
				pFileList = COM_Parse( pFileList );
				if ( strlen( com_token ) )
				{
					hasbuffer = true;
						memset( szBuffer, 0, sizeof( szBuffer ) );
						UTIL_CopyString( szBuffer, com_token, sizeof( szBuffer ) );
				}
			}

			// Check map
			if ( IS_MAP_VALID( szMap ) )
			{
					mapcycle_item_t *item = new mapcycle_item_t;

					UTIL_CopyString( item->mapname, szMap, sizeof( item->mapname ) );
					item->mapname[32] = 0;

				item->minplayers = 0;
				item->maxplayers = 0;

				memset( item->rulebuffer, 0, sizeof( item->rulebuffer ) );

				if ( hasbuffer )
				{
						char *s = g_engfuncs.pfnInfoKeyValue( szBuffer, "minplayers" );
					if ( s && *s )
					{
						item->minplayers = atoi( s );
						item->minplayers = V_max( item->minplayers, 0 );
						item->minplayers = V_min( item->minplayers, gpGlobals->maxClients );
					}
					s = g_engfuncs.pfnInfoKeyValue( szBuffer, "maxplayers" );
					if ( s && *s )
					{
						item->maxplayers = atoi( s );
						item->maxplayers = V_max( item->maxplayers, 0 );
						item->maxplayers = V_min( item->maxplayers, gpGlobals->maxClients );
					}

					// Remove keys
					//
					g_engfuncs.pfnInfo_RemoveKey( szBuffer, "minplayers" );
					g_engfuncs.pfnInfo_RemoveKey( szBuffer, "maxplayers" );
						UTIL_CopyString( item->rulebuffer, szBuffer, sizeof( item->rulebuffer ) );
				}

				item->next = cycle->items;
				cycle->items = item;
			}
			else
			{
				ALERT( at_console, "Map cycle reader: Missing/invalid map \"%s\", ignoring.\n", szMap );
				}
			}

		}

		FREE_FILE( aFileList );
	}

	cycle->count = 0;
	cycle->position = 0;
	cycle->shuffled = false;
	cycle->parted = 0;

	mapcycle_item_t *item = cycle->items;
	mapcycle_item_t *list = NULL;

	while ( item )
	{
		mapcycle_item_t *next = item->next;
		item->next = list;
		list = item;
		cycle->count++;
		item = next;
	}
	cycle->items = list;

	// Didn't parse anything
	if ( !list )
		return false;

	item = list;

	while ( item->next )
		item = item->next;

	item->next = list;

	cycle->next_item = list;

	if ( shuffle )
		cycle->shuffled = true;

	UTIL_CopyString( cycle->filename, filename, sizeof( cycle->filename ) );

	struct stat fileInfo;

	if ( stat( fullPath, &fileInfo ) == 0 && fileInfo.st_mtime )
		g_mapcycle.modified = fileInfo.st_mtime;

	return true;
}
