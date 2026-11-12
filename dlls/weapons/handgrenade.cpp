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
#include "monsters/monsters.h"
#include "weapons/weapons.h"
#include "player/player.h"
#include "weaponinfo.h"
#include "event_flags.h"

#include "gamerules/CClassicMode.h"
#include "gamerules/game.h"
#include "sound/server_sound.h"

#define WEAPONCUSTOM_SND_NULL 7
#define WEAPONCUSTOM_SND_BBCOVER 18

#undef SET_MODEL
#define SET_MODEL( entity, model ) SetModel_Intercept( entity, model )

#undef PRECACHE_SOUND
#define PRECACHE_SOUND( path ) PrecacheSound_Intercept( this, path )

enum handgrenade_e {
	HANDGRENADE_IDLE = 0,
	HANDGRENADE_FIDGET,
	HANDGRENADE_PINPULL,
	HANDGRENADE_THROW1,
	HANDGRENADE_THROW2,
	HANDGRENADE_THROW3,
	HANDGRENADE_HOLSTER,
	HANDGRENADE_DRAW
};

#undef PRECACHE_MODEL
#define PRECACHE_MODEL( path ) UTIL_PrecacheModel( this, path )

bool HandGrenadeClassicMode( void )
{
	return CClassicMode::GetInstance()->IsEnabled();
}

LINK_ENTITY_TO_CLASS( weapon_handgrenade, CHandGrenade );

void CHandGrenade::Spawn( void )
{
	m_iId = WEAPON_HANDGRENADE;

	Precache( );

	if ( HandGrenadeClassicMode() )
		SET_MODEL(ENT(pev), GetW_Model( "models/hlclassic/w_grenade.mdl" ) );
	else
		SET_MODEL( ENT( pev ), GetW_Model( "models/w_grenade.mdl" ) );

	m_flCustomDmg = pev->dmg;
	if ( !( pev->dmg > 0.0f ) )
		pev->dmg = sk_plr_hand_grenade.value;

	m_iDefaultAmmo = HANDGRENADE_DEFAULT_GIVE;

	CBasePhysicsObject::Spawn();// get ready to fall down.
}

void CHandGrenade::Precache( void )
{
	PrecacheCustomModels();

	bool bClassic = HandGrenadeClassicMode();

	PRECACHE_MODEL( bClassic ? "models/hlclassic/w_grenade.mdl" : "models/w_grenade.mdl");
	PRECACHE_MODEL( bClassic ? "models/hlclassic/v_grenade.mdl" : "models/v_grenade.mdl");
	PRECACHE_MODEL( bClassic ? "models/hlclassic/p_grenade.mdl" : "models/p_grenade.mdl");
	PRECACHE_MODEL( "models/cretegibs.mdl" );

	PRECACHE_SOUND( "misc/bbcover.wav" );
	PRECACHE_SOUND( "misc/bbimpact.wav" );
}

bool CHandGrenade::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Hand Grenade";
	p->iMaxAmmo1 = HANDGRENADE_MAX_CARRY;
	p->pszAmmo1Entity = "weapon_handgrenade";
	p->iAmmo1Drop = 1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo2Entity = NULL;
	p->iAmmo2Drop = AMMO_DROP_NONE;
	p->iMaxClip = HANDGRENADE_MAX_CLIP;
	p->iSlot = 4;
	p->iPosition = 0;
	p->iId = m_iId = WEAPON_HANDGRENADE;
	p->iWeight = HANDGRENADE_WEIGHT;
	p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;

	return true;
}

bool CHandGrenade::Deploy( void )
{
	m_flReleaseThrow = 1.0;
	m_fReleaseThrowActive = false;

	if ( HandGrenadeClassicMode() )
		return DefaultDeploy( GetV_Model( "models/hlclassic/v_grenade.mdl" ), GetP_Model( "models/hlclassic/p_grenade.mdl" ), HANDGRENADE_DRAW, "gren" );
	return DefaultDeploy( GetV_Model( "models/v_grenade.mdl" ), GetP_Model( "models/p_grenade.mdl" ), HANDGRENADE_DRAW, "gren" );
}

bool CHandGrenade::CanHolster( void )
{
	// can only holster hand grenades when not primed!
	return m_flStartThrow == 0;
}

void CHandGrenade::Holster( int skiplocal /* = 0 */ )
{
	m_fInReload = false; // cancel any reload in progress.

	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;

	if ( m_hPlayer && m_hPlayer->IsAlive() )
	{
		m_flStartThrow = 0;
		m_fStartThrowActive = false;
		m_flReleaseThrow = 1.0;
		m_fReleaseThrowActive = false;
	}

	PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), g_evWeaponCustom, 0.0, (float *) &g_vecZero, (float *) &g_vecZero,
	                     WEAPONCUSTOM_SND_NULL, 0.8, WEAPONCUSTOM_SOUND, CHAN_WEAPON, 0, 0 );

	if ( !m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] )
	{
		// no more grenades!
		SetThink( &CBasePlayerItem::DestroyItem );
		pev->nextthink = UTIL_WeaponTimeBase() + 0.1;
	}

	m_hPlayer->pev->viewmodel = 0;
}

void CHandGrenade::PrimaryAttack( void )
{
	if ( !m_flStartThrow && m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] > 0 )
	{
		m_flStartThrow = UTIL_WeaponTimeBase();
		m_flReleaseThrow = 0;
		m_fStartThrowActive = true;
		m_fReleaseThrowActive = false;
		m_fThrowAll = false;

		m_flTimeWeaponIdle = m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;

		SendWeaponAnim( HANDGRENADE_PINPULL, 0, 0 );
		m_hPlayer->SetAnimation( PLAYER_ATTACK1, 1 );
		return;
	}

	m_hPlayer->SetAnimation( PLAYER_ATTACK1, 2 );

	// a primed grenade is thrown after a while
	if ( UTIL_WeaponTimeBase() >= m_flStartThrow + 4.0 )
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase();
		WeaponIdle();
	}
}

// Primes all ten grenades at once
void CHandGrenade::SecondaryAttack( void )
{
	if ( !FRunfuncs()
	     || CVAR_GET_FLOAT( "mp_banana" ) == 0.0f
	     || m_flStartThrow )
		return;

	if ( m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] < HANDGRENADE_MAX_CARRY )
		return;

	m_flStartThrow = UTIL_WeaponTimeBase();
	m_flReleaseThrow = 0;
	m_fStartThrowActive = true;
	m_fReleaseThrowActive = false;
	m_fThrowAll = true;

	SendWeaponAnim( HANDGRENADE_PINPULL, 0, 0 );

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
}

void CHandGrenade::TertiaryAttack( void )
{
	WeaponIdle();
}

void CHandGrenade::WeaponIdle( void )
{
	if ( m_flReleaseThrow == 0 )
	{
		m_flReleaseThrow = UTIL_WeaponTimeBase();
		m_fReleaseThrowActive = true;
	}

	if ( !( m_flTimeWeaponIdle <= UTIL_WeaponTimeBase() ) )
		return;

	if ( m_flStartThrow )
	{
		Vector angThrow = m_hPlayer->pev->v_angle + m_hPlayer->pev->punchangle;

		if ( angThrow.x < 0 )
			angThrow.x = -10 + angThrow.x * ( 80 / 90.0 );
		else
			angThrow.x = -10 + angThrow.x * ( 100 / 90.0 );

		float flVel = fminf( 700.0f, ( 90 - angThrow.x ) * 10 );

		UTIL_MakeVectors( angThrow );

		Vector vecSrc = m_hPlayer->pev->origin + m_hPlayer->pev->view_ofs + gpGlobals->v_forward * 16;

		Vector vecThrow = gpGlobals->v_forward * flVel + m_hPlayer->pev->velocity;

		// alway explode 3 seconds after the pin was pulled
		float time = fmaxf( 0.0f, m_flStartThrow - UTIL_WeaponTimeBase() + 4.0f );

		bool bThrowAll = m_fThrowAll && m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= HANDGRENADE_MAX_CARRY;

		if ( bThrowAll )
		{
			hudtextparms_t params = {};

			params.x = 0.47f;
			params.y = 0.495f;
			params.r1 = 250;
			params.g1 = 252;
			params.b1 = 66;
			params.a1 = 255;
			params.fadeinTime = 0.5f;
			params.fadeoutTime = 0.5f;
			params.holdTime = 2.0f;
			params.channel = 2;

			UTIL_HudMessage( m_hPlayer, params, "BANANA ATTACK!\n" );
			CGrenade::ShootBananaCluster( m_hPlayer->pev, vecSrc, vecThrow );
		}
		else
		{
			CGrenade::ShootTimed( m_hPlayer->pev, vecSrc,
			                      m_hPlayer->IsAlive() ? vecThrow : m_hPlayer->pev->velocity, time );
		}

		if ( bThrowAll )
		{
			PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), g_evWeaponCustom, 0.0, (float *) &g_vecZero, (float *) &g_vecZero,
			                     WEAPONCUSTOM_SND_BBCOVER, 1.0, WEAPONCUSTOM_SOUND, CHAN_WEAPON, 0, 0 );
		}
		else
		{
			if ( flVel >= 500 )
				SendWeaponAnim( flVel >= 1000 ? HANDGRENADE_THROW3 : HANDGRENADE_THROW2, 0, 0 );
			else
			SendWeaponAnim( HANDGRENADE_THROW1, 0, 0 );
		}

		// player "shoot" animation
		m_hPlayer->SetAnimation( PLAYER_ATTACK1 );

		m_flStartThrow = 0;
		m_fStartThrowActive = false;

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;

		if ( bThrowAll )
			m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= HANDGRENADE_MAX_CARRY;
		else
			m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ]--;

		return;
	}

	if ( !( m_flReleaseThrow >= 0 ) )
	{
		// we've finished the throw, restart.
		m_flStartThrow = 0;
		m_fStartThrowActive = false;

		if ( m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] )
		{
			SendWeaponAnim( HANDGRENADE_DRAW, 0, 0 );

			m_flReleaseThrow = 1.0;
			m_fReleaseThrowActive = false;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );
		}
		else
		{
			RetireWeapon();
		}

		return;
	}

	if ( m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] )
	{
		int iAnim;
		float flRand = UTIL_SharedRandomFloat( m_hPlayer->random_seed, 0, 1 );
		if (flRand <= 0.75)
		{
			iAnim = HANDGRENADE_IDLE;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );// how long till we do this again.
		}
		else 
		{
			iAnim = HANDGRENADE_FIDGET;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 75.0 / 30.0;
		}

		SendWeaponAnim( iAnim, 0, 0 );
	}
}

void CHandGrenade::CustomDecrement( float flDecrement )
{
	if ( m_fStartThrowActive )
		m_flStartThrow = V_max( m_flStartThrow - flDecrement, -18.0f );

	if ( m_fReleaseThrowActive )
		m_flReleaseThrow = V_max( m_flReleaseThrow - flDecrement, -18.0f );
}

void CHandGrenade::GetWeaponData( weapon_data_t *data )
{
	data->iuser1 = m_iPrimaryAmmoType;
	data->iuser2 = m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType];
	data->iuser3 = m_iSecondaryAmmoType;
	data->iuser4 = m_hPlayer->m_rgAmmo[m_iSecondaryAmmoType];
	data->fuser1 = m_flStartThrow;
	data->fuser2 = m_flReleaseThrow;
}

void CHandGrenade::SetWeaponData( weapon_data_t *data )
{
	m_iPrimaryAmmoType = data->iuser1;
	m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] = data->iuser2;
	m_iSecondaryAmmoType = data->iuser3;
	m_hPlayer->m_rgAmmo[m_iSecondaryAmmoType] = data->iuser4;
	m_flStartThrow = data->fuser1;
	m_flReleaseThrow = data->fuser2;
}
