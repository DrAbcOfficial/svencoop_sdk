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

LINK_ENTITY_TO_CLASS( weapon_sniperrifle, CSniperRifle );

void CSniperRifle::Spawn( void )
{
	m_iId = WEAPON_SNIPERRIFLE;
	pev->classname = MAKE_STRING( "weapon_sniperrifle" );

	Precache();
	SET_MODEL( ENT( pev ), GetW_Model( "models/w_m40a1.mdl" ) );

	m_iDefaultAmmo = SNIPERRIFLE_DEFAULT_GIVE;
	m_iSecondaryAmmoType = 0;
	m_flCustomDmg = pev->dmg;

	FallInit();// get ready to fall down.
}

void CSniperRifle::Precache( void )
{
	PrecacheCustomModels();

	PRECACHE_MODEL( "models/v_m40a1.mdl" );
	PRECACHE_MODEL( "models/w_m40a1.mdl" );
	PRECACHE_MODEL( "models/p_m40a1.mdl" );
	PRECACHE_MODEL( "models/w_m40a1clip.mdl" );

	m_iShell = PRECACHE_MODEL( "models/shell.mdl" );// brass shell

	PRECACHE_SOUND( "weapons/sniper_bolt1.wav" );
	PRECACHE_SOUND( "weapons/sniper_fire.wav" );
	PRECACHE_SOUND( "weapons/sniper_reload_first_seq.wav" );
	PRECACHE_SOUND( "weapons/sniper_zoom.wav" );

	m_usSniper = PRECACHE_EVENT( 1, "events/sniperrifle.sc" );
}

bool CSniperRifle::GetItemInfo( ItemInfo *p )
{
	p->pszName = STRING( pev->classname );
	p->pszAmmo1 = "m40a1";
	p->iMaxAmmo1 = _762_MAX_CARRY;
	p->pszAmmo1Entity = "ammo_762";
	p->iAmmo1Drop = SNIPERRIFLE_AMMO1_DROP;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo2Entity = NULL;
	p->iAmmo2Drop = AMMO_DROP_NONE;
	p->iMaxClip = SNIPERRIFLE_MAX_CLIP;
	p->iSlot = 5;
	p->iPosition = 0;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_SNIPERRIFLE;
	p->iWeight = SNIPERRIFLE_WEIGHT;

	return true;
}

bool CSniperRifle::AddToPlayer( CBasePlayer *pPlayer )
{
	m_iId = WEAPON_SNIPERRIFLE;

	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		AddToPlayerMessage();
		return true;
	}

	return false;
}

void CSniperRifle::PrimaryAttack( void )
{
	m_iId = WEAPON_SNIPERRIFLE;

	if ( m_iReloadEndAnim )
	{
		WeaponIdle();
		return;
	}

	// don't fire underwater
	if ( m_hPlayer->pev->waterlevel == 3 || m_iClip <= 0 )
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15;
		return;
	}

	if ( m_iClip == 1 )
		PLAYBACK_EVENT_FULL( FEV_NOTHOST, NULL, m_usSniper, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 1, 0, 0, m_fInZoom );
	else
		PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), m_usSniper, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, m_fInZoom );

	m_hPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_hPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip--;

	// player "shoot" animation
	m_hPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc = m_hPlayer->GetGunPosition();
	Vector vecAiming = m_hPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
	Vector vecSpread;

	if ( m_fInZoom )
		vecSpread = BulletAccuracy( VECTOR_CONE_1DEGREES, g_vecZero, g_vecZero );
	else
		vecSpread = BulletAccuracy( VECTOR_CONE_9DEGREES, VECTOR_CONE_6DEGREES, VECTOR_CONE_3DEGREES );

	m_hPlayer->FireBullets( 1, vecSrc, vecAiming, vecSpread, 8192, BULLET_PLAYER_SNIPER, 2, (int)m_flCustomDmg, NULL, FBDM_DRAW );

	if ( !m_iClip && m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] <= 0 )
	{
		// HEV suit - indicate out of ammo condition
		m_hPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );
	}

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.8;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );
}

void CSniperRifle::SecondaryAttack( void )
{
	if ( !m_fInZoom )
	{
		// no scope while looking through a trigger_camera
		if ( !( m_hPlayer->m_afPhysicsFlags & PFLAG_CAMERA ) )
		{
			strncpy( m_hPlayer->m_szAnimExtention, "sniperscope", sizeof( m_hPlayer->m_szAnimExtention ) );
			m_fInZoom = true;
		}
	}
	else
	{
		strncpy( m_hPlayer->m_szAnimExtention, "sniper", sizeof( m_hPlayer->m_szAnimExtention ) );
		m_fInZoom = false;
	}

	PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), g_evWeaponCustom, 0.0, (float *)&g_vecZero, (float *)&g_vecZero,
		WEAPONCUSTOM_SND_SNIPER_ZOOM, 1.0, WEAPONCUSTOM_SOUND, CHAN_ITEM, 0, 0 );

	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.6;
}

int CSniperRifle::SecondaryAmmoIndex( void )
{
	return m_iSecondaryAmmoType;
}

bool CSniperRifle::Deploy( void )
{
	return DefaultDeploy( GetV_Model( "models/v_m40a1.mdl" ), GetP_Model( "models/p_m40a1.mdl" ), SNIPERRIFLE_DRAW, "sniper" );
}

void CSniperRifle::Holster( int skiplocal /* = 0 */ )
{
	m_fInReload = false;// cancel any reload in progress.

	if ( m_fInZoom )
		SecondaryAttack();

	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	m_hPlayer->pev->viewmodel = 0;
}

void CSniperRifle::Reload( void )
{
	if ( m_flNextPrimaryAttack > UTIL_WeaponTimeBase() )
		return;

	if ( m_fInReload && m_iReloadEndAnim )
		return;

	if ( m_iClip )
		SpecialReload( SNIPERRIFLE_MAX_CLIP, SNIPERRIFLE_RELOAD3, 2.6, false );
	else
		SpecialReload( SNIPERRIFLE_MAX_CLIP, SNIPERRIFLE_RELOAD1, 2.5, true );
}

// An empty rifle reloads in two stages, WeaponIdle plays the second one
bool CSniperRifle::SpecialReload( int iClipSize, int iAnim, float fDelay, int fTwoStage )
{
	if ( m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] <= 0 )
		return false;

	int j = V_min( iClipSize - m_iClip, m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] );

	if ( j == 0 )
		return false;

	if ( m_fInZoom )
		SecondaryAttack();

	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + fDelay;

	SendWeaponAnim( iAnim );

	m_fInReload = true;

	if ( fTwoStage )
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.2;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 3.5;
		m_iReloadEndAnim = SNIPERRIFLE_RELOAD2;
	}
	else
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.0;
		m_iReloadEndAnim = 0;
	}

	return true;
}

void CSniperRifle::ItemPostFrame( void )
{
	if ( m_fInReload && m_iReloadEndAnim )
		WeaponIdle();

	CBasePlayerWeapon::ItemPostFrame();
}

void CSniperRifle::WeaponIdle( void )
{
	ResetEmptySound();

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	if ( m_iReloadEndAnim )
	{
		// second half of the reload
		SendWeaponAnim( m_iReloadEndAnim );

		m_iReloadEndAnim = 0;
		m_fInReload = true;

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5;
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = m_flTimeWeaponIdle - 0.2;
		return;
	}

	if ( UTIL_WeaponTimeBase() > m_flNextSecondaryAttack )
	{
		// keep the zoom in sync with the scope state
		if ( m_fInZoom )
		{
			if ( !( m_hPlayer->m_afPhysicsFlags & PFLAG_CAMERA ) )
			{
				if ( FRunfuncs() )
					SetFOV( SNIPERRIFLE_ZOOM_FOV );

				strncpy( m_hPlayer->m_szAnimExtention, "sniperscope", sizeof( m_hPlayer->m_szAnimExtention ) );
			}
		}
		else
		{
			if ( FRunfuncs() )
				SetFOV( 0 );

			strncpy( m_hPlayer->m_szAnimExtention, "sniper", sizeof( m_hPlayer->m_szAnimExtention ) );
		}
	}

	SendWeaponAnim( SNIPERRIFLE_SLOWIDLE );

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );
}

void CSniperRifle::GetWeaponData( weapon_data_t *data )
{
	data->iuser1 = m_iPrimaryAmmoType;
	data->iuser2 = m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ];
	data->fuser1 = m_iReloadEndAnim;
}

void CSniperRifle::SetWeaponData( weapon_data_t *data )
{
	m_iPrimaryAmmoType = data->iuser1;
	m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] = data->iuser2;
	m_iReloadEndAnim = (int)data->fuser1;
}

class CSniperRifleAmmoClip : public CPlayerAmmo
{
public:
	CSniperRifleAmmoClip() : CPlayerAmmo( AMMO_762BOX_GIVE, "m40a1", _762_MAX_CARRY ) { }

	const char *GetDefaultModel( void ) { return "models/w_m40a1clip.mdl"; }
};

LINK_ENTITY_TO_CLASS( ammo_762, CSniperRifleAmmoClip );
