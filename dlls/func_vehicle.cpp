#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "monsters.h"
#include "trains.h"
#include "func_vehicle.h"
#include "event_flags.h"

LINK_ENTITY_TO_CLASS( func_vehicle, CFuncVehicle );
LINK_ENTITY_TO_CLASS( func_vehiclecontrols, CFuncVehicleControls );

TYPEDESCRIPTION CFuncVehicle::m_SaveData[] = {
	DEFINE_FIELD( CFuncVehicle, m_ppath, FIELD_CLASSPTR ),
	DEFINE_FIELD( CFuncVehicle, m_length, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncVehicle, m_height, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncVehicle, m_speed, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncVehicle, m_dir, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncVehicle, m_startSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncVehicle, m_controlMins, FIELD_VECTOR ),
	DEFINE_FIELD( CFuncVehicle, m_controlMaxs, FIELD_VECTOR ),
	DEFINE_FIELD( CFuncVehicle, m_sounds, FIELD_INTEGER ),
	DEFINE_FIELD( CFuncVehicle, m_flVolume, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncVehicle, m_flBank, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncVehicle, m_oldSpeed, FIELD_FLOAT ),
};

IMPLEMENT_SAVERESTORE( CFuncVehicle, CBaseEntity );

int CFuncVehicle::Classify( void )
{
	return CLASS_VEHICLE;
}

void CFuncVehicle::Spawn( void )
{
	m_speed = pev->speed == 0.0f ? 165.0f : pev->speed;

	if ( m_sounds == 0 )
		m_sounds = 3;

	ALERT( at_console, "M_speed = %f\n", m_speed );
	pev->speed = 0;
	pev->velocity = g_vecZero;
	pev->avelocity = g_vecZero;
	pev->impulse = (int) m_speed;
	m_acceleration = 5;
	m_dir = 1;
	m_flTurnStartTime = -1;

	if ( !pev->target )
		ALERT( at_console, "Vehicle with no target" );

	pev->solid = ( pev->spawnflags & SF_TRACKTRAIN_PASSABLE ) ? SOLID_NOT : SOLID_BSP;
	pev->movetype = MOVETYPE_PUSH;
	SET_MODEL( edict(), STRING( pev->model ) );
	UTIL_SetSize( pev, pev->mins, pev->maxs );
	UTIL_SetOrigin( pev, pev->origin );
	pev->oldorigin = pev->origin;
	m_controlMins = pev->mins;
	m_controlMaxs = pev->maxs;
	m_controlMaxs.z += 72.0f;
	pev->flags &= ~FL_ALWAYSTHINK;
	pev->nextthink = pev->ltime + 0.1;
	SetThink( &CFuncVehicle::Find );
	Precache();
}

void CFuncVehicle::Precache( void )
{
	if ( m_flVolume == 0.0f )
		m_flVolume = 1;

	const char *sound;

	switch ( m_sounds )
	{
		case 1:
			sound = "plats/vehicle1.wav";
			break;
		case 2:
			sound = "plats/vehicle2.wav";
			break;
		case 3:
			sound = "plats/vehicle3.wav";
			break;
		case 4:
			sound = "plats/vehicle4.wav";
			break;
		case 5:
			sound = "plats/vehicle6.wav";
			break;
		default:
			sound = "plats/vehicle7.wav";
			break;
	}

	PRECACHE_SOUND( (char *) sound );
	pev->noise = MAKE_STRING( sound );
	PRECACHE_SOUND( "plats/vehicle_brake1.wav" );
	PRECACHE_SOUND( "plats/vehicle_start1.wav" );
	m_usAdjustPitch = PRECACHE_EVENT( 1, "events/vehicle.sc" );
}

void CFuncVehicle::KeyValue( KeyValueData *key )
{
	if ( FStrEq( key->szKeyName, "length" ) )
	{
		m_length = atof( key->szValue );
		key->fHandled = TRUE;
	}

	if ( FStrEq( key->szKeyName, "width" ) )
		m_width = atof( key->szValue );
	else if ( FStrEq( key->szKeyName, "height" ) )
		m_height = atof( key->szValue );
	else if ( FStrEq( key->szKeyName, "startspeed" ) )
		m_startSpeed = atof( key->szValue );
	else if ( FStrEq( key->szKeyName, "sounds" ) )
		m_sounds = atoi( key->szValue );
	else if ( FStrEq( key->szKeyName, "volume" ) )
		m_flVolume = atoi( key->szValue ) * 0.1;
	else if ( FStrEq( key->szKeyName, "bank" ) )
		m_flBank = atof( key->szValue );
	else if ( FStrEq( key->szKeyName, "acceleration" ) )
		m_acceleration = V_max( 1, V_min( 10, atoi( key->szValue ) ) );
	else
	{
		CBaseEntity::KeyValue( key );
		return;
	}

	key->fHandled = TRUE;
}

CBasePlayer *CFuncVehicle::GetDriver( void )
{
	edict_t *entity = m_hDriver.Get();

	return entity && !entity->free && entity->pvPrivateData ? dynamic_cast<CBasePlayer *>( (CBaseEntity *) entity->pvPrivateData ) : NULL;
}

void CFuncVehicle::SetDriver( CBasePlayer *player )
{
	m_hDriver = (CBaseEntity *) NULL;

	if ( player && player->pev )
	{
		edict_t *entity = ENT( player->pev );

		if ( entity && !entity->free )
			m_hDriver.Set( entity );
	}
}

void CFuncVehicle::Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value )
{
	if ( useType != USE_SET )
	{
		if ( !ShouldToggle( useType, pev->speed != 0.0f ) )
			return;

		if ( pev->speed == 0.0f )
		{
			pev->speed = m_dir * m_speed;
			Next();
		}
		else
		{
			pev->speed = 0;
			pev->velocity = g_vecZero;
			pev->avelocity = g_vecZero;
			StopSound();
			SetThink( NULL );
		}

		return;
	}

	if ( value < 10.0f )
	{
		if ( value < 0.0f && pev->speed > 145.0f )
			StopSound();

		if ( value > 0.0f )
		{
			float ratio = pev->speed / m_speed;

			if ( pev->speed < 0.0f )
				value = m_acceleration * 0.0005 + ratio + 0.005;
			else if ( pev->speed < 10.0f )
				value = m_acceleration * 0.0005999999999999999 + ratio + 0.002142857142857143;
			else if ( pev->speed < 20.0f )
				value = m_acceleration * 0.0007 + ratio + 0.003333333333333334;
			else if ( pev->speed < 30.0f )
				value = m_acceleration * 0.0007 + ratio + 0.004166666666666667;
			else if ( pev->speed < 45.0f )
				value = m_acceleration * 0.0007 + ratio + 0.004;
			else if ( pev->speed < 60.0f )
				value = m_acceleration * 0.0008 + ratio + 0.0038;
			else if ( pev->speed < 80.0f )
				value = m_acceleration * 0.0008 + ratio + 0.0045;
			else if ( pev->speed < 100.0f )
				value = m_acceleration * 0.0009 + ratio + 0.00425;
			else if ( pev->speed < 150.0f )
				value = m_acceleration * 0.0008 + ratio + 0.002666666666666667;
			else if ( pev->speed < 225.0f )
				value = m_acceleration * 0.0007 + ratio + 0.002285714285714286;
			else if ( pev->speed < 300.0f )
				value = m_acceleration * 0.0005999999999999999 + ratio + 0.001875;
			else if ( pev->speed < 400.0f )
				value = m_acceleration * 0.0005 + ratio + 0.001444444444444444;
			else if ( pev->speed < 550.0f )
				value = m_acceleration * 0.0005 + ratio + 0.0012;
			else if ( pev->speed < 800.0f )
				value = m_acceleration * 0.0005 + ratio + 0.0009166666666666666;
		}
		else if ( value < 0.0f )
		{
			float ratio = pev->speed / m_speed;

			if ( ratio > 0.0f )
				value = ratio - 0.0125;
			else if ( ratio > -0.05 )
				value = ratio - 0.0075;
			else if ( ratio <= 0.05 && ratio > -0.1 )
				value = ratio - 0.01;
			else if ( ratio <= 0.15 && ratio > -0.15 )
				value = ratio - 0.0125;
			else if ( ratio <= 0.15 && ratio > -0.22 )
				value = ratio - 0.01375;
			else if ( ratio <= 0.22 && ratio > -0.3 )
				value = ratio - 0.0175;
			else if ( !( ratio > 0.3 ) )
				value = ratio - 0.0125;
		}

		if ( !( value > 1.0f ) )
		{
			if ( !( value >= -0.35 ) )
				value = -0.35f;
		}
		else
			value = 1;

		pev->speed = value * m_speed;
		Next();
		m_flAcceleratorTime = gpGlobals->time + 0.25;
		return;
	}

	if ( gpGlobals->time > m_flSteeringDelay )
	{
		if ( value == 20.0f )
		{
			m_iTurnAngle = V_min( m_iTurnAngle + 1, 8 );
			m_flSteeringTime = gpGlobals->time + 0.075;
		}
		else if ( value == 30.0f )
		{
			m_iTurnAngle = V_max( m_iTurnAngle - 1, -8 );
			m_flSteeringTime = gpGlobals->time + 0.075;
		}

		m_flSteeringDelay = gpGlobals->time + 0.05;
	}
}

void CFuncVehicle::SetControls( entvars_t *controls )
{
	Vector offset = controls->origin - pev->oldorigin;

	m_controlMins = controls->mins + offset;
	m_controlMaxs = controls->maxs + offset;
}

BOOL CFuncVehicle::OnControls( entvars_t *test )
{
	Vector offset = test->origin - pev->origin;

	if ( pev->spawnflags & SF_TRACKTRAIN_NOCONTROL )
		return FALSE;

	UTIL_MakeVectors( pev->angles );

	Vector local( DotProduct( offset, gpGlobals->v_forward ), -DotProduct( offset, gpGlobals->v_right ), DotProduct( offset, gpGlobals->v_up ) );

	return local.x >= m_controlMins.x && local.y >= m_controlMins.y && local.z >= m_controlMins.z && m_controlMaxs.x >= local.x && m_controlMaxs.y >= local.y && m_controlMaxs.z >= local.z;
}

void CFuncVehicle::StopSound( void )
{
	if ( m_soundPlaying && pev->noise )
	{
		unsigned short encoded = ( (unsigned short) m_sounds & 7 ) << 12;

		PLAYBACK_EVENT_FULL( FEV_RELIABLE | FEV_UPDATE, edict(), m_usAdjustPitch, 0.0, (float *) &g_vecZero,
		                     (float *) &g_vecZero, 0.0, 0.0, encoded, 0, 1, 0 );
	}

	m_soundPlaying = 0;
}

void CFuncVehicle::UpdateSound( void )
{
	if ( !pev->noise )
		return;

	float pitch = V_min( 200.0f, fabsf( pev->speed ) * 140.0f / 1500.0f + 60.0f );

	if ( m_soundPlaying )
	{
		int encoded = ( (int) ( m_flVolume * 40.0 ) & 0x3F ) | ( ( ( ( m_sounds & 7 ) << 6 ) | ( (int) ( pitch / 10.0 ) & 0x3F ) ) << 6 );

		PLAYBACK_EVENT_FULL( FEV_UPDATE, edict(), m_usAdjustPitch, 0.0, (float *) &g_vecZero,
		                     (float *) &g_vecZero, 0.0, 0.0, encoded, 0, 0, 0 );
	}
	else
	{
		if ( m_sounds < 5 )
			EMIT_SOUND_DYN( edict(), CHAN_ITEM, "plats/vehicle_brake1.wav", m_flVolume, ATTN_NORM, 0, 100 );

		EMIT_SOUND_DYN( edict(), CHAN_STATIC, STRING( pev->noise ), m_flVolume, ATTN_NORM, 0, (int) pitch );
		m_soundPlaying = 1;
	}
}

void CFuncVehicle::Find( void )
{
	m_ppath = (CPathTrack *) UTIL_FindEntityByTargetname( NULL, STRING( pev->target ) );

	if ( !m_ppath )
		return;

	if ( !FClassnameIs( m_ppath->pev, "path_track" ) )
	{
		ALERT( at_console, "func_track_train must be on a path of path_track\n" );
		m_ppath = NULL;
		return;
	}

	Vector origin = m_ppath->pev->origin;
	Vector front = origin;

	origin.z += m_height;
	m_ppath->LookAhead( &front, m_length, 0 );
	front.z += m_height;
	pev->angles = UTIL_VecToAngles( front - origin );
	pev->angles.y += 180.0f;

	if ( pev->spawnflags & SF_TRACKTRAIN_NOPITCH )
		pev->angles.x = 0;

	UTIL_SetOrigin( pev, origin );
	pev->flags &= ~FL_ALWAYSTHINK;
	pev->nextthink = pev->ltime + 0.1;
	SetThink( &CFuncVehicle::Next );
	pev->speed = m_startSpeed;
	UpdateSound();
}

void CFuncVehicle::NearestPath( void )
{
	CBaseEntity *entity = NULL;
	CPathTrack *nearest = NULL;
	float distance = 1024;

	while ( ( entity = UTIL_FindEntityInSphere( entity, pev->origin, 1024.0f ) ) != NULL )
	{
		if ( ( entity->pev->flags & ( FL_CLIENT | FL_MONSTER ) ) || !FClassnameIs( entity->pev, "path_track" ) )
			continue;

		float candidateDistance = ( pev->origin - entity->pev->origin ).Length();

		if ( distance > candidateDistance )
		{
			distance = candidateDistance;
			nearest = (CPathTrack *) entity;
		}
	}

	if ( !nearest )
	{
		ALERT( at_console, "Can't find a nearby track !!!\n" );
		SetThink( NULL );
		return;
	}

	ALERT( at_aiconsole, "TRAIN: %s, Nearest track is %s\n", STRING( pev->targetname ), STRING( nearest->pev->targetname ) );

	CPathTrack *next = nearest->GetNext();

	if ( next && ( pev->origin - nearest->pev->origin ).Length() > ( pev->origin - next->pev->origin ).Length() )
		nearest = next;

	m_ppath = nearest;

	if ( pev->speed != 0.0f )
	{
		pev->flags &= ~FL_ALWAYSTHINK;
		pev->nextthink = pev->ltime + 0.1;
		SetThink( &CFuncVehicle::Next );
	}
}

void CFuncVehicle::DeadEnd( void )
{
	CPathTrack *track = m_ppath;

	ALERT( at_aiconsole, "TRAIN(%s): Dead end ", STRING( pev->targetname ) );

	if ( track )
	{
		CPathTrack *next;

		while ( ( next = track->ValidPath( m_oldSpeed < 0.0f ? track->GetPrevious() : track->GetNext(), 1 ) ) != NULL )
			track = next;
	}

	pev->velocity = g_vecZero;
	pev->avelocity = g_vecZero;

	if ( track )
	{
		ALERT( at_aiconsole, "at %s\n", STRING( track->pev->targetname ) );

		if ( track->pev->netname )
			FireTargets( STRING( track->pev->netname ), this, this, USE_TOGGLE, 0 );
	}
	else
		ALERT( at_aiconsole, "\n" );
}

void CFuncVehicle::OverrideReset( void )
{
	pev->flags &= ~FL_ALWAYSTHINK;
	pev->nextthink = pev->ltime + 0.1;
	SetThink( &CFuncVehicle::NearestPath );
}

void CFuncVehicle::Restart( void )
{
	ALERT( at_console, "M_speed = %f\n", m_speed );
	pev->speed = 0;
	pev->velocity = g_vecZero;
	pev->avelocity = g_vecZero;
	pev->impulse = (int) m_speed;
	m_dir = 1;
	m_flTurnStartTime = -1;
	m_flNextSoundTime = -1;
	SetDriver( NULL );

	if ( !pev->target )
		ALERT( at_console, "Vehicle with no target" );

	UTIL_SetOrigin( pev, pev->oldorigin );
	STOP_SOUND( edict(), CHAN_STATIC, STRING( pev->noise ) );
	pev->flags &= ~FL_ALWAYSTHINK;
	pev->nextthink = pev->ltime + 0.1;
	SetThink( &CFuncVehicle::Find );
}

void CFuncVehicle::Blocked( CBaseEntity *other )
{
	if ( ( other->pev->flags & FL_ONGROUND ) && VARS( other->pev->groundentity ) == pev )
	{
		other->pev->velocity = pev->velocity;
		return;
	}

	other->pev->velocity = ( other->pev->origin - pev->origin ).Normalize() * pev->dmg;
	other->pev->velocity.z += 300.0f;
	pev->velocity = pev->velocity * 0.85f;
	ALERT( at_console, "VEHICLE(%s): Blocked by %s (dmg:%.2f)\n", STRING( pev->targetname ), STRING( other->pev->classname ), pev->dmg );
	UTIL_MakeVectors( pev->angles );

	Vector front = -gpGlobals->v_forward * ( m_length * 0.5f );
	Vector side = -gpGlobals->v_right * ( m_width * 0.5f );
	Vector corner = pev->origin + front - side;
	Vector opposite = pev->origin - front + side;
	Vector point = other->pev->origin;
	float height = fabs( pev->mins.z - pev->maxs.z );

	if ( fminf( corner.x, opposite.x ) > point.x || point.x > fmaxf( corner.x, opposite.x )
	     || fminf( corner.y, opposite.y ) > point.y || point.y > fmaxf( corner.y, opposite.y )
	     || pev->origin.z > point.z || point.z > height + height + pev->origin.z )
	{
		other->TakeDamage( pev, pev, 150.0f, DMG_CRUSH );
	}
}

void CFuncVehicle::TerrainFollowing( void )
{
	TraceResult trace;
	Vector end = pev->origin - Vector( 0, 0, m_height + 48.0f );

	UTIL_TraceLine( pev->origin, end, ignore_monsters, edict(), &trace );

	if ( trace.flFraction == 1.0f )
	{
		if ( trace.fInWater )
			m_vecSurfaceNormal = Vector( 0, 0, 1 );
	}
	else
		m_vecSurfaceNormal = trace.vecPlaneNormal;
}

void CFuncVehicle::CollisionDetection( void )
{
	if ( !( pev->speed > 0.0f || pev->speed < 0.0f ) )
		return;

	TraceResult trace;
	Vector start;
	Vector end;
	bool forward = pev->speed > 0.0f;
	int probe;

	for ( probe = 0; probe < 3; probe++ )
	{
		if ( forward )
			start = probe == 0 ? m_vecFrontLeft : ( probe == 1 ? m_vecFrontRight : m_vecFront );
		else
			start = probe == 0 ? m_vecBackLeft : ( probe == 1 ? m_vecBackRight : m_vecBack );

		end = forward ? start - gpGlobals->v_forward * 16.0f : start + gpGlobals->v_forward * 16.0f;
		UTIL_TraceLine( start, end, forward && probe == 0 ? dont_ignore_monsters : ignore_monsters, edict(), &trace );

		if ( trace.flFraction != 1.0f )
			break;
	}

	if ( probe == 3 )
		return;

	float facing = DotProduct( -gpGlobals->v_forward, trace.vecPlaneNormal );

	if ( !( forward ? facing > -0.7 : facing < 0.7 ) || !( trace.vecPlaneNormal.z < 0.1 ) )
	{
		if ( !( trace.vecPlaneNormal.z >= 0.65 ) || trace.fStartSolid )
			pev->speed = pev->speed * -1.0f;
		else
			m_vecSurfaceNormal = trace.vecPlaneNormal;
	}
	else
	{
		m_vecSurfaceNormal = Vector( trace.vecPlaneNormal.x, trace.vecPlaneNormal.y, 0 );
		pev->speed = pev->speed * 0.99;
	}

	if ( !forward && probe == 0 )
	{
		CBaseEntity *hit = CBaseEntity::Instance( trace.pHit );

		if ( hit && hit->Classify() == CLASS_VEHICLE )
			ALERT( at_console, "I hit another vehicle\n" );
	}
}

void CFuncVehicle::CheckTurning( void )
{
	if ( m_iTurnAngle && ( pev->speed > 0.0f || pev->speed < 0.0f ) )
	{
		Vector start;
		Vector end;
		TraceResult trace;
		bool left = m_iTurnAngle < 0;

		if ( left )
		{
			start = pev->speed > 0.0f ? m_vecFrontRight : m_vecBackLeft;
			end = pev->speed > 0.0f ? start - gpGlobals->v_right * 16.0f : start + gpGlobals->v_right * 16.0f;
		}
		else
		{
			start = pev->speed > 0.0f ? m_vecFrontLeft : m_vecBackRight;
			end = pev->speed > 0.0f ? start + gpGlobals->v_right * 16.0f : start - gpGlobals->v_right * 16.0f;
		}

		UTIL_TraceLine( start, end, ignore_monsters, edict(), &trace );

		if ( trace.flFraction != 1.0f )
			m_iTurnAngle = left ? 1 : -1;
	}

	if ( !( pev->speed > 0.0f ) )
		return;

	float maximum;
	int turn = abs( m_iTurnAngle );

	if ( turn <= 4 )
	{
		m_flTurnStartTime = -1;
		maximum = turn > 2 ? m_speed * 0.9 : m_speed;
	}
	else if ( m_flTurnStartTime == -1.0f )
	{
		m_flTurnStartTime = gpGlobals->time;
		maximum = m_speed;
	}
	else
	{
		float elapsed = gpGlobals->time - m_flTurnStartTime;

		if ( elapsed >= 0.0f )
			maximum = m_speed * 0.98;
		else if ( elapsed > 0.3 )
			maximum = m_speed * 0.95;
		else if ( elapsed > 0.6 )
			maximum = m_speed * 0.9;
		else if ( elapsed > 0.8 )
			maximum = m_speed * 0.8;
		else if ( elapsed > 1.0f )
			maximum = m_speed * 0.7;
		else if ( elapsed > 1.2 )
			maximum = m_speed * 0.5;
		else
			maximum = 0;
	}

	if ( pev->speed > maximum )
		pev->speed = pev->speed - m_speed / 10.0f;
}

void FixVehicleAngles( Vector &angles )
{
	for ( int i = 0; i < 2; i++ )
	{
		while ( angles[i] < 0.0f )
			angles[i] += 360.0f;

		while ( angles[i] > 360.0f )
			angles[i] -= 360.0f;
	}
}

void CFuncVehicle::Next( void )
{
	UTIL_MakeVectors( pev->angles );

	Vector front = -gpGlobals->v_forward * ( m_length * 0.5f );
	Vector side = -gpGlobals->v_right * ( m_width * 0.5f );
	Vector up = gpGlobals->v_up * 16.0f;

	m_vecFrontLeft = pev->origin + front - side + up;
	m_vecFrontRight = pev->origin + front + side + up;
	m_vecFront = pev->origin + front + up;
	m_vecBackLeft = pev->origin - front - side + up;
	m_vecBackRight = pev->origin - front + side + up;
	m_vecBack = pev->origin - front + up;
	m_vecSurfaceNormal = g_vecZero;
	CheckTurning();

	if ( gpGlobals->time > m_flSteeringTime )
	{
		m_flSteeringTime = gpGlobals->time + 0.1;

		if ( m_iTurnAngle < 0 )
			m_iTurnAngle++;
		else if ( m_iTurnAngle > 0 )
			m_iTurnAngle--;
	}

	if ( gpGlobals->time > m_flAcceleratorTime )
	{
		m_flAcceleratorTime = gpGlobals->time + 0.1;

		if ( pev->speed < 0.0f )
		{
			pev->speed += 20.0f;

			if ( pev->speed > 0.0f )
				pev->speed = 0;
		}
		else if ( pev->speed > 0.0f )
		{
			pev->speed -= 20.0f;

			if ( pev->speed < 0.0f )
				pev->speed = 0;
		}
	}

	if ( pev->speed == 0.0f )
	{
		m_iTurnAngle = 0;
		pev->avelocity = g_vecZero;
		pev->velocity = g_vecZero;
	}
	else
	{
		TerrainFollowing();
		CollisionDetection();

		Vector gravity = g_vecZero;

		if ( m_vecSurfaceNormal == g_vecZero )
		{
			if ( m_flLaunchTime == -1.0f )
			{
				m_flLaunchTime = gpGlobals->time;
				pev->velocity = pev->velocity * 1.5f;
			}
			else
			{
				gravity.z = ( gpGlobals->time - m_flLaunchTime ) * -35.0f;

				if ( gravity.z < -400.0f )
					gravity.z = -400.0f;
			}

			m_vecDirection = -gpGlobals->v_forward;
		}
		else
		{
			Vector axis;

			axis = CrossProduct( m_vecSurfaceNormal, gpGlobals->v_forward );
			m_vecDirection = CrossProduct( m_vecSurfaceNormal, axis );

			Vector angles = UTIL_VecToAngles( m_vecDirection );
			Vector current = pev->angles;

			angles.y += 180.0f;

			if ( m_iTurnAngle )
				angles.y += m_iTurnAngle;

			FixVehicleAngles( angles );
			FixVehicleAngles( current );

			float pitch = UTIL_AngleDistance( angles.x, current.x );
			float yaw = UTIL_AngleDistance( angles.y, current.y );

			pev->avelocity.y = (float) (int) ( V_min( 10.0f, V_max( -10.0f, yaw ) ) * 10.0f );
			pev->avelocity.x = (float) (int) ( V_min( 10.0f, V_max( -10.0f, pitch ) ) * 10.0f );
			m_flLaunchTime = -1;
			m_flPreviousGroundZ = m_vecSurfaceNormal.z;
		}

		UTIL_VecToAngles( m_vecDirection );

		if ( gpGlobals->time > m_flNextSoundTime )
		{
			UpdateSound();
			m_flNextSoundTime = gpGlobals->time + 1.0f;
		}

		if ( m_vecSurfaceNormal == g_vecZero )
			pev->velocity = pev->velocity + gravity;
		else
			pev->velocity = m_vecDirection.Normalize() * pev->speed;
	}

	SetThink( &CFuncVehicle::Next );
	pev->flags |= FL_ALWAYSTHINK;
	pev->nextthink = pev->ltime + 0.1;
}

void CFuncVehicleControls::Spawn( void )
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	SET_MODEL( edict(), STRING( pev->model ) );
	UTIL_SetSize( pev, pev->mins, pev->maxs );
	UTIL_SetOrigin( pev, pev->origin );
	SetThink( &CFuncVehicleControls::Find );
	pev->nextthink = gpGlobals->time;
}

void CFuncVehicleControls::Find( void )
{
	CBaseEntity *entity = NULL;

	while ( ( entity = UTIL_FindEntityByTargetname( entity, STRING( pev->target ) ) ) != NULL )
	{
		if ( FClassnameIs( entity->pev, "func_vehicle" ) )
		{
			( (CFuncVehicle *) entity )->SetControls( pev );
			UTIL_Remove( this );
			return;
		}
	}

	ALERT( at_console, "No vehicle %s\n", STRING( pev->target ) );
}
