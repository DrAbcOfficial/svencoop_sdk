#ifndef CINVENTORYMISC_H
#define CINVENTORYMISC_H

#include "CASServerScriptGlobal.h"
#include "CString.h"

class CBaseEntity;
class CBaseMonster;

class CInventoryMisc : public CASServerScriptGlobal<CInventoryMisc>
{
public:
	virtual ~CInventoryMisc();

	bool IsValidInventoryHolder( CBaseEntity *entity );
	bool CheckTokenStringRule( const CString &rule, const CString &value );
	int Count( CBaseMonster *holder );
	float GetWeight( CBaseMonster *holder );
	int ExistsInGroup( const CString &group );
	int HaveNumFromGroup( CBaseMonster *holder, const CString &group );
	bool HasAllInGroup( CBaseMonster *holder, const CString &group );
	void RemoveAllFromHolder( CBaseMonster *holder, bool drop, bool blockingWeaponsOnly, bool keepOnRespawn );
};

#endif
