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
/*

===== generic grenade.cpp ========================================================

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters/monsters.h"
#include "weapons/weapons.h"
#include "ai/nodes.h"
#include "ai/soundent.h"
#include "combat/decals.h"
#include "gamerules/CClassicMode.h"
#include "gamerules/game.h"
#include "sound/server_sound.h"

//===================grenade


LINK_ENTITY_TO_CLASS( grenade, CGrenade );
LINK_ENTITY_TO_CLASS( monster_handgrenade, CGrenade );

// Grenades flagged with this will be triggered when the owner calls detonateSatchelCharges
#define SF_DETONATE		0x0001

//
// Grenade Explode
//
void CGrenade::Explode( Vector vecSrc, Vector vecAim )
{
	TraceResult trace;

	UTIL_TraceLine( pev->origin, pev->origin + Vector( 0, 0, -32 ), ignore_monsters, edict(), &trace );
	m_bExploded = true;
	Explode( &trace, DMG_BLAST );
}

// UNDONE: temporary scorching for PreAlpha - find a less sleazy permenant solution.
void CGrenade::Explode( TraceResult *trace, int bitsDamageType )
{
	m_bExploded = true;
	pev->model = iStringNull;
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->takedamage = DAMAGE_NO;

	if ( trace->flFraction != 1.0f )
		pev->origin = trace->vecEndPos + trace->vecPlaneNormal * ( pev->dmg - 24 ) * 0.6f;

	int contents = UTIL_PointContents( pev->origin );

	int visual;

	if ( m_iGrenadeType == 1 )
		visual = RANDOM_LONG( 0, 2 );
	else
		visual = 1;

	if ( visual )
	{
		MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_EXPLOSION );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_SHORT( contents == CONTENTS_WATER ? g_sModelIndexWExplosion : g_sModelIndexFireball );
		WRITE_BYTE( ( pev->dmg - 50 ) * 0.6 );
		WRITE_BYTE( 15 );
		WRITE_BYTE( TE_EXPLFLAG_NOSOUND );
		MESSAGE_END();

		const char *sample = NULL;

		switch ( RANDOM_LONG( 0, 2 ) )
		{
			case 0:
				sample = "weapons/explode3.wav";
				break;
			case 1:
				sample = "weapons/explode4.wav";
				break;
			case 2:
				sample = "weapons/explode5.wav";
				break;
		}

		if ( sample )
			CServer_SoundEngine::GetSoundEngine()->PlaySound( NULL, CHAN_AUTO, (char *) sample,
			                                                  1.0f, 0.3f, 0, PITCH_NORM, 0, true, pev->origin, 0 );

		CSoundEnt::InsertSound( bits_SOUND_COMBAT, pev->origin, NORMAL_EXPLOSION_VOLUME, 3.0f, this );
	}

	entvars_t *owner;

	if ( pev->owner )
		owner = VARS( pev->owner );
	else
		owner = NULL;

	RemoveEntityFlag( 2 );
	pev->owner = NULL;
	RadiusDamage( pev, owner, pev->dmg, CLASS_NONE, bitsDamageType );
	UTIL_DecalTrace( trace, RANDOM_FLOAT( 0, 1 ) >= 0.5f ? DECAL_SCORCH2 : DECAL_SCORCH1 );

	if ( visual && m_iGrenadeType != 1 )
	{
		switch ( RANDOM_LONG( 0, 2 ) )
		{
			case 0:
				EMIT_SOUND_DYN( edict(), CHAN_VOICE, "weapons/debris1.wav", 0.55f, ATTN_NORM, 0, PITCH_NORM, 0, false, g_vecZero );
				break;
			case 1:
				EMIT_SOUND_DYN( edict(), CHAN_VOICE, "weapons/debris2.wav", 0.55f, ATTN_NORM, 0, PITCH_NORM, 0, false, g_vecZero );
				break;
			case 2:
				EMIT_SOUND_DYN( edict(), CHAN_VOICE, "weapons/debris3.wav", 0.55f, ATTN_NORM, 0, PITCH_NORM, 0, false, g_vecZero );
				break;
		}
	}

	if ( pev->dmg <= 150.0f )
		UTIL_ScreenShake( pev->origin, 15, 50, 1, pev->dmg * 4 );
	else
		UTIL_ScreenShake( pev->origin, 25, 150, 1, pev->dmg * 8 );

	pev->effects |= EF_NODRAW;
	SetThink( &CGrenade::Smoke );
	pev->velocity = g_vecZero;
	pev->nextthink = gpGlobals->time + 0.3;

	if ( !visual && contents != CONTENTS_WATER )
	{
		int count = RANDOM_LONG( 0, 3 );

		for ( int i = 0; i < count; ++i )
			CBaseEntity::Create( "spark_shower", pev->origin, trace->vecPlaneNormal, false, NULL );
	}

	if ( m_iGrenadeType == 1 )
		UTIL_Remove( this );
}


void CGrenade::Smoke( void )
{
	if (UTIL_PointContents ( pev->origin ) == CONTENTS_WATER)
	{
		UTIL_Bubbles( pev->origin - Vector( 64, 64, 64 ), pev->origin + Vector( 64, 64, 64 ), 100 );
	}
	else
	{
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( (pev->dmg - 50) * 0.80 ); // scale * 10
			WRITE_BYTE( 24 ); // framerate
		MESSAGE_END();
	}
	UTIL_Remove( this );
}

void CGrenade::Killed( entvars_t *pevAttacker, int iGib )
{
	Detonate( );
}


// Timed grenade, this think is called when time runs out.
void CGrenade::DetonateUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ){
	SetThink( &CGrenade::CallDetonate );
	pev->nextthink = gpGlobals->time;
}

void CGrenade::PreDetonate( void )
{
	CSoundEnt::InsertSound ( bits_SOUND_DANGER, pev->origin, 400, 0.3, this );

	SetThink( &CGrenade::CallDetonate );
	pev->nextthink = gpGlobals->time + 1;
}


void CGrenade::Detonate( void )
{
	TraceResult tr;
	Vector		vecSpot;// trace starts here!

	vecSpot = pev->origin + Vector ( 0 , 0 , 8 );
	UTIL_TraceLine ( vecSpot, vecSpot + Vector ( 0, 0, -40 ),  ignore_monsters, ENT(pev), & tr);

	Explode( &tr, DMG_BLAST );
}


//
// Contact grenade, explode when it touches something
//
void CGrenade::ExplodeTouch( CBaseEntity *pOther )
{
	TraceResult tr;
	Vector start = pev->origin - pev->velocity.Normalize() * 32;
	Vector end = start + pev->velocity.Normalize() * 64;

	pev->enemy = pOther->edict();
	UTIL_TraceLine( start, end, ignore_monsters, edict(), &tr );
	EMIT_SOUND_DYN( edict(), CHAN_WEAPON, "common/null.wav", 1.0f, 1.25f, 0, 80, 0, false, g_vecZero );

	if ( FClassnameIs( pev, "rpg_rocket" ) || FClassnameIs( pev, "hvr_rocket" ) )
		EMIT_SOUND_DYN( edict(), CHAN_VOICE, "weapons/rocket1.wav", 0, 0, SND_STOP, PITCH_NORM, 0, false, g_vecZero );

	Explode( &tr, DMG_BLAST );
}


void CGrenade::DangerSoundThink( void )
{
	if (!IsInWorld())
	{
		UTIL_Remove( this );
		return;
	}

	if ( gpGlobals->time - 0.2 > m_flSpawnTime )
		CSoundEnt::InsertSound ( bits_SOUND_DANGER, pev->origin + pev->velocity * 0.5,
		                        (int) pev->velocity.Length( ), 0.2, this );
	pev->nextthink = gpGlobals->time + 0.1;
	pev->angles.y = UTIL_VecToAngles( pev->velocity ).y;

	if ( m_iGrenadeType < 2 )
	{
		pev->angles.x = UTIL_VecToAngles( pev->velocity ).x;
	}
	else
	{
		pev->angles.x = UTIL_VecToAngles( pev->velocity ).x - 90;

		if ( m_flLastBounceSound < 14 )
		{
			MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_SPRITE_SPRAY );
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_COORD( g_vecZero.x );
			WRITE_COORD( g_vecZero.y );
			WRITE_COORD( 1 );
			WRITE_SHORT( g_sModelIndexWeaponSmoke );
			WRITE_BYTE( 1 );
			WRITE_BYTE( 300 );
			WRITE_BYTE( 35 );
			MESSAGE_END();
			pev->nextthink = gpGlobals->time + 0.005;
			m_flLastBounceSound += 1;
		}

		if ( pev->velocity.z < 0 && m_iGrenadeType == 2 )
		{
			EMIT_SOUND_DYN( edict(), CHAN_VOICE, "weapons/ofmortar.wav", 1, 0, 0, PITCH_NORM, 0, false, g_vecZero );
			m_iGrenadeType = 3;
		}
	}

	if (pev->waterlevel )
		pev->velocity = pev->velocity * 0.5;
	}

void CGrenade::BounceTouch( CBaseEntity *pOther )
{
	// don't hit the guy that launched this grenade
	if ( pOther->edict() == pev->owner )
		return;

	// only do damage if we're moving fairly fast
	if ( gpGlobals->time > m_flNextAttack && pev->velocity.Length() > 100)
	{
		entvars_t *pevOwner;

		if ( pev->owner )
			pevOwner = VARS( pev->owner );
		else
			pevOwner = NULL;

		if (pevOwner)
		{
			TraceResult tr = UTIL_GetGlobalTrace( );
			ClearMultiDamage( );
			pOther->TraceAttack(pevOwner, 1, gpGlobals->v_forward, &tr, DMG_CLUB );
			ApplyMultiDamage( pev, pevOwner);
		}
		m_flNextAttack = gpGlobals->time + 1.0; // debounce
	}

	Vector vecTestVelocity = pev->velocity;
	vecTestVelocity.z *= 0.45;

	if ( !m_fRegisteredSound && vecTestVelocity.Length() <= 60 )
	{
		//ALERT( at_console, "Grenade Registered!: %f\n", vecTestVelocity.Length() );

		// grenade is moving really slow. It's probably very close to where it will ultimately stop moving.
		// go ahead and emit the danger sound.

		// register a radius louder than the explosion, so we make sure everyone gets out of the way
		CSoundEnt::InsertSound ( bits_SOUND_DANGER, pev->origin, (int) ( pev->dmg + pev->dmg ), 0.3f, this );
		m_fRegisteredSound = TRUE;
	}

	if ( pOther->IsBSPModel() )
		pev->flags |= FL_ONGROUND;

	if ( ( pev->flags & FL_ONGROUND ) && gpGlobals->time >= m_flNextFriction )
	{
		// add a bit of static friction
		pev->velocity = pev->velocity * 0.9f;

		pev->sequence = 1;
		m_flNextFriction = gpGlobals->time + 0.1;
	}

	BounceSound();
	pev->flags |= FL_MONSTER;
	pev->framerate = (float) fmin( 16.0, fmax( 0.0, ( pev->origin - m_vecLastBounceOrigin ).Length() * 8.0f ) );
	m_vecLastBounceOrigin = pev->origin;
}



void CGrenade::SlideTouch( CBaseEntity *pOther )
{
	// don't hit the guy that launched this grenade
	if ( pOther->edict() == pev->owner )
		return;

	// pev->avelocity = Vector (300, 300, 300);

	if ( ( pev->flags & FL_ONGROUND ) && gpGlobals->time >= m_flNextFriction )
	{
		// add a bit of static friction
		pev->velocity = pev->velocity * 0.95;
		m_flNextFriction = gpGlobals->time + 0.1;
	}
	else
	{
		BounceSound();
	}
}

void CGrenade::BounceSound( void ) {
	if ( !( pev->velocity.Length() >= 2.0f ) || !( m_flLastBounceSound + 0.7 <= gpGlobals->time ) )
		return;

	m_flLastBounceSound = gpGlobals->time;

	if ( m_iGrenadeType == 1 )
	{
		EMIT_SOUND_DYN( edict(), CHAN_VOICE, "misc/bbimpact.wav", 0.25f, ATTN_NORM, 0, PITCH_NORM, 0, false, g_vecZero );
		return;
	}

	switch ( RANDOM_LONG( 0, 2 ) )
	{
		case 0:
			EMIT_SOUND_DYN( edict(), CHAN_VOICE, "weapons/grenade_hit1.wav", 0.25f, ATTN_NORM, 0, PITCH_NORM, 0, false, g_vecZero );
			break;
		case 1:
			EMIT_SOUND_DYN( edict(), CHAN_VOICE, "weapons/grenade_hit2.wav", 0.25f, ATTN_NORM, 0, PITCH_NORM, 0, false, g_vecZero );
			break;
		case 2:
			EMIT_SOUND_DYN( edict(), CHAN_VOICE, "weapons/grenade_hit3.wav", 0.25f, ATTN_NORM, 0, PITCH_NORM, 0, false, g_vecZero );
			break;
	}
}

void CGrenade :: TumbleThink( void )
{
	if (!IsInWorld())
	{
		UTIL_Remove( this );
		return;
	}

	StudioFrameAdvance( );
	pev->nextthink = gpGlobals->time + 0.1;

	if ( gpGlobals->time > pev->dmgtime - 1.0f )
		CSoundEnt::InsertSound ( bits_SOUND_DANGER, pev->origin + pev->velocity * (pev->dmgtime - gpGlobals->time), 400, 0.1, this );

	if ( ( pev->flags & FL_ONGROUND ) || pev->groundentity )
	{
		if ( pev->velocity == g_vecZero )
		{
			pev->framerate = 0;
		}
		else
		{
			if ( pev->sequence != 1 )
			{
				pev->sequence = 1;
				ResetSequenceInfo();
			}

			pev->angles = UTIL_VecToAngles( pev->origin );
		}
	}
	else if ( pev->sequence != 4 )
	{
		pev->sequence = 4;
		ResetSequenceInfo();
	}

	if ( gpGlobals->time >= pev->dmgtime )
		SetThink( &CGrenade::CallDetonate );

	if (pev->waterlevel )
	{
		pev->velocity = pev->velocity * 0.5;
		pev->framerate = 0.2;
	}
}

void CGrenade::Spawn( void ) {
	if ( FClassnameIs( pev, "monster_handgrenade" ) )
	{
		ShootTimed( NULL, pev->origin, g_vecZero, fmaxf( pev->health, 1.0f ) );
		SetThink( &CGrenade::SUB_Remove );
		pev->nextthink = gpGlobals->time + 1.0;
		return;
	}

	SetModel_Intercept( edict(), CClassicMode::GetInstance()->IsEnabled()
	                                 ? "models/hlclassic/grenade.mdl"
	                                 : "models/grenade.mdl" );
	UTIL_SetSize( pev, g_vecZero, g_vecZero );
	pev->movetype = MOVETYPE_BOUNCE;
	pev->classname = MAKE_STRING( "grenade" );
	pev->solid = SOLID_BBOX;
	pev->dmg = 100;
	m_fRegisteredSound = FALSE;
	m_flLastBounceSound = 0;
	m_flNextFriction = gpGlobals->time;
	m_bExploded = false;
	m_vecLastBounceOrigin = pev->origin;
	m_flSpawnTime = gpGlobals->time;
}


CGrenade *CGrenade::ShootContact( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity ){
	CGrenade *grenade = GetClassPtr( (CGrenade *) NULL );

	grenade->Spawn();
	grenade->pev->gravity = 0.7f;
	UTIL_SetOrigin( grenade->pev, vecStart );
	grenade->pev->velocity = vecVelocity;
	grenade->pev->angles = UTIL_VecToAngles( vecVelocity );

	if ( pevOwner )
		grenade->pev->owner = ENT( pevOwner );
	else
		grenade->pev->owner = NULL;

	grenade->SetThink( &CGrenade::DangerSoundThink );
	grenade->pev->nextthink = gpGlobals->time + 0.1;
	grenade->pev->avelocity.z = 2000;
	grenade->SetTouch( &CGrenade::ExplodeTouch );
	grenade->pev->dmg = sk_plr_9mmAR_grenade.value;
	grenade->m_iGrenadeType = 0;

	return grenade;
}

CGrenade *CGrenade::ShootMortar( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity )
{
	CGrenade *grenade = GetClassPtr( (CGrenade *) NULL );

	grenade->Spawn();
	SetModel_Intercept( grenade->edict(), "models/mortarshell.mdl" );
	UTIL_SetOrigin( grenade->pev, vecStart );
	grenade->pev->gravity = 1.0f;
	grenade->pev->velocity = vecVelocity;
	grenade->pev->angles = UTIL_VecToAngles( vecVelocity );
	grenade->pev->angles.x = UTIL_VecToAngles( vecVelocity ).x - 90;
	grenade->pev->owner = pevOwner ? ENT( pevOwner ) : NULL;
	grenade->pev->dmg = sk_mortar.value;
	grenade->m_iGrenadeType = 2;
	grenade->SetTouch( &CGrenade::ExplodeTouch );
	grenade->SetThink( &CGrenade::DangerSoundThink );
	grenade->pev->nextthink = gpGlobals->time + 0.05;

	return grenade;
}

CGrenade *CGrenade::ShootBananaCluster( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity )
{
	CGrenade *grenade = GetClassPtr( (CGrenade *) NULL );

	grenade->Spawn();
	SetModel_Intercept( grenade->edict(), CClassicMode::GetInstance()->IsEnabled()
	                                          ? "models/hlclassic/w_grenade.mdl"
	                                          : "models/w_grenade.mdl" );
	UTIL_SetOrigin( grenade->pev, vecStart );
	grenade->pev->velocity = vecVelocity;
	grenade->pev->angles = UTIL_VecToAngles( vecVelocity );
	grenade->pev->avelocity.z = RANDOM_FLOAT( -500, 500 );
	grenade->pev->avelocity.y = RANDOM_FLOAT( -500, 500 );
	grenade->pev->avelocity.x = RANDOM_FLOAT( -500, 500 );
	grenade->pev->owner = pevOwner ? ENT( pevOwner ) : NULL;
	grenade->pev->framerate = 1.0f;
	grenade->pev->gravity = 0.8f;
	grenade->pev->friction = 0.5f;
	grenade->m_iGrenadeType = 1;
	grenade->pev->sequence = RANDOM_LONG( 3, 6 );
	grenade->SetTouch( &CGrenade::BounceTouch );
	grenade->SetThink( &CGrenade::ClusterThink );
	grenade->pev->nextthink = gpGlobals->time + 1.2f;

	return grenade;
}

void CGrenade::ClusterThink( void )
{
	CBaseEntity *owner = CBaseEntity::Instance( pev->owner );

	UTIL_MakeVectors( pev->v_angle + pev->punchangle );

	if ( owner )
	{
		for ( int i = 0; i < 5; ++i )
		{
			float z = RANDOM_FLOAT( 2, 10 );
			float y = RANDOM_FLOAT( -1, 1 );
			float x = RANDOM_FLOAT( -1, 1 );
			Vector direction = Vector( x, y, z ).Normalize();
			float speed = RANDOM_FLOAT( 300, 700 );
			float offsetY = RANDOM_FLOAT( -8, 8 );
			float offsetX = RANDOM_FLOAT( -8, 8 );
			Vector origin = pev->origin + Vector( offsetX, offsetY, 0 );
			CGrenade *grenade = GetClassPtr( (CGrenade *) NULL );

			grenade->Spawn();
			grenade->pev->gravity = 0.7f;
			UTIL_SetOrigin( grenade->pev, origin );
			grenade->pev->velocity = direction * speed;
			grenade->pev->angles = UTIL_VecToAngles( grenade->pev->velocity );
			grenade->pev->owner = owner->edict();
			grenade->SetThink( &CGrenade::DangerSoundThink );
			grenade->SetTouch( &CGrenade::ExplodeTouch );
			grenade->pev->avelocity.z = 2000;
			grenade->pev->dmg = sk_plr_9mmAR_grenade.value;
			grenade->m_iGrenadeType = 0;
			SetModel_Intercept( grenade->edict(), "models/cretegibs.mdl" );
			grenade->pev->nextthink = gpGlobals->time + 1.0f;
			grenade->pev->dmg = 125;
			grenade->m_iGrenadeType = 1;
			grenade->pev->friction = 0.3f;
			grenade->pev->gravity = 0.4f;
			grenade->pev->movetype = MOVETYPE_TOSS;
			grenade->pev->avelocity.z = RANDOM_FLOAT( -375, 375 );
			grenade->pev->avelocity.y = RANDOM_FLOAT( -175, 175 );
			grenade->pev->avelocity.x = RANDOM_FLOAT( -375, 375 );
		}
	}

	Detonate();
	UTIL_Remove( this );
}

CGrenade * CGrenade:: ShootTimed( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time ){
	CGrenade *grenade = GetClassPtr( (CGrenade *) NULL );
	const char *model = STRING( grenade->pev->model );

	grenade->Spawn();

	if ( model && *model )
	{
		SetModel_Intercept( grenade->edict(), model );
	}
	else if ( CClassicMode::GetInstance()->IsEnabled() )
	{
		SetModel_Intercept( grenade->edict(), "models/hlclassic/w_grenade.mdl" );
	}
	else
	{
		SetModel_Intercept( grenade->edict(), "models/w_grenade.mdl" );
		grenade->SetBodygroup( 0, 1 );
	}

	UTIL_SetOrigin( grenade->pev, vecStart );
	grenade->pev->dmg = sk_plr_hand_grenade.value;
	grenade->m_iGrenadeType = 0;
	grenade->pev->velocity = vecVelocity;
	grenade->pev->angles = UTIL_VecToAngles( vecVelocity );
	grenade->pev->sequence = 4;
	grenade->ResetSequenceInfo();
	grenade->pev->framerate = 2.0f;
	grenade->pev->gravity = 1.0f;
	grenade->pev->friction = 0.8f;

	if ( pevOwner )
		grenade->pev->owner = ENT( pevOwner );
	else
		grenade->pev->owner = NULL;

	grenade->SetTouch( &CGrenade::BounceTouch );
	grenade->pev->dmgtime = gpGlobals->time + time;
	grenade->SetThink( &CGrenade::TumbleThink );
	grenade->pev->nextthink = gpGlobals->time + 0.1f;

	if ( time < 0.1f )
	{
		grenade->pev->nextthink = gpGlobals->time;
		grenade->pev->velocity = g_vecZero;
	}

	return grenade;
}

CGrenade * CGrenade :: ShootSatchelCharge( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity )
{
	CGrenade *pGrenade = GetClassPtr( (CGrenade *)NULL );
	pGrenade->pev->movetype = MOVETYPE_BOUNCE;
	pGrenade->pev->classname = MAKE_STRING( "grenade" );
	
	pGrenade->pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pGrenade->pev), "models/grenade.mdl");	// Change this to satchel charge model

	UTIL_SetSize(pGrenade->pev, Vector( 0, 0, 0), Vector(0, 0, 0));

	pGrenade->pev->dmg = 200;
	UTIL_SetOrigin( pGrenade->pev, vecStart );
	pGrenade->pev->velocity = vecVelocity;
	pGrenade->pev->angles = g_vecZero;
	pGrenade->pev->owner = ENT(pevOwner);
	
	// Detonate in "time" seconds
	pGrenade->SetThink( &CGrenade::SUB_DoNothing );
	pGrenade->SetUse( &CGrenade::DetonateUse );
	pGrenade->SetTouch( &CGrenade::SlideTouch );
	pGrenade->pev->spawnflags = SF_DETONATE;

	pGrenade->pev->friction = 0.9;

	return pGrenade;
}



void CGrenade :: UseSatchelCharges( entvars_t *pevOwner, SATCHELCODE code )
{
	edict_t *pentFind;
	edict_t *pentOwner;

	if ( !pevOwner )
		return;

	CBaseEntity	*pOwner = CBaseEntity::Instance( pevOwner );

	pentOwner = pOwner->edict();

	pentFind = FIND_ENTITY_BY_CLASSNAME( NULL, "grenade" );
	while ( !FNullEnt( pentFind ) )
	{
		CBaseEntity *pEnt = Instance( pentFind );
		if ( pEnt )
		{
			if ( FBitSet( pEnt->pev->spawnflags, SF_DETONATE ) && pEnt->pev->owner == pentOwner )
			{
				if ( code == SATCHEL_DETONATE )
					pEnt->Use( pOwner, pOwner, USE_ON, 0 );
				else	// SATCHEL_RELEASE
					pEnt->pev->owner = NULL;
			}
		}
		pentFind = FIND_ENTITY_BY_CLASSNAME( pentFind, "grenade" );
	}
}

//======================end grenade

