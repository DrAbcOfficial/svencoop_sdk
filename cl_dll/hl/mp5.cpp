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
#include "soundent.h"
#include "weaponinfo.h"
#include "event_flags.h"

LINK_ENTITY_TO_CLASS( weapon_mp5, CMP5 );
LINK_ENTITY_TO_CLASS( weapon_9mmar, CMP5 );
LINK_ENTITY_TO_CLASS( weapon_9mmAR, CMP5 );

void CMP5::Spawn( void )
{
	Precache();

	if ( CClassicModeWrapper::isEnabled() )
		SET_MODEL( ENT( pev ), GetW_Model( "models/hlclassic/w_9mmAR.mdl" ) );
	else
		SET_MODEL( ENT( pev ), GetW_Model( "models/w_9mmAR.mdl" ) );

	pev->classname = MAKE_STRING( "weapon_9mmAR" ); // hack to allow for old names

	if ( CClassicModeWrapper::isEnabled() )
		m_iDefaultAmmo = MP5_CLASSIC_DEFAULT_GIVE;
	else
		m_iDefaultAmmo = MP5_DEFAULT_GIVE;

	m_iSecondaryAmmoType = 0;
	m_iId = WEAPON_MP5;
	m_flCustomDmg = pev->dmg;

	FallInit();// get ready to fall down.
}

void CMP5::Precache( void )
{
	PrecacheCustomModels();

	bool bClassic = CClassicModeWrapper::isEnabled();

	PRECACHE_MODEL( bClassic ? "models/hlclassic/v_9mmAR.mdl" : "models/v_9mmAR.mdl" );
	PRECACHE_MODEL( bClassic ? "models/hlclassic/w_9mmAR.mdl" : "models/w_9mmAR.mdl" );
	PRECACHE_MODEL( bClassic ? "models/hlclassic/p_9mmAR.mdl" : "models/p_9mmAR.mdl" );
	PRECACHE_MODEL( bClassic ? "models/hlclassic/grenade.mdl" : "models/grenade.mdl" );// grenade
	PRECACHE_MODEL( bClassic ? "models/hlclassic/w_9mmarclip.mdl" : "models/w_mp5_clip.mdl" );

	m_iShell = PRECACHE_MODEL( bClassic ? "models/hlclassic/shell.mdl" : "models/shell.mdl" );// brass shell

	PRECACHE_SOUND( "hlclassic/items/9mmclip1.wav" );
	PRECACHE_SOUND( "hlclassic/items/clipinsert1.wav" );
	PRECACHE_SOUND( "hlclassic/items/cliprelease1.wav" );
	PRECACHE_SOUND( "hlclassic/weapons/hks1.wav" );
	PRECACHE_SOUND( "hlclassic/weapons/hks2.wav" );
	PRECACHE_SOUND( "hlclassic/weapons/hks3.wav" );
	PRECACHE_SOUND( "hlclassic/weapons/glauncher.wav" );
	PRECACHE_SOUND( "hlclassic/weapons/glauncher2.wav" );
	PRECACHE_SOUND( "hlclassic/weapons/357_cock1.wav" );
	PRECACHE_SOUND( "items/9mmclip1.wav" );
	PRECACHE_SOUND( "items/clipinsert1.wav" );
	PRECACHE_SOUND( "items/cliprelease1.wav" );
	PRECACHE_SOUND( "weapons/hks1.wav" );// H to the K
	PRECACHE_SOUND( "weapons/hks2.wav" );// H to the K
	PRECACHE_SOUND( "weapons/hks3.wav" );// H to the K
	PRECACHE_SOUND( "weapons/glauncher.wav" );
	PRECACHE_SOUND( "weapons/glauncher2.wav" );
	PRECACHE_SOUND( "weapons/357_cock1.wav" );

	m_usMP5 = PRECACHE_EVENT( 1, "events/mp5.sc" );

	UTIL_PrecacheOther( "ammo_9mmbox" );
}

// Classic mode brings back the 50 round clip and the grenade launcher
bool CMP5::GetItemInfo( ItemInfo *p )
{
	p->pszName = STRING( pev->classname );
	p->pszAmmo1 = "9mm";
	p->iMaxAmmo1 = _9MM_MAX_CARRY;

	bool bClassic = CClassicModeWrapper::isEnabled();

	int iMaxClip;

	if ( bClassic )
		iMaxClip = MP5_CLASSIC_MAX_CLIP;
	else
		iMaxClip = MP5_MAX_CLIP;

	p->iAmmo1Drop = iMaxClip;
	p->pszAmmo1Entity = "ammo_9mmAR";

	if ( bClassic )
		p->pszAmmo2 = "ARgrenades";
	else
		p->pszAmmo2 = NULL;

	if ( bClassic )
		p->iMaxAmmo2 = M203_GRENADE_MAX_CARRY;
	else
		p->iMaxAmmo2 = -1;

	if ( bClassic )
		p->pszAmmo2Entity = "ammo_ARgrenades";
	else
		p->pszAmmo2Entity = NULL;

	if ( bClassic )
		p->iAmmo2Drop = MP5_AMMO2_DROP;
	else
		p->iAmmo2Drop = AMMO_DROP_NONE;

	p->iMaxClip = iMaxClip;
	p->iSlot = 2;
	p->iPosition = 0;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_MP5;
	p->iWeight = MP5_WEIGHT;

	return true;
}

bool CMP5::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		AddToPlayerMessage();
		return true;
	}

	return false;
}

void CMP5::PrimaryAttack( void )
{
	if ( CClassicModeWrapper::isEnabled() )
	{
		ClassicPrimaryAttack();
		return;
	}

	// don't fire underwater
	if ( m_hPlayer->pev->waterlevel == 3 || m_iClip <= 0 )
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15;
		return;
	}

	m_hPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_hPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	// player "shoot" animation
	m_hPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc = m_hPlayer->GetGunPosition();
	Vector vecAiming = m_hPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( m_fInZoom )
	{
		Vector vecSpread = BulletAccuracy( VECTOR_CONE_8DEGREES, VECTOR_CONE_4DEGREES, VECTOR_CONE_3DEGREES );

		m_hPlayer->FireBullets( 1, vecSrc, vecAiming, vecSpread, 8192, BULLET_PLAYER_MP5, 2, (int)m_flCustomDmg, NULL, FBDM_DRAW );

		PLAYBACK_EVENT_FULL( FEV_NOTHOST, NULL, m_usMP5, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 1, 0 );
	}
	else
	{
		Vector vecSpread = BulletAccuracy( VECTOR_CONE_8DEGREES, VECTOR_CONE_6DEGREES, VECTOR_CONE_5DEGREES );

		m_hPlayer->FireBullets( 1, vecSrc, vecAiming, vecSpread, 8192, BULLET_PLAYER_MP5, 2, (int)m_flCustomDmg, NULL, FBDM_DRAW );

		PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), m_usMP5, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );
	}

	if ( m_iClip-- == 1 && m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] <= 0 )
	{
		// HEV suit - indicate out of ammo condition
		m_hPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );
	}

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + ( UTIL_SharedRandomFloat( m_hPlayer->random_seed, 0.02, 0.05 ) + 0.05f );
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );
}

void CMP5::ClassicPrimaryAttack( void )
{
	// don't fire underwater
	if ( m_hPlayer->pev->waterlevel == 3 || m_iClip <= 0 )
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15;
		return;
	}

	m_hPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_hPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	// player "shoot" animation
	m_hPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc = m_hPlayer->GetGunPosition();
	Vector vecAiming = m_hPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
	Vector vecSpread = BulletAccuracy( VECTOR_CONE_8DEGREES, VECTOR_CONE_6DEGREES, VECTOR_CONE_5DEGREES );

	m_hPlayer->FireBullets( 1, vecSrc, vecAiming, vecSpread, 8192, BULLET_PLAYER_MP5, 2, (int)m_flCustomDmg, NULL, FBDM_DRAW );

	PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), m_usMP5, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );

	if ( m_iClip-- == 1 && m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] <= 0 )
	{
		// HEV suit - indicate out of ammo condition
		m_hPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );
	}

	if ( UTIL_WeaponTimeBase() > m_flNextPrimaryAttack )
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.1;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );
}

void CMP5::ClassicSecondaryAttack( void )
{
	// don't fire underwater
	if ( m_hPlayer->pev->waterlevel == 3 )
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15;
		return;
	}

	if ( m_iSecondaryAmmoCarried <= 0 )
	{
		PlayEmptySound();
		return;
	}

	m_hPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_hPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	m_hPlayer->m_iExtraSoundTypes = bits_SOUND_DANGER;
	m_hPlayer->m_flStopExtraSoundTime = UTIL_WeaponTimeBase() + 0.2;

	if ( FRunfuncs() )
		m_iSecondaryAmmoCarried--;

	SendWeaponAnim( MP5_LAUNCH );

	// player "shoot" animation
	m_hPlayer->SetAnimation( PLAYER_ATTACK1 );

	if ( UTIL_SharedRandomLong( m_hPlayer->random_seed, 0, 1 ) )
	{
		PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), g_evWeaponCustom, 0.0, (float *)&g_vecZero, (float *)&g_vecZero,
			WEAPONCUSTOM_SND_GLAUNCHER, 0.8, WEAPONCUSTOM_SOUND, CHAN_WEAPON, 0, 0 );
	}
	else
	{
		PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), g_evWeaponCustom, 0.0, (float *)&g_vecZero, (float *)&g_vecZero,
			WEAPONCUSTOM_SND_GLAUNCHER2, 0.8, WEAPONCUSTOM_SOUND, CHAN_WEAPON, 0, 0 );
	}

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 5;// idle pretty soon after shooting.
}

// Zooms in, or fires a grenade in classic mode
void CMP5::SecondaryAttack( void )
{
	if ( CClassicModeWrapper::isEnabled() )
	{
		ClassicSecondaryAttack();
		return;
	}

	if ( m_fInZoom )
	{
		strncpy( m_hPlayer->m_szAnimExtention, "mp5", sizeof( m_hPlayer->m_szAnimExtention ) );
		m_fInZoom = false;
	}
	else if ( !( m_hPlayer->m_afPhysicsFlags & PFLAG_CAMERA ) )
	{
		m_fInZoom = true;
	}

	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.6;
}

int CMP5::SecondaryAmmoIndex( void )
{
	return m_iSecondaryAmmoType;
}

bool CMP5::Deploy( void )
{
	if ( CClassicModeWrapper::isEnabled() )
		return DefaultDeploy( GetV_Model( "models/hlclassic/v_9mmAR.mdl" ), GetP_Model( "models/hlclassic/p_9mmAR.mdl" ), MP5_DEPLOY, "mp5" );

	return DefaultDeploy( GetV_Model( "models/v_9mmAR.mdl" ), GetP_Model( "models/p_9mmAR.mdl" ), MP5_DEPLOY, "mp5" );
}

void CMP5::Holster( int skiplocal /* = 0 */ )
{
	m_fInReload = false;// cancel any reload in progress.

	if ( m_fInZoom )
		SecondaryAttack();

	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	m_hPlayer->pev->viewmodel = 0;
}

void CMP5::Reload( void )
{
	int iMaxClip;

	if ( CClassicModeWrapper::isEnabled() )
		iMaxClip = MP5_CLASSIC_MAX_CLIP;
	else
		iMaxClip = MP5_MAX_CLIP;

	if ( DefaultReload( iMaxClip, MP5_RELOAD, 1.6 ) && m_fInZoom )
		SecondaryAttack();
}

void CMP5::WeaponIdle( void )
{
	ResetEmptySound();

	m_hPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( UTIL_WeaponTimeBase() > m_flNextSecondaryAttack )
	{
		// keep the zoom in sync
		if ( m_fInZoom )
		{
			if ( !( m_hPlayer->m_afPhysicsFlags & PFLAG_CAMERA ) && FRunfuncs() )
				SetFOV( MP5_ZOOM_FOV );
		}
		else if ( FRunfuncs() )
		{
			SetFOV( 0 );
		}
	}

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	SendWeaponAnim( UTIL_SharedRandomLong( m_hPlayer->random_seed, 0, 1 ) ? MP5_IDLE1 : MP5_LONGIDLE );

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );// how long till we do this again.
}

void CMP5::GetWeaponData( weapon_data_t *data )
{
	data->iuser1 = m_iPrimaryAmmoType;
	data->iuser2 = m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ];
	data->iuser3 = m_hPlayer->m_rgAmmo[ m_iSecondaryAmmoType ];
}

void CMP5::SetWeaponData( weapon_data_t *data )
{
	m_iPrimaryAmmoType = data->iuser1;
	m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] = data->iuser2;
	m_iSecondaryAmmoCarried = data->iuser3;
}

class CMP5AmmoClip : public CPlayerAmmo
{
public:
	CMP5AmmoClip() : CPlayerAmmo( AMMO_MP5CLIP_GIVE, "9mm", _9MM_MAX_CARRY )
	{
		if ( CClassicModeWrapper::isEnabled() )
			m_iAmmoGive = MP5_CLASSIC_MAX_CLIP;
	}

	const char *GetDefaultModel( void ) { return CClassicModeWrapper::isEnabled() ? "models/hlclassic/w_9mmarclip.mdl" : "models/w_mp5_clip.mdl"; }
	const char *GetPickupSound( void ) { return CClassicModeWrapper::isEnabled() ? "hlclassic/items/9mmclip1.wav" : "items/9mmclip1.wav"; }
};

LINK_ENTITY_TO_CLASS( ammo_mp5clip, CMP5AmmoClip );
LINK_ENTITY_TO_CLASS( ammo_9mmAR, CMP5AmmoClip );

class CMP5Chainammo : public CPlayerAmmo
{
public:
	CMP5Chainammo() : CPlayerAmmo( AMMO_CHAINBOX_GIVE, "9mm", _9MM_MAX_CARRY ) { }

	const char *GetDefaultModel( void ) { return CClassicModeWrapper::isEnabled() ? "models/hlclassic/w_chainammo.mdl" : "models/w_chainammo.mdl"; }
	const char *GetPickupSound( void ) { return CClassicModeWrapper::isEnabled() ? "hlclassic/items/9mmclip1.wav" : "items/9mmclip1.wav"; }
};

LINK_ENTITY_TO_CLASS( ammo_9mmbox, CMP5Chainammo );

class CMP5AmmoGrenade : public CPlayerAmmo
{
public:
	CMP5AmmoGrenade() : CPlayerAmmo( AMMO_M203BOX_GIVE, "ARgrenades", M203_GRENADE_MAX_CARRY ) { }

	const char *GetDefaultModel( void ) { return CClassicModeWrapper::isEnabled() ? "models/hlclassic/w_argrenade.mdl" : "models/w_ARgrenade.mdl"; }
	const char *GetPickupSound( void ) { return CClassicModeWrapper::isEnabled() ? "hlclassic/items/9mmclip1.wav" : "items/9mmclip1.wav"; }
};

LINK_ENTITY_TO_CLASS( ammo_mp5grenades, CMP5AmmoGrenade );
LINK_ENTITY_TO_CLASS( ammo_ARgrenades, CMP5AmmoGrenade );
