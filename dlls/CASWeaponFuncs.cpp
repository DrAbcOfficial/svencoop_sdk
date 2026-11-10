#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "CASWeaponFuncs.h"

CASWeaponFuncs::~CASWeaponFuncs()
{
}

void CASWeaponFuncs::ClearMultiDamage()
{
	::ClearMultiDamage();
}

void CASWeaponFuncs::ApplyMultiDamage( entvars_t *pevInflictor, entvars_t *pevAttacker )
{
	::ApplyMultiDamage( pevInflictor, pevAttacker );
}

void CASWeaponFuncs::AddMultiDamage( entvars_t *pevInflictor, CBaseEntity *pEntity, float flDamage, int bitsDamageType )
{
	::AddMultiDamage( pevInflictor, pEntity, flDamage, bitsDamageType );
}

float CASWeaponFuncs::WeaponTimeBase()
{
	return UTIL_WeaponTimeBase();
}

void CASWeaponFuncs::DecalGunshot( TraceResult &trace, int iBulletType )
{
	::DecalGunshot( &trace, iBulletType );
}

void CASWeaponFuncs::SpawnBlood( const Vector &vecSpot, int bloodColor, float flDamage )
{
	::SpawnBlood( vecSpot, bloodColor, flDamage );
}

int CASWeaponFuncs::DamageDecal( CBaseEntity *pEntity, int bitsDamageType )
{
	return ::DamageDecal( pEntity, bitsDamageType );
}

void CASWeaponFuncs::RadiusDamage( const Vector &vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker,
                                   float flDamage, float flRadius, int iClassIgnore, int bitsDamageType )
{
	::RadiusDamage( vecSrc, pevInflictor, pevAttacker, flDamage, flRadius, iClassIgnore, bitsDamageType );
}
