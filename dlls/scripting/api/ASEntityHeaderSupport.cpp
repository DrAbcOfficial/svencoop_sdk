#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "ai/basemonster.h"
#include "weapons/weapons.h"
#include "player/player.h"
#include "items/items.h"
#include "monsters/monsters.h"
#include "items/item_inventory.h"
#include "scripting/api/ASEntityHeaderSupport.h"
#include "ai/PathCondition.h"
#include "scripting/api/ASBaseAnimating.h"
#include "scripting/api/ASBaseToggle.h"
#include "scripting/api/ASBaseEffects.h"
#include "scripting/api/ASBasePathTrack.h"
#include "scripting/api/ASBaseButton.h"
#include "scripting/api/ASBaseDoor.h"
#include "scripting/api/ASBaseGib.h"
#include "scripting/api/ASBaseCineMonster.h"
#include "scripting/api/ASBaseMonster.h"
#include "scripting/api/ASBasePlayerItem.h"
#include "scripting/api/ASBasePlayerWeapon.h"
#include "scripting/api/ASBasePlayerAmmo.h"
#include "scripting/api/ASBaseItem.h"
#include "scripting/api/ASBaseTank.h"
#include "scripting/api/ASInventory.h"
#include "scripting/api/ASPlayerEntity.h"
#include "scripting/api/ASSoundEnt.h"
#include "scripting/api/ASWeaponDependencies.h"
#include "scripting/core/CASCustomEntityFuncs.h"
#include "scripting/core/CASItemRegistry.h"
#include "scripting/core/CASEntityLoader.h"

void RegisterSCScriptPathTrackEntityDependencies( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "path_track spawnflags", "PathTrackSpawnFlags", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "", "PathTrackSpawnFlags", "SF_PATH_DISABLED", SF_PATH_DISABLED );
	documentation->RegisterEnumValue( "", "PathTrackSpawnFlags", "SF_PATH_FIREONCE", SF_PATH_FIREONCE );
	documentation->RegisterEnumValue( "", "PathTrackSpawnFlags", "SF_PATH_ALTREVERSE", SF_PATH_ALTREVERSE );
	documentation->RegisterEnumValue( "", "PathTrackSpawnFlags", "SF_PATH_DISABLE_TRAIN", SF_PATH_DISABLE_TRAIN );
	documentation->RegisterEnumValue( "", "PathTrackSpawnFlags", "SF_PATH_ALTERNATE", SF_PATH_ALTERNATE );
}

void RegisterSCScriptGrenadeEntityDependencies( CASDocumentation *documentation )
{
	documentation->RegisterObjectMethod( "Makes this grenade explode", "CGrenade",
	                                     "void Explode(Vector vecSrc, Vector vecAim)",
	                                     asMETHODPR( CGrenade, Explode, ( Vector, Vector ), void ), asCALL_THISCALL );
}

void RegisterSCScriptEntities( CASDocumentation *documentation )
{
	RegisterSCScriptSoundEnt( documentation );
	RegisterSCScriptInventory( documentation );
	RegisterSCScriptInventoryRules( documentation );

	documentation->RegisterObjectType( "Structure for a list of item_inventory's stored within CBaseMonster",
	                                   "InventoryList", 0, asOBJ_REF | asOBJ_NOCOUNT );
	documentation->RegisterObjectProperty( "Item", "InventoryList", "EHandle hItem",
	                                       asOFFSET( CInventoryItemNode, m_hItem ) );
	documentation->RegisterObjectProperty( "Next item in the list", "InventoryList", "InventoryList@ pNext",
	                                       asOFFSET( CInventoryItemNode, m_pNext ) );

	RegisterSCScriptInventoryMisc( documentation );
	RegisterSCScriptBaseEntity<CBaseEntity>( documentation, "CBaseEntity", true );
	RegisterSCScriptBaseDelayEntity<CBaseDelay>( documentation, "CBaseDelay" );
	RegisterSCScriptBaseAnimatingEntity<CBaseAnimating>( documentation, "CBaseAnimating" );
	RegisterSCScriptBaseToggleEntity<CBaseToggle>( documentation, "CBaseToggle" );
	RegisterSCScriptPlayerItemEntityDependencies( documentation );
	RegisterSCScriptBasePlayerItem<CBasePlayerItem>( documentation, "CBasePlayerItem" );
	RegisterSCScriptBaseMonsterEntity<CBaseMonster>( documentation, "CBaseMonster" );
	RegisterSCScriptBaseCineMonsterEntity<CCineMonster>( documentation, "CCineMonster" );
	RegisterSCScriptBasePlayerEntity<CBasePlayer>( documentation, "CBasePlayer" );
	RegisterSCScriptBaseMonsterEntity<CGrenade>( documentation, "CGrenade" );

	RegisterSCScriptGrenadeEntityDependencies( documentation );

	RegisterSCScriptBaseSpriteEntity<CSprite>( documentation, "CSprite" );
	RegisterSCScriptWeaponDependencies( documentation );
	RegisterSCScriptBasePlayerWeapon<CBasePlayerWeapon>( documentation, "CBasePlayerWeapon" );
	RegisterSCScriptPathTrackEntityDependencies( documentation );
	RegisterSCScriptBasePathTrackEntity<CPathTrack>( documentation, "CPathTrack" );

	documentation->RegisterGlobalFunction( "", "CPathTrack@ PathTrackEntity_Instance(edict_t@ pEdict)",
	                                       asFUNCTION( CPathTrack::Instance ), asCALL_CDECL, NULL );

	RegisterSCScriptBeamEntityDependencies( documentation );
	RegisterSCScriptBaseBeamEntity<CBeam>( documentation, "CBeam" );
	RegisterSCScriptPathConditionEntity( documentation );
	RegisterSCScriptBaseLaserEntity<CLaser>( documentation, "CLaser" );
	RegisterSCScriptTankEntityDependencies( documentation );
	RegisterSCScriptBaseTankEntity<CFuncTank>( documentation, "CBaseTank" );
	RegisterSCScriptBasePlayerAmmoEntity<CBasePlayerAmmo>( documentation, "CBasePlayerAmmo" );
	RegisterSCScriptBaseItemEntity<CItem>( documentation, "CItem" );
	RegisterSCScriptBaseButtonEntity<CBaseButton>( documentation, "CBaseButton" );
	RegisterSCScriptBaseDoorEntity<CBaseDoor>( documentation, "CBaseDoor" );
	RegisterSCScriptBaseCGib<CGib>( documentation, "CGib" );
	RegisterSCScriptItemRegistry( documentation );
	RegisterSCScriptEntityLoader( documentation );
	RegisterSCScriptCustomEntityDependencies( documentation );

	RegisterSCScriptBaseBaseClassEntity<CBaseEntity>(
	    documentation, "Baseclass for CBaseEntity.\nAllows calling of the base implementation of certain methods.",
	    "BaseEntity" );
	RegisterSCScriptBaseBaseClassMonsterEntity<CBaseMonster>(
	    documentation, "Baseclass for CBaseMonster.\nAllows calling of the base implementation of certain methods.",
	    "BaseMonster" );
	RegisterSCScriptBaseBaseClassTankEntity<CFuncTank>(
	    documentation, "Baseclass for CBaseTank.\nAllows calling of the base implementation of certain methods.",
	    "BaseTank" );
	RegisterSCScriptBaseBaseClassPlayerItemEntity<CBasePlayerItem>(
	    documentation, "Baseclass for CBasePlayerItem.\nAllows calling of the base implementation of certain methods.",
	    "BasePlayerItem" );
	RegisterSCScriptBaseBaseClassPlayerWeaponEntity<CBasePlayerWeapon>(
	    documentation, "Baseclass for CBasePlayerWeapon.\nAllows calling of the base implementation of certain methods.",
	    "BasePlayerWeapon" );
	RegisterSCScriptBaseBaseClassEntity<CBasePlayerAmmo>(
	    documentation, "Baseclass for CBasePlayerAmmo.\nAllows calling of the base implementation of certain methods.",
	    "BasePlayerAmmo" );
	RegisterSCScriptBaseBaseClassEntity<CItem>(
	    documentation, "Baseclass for CItem.\nAllows calling of the base implementation of certain methods.",
	    "BaseItem" );
}
