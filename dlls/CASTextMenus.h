#ifndef CASTEXTMENUS_H
#define CASTEXTMENUS_H

#include "CASServerScriptGlobal.h"
#include "CASTextMenu.h"
#include <map>

class CASTextMenus : public CASServerScriptGlobal<CASTextMenus>
{
public:
	CASTextMenus();
	virtual ~CASTextMenus();

	bool RegisterMenu( CASTextMenu *menu );
	void UnregisterMenu( CASTextMenu *menu );
	CASTextMenu *GetMenuById( TextMenuId_t id );
	void RemoveAllMenus();
	void HandleMenuSelect( CBasePlayer *player, int slot );

	const TextMenuId_t INVALID_TEXT_MENU_ID;

private:
	typedef std::map<TextMenuId_t, CASTextMenu *> MenuMap_t;

	MenuMap_t    m_Menus;
	TextMenuId_t m_iNextId;
};

#endif
