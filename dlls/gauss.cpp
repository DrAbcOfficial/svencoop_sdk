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
#include "gauss.h"

#ifdef CLIENT_DLL
#include "cl_entity.h"
#include "../cl_dll/hud_iface.h"
#include "../cl_dll/client_sound.h"
#else
#include "CClassicMode.h"
#include "playerammo.h"
#include "soundent.h"
#include "shake.h"

extern cvar_t sk_plr_gauss;
extern cvar_t sk_plr_secondarygauss;

#undef PRECACHE_MODEL
#define PRECACHE_MODEL( path ) UTIL_PrecacheModel( this, path )
#undef PRECACHE_SOUND
#define PRECACHE_SOUND( path ) PrecacheSound_Intercept( this, path )
#undef SET_MODEL
#define SET_MODEL( entity, path ) SetModel_Intercept( entity, path )
#endif

bool GaussClassicMode( void )
{
#ifdef CLIENT_DLL
	return CClassicModeWrapper::isEnabled();
#else
	return CClassicMode::GetInstance()->IsEnabled();
#endif
}

LINK_ENTITY_TO_CLASS( weapon_gauss, CGauss );

void CGauss::Spawn( void )
{
	Precache( );

	if ( GaussClassicMode() )
		SET_MODEL(ENT(pev), GetW_Model( "models/hlclassic/w_gauss.mdl" ) );
	else
		SET_MODEL( ENT( pev ), GetW_Model( "models/w_gauss.mdl" ) );

	m_iDefaultAmmo = GAUSS_DEFAULT_GIVE;
	m_iId = WEAPON_GAUSS;
	m_flCustomDmg = pev->dmg;

#ifdef CLIENT_DLL
	FallInit(); // get ready to fall down.
#else
	CBasePhysicsObject::Spawn();
#endif
}

void CGauss::Precache( void )
{
	PrecacheCustomModels();

	bool bClassic = GaussClassicMode();

	PRECACHE_MODEL( bClassic ? "models/hlclassic/w_gauss.mdl" : "models/w_gauss.mdl");
	PRECACHE_MODEL( bClassic ? "models/hlclassic/v_gauss.mdl" : "models/v_gauss.mdl");
	PRECACHE_MODEL( bClassic ? "models/hlclassic/p_gauss.mdl" : "models/p_gauss.mdl" );

	m_iGlow = PRECACHE_MODEL( "sprites/hotglow.spr" );
	m_iBalls = PRECACHE_MODEL( "sprites/hotglow.spr" );
	m_iBeam = PRECACHE_MODEL( "sprites/smoke.spr" );

	PRECACHE_SOUND("items/9mmclip1.wav");

	PRECACHE_SOUND("weapons/gauss2.wav");
	PRECACHE_SOUND("weapons/electro4.wav");
	PRECACHE_SOUND("weapons/electro5.wav");
	PRECACHE_SOUND("weapons/electro6.wav");
	PRECACHE_SOUND("ambience/pulsemachine.wav");

	m_usGaussFire = PRECACHE_EVENT( 1, "events/gauss.sc" );
	m_usGaussSpin = PRECACHE_EVENT( 1, "events/gaussspin.sc" );
}

bool CGauss::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "uranium";
	p->iMaxAmmo1 = URANIUM_MAX_CARRY;
	p->pszAmmo1Entity = "ammo_gaussclip";
	p->iAmmo1Drop = GAUSS_AMMO1_DROP;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo2Entity = NULL;
	p->iAmmo2Drop = AMMO_DROP_NONE;
	p->iMaxClip = GAUSS_MAX_CLIP;
	p->iSlot = 3;
	p->iPosition = 1;
	p->iId = m_iId = WEAPON_GAUSS;
	p->iFlags = 0;
	p->iWeight = GAUSS_WEIGHT;

	return true;
}

bool CGauss::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		AddToPlayerMessage();
		return TRUE;
	}
	return FALSE;
}

bool CGauss::Deploy( void )
{
	if ( GaussClassicMode() )
		return DefaultDeploy( GetV_Model( "models/hlclassic/v_gauss.mdl" ), GetP_Model( "models/hlclassic/p_gauss.mdl" ), GAUSS_DRAW, "gauss" );
	return DefaultDeploy( GetV_Model( "models/v_gauss.mdl" ), GetP_Model( "models/p_gauss.mdl" ), GAUSS_DRAW, "gauss" );
}

void CGauss::Holster( int skiplocal /* = 0 */ )
{
	m_fInReload = false;

	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	m_fInAttack = 0;

	// stop the spin sound
	PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), g_evWeaponCustom, 0.0, (float *) &g_vecZero, (float *) &g_vecZero,
	                     WEAPONCUSTOM_SND_NULL, 1.0, WEAPONCUSTOM_SOUND, CHAN_WEAPON, 0, 0 );

	m_hPlayer->pev->viewmodel = 0;
}

void CGauss::PrimaryAttack( void )
{
	// don't fire underwater
	if ( m_hPlayer->pev->waterlevel == 3 )
	{
		PlayEmptySound( );
		m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15;
		return;
	}

	if ( m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] < GAUSS_PRIMARY_AMMO )
	{
		PlayEmptySound( );
		m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
		return;
	}

	m_hPlayer->m_iWeaponVolume = GAUSS_PRIMARY_FIRE_VOLUME;
	m_fPrimaryFire = TRUE;

	m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= GAUSS_PRIMARY_AMMO;

	StartFire();
	m_fInAttack = 0;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0;
	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.2;
}

void CGauss::SecondaryAttack( void )
{
	// don't fire underwater
	if ( m_hPlayer->pev->waterlevel == 3 )
	{
		if ( m_fInAttack != 0 )
		{
			PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), g_evWeaponCustom, 0.0, (float *) &g_vecZero, (float *) &g_vecZero,
			                     WEAPONCUSTOM_SND_ELECTRO4, 1.0, WEAPONCUSTOM_SOUND, CHAN_WEAPON, 0, 0 );
			SendWeaponAnim( GAUSS_IDLE );
			m_fInAttack = 0;
		}
		else
		{
			PlayEmptySound( );
		}

		m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		return;
	}

	if ( m_fInAttack == 0 )
	{
		if ( m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		{
			PlayEmptySound();
			m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
			return;
		}

		m_fPrimaryFire = FALSE;

		// take one ammo just to start the spin
		m_flNextAmmoBurn = UTIL_WeaponTimeBase();
		m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
		m_fAmmoBurnActive = true;
		m_fInAttack = 1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
		m_flStartCharge = UTIL_WeaponTimeBase();
		m_fStartChargeActive = true;

		m_hPlayer->m_iWeaponVolume = GAUSS_PRIMARY_CHARGE_VOLUME;

		SendWeaponAnim( GAUSS_SPINUP );

		PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), m_usGaussSpin, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, GAUSS_SPIN_START_PITCH, 0, 0, 0 );

		m_iSoundState = SND_CHANGE_PITCH;
		return;
	}

	if (m_fInAttack == 1)
	{
		if ( UTIL_WeaponTimeBase() > m_flTimeWeaponIdle )
		{
			SendWeaponAnim( GAUSS_SPIN );
			m_fInAttack = 2;
		}

		return;
	}

		if ( m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		{
			// out of ammo! force the gun to fire
			StartFire();
			m_fInAttack = 0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
			return;
		}

		if ( UTIL_WeaponTimeBase() > m_flNextAmmoBurn && m_flNextAmmoBurn != -1 )
		{
			// don't eat any more ammo after gun is fully charged.
			m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
		m_fAmmoBurnActive = true;
		m_flNextAmmoBurn = UTIL_WeaponTimeBase() + 0.3;
	}

	if ( UTIL_WeaponTimeBase() - m_flStartCharge >= GetFullChargeTime() )
	{
		// don't eat any more ammo after gun is fully charged.
		m_flNextAmmoBurn = -1;
		m_fAmmoBurnActive = false;
		}

		int pitch = ( UTIL_WeaponTimeBase() - m_flStartCharge ) * ( GAUSS_MAX_PITCH_RISE / GetFullChargeTime() ) + PITCH_NORM;
		if ( pitch > 250 )
			 pitch = 250;

		// ALERT( at_console, "%d %d %d\n", m_fInAttack, m_iSoundState, pitch );

		if ( m_iSoundState == 0 )
			ALERT( at_console, "sound state %d\n", m_iSoundState );

	if ( m_flStartCharge < UTIL_WeaponTimeBase() - 10 )
	{
		// Player charged up too long. Zap him.
		m_fInAttack = 0;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
		m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;

#ifndef CLIENT_DLL
		float damage;

		if ( m_flCustomDmg > 0.0f )
			damage = m_flCustomDmg * 10.0f;
		else
			damage = sk_plr_secondarygauss.value;

		m_hPlayer->TakeDamage( VARS( eoNullEntity ), VARS( eoNullEntity ), damage, DMG_SHOCK );
		UTIL_ScreenFade( m_hPlayer, Vector( 255, 128, 0 ), 2, 0.5, 128, FFADE_IN );
#endif

		SendWeaponAnim( GAUSS_IDLE );
		return;
	}

	PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), m_usGaussSpin, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, pitch, 0, ( m_iSoundState == SND_CHANGE_PITCH ) ? 1 : 0, 0 );

		m_iSoundState = SND_CHANGE_PITCH; // hack for going through level transitions

		m_hPlayer->m_iWeaponVolume = GAUSS_PRIMARY_CHARGE_VOLUME;
}

void CGauss::TertiaryAttack( void )
{
	WeaponIdle();
}

float CGauss::GetFullChargeTime( void )
{
	return GAUSS_FULL_CHARGE_TIME;
}

//=========================================================
// StartFire- since all of this code has to run and then
// call Fire(), it was easier at this point to rip it out
// of weaponidle() and make its own function then to try to
// merge this into Fire(), which has some identical variable names
//=========================================================
void CGauss::StartFire( void )
{
	UTIL_MakeVectors( m_hPlayer->pev->v_angle + m_hPlayer->pev->punchangle );
	Vector vecAiming = gpGlobals->v_forward;
	Vector vecSrc = m_hPlayer->GetGunPosition( );
#ifdef CLIENT_DLL
	float flDamage = GAUSS_DAMAGE;

	UTIL_WeaponTimeBase();
	GetFullChargeTime();
#else
	float flDamage;

	if ( m_flCustomDmg > 0.0f )
		flDamage = m_flCustomDmg * 10.0f;
	else
		flDamage = sk_plr_secondarygauss.value;

	float charge = UTIL_WeaponTimeBase() - m_flStartCharge;

	if ( charge <= GetFullChargeTime() )
		flDamage = ( charge / GetFullChargeTime() ) * flDamage;

	if ( m_fPrimaryFire )
		flDamage = m_flCustomDmg > 0.0f ? m_flCustomDmg : sk_plr_gauss.value;
#endif

	if (m_fInAttack != 3)
	{
#ifndef CLIENT_DLL
		float flZVel = m_hPlayer->pev->velocity.z;
#endif
		if ( !m_fPrimaryFire )
			m_hPlayer->pev->velocity = m_hPlayer->pev->velocity - gpGlobals->v_forward * flDamage * 5;

#ifndef CLIENT_DLL
		if ( CVAR_GET_FLOAT( "mp_disablegaussjump" ) == 1.0f )
			m_hPlayer->pev->velocity.z = flZVel;
#endif

		m_hPlayer->SetAnimation( PLAYER_ATTACK1 );
	}

	m_fAftershockActive = true;
	m_flPlayAftershock = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 0.3, 0.8 );

	Fire( vecSrc, vecAiming, flDamage );

#ifndef CLIENT_DLL
	if ( m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		m_hPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );
#endif
}

void CGauss::Fire( Vector vecOrigSrc, Vector vecDir, float damage_radius )
{
	m_hPlayer->m_iWeaponVolume = GAUSS_PRIMARY_FIRE_VOLUME;
	PLAYBACK_EVENT_FULL( FEV_NOTHOST | FEV_RELIABLE, m_hPlayer->edict(), m_usGaussFire, 0.0,
	                     (float *) &g_vecZero, (float *) &g_vecZero, damage_radius, 0.0, 0, 0, m_fPrimaryFire ? 1 : 0, 0 );

#ifndef CLIENT_DLL
	Vector vecSrc = vecOrigSrc;
	Vector vecDest = vecSrc + vecDir * 8192;
	edict_t		*pentIgnore = m_hPlayer->edict();
	bool fFirstBeam = true;
	bool punched = false;
	int	nMaxHits = 10;

	while ( damage_radius > 10 && nMaxHits > 0)
	{
		TraceResult tr = {};

		--nMaxHits;

		// ALERT( at_console, "." );
		UTIL_TraceLine(vecSrc, vecDest, dont_ignore_monsters, pentIgnore, &tr);

		if (tr.fAllSolid)
			return;

		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

		if ( !pEntity )
			return;

		if ( fFirstBeam )
		{
			m_hPlayer->pev->effects |= EF_MUZZLEFLASH;
			fFirstBeam = false;
		}

		if (pEntity->pev->takedamage)
		{
			ClearMultiDamage();
			pEntity->TraceAttack( m_hPlayer->pev, damage_radius, vecDir, &tr, DMG_BULLET );
			ApplyMultiDamage(m_hPlayer->pev, m_hPlayer->pev);
		}

		if ( pEntity->ReflectGauss() )
		{
			float n = -DotProduct(tr.vecPlaneNormal, vecDir);

			pentIgnore = NULL;

			if (n < 0.5) // 60 degrees
			{
				vecDir = vecDir + tr.vecPlaneNormal * 2 * n;
				vecSrc = tr.vecEndPos + vecDir * 8;
				vecDest = vecSrc + vecDir * 8192;

				// explode a bit
				m_hPlayer->RadiusDamage( tr.vecEndPos, pev, m_hPlayer->pev,
				                         damage_radius * n, CLASS_NONE, DMG_BLAST );

				if (n == 0.0f ) n = 0.1;

				damage_radius = ( 1.0f - n) * damage_radius;
			}
			else
			{
				if ( punched )
					return;

				punched = true;

				// limit it to one hole punch
				if ( m_fPrimaryFire )
					return;

				TraceResult through = {};

				UTIL_TraceLine( tr.vecEndPos + vecDir * 8, vecDest, dont_ignore_monsters, NULL, &through );
					if ( through.fAllSolid)
					return;

				UTIL_TraceLine( through.vecEndPos, tr.vecEndPos, dont_ignore_monsters, NULL, &through );

						float thickness = ( through.vecEndPos - tr.vecEndPos).Length( );

						if ( damage_radius > thickness )
						{
							if ( thickness == 0.0f )
						thickness = 1.0f;

					damage_radius -= thickness;
					::RadiusDamage( through.vecEndPos + vecDir * 8, pev, m_hPlayer->pev,
					                damage_radius, damage_radius, CLASS_NONE, DMG_BLAST );

							CSoundEnt::InsertSound ( bits_SOUND_COMBAT, pev->origin, NORMAL_EXPLOSION_VOLUME, 3.0, m_hPlayer );

							vecSrc = through.vecEndPos + vecDir;
				}

			}
		}
		else
		{
			vecSrc = tr.vecEndPos + vecDir;
			pentIgnore = pEntity->edict();
		}
	}
#endif
}

void CGauss::WeaponIdle( void )
{
	ResetEmptySound( );

	// play aftershock static discharge
	if ( m_flPlayAftershock && m_flPlayAftershock < UTIL_WeaponTimeBase() )
	{
		switch ( UTIL_SharedRandomLong( m_hPlayer->random_seed, 0,3))
		{
		case 0:
				PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), g_evWeaponCustom, 0.0, (float *) &g_vecZero, (float *) &g_vecZero,
				                     WEAPONCUSTOM_SND_ELECTRO4, RANDOM_FLOAT(0.7, 0.8), WEAPONCUSTOM_SOUND, CHAN_WEAPON, 0, 0 ); break;
		case 1:
				PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), g_evWeaponCustom, 0.0, (float *) &g_vecZero, (float *) &g_vecZero,
				                     WEAPONCUSTOM_SND_ELECTRO5, RANDOM_FLOAT(0.7, 0.8), WEAPONCUSTOM_SOUND, CHAN_WEAPON, 0, 0 ); break;
		case 2:
				PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), g_evWeaponCustom, 0.0, (float *) &g_vecZero, (float *) &g_vecZero,
				                     WEAPONCUSTOM_SND_ELECTRO6, RANDOM_FLOAT(0.7, 0.8), WEAPONCUSTOM_SOUND, CHAN_WEAPON, 0, 0 ); break;
		case 3:	break; // no sound
		}

		m_flPlayAftershock = 0.0;
		m_fAftershockActive = false;
	}

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	if (m_fInAttack != 0)
	{
		StartFire();
		m_fInAttack = 0;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0;
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
		return;
	}

	int iAnim;
		float flRand = UTIL_SharedRandomFloat( m_hPlayer->random_seed, 0, 1);
		if (flRand <= 0.5)
		{
			iAnim = GAUSS_IDLE;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );
		}
		else if (flRand <= 0.75)
		{
			iAnim = GAUSS_IDLE2;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );
		}
		else
		{
			iAnim = GAUSS_FIDGET;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.25;
	}

	SendWeaponAnim( iAnim );

	}

// Plays the discharge of an overcharged gauss on the local player
void CGauss::ItemPostFrame( void )
{
#ifdef CLIENT_DLL
	if ( m_fOvercharged && FRunfuncs() )
	{
		m_fOvercharged = false;

		int iPlayer = gEngfuncs.GetLocalPlayer()->index;

		CClient_SoundEngine::GetSoundEngine()->PlayFMODSound( 0, iPlayer, g_vecZero, CHAN_WEAPON, "weapons/electro4.wav",
		                                                      1.0, ATTN_NORM, 0, 80 + UTIL_SharedRandomLong( m_hPlayer->random_seed, 0, 0x3f ), -1, 0 );
		CClient_SoundEngine::GetSoundEngine()->PlayFMODSound( 0, iPlayer, g_vecZero, CHAN_ITEM, "weapons/electro6.wav",
		                                                      1.0, ATTN_NORM, 0, 75 + UTIL_SharedRandomLong( m_hPlayer->random_seed, 0, 0x3f ), -1, 0 );
	}

#endif

	CBasePlayerWeapon::ItemPostFrame();
}

void CGauss::CustomDecrement( float flDecrement )
{
	if ( m_fAmmoBurnActive )
		m_flNextAmmoBurn = V_max( m_flNextAmmoBurn - flDecrement, -14.5 );

	if ( m_fAftershockActive )
		m_flPlayAftershock = V_max( m_flPlayAftershock - flDecrement, -14.5 );

	if ( m_fStartChargeActive )
		m_flStartCharge = V_max( m_flStartCharge - flDecrement, -14.5 );
}

void CGauss::GetWeaponData( weapon_data_t *data )
{
	data->iuser1 = m_iPrimaryAmmoType;
	data->iuser2 = m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType];
	data->iuser3 = m_iSecondaryAmmoType;
	data->iuser4 = m_hPlayer->m_rgAmmo[m_iSecondaryAmmoType];
	data->fuser1 = m_flNextAmmoBurn;

	if ( m_fOvercharged || m_fInAttack )
	{
		data->fuser2 = m_flStartCharge;
		m_fOvercharged = false;
	}
	else
	{
		data->fuser2 = UTIL_WeaponTimeBase();
	}

	data->fuser3 = m_flPlayAftershock;
	data->fuser4 = m_fInAttack;
}

void CGauss::SetWeaponData( weapon_data_t *data )
{
	m_iPrimaryAmmoType = data->iuser1;
	m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] = data->iuser2;
	m_iSecondaryAmmoType = data->iuser3;
	m_hPlayer->m_rgAmmo[m_iSecondaryAmmoType] = data->iuser4;
	m_flNextAmmoBurn = data->fuser1;
	m_flStartCharge = data->fuser2;
	m_flPlayAftershock = data->fuser3;
	m_fInAttack = (int) data->fuser4;

	if ( UTIL_WeaponTimeBase() - 10 > m_flStartCharge )
	{
		// the server zapped the player for charging too long
		m_fOvercharged = true;
		m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase();
	}
}

#ifndef CLIENT_DLL
class CGaussAmmo : public CPlayerAmmo
{
public:
	CGaussAmmo() : CPlayerAmmo( AMMO_URANIUMBOX_GIVE, "uranium", URANIUM_MAX_CARRY ) {}

	const char *GetDefaultModel( void ) { return GaussClassicMode() ? "models/hlclassic/w_gaussammo.mdl" : "models/w_gaussammo.mdl"; }
	const char *GetPickupSound( void ) { return GaussClassicMode() ? "hlclassic/items/9mmclip1.wav" : "items/9mmclip1.wav"; }
};

LINK_ENTITY_TO_CLASS( ammo_gaussclip, CGaussAmmo );
#endif
