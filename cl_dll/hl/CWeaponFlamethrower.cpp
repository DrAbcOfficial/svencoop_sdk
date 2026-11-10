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

void CWeaponFlamethrower::Precache( void )
{
	CBasePlayerWeapon::Precache();

	PRECACHE_MODEL( "models/v_flamethrower.mdl" );
	PRECACHE_MODEL( "models/p_flamethrower.mdl" );
	PRECACHE_MODEL( "models/w_flamethrower.mdl" );

	PrecacheCustomModels();

	UTIL_PrecacheOther( "flame" );

	PRECACHE_SOUND( "tfc/weapons/flmfire2.wav" );
}

void CWeaponFlamethrower::Spawn( void )
{
	m_iId = WEAPON_FLAMETHROWER;

	Precache();

	m_iDefaultAmmo = FLAMETHROWER_DEFAULT_GIVE;
	m_flCustomDmg = pev->dmg ? pev->dmg : FLAMETHROWER_DAMAGE;

	SET_MODEL( ENT( pev ), GetW_Model( "models/w_flamethrower.mdl" ) );

	FallInit();// get ready to fall down.
}

bool CWeaponFlamethrower::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		AddToPlayerMessage();
		return true;
	}

	return false;
}

bool CWeaponFlamethrower::GetItemInfo( ItemInfo *p )
{
	p->pszName = STRING( pev->classname );
	p->pszAmmo1 = "fuel";
	p->iMaxAmmo1 = FUEL_MAX_CARRY;
	p->pszAmmo1Entity = "ammo_fuel";
	p->iAmmo1Drop = FLAMETHROWER_AMMO1_DROP;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo2Entity = NULL;
	p->iAmmo2Drop = AMMO_DROP_NONE;
	p->iMaxClip = FLAMETHROWER_MAX_CLIP;
	p->iSlot = 5;
	p->iPosition = 5;
	p->iId = m_iId = WEAPON_FLAMETHROWER;
	p->iFlags = 0;
	p->iWeight = FLAMETHROWER_WEIGHT;

	return true;
}

bool CWeaponFlamethrower::Deploy( void )
{
	return DefaultDeploy( GetV_Model( "models/v_flamethrower.mdl" ), GetP_Model( "models/p_flamethrower.mdl" ), FLAMETHROWER_DRAW, "egon" );
}

void CWeaponFlamethrower::WeaponIdle( void )
{
	ResetEmptySound();

	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	int iAnim;
	float flRand = RANDOM_FLOAT( 0, 1 );

	if ( flRand <= 0.5 || m_flNextPrimaryAttack == m_flTimeWeaponIdle )
	{
		iAnim = FLAMETHROWER_IDLE;
		m_flTimeWeaponIdle = RANDOM_FLOAT( 10, 15 ) + gpGlobals->time;
	}
	else
	{
		iAnim = FLAMETHROWER_FIDGET;
		m_flTimeWeaponIdle = gpGlobals->time + 3.0;
	}

	SendWeaponAnim( iAnim, 0 );
}

void CWeaponFlamethrower::PrimaryAttack( void )
{
	if ( m_hPlayer->pev->waterlevel == 3 || m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] < FLAMETHROWER_AMMO_PER_SHOT )
	{
		PlayEmptySound();
		return;
	}

	m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] -= FLAMETHROWER_AMMO_PER_SHOT;

	UTIL_MakeVectors( m_hPlayer->pev->v_angle + m_hPlayer->pev->punchangle );

	m_hPlayer->GetGunPosition();

	m_hPlayer->m_iWeaponVolume = FLAMETHROWER_VOLUME;

	m_flNextPrimaryAttack = m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.15;
}

void CWeaponFlamethrower::GetWeaponData( weapon_data_t *data )
{
	data->iuser1 = m_iPrimaryAmmoType;
	data->iuser2 = m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ];
}

void CWeaponFlamethrower::SetWeaponData( weapon_data_t *data )
{
	m_iPrimaryAmmoType = data->iuser1;
	m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] = data->iuser2;
}
