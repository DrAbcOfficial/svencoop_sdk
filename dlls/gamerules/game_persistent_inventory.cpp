#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "server/server_support.h"
#include "player/player.h"
#include "weapons/weapons.h"
#include "gamerules/game_persistent_inventory.h"
#include "utils/ModelUtils.h"
#include "iengineserver.h"
#include "utils/CStringHash.h"
#include <unordered_map>

CGamePersistentInventory *g_pGamePersistentInventory = NULL;

struct CGamePersistentInventory::Snapshot
{
	typedef std::unordered_map<CString, int> AmmoMap;

	Snapshot()
	{
		m_iUserId = 0;
		m_szIdentity[0] = 0;
		m_flFirstLoadTime = 0;
		m_iHealth = 0;
		m_iArmor = 0;
		m_fSuit = false;
		m_fLongJump = false;
		memset( m_fItems, 0, sizeof( m_fItems ) );
		m_iActiveItem = 0;
		memset( m_iClips, 0, sizeof( m_iClips ) );
		m_pNext = NULL;
	}

	int		m_iUserId;
	char	m_szIdentity[44];
	float	m_flFirstLoadTime;

	int		m_iHealth;
	int		m_iArmor;
	bool	m_fSuit;
	bool	m_fLongJump;
	bool	m_fItems[MAX_WEAPONS];
	int		m_iActiveItem;
	int		m_iClips[MAX_WEAPONS][2];

	AmmoMap m_Ammo;

	Snapshot *m_pNext;
};

CGamePersistentInventory::CGamePersistentInventory( const char *mapName )
{
	m_szMapName[0] = 0;
	UTIL_CopyString( m_szMapName, mapName, sizeof( m_szMapName ) );
	m_pInventories = NULL;
}

void CGamePersistentInventory::CapturePlayers()
{
	Snapshot *last = NULL;

	for ( int index = 1; index <= gpGlobals->maxClients; index++ )
	{
		CBasePlayer *player = (CBasePlayer *) UTIL_PlayerByIndex( index );

		if ( !player )
			continue;

		Snapshot *snapshot = Save( player );

		if ( !snapshot )
			continue;

		if ( last )
			last->m_pNext = snapshot;
		else
			m_pInventories = snapshot;

		last = snapshot;
	}
}

CGamePersistentInventory::~CGamePersistentInventory()
{
	Snapshot *snapshot = m_pInventories;

	m_szMapName[0] = 0;

	while ( snapshot )
	{
		Snapshot *next = snapshot->m_pNext;

		delete snapshot;
		snapshot = next;
	}

	m_pInventories = NULL;
}

CGamePersistentInventory::Snapshot *CGamePersistentInventory::Save( CBasePlayer *player )
{
	if ( !player || !player->IsPlayer() )
		return NULL;

	bool lan = CVAR_GET_FLOAT( "sv_lan" ) != 0.0f;
	Snapshot *snapshot = new Snapshot;
	const char *identity;

	snapshot->m_iUserId = GETPLAYERUSERID( player->pev ? player->edict() : NULL );

	if ( lan )
		identity = g_pEngineServer->GetAddressOfPlayer( player->pev ? player->edict() : NULL, 1 );
	else
		identity = GETPLAYERAUTHID( player->pev ? player->edict() : NULL );

	UTIL_CopyString( snapshot->m_szIdentity, identity, sizeof( snapshot->m_szIdentity ) );
	snapshot->m_iHealth = (int) ( player->pev->health > 1.0f ? player->pev->health : 1.0f );
	snapshot->m_iArmor = (int) ( player->pev->armorvalue > 0.0f ? player->pev->armorvalue : 0.0f );
	snapshot->m_fSuit = ( (unsigned int) player->pev->weapons & ( 1u << WEAPON_SUIT ) ) != 0;
	snapshot->m_fLongJump = player->m_fLongJump != FALSE;

	for ( int slot = 0; slot < MAX_ITEM_TYPES; slot++ )
	{
		CBasePlayerItem *item = player->m_rgpPlayerItems[slot];

		while ( item )
		{
			snapshot->m_fItems[item->m_iId] = true;

			CBasePlayerWeapon *weapon = dynamic_cast<CBasePlayerWeapon *>( item );

			if ( weapon && weapon->GetWeaponPtr() )
			{
				if ( weapon->m_iPrimaryAmmoType >= 0 )
					snapshot->m_iClips[item->m_iId][0] = weapon->m_iClip;

				if ( weapon->m_iSecondaryAmmoType >= 0 )
					snapshot->m_iClips[item->m_iId][1] = weapon->m_iClip2;
			}

			item = item->m_pNext;
		}
	}

	for ( int index = 0; index < MAX_AMMO_SLOTS; index++ )
	{
		const char *name = CBasePlayer::GetAmmoName( index );

		if ( name && *name )
			snapshot->m_Ammo.insert( std::make_pair( CString( name ), player->m_rgAmmo[index] ) );
	}

	CBasePlayerItem *active = player->m_pActiveItem;

	snapshot->m_iActiveItem = active ? active->m_iId : -1;
	return snapshot;
}

bool CGamePersistentInventory::Load( CBasePlayer *player )
{
	if ( !player || !player->IsPlayer() || !player->IsAlive() || !player->IsInWorld() )
		return false;

	bool lan = CVAR_GET_FLOAT( "sv_lan" ) != 0.0f;
	Snapshot *snapshot = m_pInventories;
	int userId = GETPLAYERUSERID( player->pev ? player->edict() : NULL );
	char identity[44] = {};
	const char *playerIdentity;

	if ( lan )
		playerIdentity = g_pEngineServer->GetAddressOfPlayer( player->pev ? player->edict() : NULL, 1 );
	else
		playerIdentity = GETPLAYERAUTHID( player->pev ? player->edict() : NULL );

	UTIL_CopyString( identity, playerIdentity, sizeof( identity ) );

	while ( snapshot )
	{
		if ( snapshot->m_iUserId == userId ||
		     ( snapshot->m_szIdentity[0] && identity[0] && !strcmp( snapshot->m_szIdentity, identity ) ) )
			break;

		snapshot = snapshot->m_pNext;
	}

	if ( !snapshot )
		return false;

	if ( snapshot->m_flFirstLoadTime > 0.0f && gpGlobals->time >= snapshot->m_flFirstLoadTime + 60.0f )
		return false;

	float health = (float) snapshot->m_iHealth;
	float armor = (float) snapshot->m_iArmor;

	player->pev->health = health > player->pev->health ? health : player->pev->health;
	player->pev->armorvalue = armor > player->pev->armorvalue ? armor : player->pev->armorvalue;

	if ( snapshot->m_fSuit )
	{
		player->pev->weapons |= (int) ( 1u << WEAPON_SUIT );
		player->SendScoreInfo( NULL );
	}

	if ( snapshot->m_fLongJump )
	{
		player->m_fLongJump = TRUE;
		g_engfuncs.pfnSetPhysicsKeyValue( player->pev ? player->edict() : NULL, "slj", "1" );
	}

	for ( int id = 0; id < MAX_WEAPONS; id++ )
	{
		if ( snapshot->m_fItems[id] )
			player->GiveNamedItem( CBasePlayerItem::ItemInfoArray[id].pszName, 0xD400, 0 );
	}

	for ( int slot = 0; slot < MAX_ITEM_TYPES; slot++ )
	{
		CBasePlayerItem *item = player->m_rgpPlayerItems[slot];

		while ( item )
		{
			CBasePlayerWeapon *weapon = dynamic_cast<CBasePlayerWeapon *>( item );

			if ( weapon && weapon->GetWeaponPtr() && snapshot->m_fItems[item->m_iId] )
			{
				if ( weapon->m_iPrimaryAmmoType >= 0 )
					weapon->m_iClip = snapshot->m_iClips[item->m_iId][0];

				if ( weapon->m_iSecondaryAmmoType >= 0 )
					weapon->m_iClip2 = snapshot->m_iClips[item->m_iId][1];
			}

			item = item->m_pNext;
		}
	}

	for ( Snapshot::AmmoMap::const_iterator ammo = snapshot->m_Ammo.begin(); ammo != snapshot->m_Ammo.end(); ammo++ )
	{
		int index = CBasePlayer::GetAmmoIndex( ammo->first.Data() );

		if ( index < 0 || ammo->second < player->m_rgAmmo[index] )
			continue;

		int amount = player->GetMaxAmmo( index );

		if ( amount >= ammo->second )
			amount = ammo->second >= 0 ? ammo->second : 0;

		player->m_rgAmmo[index] = amount;
	}

	if ( snapshot->m_iActiveItem != -1 )
	{
		for ( int slot = 0; slot < MAX_ITEM_TYPES; slot++ )
		{
			CBasePlayerItem *item = player->m_rgpPlayerItems[slot];

			while ( item )
			{
				if ( item->m_iId == snapshot->m_iActiveItem )
				{
					player->SelectItem( CBasePlayerItem::ItemInfoArray[item->m_iId].pszName );
					break;
				}

				item = item->m_pNext;
			}
		}
	}

	if ( snapshot->m_flFirstLoadTime <= 0.0f )
		snapshot->m_flFirstLoadTime = gpGlobals->time;

	return true;
}
