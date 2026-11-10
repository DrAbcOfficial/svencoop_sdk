#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "CASTextMenus.h"
#include "CASLog.h"
#include <limits.h>

CASTextMenus::CASTextMenus() : INVALID_TEXT_MENU_ID( -1 ), m_iNextId( 0 )
{
}

CASTextMenus::~CASTextMenus()
{
}

bool CASTextMenus::RegisterMenu( CASTextMenu *menu )
{
	if ( !menu )
		return false;

	if ( menu->m_iId != INVALID_TEXT_MENU_ID )
	{
		if ( GetMenuById( menu->m_iId ) )
		{
			CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "CTextMenus::RegisterMenu: Menu registered twice!\n" );
			return false;
		}

		menu->m_iId = INVALID_TEXT_MENU_ID;
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "CTextMenus::RegisterMenu: Menu with free id attempted to register!\n" );
	}

	TextMenuId_t id = m_iNextId;
	bool reuseId = id == INT_MAX;

	if ( reuseId )
	{
		for ( id = 0; id < INT_MAX; id++ )
		{
			if ( m_Menus.find( id ) == m_Menus.end() )
				break;
		}

		if ( id == INT_MAX )
			return false;
	}

	if ( id == INVALID_TEXT_MENU_ID )
		return false;

	if ( !m_Menus.insert( std::make_pair( id, menu ) ).second )
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "CTextMenus::RegisterMenu: Unable to add text menu to menu map!\n" );
		return false;
	}

	menu->m_iId = id;

	if ( !reuseId )
		m_iNextId++;

	return true;
}

void CASTextMenus::UnregisterMenu( CASTextMenu *menu )
{
	if ( !menu || menu->m_iId == INVALID_TEXT_MENU_ID || menu->m_iId >= m_iNextId )
		return;

	MenuMap_t::iterator it = m_Menus.find( menu->m_iId );

	if ( it == m_Menus.end() )
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "CTextMenus::UnregisterMenu: Menu with free id attempted to unregister!\n" );
	else if ( it->second != menu )
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "CTextMenus::UnregisterMenu: Menu with duplicate id!\n" );
	else
		m_Menus.erase( it );

	menu->m_iId = INVALID_TEXT_MENU_ID;
}

CASTextMenu *CASTextMenus::GetMenuById( TextMenuId_t id )
{
	if ( id == INVALID_TEXT_MENU_ID || id >= m_iNextId )
		return NULL;

	MenuMap_t::iterator it = m_Menus.find( id );

	return it == m_Menus.end() ? NULL : it->second;
}

void CASTextMenus::RemoveAllMenus()
{
	for ( MenuMap_t::iterator it = m_Menus.begin(); it != m_Menus.end(); it++ )
	{
		it->second->m_iId = INVALID_TEXT_MENU_ID;
		it->second->Release();
	}

	m_Menus.clear();
	m_iNextId = 0;
}

void CASTextMenus::HandleMenuSelect( CBasePlayer *player, int slot )
{
	if ( !player || player->m_bTextMenuInputReceived )
		return;

	TextMenuId_t menuId = player->m_iTextMenuId;

	if ( menuId == INVALID_TEXT_MENU_ID )
		return;

	float expireTime = player->m_flTextMenuExpireTime;

	if ( expireTime != -1.0f && !( gpGlobals->time <= expireTime ) )
		return;

	unsigned int slotMask = 1U << ( ( (unsigned int) slot - 1 ) & 31 );

	if ( !( slotMask & player->m_iTextMenuValidSlots ) )
		return;

	CASTextMenu *menu = GetMenuById( menuId );

	if ( !menu )
		return;

	player->m_bTextMenuInputReceived = true;
	player->m_iTextMenuId = -1;
	menu->InputReceived( player, slot );
}
