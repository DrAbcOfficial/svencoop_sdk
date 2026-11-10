/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
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
#if !defined( OEM_BUILD ) && !defined( HLDEMO_BUILD )

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "monsters.h"
#include "weapons.h"
#include "effects.h"
#include "gamerules.h"
#include "CClassicMode.h"
#include "egon.h"
#include "portal_trace.h"

#include "playerammo.h"
#include "customentity.h"

extern cvar_t sk_plr_egon_wide;

#define EGON_PRIMARY_VOLUME 450
#define EGON_BEAM_SPRITE "sprites/xbeam1.spr"
#define EGON_FLARE_SPRITE "sprites/XSpark1.spr"
#define EGON_SOUND_OFF "weapons/egon_off1.wav"
#define EGON_SOUND_RUN "weapons/egon_run3.wav"
#define EGON_SOUND_STARTUP "weapons/egon_windup2.wav"
#define EGON_BEAM_FLAGS BEAM_FSINE

enum egon_e
{
	EGON_IDLE1 = 0,
	EGON_FIDGET1,
	EGON_ALTFIREON,
	EGON_ALTFIRECYCLE,
	EGON_ALTFIREOFF,
	EGON_FIRE1,
	EGON_FIRE2,
	EGON_FIRE3,
	EGON_FIRE4,
	EGON_DRAW,
	EGON_HOLSTER
};

LINK_ENTITY_TO_CLASS( weapon_egon, CEgon );

void CEgon::Spawn( void )
{
	Precache( );

	const char *model;

	if ( CClassicMode::GetInstance()->IsEnabled() )
		model = "models/hlclassic/w_egon.mdl";
	else
		model = "models/w_egon.mdl";

	SetModel_Intercept( edict(), GetW_Model( model ) );

	m_iDefaultAmmo = EGON_DEFAULT_GIVE;
	m_iId = WEAPON_EGON;
	m_flCustomDmg = pev->dmg;
	CBasePhysicsObject::Spawn();// get ready to fall down.
}

void CEgon::Precache( void )
{
	PrecacheCustomModels();

	bool classic = CClassicMode::GetInstance()->IsEnabled();

	UTIL_PrecacheModel( this, classic ? "models/hlclassic/w_egon.mdl" : "models/w_egon.mdl");
	UTIL_PrecacheModel( this, classic ? "models/hlclassic/v_egon.mdl" : "models/v_egon.mdl");
	UTIL_PrecacheModel( this, classic ? "models/hlclassic/p_egon.mdl" : "models/p_egon.mdl");
	UTIL_PrecacheModel( this, EGON_BEAM_SPRITE );
	UTIL_PrecacheModel( this, EGON_FLARE_SPRITE );
	PrecacheSound_Intercept( this, "items/9mmclip1.wav");
	PrecacheSound_Intercept( this, EGON_SOUND_OFF );
	PrecacheSound_Intercept( this, EGON_SOUND_RUN );
	PrecacheSound_Intercept( this, EGON_SOUND_STARTUP );
	PrecacheSound_Intercept( this, "weapons/357_cock1.wav");
}

bool CEgon::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "uranium";
	p->iMaxAmmo1 = URANIUM_MAX_CARRY;
	p->pszAmmo1Entity = "ammo_gaussclip";
	p->iAmmo1Drop = 20;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo2Entity = NULL;
	p->iAmmo2Drop = AMMO_DROP_NONE;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 3;
	p->iPosition = 2;
	p->iId = m_iId = WEAPON_EGON;
	p->iFlags = 0;
	p->iWeight = EGON_WEIGHT;

	return true;
}

bool CEgon::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( !CBasePlayerWeapon::AddToPlayer( pPlayer ) )
		return false;

	AddToPlayerMessage();
		return TRUE;
	}

bool CEgon::Deploy( void )
{
	m_deployed = FALSE;

	bool classic = CClassicMode::GetInstance()->IsEnabled();
	return DefaultDeploy( GetV_Model( classic ? "models/hlclassic/v_egon.mdl" : "models/v_egon.mdl" ),
	                      GetP_Model( classic ? "models/hlclassic/p_egon.mdl" : "models/p_egon.mdl" ), EGON_DRAW, "egon" );
}

void CEgon::Holster( int skiplocal /* = 0 */ )
{
	m_fInReload = false;
	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;

	if ( m_fireState != FIRE_OFF )
		EndAttack();

	m_hPlayer->pev->viewmodel = 0;
}

float CEgon::GetPulseInterval( void )
{
	return 0.1f;
}

float CEgon::GetDischargeInterval( void )
{
	g_pGameRules->IsMultiplayer();
	return 0.1f;
}

float CEgon::GetTimeDist( void )
{
	float time = ( pev->dmgtime - gpGlobals->time ) / GetDischargeInterval();

	if ( time < 0 )
		time = 0;
	else if ( time > 1 )
		time = 1;

	return 1.0f - time;
}

BOOL CEgon::HasAmmo( void )
{
	return m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0;
}

void CEgon::UseAmmo( int count )
{
	if ( m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] < count )
		m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] = 0;
	else
		m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= count;
}

void CEgon::PrimaryAttack( void )
{
	Attack();

}

void CEgon::SecondaryAttack( void )
{
	WeaponIdle();
}

void CEgon::TertiaryAttack( void )
{
	WeaponIdle();
}

void CEgon::Attack( void )
{
	// don't fire underwater
	if ( m_hPlayer->pev->waterlevel == 3 )
	{

		if ( m_Beams.empty() )
			PlayEmptySound();
		else
			EndAttack();

		return;
	}

	UTIL_MakeVectors( m_hPlayer->pev->v_angle + m_hPlayer->pev->punchangle );
	Vector vecAiming = gpGlobals->v_forward;
	Vector vecSrc	 = m_hPlayer->GetGunPosition( );

	if ( m_fireState == FIRE_OFF )
	{
			if ( !HasAmmo() )
			{
				m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.25;
				PlayEmptySound( );
				return;
			}

			m_flAmmoUseTime = gpGlobals->time;
		SendWeaponAnim( EGON_FIRE1 + RANDOM_LONG( 0, 3 ), 0, 0 );

			m_shakeTime = 0;

			m_hPlayer->m_iWeaponVolume = EGON_PRIMARY_VOLUME;
			m_flTimeWeaponIdle = gpGlobals->time + 0.1;
		m_shootTime = gpGlobals->time + 2.0f;
		EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_WEAPON, EGON_SOUND_STARTUP, 0.98f, ATTN_NORM,
		                0, 125, 0, false, g_vecZero );

			pev->dmgtime = gpGlobals->time + GetDischargeInterval();
			m_fireState = FIRE_CHARGE;
		}
	else if ( m_fireState == FIRE_CHARGE )
	{
			Fire( vecSrc, vecAiming );
			m_hPlayer->m_iWeaponVolume = EGON_PRIMARY_VOLUME;

			if ( m_shootTime != 0 && gpGlobals->time > m_shootTime )
		{
			EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_STATIC, EGON_SOUND_RUN, 0.98f, ATTN_NORM, 0, 125, 0, false, g_vecZero );
			m_shootTime = 0;
			}

			if ( !HasAmmo() )
			{
				EndAttack();
			m_fireState = FIRE_OFF;
			m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 1.0;
			}

		}

	if ( m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		m_hPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );
}

void CEgon::Fire( const Vector &vecOrigSrc, const Vector &vecDir )
{
	PortalTraceResult tr;

	UTIL_TracePortal( vecOrigSrc, vecDir, 2048, dont_ignore_monsters, dont_ignore_glass,
	                  m_hPlayer->edict(), m_hPlayer, &tr, NULL );

	if (tr.fAllSolid)
	{
		DestroyEffect();
		return;
	}

	UpdateEffect( tr );

	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

	if ( !( gpGlobals->time > pev->dmgtime ) )
		return;

	float damage = tr.m_flScale * ( m_flCustomDmg > 0.0f ? m_flCustomDmg * 2.4f : sk_plr_egon_wide.value );

	ClearMultiDamage();

	if ( pEntity && pEntity->pev->takedamage )
		pEntity->TraceAttack( m_hPlayer->pev, damage, tr.m_vecDirection, &tr, DMG_ENERGYBEAM | DMG_ALWAYSGIB );

	ApplyMultiDamage(m_hPlayer->pev, m_hPlayer->pev);

			if ( tr.flFraction < 1.0f )
		::RadiusDamage( tr.vecEndPos, pev, m_hPlayer->pev, damage * 0.25f, 128, CLASS_NONE, DMG_ENERGYBEAM | DMG_BLAST | DMG_ALWAYSGIB );

	if ( !m_hPlayer->IsAlive() )
				return;

	if ( gpGlobals->time >= m_flAmmoUseTime )
				{
					UseAmmo( 1 );
					m_flAmmoUseTime = gpGlobals->time + 0.2;
				}

	if ( tr.flFraction < 1.0f && gpGlobals->time > m_shakeTime )
	{
		UTIL_ScreenShake( tr.vecEndPos, 5, 150, 0.75f, 250 );
		m_shakeTime = gpGlobals->time + 1.5f;
	}

			pev->dmgtime = gpGlobals->time + GetDischargeInterval();
}

void CEgon::CreateBeams( unsigned int count )
{
	while ( m_Beams.size() < count )
	{
		CBeam *beam = CBeam::BeamCreate( EGON_BEAM_SPRITE, 40 );

		if ( m_Beams.empty() )
		{
			beam->PointEntInit( pev->origin, m_hPlayer->entindex() );
			beam->SetEndAttachment( 1 );
		}
		else
		{
			beam->PointsInit( pev->origin, pev->origin );
		}

		beam->SetFlags( EGON_BEAM_FLAGS );
		beam->pev->spawnflags |= SF_BEAM_TEMPORARY;
		beam->SetScrollRate( 50 );
		beam->SetNoise( 20 );

		CEntityHandle<CBeam> handle;

		handle = beam;
		m_Beams.push_back( handle );
	}

	while ( m_Noise.size() < count )
	{
		CBeam *beam = CBeam::BeamCreate( EGON_BEAM_SPRITE, 55 );

		if ( m_Noise.empty() )
		{
			beam->PointEntInit( pev->origin, m_hPlayer->entindex() );
			beam->SetEndAttachment( 1 );
		}
		else
		{
			beam->PointsInit( pev->origin, pev->origin );
		}

		beam->SetScrollRate( 25 );
		beam->SetBrightness( 150 );
		beam->pev->spawnflags |= SF_BEAM_TEMPORARY;
		beam->SetColor( 80, 80, 255 );
		beam->SetNoise( 8 );

		CEntityHandle<CBeam> handle;

		handle = beam;
		m_Noise.push_back( handle );
	}

	if ( count && !m_Sprite )
	{
		m_Sprite = CSprite::SpriteCreate( EGON_FLARE_SPRITE, pev->origin, FALSE );
		m_Sprite->SetScale( 1.0f );
		m_Sprite->SetTransparency( kRenderGlow, 255, 255, 255, 255, kRenderFxNoDissipation );
		m_Sprite->pev->spawnflags |= SF_SPRITE_TEMPORARY;
	}
}

void CEgon::DestroyEffect( unsigned int count )
{
	while ( count < m_Beams.size() )
	{
		UTIL_Remove( m_Beams.back() );
		m_Beams.pop_back();
	}

	while ( count < m_Noise.size() )
	{
		UTIL_Remove( m_Noise.back() );
		m_Noise.pop_back();
	}
	if ( count == 0 && m_Sprite )
	{
		m_Sprite->Expand( 6, 550 );
		m_Sprite = NULL;
	}
}

void CEgon::UpdateEffect( PortalTraceResult &trace )
{
	unsigned int count = (unsigned int) trace.m_Steps.size();

	CreateBeams( count );
	DestroyEffect( count );

	float blend = GetTimeDist();

	for ( unsigned int i = 0; i < count; ++i )
	{
		PortalTraceStep *step = trace.m_Steps[i];
		CBeam *beam = m_Beams[step->m_iStep];

		if ( beam )
		{
			beam->SetStartPos( step->m_vecEnd );

			if ( step->m_iStep > 0 )
				beam->SetEndPos( step->m_vecStart );

			beam->SetBrightness( (int) ( 255 - blend *180) );
			beam->SetWidth( (int) ( ( 40 - blend * 20 ) * step->m_flScale ) );
			beam->SetNoise( (int) ( (float) sqrt( (double) step->m_flScale ) *20) );
			beam->SetColor( (int) ( blend * 32 + 64 ), (int) ( blend * 38 + 64 ),
			                (int) ( fabs( (float) sin(gpGlobals->time* 10.0f ) ) * 92 + 100 ) );
		}

		beam = m_Noise[step->m_iStep];

		if ( beam )
		{
			beam->SetStartPos( step->m_vecEnd );

			if ( step->m_iStep > 0 )
				beam->SetEndPos( step->m_vecStart );

			beam->SetWidth( (int) ( step->m_flScale * 55 ) );
			beam->SetNoise( (int) ( (float) sqrt( (double) step->m_flScale ) * 8 ) );
		}
	}

	if ( m_Sprite )
	{
		UTIL_SetOrigin( m_Sprite->pev, trace.vecEndPos );
		m_Sprite->SetScale( trace.m_flScale );
		m_Sprite->pev->frame += gpGlobals->frametime * 8;
	if ( m_Sprite->pev->frame >= m_Sprite->Frames() + 1 )
			m_Sprite->pev->frame = 0;
	}
}

void CEgon::EndAttack( void )
{
	EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_STATIC, EGON_SOUND_RUN, 0, 0,
	                SND_STOP, PITCH_NORM, 0, false, g_vecZero );
	EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_WEAPON, EGON_SOUND_OFF, 0.98f, ATTN_NORM,
	                0, PITCH_NORM, 0, false, g_vecZero );
	m_fireState = FIRE_OFF;
	SendWeaponAnim( EGON_IDLE1, 0, 0 );

	m_flTimeWeaponIdle = gpGlobals->time + 2.0;
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.5f;

	DestroyEffect();
}

void CEgon::WeaponIdle( void )
{
	ResetEmptySound( );

	if ( !( m_flTimeWeaponIdle <= gpGlobals->time ) )
		return;

	if ( m_fireState != FIRE_OFF )
		 EndAttack();

	int iAnim;

	if ( RANDOM_FLOAT(0,1) <= 0.3 )
	{
		iAnim = EGON_FIDGET1;
		m_flTimeWeaponIdle = gpGlobals->time + 3.0f;
	}
	else
	{
		iAnim = EGON_IDLE1;
		m_flTimeWeaponIdle = RANDOM_FLOAT( 10, 15 ) + gpGlobals->time;
	}

	SendWeaponAnim( iAnim, 0, 0 );
	m_deployed = TRUE;
}

class CEgonAmmo : public CPlayerAmmo
{
public:
	CEgonAmmo() : CPlayerAmmo( AMMO_URANIUMBOX_GIVE, "uranium", URANIUM_MAX_CARRY ) {}

	const char *GetDefaultModel( void ) { return "models/w_chainammo.mdl"; }
};

LINK_ENTITY_TO_CLASS( ammo_egonclip, CEgonAmmo );

#endif
