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

#if ( !defined( OEM_BUILD ) && !defined( HLDEMO_BUILD ) )

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "weaponinfo.h"
#include "event_flags.h"
#include "hornetgun.h"

#include "CClassicMode.h"
#include "hornet.h"

extern cvar_t sk_hornet_pdmg;

#undef PRECACHE_MODEL
#define PRECACHE_MODEL( path ) UTIL_PrecacheModel( this, path )
#undef PRECACHE_SOUND
#define PRECACHE_SOUND( path ) PrecacheSound_Intercept( this, path )
#undef SET_MODEL
#define SET_MODEL( entity, path ) SetModel_Intercept( entity, path )

bool HornetgunClassicMode( void )
{
	return CClassicMode::GetInstance()->IsEnabled();
}

LINK_ENTITY_TO_CLASS( weapon_hornetgun, CHgun );

void CHgun::Spawn( void )
{
	m_iId = WEAPON_HORNETGUN;

	Precache( );

	if ( HornetgunClassicMode() )
		SET_MODEL(ENT(pev), GetW_Model( "models/hlclassic/w_hgun.mdl" ) );
	else
		SET_MODEL( ENT( pev ), GetW_Model( "models/w_hgun.mdl" ) );

	m_iDefaultAmmo = HIVEHAND_DEFAULT_GIVE;
	m_iFirePhase = 0;
	m_flCustomDmg = pev->dmg;

	CBasePhysicsObject::Spawn();
}

void CHgun::Precache( void )
{
	PrecacheCustomModels();

	bool bClassic = HornetgunClassicMode();

	PRECACHE_MODEL( bClassic ? "models/hlclassic/v_hgun.mdl" : "models/v_hgun.mdl");
	PRECACHE_MODEL( bClassic ? "models/hlclassic/w_hgun.mdl" : "models/w_hgun.mdl");
	PRECACHE_MODEL( bClassic ? "models/hlclassic/p_hgun.mdl" : "models/p_hgun.mdl" );

	UTIL_PrecacheOther("hornet");
}

bool CHgun::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Hornets";
	p->iMaxAmmo1 = HORNET_MAX_CARRY;
	p->pszAmmo1Entity = NULL;
	p->iAmmo1Drop = AMMO_DROP_NONE;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo2Entity = NULL;
	p->iAmmo2Drop = AMMO_DROP_NONE;
	p->iMaxClip = HORNETGUN_MAX_CLIP;
	p->iSlot = 3;
	p->iPosition = 3;
	p->iId = m_iId = WEAPON_HORNETGUN;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD;
	p->iWeight = HORNETGUN_WEIGHT;

	return true;
}

bool CHgun::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		AddToPlayerMessage();
		return TRUE;
	}
	return FALSE;
}

void CHgun::OnCreate( void )
{
	CBasePlayerWeapon::OnCreate();

	m_iBounceMaterial = BOUNCE_MATERIAL_FLESH;
}

void CHgun::PrimaryAttack( void )
{
	Reload( );

	if (m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		return;

	UTIL_MakeVectors( m_hPlayer->pev->v_angle );

	CBaseEntity *pHornet = CBaseEntity::Create( "playerhornet", m_hPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 - gpGlobals->v_up * 12, m_hPlayer->pev->v_angle, false, m_hPlayer->edict() );
	pHornet->pev->velocity = gpGlobals->v_forward * 300;

	// The attack stores this on the weapon rather than the hornet
	pev->dmg = sk_hornet_pdmg.value;

	if ( m_flCustomDmg > 0 )
		pev->dmg = m_flCustomDmg;

	m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;


	m_hPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
	m_hPlayer->m_iWeaponFlash = DIM_GUN_FLASH;

	PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), g_evWeaponCustom, 0.0, (float *)&g_vecZero, (float *)&g_vecZero,
	                     WEAPONCUSTOM_SND_AG_FIRE1 + UTIL_SharedRandomLong( m_hPlayer->random_seed, 0, 2 ), 1.0, WEAPONCUSTOM_SOUND, CHAN_WEAPON, 0, 0 );

	m_flRechargeTime = UTIL_WeaponTimeBase() + 0.5;

	SendWeaponAnim( HGUN_SHOOT );



	// player "shoot" animation
	m_hPlayer->SetAnimation( PLAYER_ATTACK1 );

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.25;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );
}

void CHgun::SecondaryAttack( void )
{
	Reload();

	if (m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		return;

	UTIL_MakeVectors( m_hPlayer->pev->v_angle );

	Vector vecSrc = m_hPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 - gpGlobals->v_up * 12;

	m_iFirePhase++;
	switch ( m_iFirePhase )
	{
	case 1:
		vecSrc = vecSrc + gpGlobals->v_up * 8;
		break;
	case 2:
		vecSrc = vecSrc + gpGlobals->v_up * 8;
		vecSrc = vecSrc + gpGlobals->v_right * 8;
		break;
	case 3:
		vecSrc = vecSrc + gpGlobals->v_right * 8;
		break;
	case 4:
		vecSrc = vecSrc - gpGlobals->v_up * 8;
		vecSrc = vecSrc + gpGlobals->v_right * 8;
		break;
	case 5:
		vecSrc = vecSrc - gpGlobals->v_up * 8;
		break;
	case 6:
		vecSrc = vecSrc - gpGlobals->v_up * 8;
		vecSrc = vecSrc - gpGlobals->v_right * 8;
		break;
	case 7:
		vecSrc = vecSrc - gpGlobals->v_right * 8;
		break;
	case 8:
		vecSrc = vecSrc + gpGlobals->v_up * 8;
		vecSrc = vecSrc - gpGlobals->v_right * 8;
		m_iFirePhase = 0;
		break;
	}

	CBaseEntity *pHornet = CBaseEntity::Create( "playerhornet", vecSrc, m_hPlayer->pev->v_angle, false, m_hPlayer->edict() );
	pHornet->pev->velocity = gpGlobals->v_forward * 1200;
	pHornet->pev->angles = UTIL_VecToAngles( pHornet->pev->velocity );

	pHornet->SetThink( &CHornet::StartDart );

	// The attack stores this on the weapon rather than the hornet
	pev->dmg = sk_hornet_pdmg.value;

	if ( m_flCustomDmg > 0 )
		pev->dmg = m_flCustomDmg;

	PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), g_evWeaponCustom, 0.0, (float *)&g_vecZero, (float *)&g_vecZero,
	                     WEAPONCUSTOM_SND_AG_FIRE1 + UTIL_SharedRandomLong( m_hPlayer->random_seed, 0, 2 ), 1.0, WEAPONCUSTOM_SOUND, CHAN_WEAPON, 0, 0 );


	m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
	m_hPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_hPlayer->m_iWeaponFlash = DIM_GUN_FLASH;

	m_flRechargeTime = UTIL_WeaponTimeBase() + 0.5;

	SendWeaponAnim( HGUN_SHOOT );

		// player "shoot" animation
	m_hPlayer->SetAnimation( PLAYER_ATTACK1 );

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.1;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );

	m_hPlayer->pev->punchangle.x = UTIL_SharedRandomFloat( m_hPlayer->random_seed, 0, 2 );
}

bool CHgun::Deploy( void )
{
	if ( HornetgunClassicMode() )
		return DefaultDeploy( GetV_Model( "models/hlclassic/v_hgun.mdl" ), GetP_Model( "models/hlclassic/p_hgun.mdl" ), HGUN_UP, "hive" );

	return DefaultDeploy( GetV_Model( "models/v_hgun.mdl" ), GetP_Model( "models/p_hgun.mdl" ), HGUN_UP, "hive" );
}

void CHgun::Holster( int skiplocal /* = 0 */ )
{
	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;

	//!!!HACKHACK - can't select hornetgun if it's empty! no way to get ammo for it, either.
	if ( !m_hPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] )
		m_hPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] = 1;

	m_hPlayer->pev->viewmodel = 0;
}

// Hornets regenerate over time instead of being reloaded
void CHgun::Reload( void )
{
	if (m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= HORNET_MAX_CARRY)
		return;

	while (m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] < HORNET_MAX_CARRY && m_flRechargeTime < UTIL_WeaponTimeBase() )
	{
		m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType]++;
		m_flRechargeTime += 1.5;
	}
}

void CHgun::WeaponIdle( void )
{
	Reload( );

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	int iAnim;
	float flRand = UTIL_SharedRandomFloat( m_hPlayer->random_seed, 0, 1 );
	if (flRand <= 0.75)
	{
		iAnim = HGUN_IDLE1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 60.0 / 16.0;
	}
	else if (flRand <= 0.875)
	{
		iAnim = HGUN_FIDGETSWAY;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 40.0 / 16.0;
	}
	else
	{
		iAnim = HGUN_FIDGETSHAKE;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 35.0 / 16.0;
	}
	SendWeaponAnim( iAnim );
}

void CHgun::InactiveItemPostFrame( void )
{
	Reload();
}

void CHgun::CustomDecrement( float flDecrement )
{
	m_flNextAnimTime = V_max( m_flNextAnimTime - flDecrement, -0.01 );
	m_flRechargeTime = V_max( m_flRechargeTime - flDecrement, -0.01 );
}

void CHgun::GetWeaponData( weapon_data_t *data )
{
	data->iuser1 = m_iPrimaryAmmoType;
	data->iuser2 = m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType];
	data->iuser3 = m_iSecondaryAmmoType;
	data->iuser4 = m_hPlayer->m_rgAmmo[m_iSecondaryAmmoType];
	data->fuser1 = m_flNextAnimTime;
	data->fuser2 = m_flRechargeTime;
}

void CHgun::SetWeaponData( weapon_data_t *data )
{
	m_iPrimaryAmmoType = data->iuser1;
	m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] = data->iuser2;
	m_iSecondaryAmmoType = data->iuser3;
	m_hPlayer->m_rgAmmo[m_iSecondaryAmmoType] = data->iuser4;
	m_flNextAnimTime = data->fuser1;
	m_flRechargeTime = data->fuser2;
}

#endif
