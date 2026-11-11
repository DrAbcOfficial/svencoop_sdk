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

#include "playerammo.h"
#include "game.h"

#undef PRECACHE_MODEL
#undef PRECACHE_SOUND
#define PRECACHE_MODEL( model ) UTIL_PrecacheModel( this, model )
#define PRECACHE_SOUND( sound ) PrecacheSound_Intercept( this, sound )

LINK_ENTITY_TO_CLASS( weapon_uzi, CUzi );
LINK_ENTITY_TO_CLASS( weapon_uziakimbo, CUzi );

void CUzi::Spawn( void )
{
	Precache();

	bool akimbo = !strcmp( STRING( pev->classname ), "weapon_uziakimbo" );

	SetAkimbo( akimbo, true );

	if ( akimbo )
		m_iClip2 = UZI_MAX_CLIP;

	m_iDefaultAmmo = UZI_DEFAULT_GIVE;
	m_iId = WEAPON_UZI;
	m_flCustomDmg = pev->dmg;

	CBasePhysicsObject::Spawn();
}

void CUzi::Precache( void )
{
	PrecacheCustomModels();

	PRECACHE_MODEL( "models/v_uzi.mdl" );
	PRECACHE_MODEL( "models/w_uzi.mdl" );
	PRECACHE_MODEL( "models/p_uzi.mdl" );
	PRECACHE_MODEL( "models/p_uzi_gold.mdl" );
	PRECACHE_MODEL( "models/w_2uzis.mdl" );
	PRECACHE_MODEL( "models/p_2uzis.mdl" );
	PRECACHE_MODEL( "models/p_2uzis_gold.mdl" );
	PRECACHE_MODEL( "models/w_uzi_clip.mdl" );

	m_iShell = PRECACHE_MODEL( "models/shell.mdl" );// brass shell

	PRECACHE_SOUND( "weapons/uzi/deploy.wav" );
	PRECACHE_SOUND( "weapons/uzi/deploy1.wav" );
	PRECACHE_SOUND( "weapons/uzi/reload1.wav" );
	PRECACHE_SOUND( "weapons/uzi/reload2.wav" );
	PRECACHE_SOUND( "weapons/uzi/reload3.wav" );
	PRECACHE_SOUND( "weapons/uzi/shoot1.wav" );
	PRECACHE_SOUND( "weapons/uzi/shoot2.wav" );
	PRECACHE_SOUND( "weapons/uzi/shoot3.wav" );
	PRECACHE_SOUND( "weapons/uzi/akimbo_deploy.wav" );
	PRECACHE_SOUND( "weapons/uzi/fire_both2.wav" );
	PRECACHE_SOUND( "weapons/uzi/fire_both1.wav" );
	PRECACHE_SOUND( "weapons/uzi/akimbo_pull1.wav" );
	PRECACHE_SOUND( "weapons/uzi/akimbo_pull2.wav" );
	PRECACHE_SOUND( "weapons/uzi/reload_left1.wav" );
	PRECACHE_SOUND( "weapons/uzi/reload_right1.wav" );
	PRECACHE_SOUND( "weapons/uzi/reload_left2.wav" );
	PRECACHE_SOUND( "weapons/uzi/reload_right2.wav" );

	m_usUzi = PRECACHE_EVENT( 1, "events/uzi.sc" );
	m_usUziAkimbo = PRECACHE_EVENT( 1, "events/uziakimbo.sc" );
}

bool CUzi::GetItemInfo( ItemInfo *p )
{
	p->pszName = STRING( pev->classname );
	p->pszAmmo1 = "9mm";
	p->iMaxAmmo1 = _9MM_MAX_CARRY;
	p->pszAmmo1Entity = "ammo_uziclip";
	p->iAmmo1Drop = UZI_AMMO1_DROP;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo2Entity = NULL;
	p->iAmmo2Drop = AMMO_DROP_NONE;
	p->iMaxClip = UZI_MAX_CLIP;
	p->iSlot = 1;
	p->iPosition = 3;
	p->iFlags = ITEM_FLAG_DUALWIELD;
	p->iId = m_iId = WEAPON_UZI;
	p->iWeight = m_fIsAkimbo ? UZI_AKIMBO_WEIGHT : UZI_WEIGHT;

	return true;
}

bool CUzi::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		AddToPlayerMessage();

		LoadSprites( pPlayer, GetHUDSpriteName() );

		return true;
	}

	return false;
}

// Picking up a second uzi turns the carried one into an akimbo pair
void CUzi::SetRightClip( int iClip )
{
	m_iClip = V_max( iClip, 0 );
}

void CUzi::SetLeftClip( int iClip )
{
	m_iClip2 = V_max( iClip, 0 );
}

bool CUzi::AddDuplicate( CBasePlayerItem *pOriginal )
{
	if ( m_iId != WEAPON_UZI )
		return CBasePlayerWeapon::AddDuplicate( pOriginal );

	CUzi *pUzi = (CUzi *)pOriginal;

	if ( pUzi->m_fAkimboAvailable )
		return CBasePlayerWeapon::AddDuplicate( pOriginal );

	int iClip = UZI_MAX_CLIP;

	if ( m_iClip > 0 || m_iClip2 > 0 )
	{
		int iRounds = V_max( m_iClip, 0 ) + V_max( m_iClip2, 0 );

		iClip = V_min( iRounds, UZI_MAX_CLIP );
		m_iDefaultAmmo += iRounds - iClip;
	}

	pUzi->SetLeftClip( iClip );

	CBasePlayerWeapon::AddAmmoFromItem( pOriginal );

	pUzi->SetAkimbo( true, false );

	if ( pUzi->m_hPlayer )
		pUzi->LoadSprites( pUzi->m_hPlayer, pUzi->GetHUDSpriteName() );

	if ( pUzi == pUzi->m_hPlayer->m_pActiveItem )
		pUzi->Deploy( UZI_AKIMBO_PULL, 1.8 );

	if ( pUzi->m_hPlayer )
	{
		float respawnTime = GetRespawnTime();

		pUzi->m_hPlayer->m_flNextWeaponPickup[WEAPON_UZI] = (float) ( (double) respawnTime + (double) gpGlobals->time );
	}

	return true;
}

void CUzi::PrimaryAttack( void )
{
	if ( !m_fIsAkimbo )
	{
		Fire( UZI_FIRE_SINGLE, m_flNextPrimaryAttack );
		return;
	}

	if ( m_iClip2 <= 0 )
	{
		if ( m_iClip <= 0 )
			WeaponIdle();
		else
			Fire( UZI_FIRE_RIGHT, m_flNextPrimaryAttack );
	}
	else
	{
		if ( m_iClip <= 0 )
			Fire( UZI_FIRE_LEFT, m_flNextPrimaryAttack );
		else
			Fire( UZI_FIRE_BOTH, m_flNextPrimaryAttack );
	}
}

void CUzi::SecondaryAttack( void )
{
	if ( !m_fAkimboAvailable )
		return;

	m_fSwitchingAkimbo = true;

	bool bAkimbo = !m_fIsAkimbo;

	SetAkimbo( bAkimbo, true );
	AkimboDeploy( m_fIsAkimbo );

	m_fSwitchingAkimbo = false;

	CheckAkimboMagazines();
}

void CUzi::Fire( int iMode, float &flNextAttack )
{
	m_iNextReloadAnim = -1;

	if ( m_hPlayer->pev->waterlevel == 3 || iMode == UZI_FIRE_NONE
		|| ( m_fIsAkimbo ? ( !m_iClip2 && !m_iClip ) : !m_iClip ) )
	{
		PlayEmptySound();
		flNextAttack = UTIL_WeaponTimeBase() + 0.15;
		return;
	}

	m_hPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_hPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	int iAnim;

	switch ( iMode )
	{
	case UZI_FIRE_SINGLE:
		iAnim = UZI_SHOOT;
		break;

	case UZI_FIRE_LEFT:
		iAnim = UZI_AKIMBO_SHOOT_LEFT;
		break;

	case UZI_FIRE_BOTH:
		iAnim = UZI_AKIMBO_SHOOT_BOTH;
		break;

	default:
		iAnim = UZI_AKIMBO_SHOOT_RIGHT;
		break;
	}

	PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), m_fIsAkimbo ? m_usUziAkimbo : m_usUzi, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, iAnim, 0, 0, 0 );

	// player "shoot" animation
	m_hPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc = m_hPlayer->GetGunPosition();
	Vector vecAiming = m_hPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
	Vector vecSpread;

	if ( m_fIsAkimbo )
		vecSpread = BulletAccuracy( VECTOR_CONE_10DEGREES, VECTOR_CONE_8DEGREES, VECTOR_CONE_7DEGREES );
	else
		vecSpread = BulletAccuracy( VECTOR_CONE_6DEGREES, VECTOR_CONE_4DEGREES, VECTOR_CONE_3DEGREES );

	int iDamage = (int) sk_plr_uzi.value;

	if ( CanHaveXtra( XTRA_GOLDEN_UZI, m_hPlayer ) )
		iDamage += UZI_GOLDEN_DAMAGE - UZI_DAMAGE;

	if ( m_flCustomDmg > 0 )
		iDamage = iDamage + m_flCustomDmg;

	m_hPlayer->FireBullets( iMode == UZI_FIRE_BOTH ? 2 : 1, vecSrc, vecAiming, vecSpread, 8192, BULLET_PLAYER_CUSTOMDAMAGE, 0, iDamage, NULL, FBDM_DRAW );

	switch ( iMode )
	{
	case UZI_FIRE_BOTH:
		SetRightClip( m_iClip - 1 );
		SetLeftClip( m_iClip2 - 1 );
		break;

	case UZI_FIRE_LEFT:
		SetLeftClip( m_iClip2 - 1 );
		break;

	case UZI_FIRE_SINGLE:
	case UZI_FIRE_RIGHT:
		SetRightClip( m_iClip - 1 );
		break;
	}

	flNextAttack = UTIL_WeaponTimeBase() + 0.07;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );

	if ( ( !m_fIsAkimbo || m_iClip2 == 0 ) && m_iClip == 0 && m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		m_hPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );
}

bool CUzi::CanDeploy( void )
{
	return CBasePlayerWeapon::CanDeploy() || m_fSwitchingAkimbo;
}

bool CUzi::Deploy( void )
{
	return Deploy( m_fIsAkimbo ? UZI_AKIMBO_DRAW : UZI_DRAW, 0 );
}

bool CUzi::Deploy( int iAnim, float flDelay )
{
	m_hPlayer->m_fGrappleActive = false;

	m_iNextReloadAnim = -1;

	if ( !CanDeploy() )
		return false;

	if ( !m_fIsAkimbo )
		EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_WEAPON, "weapons/uzi/deploy.wav", 0.7f, ATTN_NORM, 0, PITCH_NORM );

	m_hPlayer->SetAnimation( PLAYER_DEPLOY );

	if ( flDelay == 0 )
		flDelay = m_fIsAkimbo ? 1.0 : 1.3;

	bool bResult = DefaultDeploy( GetV_Model( "models/v_uzi.mdl" ), GetPModelName(), iAnim, m_fIsAkimbo ? "uzis" : "onehanded" );

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + flDelay;

	return bResult;
}

void CUzi::AkimboDeploy( bool bAkimbo )
{
	if ( bAkimbo )
		Deploy( UZI_AKIMBO_PULL, 1.8 );
	else
		Deploy( UZI_DRAW2, 0 );
}

void CUzi::Reload( void )
{
	if ( !m_fIsAkimbo )
	{
		if ( DefaultReload( UZI_MAX_CLIP, UZI_RELOAD, 2.5 ) )
			CBasePlayerWeapon::Reload();

		return;
	}

	if ( m_iClip < UZI_MAX_CLIP )
		m_iAkimboReloadAnim = ( m_iClip2 >= UZI_MAX_CLIP ) ? UZI_AKIMBO_RELOAD_RIGHT : UZI_AKIMBO_RELOAD_BOTH;
	else if ( m_iClip2 < UZI_MAX_CLIP )
		m_iAkimboReloadAnim = UZI_AKIMBO_RELOAD_LEFT;
	else
		return;

	ReloadAkimbo( UZI_MAX_CLIP, m_iAkimboReloadAnim, 1.8, 0 );

	CBasePlayerWeapon::Reload();
}

// The right uzi uses m_iClip, the left one m_iClip2. A double reload plays one side, then the other
bool CUzi::ReloadAkimbo( int iClipSize, int iAnim, float fDelay, int body )
{
	int iReloadAnim = iAnim;

	if ( iAnim == UZI_AKIMBO_RELOAD_BOTH )
		iReloadAnim = UTIL_SharedRandomLong( m_hPlayer->random_seed, 0, 1 ) ? UZI_AKIMBO_RELOAD_LEFT : UZI_AKIMBO_RELOAD_RIGHT;

	bool bResult = DefaultReload( iClipSize, iReloadAnim, fDelay, body, ( iReloadAnim == UZI_AKIMBO_RELOAD_RIGHT ) ? m_iClip : m_iClip2, m_iPrimaryAmmoType );

	if ( iAnim == UZI_AKIMBO_RELOAD_BOTH )
	{
		m_iReloadSide = iReloadAnim;
		m_iNextReloadAnim = ( iReloadAnim == UZI_AKIMBO_RELOAD_LEFT ) ? UZI_AKIMBO_RELOAD_RIGHT : UZI_AKIMBO_RELOAD_LEFT;
	}
	else
	{
		m_iReloadSide = iAnim;
		m_iNextReloadAnim = -1;
	}

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + fDelay;
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.07;

	return bResult;
}

void CUzi::FinishReload( void )
{
	if ( !m_fIsAkimbo )
	{
		CBasePlayerWeapon::FinishReload();
		return;
	}

	if ( !m_fInReload )
		return;

	if ( UTIL_WeaponTimeBase() < m_hPlayer->m_flNextAttack )
		return;

	if ( m_iNextReloadAnim == -1 )
	{
		if ( m_iReloadSide == UZI_AKIMBO_RELOAD_LEFT )
			ReloadClip( iMaxClip(), m_iClip2, m_iPrimaryAmmoType );
		else if ( m_iReloadSide == UZI_AKIMBO_RELOAD_RIGHT )
			ReloadClip( iMaxClip(), m_iClip, m_iPrimaryAmmoType );

		m_iReloadSide = -1;
	}
	else if ( m_iAkimboReloadAnim == UZI_AKIMBO_RELOAD_BOTH )
	{
		if ( m_iReloadSide == UZI_AKIMBO_RELOAD_LEFT )
		{
			ReloadClip( iMaxClip(), m_iClip2, m_iPrimaryAmmoType );
			m_iReloadSide = UZI_AKIMBO_RELOAD_RIGHT;
		}
		else if ( m_iReloadSide == UZI_AKIMBO_RELOAD_RIGHT )
		{
			ReloadClip( iMaxClip(), m_iClip, m_iPrimaryAmmoType );
			m_iReloadSide = UZI_AKIMBO_RELOAD_LEFT;
		}
	}
	else if ( m_iAkimboReloadAnim == UZI_AKIMBO_RELOAD_RIGHT )
	{
		ReloadClip( iMaxClip(), m_iClip, m_iPrimaryAmmoType );
	}
	else
	{
		ReloadClip( iMaxClip(), m_iClip2, m_iPrimaryAmmoType );
	}

	int iNextAnim = m_iNextReloadAnim;

	if ( iNextAnim == -1 || ( m_hPlayer->pev->button & ( IN_ATTACK | IN_ATTACK2 ) ) )
	{
		m_fInReload = false;
	}
	else if ( m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] )
	{
		// reload the other side
		SendWeaponAnim( iNextAnim, 1 );

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.8;
		m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.8;
		m_iNextReloadAnim = -1;
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = m_flTimeWeaponIdle + 0.1;

		m_hPlayer->SetAnimation( PLAYER_RELOAD );
	}
	else
	{
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = m_flTimeWeaponIdle = UTIL_WeaponTimeBase();
		m_iNextReloadAnim = -1;
	}
}

bool CUzi::ShouldReload( void )
{
	return CBasePlayerWeapon::ShouldReload() && ( !m_fIsAkimbo || !m_iClip2 );
}

void CUzi::WeaponIdle( void )
{
	ResetEmptySound();

	m_hPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	if ( m_fIsAkimbo )
	{
		SendWeaponAnim( UZI_AKIMBO_IDLE, UseDecrement() );
	}
	else
	{
		int iAnim = UTIL_SharedRandomLong( m_hPlayer->random_seed, 0, 1 ) ? UZI_IDLE3 : UZI_IDLE2;

		SendWeaponAnim( iAnim, UseDecrement() );
	}

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );
}

void CUzi::InactiveItemPostFrame( void )
{
	CheckAkimboMagazines();
}

void CUzi::Holster( int skiplocal /* = 0 */ )
{
	ChangeWeaponSkin( UZI_SKIN_DEFAULT );
	CBasePlayerWeapon::Holster( skiplocal );
}

void CUzi::CheckGilded( int fAllowGilded )
{
	if ( fAllowGilded && CanHaveXtra( XTRA_GOLDEN_UZI, m_hPlayer ) )
		ChangeWeaponSkin( UZI_SKIN_GILDED );
	else
		ChangeWeaponSkin( UZI_SKIN_DEFAULT );
}

const char *CUzi::GetHUDSpriteName( void )
{
	return m_fAkimboAvailable ? "weapon_uziakimbo" : "weapon_uzi";
}

void CUzi::SetAkimbo( bool bAkimbo, bool bForce )
{
	pev->classname = MAKE_STRING( "weapon_uzi" );

	if ( bAkimbo && CVAR_GET_FLOAT( "mp_no_akimbo_uzis" ) != 0 )
		bAkimbo = false;

	if ( !bForce && bAkimbo == m_fIsAkimbo )
		return;

	const char *pszModel = GetW_Model( bAkimbo ? "models/w_2uzis.mdl" : "models/w_uzi.mdl" );

	SET_MODEL( ENT( pev ), pszModel );
	m_iszWorldModel = MAKE_STRING( pszModel );
	m_fIsAkimbo = bAkimbo;

	if ( bAkimbo )
		m_fAkimboAvailable = true;
}

const char *CUzi::GetPModelName( void )
{
	const char *pszGoldModel = m_fIsAkimbo ? "models/p_2uzis_gold.mdl" : "models/p_uzi_gold.mdl";
	const char *pszModel = m_fIsAkimbo ? "models/p_2uzis.mdl" : "models/p_uzi.mdl";

	if ( CanHaveXtra( XTRA_GOLDEN_UZI, m_hPlayer ) )
		pszModel = pszGoldModel;

	return GetP_Model( pszModel );
}

CBaseEntity *CUzi::Respawn( void )
{
	CUzi *pNewWeapon = (CUzi *)CBasePlayerWeapon::Respawn();

	if ( pNewWeapon && pNewWeapon->m_fIsAkimbo != m_fIsAkimbo )
	{
		pNewWeapon->SetAkimbo( m_fIsAkimbo, true );

		if ( pNewWeapon->m_fIsAkimbo )
			pNewWeapon->m_iClip2 = UZI_MAX_CLIP;
	}

	return pNewWeapon;
}

// Dropping an akimbo pair leaves the second uzi behind and keeps the first one
CBasePlayerItem *CUzi::DropItem( void )
{
	if ( !m_fAkimboAvailable )
		return CBasePlayerWeapon::DropItem();

	CheckAkimboMagazines();

	CUzi *pUzi = (CUzi *) CBaseEntity::Create( "weapon_uzi", pev->origin, pev->angles, false, NULL );

	pUzi->m_iDefaultAmmo = 0;
	pUzi->m_iClip = m_iClip2;

	m_iClip2 = 0;
	m_fAkimboAvailable = false;

	SetAkimbo( false, false );

	LoadSprites( m_hPlayer, GetHUDSpriteName() );

	if ( this == m_hPlayer->m_pActiveItem )
	{
		Holster();
		Deploy( UZI_DRAW2, 0 );
	}

	return pUzi;
}

// Moves the left magazine into the right uzi once the right one and the reserve are empty
void CUzi::CheckAkimboMagazines( void )
{
	if ( m_iClip == 0 && m_iClip2 > 0 && m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] <= 0 )
	{
		m_iClip = m_iClip2;
		m_iClip2 = 0;
	}
}

void CUzi::GetWeaponData( weapon_data_t *data )
{
	data->iuser1 = m_iPrimaryAmmoType;
	data->iuser2 = m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ];
	data->iuser3 = m_fIsAkimbo;
	data->iuser4 = m_iClip2;
}

void CUzi::SetWeaponData( weapon_data_t *data )
{
	m_iPrimaryAmmoType = data->iuser1;
	m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] = data->iuser2;
	m_fIsAkimbo = data->iuser3 != 0;
	m_iClip2 = data->iuser4;
}

class CUziAmmoClip : public CPlayerAmmo
{
public:
	CUziAmmoClip() : CPlayerAmmo( AMMO_UZICLIP_GIVE, "9mm", _9MM_MAX_CARRY ) { }

	const char *GetDefaultModel( void ) { return "models/w_uzi_clip.mdl"; }
};

LINK_ENTITY_TO_CLASS( ammo_uziclip, CUziAmmoClip );
