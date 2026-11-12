#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters/monsters.h"
#include "ai/soundent.h"
#include "combat/decals.h"
#include "weapons/weapons.h"
#include "weapons/sporegrenade.h"
#include "server/user_messages.h"

extern cvar_t sk_plr_spore;

LINK_ENTITY_TO_CLASS( sporegrenade, CSporeGrenade );

void CSporeGrenade::Spawn( void )
{
	Precache();

	pev->movetype = MOVETYPE_BOUNCE;
	pev->classname = MAKE_STRING( "sporegrenade" );
	pev->solid = SOLID_SLIDEBOX;
	pev->dmg = 100;

	UTIL_SetModel( this, "models/spore.mdl" );
	UTIL_SetSize( pev, Vector( -3, -3, 0 ), Vector( 3, 3, 3 ) );

	m_fRegisteredSound = false;
	m_flBounceSoundTime = 0;
}

void CSporeGrenade::Precache( void )
{
	UTIL_PrecacheModel( this, "models/spore.mdl" );
	UTIL_PrecacheModel( this, "sprites/glow01.spr" );
	UTIL_PrecacheModel( this, "sprites/spore_exp_01.spr" );
	UTIL_PrecacheModel( this, "sprites/spore_exp_c_01.spr" );
	m_iSporeSprite = UTIL_PrecacheModel( this, "sprites/pitdronespit.spr" );
	PrecacheSound_Intercept( this, "weapons/splauncher_impact.wav" );
	PrecacheSound_Intercept( this, "weapons/spore_hit1.wav" );
}

void CSporeGrenade::OnDestroy( void )
{
	SetClientEffectsEnabled( false );
	CBaseMonster::OnDestroy();
}

void CSporeGrenade::SetClientEffectsEnabled( bool enabled )
{
	if ( m_fClientEffectsEnabled == enabled )
		return;

	MESSAGE_BEGIN( MSG_ALL, gmsgSporeTrail );
	WRITE_SHORT( entindex() );
	WRITE_BYTE( enabled );
	MESSAGE_END();
	m_fClientEffectsEnabled = enabled;
}

void CSporeGrenade::Explode( TraceResult *trace, int damageType )
{
	pev->model = iStringNull;
	pev->solid = SOLID_NOT;
	pev->takedamage = DAMAGE_NO;
	pev->effects |= EF_NODRAW;
	pev->velocity = g_vecZero;

	if ( trace->flFraction != 1.0f )
		pev->origin = trace->vecEndPos + trace->vecPlaneNormal * ( pev->dmg - 24 ) * 0.3f;

	CSoundEnt::InsertSound( bits_SOUND_COMBAT, pev->origin, 1024, 3.0f, this );

	entvars_t *attacker = pev->owner ? VARS( pev->owner ) : NULL;

	pev->owner = NULL;
	UTIL_DecalTrace( trace, DECAL_SPORESPLAT1 + RANDOM_LONG( 0, 2 ) );
	RadiusDamage( pev, attacker, pev->dmg, CLASS_NONE, damageType );
	EMIT_SOUND_DYN( edict(), CHAN_VOICE, "weapons/splauncher_impact.wav", 1.0f, 0.8f, 0, PITCH_NORM );
	SetThink( &CSporeGrenade::Smoke );
	pev->nextthink = gpGlobals->time;
}

void CSporeGrenade::Explode( Vector source, Vector direction )
{
	TraceResult trace;

	UTIL_TraceLine( pev->origin, pev->origin + Vector( 0, 0, -32 ), ignore_monsters, edict(), &trace );
	Explode( &trace, DMG_ACID | DMG_POISON );
}

void CSporeGrenade::Detonate( void )
{
	TraceResult trace;
	Vector start = pev->origin + Vector( 0, 0, 8 );

	UTIL_TraceLine( start, start + Vector( 0, 0, -40 ), ignore_monsters, edict(), &trace );
	Explode( &trace, DMG_ACID | DMG_POISON );
}

void CSporeGrenade::Killed( entvars_t *attacker, int gib )
{
	Detonate();
}

void CSporeGrenade::DetonateUse( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value )
{
	SetThink( &CSporeGrenade::Detonate );
	pev->nextthink = gpGlobals->time;
}

void CSporeGrenade::Smoke( void )
{
	SetClientEffectsEnabled( false );
	UTIL_Remove( this );
}

void CSporeGrenade::IgniteThink( void )
{
	SetClientEffectsEnabled( true );
	SetThink( &CSporeGrenade::TumbleThink );
	pev->nextthink = gpGlobals->time;
}

void CSporeGrenade::TumbleThink( void )
{
	if ( !IsInWorld() )
	{
		UTIL_Remove( this );
		return;
	}

	StudioFrameAdvance();
	pev->nextthink = gpGlobals->time + 0.06f;

	if ( gpGlobals->time > pev->dmgtime - 1.0f )
	{
		Vector soundOrigin = pev->origin + pev->velocity * ( pev->dmgtime - gpGlobals->time );

		CSoundEnt::InsertSound( bits_SOUND_DANGER, soundOrigin, 400, 0.1f, this );
	}

	if ( gpGlobals->time >= pev->dmgtime )
		SetThink( &CSporeGrenade::Detonate );

	if ( pev->waterlevel != 0 )
	{
		pev->velocity = pev->velocity * 0.5f;
		pev->framerate = 0.2f;
	}
}

void CSporeGrenade::DangerSoundThink( void )
{
	if ( !IsInWorld() )
	{
		UTIL_Remove( this );
		return;
	}

	SetClientEffectsEnabled( true );
	CSoundEnt::InsertSound( bits_SOUND_DANGER, pev->origin + pev->velocity * 0.5f, (int) pev->velocity.Length(), 0.2f, this );
	pev->nextthink = gpGlobals->time + 0.2f;

	if ( pev->waterlevel != 0 )
		pev->velocity = pev->velocity * 0.5f;
}

void CSporeGrenade::ExplodeTouch( CBaseEntity *other )
{
	pev->enemy = other->edict();

	TraceResult trace;
	Vector direction = pev->velocity.Normalize();
	Vector start = pev->origin - direction * 32;
	Vector end = start + pev->velocity.Normalize() * 64;

	UTIL_TraceLine( start, end, ignore_monsters, edict(), &trace );
	Explode( &trace, DMG_ACID | DMG_POISON );
}

void CSporeGrenade::BounceTouch( CBaseEntity *other )
{
	if ( other->edict() == pev->owner )
		return;

	if ( ( other->pev->flags & ( FL_CLIENT | FL_MONSTER ) ) || other->MyMonsterPointer() != NULL )
	{
		ExplodeTouch( other );
		return;
	}

	if ( gpGlobals->time > m_flNextAttack && pev->velocity.Length() > 100 )
	{
		entvars_t *attacker = pev->owner ? VARS( pev->owner ) : NULL;

		if ( attacker != NULL )
		{
			TraceResult trace = UTIL_GetGlobalTrace();

			ClearMultiDamage();
			other->TraceAttack( attacker, 1, gpGlobals->v_forward, &trace, DMG_CLUB );
			ApplyMultiDamage( pev, attacker );
		}

		m_flNextAttack = gpGlobals->time + 1.0f;
	}

	if ( !m_fRegisteredSound )
	{
		Vector testVelocity = pev->velocity;

		testVelocity.z = testVelocity.z * -0.7f;

		if ( testVelocity.Length() <= 60 )
		{
			CSoundEnt::InsertSound( bits_SOUND_DANGER, pev->origin, (int) ( pev->dmg * 2 ), 0.3f, this );
			m_fRegisteredSound = true;
		}
	}

	pev->velocity.x = pev->velocity.x * 0.8f;
	pev->velocity.y = pev->velocity.y * 0.8f;
	BounceSound();
}

void CSporeGrenade::BounceSound( void )
{
	if ( pev->velocity.Length() >= 2.0f && gpGlobals->time >= m_flBounceSoundTime + 0.5f )
	{
		EMIT_SOUND_DYN( edict(), CHAN_VOICE, "weapons/spore_hit1.wav", 1.0f, 0.6f, 0, PITCH_NORM );
		m_flBounceSoundTime = gpGlobals->time;
	}
}

CSporeGrenade *CSporeGrenade::ShootTimed( entvars_t *owner, Vector start, Vector velocity, float time )
{
	CSporeGrenade *spore = GetClassPtr( (CSporeGrenade *) NULL );

	spore->Spawn();
	UTIL_SetOrigin( spore->pev, start );
	spore->pev->gravity = 1.0f;
	spore->pev->friction = 0.5f;
	spore->pev->velocity = velocity;
	spore->pev->angles = UTIL_VecToAngles( velocity );
	spore->pev->owner = owner ? ENT( owner ) : NULL;
	spore->pev->dmg = sk_plr_spore.value;
	spore->pev->dmgtime = gpGlobals->time + time;
	spore->SetTouch( &CSporeGrenade::BounceTouch );
	spore->SetThink( &CSporeGrenade::IgniteThink );
	spore->pev->nextthink = gpGlobals->time;

	if ( time < 0.1f )
	{
		spore->pev->nextthink = gpGlobals->time;
		spore->pev->velocity = g_vecZero;
	}

	return spore;
}

CSporeGrenade *CSporeGrenade::ShootContact( entvars_t *owner, Vector start, Vector velocity, float damage )
{
	CSporeGrenade *spore = GetClassPtr( (CSporeGrenade *) NULL );

	spore->Spawn();
	UTIL_SetOrigin( spore->pev, start );
	spore->pev->gravity = 1.0f;
	spore->pev->friction = 0.5f;
	spore->pev->velocity = velocity;
	spore->pev->angles = UTIL_VecToAngles( velocity );
	spore->pev->owner = owner ? ENT( owner ) : NULL;
	spore->pev->dmg = damage > 0 ? damage : sk_plr_spore.value;

	if ( spore->pev->dmg <= 0 )
		spore->pev->dmg = 100;

	spore->SetTouch( &CSporeGrenade::ExplodeTouch );
	spore->SetThink( &CSporeGrenade::DangerSoundThink );
	spore->pev->nextthink = gpGlobals->time;

	return spore;
}

CSporeGrenade *CSporeGrenade::ShootStraight( entvars_t *owner, Vector start, Vector velocity, float damage )
{
	CSporeGrenade *spore = GetClassPtr( (CSporeGrenade *) NULL );

	spore->Spawn();
	UTIL_SetOrigin( spore->pev, start );
	spore->pev->gravity = 0.075f;
	spore->pev->friction = 0.075f;
	spore->pev->velocity = velocity;
	spore->pev->angles = UTIL_VecToAngles( velocity );
	spore->pev->owner = owner ? ENT( owner ) : NULL;
	spore->pev->dmg = damage > 0 ? damage : sk_plr_spore.value;

	if ( spore->pev->dmg <= 0 )
		spore->pev->dmg = 100;

	spore->SetTouch( &CSporeGrenade::ExplodeTouch );
	spore->SetThink( &CSporeGrenade::DangerSoundThink );
	spore->pev->nextthink = gpGlobals->time;

	return spore;
}

CSporeGrenade *CSporeGrenade::ShootLob( entvars_t *owner, Vector start, Vector velocity, bool randomize, float damage )
{
	CSporeGrenade *spore = GetClassPtr( (CSporeGrenade *) NULL );

	spore->Spawn();
	UTIL_SetOrigin( spore->pev, start );

	if ( randomize )
	{
		// This changes the already-applied start value, retaining two RNG draws
		start.x += RANDOM_FLOAT( -20, 20 );
		start.z += RANDOM_FLOAT( -20, 20 );
	}

	spore->pev->gravity = 0.8f;
	spore->pev->friction = 0.4f;
	spore->pev->velocity = velocity;
	spore->pev->angles = UTIL_VecToAngles( velocity );
	spore->pev->owner = owner ? ENT( owner ) : NULL;
	spore->pev->dmg = damage > 0 ? damage : sk_plr_spore.value;

	if ( spore->pev->dmg <= 0 )
		spore->pev->dmg = 100;

	spore->pev->dmgtime = gpGlobals->time + 4.0f;
	spore->SetTouch( &CSporeGrenade::BounceTouch );
	spore->SetThink( &CSporeGrenade::IgniteThink );
	spore->pev->nextthink = gpGlobals->time;

	return spore;
}
