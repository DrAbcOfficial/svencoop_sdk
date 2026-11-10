#ifndef ITEM_INVENTORY_H
#define ITEM_INVENTORY_H

#include "CCustomRespawnDelay.h"

class CBaseMonster;

struct CInventoryItemNode
{
	EHANDLE				m_hItem;
	CInventoryItemNode *m_pNext;
};

class CItemInventory : public CBaseEntity
{
public:
	CItemInventory();
	void OnCreate( void );
	void Spawn( void );
	void Precache( void );
	void KeyValue( KeyValueData *data );
	int ObjectCaps( void );
	void OnSetOriginByMap( void );
	void UpdateOnRemove( void );
	CBaseEntity *Respawn( void );
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );
	void Touch( CBaseEntity *other );
	void EXPORT Materialize( void );
	void EXPORT IdleThink( void );
	void EXPORT HeldThink( void );
	bool CanCollect( CBaseEntity *entity );
	bool TryCollect( CBaseEntity *entity, bool ignoreTriggers );
	bool Activate( bool ignoreTriggers );
	bool Drop( bool force, CBaseEntity *caller, bool ignoreTriggers );
	void Return( bool ignoreTriggers );
	void Destroy( bool ignoreTriggers );
	void HandleTriggers( CBaseEntity *activator, string_t self, string_t team, string_t others );
	void AddItemEffects( void );
	void RemoveItemEffects( void );

	EHANDLE		m_hHolder;
	Vector		m_vecSpawnOrigin;
	Vector		m_vecSpawnAngles;
	int			m_iMoveType;
	string_t	m_szItemName;
	string_t	m_szItemGroup;
	string_t	m_szDisplayName;
	string_t	m_szDescription;
	string_t	m_iszIcon;
	int			m_iCollectLimit;
	int			m_iActivateLimit;
	float		m_flWeight;
	bool		m_fHiddenWhenCarried;
	int			m_iIdleSkin;
	int			m_iIdleBody;
	string_t	m_szIdleSequenceName;
	int			m_iIdleSequence;
	int			m_iCarriedSkin;
	int			m_iCarriedBody;
	string_t	m_szCarriedSequenceName;
	int			m_iCarriedSequence;
	string_t	m_szAllowedTargetNames;
	string_t	m_szAllowedClassNames;
	string_t	m_szAllowedTeams;
	int			m_iAllowedNpcClassify;
	string_t	m_szRequiresItemName;
	string_t	m_szRequiresItemGroup;
	int			m_iRequiresItemGroupNum;
	string_t	m_szItemNameMoved;
	string_t	m_szCantHaveItemName;
	string_t	m_szCantHaveItemGroup;
	int			m_iCantHaveItemGroupNum;
	string_t	m_szItemNameNotMoved;
	float		m_flMaximumHoldTime;
	float		m_flActivateWaitTime;
	float		m_flWearOutTime;
	bool		m_fCanBeActivated;
	bool		m_fHoldTimeWaitUntilActivated;
	bool		m_fCanBeDropped;
	float		m_flReturnTime;
	bool		m_fDelayedRespawn;
	bool		m_fKeepOnDeath;
	bool		m_fKeepOnRespawn;

	string_t	m_szTriggerOnCollectSelf;
	string_t	m_szTriggerOnCollectTeam;
	string_t	m_szTriggerOnCollectOther;
	string_t	m_szTriggerOnCantCollectSelf;
	string_t	m_szTriggerOnCantCollectTeam;
	string_t	m_szTriggerOnCantCollectOther;
	string_t	m_szTriggerOnDropSelf;
	string_t	m_szTriggerOnDropTeam;
	string_t	m_szTriggerOnDropOther;
	string_t	m_szTriggerOnCantDropSelf;
	string_t	m_szTriggerOnCantDropTeam;
	string_t	m_szTriggerOnCantDropOther;
	string_t	m_szTriggerOnActivateSelf;
	string_t	m_szTriggerOnActivateTeam;
	string_t	m_szTriggerOnActivateOther;
	string_t	m_szTriggerOnCantActivateSelf;
	string_t	m_szTriggerOnCantActivateTeam;
	string_t	m_szTriggerOnCantActivateOther;
	string_t	m_szTriggerOnUseSelf;
	string_t	m_szTriggerOnUseTeam;
	string_t	m_szTriggerOnUseOther;
	string_t	m_szTriggerOnWearingOutSelf;
	string_t	m_szTriggerOnWearingOutTeam;
	string_t	m_szTriggerOnWearingOutOther;
	string_t	m_szTriggerOnReturnSelf;
	string_t	m_szTriggerOnReturnTeam;
	string_t	m_szTriggerOnReturnOther;
	string_t	m_szTriggerOnMaterialise;
	string_t	m_szTriggerOnDestroy;

	bool		m_fEffectsWaitUntilActivated;
	bool		m_fEffectsPermanent;
	Vector		m_vecEffectGlowColor;
	bool		m_fEffectBlockWeapons;
	bool		m_fEffectInvulnerable;
	bool		m_fEffectInvisible;
	bool		m_fEffectNonSolid;
	float		m_flEffectRespiration;
	float		m_flEffectGravity;
	float		m_flEffectFriction;
	float		m_flEffectSpeed;
	float		m_flEffectDamage;
	int			m_iCollectCount;
	int			m_iActivateCount;
	float		m_flTouchedTime;
	float		m_flCollectTime;
	float		m_flDropTime;
	float		m_flActivateTime;
	bool		m_fRemovedByForced;
	bool		m_fHandledWearingOut;

	CCustomRespawnDelay m_RespawnDelay;

private:
	CBaseMonster *GetHolder( void );
};

int Inventory_Count( CBaseMonster *holder );
float Inventory_GetWeight( CBaseMonster *holder );
void Inventory_RemoveAllFromHolder( CBaseMonster *holder, bool drop, bool onlyBlockWeapons, bool keepOnRespawn );

#endif
