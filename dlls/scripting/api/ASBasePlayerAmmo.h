#ifndef ASBASEPLAYERAMMO_H
#define ASBASEPLAYERAMMO_H

#include "scripting/api/ASBaseEntity.h"
#include "scripting/api/ASEntityHeaderSupport.h"
#include "weapons/weapons.h"

template <class T>
void RegisterSCScriptBasePlayerAmmoEntity( CASDocumentation *documentation, const char *name )
{
	RegisterASScriptEntityHeaderCasts<CBasePlayerAmmo, T>( documentation, "CBasePlayerAmmo", name );
	RegisterSCScriptBaseEntity<T>( documentation, name, true );

	documentation->RegisterObjectMethod(
	    "Default touch", name, "void DefaultTouch(CBaseEntity@ pOther)",
	    asMETHODPR( T, DefaultTouch, (CBaseEntity *), void ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Adds ammo to the given entity.", name, "bool AddAmmo(CBaseEntity@ pOther)",
	    asMETHODPR( T, AddAmmo, (CBaseEntity *), BOOL ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Makes this entity visible and tangible.", name, "void Materialize()",
	    asMETHODPR( T, Materialize, (), void ), asCALL_THISCALL );
}

#endif
