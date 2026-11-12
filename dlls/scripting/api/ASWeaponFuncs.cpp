#include "scripting/api/ASWeaponFuncs.h"
#include "scripting/core/CASWeaponFuncs.h"
#include "scripting/core/CASDocumentation.h"

void RegisterSCScriptWeaponFuncs( CASDocumentation *documentation )
{
	documentation->RegisterObjectType( "Weapon functions class.", "CWeaponFuncs", 0, asOBJ_REF | asOBJ_NOCOUNT );
	documentation->RegisterObjectMethod(
	    "Clears the multi damage data.", "CWeaponFuncs", "void ClearMultiDamage()",
	    asMETHOD( CASWeaponFuncs, ClearMultiDamage ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Applies multi damage.", "CWeaponFuncs",
	    "void ApplyMultiDamage(entvars_t@ pevInflictor, entvars_t@ pevAttacker)",
	    asMETHOD( CASWeaponFuncs, ApplyMultiDamage ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Add multi damage.", "CWeaponFuncs",
	    "void AddMultiDamage(entvars_t@ pevInflictor, CBaseEntity@ pEntity, float flDamage, int bitsDamageType)",
	    asMETHOD( CASWeaponFuncs, AddMultiDamage ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the weapon time base.", "CWeaponFuncs", "float WeaponTimeBase()",
	    asMETHOD( CASWeaponFuncs, WeaponTimeBase ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Creates a gunshot decal at the trace hit point.", "CWeaponFuncs",
	    "void DecalGunshot(TraceResult& in trace, int iBulletType)",
	    asMETHOD( CASWeaponFuncs, DecalGunshot ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Spawns blood at the given location.", "CWeaponFuncs",
	    "void SpawnBlood(const Vector& in vecSpot, int bloodColor, float flDamage)",
	    asMETHOD( CASWeaponFuncs, SpawnBlood ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Creates a damage decal on the given entity.", "CWeaponFuncs",
	    "int DamageDecal(CBaseEntity@ pEntity, int bitsDamageType)",
	    asMETHOD( CASWeaponFuncs, DamageDecal ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Deals radius damage.", "CWeaponFuncs",
	    "void RadiusDamage(const Vector& in vecSrc, entvars_t@ pevInflictor, entvars_t@ pevAttacker, "
	    "float flDamage, float flRadius, int iClassIgnore, int bitsDamageType)",
	    asMETHOD( CASWeaponFuncs, RadiusDamage ), asCALL_THISCALL );
	documentation->RegisterGlobalProperty(
	    "Weapon functions global instance.", "CWeaponFuncs g_WeaponFuncs", CASWeaponFuncs::GetInstance() );
}
