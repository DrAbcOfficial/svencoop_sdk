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

#ifndef CLIENT_DLL
#include "CClassicMode.h"
#include "game.h"
#include "playerammo.h"

#undef PRECACHE_MODEL
#define PRECACHE_MODEL( path ) UTIL_PrecacheModel( this, path )
#undef PRECACHE_SOUND
#define PRECACHE_SOUND( path ) PrecacheSound_Intercept( this, path )
#undef SET_MODEL
#define SET_MODEL( entity, model ) SetModel_Intercept( entity, model )
#define m_bFireOnEmpty m_fFireOnEmpty
#define m_iSilencerState m_iSpecialState
#define GLOCK_BODY_PLAIN 0
#define GLOCK_BODY_SILENCER 1
#define GLOCK_SILENCER_NONE 0
#define GLOCK_SILENCER_ADD 1
#define GLOCK_SILENCER_REMOVE 2
#define GLOCK_AMMO1_DROP AMMO_GLOCKCLIP_GIVE

enum glock_e
{
	GLOCK_IDLE1 = 0,
	GLOCK_IDLE2,
	GLOCK_IDLE3,
	GLOCK_SHOOT,
	GLOCK_SHOOT_EMPTY,
	GLOCK_RELOAD,
	GLOCK_RELOAD_NOT_EMPTY,
	GLOCK_DRAW,
	GLOCK_HOLSTER,
	GLOCK_ADD_SILENCER,
	GLOCK_REMOVE_SILENCER
};
#endif

bool GlockClassicMode( void )
{
#ifdef CLIENT_DLL
	return CClassicModeWrapper::isEnabled();
#else
	return CClassicMode::GetInstance()->IsEnabled();
#endif
}

LINK_ENTITY_TO_CLASS( weapon_glock, CGlock );
LINK_ENTITY_TO_CLASS( weapon_9mmhandgun, CGlock );

void CGlock::Spawn( void )
{
	pev->classname = MAKE_STRING( "weapon_9mmhandgun" ); // hack to allow for old names

	m_iDefaultAmmo = GLOCK_DEFAULT_GIVE;
	m_iId = WEAPON_GLOCK;
	m_flCustomDmg = pev->dmg;

	Precache();

	if ( GlockClassicMode() )
		SET_MODEL( ENT( pev ), GetW_Model( "models/hlclassic/w_9mmhandgun.mdl" ) );
	else
		SET_MODEL( ENT( pev ), GetW_Model( "models/w_9mmhandgun.mdl" ) );

	pev->body = GLOCK_BODY_PLAIN;

#ifdef CLIENT_DLL
	FallInit();// get ready to fall down.
#else
	CBasePhysicsObject::Spawn();
#endif
}

void CGlock::Precache( void )
{
	PrecacheCustomModels();

	bool bClassic = GlockClassicMode();

	PRECACHE_MODEL( bClassic ? "models/hlclassic/v_9mmhandgun.mdl" : "models/v_9mmhandgun.mdl" );
	PRECACHE_MODEL( bClassic ? "models/hlclassic/w_9mmhandgun.mdl" : "models/w_9mmhandgun.mdl" );
	PRECACHE_MODEL( bClassic ? "models/hlclassic/p_9mmhandgun.mdl" : "models/p_9mmhandgun.mdl" );

	PRECACHE_SOUND( "hlclassic/items/9mmclip1.wav" );
	PRECACHE_SOUND( "hlclassic/items/9mmclip2.wav" );
	PRECACHE_SOUND( "hlclassic/weapons/pl_gun3.wav" );
	PRECACHE_SOUND( "items/9mmclip1.wav" );
	PRECACHE_SOUND( "items/9mmclip2.wav" );
	PRECACHE_SOUND( "weapons/pl_gun1.wav" );// silenced handgun
	PRECACHE_SOUND( "weapons/pl_gun2.wav" );// silenced handgun
	PRECACHE_SOUND( "weapons/pl_gun3.wav" );// handgun

	m_iShell = PRECACHE_MODEL( GlockClassicMode() ? "models/hlclassic/shell.mdl" : "models/shell.mdl" );// brass shell

	m_usFireGlock1 = PRECACHE_EVENT( 1, "events/glock1.sc" );
	m_usFireGlock2 = PRECACHE_EVENT( 1, "events/glock2.sc" );
}

bool CGlock::GetItemInfo( ItemInfo *p )
{
	p->pszName = STRING( pev->classname );
	p->pszAmmo1 = "9mm";
	p->iMaxAmmo1 = _9MM_MAX_CARRY;
	p->pszAmmo1Entity = "ammo_9mmclip";
	p->iAmmo1Drop = GLOCK_AMMO1_DROP;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo2Entity = NULL;
	p->iAmmo2Drop = AMMO_DROP_NONE;
	p->iMaxClip = GLOCK_MAX_CLIP;
	p->iSlot = 1;
	p->iPosition = 0;
	p->iFlags = 0;
	p->iWeight = GLOCK_WEIGHT;
	p->iId = m_iId = WEAPON_GLOCK;

	return true;
}

bool CGlock::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		AddToPlayerMessage();
		return true;
	}

	return false;
}

void CGlock::Holster( int skiplocal /* = 0 */ )
{
	m_fInReload = false;// cancel any reload in progress.

	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	m_hPlayer->pev->viewmodel = 0;
}

void CGlock::PrimaryAttack( void )
{
	if ( !m_fInZoom || pev->body != GLOCK_BODY_PLAIN )
		GlockFire( 0.01, 0.3, true );
	else
		GlockFire( 0.1, 0.2, false );
}

// m_fInZoom means the server allows the silencer, pev->body shows it
void CGlock::SecondaryAttack( void )
{
	if ( m_fInZoom )
	{
		float flDelay;

		if ( pev->body == GLOCK_BODY_SILENCER )
		{
			SendWeaponAnim( GLOCK_REMOVE_SILENCER, 0, GLOCK_BODY_SILENCER );
			m_iSilencerState = GLOCK_SILENCER_REMOVE;
			flDelay = UTIL_WeaponTimeBase() + 1.5;
		}
		else
		{
			SendWeaponAnim( GLOCK_HOLSTER, 0, GLOCK_BODY_PLAIN );
			m_iSilencerState = GLOCK_SILENCER_ADD;
			flDelay = UTIL_WeaponTimeBase() + 0.3;
		}

		m_flTimeWeaponIdle = flDelay;
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 2.0;
	}
	else if ( GlockClassicMode() )
	{
		GlockFire( 0.1, 0.2, false );
	}
	else
	{
		GlockFire( 0.1, UTIL_SharedRandomFloat( m_hPlayer->random_seed, 0, 0.01 ) + 0.16, false );
	}
}

void CGlock::GlockFire( float flSpread, float flCycleTime, bool fUseAutoAim )
{
	if ( m_iClip <= 0 )
	{
		if ( m_bFireOnEmpty )
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2;
		}

		return;
	}

	if ( pev->body == GLOCK_BODY_SILENCER && m_fInZoom )
	{
		m_hPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
		m_hPlayer->m_iWeaponFlash = DIM_GUN_FLASH;
	}
	else
	{
		m_hPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
		m_hPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
		pev->body = GLOCK_BODY_PLAIN;
	}

	m_iClip--;

	m_hPlayer->pev->effects |= EF_MUZZLEFLASH;

	// player "shoot" animation
	m_hPlayer->SetAnimation( PLAYER_ATTACK1 );

	PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), fUseAutoAim ? m_usFireGlock1 : m_usFireGlock2, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, m_iClip == 0, pev->body );

	Vector vecSrc = m_hPlayer->GetGunPosition();
	Vector vecAiming;

	if ( fUseAutoAim )
		vecAiming = m_hPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );
	else
		vecAiming = gpGlobals->v_forward;

	m_hPlayer->FireBullets( 1, vecSrc, vecAiming, Vector( flSpread, flSpread, flSpread ), 8192, BULLET_PLAYER_9MM, 0, (int)m_flCustomDmg, NULL, FBDM_DRAW );

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 0, 0.01 );

	if ( !m_iClip && m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] <= 0 )
	{
		// HEV suit - indicate out of ammo condition
		m_hPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );
	}

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );
}

bool CGlock::Deploy( void )
{
	m_iSilencerState = GLOCK_SILENCER_NONE;
#ifndef CLIENT_DLL
	m_fInZoom = CVAR_GET_FLOAT( "weaponmode_9mmhandgun" ) == 1.0f;
#endif

	if ( !m_fInZoom )
		pev->body = GLOCK_BODY_PLAIN;

	if ( GlockClassicMode() )
		return DefaultDeploy( GetV_Model( "models/hlclassic/v_9mmhandgun.mdl" ), GetP_Model( "models/hlclassic/p_9mmhandgun.mdl" ), GLOCK_DRAW, "onehanded", 0, pev->body );

	return DefaultDeploy( GetV_Model( "models/v_9mmhandgun.mdl" ), GetP_Model( "models/p_9mmhandgun.mdl" ), GLOCK_DRAW, "onehanded", 0, pev->body );
}

void CGlock::Reload( void )
{
	if ( m_iClip == GLOCK_MAX_CLIP )
		return;

	if ( DefaultReload( GLOCK_MAX_CLIP, m_iClip ? GLOCK_RELOAD_NOT_EMPTY : GLOCK_RELOAD, 1.5, pev->body ) )
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );

#ifndef CLIENT_DLL
	CBasePlayerWeapon::Reload();
#endif
}

void CGlock::WeaponIdle( void )
{
	ResetEmptySound();

	m_hPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	if ( m_iSilencerState > GLOCK_SILENCER_NONE )
	{
		if ( m_iSilencerState == GLOCK_SILENCER_ADD )
		{
			pev->body = GLOCK_BODY_SILENCER;
			SendWeaponAnim( GLOCK_ADD_SILENCER, 0, pev->body );

			m_iSilencerState = GLOCK_SILENCER_NONE;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.7;
			return;
		}

		if ( m_iSilencerState == GLOCK_SILENCER_REMOVE )
		{
			pev->body = GLOCK_BODY_PLAIN;
			SendWeaponAnim( GLOCK_DRAW, 0, pev->body );

			m_iSilencerState = GLOCK_SILENCER_NONE;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.75;
			return;
		}

		m_iSilencerState = GLOCK_SILENCER_NONE;
	}

	// only idle if the slid isn't back
	if ( m_iClip == 0 )
		return;

	int iAnim;
	float flRand = UTIL_SharedRandomFloat( m_hPlayer->random_seed, 0, 1 );

	if ( flRand <= 0.3 )
	{
		iAnim = GLOCK_IDLE3;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 49.0 / 16;
	}
	else if ( flRand <= 0.6 )
	{
		iAnim = GLOCK_IDLE1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 60.0 / 16.0;
	}
	else
	{
		iAnim = GLOCK_IDLE2;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 40.0 / 16.0;
	}

	SendWeaponAnim( iAnim, 1, pev->body );
}

void CGlock::GetWeaponData( weapon_data_t *data )
{
	data->iuser1 = m_iPrimaryAmmoType;
	data->iuser2 = m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ];

	if ( m_fInZoom )
		data->m_iWeaponState = pev->body;
}

void CGlock::SetWeaponData( weapon_data_t *data )
{
	m_iPrimaryAmmoType = data->iuser1;
	m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] = data->iuser2;

	if ( m_fInZoom )
		pev->body = data->m_iWeaponState;
}

class CGlockAmmo : public CPlayerAmmo
{
public:
	CGlockAmmo() : CPlayerAmmo( AMMO_GLOCKCLIP_GIVE, "9mm", _9MM_MAX_CARRY ) { }

	const char *GetDefaultModel( void ) { return GlockClassicMode() ? "models/hlclassic/w_9mmclip.mdl" : "models/w_9mmclip.mdl"; }
	const char *GetPickupSound( void ) { return GlockClassicMode() ? "hlclassic/items/9mmclip1.wav" : "items/9mmclip1.wav"; }
};

LINK_ENTITY_TO_CLASS( ammo_glockclip, CGlockAmmo );
LINK_ENTITY_TO_CLASS( ammo_9mmclip, CGlockAmmo );
