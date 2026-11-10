#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "saverestore.h"
#include "squadmaker.h"
#include "EntityHandle.h"
#include "hgrunt_variants.h"
#include "turret.h"
#include "xenmaker.h"
#include "CASServerManager.h"
#include "CASModule.h"

#define SF_SQUADMAKER_START_ON 1
#define SF_SQUADMAKER_CYCLIC 4
#define SF_SQUADMAKER_MONSTERCLIP 8
#define SF_SQUADMAKER_PRISONER 16
#define SF_SQUADMAKER_MONSTER_SIZE 32
#define SF_SQUADMAKER_QUEUE 64

void CopySquadMakerRenderVars( entvars_t *target, const entvars_t *source )
{
	target->rendermode = source->rendermode;
	target->renderfx = source->renderfx;
	target->renderamt = source->renderamt;
	target->rendercolor = source->rendercolor;
}

void CopySquadMakerOriginalRenderVars( CBaseEntity *target, const entvars_t *source )
{
	target->m_iOriginalRenderMode = source->rendermode;
	target->m_iOriginalRenderFX = source->renderfx;
	target->m_flOriginalRenderAmount = source->renderamt;
	target->m_vecOriginalRenderColor = source->rendercolor;
}

class CSquadMakerRenderFix : public CBaseEntity
{
public:
	void Spawn( void ) override;
	void EXPORT RenderSet( void );

	EntityHandle<CBaseEntity> m_hMonster;
};

LINK_ENTITY_TO_CLASS( _squadfix, CSquadMakerRenderFix );
LINK_ENTITY_TO_CLASS( squadmaker, CSquadMaker );

void CSquadMakerRenderFix::Spawn( void )
{
	SetThink( &CSquadMakerRenderFix::RenderSet );
	pev->nextthink = gpGlobals->time + 0.1;
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;
}

void CSquadMakerRenderFix::RenderSet( void )
{
	CBaseEntity *monster = m_hMonster;

	if ( !monster )
		return;

	CopySquadMakerRenderVars( monster->pev, pev );
	CopySquadMakerOriginalRenderVars( monster, pev );
	UTIL_Remove( this );
}

CSquadMaker::CSquadMaker()
    : m_iMaxLiveChildren( 0 ), m_bChangeRenderMode( false ), m_iszNewModel( 0 ),
      m_iNewBody( -1 ), m_iNewSolid( -1 ), m_iNewSkin( -1 )
{
}

TYPEDESCRIPTION CSquadMaker::m_SaveData[] =
{
	DEFINE_FIELD( CSquadMaker, m_iszMonsterClassname, FIELD_STRING ),
	DEFINE_FIELD( CSquadMaker, m_cNumMonsters, FIELD_INTEGER ),
	DEFINE_FIELD( CSquadMaker, m_cLiveChildren, FIELD_INTEGER ),
	DEFINE_FIELD( CSquadMaker, m_iMaxLiveChildren, FIELD_INTEGER ),
	DEFINE_FIELD( CSquadMaker, m_fActive, FIELD_CHARACTER ),
	DEFINE_FIELD( CSquadMaker, m_fFadeChildren, FIELD_CHARACTER ),
};

IMPLEMENT_SAVERESTORE( CSquadMaker, CBaseMonster );

void CSquadMaker::Spawn( void )
{
	if ( m_flDelay <= 0.0f )
		m_flDelay = 0.5f;

	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;
	pev->takedamage = DAMAGE_NO;
	m_cLiveChildren = 0;

	Precache();

	if ( pev->targetname )
	{
		if ( pev->spawnflags & SF_SQUADMAKER_CYCLIC )
			SetUse( &CSquadMaker::CyclicUse );
		else
			SetUse( &CSquadMaker::ToggleUse );

		if ( pev->spawnflags & SF_SQUADMAKER_START_ON )
		{
			pev->nextthink = gpGlobals->time + m_flDelay;
			m_fActive = true;

			if ( pev->spawnflags & SF_SQUADMAKER_CYCLIC )
				CyclicUseThink();
			else
				SetThink( &CSquadMaker::MakerThink );
		}
		else
		{
			m_fActive = false;
			SetThink( &CBaseEntity::SUB_DoNothing );
		}
	}
	else
	{
		pev->nextthink = gpGlobals->time + m_flDelay;
		m_fActive = true;
		SetThink( &CSquadMaker::MakerThink );
	}

	m_fFadeChildren = m_cNumMonsters != 1;

	if ( pev->spawnflags & SF_SQUADMAKER_MONSTER_SIZE )
	{
		edict_t *monster = SpawnActualMonster();

		if ( !monster )
		{
			pev->maxs = g_vecZero;
			pev->mins = pev->maxs;
			return;
		}

		pev->mins = VARS( monster )->mins;
		pev->maxs = VARS( monster )->maxs;
		REMOVE_ENTITY( monster );
	}

	m_iQueuedSpawns = 0;
}

void CSquadMaker::Precache( void )
{
	CBaseEntity::Precache();

	if ( m_iszNewModel )
		UTIL_PrecacheModel( this, STRING( m_iszNewModel ) );

	UTIL_PrecacheMonster( STRING( m_iszMonsterClassname ), IsPlayerAlly() );
	pev->noise = m_iszMonsterClassname;
}

void *CSquadMaker::GetKeyValue( const char *name, entvartype_s &type )
{
	if ( FStrEq( name, "monstercount" ) )
	{
		type = VAR_INTEGER;
		return &m_cNumMonsters;
	}

	if ( FStrEq( name, "monstertype" ) )
	{
		type = VAR_STRING;
		return &m_iszMonsterClassname;
	}

	return CBaseEntity::GetKeyValue( name, type );
}

void CSquadMaker::KeyValue( KeyValueData *data )
{
	if ( FStrEq( data->szKeyName, "monstercount" ) )
		m_cNumMonsters = atoi( data->szValue );
	else if ( FStrEq( data->szKeyName, "respawn_as_playerally" ) )
		m_fPlayerAlly = atoi( data->szValue ) != 0;
	else if ( FStrEq( data->szKeyName, "change_rendermode" ) )
		m_bChangeRenderMode = atoi( data->szValue ) != 0;
	else if ( FStrEq( data->szKeyName, "trigger_target" ) )
		m_iszChildTriggerTarget = AllocStringLocal( data->szValue );
	else if ( FStrEq( data->szKeyName, "trigger_condition" ) )
		m_iChildTriggerCondition = atoi( data->szValue );
	else if ( FStrEq( data->szKeyName, "new_model" ) )
		m_iszNewModel = AllocStringLocal( data->szValue );
	else if ( FStrEq( data->szKeyName, "new_body" ) )
		m_iNewBody = atoi( data->szValue );
	else if ( FStrEq( data->szKeyName, "new_solid" ) )
		m_iNewSolid = atoi( data->szValue );
	else if ( FStrEq( data->szKeyName, "new_skin" ) )
		m_iNewSkin = atoi( data->szValue );
	else if ( FStrEq( data->szKeyName, "m_imaxlivechildren" ) )
		m_iMaxLiveChildren = atoi( data->szValue );
	else if ( FStrEq( data->szKeyName, "monstertype" ) )
		m_iszMonsterClassname = AllocStringLocal( data->szValue );
	else if ( FStrEq( data->szKeyName, "gag" ) )
		m_bGag = atoi( data->szValue ) >= 1;
	else if ( FStrEq( data->szKeyName, "squadname" ) )
		m_iszSquadName = AllocStringLocal( data->szValue );
	else if ( FStrEq( data->szKeyName, "displayname" ) )
	{
		m_bOverrideDisplayName = true;
		m_iszChildDisplayName = AllocStringLocal( data->szValue );
	}
	else if ( FStrEq( data->szKeyName, "is_squad_leader" ) )
		m_bSquadLeader = atoi( data->szValue ) != 0;
	else if ( FStrEq( data->szKeyName, "freeroam" ) )
	{
		m_iFreeRoam = atoi( data->szValue );
		return;
	}
	else if ( FStrEq( data->szKeyName, "bloodcolor" ) )
	{
		int color = atoi( data->szValue );

		if ( color == -1 || color == 1 || color == 2 )
			m_iBloodColor = color;
	}
	else if ( FStrEq( data->szKeyName, "classify" ) )
	{
		int classification = atoi( data->szValue );

		if ( classification <= -1 )
		{
			m_bOverrideClassification = true;
			m_iChildClassification = 0;
		}
		else if ( classification >= 1 && classification <= 19 )
		{
			m_bOverrideClassification = true;
			m_iChildClassification = classification;
		}
	}
	else if ( FStrEq( data->szKeyName, "wpn_v_model" ) )
		m_iszWeaponViewModel = AllocStringLocal( data->szValue );
	else if ( FStrEq( data->szKeyName, "wpn_w_model" ) )
		m_iszWeaponWorldModel = AllocStringLocal( data->szValue );
	else if ( FStrEq( data->szKeyName, "wpn_p_model" ) )
		m_iszWeaponPlayerModel = AllocStringLocal( data->szValue );
	else if ( FStrEq( data->szKeyName, "guard_ent" ) )
		m_iszChildGuardEntity = AllocStringLocal( data->szValue );
	else if ( FStrEq( data->szKeyName, "spawn_mode" ) )
	{
		int mode = atoi( data->szValue );

		if ( mode >= 0 && mode <= 2 )
			m_iSpawnMode = mode;
		else
			ALERT( at_console, "Unknown spawn mode in squadmaker: %d\n", mode );

		return;
	}
	else if ( FStrEq( data->szKeyName, "damage" ) )
		pev->dmg = atof( data->szValue );
	else if ( FStrEq( data->szKeyName, "xenmaker" ) )
		m_iszXenMaker = AllocStringLocal( data->szValue );
	else if ( FStrEq( data->szKeyName, "orientation" ) )
		m_iOrientation = atoi( data->szValue );
	else if ( FStrEq( data->szKeyName, "path_name" ) )
		m_iszChildPathName = AllocStringLocal( data->szValue );
	else if ( FStrEq( data->szKeyName, "function_name" ) )
	{
		char declaration[512];

		if ( FormatASFunction( declaration, sizeof( declaration ), "void", data->szValue,
		                       "CBaseMonster@, CBaseEntity@", true ) )
		{
			m_iszFunctionDeclaration = AllocStringLocal( declaration );
		}
	}
	else
	{
		CBaseMonster::KeyValue( data );
		return;
	}

	data->fHandled = TRUE;
}

void CSquadMaker::ToggleUse( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value )
{
	if ( !ShouldToggle( useType, m_fActive ) )
		return;

	m_fActive = !m_fActive;
	SetThink( m_fActive ? &CSquadMaker::MakerThink : NULL );
	pev->nextthink = gpGlobals->time;
}

void CSquadMaker::CyclicUse( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value )
{
	if ( pev->spawnflags & SF_SQUADMAKER_QUEUE )
		m_iQueuedSpawns++;

	CyclicUseThink();
}

void CSquadMaker::MakerThink( void )
{
	bool spawned = MakeMonster();

	if ( !spawned && m_iSpawnMode == 1 )
		pev->nextthink = gpGlobals->time;
	else
		pev->nextthink = gpGlobals->time + m_flDelay;
}

void CSquadMaker::CyclicUseThink( void )
{
	bool spawned = MakeMonster();

	if ( ( !spawned && m_iSpawnMode == 1 && m_cNumMonsters != 0 ) || m_iQueuedSpawns > 0 )
	{
		SetThink( &CSquadMaker::CyclicUseThink );
		pev->nextthink = m_iSpawnMode == 1 ? gpGlobals->time : gpGlobals->time + m_flDelay;
	}
	else
		SetThink( NULL );
}

void CSquadMaker::DeathNotice( entvars_t *child )
{
	m_cLiveChildren--;
	child->owner = NULL;
}

bool CSquadMaker::MakeMonster( void )
{
	if ( m_cNumMonsters == 0 )
		return false;

	if ( m_iMaxLiveChildren > 0 && m_cLiveChildren >= m_iMaxLiveChildren )
		return true;

	if ( pev->mins.z == 0.0f && !( pev->spawnflags & SF_SQUADMAKER_MONSTER_SIZE ) )
	{
		TraceResult trace = {};

		UTIL_TraceLine( pev->origin, pev->origin - Vector( 0, 0, 2048 ), ignore_monsters, edict(), &trace );
		pev->mins = Vector( -34, -34, trace.vecEndPos.z - pev->origin.z );
		pev->maxs = Vector( 34, 34, 1 );
	}

	CBaseEntity *blockers[64];
	int count = UTIL_EntitiesInBox( blockers, ARRAYSIZE( blockers ), pev->origin + pev->mins,
	                                pev->origin + pev->maxs, FL_CLIENT | FL_MONSTER );
	int liveBlockers = 0;

	if ( !( pev->dmg <= 0.0f && m_iSpawnMode == 2 ) )
	{
		for ( int i = 0; i < count; i++ )
		{
			CBaseEntity *blocker = blockers[i];

			if ( blocker->pev == pev || blocker->pev->deadflag == DEAD_DEAD )
				continue;

			blocker->TakeDamage( VARS( INDEXENT( 0 ) ), VARS( INDEXENT( 0 ) ), pev->dmg, DMG_ALWAYSGIB );

			if ( blocker->pev->solid != SOLID_NOT && blocker->pev->solid != SOLID_TRIGGER )
				liveBlockers++;
		}
	}

	if ( liveBlockers && m_iSpawnMode != 2 )
		return false;

	edict_t *child = SpawnActualMonster();

	if ( child )
	{
		if ( m_iQueuedSpawns )
			m_iQueuedSpawns--;

		m_cLiveChildren++;
		m_cNumMonsters--;

		if ( pev->target )
			FireTargets( STRING( pev->target ), this, this, USE_TOGGLE, 0 );

		CASServerManager *manager = CASServerManager::GetInstance();

		if ( manager->m_bBuildSucceeded && m_iszFunctionDeclaration && !child->free && child->pvPrivateData )
		{
			CBaseEntity *entity = (CBaseEntity *) child->pvPrivateData;
			CASModule *module = CASServerManager::GetInstance()->GetMapModule();

			if ( module )
				module->CallFunction( false, STRING( m_iszFunctionDeclaration ), 0, this, entity );
		}
	}

	return true;
}

edict_t *CSquadMaker::SpawnActualMonster( void )
{
	edict_t *child = CREATE_NAMED_ENTITY( m_iszMonsterClassname );

	if ( !child || child->free || !ENTINDEX( child ) )
	{
		ALERT( at_console, "NULL entity created by squad maker (X%f Y%f Z%f)\n",
		       pev->origin.x, pev->origin.y, pev->origin.z );
		return NULL;
	}

	CBaseEntity *entity = CBaseEntity::Instance( child );
	entvars_t *vars = VARS( child );

	vars->model = m_iszNewModel;
	vars->skin = pev->skin;
	vars->origin = pev->origin;
	vars->angles = pev->angles;
	vars->health = pev->health;
	vars->weapons = pev->weapons;
	vars->spawnflags |= SF_MONSTER_FALL_TO_GROUND;

	if ( pev->netname )
		vars->targetname = pev->netname;

	if ( m_bChangeRenderMode )
		CopySquadMakerRenderVars( vars, pev );

	if ( pev->spawnflags & SF_SQUADMAKER_PRISONER )
		vars->spawnflags |= SF_MONSTER_PRISONER;

	if ( m_iszSquadName && m_bSquadLeader )
		vars->netname = AllocStringLocal( STRING( m_iszSquadName ) );

	if ( m_bSquadLeader )
		vars->spawnflags |= 0x20;

	if ( m_bGag )
		vars->spawnflags |= SF_MONSTER_GAG;

	if ( pev->spawnflags & SF_SQUADMAKER_MONSTERCLIP )
		vars->spawnflags |= SF_MONSTER_HITMONSTERCLIP;

	if ( pev->spawnflags & SF_MONSTER_NO_WPN_DROP )
		vars->spawnflags |= SF_MONSTER_NO_WPN_DROP;

	if ( strstr( STRING( m_iszMonsterClassname ), "weapon_" ) )
	{
		if ( m_iszWeaponPlayerModel && strlen( STRING( m_iszWeaponPlayerModel ) ) > 2 )
			vars->iuser1 = m_iszWeaponPlayerModel;

		if ( m_iszWeaponWorldModel && strlen( STRING( m_iszWeaponWorldModel ) ) > 2 )
			vars->iuser2 = m_iszWeaponWorldModel;

		if ( m_iszWeaponViewModel && strlen( STRING( m_iszWeaponViewModel ) ) > 2 )
			vars->iuser3 = m_iszWeaponViewModel;
	}

	if ( FClassnameIs( vars, "monster_human_grunt_ally" ) )
		SquadMakerPrepareHumanGrunt( entity );

	if ( m_iszSoundList )
	{
		entity->m_iszSoundList = m_iszSoundList;
		entity->m_pSoundReplacementMap = m_pSoundReplacementMap;
	}

	if ( FClassnameIs( vars, "monster_turret" ) || FClassnameIs( vars, "monster_miniturret" ) )
		SquadMakerInitializeTurret( entity, m_iOrientation );

	CBaseMonster *monster = entity->MyMonsterPointer();

	if ( monster )
	{
		CopySquadMakerOriginalRenderVars( monster, pev );
		monster->SetPlayerAlly( IsPlayerAlly() );

		if ( m_bOverrideClassification )
		{
			monster->m_fOverrideClass = m_bOverrideClassification;
			monster->m_iClassSelection = m_iChildClassification;
		}

		if ( m_iBloodColor )
			monster->m_iBloodColorOverride = m_iBloodColor;

		if ( m_iszChildGuardEntity )
			monster->m_iszGuardEntName = m_iszChildGuardEntity;

		if ( m_iFreeRoam == 1 )
			monster->m_fFreeRoamMode1 = true;
		else if ( m_iFreeRoam == 2 )
			monster->m_fFreeRoamMode2 = true;

		monster->pev->vuser1 = pev->vuser1;
		monster->pev->vuser2 = pev->vuser2;

		if ( m_bOverrideDisplayName )
			monster->m_FormattedName = m_iszChildDisplayName;

		monster->m_iszPathName = m_iszChildPathName;
		monster->m_fCheckCreatureDanger = m_fCheckCreatureDanger;
	}

	if ( m_iNewBody == -2 )
		vars->body = -1;

	DispatchSpawn( child );
	vars->owner = edict();

	if ( m_iszXenMaker )
	{
		CBaseEntity *templateEntity = NULL;

		while ( ( templateEntity = UTIL_FindEntityByTargetname( templateEntity, STRING( m_iszXenMaker ) ) ) != NULL )
		{
			if ( FClassnameIs( templateEntity->pev, "env_xenmaker" ) )
				break;
		}

		if ( templateEntity )
		{
			CXenMaker *source = static_cast<CXenMaker *>( templateEntity );
			Vector origin = pev->origin + Vector( 0, 0, entity->pev->maxs.z * 0.5f );
			CXenMaker *effect = static_cast<CXenMaker *>( Create( "env_xenmaker", origin, pev->angles, true, child ) );

			effect->pev->spawnflags |= SF_XENMAKER_TRY_ONCE | SF_XENMAKER_NO_MONSTER;
			effect->m_bTryOnce = true;
			effect->m_bMakeMonster = false;
			effect->m_iszMonsterClassname = entity->pev->classname;
			effect->m_iBeamCount = source->m_iBeamCount;
			effect->m_vBeamColor = source->m_vBeamColor;
			effect->m_iBeamAlpha = source->m_iBeamAlpha;
			effect->m_flBeamRadius = source->m_flBeamRadius;
			effect->m_vLightColor = source->m_vLightColor;
			effect->m_flLightRadius = source->m_flLightRadius;
			effect->m_vStartSpriteColor = source->m_vStartSpriteColor;
			effect->m_flStartSpriteScale = source->m_flStartSpriteScale;
			effect->m_flStartSpriteFramerate = source->m_flStartSpriteFramerate;
			effect->m_iStartSpriteAlpha = source->m_iStartSpriteAlpha;
			effect->m_vEndSpriteColor = source->m_vEndSpriteColor;
			effect->m_flEndSpriteScale = source->m_flEndSpriteScale;
			effect->m_flEndSpriteFramerate = source->m_flEndSpriteFramerate;
			effect->m_iEndSpriteAlpha = source->m_iEndSpriteAlpha;
			effect->Spawn();
			effect->Use( NULL, NULL, USE_ON, 0 );
		}
		else
		{
			ALERT( at_console, "Xenmaker template '%s' not found for squadmaker (X%f Y%f Z%f)\n",
			       STRING( m_iszXenMaker ), pev->origin.x, pev->origin.y, pev->origin.z );
		}
	}

	if ( m_iNewSolid >= 0 )
	{
		if ( m_iNewSolid <= SOLID_BSP )
			vars->solid = m_iNewSolid;
		else
			ALERT( at_console, "Invalid solid type %d for squadmaker %s\n", m_iNewSolid, STRING( pev->targetname ) );
	}

	if ( m_iNewBody >= 0 )
	{
		if ( FClassnameIs( vars, "monster_human_grunt_ally" ) )
			monster->SetBodygroup( 1, m_iNewBody );
		else
			vars->body = m_iNewBody;
	}

	if ( m_iNewSkin >= 0 )
		vars->skin = m_iNewSkin;

	if ( monster )
	{
		monster->m_iszTriggerTarget = m_iszChildTriggerTarget;
		monster->m_iTriggerCondition = m_iChildTriggerCondition;
	}

	if ( m_bChangeRenderMode )
	{
		CSquadMakerRenderFix *fix = static_cast<CSquadMakerRenderFix *>( Create( "_squadfix", g_vecZero, g_vecZero, false, NULL ) );

		CopySquadMakerRenderVars( fix->pev, pev );
		CopySquadMakerOriginalRenderVars( fix, pev );
		fix->m_hMonster = entity;
		fix->pev->nextthink = gpGlobals->time + 0.15;
		fix->SetThink( &CSquadMakerRenderFix::RenderSet );
	}

	return child;
}
