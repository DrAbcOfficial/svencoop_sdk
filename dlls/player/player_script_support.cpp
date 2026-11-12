#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player/player.h"
#include "weapons/weapons.h"
#include "items/CBasePickupObject.h"
#include "player/client.h"

void CBasePlayer::SetItemPickupTimes( float time )
{
	for ( int index = 0; index < MAX_WEAPONS; index++ )
		m_flNextWeaponPickup[index] = time;
}

void CBasePlayer::StartPlayerDeathView( const Vector &position, const Vector &viewAngle )
{
	if ( IsAlive() )
		return;

	m_afPhysicsFlags |= PFLAG_OBSERVER;
	pev->view_ofs = Vector( 0, 0, -32 );
	pev->v_angle = viewAngle;
	pev->angles = pev->v_angle;
	pev->fixangle = 1;
	pev->solid = SOLID_NOT;
	pev->takedamage = DAMAGE_NO;
	pev->movetype = MOVETYPE_NONE;
}

CBasePlayerItem *CBasePlayer::DropPlayerAmmo( const char *itemName, bool giveToPlayer, bool secondary )
{
	if ( !itemName || stricmp( itemName, "weapon_shockrifle" ) )
	{
		int dropWeapons = (int) CVAR_GET_FLOAT( "mp_dropweapons" );

		if ( dropWeapons <= -1 )
			return NULL;

		if ( dropWeapons < 1 && ( !itemName || stricmp( itemName, "weapon_minigun" ) ) )
			return NULL;
	}

	CBasePlayerItem *item = itemName && itemName[0] ? FindItem( itemName ) : (CBasePlayerItem *) m_pActiveItem;

	if ( !item )
		return NULL;

	CBasePlayerWeapon *weapon = static_cast<CBasePlayerWeapon *>( item->GetWeaponPtr() );

	if ( !weapon )
		return NULL;

	const ItemInfo &itemInfo = CBasePlayerItem::ItemInfoArray[item->m_iId];
	const char *ammoName = secondary ? itemInfo.pszAmmo2 : itemInfo.pszAmmo1;
	int ammoIndex = GetAmmoIndex( ammoName );

	if ( ammoIndex == -1 )
		return NULL;

	int amount = secondary ? weapon->GetAmmo2Drop() : weapon->GetAmmo1Drop();

	if ( !amount )
		return NULL;

	int remaining = m_rgAmmo[ammoIndex] - amount;

	if ( !m_rgAmmo[ammoIndex] || ( secondary ? remaining < 0 : remaining <= 0 ) )
		return NULL;

	const ItemInfo &weaponInfo = CBasePlayerItem::ItemInfoArray[weapon->m_iId];
	const char *className = secondary ? weaponInfo.pszAmmo2Entity : weaponInfo.pszAmmo1Entity;

	if ( !className )
		return NULL;

	if ( giveToPlayer )
	{
		Vector start = pev->origin + pev->view_ofs;

		UTIL_MakeVectors( pev->v_angle );

		TraceResult trace;

		UTIL_TraceLine( start, start + gpGlobals->v_forward * 100, dont_ignore_monsters, edict(), &trace );

		edict_t *hit = trace.pHit ? trace.pHit : INDEXENT( 0 );
		CBaseEntity *entity = hit && !hit->free ? (CBaseEntity *) hit->pvPrivateData : NULL;

		if ( entity && entity->IsPlayer() && entity->IsAlive() )
		{
			CBasePlayer *recipient = static_cast<CBasePlayer *>( entity );
			int maximum = recipient->GetMaxAmmo( ammoName );

			if ( amount + recipient->m_rgAmmo[ammoIndex] <= maximum )
			{
				recipient->GiveAmmo( amount, ammoName, maximum, true, NULL );
				m_rgAmmo[ammoIndex] = remaining;
				return NULL;
			}
		}
	}

	UTIL_MakeVectors( pev->v_angle );

	CBaseEntity *ammo = CBaseEntity::Create( className, pev->origin, pev->angles, true, edict() );

	if ( !ammo )
	{
		ALERT( at_console, "CreatePlayerItem: Can't create '%s'!\n", className );
		return NULL;
	}

	if ( !dynamic_cast<CBasePickupObject *>( ammo ) )
	{
		ALERT( at_console, "CreatePlayerItem: '%s' is not a valid player item!\n", className );
		UTIL_Remove( ammo );
		return NULL;
	}

	ammo->pev->spawnflags |= SF_NORESPAWN | SF_CREATEDWEAPON;
	DispatchSpawn( ammo->edict() );

	if ( !DropItem( ammo, amount, 0, true, static_cast<DROP_TYPE>( 1 ) ) )
		return NULL;

	m_rgAmmo[ammoIndex] = remaining;
	return item;
}
