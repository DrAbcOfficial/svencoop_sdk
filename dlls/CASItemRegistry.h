#ifndef CASITEMREGISTRY_H
#define CASITEMREGISTRY_H

#include "CASServerScriptGlobal.h"
#include "CString.h"

class CASDocumentation;

class CASItemRegistry : public CASServerScriptGlobal<CASItemRegistry>
{
public:
	enum
	{
		MAX_ITEM_NAME_LENGTH = 32,
		MAX_SPRITE_DIR_LENGTH = 128,
		MAX_REGISTERED_ITEMS = 226,
		FIRST_ITEM_ID = 31	// after the built-in weapon ids
	};

	struct ItemData_t
	{
		char	szName[MAX_ITEM_NAME_LENGTH + 1];
		char	szAmmo1[MAX_ITEM_NAME_LENGTH + 1];
		char	szAmmo2[MAX_ITEM_NAME_LENGTH + 1];
		char	szAmmo1Entity[MAX_ITEM_NAME_LENGTH + 1];
		char	szAmmo2Entity[MAX_ITEM_NAME_LENGTH + 1];
		char	szSpriteDir[MAX_SPRITE_DIR_LENGTH + 1];
		int		iId;
	};

	virtual ~CASItemRegistry();
	virtual void Shutdown( void );

	const ItemData_t *GetItemInfoForName( const char *pszName ) const;
	int GetIdForName( const char *pszName ) const;
	int GetIdForName( const CString &name ) const;
	int RegisterItem( const CString &name, const CString &spriteDir, const CString &ammo1, const CString &ammo2,
	                  const CString &ammo1Entity, const CString &ammo2Entity );

private:
	friend class CASBaseScriptGlobal<IASServerScriptGlobal, CASItemRegistry>;

	CASItemRegistry();

	ItemData_t		m_Items[MAX_REGISTERED_ITEMS];
	unsigned int	m_iItemCount;
};

void RegisterSCScriptItemRegistry( CASDocumentation *documentation );
void RegisterSCScriptPlayerItemEntityDependencies( CASDocumentation *documentation );

#endif
