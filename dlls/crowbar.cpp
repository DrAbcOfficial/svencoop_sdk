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

#include "extdll.h"
#include "util.h"
#include "portal_trace.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "CClassicMode.h"
#include "game.h"
#include "user_messages.h"

#define CBELEC_ON 0x40 // gmsgCbElec: the entity's electric effect is on

#define	CROWBAR_BODYHIT_VOLUME 128
#define	CROWBAR_WALLHIT_VOLUME 512

LINK_ENTITY_TO_CLASS( weapon_crowbar, CCrowbar );

enum crowbar_e
{
	CROWBAR_IDLE = 0,
	CROWBAR_DRAW,
	CROWBAR_HOLSTER,
	CROWBAR_ATTACK1HIT,
	CROWBAR_ATTACK1MISS,
	CROWBAR_ATTACK2MISS,
	CROWBAR_ATTACK2HIT,
	CROWBAR_ATTACK3MISS,
	CROWBAR_ATTACK3HIT,
	CROWBAR_IDLE2,
	CROWBAR_IDLE3
};

void CCrowbar::Spawn( void )
{
	Precache( );
	m_iId = WEAPON_CROWBAR;

	const char *model;

	if ( CClassicMode::GetInstance()->IsEnabled() )
		model = "models/hlclassic/w_crowbar.mdl";
	else
		model = "models/w_crowbar.mdl";

	SetModel_Intercept( edict(), GetW_Model( model ) );
	m_iClip = WEAPON_NOCLIP;
	m_flCustomDmg = pev->dmg;
	CBasePhysicsObject::Spawn();// get ready to fall down.
}

void CCrowbar::OnCreate( void )
{
	CBasePlayerWeapon::OnCreate();
	m_iBounceMaterial = 1;
}

void CCrowbar::Precache( void )
{
	PrecacheCustomModels();

	bool classic = CClassicMode::GetInstance()->IsEnabled();

	UTIL_PrecacheModel( this, classic ? "models/hlclassic/v_crowbar.mdl" : "models/v_crowbar.mdl");
	UTIL_PrecacheModel( this, classic ? "models/hlclassic/w_crowbar.mdl" : "models/w_crowbar.mdl");
	UTIL_PrecacheModel( this, classic ? "models/hlclassic/p_crowbar.mdl" : "models/p_crowbar.mdl");

	const char *sounds[] = {
		"weapons/cbar_hit1.wav", "weapons/cbar_hit2.wav",
		"weapons/cbar_hitbod1.wav", "weapons/cbar_hitbod2.wav", "weapons/cbar_hitbod3.wav",
		"weapons/cbar_miss1.wav", "weapons/cbe/cbe_idle1.wav",
		"weapons/cbe/cbe_hit1.wav", "weapons/cbe/cbe_hit2.wav",
		"weapons/cbe/cbe_hitbod1.wav", "weapons/cbe/cbe_hitbod2.wav", "weapons/cbe/cbe_hitbod3.wav",
		"weapons/cbe/cbe_miss1.wav", "weapons/cbe/cbe_miss2.wav",
		"weapons/cbe/cbe_off.wav", "weapons/cbe/cbe_on.wav"
	};

	for ( int i = 0; i < ARRAYSIZE( sounds ); ++i )
		PrecacheSound_Intercept( this, sounds[i] );
}

bool CCrowbar::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo1Entity = NULL;
	p->iAmmo1Drop = AMMO_DROP_NONE;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo2Entity = NULL;
	p->iAmmo2Drop = AMMO_DROP_NONE;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 0;
	p->iPosition = 0;
	p->iId = WEAPON_CROWBAR;
	p->iFlags = ITEM_FLAG_ESSENTIAL;
	p->iWeight = CROWBAR_WEIGHT;
	return true;
}

bool CCrowbar::AddToPlayer( CBasePlayer *player )
{
	if ( !CBasePlayerWeapon::AddToPlayer( player ) )
		return false;

	AddToPlayerMessage();
	return true;
}

bool CCrowbar::Deploy( void )
{
	TurnElectricOff();
	m_fThrowPending = false;

	bool classic = CClassicMode::GetInstance()->IsEnabled();

	return DefaultDeploy( GetV_Model( classic ? "models/hlclassic/v_crowbar.mdl" : "models/v_crowbar.mdl" ),
	                      GetP_Model( classic ? "models/hlclassic/p_crowbar.mdl" : "models/p_crowbar.mdl" ), CROWBAR_DRAW, "crowbar" );
}

void CCrowbar::Holster( int skiplocal /* = 0 */ )
{
	m_fInReload = false;
	TurnElectricOff();
	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	m_hPlayer->pev->viewmodel = 0;
}


void FindHullIntersection( const Vector &vecSrc, TraceResult &tr, float *mins, float *maxs, edict_t *pEntity )
{
	int			i, j, k;
	float		distance;
	float		*minmaxs[2] = {mins, maxs};
	TraceResult tmpTrace;
	Vector		vecHullEnd = tr.vecEndPos;
	Vector		vecEnd;

	distance = 1e6f;

	vecHullEnd = vecSrc + ((vecHullEnd - vecSrc)*2);
	UTIL_TraceLine( vecSrc, vecHullEnd, dont_ignore_monsters, pEntity, &tmpTrace );
	if ( tmpTrace.flFraction < 1.0 )
	{
		tr = tmpTrace;
		return;
	}

	for ( i = 0; i < 2; i++ )
	{
		for ( j = 0; j < 2; j++ )
		{
			for ( k = 0; k < 2; k++ )
			{
				vecEnd.x = vecHullEnd.x + minmaxs[i][0];
				vecEnd.y = vecHullEnd.y + minmaxs[j][1];
				vecEnd.z = vecHullEnd.z + minmaxs[k][2];

				UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, pEntity, &tmpTrace );
				if ( tmpTrace.flFraction < 1.0 )
				{
					float thisDistance = (tmpTrace.vecEndPos - vecSrc).Length();
					if ( thisDistance < distance )
					{
						tr = tmpTrace;
						distance = thisDistance;
					}
				}
			}
		}
	}
}

void CCrowbar::PrimaryAttack( void )
{
	if (!( m_flNextTertiaryAttack <= gpGlobals->time ) )
		return;

	if ( m_fThrowPending )
	{
		TertiaryAttack();
		return;
	}

	if ( !Swing( true ))
	{
		SetThink( &CCrowbar::SwingAgain );
		pev->nextthink = gpGlobals->time + 0.1;
	}
}


void CCrowbar::Smack( )
{
	DecalGunshot( &m_trHit, BULLET_PLAYER_CROWBAR );
}


void CCrowbar::SwingAgain( void )
{
	Swing( 0 );
}

bool CCrowbar::Swing( bool fFirst )
{
	PortalTraceResult tr;

	UTIL_MakeVectors (m_hPlayer->pev->v_angle);
	Vector vecSrc	= m_hPlayer->GetGunPosition( );
	UTIL_TracePortal( vecSrc, gpGlobals->v_forward, 32, dont_ignore_monsters, dont_ignore_glass,
	                  m_hPlayer->edict(), m_hPlayer, &tr );

	if ( tr.flFraction >= 1.0 )
	{
		for ( unsigned int i = 0; i < tr.m_Steps.size(); ++i )
		{
			PortalTraceStep *step = tr.m_Steps[i];

			edict_t *ignore;

			if ( step->m_iStep )
				ignore = NULL;
			else
				ignore = m_hPlayer->edict();

			UTIL_TraceHull( step->m_vecStart, step->m_vecEnd, dont_ignore_monsters, head_hull, ignore, &tr );
		if ( tr.flFraction < 1.0 )
		{
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			CBaseEntity *target = CBaseEntity::Instance( tr.pHit );
			if ( !target || target->IsBSPModel() )
					UTIL_FindHullIntersection( step->m_vecStart, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX,
					                           ignore, 1000000.0f );	// This is the point on the actual surface (the hull could have hit space)
		}
	}
	}

	if ( tr.flFraction >= 1.0 )
	{
		if (fFirst)
		{
			int swing = m_iSwing;

			m_iSwing = (int) ( (unsigned int) m_iSwing + 1 );

			switch ( swing % 3 )
		{
		case 0:
			SendWeaponAnim( CROWBAR_ATTACK1MISS, 0, 0 ); break;
		case 1:
			SendWeaponAnim( CROWBAR_ATTACK2MISS, 0, 0 ); break;
		case 2:
			SendWeaponAnim( CROWBAR_ATTACK3MISS, 0, 0 ); break;
		}

			m_flNextPrimaryAttack = gpGlobals->time + 0.5f;

		if ( m_fElectric )
			{
				const char *sample;

				if ( RANDOM_LONG(0,1) )
					sample = "weapons/cbe/cbe_miss2.wav";
				else
					sample = "weapons/cbe/cbe_miss1.wav";

				EMIT_SOUND_DYN(m_hPlayer->edict(), CHAN_ITEM, sample, 1, ATTN_NORM, 0, 98 + RANDOM_LONG(0,3), 0, false, g_vecZero );
			}
			else
			{
				EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_WEAPON, "weapons/cbar_miss1.wav", 1, ATTN_NORM, 0, 94 + RANDOM_LONG( 0, 15 ), 0, false, g_vecZero );
			}

			m_hPlayer->SetAnimation( PLAYER_ATTACK1 );
		}

		m_flTimeWeaponIdle = m_flNextPrimaryAttack + 0.5f;
		return false;
	}

	CBaseEntity *target = CBaseEntity::Instance( tr.pHit );
	int swing = m_iSwing;

	m_iSwing = (int) ( (unsigned int) m_iSwing + 1 );

	switch ( swing % 3 )
	{
		case 0:
			SendWeaponAnim( CROWBAR_ATTACK1HIT, 0, 0 );
				break;
			case 1:
			SendWeaponAnim( CROWBAR_ATTACK2HIT, 0, 0 );
			break;
		case 2:
			SendWeaponAnim( CROWBAR_ATTACK3HIT, 0, 0 );
			break;
	}

	m_hPlayer->SetAnimation( PLAYER_ATTACK1 );

	float health = target->pev->health;

	float damage;

	if ( m_flCustomDmg > 0 )
		damage = m_flCustomDmg;
	else
		damage = sk_plr_crowbar.value;

	ClearMultiDamage();

	if ( m_fElectric )
		damage += damage;
	else if ( gpGlobals->time <= m_flNextPrimaryAttack + 1.0f )
		damage *= 0.5f;

	target->TraceAttack(m_hPlayer->pev, damage, tr.m_vecDirection, &tr, DMG_CLUB );
	ApplyMultiDamage( m_hPlayer->pev, m_hPlayer->pev );
	m_flNextPrimaryAttack = gpGlobals->time + 0.3;

	float volume = 1.0f;
	bool hitWorld = true;

	if ( target->Classify() != CLASS_NONE && target->Classify() != CLASS_MACHINE && target->BloodColor() != DONT_BLEED )
	{
		if ( target->IsPlayer() )
			target->pev->velocity = target->pev->velocity + ( pev->origin - target->pev->origin ).Normalize() * 120;

		int sound = RANDOM_LONG( 0, 2 );
		const char *sample;

		if ( m_fElectric )
			sample = sound == 0   ? "weapons/cbe/cbe_hitbod1.wav"
			         : sound == 1 ? "weapons/cbe/cbe_hitbod2.wav"
			                      : "weapons/cbe/cbe_hitbod3.wav";
		else
			sample = sound == 0   ? "weapons/cbar_hitbod1.wav"
			         : sound == 1 ? "weapons/cbar_hitbod2.wav"
			                      : "weapons/cbar_hitbod3.wav";

		EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_WEAPON, sample, 1, ATTN_NORM, 0, 100, 0, false, g_vecZero );
		m_hPlayer->m_iWeaponVolume = CROWBAR_BODYHIT_VOLUME;

		if ( !target->IsAlive() )
			return true;

		volume = 0.1f;
		hitWorld = false;
	}

	if ( m_fElectric )
	{
		if ( target->IsPlayer() )
			m_hPlayer->pev->armorvalue -= 1;
		else if ( health != target->pev->health )
			m_hPlayer->pev->armorvalue -= 3;
	}

	if ( hitWorld )
	{
		PortalTraceStep *step = tr.m_Steps.back();
		float strikeVolume = TEXTURETYPE_PlaySound( &tr, step->m_vecStart,
		                                            step->m_vecStart + ( step->m_vecEnd - step->m_vecStart ) * 2, BULLET_PLAYER_CROWBAR );
		int sound = RANDOM_LONG( 0, 1 );
		const char *sample;

		m_flNextPrimaryAttack = gpGlobals->time + 0.25f;

		if ( m_fElectric )
			sample = sound ? "weapons/cbe/cbe_hit2.wav" : "weapons/cbe/cbe_hit1.wav";
		else
			sample = sound ? "weapons/cbar_hit2.wav" : "weapons/cbar_hit1.wav";

		EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_WEAPON, sample, strikeVolume, ATTN_NORM, 0, 98 + RANDOM_LONG(0,3), 0, false, g_vecZero );
			}

			// delay the decal a bit
			m_trHit = tr;
	SetThink( m_fElectric ? &CCrowbar::ElectricSmack : &CCrowbar::Smack );
		pev->nextthink = gpGlobals->time + 0.2;
	m_hPlayer->m_iWeaponVolume = (int) ( volume * CROWBAR_WALLHIT_VOLUME );
	m_flTimeWeaponIdle = m_flNextPrimaryAttack + 0.5f;
	return true;
}

void CCrowbar::SecondaryAttack( void )
{
	if ( !( m_flNextTertiaryAttack <= gpGlobals->time ) )
		return;

	if ( m_fThrowPending )
	{
		TertiaryAttack();
		return;
	}

	if ( !CanHaveXtra( XTRA_ELECTRIC_CROWBAR, m_hPlayer ) )
	{
		TurnElectricOff();
		m_flNextSecondaryAttack = gpGlobals->time + 2.0f;
		return;
	}

	if ( m_hPlayer->pev->waterlevel > 0 || m_hPlayer->pev->armorvalue < 3 )
	{
		TurnElectricOff();
		m_flNextSecondaryAttack = gpGlobals->time + 0.5f;
		return;
	}

	ToggleElectric();
	m_flNextSecondaryAttack = gpGlobals->time + 0.2;
}

void CCrowbar::TurnElectricOn( void )
{
	if ( m_fElectric || ( m_hPlayer->pev->flags & FL_INWATER ) )
	{
		TurnElectricOff();
		return;
	}

	m_fElectric = true;
	MESSAGE_BEGIN( MSG_ALL, gmsgCbElec );
	WRITE_BYTE( m_hPlayer->entindex() | CBELEC_ON );
	MESSAGE_END();
}

void CCrowbar::TurnElectricOff( void )
{
	if ( !m_fElectric )
		return;

	m_fElectric = false;
	MESSAGE_BEGIN( MSG_ALL, gmsgCbElec );
	WRITE_BYTE( m_hPlayer->entindex() );
	MESSAGE_END();
}

void CCrowbar::ToggleElectric( void )
{
	if ( m_fElectric )
		TurnElectricOff();
	else
		TurnElectricOn();
}

void CCrowbar::ItemPreFrame( void )
{
	if ( ( m_hPlayer->pev->flags & FL_INWATER ) || m_hPlayer->pev->armorvalue < 3 )
		TurnElectricOff();
}

void CCrowbar::ElectricSmack( void )
{
	UTIL_Sparks( m_trHit.vecEndPos );
	Smack();
}

void CCrowbar::TertiaryAttack( void )
{
	if ( !m_hPlayer->CanDropPlayerItem( NULL ) || !( m_flNextTertiaryAttack <= gpGlobals->time ) )
		return;

	if ( m_fThrowPending )
	{
		LaunchCrowbar();
		return;
	}

	SendWeaponAnim( CROWBAR_ATTACK1HIT, 0, 0 );
	m_fThrowPending = true;
	m_flTimeWeaponIdle = gpGlobals->time + 0.25f;
	m_flNextTertiaryAttack = gpGlobals->time + 0.25f;
}

void CCrowbar::WeaponIdle( void )
{
	if ( !( m_flTimeWeaponIdle <= gpGlobals->time ) )
		return;

	if ( m_fThrowPending )
	{
		LaunchCrowbar();
		return;
	}

	int animation = RANDOM_LONG( 0, 2 );

	SendWeaponAnim( animation == 0 ? CROWBAR_IDLE : animation == 1 ? CROWBAR_IDLE2
	                                                               : CROWBAR_IDLE3,
	                0, 0 );
	m_flTimeWeaponIdle = gpGlobals->time + ( animation == 0 ? 2.7f : 5.4f );
}

void CCrowbar::LaunchCrowbar( void )
{
	UTIL_MakeVectors( m_hPlayer->pev->v_angle + m_hPlayer->pev->punchangle );

	Vector origin = m_hPlayer->pev->origin + gpGlobals->v_forward * 10 + gpGlobals->v_up * 6 + gpGlobals->v_right * 20;
	float yaw = m_hPlayer->pev->angles.y;
	edict_t *owner = m_hPlayer->edict();

	DetachFromPlayer();
	m_iUnknown180 = 1;
	Dropped( this );
	pev->owner = owner;
	pev->origin = origin;
	pev->angles = Vector( 90, yaw, -90 );
	pev->velocity = gpGlobals->v_forward * 1200;
	pev->avelocity = Vector( -800, 0, 0 );
	pev->spawnflags |= SF_DODAMAGE;
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;
	m_fThrowActive = true;
	UTIL_SetOrigin( pev, pev->origin );
	SetThink( &CCrowbar::ThrownThink );
	SetTouch( &CCrowbar::ThrownTouch );
	m_fThrowPending = false;
}

void CCrowbar::ThrownThink( void )
{
	pev->nextthink = gpGlobals->time + 0.1;

	if ( pev->movetype == MOVETYPE_NONE || pev->movetype == MOVETYPE_FLY || pev->movetype == MOVETYPE_NOCLIP )
		return;

	if ( ( pev->spawnflags & SF_DODAMAGE ) && m_fThrowActive && gpGlobals->time > m_flNextThrowSound )
	{
		EMIT_SOUND_DYN( edict(), CHAN_VOICE, "weapons/cbar_miss1.wav", 0.25f, ATTN_NORM, 0, 100, 0, false, g_vecZero );
		m_flNextThrowSound = gpGlobals->time + 0.2;
	}

	CBasePhysicsObject::FallThink();
}

void CCrowbar::ThrownTouch( CBaseEntity *other )
{
	if ( ( pev->spawnflags & SF_DODAMAGE ) && pev->velocity.Length() > 100 )
	{
		TraceResult trace = UTIL_GetGlobalTrace();

		if ( other->IsBSPModel() && m_fThrowActive )
			UTIL_Sparks( trace.vecEndPos );

		if ( other->pev->flags & ( FL_MONSTER | FL_CLIENT ) )
		{
			if ( other->pev->flags & FL_CLIENT )
			{
				CBaseMonster *monster = other->MyMonsterPointer();

				if ( !monster->CheckAttacker( CBaseEntity::Instance( pev->owner ) ) )
					monster->SetConditions( bits_COND_HEAVY_DAMAGE );
			}

			char sample[64];

			sprintf( sample, "weapons/cbar_hitbod%d.wav", RANDOM_LONG( 0, 2 ) + 1 );
			EMIT_SOUND_DYN( edict(), CHAN_VOICE, sample, 0.9f, 2.0f, 0, 100, 0, false, g_vecZero );
			pev->velocity = pev->velocity * 0.3f;
		}

		ClearMultiDamage();
		other->TraceAttack( pev->owner ? VARS( pev->owner ) : NULL, sk_plr_crowbar.value * 2,
		                    pev->velocity.Normalize(), &trace, DMG_CLUB | DMG_NEVERGIB );
		ApplyMultiDamage( pev, pev->owner ? VARS( pev->owner ) : NULL );
		m_fThrowActive = false;
		pev->spawnflags &= ~SF_DODAMAGE;
	}

	CBasePickupObject::DefaultTouch( other );
}
