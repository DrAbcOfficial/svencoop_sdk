#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters/monsters.h"
#include "weapons/weapons.h"
#include "customentity.h"
#include "world/effects.h"
#include "combat/decals.h"
#include "server/user_messages.h"
#include "monsters/shock.h"

extern cvar_t sk_shocktrooper_maxcharge;
extern cvar_t sk_plr_shockrifle;

LINK_ENTITY_TO_CLASS( shock_beam, CShockEffect );

void CShockEffect::Spawn( void )
{
	pev->movetype = MOVETYPE_BOUNCEMISSILE;
	pev->classname = MAKE_STRING( "shock_beam" );
	pev->solid = SOLID_BBOX;
	UTIL_SetModel( this, "models/shock_effect.mdl" );
	UTIL_SetSize( pev, g_vecZero, g_vecZero );
	UTIL_SetOrigin( pev, pev->origin );
	SetThink( &CShockEffect::IgniteThink );
	pev->nextthink = gpGlobals->time;
}

void CShockEffect::Precache( void )
{
	PrecacheSound_Intercept( this, "weapons/shock_impact.wav" );
	UTIL_PrecacheModel( this, "models/shock_effect.mdl" );
	UTIL_PrecacheModel( this, "sprites/redflare2.spr" );
	UTIL_PrecacheModel( this, "sprites/lgtning.spr" );
}

void CShockEffect::Shoot( entvars_t *owner, Vector origin, Vector velocity )
{
	CShockEffect *effect = GetClassPtr( (CShockEffect *) NULL );

	effect->Spawn();
	UTIL_SetOrigin( effect->pev, origin );
	effect->pev->angles = UTIL_VecToAngles( velocity );
	effect->pev->velocity = velocity;
	effect->pev->owner = owner ? ENT( owner ) : NULL;
	effect->SetThink( &CShockEffect::IgniteThink );
	effect->pev->nextthink = gpGlobals->time;
}

void CShockEffect::IgniteThink( void )
{
	m_hBeams[0] = NULL;
	m_hBeams[1] = NULL;
	m_hBeams[0] = CBeam::BeamCreate( "sprites/lgtning.spr", 40 );

	if ( m_hBeams[0] != NULL )
	{
		CBeam *beam = (CBeam *) (CBaseEntity *) m_hBeams[0];

		beam->EntsInit( entindex(), entindex() );
		beam->SetStartAttachment( 1 );
		beam->SetEndAttachment( 0 );
		beam->SetColor( 180, 255, 96 );
		beam->SetBrightness( 100 );
		beam->SetNoise( 40 );
		beam->SetFlags( BEAM_FSHADEOUT );
		beam->RelinkBeam();
	}

	m_hBeams[1] = CBeam::BeamCreate( "sprites/lgtning.spr", 80 );

	if ( m_hBeams[1] != NULL )
	{
		CBeam *beam = (CBeam *) (CBaseEntity *) m_hBeams[1];

		beam->EntsInit( entindex(), entindex() );
		beam->SetStartAttachment( 1 );
		beam->SetEndAttachment( 0 );
		beam->SetColor( 85, 200, 255 );
		beam->SetBrightness( 200 );
		beam->SetNoise( 0 );
		beam->SetFlags( BEAM_FSHADEOUT );
		beam->RelinkBeam();
	}

	SetThink( NULL );
}

void CShockEffect::Touch( CBaseEntity *other )
{
	SetTouch( NULL );
	SetThink( NULL );

	for ( int i = 0; i < ARRAYSIZE( m_hBeams ); i++ )
	{
		if ( m_hBeams[i] != NULL )
		{
			CBeam *beam = (CBeam *) (CBaseEntity *) m_hBeams[i];

			beam->SetStartAttachment( 0 );
			beam->SetEndAttachment( 0 );
			UTIL_Remove( beam );
		}

		m_hBeams[i] = NULL;
	}

	TraceResult trace;

	if ( other->pev->takedamage == DAMAGE_NO )
	{
		UTIL_Sparks( pev->origin );
		trace = UTIL_GetGlobalTrace();
		UTIL_DecalTrace( &trace, DECAL_SMALLSCORCH1 + RANDOM_LONG( 0, 2 ) );
	}
	else
	{
		trace = UTIL_GetGlobalTrace();

		entvars_t *owner = pev->owner ? VARS( pev->owner ) : NULL;

		if ( other->pev->health <= 10.0f && ( other->pev->flags & FL_CLIENT ) && RANDOM_LONG( 0, 5 ) == 5 )
			other->Killed( pev, GIB_ALWAYS );
		else
		{
			float damage = sk_shocktrooper_maxcharge.value;

			if ( pev->owner && !pev->owner->free && GET_PRIVATE( pev->owner ) && ( pev->owner->v.flags & FL_CLIENT ) )
				damage = sk_plr_shockrifle.value;

			other->TakeDamage( pev, owner, damage, DMG_SHOCK | DMG_SHOCK_GLOW );
		}

		pev->velocity = g_vecZero;
		SpawnBlood( pev->origin, other->BloodColor(), 15 );
	}

	MESSAGE_BEGIN( MSG_PVS, gmsgShkFlash, pev->origin );
	WRITE_COORD( pev->origin.x );
	WRITE_COORD( pev->origin.y );
	WRITE_COORD( pev->origin.z );
	WRITE_BYTE( 1 );
	MESSAGE_END();
	SetThink( &CShockEffect::SUB_Remove );
	pev->nextthink = gpGlobals->time;
}
