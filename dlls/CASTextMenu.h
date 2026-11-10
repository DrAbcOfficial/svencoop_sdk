#ifndef CASTEXTMENU_H
#define CASTEXTMENU_H

#include "CASGCBaseClass.h"
#include "CString.h"
#include <stddef.h>
#include <string>
#include <vector>

class asIScriptEngine;
class asIScriptFunction;
class CBasePlayer;
class CASFunction;
class CScriptAny;
class CScriptArray;

typedef int TextMenuId_t;

class CASTextMenu : public CASGCBaseClass<CASRefCountedBaseClass>
{
public:
	struct Item
	{
		Item( const CString &name, CScriptAny *userData );
		Item( Item &&other );
		~Item();

		CString     m_szName;
		CScriptAny *m_pUserData;

	private:
		Item( const Item &other );
		Item &operator=( const Item &other );
	};

	CASTextMenu( asIScriptFunction *inputCallback );
	~CASTextMenu();

	void Release() const;
	void EnumReferences( asIScriptEngine *engine );
	void ReleaseReferences( asIScriptEngine *engine );
	void ClearCallbacks();

	TextMenuId_t GetId() const;
	bool IsRegistered() const;
	bool Register();
	void Unregister();
	void Open( int displayTime, unsigned int page, CBasePlayer *player );
	void Open( int displayTime, unsigned int page, CScriptArray *players );

	size_t GetItemCount() const;
	unsigned int GetPageCount() const;
	const Item *GetItem( size_t index ) const;
	const CString &GetTitle() const;
	void SetTitle( const CString &title );
	void AddItem( const CString &name, CScriptAny *userData );

	void InputReceived( CBasePlayer *player, int slot );

private:
	friend class CASTextMenus;

	unsigned int GetFirstItemForPage( unsigned int page ) const;
	unsigned int GetItemCountForPage( unsigned int page ) const;
	std::string GetPageText( CBasePlayer *player, unsigned int page, int &validSlots ) const;
	void ShowMenu( CBasePlayer *player, int validSlots, int displayTime, char *text, unsigned int page );

	TextMenuId_t      m_iId;
	CASFunction      *m_pInputCallback;
	CString           m_szTitle;
	std::vector<Item> m_Items;
	unsigned int      m_uiItemsPerPage;
};

#endif
