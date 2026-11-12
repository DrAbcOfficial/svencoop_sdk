#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters/monsters.h"
#include "ai/schedule.h"
#include "ai/soundent.h"
#include "server/user_messages.h"

enum
{
	TASK_CHUMTOAD_FART_CLOUD = LAST_COMMON_TASK + 1
};

class CChumtoad : public CBaseMonster
{
public:
	void Spawn( void );
	void Precache( void );
	int ObjectCaps( void );
	int Classify( void );
	Vector BodyTarget( const Vector &source );
	void SetPlayerAlly( bool state );
	void SetYawSpeed( void );
	void PrescheduleThink( void );
	BOOL CheckRangeAttack1( float dot, float distance );
	void StartTask( Task_t *task );
	void RunTask( Task_t *task );
	Schedule_t *GetSchedule( void );
	Schedule_t *GetScheduleOfType( int type );
	bool EmitCloud( void );

	CUSTOM_SCHEDULES;

	float   m_flCloudEndTime;
	bool    m_fEmittingCloud;
	float   m_flNextCloudMessage;
	EHANDLE m_hChumtoadOwner;
};

LINK_ENTITY_TO_CLASS( monster_chumtoad, CChumtoad );

Task_t tlChumtoadFartCloud[] =
{
	{ TASK_STOP_MOVING, 0 },
	{ TASK_FACE_ENEMY, 0 },
	{ TASK_PLAY_SEQUENCE, ACT_FEAR_DISPLAY },
	{ TASK_CHUMTOAD_FART_CLOUD, ACT_TWITCH },
	{ TASK_WAIT, 3 },
	{ TASK_PLAY_SEQUENCE, ACT_STAND },
	{ TASK_FACE_ENEMY, 0 }
};

Schedule_t slChumtoadFartCloud[] =
{
	{
		tlChumtoadFartCloud,
		ARRAYSIZE( tlChumtoadFartCloud ),
		0,
		0x100000,
		"Fart Cloud"
	}
};

DEFINE_CUSTOM_SCHEDULES( CChumtoad )
{
	slChumtoadFartCloud
};

IMPLEMENT_CUSTOM_SCHEDULES( CChumtoad, CBaseMonster );

void CChumtoad::Spawn( void )
{
	Precache();

	if ( !SetupModel() )
		UTIL_SetModel( this, "models/chumtoad.mdl" );

	UTIL_SetSize( pev, Vector( -8, -8, 0 ), Vector( 8, 8, 12 ) );
	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_STEP;
	m_bloodColor = BLOOD_COLOR_YELLOW;
	pev->effects = 0;

	if ( m_iMaxHealth <= 0 )
		m_iMaxHealth = 100;

	if ( pev->health <= 0.0f )
		pev->health = m_iMaxHealth;

	pev->view_ofs = Vector( 0, 0, 20 );
	pev->yaw_speed = 5;
	m_flFieldOfView = 0.2f;
	m_MonsterState = MONSTERSTATE_NONE;
	m_afCapability = bits_CAP_RANGE_ATTACK1;
	m_flCloudEndTime = gpGlobals->time;

	if ( pev->owner )
		m_hChumtoadOwner = !pev->owner->free ? CBaseEntity::Instance( pev->owner ) : NULL;

	if ( !m_FormattedName )
	{
		if ( m_hChumtoadOwner != NULL )
		{
			char name[128];

			snprintf( name, sizeof( name ), "%s's Chumtoad", STRING( m_hChumtoadOwner->pev->netname ) );
			m_FormattedName = AllocStringLocal( name );
		}
		else
			m_FormattedName = MAKE_STRING( IsPlayerAlly() ? "Chubby" : "Angry Chumtoad" );
	}

	MonsterInit();

	if ( m_hChumtoadOwner != NULL )
		StartPlayerFollowing( m_hChumtoadOwner, false );
	else if ( !( pev->spawnflags & SF_MONSTER_PREDISASTER ) )
	{
		SetUse( &CBaseMonster::CallFollowerPlayerUse );
		m_fCanPlayerFollow = true;
	}
}

void CChumtoad::Precache( void )
{
	CBaseMonster::Precache();
	UTIL_PrecacheModel( this, "models/chumtoad.mdl" );
	PrecacheSound_Intercept( this, "ambience/disgusting.wav" );
	PrecacheSound_Intercept( this, "doors/aliendoor1.wav" );
}

int CChumtoad::ObjectCaps( void )
{
	return IsPlayerAlly() && m_hChumtoadOwner == NULL ? FCAP_IMPULSE_USE : FCAP_ACROSS_TRANSITION;
}

int CChumtoad::Classify( void )
{
	return GetClassification( CLASS_ALIEN_MONSTER );
}

Vector CChumtoad::BodyTarget( const Vector &source )
{
	Vector target = Center();

	if ( !m_fEmittingCloud )
		return target;

	float z = RANDOM_LONG( 12, 48 );
	float y = RANDOM_LONG( -128, 128 );
	float x = RANDOM_LONG( -128, 128 );

	return target + Vector( x, y, z );
}

void CChumtoad::SetPlayerAlly( bool state )
{
	SetPlayerAllyDirect( !state );
}

void CChumtoad::SetYawSpeed( void )
{
	pev->yaw_speed = 270;
}

void CChumtoad::PrescheduleThink( void )
{
	CBaseEntity *owner = pev->owner && !pev->owner->free ? CBaseEntity::Instance( pev->owner ) : NULL;

	if ( owner && ( owner->pev->origin - pev->origin ).Length() > 64.0f )
	{
		pev->owner = NULL;
		pev->angles.x = 0;
		pev->angles.z = 0;
	}

	if ( pev->skin )
		pev->skin--;
	else if ( RANDOM_LONG( 0, 127 ) == 0 )
		pev->skin = 2;
}

BOOL CChumtoad::CheckRangeAttack1( float dot, float distance )
{
	return distance < 300.0f;
}

bool CChumtoad::EmitCloud( void )
{
	Vector center = pev->origin + Vector( 0, 0, 12 );
	bool livingEnemy = false;

	CSoundEnt::InsertSound( bits_SOUND_DANGER, pev->origin, 384, 0.1f, this );

	if ( gmsgToxicCloud > 0 && gpGlobals->time >= m_flNextCloudMessage )
	{
		( *g_engfuncs.pfnMessageBegin )( MSG_PVS, gmsgToxicCloud, pev->origin, NULL );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		MESSAGE_END();

		m_flNextCloudMessage = gpGlobals->time + 5.0f;
	}

	CBaseEntity *entity = NULL;

	while ( ( entity = UTIL_FindEntityInSphere( entity, center, 320.0f ) ) != NULL )
	{
		if ( entity->pev->takedamage == DAMAGE_NO || IRelationship( entity ) <= R_NO || entity->Classify() == CLASS_MACHINE )
			continue;

		float distance = ( entity->Center() - center ).Length();
		float damage = 10.0f - ( distance / 320.0f ) * 10.0f;

		if ( !( damage > 0.0f ) )
			continue;

		entity->TakeDamage( pev, pev, damage, DMG_NERVEGAS | DMG_NEVERGIB );

		if ( entity->IsAlive() )
			livingEnemy = true;
	}

	return livingEnemy;
}

void CChumtoad::StartTask( Task_t *task )
{
	if ( task->iTask != TASK_CHUMTOAD_FART_CLOUD )
		CBaseMonster::StartTask( task );
}

void CChumtoad::RunTask( Task_t *task )
{
	if ( task->iTask != TASK_CHUMTOAD_FART_CLOUD )
	{
		CBaseMonster::RunTask( task );
		return;
	}

	bool livingEnemy = EmitCloud();

	m_fEmittingCloud = true;

	if ( m_fSequenceFinished )
		SetActivity( ACT_TWITCH );

	if ( livingEnemy || gpGlobals->time - 3.0f > m_flCloudEndTime )
		m_flCloudEndTime = gpGlobals->time + 12.0f;
	else if ( HasConditions( bits_COND_ENEMY_OCCLUDED ) && m_flCloudEndTime > gpGlobals->time + 6.0f )
		m_flCloudEndTime = gpGlobals->time + 6.0f;

	if ( gpGlobals->time > m_flCloudEndTime || ( HasConditions( bits_COND_ENEMY_DEAD ) && !livingEnemy ) )
	{
		m_fEmittingCloud = false;

		if ( !HasConditions( bits_COND_TASK_FAILED ) )
			TaskComplete();
	}
}

Schedule_t *CChumtoad::GetSchedule( void )
{
	m_fEmittingCloud = false;
	return CBaseMonster::GetSchedule();
}

Schedule_t *CChumtoad::GetScheduleOfType( int type )
{
	if ( type == SCHED_RANGE_ATTACK1 )
		return slChumtoadFartCloud;

	return CBaseMonster::GetScheduleOfType( type );
}
