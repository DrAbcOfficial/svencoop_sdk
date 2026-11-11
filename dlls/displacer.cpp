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
#include "displacer.h"
#include "event_flags.h"

#include "game.h"
#include "effects.h"
#include "server_sound.h"
#include "teleportdestination.h"

#undef PRECACHE_MODEL
#define PRECACHE_MODEL( path ) UTIL_PrecacheModel( this, path )
#undef PRECACHE_SOUND
#define PRECACHE_SOUND( path ) PrecacheSound_Intercept( this, path )

#define SF_DISPLACER_ROTATE			128
#define SF_DISPLACER_IGNORE_DELAYS	4096
#define DISPLACER_TELEPORT_FLAGS	( SF_TELEPORT_RELATIVE | SF_TELEPORT_ROTATE | SF_TELEPORT_IGNORE_DELAYS )

LINK_ENTITY_TO_CLASS( weapon_displacer, CDisplacer );

bool CDisplacer::AllowTertiaryFire( void ) const
{
	return m_TertiaryMode == DISPLACER_TERTIARY_ENABLED
		|| ( m_TertiaryMode == 0 && weaponmode_displacer.value != 0.0f );
}

bool CDisplacer::UseDecrement( void )
{
	return true;
}


void CDisplacer::KeyValue( KeyValueData *pkvd )
{
	if ( !stricmp( "m_TertiaryMode", pkvd->szKeyName ) )
	{
		m_TertiaryMode = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( !stricmp( "m_flPortalSpeed", pkvd->szKeyName ) )
	{
		m_flPortalSpeed = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( !stricmp( "m_flPortalRadius", pkvd->szKeyName ) )
	{
		m_flPortalRadius = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( !stricmp( "m_iszTeleportDestination", pkvd->szKeyName ) )
	{
		m_iszTeleportDestination = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( !stricmp( "m_flPrimaryAmmoNeeded", pkvd->szKeyName ) )
	{
		m_flPrimaryAmmoNeeded = fabs( atof( pkvd->szValue ) );
		pkvd->fHandled = TRUE;
	}
	else if ( !stricmp( "m_flSecondaryAmmoNeeded", pkvd->szKeyName ) )
	{
		m_flSecondaryAmmoNeeded = fabs( atof( pkvd->szValue ) );
		pkvd->fHandled = TRUE;
	}
	else if ( !stricmp( "m_flTertiaryAmmoNeeded", pkvd->szKeyName ) )
	{
		m_flTertiaryAmmoNeeded = fabs( atof( pkvd->szValue ) );
		pkvd->fHandled = TRUE;
	}
	else
	{
		CBasePlayerWeapon::KeyValue( pkvd );
	}
}

void CDisplacer::Precache( void )
{
	PrecacheCustomModels();

	PRECACHE_MODEL( "models/w_displacer.mdl" );
	PRECACHE_MODEL( "models/v_displacer.mdl" );
	PRECACHE_MODEL( "models/p_displacer.mdl" );

	m_usDisplacer = PRECACHE_EVENT( 1, "events/displacer.sc" );

	PRECACHE_SOUND( "weapons/displacer_spin.wav" );
	PRECACHE_SOUND( "weapons/displacer_fire.wav" );
	PRECACHE_SOUND( "weapons/displacer_spin2.wav" );
	PRECACHE_SOUND( "weapons/displacer_self.wav" );
	PRECACHE_SOUND( "weapons/displacer_teleport_player.wav" );
	PRECACHE_SOUND( "buttons/button11.wav" );

	UTIL_PrecacheOther( "displacer_portal" );
}

void CDisplacer::Spawn( void )
{
	Precache();
	SET_MODEL( ENT( pev ), GetW_Model( "models/w_displacer.mdl" ) );

	m_iDefaultAmmo = DISPLACER_DEFAULT_GIVE;
	m_iId = WEAPON_DISPLACER;

	m_flCustomDmg = pev->dmg != 0.0f ? pev->dmg : sk_plr_displacer_other.value;

	if ( m_flPortalRadius == 0.0f )
		m_flPortalRadius = sk_plr_displacer_radius.value;

	if ( m_flPortalSpeed == 0 )
		m_flPortalSpeed = DISPLACER_PORTAL_SPEED;

	if ( m_flPrimaryAmmoNeeded == 0 )
		m_flPrimaryAmmoNeeded = DISPLACER_PRIMARY_AMMO;

	if ( m_flSecondaryAmmoNeeded == 0 )
		m_flSecondaryAmmoNeeded = DISPLACER_SECONDARY_AMMO;

	if ( m_flTertiaryAmmoNeeded == 0 )
		m_flTertiaryAmmoNeeded = DISPLACER_TERTIARY_AMMO;

	FallInit();// get ready to fall down.
}

bool CDisplacer::GetItemInfo( ItemInfo *p )
{
	p->pszName = STRING( pev->classname );
	p->pszAmmo1 = "uranium";
	p->iMaxAmmo1 = URANIUM_MAX_CARRY;
	p->pszAmmo1Entity = "ammo_gaussclip";
	p->iAmmo1Drop = DISPLACER_AMMO1_DROP;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo2Entity = NULL;
	p->iAmmo2Drop = AMMO_DROP_NONE;
	p->iMaxClip = DISPLACER_MAX_CLIP;
	p->iSlot = 5;
	p->iPosition = 3;
	p->iId = m_iId = WEAPON_DISPLACER;
	p->iFlags = 0;
	p->iWeight = DISPLACER_WEIGHT;

	return true;
}

bool CDisplacer::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		AddToPlayerMessage();
		return true;
	}

	return false;
}

CBaseEntity *CDisplacer::Respawn( void )
{
	CDisplacer *pNewWeapon = (CDisplacer *)CBasePlayerWeapon::Respawn();

	if ( pNewWeapon )
	{
		pNewWeapon->m_TertiaryMode = m_TertiaryMode;
		pNewWeapon->m_flPortalSpeed = m_flPortalSpeed;
		pNewWeapon->m_flPortalRadius = m_flPortalRadius;
		pNewWeapon->m_iszTeleportDestination = m_iszTeleportDestination;
		pNewWeapon->m_flPrimaryAmmoNeeded = m_flPrimaryAmmoNeeded;
		pNewWeapon->m_flSecondaryAmmoNeeded = m_flSecondaryAmmoNeeded;
		pNewWeapon->m_flTertiaryAmmoNeeded = m_flTertiaryAmmoNeeded;
	}

	return pNewWeapon;
}

bool CDisplacer::Deploy( void )
{
	if ( !CanDeploy() )
		return false;

	return DefaultDeploy( GetV_Model( "models/v_displacer.mdl" ), GetP_Model( "models/p_displacer.mdl" ), DISPLACER_DRAW, "egon" );
}

void CDisplacer::CancelFire( void )
{
	if ( m_iFireMode == DISPLACER_ATTACK_NONE )
		return;

	// cancel the spin up
	SendWeaponAnim( DISPLACER_IDLE1 );

	m_iFireMode = DISPLACER_ATTACK_NONE;
	SetThink( NULL );
}

void CDisplacer::Holster( int skiplocal /* = 0 */ )
{
	CancelFire();

	m_fInReload = false;// cancel any reload in progress.

	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	m_hPlayer->pev->viewmodel = 0;
}

bool CDisplacer::PlayEmptySound( void )
{
	if ( m_bPlayEmptySound )
	{
		EMIT_SOUND_DYN( ENT( m_hPlayer->pev ), CHAN_WEAPON, "buttons/button11.wav", 0.8, ATTN_NORM, 0, PITCH_NORM );
		m_bPlayEmptySound = false;
	}

	return false;
}

// Spins the displacer up, the think function fires once the spin is done
void CDisplacer::ProjectileAttack( float flRecoil, int iAmmoNeeded, float flSpinTime, float flFireTime, int iMode, const char *pszSpinSound, float flThinkDelay, void ( CBaseEntity::*pfnThink )( void ) )
{
	if ( m_iFireMode != DISPLACER_ATTACK_NONE )
		return;

	if ( m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] < iAmmoNeeded )
	{
		PlayEmptySound();
		m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.25;
		return;
	}

	m_hPlayer->m_iWeaponVolume = DISPLACER_VOLUME;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + flSpinTime + flFireTime;
	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + flSpinTime + 1.0;

	SendWeaponAnim( DISPLACER_SPINUP );

	EMIT_SOUND_DYN( ENT( m_hPlayer->pev ), CHAN_WEAPON, pszSpinSound, 1.0, ATTN_NORM, 0, PITCH_NORM );

	PLAYBACK_EVENT_FULL( FEV_NOTHOST | FEV_RELIABLE, m_hPlayer->edict(), m_usDisplacer, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, flFireTime, 0.0, 0, 0, 0, 0 );

	m_iFireMode = iMode;

	m_pfnThink = pfnThink;
	pev->nextthink = gpGlobals->time + flThinkDelay;
}

void CDisplacer::PrimaryAttack( void )
{
	ProjectileAttack( 0.15, (int)m_flPrimaryAmmoNeeded, 31.0 / 30.0, 1.0, DISPLACER_ATTACK_PRIMARY, "weapons/displacer_spin.wav", 31.0 / 30.0, static_cast<void ( CBaseEntity::* )( void )>( &CDisplacer::FirePrimary ) );
}

void CDisplacer::SecondaryAttack( void )
{
	ProjectileAttack( 0.5, (int)m_flSecondaryAmmoNeeded, 31.0 / 30.0, 1.0, DISPLACER_ATTACK_SECONDARY, "weapons/displacer_spin2.wav", 31.0 / 30.0, static_cast<void ( CBaseEntity::* )( void )>( &CDisplacer::FireSecondary ) );
}

void CDisplacer::TertiaryAttack( void )
{
	if ( !AllowTertiaryFire() )
		return;

	if ( m_iFireMode == DISPLACER_ATTACK_TERTIARY )
	{
		FireTertiary();
		m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
		return;
	}

	ProjectileAttack( 0.5, (int)m_flTertiaryAmmoNeeded, 62.0 / 30.0, 2.0, DISPLACER_ATTACK_TERTIARY, "weapons/displacer_spin.wav", 31.0 / 30.0, static_cast<void ( CBaseEntity::* )( void )>( &CDisplacer::StartSpinTertiary ) );
}

void CDisplacer::WeaponIdle( void )
{
	if ( m_iFireMode != DISPLACER_ATTACK_NONE )
		return;

	ResetEmptySound();

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	int iAnim = UTIL_SharedRandomFloat( m_hPlayer->random_seed, 0, 1 ) > 0.5 ? DISPLACER_IDLE2 : DISPLACER_IDLE1;

	float flIdleTime = UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + flIdleTime;

	SendWeaponAnim( iAnim );
}

void CDisplacer::FireProjectile( const Vector &vecDir, float flDamage, int iAmmo, float flRadius )
{
	m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] -= iAmmo;

	if ( m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] <= 0 )
		m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] = 0;

	m_hPlayer->m_iWeaponVolume = DISPLACER_VOLUME;

	Vector vecOrigin = m_hPlayer->GetGunPosition();

	m_hPlayer->SetAnimation( PLAYER_ATTACK1 );
	SendWeaponAnim( DISPLACER_FIRE );
	EMIT_SOUND_DYN( ENT( m_hPlayer->pev ), CHAN_WEAPON, "weapons/displacer_fire.wav", 1.0, ATTN_NORM, 0, PITCH_NORM );

	CDisplacerPortal::Create( vecOrigin, vecDir * m_flPortalSpeed * flRadius,
		m_hPlayer->edict(), flDamage, m_flPortalRadius );
}

void CDisplacer::FirePrimary( void )
{
	m_iFireMode = DISPLACER_ATTACK_NONE;

	UTIL_MakeVectors( m_hPlayer->pev->v_angle + m_hPlayer->pev->punchangle );

	FireProjectile( gpGlobals->v_forward, m_flCustomDmg, (int)m_flPrimaryAmmoNeeded, 1.0 );
}

void CDisplacer::FireSecondary( void )
{
	m_iFireMode = DISPLACER_ATTACK_NONE;

	string_t iszDestination = m_iszTeleportDestination ? m_iszTeleportDestination : MAKE_STRING( "displacer_global_target" );
	CBaseEntity *pDestination = NULL;

	if ( iszDestination )
	{
		pDestination = pev->spawnflags & SF_TELEPORT_RANDOM_DESTINATION
			? UTIL_RandomTargetname( STRING( iszDestination ) )
			: UTIL_FindEntityByTargetname( NULL, STRING( iszDestination ) );
	}

	if ( !pDestination )
	{
		SendWeaponAnim( DISPLACER_IDLE1 );
		ResetEmptySound();
		PlayEmptySound();
		return;
	}

	SendWeaponAnim( DISPLACER_FIRE );

	Vector vecOldOrigin = m_hPlayer->pev->origin;
	int iTeleportFlags = pev->spawnflags &= ~DISPLACER_TELEPORT_FLAGS;

	if ( pev->spawnflags & SF_DISPLACER_ROTATE )
		iTeleportFlags |= SF_TELEPORT_ROTATE;

	if ( pev->spawnflags & SF_DISPLACER_IGNORE_DELAYS )
		iTeleportFlags |= SF_TELEPORT_IGNORE_DELAYS;

	if ( !UTIL_CanTeleportToEntity( pDestination, iTeleportFlags ) )
	{
		ResetEmptySound();
		PlayEmptySound();
		return;
	}

	FireProjectile( Vector( 0, 0, -1 ), m_flCustomDmg, (int) m_flSecondaryAmmoNeeded, 1.0 );

	iTeleportFlags = pev->spawnflags &= ~DISPLACER_TELEPORT_FLAGS;

	if ( pev->spawnflags & SF_DISPLACER_ROTATE )
		iTeleportFlags |= SF_TELEPORT_ROTATE;

	if ( pev->spawnflags & SF_DISPLACER_IGNORE_DELAYS )
		iTeleportFlags |= SF_TELEPORT_IGNORE_DELAYS;

	UTIL_TeleportEntity( m_hPlayer, pDestination, iTeleportFlags, g_vecZero );
	m_hPlayer->SetAnimation( PLAYER_ATTACK1 );
	EMIT_SOUND_DYN( ENT( m_hPlayer->pev ), CHAN_WEAPON, "weapons/displacer_fire.wav", 1.0, ATTN_NORM, 0, PITCH_NORM );
	EMIT_SOUND_DYN( ENT( m_hPlayer->pev ), CHAN_WEAPON, "weapons/displacer_self.wav", 1.0, ATTN_NORM, 0, PITCH_NORM );
	CServer_SoundEngine::GetSoundEngine()->PlaySound( m_hPlayer->edict(), CHAN_STATIC,
		"weapons/displacer_teleport_player.wav", 1.0, ATTN_NORM, 0, PITCH_NORM, 0, true, vecOldOrigin, 0 );
}

void CDisplacer::StartSpinTertiary( void )
{
	SendWeaponAnim( DISPLACER_SPIN );

	SetThink( &CDisplacer::EndSpinTertiary );
	pev->nextthink = gpGlobals->time + 31.0 / 30.0;
}

void CDisplacer::EndSpinTertiary( void )
{
	FireTertiary();
	SetThink( NULL );
}

void CDisplacer::FireTertiary( void )
{
	m_iFireMode = DISPLACER_ATTACK_NONE;

	UTIL_MakeVectors( m_hPlayer->pev->v_angle + m_hPlayer->pev->punchangle );

	FireProjectile( gpGlobals->v_forward, m_flCustomDmg * 3, (int)m_flTertiaryAmmoNeeded, 5.0 );
}

void CDisplacer::GetWeaponData( weapon_data_t *data )
{
	data->iuser1 = m_iPrimaryAmmoType;
	data->iuser2 = m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ];
	data->iuser3 = AllowTertiaryFire() ? DISPLACER_TERTIARY_ENABLED : DISPLACER_TERTIARY_DISABLED;
	data->fuser1 = m_iFireMode;
}

void CDisplacer::SetWeaponData( weapon_data_t *data )
{
	m_iPrimaryAmmoType = data->iuser1;
	m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] = data->iuser2;
	m_TertiaryMode = data->iuser3;
	m_iFireMode = (int)data->fuser1;
}

LINK_ENTITY_TO_CLASS( displacer_portal, CDisplacerPortal );

void CDisplacerPortal::Precache( void )
{
	PRECACHE_MODEL( "sprites/exit1.spr" );
	PRECACHE_MODEL( "sprites/lgtning.spr" );
	m_iRingSprite = PRECACHE_MODEL( "sprites/disp_ring.spr" );
	PRECACHE_SOUND( "weapons/displacer_start.wav" );
	PRECACHE_SOUND( "weapons/displacer_impact.wav" );
	PRECACHE_SOUND( "weapons/displacer_teleport.wav" );
}

void CDisplacerPortal::Spawn( void )
{
	Precache();

	pev->classname = MAKE_STRING( "displacer_portal" );
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 200;
	pev->scale = 1;
	pev->framerate = 40;
	SET_MODEL( ENT( pev ), "sprites/exit1.spr" );
	UTIL_SetSize( pev, g_vecZero, g_vecZero );
	EMIT_SOUND_DYN( ENT( pev ), CHAN_WEAPON, "weapons/displacer_start.wav", 1.0, ATTN_NORM, 0, PITCH_NORM );

	SetThink( &CDisplacerPortal::BeamThink );
	SetTouch( &CDisplacerPortal::ProjectileTouch );
	pev->nextthink = gpGlobals->time + 0.1;
	pev->fuser2 = gpGlobals->time + 60;
}

CDisplacerPortal *CDisplacerPortal::Create( const Vector &vecOrigin, const Vector &vecVelocity, edict_t *pOwner, float flDamage, float flRadius )
{
	CDisplacerPortal *pPortal = GetClassPtr( (CDisplacerPortal *) NULL );

	UTIL_SetOrigin( pPortal->pev, vecOrigin );
	pPortal->pev->velocity = vecVelocity;
	pPortal->pev->owner = pOwner;
	pPortal->pev->dmg = flDamage;
	pPortal->pev->fuser1 = flRadius;
	pPortal->pev->vuser1 = vecVelocity;
	pPortal->pev->angles = UTIL_VecToAngles( vecVelocity );
	pPortal->Spawn();

	return pPortal;
}

void CDisplacerPortal::ClearBeams( void )
{
	for ( int i = 0; i < ARRAYSIZE( m_hBeams ); i++ )
	{
		CBeam *pBeam = m_hBeams[ i ];

		if ( pBeam )
		{
			UTIL_Remove( pBeam );
			m_hBeams[ i ] = NULL;
		}
	}
}

void CDisplacerPortal::DoRadiusDamage( float flRadius )
{
	entvars_t *pevAttacker = pev->owner ? VARS( pev->owner ) : pev;

	::RadiusDamage( pev->origin, pev, pevAttacker, pev->dmg, flRadius, CLASS_NONE, DMG_ALWAYSGIB | DMG_ENERGYBEAM );
}

void CDisplacerPortal::ProjectileTouch( CBaseEntity *pOther )
{
	if ( pOther->edict() == pev->owner )
		return;

	entvars_t *pevAttacker = pev->owner ? VARS( pev->owner ) : pev;

	if ( pOther->pev->takedamage != DAMAGE_NO )
		pOther->TakeDamage( pevAttacker, pevAttacker, pev->dmg, DMG_NEVERGIB );

	m_hTarget = pOther;
	SetTouch( NULL );
	Detonate();
}

void CDisplacerPortal::Detonate( void )
{
	pev->velocity = g_vecZero;
	SetThink( &CDisplacerPortal::RadiusDmgThink );
	pev->nextthink = gpGlobals->time + 0.75;

	MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_BEAMCYLINDER );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z + 16 );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z + 16 + sk_plr_displacer_radius.value / 0.2 );
		WRITE_SHORT( m_iRingSprite );
		WRITE_BYTE( 0 );
		WRITE_BYTE( 0 );
		WRITE_BYTE( 2 );
		WRITE_BYTE( 16 );
		WRITE_BYTE( 0 );
		WRITE_BYTE( 255 );
		WRITE_BYTE( 255 );
		WRITE_BYTE( 255 );
		WRITE_BYTE( 255 );
		WRITE_BYTE( 0 );
	MESSAGE_END();

	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_DLIGHT );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_BYTE( 12 );
		WRITE_BYTE( 255 );
		WRITE_BYTE( 180 );
		WRITE_BYTE( 96 );
		WRITE_BYTE( 3 );
		WRITE_BYTE( 16 );
	MESSAGE_END();
}

void CDisplacerPortal::RadiusDmgThink( void )
{
	SetThink( NULL );
	ClearBeams();
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;
	pev->effects |= EF_NODRAW;

	CBaseEntity *pTarget = m_hTarget;

	if ( pTarget && pTarget->IsMonster() && !pTarget->IsAlive() )
	{
		pTarget->pev->effects |= EF_NODRAW;
		pTarget->pev->takedamage = DAMAGE_NO;
		UTIL_Remove( pTarget );
	}

	DoRadiusDamage( pev->fuser1 );
	EMIT_SOUND_DYN( ENT( pev ), CHAN_WEAPON, "weapons/displacer_impact.wav", 1.0, ATTN_NORM, 0, PITCH_NORM );
	EMIT_SOUND_DYN( ENT( pev ), CHAN_WEAPON, "weapons/displacer_teleport.wav", 1.0, ATTN_NORM, 0, PITCH_NORM );
	UTIL_Remove( this );
}

void CDisplacerPortal::BeamThink( void )
{
	if ( gpGlobals->time > pev->fuser2 )
	{
		Detonate();
		return;
	}

	if ( gpGlobals->time - m_flLastDamageTime >= 0.1f )
	{
		m_flLastDamageTime = gpGlobals->time;
		DoRadiusDamage( 16 );
	}

	if ( gpGlobals->time - m_flLastBeamTime >= 0.1f )
	{
		m_flLastBeamTime = gpGlobals->time;
		UTIL_MakeAimVectors( pev->angles );

		Vector vecOrigin = pev->origin;

		for ( int i = 0; i < ARRAYSIZE( m_hBeams ); i++ )
		{
			CBeam *pBeam = m_hBeams[ i ];

			if ( pBeam )
			{
				pBeam->SetThink( &CBaseEntity::SUB_Remove );
				pBeam->pev->nextthink = gpGlobals->time + 60;
			}

			if ( RANDOM_LONG( 0, 1 ) != 0 )
				continue;

			float flFraction = 1;
			int iSide = RANDOM_LONG( 0, 1 ) ? 1 : -1;
			TraceResult trBest;

			for ( int j = 0; j < 3; j++ )
			{
				Vector vecRight = gpGlobals->v_right;
				float flRight = RANDOM_FLOAT( 0, 1 );
				float flUp = RANDOM_FLOAT( -1, 1 );
				Vector vecEnd = vecOrigin + ( vecRight * iSide * flRight + gpGlobals->v_up * flUp ) * 512;
				TraceResult tr;

				UTIL_TraceLine( vecOrigin, vecEnd, dont_ignore_monsters, ENT( pev ), &tr );

				if ( flFraction > tr.flFraction )
				{
					trBest = tr;
					flFraction = tr.flFraction;
				}
			}

			if ( flFraction == 1 )
				continue;

			DecalGunshot( &trBest, BULLET_PLAYER_CROWBAR );

			pBeam = m_hBeams[ i ];

			if ( pBeam )
				UTIL_Remove( pBeam );

			m_hBeams[ i ] = CBeam::BeamCreate( "sprites/lgtning.spr", 30 );
			pBeam = m_hBeams[ i ];

			if ( !pBeam )
				return;

			pBeam->PointEntInit( trBest.vecEndPos, entindex() );
			pBeam->SetColor( 96, 128, 16 );
			pBeam->SetBrightness( 200 );
			pBeam->SetNoise( 80 );
		}
	}

	pev->nextthink = gpGlobals->time + 0.1;
}
