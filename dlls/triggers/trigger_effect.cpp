#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "ai/basemonster.h"
#include "triggers/trigger_effect.h"

#define SF_TRIGGEREFFECT_START_ON 1

LINK_ENTITY_TO_CLASS( trigger_effect, CTriggerEffect );

TYPEDESCRIPTION CTriggerEffect::m_SaveData[] =
{
	DEFINE_FIELD( CTriggerEffect, m_vecEffectGlowColor, FIELD_VECTOR ),
	DEFINE_FIELD( CTriggerEffect, m_fEffectBlockWeapons, FIELD_BOOLEAN ),
	DEFINE_FIELD( CTriggerEffect, m_fEffectInvulnerable, FIELD_BOOLEAN ),
	DEFINE_FIELD( CTriggerEffect, m_fEffectInvisible, FIELD_BOOLEAN ),
	DEFINE_FIELD( CTriggerEffect, m_fEffectNonSolid, FIELD_BOOLEAN ),
	DEFINE_FIELD( CTriggerEffect, m_flEffectRespiration, FIELD_FLOAT ),
	DEFINE_FIELD( CTriggerEffect, m_flEffectGravity, FIELD_FLOAT ),
	DEFINE_FIELD( CTriggerEffect, m_flEffectFriction, FIELD_FLOAT ),
	DEFINE_FIELD( CTriggerEffect, m_flEffectSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( CTriggerEffect, m_flEffectDamage, FIELD_FLOAT ),
};

IMPLEMENT_SAVERESTORE( CTriggerEffect, CBaseDelay );

CTriggerEffect::CTriggerEffect()
    : m_iEffectGlowMode( 0 ),
      m_vecEffectGlowColor( g_vecZero ),
      m_iEffectBlockWeaponsMode( 0 ),
      m_fEffectBlockWeapons( false ),
      m_iEffectInvulnerableMode( 0 ),
      m_fEffectInvulnerable( false ),
      m_iEffectInvisibleMode( 0 ),
      m_fEffectInvisible( false ),
      m_iEffectNonSolidMode( 0 ),
      m_fEffectNonSolid( false ),
      m_iEffectRespirationMode( 0 ),
      m_flEffectRespiration( 0 ),
      m_iEffectGravityMode( 0 ),
      m_flEffectGravity( 0 ),
      m_iEffectFrictionMode( 0 ),
      m_flEffectFriction( 0 ),
      m_iEffectSpeedMode( 0 ),
      m_flEffectSpeed( 0 ),
      m_iEffectDamageMode( 0 ),
      m_flEffectDamage( 0 )
{
}

void CTriggerEffect::KeyValue( KeyValueData *data )
{
	if ( FStrEq( data->szKeyName, "effect_glow_mode" ) )
	{
		m_iEffectGlowMode = atoi( data->szValue );
		data->fHandled = TRUE;
	}
	else if ( FStrEq( data->szKeyName, "effect_glow" ) )
	{
		UTIL_StringToVector( m_vecEffectGlowColor, data->szValue, ' ' );
		data->fHandled = TRUE;
	}
	else if ( FStrEq( data->szKeyName, "effect_block_weapons_mode" ) )
	{
		m_iEffectBlockWeaponsMode = atoi( data->szValue );
		data->fHandled = TRUE;
	}
	else if ( FStrEq( data->szKeyName, "effect_block_weapons" ) )
	{
		m_fEffectBlockWeapons = atoi( data->szValue ) >= 1;
		data->fHandled = TRUE;
	}
	else if ( FStrEq( data->szKeyName, "effect_invulnerable_mode" ) )
	{
		m_iEffectInvulnerableMode = atoi( data->szValue );
		data->fHandled = TRUE;
	}
	else if ( FStrEq( data->szKeyName, "effect_invulnerable" ) )
	{
		m_fEffectInvulnerable = atoi( data->szValue ) >= 1;
		data->fHandled = TRUE;
	}
	else if ( FStrEq( data->szKeyName, "effect_invisible_mode" ) )
	{
		m_iEffectInvisibleMode = atoi( data->szValue );
		data->fHandled = TRUE;
	}
	else if ( FStrEq( data->szKeyName, "effect_invisible" ) )
	{
		m_fEffectInvisible = atoi( data->szValue ) >= 1;
		data->fHandled = TRUE;
	}
	else if ( FStrEq( data->szKeyName, "effect_nonsolid_mode" ) )
	{
		m_iEffectNonSolidMode = atoi( data->szValue );
		data->fHandled = TRUE;
	}
	else if ( FStrEq( data->szKeyName, "effect_nonsolid" ) )
	{
		m_fEffectNonSolid = atoi( data->szValue ) >= 1;
		data->fHandled = TRUE;
	}
	else if ( FStrEq( data->szKeyName, "effect_respiration_mode" ) )
	{
		m_iEffectRespirationMode = atoi( data->szValue );
		data->fHandled = TRUE;
	}
	else if ( FStrEq( data->szKeyName, "effect_respiration" ) )
	{
		m_flEffectRespiration = (float) atof( data->szValue );
		data->fHandled = TRUE;
	}
	else if ( FStrEq( data->szKeyName, "effect_gravity_mode" ) )
	{
		m_iEffectGravityMode = atoi( data->szValue );
		data->fHandled = TRUE;
	}
	else if ( FStrEq( data->szKeyName, "effect_gravity" ) )
	{
		double gravity = atof( data->szValue ) / 100.0;

		m_flEffectGravity = (float) ( gravity > 0.0 ? gravity : 0.0 );
		data->fHandled = TRUE;
	}
	else if ( FStrEq( data->szKeyName, "effect_friction_mode" ) )
	{
		m_iEffectFrictionMode = atoi( data->szValue );
		data->fHandled = TRUE;
	}
	else if ( FStrEq( data->szKeyName, "effect_friction" ) )
	{
		m_flEffectFriction = (float) ( atof( data->szValue ) / 100.0 );
		data->fHandled = TRUE;
	}
	else if ( FStrEq( data->szKeyName, "effect_speed_mode" ) )
	{
		m_iEffectSpeedMode = atoi( data->szValue );
		data->fHandled = TRUE;
	}
	else if ( FStrEq( data->szKeyName, "effect_speed" ) )
	{
		m_flEffectSpeed = (float) ( atof( data->szValue ) / 100.0 );
		data->fHandled = TRUE;
	}
	else if ( FStrEq( data->szKeyName, "effect_damage_mode" ) )
	{
		m_iEffectDamageMode = atoi( data->szValue );
		data->fHandled = TRUE;
	}
	else if ( FStrEq( data->szKeyName, "effect_damage" ) )
	{
		m_flEffectDamage = (float) ( atof( data->szValue ) / 100.0 );
		data->fHandled = TRUE;
	}
	else
		CBaseDelay::KeyValue( data );
}

void CTriggerEffect::Spawn( void )
{
	if ( !pev->targetname )
	{
		ALERT( at_error, "trigger_effect without targetname, impossible to use -- removing. (X%f Y%f Z%f)\n",
		       pev->origin.x, pev->origin.y, pev->origin.z );
		UTIL_Remove( this );
		return;
	}

	if ( pev->spawnflags & SF_TRIGGEREFFECT_START_ON )
		Use( this, this, USE_TOGGLE, 0 );
}

void CTriggerEffect::Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value )
{
	if ( !pev->target )
		return;

	if ( FStrEq( STRING( pev->target ), "!activator" ) )
	{
		SetEffects( activator );
		return;
	}

	if ( FStrEq( STRING( pev->target ), "!caller" ) )
	{
		SetEffects( caller );
		return;
	}

	CBaseEntity *entity = NULL;

	for ( ;; )
	{
		edict_t *candidate = FIND_ENTITY_BY_STRING( entity && entity->pev ? entity->edict() : NULL,
		                                            "targetname", STRING( pev->target ) );

		if ( !candidate || candidate->free || FNullEnt( candidate ) )
			break;

		entity = (CBaseEntity *) candidate->pvPrivateData;

		if ( !entity )
			break;

		SetEffects( entity );
	}
}

void CTriggerEffect::SetEffects( CBaseEntity *entity )
{
	if ( !entity )
	{
		ALERT( at_error, "trigger_effect target entity not specified. (X%f Y%f Z%f)\n",
		       pev->origin.x, pev->origin.y, pev->origin.z );
		return;
	}

	if ( !entity->IsMonster() && !entity->IsPlayer() )
	{
		ALERT( at_error, "trigger_effect target entity #%d [%s] must be a monster or player. (X%f Y%f Z%f)\n",
		       ENTINDEX( entity->edict() ), STRING( entity->pev->classname ),
		       pev->origin.x, pev->origin.y, pev->origin.z );
		return;
	}

	CBaseMonster *monster = dynamic_cast<CBaseMonster *>( entity );

	if ( !monster )
	{
		ALERT( at_error, "trigger_effect target entity #%d [%s] monster pointer invalid. (X%f Y%f Z%f)\n",
		       ENTINDEX( entity->edict() ), STRING( entity->pev->classname ),
		       pev->origin.x, pev->origin.y, pev->origin.z );
		return;
	}

	switch ( m_iEffectGlowMode )
	{
		case 1:
			monster->m_vecEffectGlowColor = m_vecEffectGlowColor + monster->m_vecEffectGlowColor;
			ALERT( at_aiconsole, "trigger_effect target entity #%d [%s] adding glow shell R%.0f G%.0f B%.0f. (X%f Y%f Z%f)\n",
			       ENTINDEX( entity->edict() ), STRING( entity->pev->classname ),
			       m_vecEffectGlowColor.x, m_vecEffectGlowColor.y, m_vecEffectGlowColor.z,
			       pev->origin.x, pev->origin.y, pev->origin.z );
			break;
		case 2:
			monster->m_vecEffectGlowColor = monster->m_vecEffectGlowColor - m_vecEffectGlowColor;
			ALERT( at_aiconsole, "trigger_effect target entity #%d [%s] subtracting glow shell R%.0f G%.0f B%.0f. (X%f Y%f Z%f)\n",
			       ENTINDEX( entity->edict() ), STRING( entity->pev->classname ),
			       m_vecEffectGlowColor.x, m_vecEffectGlowColor.y, m_vecEffectGlowColor.z,
			       pev->origin.x, pev->origin.y, pev->origin.z );
			break;
	}

	switch ( m_iEffectBlockWeaponsMode )
	{
		case 1:
			monster->m_iEffectBlockWeapons++;
			ALERT( at_aiconsole, "trigger_effect target entity #%d [%s] adding block weapons. (X%f Y%f Z%f)\n",
			       ENTINDEX( entity->edict() ), STRING( entity->pev->classname ),
			       pev->origin.x, pev->origin.y, pev->origin.z );
			break;
		case 2:
			monster->m_iEffectBlockWeapons--;
			ALERT( at_aiconsole, "trigger_effect target entity #%d [%s] subtracting block weapons. (X%f Y%f Z%f)\n",
			       ENTINDEX( entity->edict() ), STRING( entity->pev->classname ),
			       pev->origin.x, pev->origin.y, pev->origin.z );
			break;
	}

	switch ( m_iEffectInvulnerableMode )
	{
		case 1:
			monster->m_iEffectInvulnerable++;
			ALERT( at_aiconsole, "trigger_effect target entity #%d [%s] adding invulnerable. (X%f Y%f Z%f)\n",
			       ENTINDEX( entity->edict() ), STRING( entity->pev->classname ),
			       pev->origin.x, pev->origin.y, pev->origin.z );
			break;
		case 2:
			monster->m_iEffectInvulnerable--;
			ALERT( at_aiconsole, "trigger_effect target entity #%d [%s] subtracting invulnerable. (X%f Y%f Z%f)\n",
			       ENTINDEX( entity->edict() ), STRING( entity->pev->classname ),
			       pev->origin.x, pev->origin.y, pev->origin.z );
			break;
	}

	switch ( m_iEffectInvisibleMode )
	{
		case 1:
			monster->m_iEffectInvisible++;
			ALERT( at_aiconsole, "trigger_effect target entity #%d [%s] adding invisible. (X%f Y%f Z%f)\n",
			       ENTINDEX( entity->edict() ), STRING( entity->pev->classname ),
			       pev->origin.x, pev->origin.y, pev->origin.z );
			break;
		case 2:
			monster->m_iEffectInvisible--;
			ALERT( at_aiconsole, "trigger_effect target entity #%d [%s] subtracting invisible. (X%f Y%f Z%f)\n",
			       ENTINDEX( entity->edict() ), STRING( entity->pev->classname ),
			       pev->origin.x, pev->origin.y, pev->origin.z );
			break;
	}

	switch ( m_iEffectNonSolidMode )
	{
		case 1:
			monster->m_iEffectNonSolid++;
			ALERT( at_aiconsole, "trigger_effect target entity #%d [%s] adding non-solid. (X%f Y%f Z%f)\n",
			       ENTINDEX( entity->edict() ), STRING( entity->pev->classname ),
			       pev->origin.x, pev->origin.y, pev->origin.z );
			break;
		case 2:
			monster->m_iEffectNonSolid--;
			ALERT( at_aiconsole, "trigger_effect target entity #%d [%s] subtracting non-solid. (X%f Y%f Z%f)\n",
			       ENTINDEX( entity->edict() ), STRING( entity->pev->classname ),
			       pev->origin.x, pev->origin.y, pev->origin.z );
			break;
	}

	switch ( m_iEffectRespirationMode )
	{
		case 1:
			monster->m_flEffectRespiration = m_flEffectRespiration + monster->m_flEffectRespiration;
			ALERT( at_aiconsole, "trigger_effect target entity #%d [%s] adding underwater breathing %f. (X%f Y%f Z%f)\n",
			       ENTINDEX( entity->edict() ), STRING( entity->pev->classname ),
			       m_flEffectRespiration,
			       pev->origin.x, pev->origin.y, pev->origin.z );
			break;
		case 2:
			monster->m_flEffectRespiration = monster->m_flEffectRespiration - m_flEffectRespiration;
			ALERT( at_aiconsole, "trigger_effect target entity #%d [%s] subtracting underwater breathing %f. (X%f Y%f Z%f)\n",
			       ENTINDEX( entity->edict() ), STRING( entity->pev->classname ),
			       m_flEffectRespiration,
			       pev->origin.x, pev->origin.y, pev->origin.z );
			break;
	}

	switch ( m_iEffectGravityMode )
	{
		case 1:
			monster->m_flEffectGravity = monster->m_flEffectGravity + m_flEffectGravity;
			ALERT( at_aiconsole, "trigger_effect target entity #%d [%s] adding gravity %f%%. (X%f Y%f Z%f)\n",
			       ENTINDEX( entity->edict() ), STRING( entity->pev->classname ),
			       m_flEffectGravity * 100.0f,
			       pev->origin.x, pev->origin.y, pev->origin.z );
			break;
		case 2:
			monster->m_flEffectGravity = monster->m_flEffectGravity - m_flEffectGravity;
			ALERT( at_aiconsole, "trigger_effect target entity #%d [%s] subtracting gravity %f%%. (X%f Y%f Z%f)\n",
			       ENTINDEX( entity->edict() ), STRING( entity->pev->classname ),
			       m_flEffectGravity * 100.0f,
			       pev->origin.x, pev->origin.y, pev->origin.z );
			break;
	}

	switch ( m_iEffectFrictionMode )
	{
		case 1:
			monster->m_flEffectFriction = m_flEffectFriction + monster->m_flEffectFriction;
			ALERT( at_aiconsole, "trigger_effect target entity #%d [%s] adding friction %f%%. (X%f Y%f Z%f)\n",
			       ENTINDEX( entity->edict() ), STRING( entity->pev->classname ),
			       m_flEffectFriction * 100.0f,
			       pev->origin.x, pev->origin.y, pev->origin.z );
			break;
		case 2:
			monster->m_flEffectFriction = monster->m_flEffectFriction - m_flEffectFriction;
			ALERT( at_aiconsole, "trigger_effect target entity #%d [%s] subtracting friction %f%%. (X%f Y%f Z%f)\n",
			       ENTINDEX( entity->edict() ), STRING( entity->pev->classname ),
			       m_flEffectFriction * 100.0f,
			       pev->origin.x, pev->origin.y, pev->origin.z );
			break;
	}

	switch ( m_iEffectSpeedMode )
	{
		case 1:
			monster->m_flEffectSpeed = m_flEffectSpeed + monster->m_flEffectSpeed;
			ALERT( at_aiconsole, "trigger_effect target entity #%d [%s] adding speed %f%%. (X%f Y%f Z%f)\n",
			       ENTINDEX( entity->edict() ), STRING( entity->pev->classname ),
			       m_flEffectSpeed * 100.0f,
			       pev->origin.x, pev->origin.y, pev->origin.z );
			break;
		case 2:
			monster->m_flEffectSpeed = monster->m_flEffectSpeed - m_flEffectSpeed;
			ALERT( at_aiconsole, "trigger_effect target entity #%d [%s] subtracting speed %f%%. (X%f Y%f Z%f)\n",
			       ENTINDEX( entity->edict() ), STRING( entity->pev->classname ),
			       m_flEffectSpeed * 100.0f,
			       pev->origin.x, pev->origin.y, pev->origin.z );
			break;
	}

	switch ( m_iEffectDamageMode )
	{
		case 1:
			monster->m_flEffectDamage = m_flEffectDamage + monster->m_flEffectDamage;
			ALERT( at_aiconsole, "trigger_effect target entity #%d [%s] adding damage %f%%. (X%f Y%f Z%f)\n",
			       ENTINDEX( entity->edict() ), STRING( entity->pev->classname ),
			       m_flEffectDamage * 100.0f,
			       pev->origin.x, pev->origin.y, pev->origin.z );
			break;
		case 2:
			monster->m_flEffectDamage = monster->m_flEffectDamage - m_flEffectDamage;
			ALERT( at_aiconsole, "trigger_effect target entity #%d [%s] subtracting damage %f%%. (X%f Y%f Z%f)\n",
			       ENTINDEX( entity->edict() ), STRING( entity->pev->classname ),
			       m_flEffectDamage * 100.0f,
			       pev->origin.x, pev->origin.y, pev->origin.z );
			break;
	}

	ALERT( at_aiconsole, "trigger_effect target entity #%d [%s] now applying effects. (X%f Y%f Z%f)\n",
	       ENTINDEX( entity->edict() ), STRING( entity->pev->classname ),
	       pev->origin.x, pev->origin.y, pev->origin.z );
	monster->ApplyEffects();
}
