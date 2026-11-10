#ifndef CASWEAPONFUNCS_H
#define CASWEAPONFUNCS_H

#include "extdll.h"
#include "CASServerScriptGlobal.h"

class CBaseEntity;

class CASWeaponFuncs : public CASServerScriptGlobal<CASWeaponFuncs>
{
public:
	virtual ~CASWeaponFuncs();

	void ClearMultiDamage();
	void ApplyMultiDamage( entvars_t *pevInflictor, entvars_t *pevAttacker );
	void AddMultiDamage( entvars_t *pevInflictor, CBaseEntity *pEntity, float flDamage, int bitsDamageType );
	float WeaponTimeBase();
	void DecalGunshot( TraceResult &trace, int iBulletType );
	void SpawnBlood( const Vector &vecSpot, int bloodColor, float flDamage );
	int DamageDecal( CBaseEntity *pEntity, int bitsDamageType );
	void RadiusDamage( const Vector &vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker,
	                   float flDamage, float flRadius, int iClassIgnore, int bitsDamageType );
};

#endif
