#include "extdll.h"
#include "util.h"
#include "CASMapCycle.h"
#include "MapCycle.h"
#include "CASLog.h"
#include "../external/angelscript/sdk/add_on/scriptarray/scriptarray.h"

CASMapCycle::CASMapCycle()
{
}

CASMapCycle::~CASMapCycle()
{
}

size_t CASMapCycle::Count( void ) const
{
	size_t count = 0;

	for ( mapcycle_item_t *item = g_mapcycle.items;
	      item && ( !count || item != g_mapcycle.items ); item = item->next )
		count++;

	return count;
}

CString CASMapCycle::GetNextMap( void ) const
{
	return CString( CVAR_GET_STRING( "mp_nextmap_cycle" ) );
}

CString CASMapCycle::GetNextMap( const CString &mapName ) const
{
	if ( !mapName.Length() )
		return CString( "" );

	if ( mapName == STRING( gpGlobals->mapname ) )
		return GetNextMap();

	mapcycle_item_t *item = g_mapcycle.items;

	if ( !item )
		return CString( "" );

	do
	{
		bool found = mapName == item->mapname;

		item = item->next;

		if ( found )
		{
			if ( !item )
				item = g_mapcycle.items;

			return CString( item->mapname );
		}
	} while ( item && item != g_mapcycle.items );

	return CString( g_mapcycle.items ? g_mapcycle.items->mapname : "" );
}

CScriptArray *CASMapCycle::GetMapCycle( void ) const
{
	size_t count = Count();
	asIScriptEngine *engine = CASServerManager::GetInstance()->m_pEngine;
	asITypeInfo *typeInfo = engine->GetTypeInfoById( engine->GetTypeIdByDecl( "array<string>" ) );

	if ( !typeInfo )
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT,
		                              "CASMapCycle::GetMapCycle: Failed to get array<string> object type. Tell a programmer\n" );
		return NULL;
	}

	CScriptArray *array = CScriptArray::Create( typeInfo, count );
	mapcycle_item_t *item = g_mapcycle.items;

	for ( size_t index = 0; item && ( !index || item != g_mapcycle.items ); index++ )
	{
		*(CString *) array->At( index ) = item->mapname;
		item = item->next;
	}

	return array;
}

mapcycle_item_s *CASMapCycle::GetMapInfo( const CString &mapName ) const
{
	if ( !mapName.Length() )
		return NULL;

	mapcycle_item_t *item = g_mapcycle.items;

	if ( !item )
		return NULL;

	do
	{
		if ( mapName == item->mapname )
			return item;

		item = item->next;
	} while ( item && item != g_mapcycle.items );

	return NULL;
}
