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
#include "weapons/shotgun.h"

#include "gamerules/CClassicMode.h"
#include "player/playerammo.h"

#undef PRECACHE_MODEL
#define PRECACHE_MODEL( path ) UTIL_PrecacheModel( this, path )
#undef PRECACHE_SOUND
#define PRECACHE_SOUND( path ) PrecacheSound_Intercept( this, path )
#undef SET_MODEL
#define SET_MODEL( entity, path ) SetModel_Intercept( entity, path )

bool ShotgunClassicMode( void )
{
	return CClassicMode::GetInstance()->IsEnabled();
}

LINK_ENTITY_TO_CLASS( weapon_shotgun, CShotgun );

void CShotgun::Spawn( void )
{
	Precache( );

	// the view model tells whether it has a double barrel animation
	if ( ShotgunClassicMode() )
		SET_MODEL(ENT(pev), GetV_Model( "models/hlclassic/v_shotgun.mdl" ) );
	else
		SET_MODEL( ENT( pev ), GetV_Model( "models/v_shotgun.mdl" ) );

	m_fHasDoubleAnim = LookupSequence( "shoot_double" ) != -1;

	if ( ShotgunClassicMode() )
		SET_MODEL( ENT( pev ), GetW_Model( "models/hlclassic/w_shotgun.mdl" ) );
	else
		SET_MODEL( ENT( pev ), GetW_Model( "models/w_shotgun.mdl" ) );

	m_iDefaultAmmo = SHOTGUN_DEFAULT_GIVE;
	m_iId = WEAPON_SHOTGUN;
	m_flCustomDmg = pev->dmg;

	CBasePhysicsObject::Spawn();
}

void CShotgun::Precache( void )
{
	PrecacheCustomModels();

	bool bClassic = ShotgunClassicMode();

	PRECACHE_MODEL( bClassic ? "models/hlclassic/v_shotgun.mdl" : "models/v_shotgun.mdl");
	PRECACHE_MODEL( bClassic ? "models/hlclassic/w_shotgun.mdl" : "models/w_shotgun.mdl");
	PRECACHE_MODEL( bClassic ? "models/hlclassic/p_shotgun.mdl" : "models/p_shotgun.mdl");

	m_iShell = PRECACHE_MODEL ( bClassic ? "models/hlclassic/shotgunshell.mdl" : "models/shotgunshell.mdl" ); // shotgun shell

	PRECACHE_SOUND( "hlclassic/items/9mmclip1.wav" );
	PRECACHE_SOUND( "hlclassic/weapons/dbarrel1.wav" );
	PRECACHE_SOUND( "hlclassic/weapons/sbarrel1.wav" );
	PRECACHE_SOUND( "hlclassic/weapons/reload1.wav" );
	PRECACHE_SOUND( "hlclassic/weapons/reload3.wav" );
	PRECACHE_SOUND( "hlclassic/weapons/357_cock1.wav" );
	PRECACHE_SOUND( "hlclassic/weapons/scock1.wav" );// shotgun shell

	PRECACHE_SOUND("items/9mmclip1.wav");

	PRECACHE_SOUND ("weapons/dbarrel1.wav");//shotgun
	PRECACHE_SOUND ("weapons/sbarrel1.wav");//shotgun

	PRECACHE_SOUND ("weapons/reload1.wav");	// shotgun reload
	PRECACHE_SOUND ("weapons/reload3.wav");	// shotgun reload

//	PRECACHE_SOUND ("weapons/sshell1.wav");	// shotgun reload - played on client
//	PRECACHE_SOUND ("weapons/sshell3.wav");	// shotgun reload - played on client

	PRECACHE_SOUND ("weapons/357_cock1.wav"); // gun empty sound
	PRECACHE_SOUND ("weapons/scock1.wav");	// cock gun

	m_usSingleFire = PRECACHE_EVENT( 1, "events/shotgun1.sc" );
	m_usDoubleFire = PRECACHE_EVENT( 1, "events/shotgun2.sc" );
}

bool CShotgun::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "buckshot";
	p->iMaxAmmo1 = BUCKSHOT_MAX_CARRY;
	p->pszAmmo1Entity = "ammo_buckshot";
	p->iAmmo1Drop = SHOTGUN_AMMO1_DROP;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo2Entity = NULL;
	p->iAmmo2Drop = AMMO_DROP_NONE;
	p->iMaxClip = SHOTGUN_MAX_CLIP;
	p->iSlot = 2;
	p->iPosition = 1;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_SHOTGUN;
	p->iWeight = SHOTGUN_WEIGHT;

	return true;
}

bool CShotgun::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		AddToPlayerMessage();
		return TRUE;
	}
	return FALSE;
}

void CShotgun::ItemPostFrame( void )
{
	if ( m_fNeedPump && UTIL_WeaponTimeBase() + 4.5 > m_flTimeWeaponIdle )
	{
		// pump the shotgun after a shot
		PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), g_evWeaponCustom, 0.0, (float *) &g_vecZero, (float *) &g_vecZero,
		                     WEAPONCUSTOM_SND_SCOCK1, 1.0, WEAPONCUSTOM_SOUND, CHAN_ITEM, 0, 0 );

		m_fNeedPump = false;

		if ( m_iClip == 0 )
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.25;
	}

	CBasePlayerWeapon::ItemPostFrame();
}

void CShotgun::PrimaryAttack( void )
{
	// don't fire underwater
	if (m_hPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15;
		return;
	}

	if (m_iClip <= 0)
	{
		Reload( );
		PlayEmptySound( );
		return;
	}

	PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), m_usSingleFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );

	m_hPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_hPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip--;

	// player "shoot" animation
	m_hPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc = m_hPlayer->GetGunPosition();
	Vector vecAiming = m_hPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	m_hPlayer->FireBullets( SHOTGUN_SINGLE_PELLETS, vecSrc, vecAiming, VECTOR_CONE_DM_SHOTGUN, 2048, BULLET_PLAYER_BUCKSHOT, 0, (int) m_flCustomDmg, NULL, FBDM_DRAW );


	if (!m_iClip && m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		// HEV suit - indicate out of ammo condition
		m_hPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
	}

	m_flPumpTime = UTIL_WeaponTimeBase() + 0.5;

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + ( m_fInZoom ? 0.95 : 0.75);
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.75;
	if (m_iClip == 0 )
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.75;
	m_fInSpecialReload = 0;
	m_fNeedPump = true;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 5.0;
}

// m_fInZoom selects the double barrel blast, otherwise this is a fast single shot
void CShotgun::SecondaryAttack( void )
{
	// don't fire underwater
	if (m_hPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15;
		return;
	}

	if (m_iClip <= 0 )
	{
		Reload( );
		PlayEmptySound( );
		return;
	}

	m_hPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_hPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	if ( m_fInZoom )
	{
		if ( m_iClip <= 1 )
		{
			Reload();
			PlayEmptySound();
			return;
		}

		PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), m_usDoubleFire, 0.0, (float *) &g_vecZero, (float *) &g_vecZero, 0.0, 0.0, 0, 0, m_fHasDoubleAnim, 0 );

		m_iClip -= 2;

	// player "shoot" animation
	m_hPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc	 = m_hPlayer->GetGunPosition( );
	Vector vecAiming = m_hPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

		m_hPlayer->FireBullets( SHOTGUN_DOUBLE_PELLETS, vecSrc, vecAiming, VECTOR_CONE_DM_DOUBLESHOTGUN, 2048, BULLET_PLAYER_BUCKSHOT, 0, (int) m_flCustomDmg, NULL, FBDM_DRAW );

		if ( !m_iClip && m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		{
			// HEV suit - indicate out of ammo condition
			m_hPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );
	}

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.5;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.5;

		m_fNeedPump = true;
		m_fInSpecialReload = 0;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 5.5;
		return;
	}

	PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), m_usDoubleFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );

	m_iClip--;

	// player "shoot" animation
	m_hPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc = m_hPlayer->GetGunPosition();
	Vector vecAiming = m_hPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	m_hPlayer->FireBullets( SHOTGUN_SINGLE_PELLETS, vecSrc, vecAiming, VECTOR_CONE_DM_DOUBLESHOTGUN, 2048, BULLET_PLAYER_BUCKSHOT, 0, 0, NULL, FBDM_DRAW );

	if (!m_iClip && m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		// HEV suit - indicate out of ammo condition
		m_hPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
	}

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.25;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + ( m_iClip == 0 ? 1.5 : 6.0 );

	m_fNeedPump = false;

	m_fInSpecialReload = 0;

}

void CShotgun::Holster( int skiplocal /* = 0 */ )
{
	m_fInSpecialReload = 0;

	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	m_hPlayer->pev->viewmodel = 0;
}

bool CShotgun::Deploy( void )
{
	m_fInZoom = CVAR_GET_FLOAT( "weaponmode_shotgun" ) == 1.0f || ShotgunClassicMode();

	if ( ShotgunClassicMode() )
		return DefaultDeploy( GetV_Model( "models/hlclassic/v_shotgun.mdl" ), GetP_Model( "models/hlclassic/p_shotgun.mdl" ), SHOTGUN_DRAW, "shotgun" );

	return DefaultDeploy( GetV_Model( "models/v_shotgun.mdl" ), GetP_Model( "models/p_shotgun.mdl" ), SHOTGUN_DRAW, "shotgun" );
}

void CShotgun::Reload( void )
{
	if (m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == SHOTGUN_MAX_CLIP)
		return;

	// don't reload until recoil is done
	if ( m_flNextReload > UTIL_WeaponTimeBase())
		return;

	// check to see if we're ready to reload
	if (m_fInSpecialReload == 0)
	{
		if ( m_flNextPrimaryAttack > UTIL_WeaponTimeBase() )
			return;

		SendWeaponAnim( SHOTGUN_START_RELOAD );
		m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.7;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.7;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.7;
		m_fInSpecialReload = 1;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.7;
		return;
	}

	if (m_fInSpecialReload != 1)
	{
		CBasePlayerWeapon::Reload();
		return;
	}

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
			return;

	if ( m_iClip >= SHOTGUN_MAX_CLIP )
	{
		m_fInSpecialReload = 0;
		return;
	}

	// was waiting for gun to move to side
	SendWeaponAnim( SHOTGUN_RELOAD );

	if ( ShotgunClassicMode() )
	{
		m_flNextReload = UTIL_WeaponTimeBase() + 0.5;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.7;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.7;
	}
	else
	{
		m_flNextReload = UTIL_WeaponTimeBase() + 0.3;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.3;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
	}

	// Add them to the clip
	m_iClip++;
	m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;

	bool bClassic = ShotgunClassicMode();

	switch (RANDOM_LONG(0,1))
	{
		case 0:
			EMIT_SOUND_DYN(ENT(m_hPlayer->pev), CHAN_ITEM, bClassic ? "hlclassic/weapons/reload1.wav" : "weapons/reload1.wav",
			                1, ATTN_NORM, 0, 85 + RANDOM_LONG( 0, 0x1f ), 0, false, g_vecZero );
			break;

		case 1:
			EMIT_SOUND_DYN( ENT( m_hPlayer->pev ), CHAN_ITEM, bClassic ? "hlclassic/weapons/reload3.wav" : "weapons/reload3.wav",
			                1, ATTN_NORM, 0, 85 + RANDOM_LONG( 0, 0x1f ), 0, false, g_vecZero );
			break;
	}

	CBasePlayerWeapon::Reload();
}

void CShotgun::WeaponIdle( void )
{
	ResetEmptySound( );

	m_hPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( UTIL_WeaponTimeBase() <= m_flTimeWeaponIdle )
		return;

	if (m_iClip == 0 && m_fInSpecialReload == 0 && m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType])
		{
			Reload( );
		return;
	}

	if (m_fInSpecialReload != 0)
		{
			if (m_iClip < SHOTGUN_MAX_CLIP && m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType])
			{
				Reload( );
			return;
		}

		// reload debounce has timed out
		SendWeaponAnim( SHOTGUN_PUMP );

		// play cocking sound
		PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), g_evWeaponCustom, 0.0, (float *) &g_vecZero, (float *) &g_vecZero,
		                     WEAPONCUSTOM_SND_SCOCK1, 1.0, WEAPONCUSTOM_SOUND, CHAN_ITEM, 0, 1 );

		m_fNeedPump = false;
				m_fInSpecialReload = 0;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
		return;
	}

	int iAnim;
			float flRand = UTIL_SharedRandomFloat( m_hPlayer->random_seed, 0, 1 );
			if (flRand <= 0.8)
			{
				iAnim = SHOTGUN_IDLE_DEEP;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (60.0/12.0);// * RANDOM_LONG(2, 5);
			}
			else if (flRand <= 0.95)
			{
				iAnim = SHOTGUN_IDLE;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (20.0/9.0);
			}
			else
			{
				iAnim = SHOTGUN_IDLE4;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (20.0/9.0);
			}
			SendWeaponAnim( iAnim );
		}

void CShotgun::CustomDecrement( float flDecrement )
{
	m_flNextReload = V_max( m_flNextReload - flDecrement, -32.0 );
	m_flPumpTime = V_max( m_flPumpTime - flDecrement, -32.0 );
}

void CShotgun::GetWeaponData( weapon_data_t *data )
{
	data->iuser1 = m_iPrimaryAmmoType;
	data->iuser2 = m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType];
	data->iuser3 = m_iSecondaryAmmoType;
	data->iuser4 = m_hPlayer->m_rgAmmo[m_iSecondaryAmmoType];
	data->fuser1 = m_flNextReload;
	data->fuser2 = m_flPumpTime;
	data->fuser3 = m_fHasDoubleAnim;
	data->fuser4 = m_fInSpecialReload;
}

void CShotgun::SetWeaponData( weapon_data_t *data )
{
	m_iPrimaryAmmoType = data->iuser1;
	m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] = data->iuser2;
	m_iSecondaryAmmoType = data->iuser3;
	m_hPlayer->m_rgAmmo[m_iSecondaryAmmoType] = data->iuser4;
	m_flNextReload = data->fuser1;
	m_flPumpTime = data->fuser2;
	m_fHasDoubleAnim = data->fuser3 != 0;
	m_fInSpecialReload = (int) data->fuser4;
}

class CShotgunAmmo : public CPlayerAmmo
{
public:
	CShotgunAmmo() : CPlayerAmmo( AMMO_BUCKSHOTBOX_GIVE, "buckshot", BUCKSHOT_MAX_CARRY ) {}

	const char *GetDefaultModel( void ) { return ShotgunClassicMode() ? "models/hlclassic/w_shotbox.mdl" : "models/w_shotbox.mdl"; }
	const char *GetPickupSound( void ) { return ShotgunClassicMode() ? "hlclassic/items/9mmclip1.wav" : "items/9mmclip1.wav"; }
};

LINK_ENTITY_TO_CLASS( ammo_buckshot, CShotgunAmmo );
