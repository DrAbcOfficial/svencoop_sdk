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
#include "weapons/m249.h"
#include "player/player.h"
#include "weaponinfo.h"
#include "event_flags.h"
#include "studio.h"

LINK_ENTITY_TO_CLASS( weapon_m249, CM249 );
LINK_ENTITY_TO_CLASS( weapon_saw, CM249 );

void CM249::Spawn( void )
{
	Precache();
	SetModel_Intercept( edict(), GetW_Model( "models/w_saw.mdl" ) );

	m_iDefaultAmmo = M249_DEFAULT_GIVE;
	m_iSecondaryAmmoType = 0;
	pev->classname = MAKE_STRING( "weapon_m249" );
	m_iId = WEAPON_M249;
	m_flCustomDmg = pev->dmg;
	m_fLegacyModel = false;

	CBasePhysicsObject::Spawn();
}

void CM249::Precache( void )
{
	PrecacheCustomModels();

	m_iShell = PrecacheModel_Intercept( this, "models/saw_shell.mdl" ); // brass shell

	PrecacheModel_Intercept( this, "models/v_saw.mdl" );
	PrecacheModel_Intercept( this, "models/w_saw.mdl" );
	PrecacheModel_Intercept( this, "models/p_saw.mdl" );

	PrecacheSound_Intercept( this, "weapons/saw_fire1.wav" );
	PrecacheSound_Intercept( this, "weapons/saw_fire2.wav" );
	PrecacheSound_Intercept( this, "weapons/saw_fire3.wav" );
	PrecacheSound_Intercept( this, "weapons/saw_reload.wav" );
	PrecacheSound_Intercept( this, "weapons/saw_reload2.wav" );
	PrecacheSound_Intercept( this, "weapons/357_cock1.wav" );
	PrecacheSound_Intercept( this, "weapons/saw_coveropen.wav" );
	PrecacheSound_Intercept( this, "weapons/saw_boxout.wav" );
	PrecacheSound_Intercept( this, "weapons/saw_boxin.wav" );
	PrecacheSound_Intercept( this, "weapons/saw_beltin.wav" );
	PrecacheSound_Intercept( this, "weapons/saw_coverdown.wav" );
	PrecacheSound_Intercept( this, "weapons/saw_bolt.wav" );

	m_usFireM249Legacy = PRECACHE_EVENT( 1, "events/m249_LEGACY.sc" );
	m_usFireM249 = PRECACHE_EVENT( 1, "events/m249.sc" );
}

bool CM249::GetItemInfo( ItemInfo *p )
{
	p->pszName = STRING( pev->classname );
	p->pszAmmo1 = "556";
	p->iMaxAmmo1 = _556_MAX_CARRY;
	p->pszAmmo1Entity = "ammo_556";
	p->iAmmo1Drop = M249_AMMO1_DROP;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo2Entity = NULL;
	p->iAmmo2Drop = AMMO_DROP_NONE;
	p->iMaxClip = M249_MAX_CLIP;
	p->iSlot = 5;
	p->iPosition = 1;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_M249;
	p->iWeight = M249_WEIGHT;

	return true;
}

bool CM249::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		AddToPlayerMessage();
		return true;
	}

	return false;
}

// The belt submodel shows the last rounds of the box
int CM249::GetBodyFromClip( void )
{
	if ( m_fLegacyModel )
	{
		if ( (unsigned int) m_iClip > M249_LEGACY_BELT_ROUNDS )
			return 0;

		return M249_LEGACY_BELT_ROUNDS + 1 - m_iClip;
	}

	if ( (unsigned int) m_iClip > M249_BELT_ROUNDS )
		return 0;

	return M249_BELT_ROUNDS - m_iClip;
}

bool CM249::CheckForLegacyModel( void )
{
	int fileLength = 0;
	byte *modelData;

	m_fLegacyModel = true;

	// The server interface does not expose cached viewmodel headers
	modelData = LOAD_FILE_FOR_ME( (char *) STRING( m_hPlayer->pev->viewmodel ), &fileLength );

	if ( !modelData )
		return true;

	studiohdr_t *header = (studiohdr_t *) modelData;

	if ( fileLength >= (int) sizeof( studiohdr_t ) && header->numseq > 0 &&
	     header->seqindex >= 0 && header->seqindex <= fileLength &&
	     header->numseq <= ( fileLength - header->seqindex ) / (int) sizeof( mstudioseqdesc_t ) )
	{
		mstudioseqdesc_t *sequences = (mstudioseqdesc_t *) ( modelData + header->seqindex );

		for ( int i = 0; i < header->numseq; i++ )
		{
			if ( !strcmp( sequences[i].label, "deploy" ) )
			{
				m_fLegacyModel = false;
				break;
			}
		}
	}

	FREE_FILE( modelData );

	return m_fLegacyModel;
}

// Starts the two stage reload, WeaponIdle plays the second half
bool CM249::SpecialReload( int iClipSize, int iAnim, float fDelay, int body )
{
	if ( m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		return false;

	int j = V_min( iClipSize - m_iClip, m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] );

	if ( j == 0 )
		return false;

	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + fDelay;

	SendWeaponAnim( iAnim, 1, GetBodyFromClip() );

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + ( m_fLegacyModel ? 1.5 : 2.5 );
	m_iReloadEndAnim = M249_RELOAD_END;
	m_fInReload = true;
	CBasePlayerWeapon::Reload();

	return true;
}

void CM249::ItemPostFrame( void )
{
	if ( m_fInReload && m_iReloadEndAnim )
		WeaponIdle();

	CBasePlayerWeapon::ItemPostFrame();
}

void CM249::PrimaryAttack( void )
{
	m_iId = WEAPON_M249;

	// don't fire underwater
	if ( m_hPlayer->pev->waterlevel == 3 || m_iClip <= 0 )
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15;
		return;
	}

	m_hPlayer->pev->effects |= EF_MUZZLEFLASH;

	m_hPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_hPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	m_iClip--;

	// player "shoot" animation
	m_hPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc = m_hPlayer->GetGunPosition();
	Vector vecAiming = m_hPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
	Vector vecSpread = BulletAccuracy( VECTOR_CONE_7DEGREES, VECTOR_CONE_4DEGREES, VECTOR_CONE_3DEGREES );

	PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), m_fLegacyModel ? m_usFireM249Legacy : m_usFireM249, 0.0, (float *) &g_vecZero, (float *) &g_vecZero, 0.0, 0.0, m_iClip, 0, 0, 0 );

	m_hPlayer->FireBullets( 1, vecSrc, vecAiming, vecSpread, 2048, BULLET_PLAYER_SAW, 2, (int) m_flCustomDmg, NULL, FBDM_DRAW );

	if ( !m_iClip && m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
	{
		// HEV suit - indicate out of ammo condition
		m_hPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );
	}

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.08f;

	m_hPlayer->pev->punchangle.x -= UTIL_SharedRandomFloat( m_hPlayer->random_seed, 0, 2 );
	m_hPlayer->pev->punchangle.y += UTIL_SharedRandomFloat( m_hPlayer->random_seed, -0.5, 0.5 );

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );
}

void CM249::SecondaryAttack( void )
{
	m_iId = WEAPON_M249;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
}

int CM249::SecondaryAmmoIndex( void )
{
	return m_iSecondaryAmmoType;
}

bool CM249::Deploy( void )
{
	bool bResult = DefaultDeploy( GetV_Model( "models/v_saw.mdl" ), GetP_Model( "models/p_saw.mdl" ), M249_DRAW, "saw", 0, GetBodyFromClip() );

	if ( bResult )
		CheckForLegacyModel();

	return bResult;
}

void CM249::Holster( int skiplocal /* = 0 */ )
{
	m_fInReload = false; // cancel any reload in progress.
	m_iReloadEndAnim = 0;

	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	m_hPlayer->pev->viewmodel = 0;
}

void CM249::Reload( void )
{
	if ( m_flNextPrimaryAttack > UTIL_WeaponTimeBase() )
		return;

	if ( m_fInReload && m_iReloadEndAnim )
		return;

	SpecialReload( M249_MAX_CLIP, M249_RELOAD_START, m_fLegacyModel ? 1.5 : 3.5, 1 );
}

void CM249::WeaponIdle( void )
{
	ResetEmptySound();

	m_hPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	if ( m_iReloadEndAnim )
	{
		// second half of the reload
		SendWeaponAnim( m_iReloadEndAnim, 0 );

		float flDelay = m_fLegacyModel ? 2.5 : 3.0;

		m_iReloadEndAnim = 0;
		m_fInReload = true;

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + flDelay;
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = m_flTimeWeaponIdle + 0.1f;
		return;
	}

	int iAnim;

	if ( UTIL_SharedRandomLong( m_hPlayer->random_seed, 0, 4 ) == 1 )
	{
		iAnim = M249_SLOWIDLE;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 9.0;
	}
	else
	{
		iAnim = M249_IDLE2;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 13.0;
	}

	SendWeaponAnim( iAnim, 0, GetBodyFromClip() );
}

void CM249::GetWeaponData( weapon_data_t *data )
{
	data->iuser1 = m_iPrimaryAmmoType;
	data->iuser2 = m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType];
	data->iuser3 = m_fLegacyModel;
	data->fuser1 = m_iReloadEndAnim;
}

void CM249::SetWeaponData( weapon_data_t *data )
{
	m_iPrimaryAmmoType = data->iuser1;
	m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] = data->iuser2;
	m_fLegacyModel = data->iuser3 != 0;
	m_iReloadEndAnim = (int) data->fuser1;
}
