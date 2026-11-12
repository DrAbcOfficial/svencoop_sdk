#ifndef INVENTORY_RULES_H
#define INVENTORY_RULES_H

class CASDocumentation;

class CInventoryRules
{
	friend void RegisterSCScriptInventoryRules( CASDocumentation *documentation );

public:
	CInventoryRules( CBaseEntity *owner );

	bool KeyValue( KeyValueData *data );
	bool CanUseTrigger( CBaseEntity *pActivator, bool fContinueByDefault = false );

private:
	EHANDLE m_hEntity;

	string_t	m_szRequiresItemName;
	string_t	m_szRequiresItemGroup;
	int			m_iRequiresItemGroupNum;

	string_t	m_szCantHaveItemName;
	string_t	m_szCantHaveItemGroup;
	int			m_iCantHaveItemGroupNum;

	bool		m_fIgnoreUseTriggers;
	string_t	m_szDropItemNameOnUse;
	string_t	m_szDropItemGroupOnUse;
	bool		m_fIgnoreDropTriggers;

	string_t	m_szReturnItemNameOnUse;
	string_t	m_szReturnItemGroupOnUse;
	bool		m_fIgnoreReturnTriggers;

	string_t	m_szDestroyItemNameOnUse;
	string_t	m_szDestroyItemGroupOnUse;
	bool		m_fIgnoreDestroyTriggers;

	string_t m_szTargetOnFail;
};

bool Inventory_CheckTokenStringRule( const char *rule, const char *value );
bool IsValidInventoryHolder( CBaseEntity *entity );
int Inventory_ExistsInGroup( const char *group );
int Inventory_HaveNumFromGroup( CBaseMonster *holder, const char *group );
bool Inventory_HasAllInGroup( CBaseMonster *holder, const char *group );

#endif // INVENTORY_RULES_H
