#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "ai/basemonster.h"
#include "items/item_inventory.h"
#include "items/InventoryRules.h"
#include "scripting/api/ASInventory.h"
#include "scripting/api/ASBaseEntity.h"
#include "items/CInventoryMisc.h"

void RegisterSCScriptInventory( CASDocumentation *documentation )
{
	RegisterSCScriptBaseEntity<CItemInventory>( documentation, "CItemInventory", true );

	documentation->RegisterObjectMethod(
	    "Try to collect this item.", "CItemInventory",
	    "bool TryCollect(CBaseEntity@ pOther, bool fIgnoreTriggers = false)",
	    asMETHOD( CItemInventory, TryCollect ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether this item can be collected by the given entity.", "CItemInventory",
	    "bool CanCollect(CBaseEntity@ pOther)",
	    asMETHOD( CItemInventory, CanCollect ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Drop this item if it's being carried.", "CItemInventory",
	    "bool Drop(bool fForce = false, CBaseEntity@ pCaller = null, bool fIgnoreTriggers = false)",
	    asMETHOD( CItemInventory, Drop ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Return this item to its original spawn location.", "CItemInventory",
	    "void Return(bool fIgnoreTriggers = false)",
	    asMETHOD( CItemInventory, Return ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Destroy this item.", "CItemInventory",
	    "void Destroy(bool fIgnoreTriggers = false)",
	    asMETHOD( CItemInventory, Destroy ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Prepares to add effects to holder.", "CItemInventory",
	    "void AddItemEffects()",
	    asMETHOD( CItemInventory, AddItemEffects ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Prepares to remove effects from holder.", "CItemInventory",
	    "void RemoveItemEffects()",
	    asMETHOD( CItemInventory, RemoveItemEffects ), asCALL_THISCALL );
	documentation->RegisterObjectProperty(
	    "The monster or player holding this item.", "CItemInventory",
	    "EHandle m_hHolder", asOFFSET( CItemInventory, m_hHolder ) );
	documentation->RegisterObjectProperty(
	    "Spawn-time origin.", "CItemInventory",
	    "Vector m_vecSpawnOrigin", asOFFSET( CItemInventory, m_vecSpawnOrigin ) );
	documentation->RegisterObjectProperty(
	    "Spawn-time angles.", "CItemInventory",
	    "Vector m_vecSpawnAngles", asOFFSET( CItemInventory, m_vecSpawnAngles ) );
	documentation->RegisterObjectProperty(
	    "Spawn-time move type.", "CItemInventory",
	    "int m_iMoveType", asOFFSET( CItemInventory, m_iMoveType ) );
	documentation->RegisterObjectProperty(
	    "Item name referred to by triggers, doesn't need to be unique -- remember the targetname refers to the entity "
	    "itself only.",
	    "CItemInventory",
	    "string_t m_szItemName", asOFFSET( CItemInventory, m_szItemName ) );
	documentation->RegisterObjectProperty(
	    "Group name referred to by triggers.", "CItemInventory",
	    "string_t m_szItemGroup", asOFFSET( CItemInventory, m_szItemGroup ) );
	documentation->RegisterObjectProperty(
	    "Friendly item name for client-side UI.", "CItemInventory",
	    "string_t m_szDisplayName", asOFFSET( CItemInventory, m_szDisplayName ) );
	documentation->RegisterObjectProperty(
	    "Friendly item description for client-side UI.", "CItemInventory",
	    "string_t m_szDescription", asOFFSET( CItemInventory, m_szDescription ) );
	documentation->RegisterObjectProperty(
	    "How many times the item can be picked up, destroyed when limit is reached (0 = infinite).", "CItemInventory",
	    "int m_iCollectLimit", asOFFSET( CItemInventory, m_iCollectLimit ) );
	documentation->RegisterObjectProperty(
	    "How many times the item can be activated (0 = infinite).", "CItemInventory",
	    "int m_iActivateLimit", asOFFSET( CItemInventory, m_iActivateLimit ) );
	documentation->RegisterObjectProperty(
	    "How heavy the item is (0-100),\nholders can hold multiple items up to a total weight 100, think of this as KG if "
	    "you like (though what person can carry 100KG!?).",
	    "CItemInventory",
	    "float m_flWeight", asOFFSET( CItemInventory, m_flWeight ) );
	documentation->RegisterObjectProperty(
	    "Model is hidden while it is being carried.", "CItemInventory",
	    "bool m_fHiddenWhenCarried", asOFFSET( CItemInventory, m_fHiddenWhenCarried ) );
	documentation->RegisterObjectProperty(
	    "Model skin while IDLE (not carried).", "CItemInventory",
	    "int m_iIdleSkin", asOFFSET( CItemInventory, m_iIdleSkin ) );
	documentation->RegisterObjectProperty(
	    "Model body while IDLE.", "CItemInventory",
	    "int m_iIdleBody", asOFFSET( CItemInventory, m_iIdleBody ) );
	documentation->RegisterObjectProperty(
	    "Model sequence name while IDLE.", "CItemInventory",
	    "string_t m_szIdleSequenceName", asOFFSET( CItemInventory, m_szIdleSequenceName ) );
	documentation->RegisterObjectProperty(
	    "Model sequence number while IDLE.", "CItemInventory",
	    "int m_iIdleSequence", asOFFSET( CItemInventory, m_iIdleSequence ) );
	documentation->RegisterObjectProperty(
	    "Model skin while CARRIED.", "CItemInventory",
	    "int m_iCarriedSkin", asOFFSET( CItemInventory, m_iCarriedSkin ) );
	documentation->RegisterObjectProperty(
	    "Model body while CARRIED.", "CItemInventory",
	    "int m_iCarriedBody", asOFFSET( CItemInventory, m_iCarriedBody ) );
	documentation->RegisterObjectProperty(
	    "Model sequence name while CARRIED.", "CItemInventory",
	    "string_t m_szCarriedSequenceName", asOFFSET( CItemInventory, m_szCarriedSequenceName ) );
	documentation->RegisterObjectProperty(
	    "Model sequence number while CARRIED.", "CItemInventory",
	    "int m_iCarriedSequence", asOFFSET( CItemInventory, m_iCarriedSequence ) );
	documentation->RegisterObjectProperty(
	    "CBaseEntity target name filters", "CItemInventory",
	    "string_t m_szAllowedTargetNames", asOFFSET( CItemInventory, m_szAllowedTargetNames ) );
	documentation->RegisterObjectProperty(
	    "CBaseEntity class name filters", "CItemInventory",
	    "string_t m_szAllowedClassNames", asOFFSET( CItemInventory, m_szAllowedClassNames ) );
	documentation->RegisterObjectProperty(
	    "Team filters", "CItemInventory",
	    "string_t m_szAllowedTeams", asOFFSET( CItemInventory, m_szAllowedTeams ) );
	documentation->RegisterObjectProperty(
	    "NPC classification filter", "CItemInventory",
	    "int m_iAllowedNpcClassify", asOFFSET( CItemInventory, m_iAllowedNpcClassify ) );
	documentation->RegisterObjectProperty(
	    "Require these item(s)", "CItemInventory",
	    "string_t m_szRequiresItemName", asOFFSET( CItemInventory, m_szRequiresItemName ) );
	documentation->RegisterObjectProperty(
	    "Require an item from these group(s)", "CItemInventory",
	    "string_t m_szRequiresItemGroup", asOFFSET( CItemInventory, m_szRequiresItemGroup ) );
	documentation->RegisterObjectProperty(
	    "Number of item(s) from the required group(s) required (0 = all)", "CItemInventory",
	    "int m_iRequiresItemGroupNum", asOFFSET( CItemInventory, m_iRequiresItemGroupNum ) );
	documentation->RegisterObjectProperty(
	    "These item(s) must have moved", "CItemInventory",
	    "string_t m_szItemNameMoved", asOFFSET( CItemInventory, m_szItemNameMoved ) );
	documentation->RegisterObjectProperty(
	    "Must not have these item(s)", "CItemInventory",
	    "string_t m_szCantHaveItemName", asOFFSET( CItemInventory, m_szCantHaveItemName ) );
	documentation->RegisterObjectProperty(
	    "Must not have an item in these group(s)", "CItemInventory",
	    "string_t m_szCantHaveItemGroup", asOFFSET( CItemInventory, m_szCantHaveItemGroup ) );
	documentation->RegisterObjectProperty(
	    "Number of item(s) from the can't have group(s) (0 = all)", "CItemInventory",
	    "int m_iCantHaveItemGroupNum", asOFFSET( CItemInventory, m_iCantHaveItemGroupNum ) );
	documentation->RegisterObjectProperty(
	    "These item(s) must NOT have moved", "CItemInventory",
	    "string_t m_szItemNameNotMoved", asOFFSET( CItemInventory, m_szItemNameNotMoved ) );
	documentation->RegisterObjectProperty(
	    "Limit to how long this item can be held for, forcibly dropped after (0 = no limit)", "CItemInventory",
	    "float m_flMaximumHoldTime", asOFFSET( CItemInventory, m_flMaximumHoldTime ) );
	documentation->RegisterObjectProperty(
	    "Time to wait between each activation (0 = none)", "CItemInventory",
	    "float m_flActivateWaitTime", asOFFSET( CItemInventory, m_flActivateWaitTime ) );
	documentation->RegisterObjectProperty(
	    "Perform a trigger prior to this item being forcibly dropped (0 = none)", "CItemInventory",
	    "float m_flWearOutTime", asOFFSET( CItemInventory, m_flWearOutTime ) );
	documentation->RegisterObjectProperty(
	    "Holder is allowed to activate this item by choice at any time", "CItemInventory",
	    "bool m_fCanBeActivated", asOFFSET( CItemInventory, m_fCanBeActivated ) );
	documentation->RegisterObjectProperty(
	    "Maximum hold time is not started until the item is activated (ignored if m_fCanBeActivated is false)", "CItemInventory",
	    "bool m_fHoldTimeWaitUntilActivated", asOFFSET( CItemInventory, m_fHoldTimeWaitUntilActivated ) );
	documentation->RegisterObjectProperty(
	    "Holder is allowed to drop this item by choice", "CItemInventory",
	    "bool m_fCanBeDropped", asOFFSET( CItemInventory, m_fCanBeDropped ) );
	documentation->RegisterObjectProperty(
	    "How long this item returns to its' original location when dropped (-1 = never, 0 = instant)", "CItemInventory",
	    "float m_flReturnTime", asOFFSET( CItemInventory, m_flReturnTime ) );
	documentation->RegisterObjectProperty(
	    "Delayed respawn on return (like with weapons/ammo/pickups)", "CItemInventory",
	    "bool m_fDelayedRespawn", asOFFSET( CItemInventory, m_fDelayedRespawn ) );
	documentation->RegisterObjectProperty(
	    "Holder still has the item after dying (i.e. so they can keep it while being revived)", "CItemInventory",
	    "bool m_fKeepOnDeath", asOFFSET( CItemInventory, m_fKeepOnDeath ) );
	documentation->RegisterObjectProperty(
	    "Holder still has the item after respawning (only applies to players -- re-equip NPC's manually)", "CItemInventory",
	    "bool m_fKeepOnRespawn", asOFFSET( CItemInventory, m_fKeepOnRespawn ) );
	documentation->RegisterObjectProperty(
	    "On successful collection (for collector)", "CItemInventory",
	    "string_t m_szTriggerOnCollectSelf", asOFFSET( CItemInventory, m_szTriggerOnCollectSelf ) );
	documentation->RegisterObjectProperty(
	    "On successful collection (for collector's team)", "CItemInventory",
	    "string_t m_szTriggerOnCollectTeam", asOFFSET( CItemInventory, m_szTriggerOnCollectTeam ) );
	documentation->RegisterObjectProperty(
	    "On successful collection (for everyone else)", "CItemInventory",
	    "string_t m_szTriggerOnCollectOther", asOFFSET( CItemInventory, m_szTriggerOnCollectOther ) );
	documentation->RegisterObjectProperty(
	    "On failed collection (for collector)", "CItemInventory",
	    "string_t m_szTriggerOnCantCollectSelf", asOFFSET( CItemInventory, m_szTriggerOnCantCollectSelf ) );
	documentation->RegisterObjectProperty(
	    "On failed collection (for collector's team)", "CItemInventory",
	    "string_t m_szTriggerOnCantCollectTeam", asOFFSET( CItemInventory, m_szTriggerOnCantCollectTeam ) );
	documentation->RegisterObjectProperty(
	    "On failed collection (for everyone else)", "CItemInventory",
	    "string_t m_szTriggerOnCantCollectOther", asOFFSET( CItemInventory, m_szTriggerOnCantCollectOther ) );
	documentation->RegisterObjectProperty(
	    "On successful drop (for collector)", "CItemInventory",
	    "string_t m_szTriggerOnDropSelf", asOFFSET( CItemInventory, m_szTriggerOnDropSelf ) );
	documentation->RegisterObjectProperty(
	    "On successful drop (for collector's team)", "CItemInventory",
	    "string_t m_szTriggerOnDropTeam", asOFFSET( CItemInventory, m_szTriggerOnDropTeam ) );
	documentation->RegisterObjectProperty(
	    "On successful drop (for everyone else)", "CItemInventory",
	    "string_t m_szTriggerOnDropOther", asOFFSET( CItemInventory, m_szTriggerOnDropOther ) );
	documentation->RegisterObjectProperty(
	    "On failed drop (for collector)", "CItemInventory",
	    "string_t m_szTriggerOnCantDropSelf", asOFFSET( CItemInventory, m_szTriggerOnCantDropSelf ) );
	documentation->RegisterObjectProperty(
	    "On failed drop (for collector's team)", "CItemInventory",
	    "string_t m_szTriggerOnCantDropTeam", asOFFSET( CItemInventory, m_szTriggerOnCantDropTeam ) );
	documentation->RegisterObjectProperty(
	    "On failed drop (for everyone else)", "CItemInventory",
	    "string_t m_szTriggerOnCantDropOther", asOFFSET( CItemInventory, m_szTriggerOnCantDropOther ) );
	documentation->RegisterObjectProperty(
	    "On self-activation (for collector)", "CItemInventory",
	    "string_t m_szTriggerOnActivateSelf", asOFFSET( CItemInventory, m_szTriggerOnActivateSelf ) );
	documentation->RegisterObjectProperty(
	    "On self-activation (for collector's team)", "CItemInventory",
	    "string_t m_szTriggerOnActivateTeam", asOFFSET( CItemInventory, m_szTriggerOnActivateTeam ) );
	documentation->RegisterObjectProperty(
	    "On self-activation (for everyone else)", "CItemInventory",
	    "string_t m_szTriggerOnActivateOther", asOFFSET( CItemInventory, m_szTriggerOnActivateOther ) );
	documentation->RegisterObjectProperty(
	    "On failed self - activation(for collector)", "CItemInventory",
	    "string_t m_szTriggerOnCantActivateSelf", asOFFSET( CItemInventory, m_szTriggerOnCantActivateSelf ) );
	documentation->RegisterObjectProperty(
	    "On failed self-activation (for collector's team)", "CItemInventory",
	    "string_t m_szTriggerOnCantActivateTeam", asOFFSET( CItemInventory, m_szTriggerOnCantActivateTeam ) );
	documentation->RegisterObjectProperty(
	    "On failed self-activation (for everyone else)", "CItemInventory",
	    "string_t m_szTriggerOnCantActivateOther", asOFFSET( CItemInventory, m_szTriggerOnCantActivateOther ) );
	documentation->RegisterObjectProperty(
	    "On use by trigger (for collector)", "CItemInventory",
	    "string_t m_szTriggerOnUseSelf", asOFFSET( CItemInventory, m_szTriggerOnUseSelf ) );
	documentation->RegisterObjectProperty(
	    "On use by trigger (for collector's team)", "CItemInventory",
	    "string_t m_szTriggerOnUseTeam", asOFFSET( CItemInventory, m_szTriggerOnUseTeam ) );
	documentation->RegisterObjectProperty(
	    "On use by trigger (for everyone else)", "CItemInventory",
	    "string_t m_szTriggerOnUseOther", asOFFSET( CItemInventory, m_szTriggerOnUseOther ) );
	documentation->RegisterObjectProperty(
	    "On wearing out (for collector)", "CItemInventory",
	    "string_t m_szTriggerOnWearingOutSelf", asOFFSET( CItemInventory, m_szTriggerOnWearingOutSelf ) );
	documentation->RegisterObjectProperty(
	    "On wearing out (for collector's team)", "CItemInventory",
	    "string_t m_szTriggerOnWearingOutTeam", asOFFSET( CItemInventory, m_szTriggerOnWearingOutTeam ) );
	documentation->RegisterObjectProperty(
	    "On wearing out (for everyone else)", "CItemInventory",
	    "string_t m_szTriggerOnWearingOutOther", asOFFSET( CItemInventory, m_szTriggerOnWearingOutOther ) );
	documentation->RegisterObjectProperty(
	    "On return (for collector)", "CItemInventory",
	    "string_t m_szTriggerOnReturnSelf", asOFFSET( CItemInventory, m_szTriggerOnReturnSelf ) );
	documentation->RegisterObjectProperty(
	    "On return (for collector's team)", "CItemInventory",
	    "string_t m_szTriggerOnReturnTeam", asOFFSET( CItemInventory, m_szTriggerOnReturnTeam ) );
	documentation->RegisterObjectProperty(
	    "On return (for everyone else)", "CItemInventory",
	    "string_t m_szTriggerOnReturnOther", asOFFSET( CItemInventory, m_szTriggerOnReturnOther ) );
	documentation->RegisterObjectProperty(
	    "On materialise after return", "CItemInventory",
	    "string_t m_szTriggerOnMaterialise", asOFFSET( CItemInventory, m_szTriggerOnMaterialise ) );
	documentation->RegisterObjectProperty(
	    "On destruction", "CItemInventory",
	    "string_t m_szTriggerOnDestroy", asOFFSET( CItemInventory, m_szTriggerOnDestroy ) );
	documentation->RegisterObjectProperty(
	    "Holder effects do not apply until the item is activated (ignored if m_fCanBeActivated is false)", "CItemInventory",
	    "bool m_fEffectsWaitUntilActivated", asOFFSET( CItemInventory, m_fEffectsWaitUntilActivated ) );
	documentation->RegisterObjectProperty(
	    "Holder keeps effects after dropping the item", "CItemInventory",
	    "bool m_fEffectsPermanent", asOFFSET( CItemInventory, m_fEffectsPermanent ) );
	documentation->RegisterObjectProperty(
	    "Holder has a glow shell", "CItemInventory",
	    "Vector m_vecEffectGlowColor", asOFFSET( CItemInventory, m_vecEffectGlowColor ) );
	documentation->RegisterObjectProperty(
	    "Holder can't use weapons", "CItemInventory",
	    "bool m_fEffectBlockWeapons", asOFFSET( CItemInventory, m_fEffectBlockWeapons ) );
	documentation->RegisterObjectProperty(
	    "Holder is invulnerable (god mode)", "CItemInventory",
	    "bool m_fEffectInvulnerable", asOFFSET( CItemInventory, m_fEffectInvulnerable ) );
	documentation->RegisterObjectProperty(
	    "Holder is invisible (render + non-targetable)", "CItemInventory",
	    "bool m_fEffectInvisible", asOFFSET( CItemInventory, m_fEffectInvisible ) );
	documentation->RegisterObjectProperty(
	    "Holder is non-solid", "CItemInventory",
	    "bool m_fEffectNonSolid", asOFFSET( CItemInventory, m_fEffectNonSolid ) );
	documentation->RegisterObjectProperty(
	    "Extra/less breathing time underwater in seconds", "CItemInventory",
	    "float m_flEffectRespiration", asOFFSET( CItemInventory, m_flEffectRespiration ) );
	documentation->RegisterObjectProperty(
	    "Gravity modifier (%)", "CItemInventory",
	    "float m_flEffectGravity", asOFFSET( CItemInventory, m_flEffectGravity ) );
	documentation->RegisterObjectProperty(
	    "Movement friction modifier (%)", "CItemInventory",
	    "float m_flEffectFriction", asOFFSET( CItemInventory, m_flEffectFriction ) );
	documentation->RegisterObjectProperty(
	    "Movement speed modifier (%)", "CItemInventory",
	    "float m_flEffectSpeed", asOFFSET( CItemInventory, m_flEffectSpeed ) );
	documentation->RegisterObjectProperty(
	    "Damage modifier (%)", "CItemInventory",
	    "float m_flEffectDamage", asOFFSET( CItemInventory, m_flEffectDamage ) );
	documentation->RegisterObjectProperty(
	    "Number of times the item has been collected", "CItemInventory",
	    "int m_iCollectCount", asOFFSET( CItemInventory, m_iCollectCount ) );
	documentation->RegisterObjectProperty(
	    "Number of times the item has been self-activated", "CItemInventory",
	    "int m_iActivateCount", asOFFSET( CItemInventory, m_iActivateCount ) );
	documentation->RegisterObjectProperty(
	    "Time the item was last touched", "CItemInventory",
	    "float m_flTouchedTime", asOFFSET( CItemInventory, m_flTouchedTime ) );
	documentation->RegisterObjectProperty(
	    "Time the item was last collected", "CItemInventory",
	    "float m_flCollectTime", asOFFSET( CItemInventory, m_flCollectTime ) );
	documentation->RegisterObjectProperty(
	    "Time the item was last dropped", "CItemInventory",
	    "float m_flDropTime", asOFFSET( CItemInventory, m_flDropTime ) );
	documentation->RegisterObjectProperty(
	    "Time the item was last self-activated", "CItemInventory",
	    "float m_flActivateTime", asOFFSET( CItemInventory, m_flActivateTime ) );
	documentation->RegisterObjectProperty(
	    "Item was just removed by force", "CItemInventory",
	    "bool m_fRemovedByForced", asOFFSET( CItemInventory, m_fRemovedByForced ) );
	documentation->RegisterObjectProperty(
	    "Item has performed its wearing out trigger", "CItemInventory",
	    "bool m_fHandledWearingOut", asOFFSET( CItemInventory, m_fHandledWearingOut ) );
}

void RegisterSCScriptInventoryRules( CASDocumentation *documentation )
{
	documentation->RegisterObjectType(
	    "Inventory rules: Contained by CBaseToggle to hold and\nmanage inventory rules attached to a triggerable entity",
	    "InventoryRules", 0, asOBJ_REF | asOBJ_NOCOUNT );

	documentation->RegisterObjectMethod(
	    "Returns whether the owning trigger can be triggered by the given activator.", "InventoryRules",
	    "bool CanUseTrigger(CBaseEntity@ pActivator, bool fContinueByDefault = false)",
	    asMETHOD( CInventoryRules, CanUseTrigger ), asCALL_THISCALL );
	documentation->RegisterObjectProperty(
	    "Owning entity", "InventoryRules",
	    "EHandle m_hEntity", asOFFSET( CInventoryRules, m_hEntity ) );
	documentation->RegisterObjectProperty(
	    "Inventory: Require these item(s)", "InventoryRules",
	    "string_t m_szRequiresItemName", asOFFSET( CInventoryRules, m_szRequiresItemName ) );
	documentation->RegisterObjectProperty(
	    "Inventory: Require an item from these group(s)", "InventoryRules",
	    "string_t m_szRequiresItemGroup", asOFFSET( CInventoryRules, m_szRequiresItemGroup ) );
	documentation->RegisterObjectProperty(
	    "Inventory: Number of item(s) from the required group(s) required (0 = all)", "InventoryRules",
	    "int m_iRequiresItemGroupNum", asOFFSET( CInventoryRules, m_iRequiresItemGroupNum ) );
	documentation->RegisterObjectProperty(
	    "Inventory: Must not have these item(s)", "InventoryRules",
	    "string_t m_szCantHaveItemName", asOFFSET( CInventoryRules, m_szCantHaveItemName ) );
	documentation->RegisterObjectProperty(
	    "Inventory: Must not have an item in these group(s)", "InventoryRules",
	    "string_t m_szCantHaveItemGroup", asOFFSET( CInventoryRules, m_szCantHaveItemGroup ) );
	documentation->RegisterObjectProperty(
	    "Inventory: Number of item(s) from the can't have group(s) (0 = all)", "InventoryRules",
	    "int m_iCantHaveItemGroupNum", asOFFSET( CInventoryRules, m_iCantHaveItemGroupNum ) );
	documentation->RegisterObjectProperty(
	    "On pass: Ignore item's on use triggers?", "InventoryRules",
	    "bool m_fIgnoreUseTriggers", asOFFSET( CInventoryRules, m_fIgnoreUseTriggers ) );
	documentation->RegisterObjectProperty(
	    "On pass: Drop item(s)", "InventoryRules",
	    "string_t m_szDropItemNameOnUse", asOFFSET( CInventoryRules, m_szDropItemNameOnUse ) );
	documentation->RegisterObjectProperty(
	    "On pass: Drop item(s) in these group(s)", "InventoryRules",
	    "string_t m_szDropItemGroupOnUse", asOFFSET( CInventoryRules, m_szDropItemGroupOnUse ) );
	documentation->RegisterObjectProperty(
	    "On pass: Ignore item's on drop triggers?", "InventoryRules",
	    "bool m_fIgnoreDropTriggers", asOFFSET( CInventoryRules, m_fIgnoreDropTriggers ) );
	documentation->RegisterObjectProperty(
	    "On pass: Return item(s)", "InventoryRules",
	    "string_t m_szReturnItemNameOnUse", asOFFSET( CInventoryRules, m_szReturnItemNameOnUse ) );
	documentation->RegisterObjectProperty(
	    "On pass: Return item(s) in these group(s)", "InventoryRules",
	    "string_t m_szReturnItemGroupOnUse", asOFFSET( CInventoryRules, m_szReturnItemGroupOnUse ) );
	documentation->RegisterObjectProperty(
	    "On pass: Ignore item's on return triggers?", "InventoryRules",
	    "bool m_fIgnoreReturnTriggers", asOFFSET( CInventoryRules, m_fIgnoreReturnTriggers ) );
	documentation->RegisterObjectProperty(
	    "On pass: Destroy item(s)", "InventoryRules",
	    "string_t m_szDestroyItemNameOnUse", asOFFSET( CInventoryRules, m_szDestroyItemNameOnUse ) );
	documentation->RegisterObjectProperty(
	    "On pass: Destroy item(s) in these group(s)", "InventoryRules",
	    "string_t m_szDestroyItemGroupOnUse", asOFFSET( CInventoryRules, m_szDestroyItemGroupOnUse ) );
	documentation->RegisterObjectProperty(
	    "On pass: Ignore item's on destroy triggers?", "InventoryRules",
	    "bool m_fIgnoreDestroyTriggers", asOFFSET( CInventoryRules, m_fIgnoreDestroyTriggers ) );
	documentation->RegisterObjectProperty(
	    "Target: Inventory rules failed", "InventoryRules",
	    "string_t m_szTargetOnFail", asOFFSET( CInventoryRules, m_szTargetOnFail ) );
}

void RegisterSCScriptInventoryMisc( CASDocumentation *documentation )
{
	documentation->RegisterObjectType( "Misc functions for handling inventories across classes",
	                                   "CInventoryMisc", 0, asOBJ_REF | asOBJ_NOCOUNT );

	documentation->RegisterObjectMethod(
	    "Returns whether the given entity is a valid inventory holder", "CInventoryMisc",
	    "bool IsValidInventoryHolder(CBaseEntity@ pOther)",
	    asMETHOD( CInventoryMisc, IsValidInventoryHolder ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether the given value is in the given rule", "CInventoryMisc",
	    "bool CheckTokenStringRule(const string& in szRule, const string& in szValue)",
	    asMETHOD( CInventoryMisc, CheckTokenStringRule ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Counts the number of inventory items held by the given holder", "CInventoryMisc",
	    "int Count(CBaseMonster@ pHolder)",
	    asMETHOD( CInventoryMisc, Count ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the total weight of all inventory items held by the given holder", "CInventoryMisc",
	    "float GetWeight(CBaseMonster@ pHolder)",
	    asMETHOD( CInventoryMisc, GetWeight ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Counts all the existing items in a group", "CInventoryMisc",
	    "int ExistsInGroup(const string& in szGroup)",
	    asMETHOD( CInventoryMisc, ExistsInGroup ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns count of how many items someone holds of a group", "CInventoryMisc",
	    "int HaveNumFromGroup(CBaseMonster@ pHolder, const string& in szGroup)",
	    asMETHOD( CInventoryMisc, HaveNumFromGroup ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns true if is holding all items of a group", "CInventoryMisc",
	    "bool HasAllInGroup(CBaseMonster@ pHolder, const string& in szGroup)",
	    asMETHOD( CInventoryMisc, HasAllInGroup ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Clears all held inventory", "CInventoryMisc",
	    "void RemoveAllFromHolder(CBaseMonster@ pHolder, const bool fDrop = false, const bool fBlockingWeaponsOnly = "
	    "false, const bool fKeepOnRespawn = false)",
	    asMETHOD( CInventoryMisc, RemoveAllFromHolder ), asCALL_THISCALL );
	documentation->RegisterGlobalProperty(
	    "Inventory misc functions instance", "CInventoryMisc g_InventoryMisc", CInventoryMisc::GetInstance() );
}
