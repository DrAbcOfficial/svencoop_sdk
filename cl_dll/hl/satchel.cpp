/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "weaponinfo.h"
#include "event_flags.h"

LINK_ENTITY_TO_CLASS( weapon_satchel, CSatchel );

void CSatchel::Spawn( void )
{
	Precache();

	m_iId = WEAPON_SATCHEL;
	m_iDefaultAmmo = SATCHEL_DEFAULT_GIVE;

	if ( CClassicModeWrapper::isEnabled() )
		SET_MODEL( ENT( pev ), GetW_Model( "models/hlclassic/w_satchel.mdl" ) );
	else
		SET_MODEL( ENT( pev ), GetW_Model( "models/w_satchel.mdl" ) );

	m_flCustomDmg = pev->dmg;

	FallInit();// get ready to fall down.
}

void CSatchel::Precache( void )
{
	PrecacheCustomModels();

	bool bClassic = CClassicModeWrapper::isEnabled();

	PRECACHE_MODEL( bClassic ? "models/hlclassic/v_satchel.mdl" : "models/v_satchel.mdl" );
	PRECACHE_MODEL( bClassic ? "models/hlclassic/v_satchel_radio.mdl" : "models/v_satchel_radio.mdl" );
	PRECACHE_MODEL( bClassic ? "models/hlclassic/w_satchel.mdl" : "models/w_satchel.mdl" );
	PRECACHE_MODEL( bClassic ? "models/hlclassic/p_satchel.mdl" : "models/p_satchel.mdl" );
	PRECACHE_MODEL( bClassic ? "models/hlclassic/p_satchel_radio.mdl" : "models/p_satchel_radio.mdl" );
}

bool CSatchel::GetItemInfo( ItemInfo *p )
{
	p->pszName = STRING( pev->classname );
	p->pszAmmo1 = "Satchel Charge";
	p->iMaxAmmo1 = SATCHEL_MAX_CARRY;
	p->pszAmmo1Entity = "weapon_satchel";
	p->iAmmo1Drop = SATCHEL_AMMO1_DROP;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo2Entity = NULL;
	p->iAmmo2Drop = AMMO_DROP_NONE;
	p->iMaxClip = SATCHEL_MAX_CLIP;
	p->iSlot = 4;
	p->iPosition = 1;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY | ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;
	p->iId = m_iId = WEAPON_SATCHEL;
	p->iWeight = SATCHEL_WEIGHT;

	return true;
}

bool CSatchel::AddToPlayer( CBasePlayer *pPlayer )
{
	return false;
}

bool CSatchel::AddDuplicate( CBasePlayerItem *pOriginal )
{
	return true;
}

bool CSatchel::IsUseable( void )
{
	// either we have satchels to throw, or a charge out there to detonate
	return m_hPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] > 0 || m_chargeReady != SATCHEL_IDLE;
}

bool CSatchel::CanDeploy( void )
{
	return m_hPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] > 0 || m_chargeReady != SATCHEL_IDLE;
}

bool CSatchel::Deploy( void )
{
	if ( m_chargeReady == SATCHEL_RELOAD )
		m_chargeReady = SATCHEL_IDLE;

	const char *pszAnimExt;
	bool bClassic = CClassicModeWrapper::isEnabled();

	if ( m_chargeReady == SATCHEL_IDLE )
	{
		SetV_Model( GetV_Model( bClassic ? "models/hlclassic/v_satchel.mdl" : "models/v_satchel.mdl" ) );
		SetP_Model( GetP_Model( bClassic ? "models/hlclassic/p_satchel.mdl" : "models/p_satchel.mdl" ) );
		SendWeaponAnim( SATCHEL_DRAW );
		pszAnimExt = "trip";
	}
	else
	{
		SetV_Model( GetV_Model( bClassic ? "models/hlclassic/v_satchel_radio.mdl" : "models/v_satchel_radio.mdl" ) );
		SetP_Model( GetP_Model( bClassic ? "models/hlclassic/p_satchel_radio.mdl" : "models/p_satchel_radio.mdl" ) );
		SendWeaponAnim( SATCHEL_RADIO_DRAW );
		pszAnimExt = "hive";
	}

	strncpy( m_hPlayer->m_szAnimExtention, pszAnimExt, sizeof( m_hPlayer->m_szAnimExtention ) );

	m_hPlayer->SetAnimation( PLAYER_DEPLOY );

	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );

	return true;
}

void CSatchel::Holster( int skiplocal /* = 0 */ )
{
	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;

	PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), g_evWeaponCustom, 0.0, (float *)&g_vecZero, (float *)&g_vecZero,
		WEAPONCUSTOM_SND_NULL, 1.0, WEAPONCUSTOM_SOUND, CHAN_WEAPON, 0, 0 );

	if ( !m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] && ( m_chargeReady == SATCHEL_IDLE || m_chargeReady == SATCHEL_RELOAD ) )
	{
		SetThink( &CBasePlayerItem::DestroyItem );
		pev->nextthink = UTIL_WeaponTimeBase() + 0.1;
	}

	m_hPlayer->pev->viewmodel = 0;
}

void CSatchel::PrimaryAttack( void )
{
	switch ( m_chargeReady )
	{
	case SATCHEL_IDLE:
		Throw();
		break;

	case SATCHEL_READY:
		SendWeaponAnim( SATCHEL_RADIO_FIRE );

		m_chargeReady = SATCHEL_RELOAD;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
		break;

	case SATCHEL_RELOAD:
		// we're reloading, don't allow fire
		break;
	}
}

void CSatchel::SecondaryAttack( void )
{
	if ( m_chargeReady != SATCHEL_RELOAD )
		Throw();
}

// The satchel itself is created by the server, the client switches to the radio
void CSatchel::Throw( void )
{
	if ( m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] <= 0 )
		return;

	bool bClassic = CClassicModeWrapper::isEnabled();

	SetV_Model( GetV_Model( bClassic ? "models/hlclassic/v_satchel_radio.mdl" : "models/v_satchel_radio.mdl" ) );
	SetP_Model( GetP_Model( bClassic ? "models/hlclassic/p_satchel_radio.mdl" : "models/p_satchel_radio.mdl" ) );

	SendWeaponAnim( SATCHEL_RADIO_DRAW );

	// player "shoot" animation
	m_hPlayer->SetAnimation( PLAYER_ATTACK1 );

	m_chargeReady = SATCHEL_READY;

	m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ]--;

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
}

void CSatchel::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	switch ( m_chargeReady )
	{
	case SATCHEL_IDLE:
		SendWeaponAnim( SATCHEL_FIDGET1 );
		// use tripmine animations
		strncpy( m_hPlayer->m_szAnimExtention, "trip", sizeof( m_hPlayer->m_szAnimExtention ) );
		break;

	case SATCHEL_READY:
		SendWeaponAnim( SATCHEL_RADIO_FIDGET1 );
		// use hivehand animations
		strncpy( m_hPlayer->m_szAnimExtention, "hive", sizeof( m_hPlayer->m_szAnimExtention ) );
		break;

	case SATCHEL_RELOAD:
	{
		if ( !m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] )
		{
			m_chargeReady = SATCHEL_IDLE;
			RetireWeapon();
			return;
		}

		bool bClassic = CClassicModeWrapper::isEnabled();

		SetV_Model( GetV_Model( bClassic ? "models/hlclassic/v_satchel.mdl" : "models/v_satchel.mdl" ) );
		SetP_Model( GetP_Model( bClassic ? "models/hlclassic/p_satchel.mdl" : "models/p_satchel.mdl" ) );
		SendWeaponAnim( SATCHEL_DRAW );

		// use tripmine animations
		strncpy( m_hPlayer->m_szAnimExtention, "trip", sizeof( m_hPlayer->m_szAnimExtention ) );

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_chargeReady = SATCHEL_IDLE;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
		break;
	}
	}

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );// how long till we do this again.
}

void CSatchel::GetWeaponData( weapon_data_t *data )
{
	data->iuser1 = m_iPrimaryAmmoType;
	data->iuser2 = m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ];
	data->iuser3 = m_iSecondaryAmmoType;
	data->iuser4 = m_hPlayer->m_rgAmmo[ m_iSecondaryAmmoType ];
	data->fuser1 = m_chargeReady;
}

void CSatchel::SetWeaponData( weapon_data_t *data )
{
	m_iPrimaryAmmoType = data->iuser1;
	m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] = data->iuser2;
	m_iSecondaryAmmoType = data->iuser3;
	m_hPlayer->m_rgAmmo[ m_iSecondaryAmmoType ] = data->iuser4;
	m_chargeReady = (int)data->fuser1;
}
