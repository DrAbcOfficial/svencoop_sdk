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
#include "m16.h"
#include "playerammo.h"
#include "player.h"
#include "soundent.h"
#include "weaponinfo.h"
#include "event_flags.h"

LINK_ENTITY_TO_CLASS( weapon_m16, CM16 );

void CM16::Spawn( void )
{
	Precache();
	UTIL_SetModel( this, GetW_Model( "models/w_m16.mdl" ) );
	pev->classname = MAKE_STRING( "weapon_m16" );

	m_iDefaultAmmo = M16_DEFAULT_GIVE;
	m_iSecondaryAmmoType = 0;
	m_iId = WEAPON_M16;
	m_iGrenadeReloadAnim = M16_RELOAD_M203;
	m_flCustomDmg = pev->dmg;
	m_iClip2 = 0;

	if ( g_iARGrenadesHack == 0 )
		g_iARGrenadesHack = -1;

	CBasePhysicsObject::Spawn();
}

void CM16::Precache( void )
{
	PrecacheCustomModels();

	UTIL_PrecacheModel( this, "models/v_m16a2.mdl" );
	UTIL_PrecacheModel( this, "models/w_m16.mdl" );
	UTIL_PrecacheModel( this, "models/p_m16.mdl" );
	UTIL_PrecacheModel( this, "models/grenade.mdl" );// grenade
	UTIL_PrecacheModel( this, "models/w_9mmARclip.mdl" );

	m_iShell = UTIL_PrecacheModel( this, "models/saw_shell.mdl" );// brass shell

	PrecacheSound_Intercept( this, "items/9mmclip1.wav" );
	PrecacheSound_Intercept( this, "items/clipinsert1.wav" );
	PrecacheSound_Intercept( this, "items/cliprelease1.wav" );
	PrecacheSound_Intercept( this, "weapons/m16_single.wav" );
	PrecacheSound_Intercept( this, "weapons/glauncher.wav" );
	PrecacheSound_Intercept( this, "weapons/glauncher2.wav" );
	PrecacheSound_Intercept( this, "weapons/357_cock1.wav" );
	PrecacheSound_Intercept( this, "weapons/gl_reload.wav" );
	PrecacheSound_Intercept( this, "weapons/m16_charge.wav" );
	PrecacheSound_Intercept( this, "weapons/m16_magout_metallic.wav" );
	PrecacheSound_Intercept( this, "weapons/m16_magin_metallic.wav" );
	PrecacheSound_Intercept( this, "weapons/m16_draw.wav" );

	m_usBurstFire = PRECACHE_EVENT( 1, "events/m16a2.sc" );
}

bool CM16::GetItemInfo( ItemInfo *p )
{
	p->pszName = STRING( pev->classname );
	p->pszAmmo1 = "556";
	p->iMaxAmmo1 = _556_MAX_CARRY;
	p->pszAmmo1Entity = "ammo_556clip";
	p->iAmmo1Drop = M16_AMMO1_DROP;
	p->pszAmmo2 = "ARgrenades";
	p->iMaxAmmo2 = M203_GRENADE_MAX_CARRY;
	p->pszAmmo2Entity = "ammo_ARgrenades";
	p->iAmmo2Drop = M16_AMMO2_DROP;
	p->iMaxClip = M16_MAX_CLIP;
	p->iSlot = 2;
	p->iPosition = 3;
	p->iId = m_iId = WEAPON_M16;
	p->iWeight = M16_WEIGHT;
	p->iFlags = 0;

	return true;
}

bool CM16::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		AddToPlayerMessage();
		return true;
	}

	return false;
}

void CM16::ItemPostFrame( void )
{
	if ( pev->fuser2 > 0 )
		WeaponIdle();

	CBasePlayerWeapon::ItemPostFrame();
}

// Fires one round of the three round burst started by PrimaryAttack
void CM16::BurstSupplement( void )
{
	m_iId = WEAPON_M16;

	// player "shoot" animation
	m_hPlayer->SetAnimation( PLAYER_ATTACK1 );

	m_hPlayer->m_iWeaponVolume = 1024;
	m_hPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	Vector vecSrc = m_hPlayer->GetGunPosition();
	Vector vecAiming = m_hPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
	Vector vecSpread = BulletAccuracy( VECTOR_CONE_6DEGREES, VECTOR_CONE_4DEGREES, VECTOR_CONE_3DEGREES );

	PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), m_usBurstFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, m_iClip, 0, pev->fuser2 == M16_BURST_ROUNDS, 0 );

	m_hPlayer->FireBullets( 1, vecSrc, vecAiming, vecSpread, 8192, BULLET_PLAYER_SAW, 2, (int)m_flCustomDmg, NULL, FBDM_DRAW );

	m_flNextSecondaryAttack = m_flNextSecondaryAttack + 0.07;

	if ( UTIL_WeaponTimeBase() > m_flNextSecondaryAttack )
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.07;

	pev->fuser2 = pev->fuser2 - 1;
	m_iClip--;

	if ( !m_iClip && m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		m_hPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );
}

void CM16::Holster( int skiplocal /* = 0 */ )
{
	pev->fuser2 = 0;
	m_fInReload = false;// cancel any reload in progress.

	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	m_hPlayer->pev->viewmodel = 0;
}

void CM16::PrimaryAttack( void )
{
	m_iId = WEAPON_M16;

	// don't fire underwater
	if ( m_hPlayer->pev->waterlevel == 3 || m_iClip <= 0 )
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15;
		return;
	}

	if ( pev->fuser2 <= 0 )
	{
		pev->fuser2 = M16_BURST_ROUNDS;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
	}
}

void CM16::SecondaryAttack( void )
{
	int &reloadAnim = m_iGrenadeReloadAnim;

	// don't fire underwater
	if ( m_hPlayer->pev->waterlevel == 3 )
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15;
		return;
	}

	if ( m_hPlayer->m_rgAmmo[m_iSecondaryAmmoType] <= 0 && m_iClip2 <= 0 )
	{
		PlayEmptySound();
		return;
	}

	if ( reloadAnim > 0 )
	{
		// load a grenade into the launcher first
		EMIT_SOUND_DYN( ENT( m_hPlayer->pev ), CHAN_WEAPON, "weapons/gl_reload.wav", 1.0, ATTN_NORM, 0, PITCH_NORM );

		m_hPlayer->SetAnimation( PLAYER_ATTACK1, 1 );
		SendWeaponAnim( reloadAnim, 0, 0 );

		float flTime = UTIL_WeaponTimeBase() + 1.5;

		m_hPlayer->m_flNextAttack = flTime;
		m_flNextSecondaryAttack = flTime;
		m_flNextPrimaryAttack = flTime;
		m_flTimeWeaponIdle = flTime;

		reloadAnim = 0;
		m_iClip2 = 1;
		m_hPlayer->m_rgAmmo[ m_iSecondaryAmmoType ]--;
		return;
	}

	m_hPlayer->m_iWeaponVolume = 1024;
	m_hPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	m_hPlayer->m_iExtraSoundTypes = bits_SOUND_DANGER;
	m_hPlayer->m_flStopExtraSoundTime = UTIL_WeaponTimeBase() + 0.2;

	m_iClip2 = 0;
	reloadAnim = M16_RELOAD_M203;

	SendWeaponAnim( M16_LAUNCH, 0, 0 );

	// player "shoot" animation
	m_hPlayer->SetAnimation( PLAYER_ATTACK1, 2 );

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

	UTIL_MakeVectors( m_hPlayer->pev->v_angle + m_hPlayer->pev->punchangle );

	Vector source;

	if ( m_hPlayer->pev->button & IN_DUCK )
		source = m_hPlayer->pev->origin + gpGlobals->v_forward * 16 + gpGlobals->v_right * 6;
	else
		source = m_hPlayer->pev->origin + m_hPlayer->pev->view_ofs * 0.5f
			+ gpGlobals->v_forward * 16 + gpGlobals->v_right * 6;

	CGrenade::ShootContact( m_hPlayer->pev, source, gpGlobals->v_forward * 1000 );

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.8;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.8;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 5;// idle pretty soon after shooting.

	if ( m_hPlayer->m_rgAmmo[m_iSecondaryAmmoType] <= 0 )
		m_hPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );
}

int CM16::SecondaryAmmoIndex( void )
{
	return m_iSecondaryAmmoType;
}

bool CM16::Deploy( void )
{
	pev->fuser2 = 0;

	return DefaultDeploy( GetV_Model( "models/v_m16a2.mdl" ), GetP_Model( "models/p_m16.mdl" ), M16_DRAW, "m16" );
}

void CM16::Reload( void )
{
	// don't interrupt a burst
	if ( pev->fuser2 != 0 )
		return;

	if ( DefaultReload( M16_MAX_CLIP, M16_RELOAD_M16, 3.0 ) )
		CBasePlayerWeapon::Reload();
}

void CM16::WeaponIdle( void )
{
	m_iId = WEAPON_M16;

	if ( m_iClip <= 0 )
		pev->fuser2 = 0;

	if ( pev->fuser2 > 0 && UTIL_WeaponTimeBase() >= m_flNextSecondaryAttack )
	{
		BurstSupplement();
		return;
	}

	ResetEmptySound();

	m_hPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	int iAnim = UTIL_SharedRandomLong( m_hPlayer->random_seed, 0, 1 ) ? M16_IDLE : M16_FIDGET;

	SendWeaponAnim( iAnim, 0, 0 );

	float idleDelay = UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + idleDelay;
}

void CM16::GetWeaponData( weapon_data_t *data )
{
	data->iuser1 = m_iPrimaryAmmoType;
	data->iuser2 = m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ];
	data->iuser3 = m_hPlayer->m_rgAmmo[ m_iSecondaryAmmoType ];
	data->fuser1 = m_iGrenadeReloadAnim;
	data->fuser2 = pev->fuser2;
}

void CM16::SetWeaponData( weapon_data_t *data )
{
	m_iPrimaryAmmoType = data->iuser1;
	m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] = data->iuser2;
	m_iGrenadeReloadAnim = (int)data->fuser1;
	pev->fuser2 = data->fuser2;
}

class CM16AmmoClip : public CPlayerAmmo
{
public:
	CM16AmmoClip() : CPlayerAmmo( AMMO_556CLIP_GIVE, "556", _556_MAX_CARRY ) { }

	const char *GetDefaultModel( void ) { return "models/w_9mmARclip.mdl"; }
};

LINK_ENTITY_TO_CLASS( ammo_556clip, CM16AmmoClip );
