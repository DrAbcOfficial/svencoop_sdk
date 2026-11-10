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
#include "desert_eagle.h"
#include "player.h"
#include "weaponinfo.h"
#include "event_flags.h"

LINK_ENTITY_TO_CLASS( weapon_eagle, CDesertEagle );

#ifndef CLIENT_DLL
LINK_ENTITY_TO_CLASS( desert_eagle_spot, CDESpot );

CDesertEagle::CDesertEagle()
{
	m_hSpot = NULL;
}

void CDESpot::Spawn( void )
{
	Precache();
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;
	pev->scale = 0.35f;
	pev->rendermode = kRenderGlow;
	pev->renderfx = kRenderFxNoDissipation;
	pev->renderamt = 255;
	UTIL_SetModel( this, "sprites/laserdot.spr" );
	UTIL_SetOrigin( pev, pev->origin );
}

void CDESpot::Precache( void )
{
	UTIL_PrecacheModel( this, "sprites/laserdot.spr" );
}

void CDESpot::Suspend( float delay )
{
	pev->effects |= EF_NODRAW;
	SetThink( &CDESpot::Revive );
	pev->nextthink = gpGlobals->time + delay;
}

void CDESpot::Revive( void )
{
	pev->effects &= ~EF_NODRAW;
	SetThink( NULL );
}

CDESpot *CDESpot::CreateDESpot( void )
{
	CDESpot *spot = GetClassPtr( (CDESpot *) NULL );

	spot->Spawn();
	spot->pev->classname = MAKE_STRING( "desert_eagle_spot" );
	return spot;
}
#endif

void CDesertEagle::Spawn( void )
{
	Precache();
#ifdef CLIENT_DLL
	SET_MODEL( ENT( pev ), GetW_Model( "models/w_desert_eagle.mdl" ) );
#else
	UTIL_SetModel( this, GetW_Model( "models/w_desert_eagle.mdl" ) );
#endif
	pev->classname = MAKE_STRING( "weapon_eagle" );

	m_iDefaultAmmo = EAGLE_DEFAULT_GIVE;
	m_iId = WEAPON_EAGLE;
	m_flCustomDmg = pev->dmg;
	m_fInZoom = false;
	m_fLaserOn = TRUE;

#ifdef CLIENT_DLL
	FallInit();// get ready to fall down.
#else
	CBasePhysicsObject::Spawn();
#endif
}

void CDesertEagle::Precache( void )
{
#ifdef CLIENT_DLL
	PrecacheCustomModels();

	PRECACHE_MODEL( "models/v_desert_eagle.mdl" );
	PRECACHE_MODEL( "models/w_desert_eagle.mdl" );
	PRECACHE_MODEL( "models/p_desert_eagle.mdl" );

	PRECACHE_SOUND( "items/9mmclip1.wav" );
	PRECACHE_SOUND( "weapons/de_shot1.wav" );
	PRECACHE_SOUND( "weapons/desert_eagle_sight.wav" );
	PRECACHE_SOUND( "weapons/desert_eagle_sight2.wav" );
	PRECACHE_SOUND( "weapons/desert_eagle_reload.wav" );
	PRECACHE_SOUND( "weapons/desert_eagle_slidepush.wav" );

	m_usFireEagle = PRECACHE_EVENT( 1, "events/deagle.sc" );
#else
	PrecacheCustomModels();

	UTIL_PrecacheModel( this, "models/v_desert_eagle.mdl" );
	UTIL_PrecacheModel( this, "models/w_desert_eagle.mdl" );
	UTIL_PrecacheModel( this, "models/p_desert_eagle.mdl" );

	PrecacheSound_Intercept( this, "items/9mmclip1.wav" );
	PrecacheSound_Intercept( this, "weapons/de_shot1.wav" );
	PrecacheSound_Intercept( this, "weapons/desert_eagle_sight.wav" );
	PrecacheSound_Intercept( this, "weapons/desert_eagle_sight2.wav" );
	PrecacheSound_Intercept( this, "weapons/desert_eagle_reload.wav" );
	PrecacheSound_Intercept( this, "weapons/desert_eagle_slidepush.wav" );

	m_usFireEagle = PRECACHE_EVENT( 1, "events/deagle.sc" );
#endif
}

bool CDesertEagle::GetItemInfo( ItemInfo *p )
{
	p->pszName = STRING( pev->classname );
	p->pszAmmo1 = "357";
	p->iMaxAmmo1 = _357_MAX_CARRY;
	p->pszAmmo1Entity = "ammo_357";
	p->iAmmo1Drop = EAGLE_AMMO1_DROP;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo2Entity = NULL;
	p->iAmmo2Drop = AMMO_DROP_NONE;
	p->iMaxClip = EAGLE_MAX_CLIP;
	p->iFlags = 0;
	p->iSlot = 1;
	p->iPosition = 2;
	p->iId = m_iId = WEAPON_EAGLE;
	p->iWeight = EAGLE_WEIGHT;

	return true;
}

bool CDesertEagle::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		AddToPlayerMessage();
		return true;
	}

	return false;
}

void CDesertEagle::PrimaryAttack( void )
{
	if ( m_hPlayer->pev->waterlevel == 3 || m_iClip <= 0 )
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.3;
		return;
	}

#ifndef CLIENT_DLL
	if ( m_fLaserOn && m_hSpot )
		m_hSpot->pev->nextthink = gpGlobals->time;
#endif

	m_iClip--;

#ifdef CLIENT_DLL
	m_hPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
#else
	m_hPlayer->m_iWeaponVolume = 1536;
#endif
	m_hPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	m_hPlayer->pev->effects |= EF_MUZZLEFLASH;

	// player "shoot" animation
	m_hPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc = m_hPlayer->GetGunPosition();

	PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), m_usFireEagle, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, m_iClip == 0, 1 );

	if ( m_fInZoom )
	{
		Vector vecAiming = m_hPlayer->GetAutoaimVector( AUTOAIM_8DEGREES );
		Vector vecSpread = BulletAccuracy( VECTOR_CONE_5DEGREES, VECTOR_CONE_4DEGREES, VECTOR_CONE_3DEGREES );

		m_hPlayer->FireBullets( 1, vecSrc, vecAiming, vecSpread, 8192, BULLET_PLAYER_EAGLE, 0, (int)m_flCustomDmg, NULL, FBDM_DRAW );
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.3;
	}
	else
	{
		Vector vecAiming = m_hPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
		Vector vecSpread = BulletAccuracy( VECTOR_CONE_3DEGREES, VECTOR_CONE_2DEGREES, VECTOR_CONE_1DEGREES );

		m_hPlayer->FireBullets( 1, vecSrc, vecAiming, vecSpread, 8192, BULLET_PLAYER_EAGLE, 0, (int)m_flCustomDmg, NULL, FBDM_DRAW );
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.6;
	}

#ifdef CLIENT_DLL
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );
#else
	float idleDelay = UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + idleDelay;
#endif

	if ( !m_iClip && m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] <= 0 )
	{
		// HEV suit - indicate out of ammo condition
		m_hPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );
	}

	UpdateSpot();
}

void CDesertEagle::SecondaryAttack( void )
{
	if ( m_fSpotActive )
	{
		m_fLaserOn = !m_fLaserOn;

		if ( !m_fLaserOn )
		{
#ifndef CLIENT_DLL
			if ( m_hSpot )
			{
				m_hSpot->Killed( NULL, GIB_NORMAL );
				m_hSpot = NULL;
			}
#endif

			m_fInZoom = true;
			PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), g_evWeaponCustom, 0.0, (float *)&g_vecZero, (float *)&g_vecZero,
				WEAPONCUSTOM_SND_DESERT_EAGLE_SIGHT2, 1.0, WEAPONCUSTOM_SOUND, CHAN_WEAPON, 0, 0 );
		}
		else
		{
			PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), g_evWeaponCustom, 0.0, (float *)&g_vecZero, (float *)&g_vecZero,
				WEAPONCUSTOM_SND_DESERT_EAGLE_SIGHT, 1.0, WEAPONCUSTOM_SOUND, CHAN_WEAPON, 0, 0 );
			m_fInZoom = false;
		}

		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.6;
	}
	else
	{
#ifndef CLIENT_DLL
		if ( m_hSpot )
		{
			m_hSpot->Killed( NULL, GIB_NORMAL );
			m_hSpot = NULL;
		}
#endif

		m_fLaserOn = FALSE;
		m_fInZoom = true;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
	}

#ifdef CLIENT_DLL
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 6, 12 );
#else
	float idleDelay = UTIL_SharedRandomFloat( m_hPlayer->random_seed, 6, 12 );

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + idleDelay;
#endif
}

bool CDesertEagle::Deploy( void )
{
#ifndef CLIENT_DLL
	if ( CVAR_GET_FLOAT( "weaponmode_eagle" ) == 1.0f )
	{
		m_fSpotActive = false;
		m_fLaserOn = FALSE;
		m_fInZoom = true;
		pev->body = 1;
	}
	else
	{
		m_fSpotActive = true;
		pev->body = 0;
	}
#endif

	return DefaultDeploy( GetV_Model( "models/v_desert_eagle.mdl" ), GetP_Model( "models/p_desert_eagle.mdl" ), EAGLE_DRAW, "onehanded" );
}

void CDesertEagle::Holster( int skiplocal /* = 0 */ )
{
	m_fInReload = false;// cancel any reload in progress.

	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;

#ifndef CLIENT_DLL
	if ( m_hSpot )
	{
		m_hSpot->Killed( NULL, GIB_NEVER );
		m_hSpot = NULL;
	}
#endif

	m_hPlayer->pev->viewmodel = 0;
}

void CDesertEagle::Reload( void )
{
	UpdateSpot();

	if ( m_iClip == EAGLE_MAX_CLIP )
		return;

	if ( DefaultReload( EAGLE_MAX_CLIP, m_iClip ? EAGLE_RELOAD_NOSHOT : EAGLE_RELOAD, 1.5 ) )
	{
		if ( m_fLaserOn )
		{
#ifndef CLIENT_DLL
			if ( m_hSpot )
				m_hSpot->Suspend( 1.6f );
#endif

			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.6;
		}

#ifdef CLIENT_DLL
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 8, 14 );
#else
		float idleDelay = UTIL_SharedRandomFloat( m_hPlayer->random_seed, 8, 14 );

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + idleDelay;
#endif
	}

#ifndef CLIENT_DLL
	CBasePlayerWeapon::Reload();
#endif
}

void CDesertEagle::WeaponIdle( void )
{
	UpdateSpot();

	ResetEmptySound();

	m_hPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	int iAnim;
	float flTime;
	float flRand = UTIL_SharedRandomFloat( m_hPlayer->random_seed, 0, 1 );

	if ( flRand <= 0.2 )
	{
		iAnim = EAGLE_IDLE1;
		flTime = UTIL_WeaponTimeBase() + 152.0 / 60.0;
		m_flTimeWeaponIdle = flTime + flRand * 60;

#ifndef CLIENT_DLL
		if ( m_hSpot && m_fLaserOn )
			m_hSpot->Suspend( 136.0f / 60.0f );
#endif
	}
	else if ( flRand <= 0.3 )
	{
		iAnim = EAGLE_IDLE2;
		flTime = UTIL_WeaponTimeBase() + 155.0 / 60.0;
		m_flTimeWeaponIdle = flTime + ( flRand - 0.2 ) * 70;

#ifndef CLIENT_DLL
		if ( m_hSpot && m_fLaserOn )
			m_hSpot->Suspend( 145.0f / 60.0f );
#endif
	}
	else if ( flRand <= 0.6 )
	{
		iAnim = EAGLE_IDLE3;
		flTime = UTIL_WeaponTimeBase() + 104.0 / 60.0;
		m_flTimeWeaponIdle = flTime + ( flRand - 0.3 ) * 25;
	}
	else if ( flRand <= 0.8 )
	{
		iAnim = EAGLE_IDLE4;
		flTime = UTIL_WeaponTimeBase() + 152.0 / 60.0;
		m_flTimeWeaponIdle = flTime + ( flRand - 0.6 ) * 60;
	}
	else
	{
		iAnim = EAGLE_IDLE5;
		flTime = UTIL_WeaponTimeBase() + 122.0 / 60.0;
		m_flTimeWeaponIdle = flTime + ( flRand - 0.8 ) * 60;
	}

#ifdef CLIENT_DLL
	SendWeaponAnim( iAnim );
#else
	SendWeaponAnim( iAnim, 0, 0 );
#endif
}

void CDesertEagle::UpdateSpot( void )
{
	if ( !m_fSpotActive )
		m_fLaserOn = FALSE;

#ifndef CLIENT_DLL
	if ( m_fLaserOn )
	{
		if ( !m_hSpot )
		{
			m_hSpot = CDESpot::CreateDESpot();
			m_hSpot->pev->owner = m_hPlayer->edict();
		}

		UTIL_MakeVectors( m_hPlayer->pev->v_angle );

		Vector source = m_hPlayer->GetGunPosition();
		TraceResult trace;

		UTIL_TraceLine( source, source + gpGlobals->v_forward * 8192,
			dont_ignore_monsters, m_hPlayer->edict(), &trace );
		UTIL_SetOrigin( m_hSpot->pev, trace.vecEndPos );
	}
#endif
}

void CDesertEagle::GetWeaponData( weapon_data_t *data )
{
	data->iuser1 = m_iPrimaryAmmoType;
	data->iuser2 = m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ];
	data->iuser3 = m_fSpotActive;
}

void CDesertEagle::SetWeaponData( weapon_data_t *data )
{
	m_iPrimaryAmmoType = data->iuser1;
	m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] = data->iuser2;
	m_fSpotActive = data->iuser3 != 0;
}
