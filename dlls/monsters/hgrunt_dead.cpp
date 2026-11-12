#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters/monsters.h"
#include "gamerules/CClassicMode.h"

#define HGRUNT_SHOTGUN	( 1 << 3 )
#define HGRUNT_SAW		( 1 << 4 )

static const char *g_GruntCorpsePoses[] = { "deadstomach", "deadside", "deadsitting" };
static const char *g_Op4GruntCorpsePoses[] = {
	"deadstomach", "deadside", "deadsitting", "dead_on_back", "hgrunt_dead_stomach", "dead_headcrabed"
};

int ClampGruntCorpsePose( int pose, int maximum )
{
	if ( pose < 0 )
		return 0;

	return pose > maximum ? maximum : pose;
}

bool PrepareGruntCorpse( CBaseMonster *corpse, const char *defaultModel,
                                const char *const *poses, int maximumPose, int &pose, int bloodColor )
{
	const char *model = FStringNull( corpse->pev->model ) ? defaultModel : STRING( corpse->pev->model );

	UTIL_PrecacheModel( corpse, model );
	UTIL_SetModel( corpse, model );
	corpse->m_bloodColor = bloodColor;
	corpse->pev->health = 8;
	corpse->pev->effects = 0;
	corpse->pev->skin = 0;
	corpse->pev->sequence = 0;
	corpse->pev->yaw_speed = 8;
	pose = ClampGruntCorpsePose( pose, maximumPose );
	corpse->pev->sequence = corpse->LookupSequence( poses[pose] );

	if ( corpse->pev->sequence == -1 )
	{
		ALERT( at_console, "Dead \"%s\" with bad pose %d at (X%f Y%f Z%f)\n",
		       STRING( corpse->pev->classname ), pose,
		       corpse->pev->origin.x, corpse->pev->origin.y, corpse->pev->origin.z );
		UTIL_Remove( corpse );
		return false;
	}

	return true;
}

void SpawnDeadHumanGrunt( CBaseMonster *corpse, int &pose )
{
	const char *model = CClassicMode::GetInstance()->IsEnabled() ? "models/hlclassic/hgrunt.mdl" : "models/hgrunt.mdl";

	if ( !PrepareGruntCorpse( corpse, model, g_GruntCorpsePoses, 2, pose, BLOOD_COLOR_RED ) )
		return;

	int body = corpse->pev->body;

	if ( body >= 0 && body <= 3 )
	{
		corpse->SetBodygroup( 1, body & 1 );
		corpse->SetBodygroup( 2, body < 2 ? 4 : 2 );
	}

	corpse->MonsterInitDead();
}

class CDeadOp4HGrunt : public CBaseMonster
{
public:
	void Spawn( void );
	void KeyValue( KeyValueData *pkvd );
	int Classify( void ) { return GetClassification( CLASS_PLAYER_ALLY ); }

	int m_iPose;
	int m_iHead;
};

class CDeadOp4HGruntMedic : public CBaseMonster
{
public:
	void Spawn( void );
	void KeyValue( KeyValueData *pkvd );
	int Classify( void ) { return GetClassification( CLASS_HUMAN_MILITARY ); }

	int m_iPose;
};

class CDeadOp4HGruntTorch : public CBaseMonster
{
public:
	void Spawn( void );
	void KeyValue( KeyValueData *pkvd );
	int Classify( void ) { return GetClassification( CLASS_HUMAN_MILITARY ); }

	int m_iPose;
};

class CDeadRobotGrunt : public CBaseMonster
{
public:
	void Spawn( void );
	void KeyValue( KeyValueData *pkvd );
	int Classify( void ) { return GetClassification( CLASS_MACHINE ); }

	int m_iPose;
};

LINK_ENTITY_TO_CLASS( monster_grunt_ally_dead, CDeadOp4HGrunt );
LINK_ENTITY_TO_CLASS( monster_human_grunt_ally_dead, CDeadOp4HGrunt );
LINK_ENTITY_TO_CLASS( monster_grunt_ally_medic_dead, CDeadOp4HGruntMedic );
LINK_ENTITY_TO_CLASS( monster_human_medic_ally_dead, CDeadOp4HGruntMedic );
LINK_ENTITY_TO_CLASS( monster_grunt_ally_torch_dead, CDeadOp4HGruntTorch );
LINK_ENTITY_TO_CLASS( monster_human_torch_ally_dead, CDeadOp4HGruntTorch );
LINK_ENTITY_TO_CLASS( monster_robogrunt_dead, CDeadRobotGrunt );

void CDeadOp4HGrunt::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "pose" ) )
	{
		m_iPose = ClampGruntCorpsePose( atoi( pkvd->szValue ), 5 );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "head" ) )
	{
		m_iHead = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseMonster::KeyValue( pkvd );
}

void CDeadOp4HGruntMedic::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "pose" ) )
	{
		m_iPose = ClampGruntCorpsePose( atoi( pkvd->szValue ), 2 );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseMonster::KeyValue( pkvd );
}

void CDeadOp4HGruntTorch::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "pose" ) )
	{
		m_iPose = ClampGruntCorpsePose( atoi( pkvd->szValue ), 2 );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseMonster::KeyValue( pkvd );
}

void CDeadRobotGrunt::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "pose" ) )
	{
		m_iPose = ClampGruntCorpsePose( atoi( pkvd->szValue ), 2 );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseMonster::KeyValue( pkvd );
}

void CDeadOp4HGrunt::Spawn( void )
{
	if ( !PrepareGruntCorpse( this, "models/hgrunt_opforf.mdl", g_Op4GruntCorpsePoses,
	                          5, m_iPose, BLOOD_COLOR_RED ) )
		return;

	if ( pev->body >= 0 && pev->body <= 3 )
	{
		int body = pev->body;

		pev->body = 0;
		pev->skin = 0;
		SetBodygroup( 1, body & 1 );
		SetBodygroup( 3, body < 2 ? 4 : 3 );
	}

	if ( m_iHead >= 0 && m_iHead <= 5 )
		SetBodygroup( 1, m_iHead );

	if ( !pev->weapons )
	{
		SetBodygroup( 2, 0 );
		SetBodygroup( 3, 3 );
	}

	if ( pev->weapons & HGRUNT_SHOTGUN )
	{
		SetBodygroup( 2, 3 );
		SetBodygroup( 3, 1 );
	}
	else if ( pev->weapons & HGRUNT_SAW )
	{
		SetBodygroup( 2, 1 );
		SetBodygroup( 3, 2 );
	}

	MonsterInitDead();
}

void CDeadOp4HGruntMedic::Spawn( void )
{
	if ( !PrepareGruntCorpse( this, "models/hgrunt_medic.mdl", g_GruntCorpsePoses,
	                          2, m_iPose, BLOOD_COLOR_RED ) )
		return;

	MonsterInitDead();
}

void CDeadOp4HGruntTorch::Spawn( void )
{
	if ( !PrepareGruntCorpse( this, "models/hgrunt_torch.mdl", g_GruntCorpsePoses,
	                          2, m_iPose, BLOOD_COLOR_RED ) )
		return;

	if ( pev->body >= 0 && pev->body <= 3 )
	{
		int body = pev->body;

		pev->body = 0;
		pev->skin = 0;
		SetBodygroup( 1, body & 1 );
		SetBodygroup( 2, body < 2 ? 0 : 2 );
	}

	MonsterInitDead();
}

void CDeadRobotGrunt::Spawn( void )
{
	if ( !PrepareGruntCorpse( this, "models/rgrunt.mdl", g_GruntCorpsePoses,
	                          2, m_iPose, DONT_BLEED ) )
		return;

	if ( pev->body >= 0 && pev->body <= 3 )
	{
		int body = pev->body;

		pev->body = 0;
		pev->skin = 0;
		SetBodygroup( 1, body & 1 );
		SetBodygroup( 2, body < 2 ? 4 : 2 );
	}

	MonsterInitDead();
}
