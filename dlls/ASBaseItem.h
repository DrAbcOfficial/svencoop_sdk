#ifndef ASBASEITEM_H
#define ASBASEITEM_H

#include "ASBaseEntity.h"
#include "ASEntityHeaderSupport.h"
#include "items.h"

template <class T>
void RegisterSCScriptBaseItemEntity( CASDocumentation *documentation, const char *name )
{
	RegisterASScriptEntityHeaderCasts<CItem, T>( documentation, "CItem", name );
	RegisterSCScriptBaseEntity<T>( documentation, name, true );

	documentation->RegisterObjectMethod(
	    "Item touch function", name, "void ItemTouch(CBaseEntity@ pOther)",
	    asMETHODPR( T, ItemTouch, (CBaseEntity *), void ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Makes this entity visible and tangible.", name, "void Materialize()",
	    asMETHODPR( T, Materialize, (), void ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "The item's touch function.", name, "bool MyTouch(CBasePlayer@ pPlayer)",
	    asMETHODPR( T, MyTouch, (CBasePlayer *), BOOL ), asCALL_THISCALL );
}

#endif
