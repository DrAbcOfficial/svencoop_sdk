#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "basemonster.h"
#include "InventoryRules.h"
#include "item_inventory.h"
#include "CInventoryMisc.h"

CInventoryMisc::~CInventoryMisc()
{
}

bool CInventoryMisc::IsValidInventoryHolder( CBaseEntity *entity )
{
	return ::IsValidInventoryHolder( entity );
}

bool CInventoryMisc::CheckTokenStringRule( const CString &rule, const CString &value )
{
	return Inventory_CheckTokenStringRule( rule.Data(), value.Data() );
}

int CInventoryMisc::Count( CBaseMonster *holder )
{
	return Inventory_Count( holder );
}

float CInventoryMisc::GetWeight( CBaseMonster *holder )
{
	return Inventory_GetWeight( holder );
}

int CInventoryMisc::ExistsInGroup( const CString &group )
{
	return Inventory_ExistsInGroup( group.Data() );
}

int CInventoryMisc::HaveNumFromGroup( CBaseMonster *holder, const CString &group )
{
	return Inventory_HaveNumFromGroup( holder, group.Data() );
}

bool CInventoryMisc::HasAllInGroup( CBaseMonster *holder, const CString &group )
{
	return Inventory_HasAllInGroup( holder, group.Data() );
}

void CInventoryMisc::RemoveAllFromHolder( CBaseMonster *holder, bool drop, bool blockingWeaponsOnly, bool keepOnRespawn )
{
	Inventory_RemoveAllFromHolder( holder, drop, blockingWeaponsOnly, keepOnRespawn );
}
