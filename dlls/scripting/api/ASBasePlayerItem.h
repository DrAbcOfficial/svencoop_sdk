#ifndef ASBASEPLAYERITEM_H
#define ASBASEPLAYERITEM_H

#include "scripting/api/ASBaseAnimating.h"
#include "scripting/api/ASEntityHeaderSupport.h"
#include "CString.h"
#include "player/player.h"
#include "weapons/weapons.h"

template <class T>
bool BasePlayerItemEntity_AddToPlayer( CBasePlayer *player, T *entity )
{
	return entity->T::AddToPlayer( player );
}

template <class T>
bool BasePlayerItemEntity_AddDuplicate( CBasePlayerItem *item, T *entity )
{
	return entity->T::AddDuplicate( item );
}

template <class T>
void BasePlayerItemEntity_Materialize( T *entity )
{
	entity->T::Materialize();
}

template <class T>
bool BasePlayerItemEntity_GetItemInfo( ItemInfo &info, T *entity )
{
	return entity->T::GetItemInfo( &info );
}

template <class T>
bool BasePlayerItemEntity_CanDeploy( T *entity )
{
	return entity->T::CanDeploy();
}

template <class T>
bool BasePlayerItemEntity_Deploy( T *entity )
{
	return entity->T::Deploy();
}

template <class T>
bool BasePlayerItemEntity_CanHolster( T *entity )
{
	return entity->T::CanHolster();
}

template <class T>
void BasePlayerItemEntity_Holster( int skipLocal, T *entity )
{
	entity->T::Holster( skipLocal );
}

template <class T>
void BasePlayerItemEntity_UpdateItemInfo( T *entity )
{
	entity->T::UpdateItemInfo();
}

template <class T>
void BasePlayerItemEntity_ItemPreFrame( T *entity )
{
	entity->T::ItemPreFrame();
}

template <class T>
void BasePlayerItemEntity_ItemPostFrame( T *entity )
{
	entity->T::ItemPostFrame();
}

template <class T>
void BasePlayerItemEntity_InactiveItemPreFrame( T *entity )
{
	entity->T::InactiveItemPreFrame();
}

template <class T>
void BasePlayerItemEntity_InactiveItemPostFrame( T *entity )
{
	entity->T::InactiveItemPostFrame();
}

template <class T>
void BasePlayerItemEntity_Kill( T *entity )
{
	entity->T::Kill();
}

template <class T>
void BasePlayerItemEntity_AttachToPlayer( CBasePlayer *player, T *entity )
{
	entity->T::AttachToPlayer( player );
}

template <class T>
int BasePlayerItemEntity_PrimaryAmmoIndex( T *entity )
{
	return entity->T::PrimaryAmmoIndex();
}

template <class T>
int BasePlayerItemEntity_SecondaryAmmoIndex( T *entity )
{
	return entity->T::SecondaryAmmoIndex();
}

template <class T>
bool BasePlayerItemEntity_UpdateClientData( CBasePlayer *player, T *entity )
{
	return entity->T::UpdateClientData( player );
}

template <class T>
float BasePlayerItemEntity_GetRespawnTime( T *entity )
{
	return entity->T::GetRespawnTime();
}

template <class T>
CBasePlayerItem *BasePlayerItemEntity_DropItem( T *entity )
{
	return entity->T::DropItem();
}

template <class T>
bool BasePlayerItemEntity_CanHaveDuplicates( T *entity )
{
	return entity->T::CanHaveDuplicates();
}

template <class T>
void RegisterSCScriptBaseBaseClassPlayerItemEntity( CASDocumentation *documentation, const char *description, const char *name )
{
	RegisterSCScriptBaseBaseClassEntity<T>( documentation, description, name );
	RegisterASScriptEntityHeaderCasts<CBasePlayerItem, T>( documentation, "BasePlayerItem", name );

	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of AddToPlayer", name,
	    "bool AddToPlayer(CBasePlayer@ pPlayer)",
	    asFUNCTION( BasePlayerItemEntity_AddToPlayer<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of AddDuplicate", name,
	    "bool AddDuplicate(CBasePlayerItem@ pItem)",
	    asFUNCTION( BasePlayerItemEntity_AddDuplicate<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of Materialize", name,
	    "void Materialize()",
	    asFUNCTION( BasePlayerItemEntity_Materialize<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of GetItemInfo", name,
	    "bool GetItemInfo(ItemInfo& out info)",
	    asFUNCTION( BasePlayerItemEntity_GetItemInfo<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of CanDeploy", name,
	    "bool CanDeploy()",
	    asFUNCTION( BasePlayerItemEntity_CanDeploy<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of Deploy", name,
	    "bool Deploy()",
	    asFUNCTION( BasePlayerItemEntity_Deploy<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of CanHolster", name,
	    "bool CanHolster()",
	    asFUNCTION( BasePlayerItemEntity_CanHolster<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of Holster", name,
	    "void Holster(int skiplocal = 0)",
	    asFUNCTION( BasePlayerItemEntity_Holster<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of UpdateItemInfo", name,
	    "void UpdateItemInfo()",
	    asFUNCTION( BasePlayerItemEntity_UpdateItemInfo<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of ItemPreFrame", name,
	    "void ItemPreFrame()",
	    asFUNCTION( BasePlayerItemEntity_ItemPreFrame<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of ItemPostFrame", name,
	    "void ItemPostFrame()",
	    asFUNCTION( BasePlayerItemEntity_ItemPostFrame<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of InactiveItemPreFrame", name,
	    "void InactiveItemPreFrame()",
	    asFUNCTION( BasePlayerItemEntity_InactiveItemPreFrame<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of InactiveItemPostFrame", name,
	    "void InactiveItemPostFrame()",
	    asFUNCTION( BasePlayerItemEntity_InactiveItemPostFrame<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of Kill", name,
	    "void Kill()",
	    asFUNCTION( BasePlayerItemEntity_Kill<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of AttachToPlayer", name,
	    "void AttachToPlayer(CBasePlayer@ pPlayer)",
	    asFUNCTION( BasePlayerItemEntity_AttachToPlayer<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of PrimaryAmmoIndex", name,
	    "int PrimaryAmmoIndex()",
	    asFUNCTION( BasePlayerItemEntity_PrimaryAmmoIndex<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of SecondaryAmmoIndex", name,
	    "int SecondaryAmmoIndex()",
	    asFUNCTION( BasePlayerItemEntity_SecondaryAmmoIndex<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of UpdateClientData", name,
	    "bool UpdateClientData(CBasePlayer@ pPlayer)",
	    asFUNCTION( BasePlayerItemEntity_UpdateClientData<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of GetRespawnTime", name,
	    "float GetRespawnTime() const",
	    asFUNCTION( BasePlayerItemEntity_GetRespawnTime<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of DropItem", name,
	    "CBasePlayerItem@ DropItem()",
	    asFUNCTION( BasePlayerItemEntity_DropItem<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of CanHaveDuplicates", name,
	    "bool CanHaveDuplicates()",
	    asFUNCTION( BasePlayerItemEntity_CanHaveDuplicates<T> ), asCALL_CDECL_OBJLAST );
}

template <class T>
void BasePlayerItem_LoadSprites( CBasePlayer *player, const CString &name, T *entity )
{
	entity->LoadSprites( player, name.Data() );
}

template <class T>
CString PlayerItemEntity_pszAmmo1( T *entity )
{
	return CString( entity->pszAmmo1() );
}

template <class T>
CString PlayerItemEntity_pszAmmo2( T *entity )
{
	return CString( entity->pszAmmo2() );
}

template <class T>
CString PlayerItemEntity_pszName( T *entity )
{
	return CString( entity->pszName() );
}

template <class T>
void RegisterSCScriptBasePlayerItem( CASDocumentation *documentation, const char *name )
{
	RegisterASScriptEntityHeaderCasts<CBasePlayerItem, T>( documentation, "CBasePlayerItem", name );
	RegisterSCScriptBaseAnimatingEntity<T>( documentation, name );

	documentation->RegisterObjectMethod(
	    "Returns true if this item wants itself added to the player's inventory.", name,
	    "bool AddToPlayer(CBasePlayer@ pPlayer)",
	    asMETHOD( T, AddToPlayer ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "returns true if this item wants the duplicate removed from the world.", name,
	    "bool AddDuplicate(CBasePlayerItem@ pItem)",
	    asMETHOD( T, AddDuplicate ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Think function. Destroys this item.", name,
	    "void DestroyItem()",
	    asMETHOD( T, DestroyItem ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Attempts to let pOther collect this item.", name,
	    "void Collect(CBaseEntity@ pOther)",
	    asMETHODPR( T, Collect, (CBaseEntity *), void ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Attempts to let pOther collect this item, while specifying the collect type.", name,
	    "void Collect(CBaseEntity@ pOther, CollectTypes collectType)",
	    asMETHODPR( T, Collect, ( CBaseEntity *, COLLECT_TYPE ), void ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Touch function. Default touch.", name,
	    "void DefaultTouch(CBaseEntity@ pOther)",
	    asMETHOD( T, DefaultTouch ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Default weapon use function.", name,
	    "void ItemUse(CBaseEntity@ pActivator, CBaseEntity@ pCaller, USE_TYPE useType, float flValue)",
	    asMETHOD( T, ItemUse ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Think function. When an item is first spawned, this think is run to determine when the object has hit the "
	    "ground.",
	    name,
	    "void FallThink()",
	    asMETHOD( T, FallThink ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Make a weapon visible and tangible.", name,
	    "void Materialize()",
	    asMETHOD( T, Materialize ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Think function. The weapon desires to become visible and tangible, if the game rules allow for it.", name,
	    "void AttemptToMaterialize()",
	    asMETHOD( T, AttemptToMaterialize ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Copy a weapon.", name,
	    "CBasePlayerItem@ RespawnItem()",
	    asMETHOD( T, Respawn ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Fall init.", name,
	    "void FallInit()",
	    asMETHOD( T, FallInit ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Check respawn.", name,
	    "void CheckRespawn()",
	    asMETHOD( T, CheckRespawn ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets item info.", name,
	    "bool GetItemInfo(ItemInfo& out info)",
	    asMETHOD( T, GetItemInfo ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether this item can deploy.", name,
	    "bool CanDeploy()",
	    asMETHOD( T, CanDeploy ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns if deploy was successful.", name,
	    "bool Deploy()",
	    asMETHOD( T, Deploy ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Can this weapon be put away right now?", name,
	    "bool CanHolster()",
	    asMETHOD( T, CanHolster ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Holsters this weapon.", name,
	    "void Holster(int iSkipLocal = 0)",
	    asMETHOD( T, Holster ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Updates item info.", name,
	    "void UpdateItemInfo()",
	    asMETHOD( T, UpdateItemInfo ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Load item sprites.", name,
	    "void LoadSprites(CBasePlayer@ pPlayer, const string& in szName)",
	    asFUNCTION( BasePlayerItem_LoadSprites<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Kills this item.", name,
	    "void Kill()",
	    asMETHOD( T, Kill ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Attach this item to the given player.", name,
	    "void AttachToPlayer(CBasePlayer@ pPlayer)",
	    asMETHOD( T, AttachToPlayer ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the primary ammo index, or -1 if this item doesn't use primary ammo.", name,
	    "int PrimaryAmmoIndex()",
	    asMETHOD( T, PrimaryAmmoIndex ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the secondary ammo index, or -1 if this item doesn't use secondary ammo.", name,
	    "int SecondaryAmmoIndex()",
	    asMETHOD( T, SecondaryAmmoIndex ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Updates client data.", name,
	    "bool UpdateClientData(CBasePlayer@ pPlayer)",
	    asMETHOD( T, UpdateClientData ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets this item as a weapon. Returns null if this is not a weapon.", name,
	    "CBasePlayerWeapon@ GetWeaponPtr()",
	    asMETHOD( T, GetWeaponPtr ), asCALL_THISCALL );
	documentation->RegisterObjectProperty(
	    "The owning player. null if no owner.", name,
	    "EHandle m_hPlayer", asOFFSET( T, m_hPlayer ) );
	documentation->RegisterObjectProperty(
	    "Gets the next item. null if no next item.", name,
	    "EHandle m_hNextItem", asOFFSET( T, m_pNext ) );
	documentation->RegisterObjectProperty(
	    "The item id.", name,
	    "const int m_iId", asOFFSET( T, m_iId ) );
	documentation->RegisterObjectMethod(
	    "Gets the slot this item is in. 0 to MAX_ITEMS_SLOTS.", name,
	    "int iItemSlot()",
	    asMETHOD( T, iItemSlot ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the item position.", name,
	    "int iItemPosition()",
	    asMETHOD( T, iItemPosition ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the name of the primary ammo.", name,
	    "string pszAmmo1()",
	    asFUNCTION( PlayerItemEntity_pszAmmo1<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Gets the primary ammo maximum.", name,
	    "int iMaxAmmo1()",
	    asMETHOD( T, iMaxAmmo1 ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the name of the secondary ammo.", name,
	    "string pszAmmo2()",
	    asFUNCTION( PlayerItemEntity_pszAmmo2<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Gets the secondary ammo maximum.", name,
	    "int iMaxAmmo2()",
	    asMETHOD( T, iMaxAmmo2 ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the name of this item.", name,
	    "string pszName()",
	    asFUNCTION( PlayerItemEntity_pszName<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Gets the max clip amount.", name,
	    "int iMaxClip()",
	    asMETHOD( T, iMaxClip ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the item weight.", name,
	    "int iWeight()",
	    asMETHOD( T, iWeight ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the item flags.", name,
	    "int iFlags()",
	    asMETHOD( T, iFlags ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets this item's respawn time (time from pickup to respawn).", name,
	    "float GetRespawnTime() const",
	    asMETHOD( T, GetRespawnTime ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "CBaseEntity to drop.\nBy default, this entity.", name,
	    "CBasePlayerItem@ DropItem()",
	    asMETHOD( T, DropItem ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Can this weapon have duplicates added?", name,
	    "bool CanHaveDuplicates()",
	    asMETHOD( T, CanHaveDuplicates ), asCALL_THISCALL );
	documentation->RegisterObjectProperty(
	    "The last bounce time.", name,
	    "float m_flLastBounce", asOFFSET( T, m_flLastBounce ) );
	documentation->RegisterObjectProperty(
	    "If set, as long as a player has this item, the player must keep this weapon selected.No more than one item with "
	    "this set can be carried at one time.",
	    name,
	    "bool m_bExclusiveHold", asOFFSET( T, m_bExclusiveHold ) );
	documentation->RegisterObjectProperty(
	    "The current drop type. Check the DROP_TYPE Enum.", name,
	    "int m_dropType", asOFFSET( T, m_iUnknown180 ) );
}

#endif
