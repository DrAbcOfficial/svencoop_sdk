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

LINK_ENTITY_TO_CLASS( weapon_python, CPython );
LINK_ENTITY_TO_CLASS( weapon_357, CPython );

void CPython::Spawn( void )
{
	Precache();

	if ( CClassicModeWrapper::isEnabled() )
		SET_MODEL( ENT( pev ), GetW_Model( "models/hlclassic/w_357.mdl" ) );
	else
		SET_MODEL( ENT( pev ), GetW_Model( "models/w_357.mdl" ) );

	pev->classname = MAKE_STRING( "weapon_357" ); // hack to allow for old names

	m_iDefaultAmmo = PYTHON_DEFAULT_GIVE;
	m_iId = WEAPON_PYTHON;
	m_flCustomDmg = pev->dmg;

	FallInit();// get ready to fall down.
}

void CPython::Precache( void )
{
	PrecacheCustomModels();

	bool bClassic = CClassicModeWrapper::isEnabled();

	PRECACHE_MODEL( bClassic ? "models/hlclassic/v_357.mdl" : "models/v_357.mdl" );
	PRECACHE_MODEL( bClassic ? "models/hlclassic/w_357.mdl" : "models/w_357.mdl" );
	PRECACHE_MODEL( bClassic ? "models/hlclassic/p_357.mdl" : "models/p_357.mdl" );

	PRECACHE_SOUND( "items/9mmclip1.wav" );
	PRECACHE_SOUND( "weapons/357_reload1.wav" );
	PRECACHE_SOUND( "weapons/357_cock1.wav" );
	PRECACHE_SOUND( "weapons/357_shot1.wav" );
	PRECACHE_SOUND( "weapons/357_chamberin.wav" );
	PRECACHE_SOUND( "weapons/357_chamberout.wav" );
	PRECACHE_SOUND( "weapons/357_idle_fidgetpull.wav" );
	PRECACHE_SOUND( "weapons/357_idle_fidgetrelease.wav" );
	PRECACHE_SOUND( "weapons/357_idle_squeeze.wav" );
	PRECACHE_SOUND( "weapons/357_idle_unsqueeze.wav" );
	PRECACHE_SOUND( "weapons/357_quickloader.wav" );
	PRECACHE_SOUND( "weapons/357_shellsin.wav" );
	PRECACHE_SOUND( "weapons/357_shellsout.wav" );

	m_usFirePython = PRECACHE_EVENT( 1, "events/python.sc" );
	g_EvCheckClient = PRECACHE_EVENT( 1, "events/clcheck.sc" );
}

bool CPython::GetItemInfo( ItemInfo *p )
{
	p->pszName = STRING( pev->classname );
	p->pszAmmo1 = "357";
	p->iMaxAmmo1 = _357_MAX_CARRY;
	p->pszAmmo1Entity = "ammo_357";
	p->iAmmo1Drop = PYTHON_AMMO1_DROP;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo2Entity = NULL;
	p->iAmmo2Drop = AMMO_DROP_NONE;
	p->iMaxClip = PYTHON_MAX_CLIP;
	p->iFlags = 0;
	p->iSlot = 1;
	p->iPosition = 1;
	p->iId = m_iId = WEAPON_PYTHON;
	p->iWeight = PYTHON_WEIGHT;

	return true;
}

bool CPython::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		AddToPlayerMessage();
		return true;
	}

	return false;
}

void CPython::PrimaryAttack( void )
{
	// don't fire underwater
	if ( m_hPlayer->pev->waterlevel == 3 || m_iClip <= 0 )
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.75;
		return;
	}

	m_iClip--;

	m_hPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_hPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	m_hPlayer->pev->effects |= EF_MUZZLEFLASH;

	// player "shoot" animation
	m_hPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc = m_hPlayer->GetGunPosition();

	PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), m_usFirePython, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, m_iClip == 0, 0 );

	Vector vecAiming = m_hPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );
	Vector vecSpread = BulletAccuracy( VECTOR_CONE_4DEGREES, VECTOR_CONE_3DEGREES, VECTOR_CONE_2DEGREES );

	m_hPlayer->FireBullets( 1, vecSrc, vecAiming, vecSpread, 8192, BULLET_PLAYER_357, 0, (int)m_flCustomDmg, NULL, FBDM_DRAW );

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.75;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );

	if ( !m_iClip && m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] <= 0 )
	{
		// HEV suit - indicate out of ammo condition
		m_hPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );
	}
}

void CPython::SecondaryAttack( void )
{
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.6;
}

bool CPython::Deploy( void )
{
	if ( CClassicModeWrapper::isEnabled() )
		return DefaultDeploy( GetV_Model( "models/hlclassic/v_357.mdl" ), GetP_Model( "models/hlclassic/p_357.mdl" ), PYTHON_DRAW, "python" );

	return DefaultDeploy( GetV_Model( "models/v_357.mdl" ), GetP_Model( "models/p_357.mdl" ), PYTHON_DRAW, "python" );
}

void CPython::Holster( int skiplocal /* = 0 */ )
{
	m_fInReload = false;// cancel any reload in progress.

	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	m_hPlayer->pev->viewmodel = 0;
}

void CPython::Reload( void )
{
	if ( DefaultReload( PYTHON_MAX_CLIP, PYTHON_RELOAD, 2.5 ) )
	{
		PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), g_evWeaponCustom, 1.5, (float *)&g_vecZero, (float *)&g_vecZero,
			WEAPONCUSTOM_SND_357_RELOAD1, RANDOM_FLOAT( 0.8, 0.9 ), WEAPONCUSTOM_SOUND, CHAN_WEAPON, 0, 0 );
	}
}

void CPython::WeaponIdle( void )
{
	ResetEmptySound();

	m_hPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	int iAnim;
	float flRand = UTIL_SharedRandomFloat( m_hPlayer->random_seed, 0, 1 );

	if ( flRand <= 0.4 )
	{
		iAnim = PYTHON_IDLE1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 71.0 / 30.0;
		m_flTimeWeaponIdle = flRand * 10 + m_flTimeWeaponIdle;
	}
	else if ( flRand <= 0.75 )
	{
		iAnim = PYTHON_IDLE2;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 71.0 / 30.0;
		m_flTimeWeaponIdle = ( flRand - 0.4 ) * 20 + m_flTimeWeaponIdle;
	}
	else if ( flRand <= 0.9 )
	{
		iAnim = PYTHON_IDLE3;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 71.0 / 30.0;
		m_flTimeWeaponIdle = ( flRand - 0.75 ) * 60 + m_flTimeWeaponIdle;
	}
	else
	{
		iAnim = PYTHON_FIDGET;
		m_flTimeWeaponIdle = (float)( UTIL_WeaponTimeBase() + 170.0 / 30.0 ) + ( flRand - 0.9 ) * 70;
	}

	SendWeaponAnim( iAnim );
}

void CPython::GetWeaponData( weapon_data_t *data )
{
	data->iuser1 = m_iPrimaryAmmoType;
	data->iuser2 = m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ];
}

void CPython::SetWeaponData( weapon_data_t *data )
{
	m_iPrimaryAmmoType = data->iuser1;
	m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] = data->iuser2;
}

class CPythonAmmo : public CPlayerAmmo
{
public:
	CPythonAmmo() : CPlayerAmmo( AMMO_357BOX_GIVE, "357", _357_MAX_CARRY ) { }

	const char *GetDefaultModel( void ) { return CClassicModeWrapper::isEnabled() ? "models/hlclassic/w_357ammobox.mdl" : "models/w_357ammobox.mdl"; }
	const char *GetPickupSound( void ) { return CClassicModeWrapper::isEnabled() ? "hlclassic/items/9mmclip1.wav" : "items/9mmclip1.wav"; }
};

LINK_ENTITY_TO_CLASS( ammo_357, CPythonAmmo );
