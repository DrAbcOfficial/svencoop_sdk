#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "ASMonsterDependencies.h"
#include "CASDocumentation.h"
#include "CASLog.h"
#include "CObjectLifetimeHandler.h"
#include "CString.h"
#include "schedule.h"
#include "monsterevent.h"
#include "activity.h"
#include <new>

extern Schedule_t slIdleStand[];
extern Schedule_t slIdleTrigger[];
extern Schedule_t slIdleWalk[];
extern Schedule_t slWakeAngry[];
extern Schedule_t slAlertFace[];
extern Schedule_t slAlertStand[];
extern Schedule_t slCombatStand[];
extern Schedule_t slCombatFace[];
extern Schedule_t slReload[];
extern Schedule_t slRangeAttack1[];
extern Schedule_t slRangeAttack2[];
extern Schedule_t slChaseEnemy[];
extern Schedule_t slSmallFlinch[];
extern Schedule_t slDie[];
extern Schedule_t slError[];
extern Schedule_t slWalkToScript[];
extern Schedule_t slRunToScript[];
extern Schedule_t slWaitScript[];
extern Schedule_t slTakeCoverFromBestSound[];
extern Schedule_t slFail[];

static const size_t AS_MAX_OLD_ENEMIES = 4;
static const size_t AS_ROUTE_SIZE = 8;
static const Vector AS_VEC_HULL_MIN( -16, -16, -36 );
static const Vector AS_VEC_HULL_MAX( 16, 16, 36 );
static const Vector AS_VEC_HUMAN_HULL_MIN( -16, -16, 0 );
static const Vector AS_VEC_HUMAN_HULL_MAX( 16, 16, 72 );
static const Vector AS_VEC_HUMAN_HULL_DUCK( 16, 16, 36 );
static const Vector AS_VEC_VIEW( 0, 0, 28 );
static const Vector AS_VEC_DUCK_HULL_MIN( -16, -16, -18 );
static const Vector AS_VEC_DUCK_HULL_MAX( 16, 16, 18 );
static const Vector AS_VEC_DUCK_VIEW( 0, 0, 12 );

CString ActivityMap_GetName( int activity, const activity_map_t *map )
{
	return CString( map[activity].name );
}

void DefaultConstructScriptTask( void *memory )
{
	new ( memory ) Task_t;
}

void ConstructScriptTask( int task, float data, Task_t *memory )
{
	memory->iTask = task;
	memory->flData = data;
}

void DestructScriptTask( Task_t *task )
{
	task->~Task_t();
}

void DestructScriptSchedule( void *object )
{
	Schedule_t *schedule = (Schedule_t *) object;

	delete[] schedule->pTasklist;
	delete schedule;
}

Schedule_t *ConstructScriptSchedule( int interruptMask, int soundMask, const CString &name )
{
	Schedule_t *schedule = new Schedule_t;

	schedule->pTasklist = NULL;
	schedule->cTasks = 0;
	schedule->pName = NULL;
	schedule->iInterruptMask = interruptMask;
	schedule->iSoundMask = soundMask;
	schedule->pName = STRING( AllocStringLocal( name.Data() ) );
	g_MapObjectLifetimeHandler.AddObject( schedule, DestructScriptSchedule );

	return schedule;
}

Schedule_t *ScriptScheduleToScheduleCast( Schedule_t *schedule )
{
	return schedule;
}

void ScriptSchedule_AddTask( const Task_t &task, Schedule_t *schedule )
{
	Task_t *tasks = new Task_t[schedule->cTasks + 1];

	memmove( tasks, schedule->pTasklist, sizeof( Task_t ) * schedule->cTasks );
	tasks[schedule->cTasks] = task;
	delete[] schedule->pTasklist;
	schedule->cTasks++;
	schedule->pTasklist = tasks;
}

Task_t *Schedule_GetTask( int index, Schedule_t *schedule )
{
	if ( index < 0 || index >= schedule->cTasks )
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "Schedule::GetTask: invalid index!\n" );
		return NULL;
	}

	return &schedule->pTasklist[index];
}

CString Schedule_GetName( const Schedule_t *schedule )
{
	return CString( schedule->pName );
}

CString MonsterEvent_GetOptions( const MonsterEvent_t *event )
{
	return CString( event->options );
}

void RegisterMonsterConditions( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "these bits represent conditions that may befall the monster, of which some are allowed\n"
	                             "to interrupt certain schedules. ",
	                             "MonsterCondition", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "weapon needs to be reloaded!", "MonsterCondition", "bits_COND_NO_AMMO_LOADED", 1 );
	documentation->RegisterEnumValue( "see something that you hate", "MonsterCondition", "bits_COND_SEE_HATE", 2 );
	documentation->RegisterEnumValue( "see something that you are afraid of", "MonsterCondition", "bits_COND_SEE_FEAR", 4 );
	documentation->RegisterEnumValue( "see something that you dislike", "MonsterCondition", "bits_COND_SEE_DISLIKE", 8 );
	documentation->RegisterEnumValue( "target entity is in full view.", "MonsterCondition", "bits_COND_SEE_ENEMY", 16 );
	documentation->RegisterEnumValue( "target entity occluded by the world", "MonsterCondition", "bits_COND_ENEMY_OCCLUDED", 32 );
	documentation->RegisterEnumValue( "", "MonsterCondition", "bits_COND_SMELL_FOOD", 64 );
	documentation->RegisterEnumValue( "", "MonsterCondition", "bits_COND_ENEMY_TOOFAR", 128 );
	documentation->RegisterEnumValue( "hurt a little", "MonsterCondition", "bits_COND_LIGHT_DAMAGE", 256 );
	documentation->RegisterEnumValue( "hurt a lot", "MonsterCondition", "bits_COND_HEAVY_DAMAGE", 512 );
	documentation->RegisterEnumValue( "", "MonsterCondition", "bits_COND_CAN_RANGE_ATTACK1", 1024 );
	documentation->RegisterEnumValue( "", "MonsterCondition", "bits_COND_CAN_MELEE_ATTACK1", 2048 );
	documentation->RegisterEnumValue( "", "MonsterCondition", "bits_COND_CAN_RANGE_ATTACK2", 4096 );
	documentation->RegisterEnumValue( "", "MonsterCondition", "bits_COND_CAN_MELEE_ATTACK2", 0x2000 );
	documentation->RegisterEnumValue( "", "MonsterCondition", "bits_COND_PROVOKED", 0x8000 );
	documentation->RegisterEnumValue( "", "MonsterCondition", "bits_COND_NEW_ENEMY", 0x10000 );
	documentation->RegisterEnumValue( "there is an interesting sound", "MonsterCondition", "bits_COND_HEAR_SOUND", 0x20000 );
	documentation->RegisterEnumValue( "there is an interesting scent", "MonsterCondition", "bits_COND_SMELL", 0x40000 );
	documentation->RegisterEnumValue( "enemy is facing me", "MonsterCondition", "bits_COND_ENEMY_FACING_ME", 0x80000 );
	documentation->RegisterEnumValue( "enemy was killed. If you get this in combat, try to find another enemy. If you get it in alert, victory dance.", "MonsterCondition", "bits_COND_ENEMY_DEAD", 0x100000 );
	documentation->RegisterEnumValue( "see a client", "MonsterCondition", "bits_COND_SEE_CLIENT", 0x200000 );
	documentation->RegisterEnumValue( "see my nemesis", "MonsterCondition", "bits_COND_SEE_NEMESIS", 0x400000 );
	documentation->RegisterEnumValue( "", "MonsterCondition", "bits_COND_CAN_RANGE_ATTACK1_MOVING", 0x800000 );
	documentation->RegisterEnumValue( "", "MonsterCondition", "bits_COND_CAN_MELEE_ATTACK1_MOVING", 0x1000000 );
	documentation->RegisterEnumValue( "", "MonsterCondition", "bits_COND_CAN_RANGE_ATTACK2_MOVING", 0x2000000 );
	documentation->RegisterEnumValue( "", "MonsterCondition", "bits_COND_CAN_MELEE_ATTACK2_MOVING", 0x4000000 );
	documentation->RegisterEnumValue( "Defined by individual monster", "MonsterCondition", "bits_COND_SPECIAL1", 0x10000000 );
	documentation->RegisterEnumValue( "Defined by individual monster", "MonsterCondition", "bits_COND_SPECIAL2", 0x20000000 );
	documentation->RegisterEnumValue( "", "MonsterCondition", "bits_COND_TASK_FAILED", 0x40000000 );
	documentation->RegisterEnumValue( "", "MonsterCondition", "bits_COND_SCHEDULE_DONE", 0x80000000 );
	documentation->RegisterEnumValue( "This monster can use a func_tank now", "MonsterCondition", "bits_COND_CAN_USE_TANK", 0x4000 );
	documentation->RegisterEnumValue( "", "MonsterCondition", "bits_COND_ALL_SPECIAL", 805306368 );
	documentation->RegisterEnumValue( "", "MonsterCondition", "bits_COND_CAN_ATTACK", 15360 );
}

void RegisterMonsterScheduleTypes( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "Schedule types for monster AI", "SCHEDULE_TYPE", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_NONE", 0 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_IDLE_STAND", 1 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_IDLE_WALK", 2 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_WAKE_ANGRY", 3 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_WAKE_CALLED", 4 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_ALERT_FACE", 5 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_ALERT_SMALL_FLINCH", 6 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_ALERT_BIG_FLINCH", 7 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_ALERT_STAND", 8 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_INVESTIGATE_SOUND", 9 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_INVESTIGATE_COMBAT", 10 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_COMBAT_FACE", 11 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_COMBAT_STAND", 12 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_CHASE_ENEMY", 13 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_CHASE_ENEMY_FAILED", 14 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_VICTORY_DANCE", 15 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_TARGET_FACE", 16 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_TARGET_CHASE", 17 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_SMALL_FLINCH", 18 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_TAKE_COVER_FROM_ENEMY", 19 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_TAKE_COVER_FROM_BEST_SOUND", 20 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_TAKE_COVER_FROM_ORIGIN", 21 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_COWER", 22 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_MELEE_ATTACK1", 23 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_MELEE_ATTACK2", 24 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_RANGE_ATTACK1", 25 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_RANGE_ATTACK2", 26 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_SPECIAL_ATTACK1", 27 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_SPECIAL_ATTACK2", 28 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_STANDOFF", 29 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_ARM_WEAPON", 30 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_RELOAD", 31 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_GUARD", 32 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_AMBUSH", 33 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_DIE", 34 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_WAIT_TRIGGER", 35 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_WAIT_TILL_SEEN", 36 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_FOLLOW", 37 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_SLEEP", 38 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_WAKE", 39 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_BARNACLE_VICTIM_GRAB", 40 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_BARNACLE_VICTIM_CHOMP", 41 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_AISCRIPT", 42 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_FAIL", 43 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_TARGET_PLAYERFACE", 44 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_TARGET_PLAYERCHASE", 45 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_TARGET_PLAYERCHASE_FORCE", 46 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_MOVE_AWAY_PLAYERFOLLOW", 47 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_MOVE_TO_TANK", 48 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_WAIT_AT_TANK", 49 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_FIND_ATTACK_POINT", 50 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_LOSE_ENEMY", 51 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_RANGE_ATTACK1_DEFAULT", 52 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_SMALL_FLINCH_SPECIAL", 53 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_TELEPORT_FALL", 54 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_GUARD_POINT", 55 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "SCHED_PATH_WAYPOINT", 56 );
	documentation->RegisterEnumValue( "", "SCHEDULE_TYPE", "LAST_COMMON_SCHEDULE", 57 );
}

void RegisterMonsterTaskTypes( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "Task types for the monster AI", "TASK_TYPE", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_INVALID", 0 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_WAIT", 1 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_WAIT_FACE_ENEMY", 2 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_WAIT_PVS", 3 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_SUGGEST_STATE", 4 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_WALK_TO_TARGET", 5 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_RUN_TO_TARGET", 6 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_MOVE_TO_TARGET_RANGE", 7 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_GET_PATH_TO_ENEMY", 8 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_GET_PATH_TO_ENEMY_LKP", 9 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_GET_PATH_TO_ENEMY_CORPSE", 10 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_GET_PATH_TO_LEADER", 11 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_GET_PATH_TO_SPOT", 12 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_GET_PATH_TO_TARGET", 13 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_GET_PATH_TO_HINTNODE", 14 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_GET_PATH_TO_LASTPOSITION", 15 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_GET_PATH_TO_BESTSOUND", 16 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_GET_PATH_TO_BESTSCENT", 17 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_RUN_PATH", 18 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_WALK_PATH", 19 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_STRAFE_PATH", 20 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_CLEAR_MOVE_WAIT", 21 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_STORE_LASTPOSITION", 22 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_CLEAR_LASTPOSITION", 23 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_PLAY_ACTIVE_IDLE", 24 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_FIND_HINTNODE", 25 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_CLEAR_HINTNODE", 26 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_SMALL_FLINCH", 27 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_FACE_IDEAL", 28 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_FACE_ROUTE", 29 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_FACE_ENEMY", 30 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_FACE_HINTNODE", 31 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_FACE_TARGET", 32 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_FACE_LASTPOSITION", 33 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_RANGE_ATTACK1", 34 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_RANGE_ATTACK2", 35 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_MELEE_ATTACK1", 36 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_MELEE_ATTACK2", 37 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_RELOAD", 38 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_RANGE_ATTACK1_NOTURN", 39 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_RANGE_ATTACK2_NOTURN", 40 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_MELEE_ATTACK1_NOTURN", 41 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_MELEE_ATTACK2_NOTURN", 42 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_RELOAD_NOTURN", 43 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_SPECIAL_ATTACK1", 44 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_SPECIAL_ATTACK2", 45 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_CROUCH", 46 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_STAND", 47 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_GUARD", 48 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_STEP_LEFT", 49 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_STEP_RIGHT", 50 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_STEP_FORWARD", 51 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_STEP_BACK", 52 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_DODGE_LEFT", 53 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_DODGE_RIGHT", 54 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_SOUND_ANGRY", 55 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_SOUND_DEATH", 56 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_SET_ACTIVITY", 57 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_SET_SCHEDULE", 58 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_SET_FAIL_SCHEDULE", 59 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_CLEAR_FAIL_SCHEDULE", 60 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_PLAY_SEQUENCE", 61 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_PLAY_SEQUENCE_FACE_ENEMY", 62 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_PLAY_SEQUENCE_FACE_TARGET", 63 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_SOUND_IDLE", 64 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_SOUND_WAKE", 65 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_SOUND_PAIN", 66 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_SOUND_DIE", 67 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_FIND_COVER_FROM_BEST_SOUND", 68 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_FIND_COVER_FROM_ENEMY", 69 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_FIND_LATERAL_COVER_FROM_ENEMY", 70 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_FIND_NODE_COVER_FROM_ENEMY", 71 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_FIND_NEAR_NODE_COVER_FROM_ENEMY", 72 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_FIND_FAR_NODE_COVER_FROM_ENEMY", 73 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_FIND_COVER_FROM_ORIGIN", 74 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_EAT", 75 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_DIE", 76 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_WAIT_FOR_SCRIPT", 77 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_PLAY_SCRIPT", 78 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_ENABLE_SCRIPT", 79 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_PLANT_ON_SCRIPT", 80 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_FACE_SCRIPT", 81 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_WAIT_RANDOM", 82 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_WAIT_INDEFINITE", 83 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_STOP_MOVING", 84 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_TURN_LEFT", 85 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_TURN_RIGHT", 86 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_REMEMBER", 87 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_FORGET", 88 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_WAIT_FOR_MOVEMENT", 89 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_WAIT_FOR_MOVEMENT_AP", 90 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_WAIT_FOR_MOVEMENT_ENEMY_OCCLUDED", 91 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_WALK_PLAYERPATH_FOR_UNITS", 92 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_MOVE_AWAY_PLAYERPATH", 93 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_FIND_TANK", 94 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_GET_PATH_TO_TANK", 95 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_WAIT_AT_TANK", 96 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_FIND_ATTACKPOINT_TO_ENEMY", 97 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_WAIT_FOR_MOVEMENT_GRENADE", 98 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_MOVE_TO_TARGET_RANGE_FORCE", 99 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_PLAY_SEQUENCE_COWER", 100 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_USE_TANK", 101 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_RUN_TO_TANK", 102 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_FIND_COVER_DISTANCE_FROM_ENEMY", 103 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_GET_PATH_TO_BESTCOMBATSOUND", 104 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_MOVE_TO_ENEMY", 105 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_PATH_WAYPOINT_INITIALIZE", 106 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_PATH_WAYPOINT_MOVEMENT", 107 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_PATH_WAYPOINT_ANGLES", 108 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_PATH_WAYPOINT_ARRIVAL", 109 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_PATH_WAYPOINT_WAIT", 110 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "TASK_PATH_WAYPOINT_DEPARTURE", 111 );
	documentation->RegisterEnumValue( "", "TASK_TYPE", "LAST_COMMON_TASK", 113 );
}

void RegisterMonsterCapabilities( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "Tells us what a monster can do.\nSee MonsterEntity::m_afCapability", "bits_CAPABILITY", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "crouch", "bits_CAPABILITY", "bits_CAP_DUCK", 1 );
	documentation->RegisterEnumValue( "jump/leap", "bits_CAPABILITY", "bits_CAP_JUMP", 2 );
	documentation->RegisterEnumValue( "strafe ( walk/run sideways)", "bits_CAPABILITY", "bits_CAP_STRAFE", 4 );
	documentation->RegisterEnumValue( "can form squads", "bits_CAPABILITY", "bits_CAP_SQUAD", 8 );
	documentation->RegisterEnumValue( "proficiently navigate in water", "bits_CAPABILITY", "bits_CAP_SWIM", 16 );
	documentation->RegisterEnumValue( "climb ladders/ropes", "bits_CAPABILITY", "bits_CAP_CLIMB", 32 );
	documentation->RegisterEnumValue( "open doors/push buttons/pull levers", "bits_CAPABILITY", "bits_CAP_USE", 64 );
	documentation->RegisterEnumValue( "can hear forced sounds", "bits_CAPABILITY", "bits_CAP_HEAR", 128 );
	documentation->RegisterEnumValue( "can trigger auto doors", "bits_CAPABILITY", "bits_CAP_AUTO_DOORS", 256 );
	documentation->RegisterEnumValue( "can open manual doors", "bits_CAPABILITY", "bits_CAP_OPEN_DOORS", 512 );
	documentation->RegisterEnumValue( "can turn head, always bone controller 0", "bits_CAPABILITY", "bits_CAP_TURN_HEAD", 1024 );
	documentation->RegisterEnumValue( "can do a range attack 1", "bits_CAPABILITY", "bits_CAP_RANGE_ATTACK1", 2048 );
	documentation->RegisterEnumValue( "can do a range attack 2", "bits_CAPABILITY", "bits_CAP_RANGE_ATTACK2", 4096 );
	documentation->RegisterEnumValue( "can do a melee attack 1", "bits_CAPABILITY", "bits_CAP_MELEE_ATTACK1", 0x2000 );
	documentation->RegisterEnumValue( "can do a melee attack 2", "bits_CAPABILITY", "bits_CAP_MELEE_ATTACK2", 0x4000 );
	documentation->RegisterEnumValue( "can fly, move all around", "bits_CAPABILITY", "bits_CAP_FLY", 0x8000 );
	documentation->RegisterEnumValue( "can use func_tanks", "bits_CAPABILITY", "bits_CAP_USE_TANK", 0x10000 );
	documentation->RegisterEnumValue( "can take fall damage", "bits_CAPABILITY", "bits_CAP_FALL_DAMAGE", 0x20000 );
	documentation->RegisterEnumValue( "Combination of bits_CAP_USE, bits_CAP_AUTO_DOORS, and bits_CAP_OPEN_DOORS", "bits_CAPABILITY", "bits_CAP_DOORS_GROUP", 832 );
}

template <typename T>
void RegisterSCScriptBaseTask( CASDocumentation *documentation, const char *typeName )
{
	documentation->RegisterObjectProperty( "Task to execute.\nCan be monster-specific.", typeName, "int iTask", asOFFSET( T, iTask ) );
	documentation->RegisterObjectProperty( "Task data", typeName, "float flData", asOFFSET( T, flData ) );
}

template <typename T>
void RegisterSCScriptBaseSchedule( CASDocumentation *documentation, const char *typeName )
{
	documentation->RegisterObjectMethod( "Gets a task in this schedule.\nMust be between 0 and cTasks.", typeName, "Task@ GetTask(int iIndex) const", asFUNCTION( Schedule_GetTask ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectProperty( "Number of tasks in this schedule.", typeName, "int cTasks", asOFFSET( T, cTasks ) );
	documentation->RegisterObjectProperty( "A bit mask of conditions that can interrupt this schedule.", typeName, "int iInterruptMask", asOFFSET( T, iInterruptMask ) );
	documentation->RegisterObjectProperty( "A more specific mask that indicates which TYPES of sounds will interrupt the schedule in the event that the sch"
	                                       "edule is broken by COND_HEAR_SOUND.",
	                                       typeName, "int iSoundMask", asOFFSET( T, iSoundMask ) );
	documentation->RegisterObjectMethod( "Gets the name of this schedule.", typeName, "string szName() const", asFUNCTION( Schedule_GetName ), asCALL_CDECL_OBJLAST );
}

void RegisterSCScriptActivity( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "Activity currently being played by a monster", "Activity", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "Set m_Activity to this invalid value to force a reset to m_IdealActivity", "Activity", "ACT_RESET", 0 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_IDLE", 1 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_GUARD", 2 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_WALK", 3 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_RUN", 4 );
	documentation->RegisterEnumValue( "Fly (and flap if appropriate)", "Activity", "ACT_FLY", 5 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_SWIM", 6 );
	documentation->RegisterEnumValue( "vertical jump", "Activity", "ACT_HOP", 7 );
	documentation->RegisterEnumValue( "long forward jump", "Activity", "ACT_LEAP", 8 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_FALL", 9 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_LAND", 10 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_STRAFE_LEFT", 11 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_STRAFE_RIGHT", 12 );
	documentation->RegisterEnumValue( "tuck and roll, left", "Activity", "ACT_ROLL_LEFT", 13 );
	documentation->RegisterEnumValue( "tuck and roll, right", "Activity", "ACT_ROLL_RIGHT", 14 );
	documentation->RegisterEnumValue( "turn quickly left (stationary)", "Activity", "ACT_TURN_LEFT", 15 );
	documentation->RegisterEnumValue( "turn quickly right (stationary)", "Activity", "ACT_TURN_RIGHT", 16 );
	documentation->RegisterEnumValue( "the act of crouching down from a standing position", "Activity", "ACT_CROUCH", 17 );
	documentation->RegisterEnumValue( "holding body in crouched position (loops)", "Activity", "ACT_CROUCHIDLE", 18 );
	documentation->RegisterEnumValue( "the act of standing from a crouched position", "Activity", "ACT_STAND", 19 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_USE", 20 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_SIGNAL1", 21 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_SIGNAL2", 22 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_SIGNAL3", 23 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_TWITCH", 24 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_COWER", 25 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_SMALL_FLINCH", 26 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_BIG_FLINCH", 27 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_RANGE_ATTACK1", 28 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_RANGE_ATTACK2", 29 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_MELEE_ATTACK1", 30 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_MELEE_ATTACK2", 31 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_RELOAD", 32 );
	documentation->RegisterEnumValue( "pull out gun, for instance", "Activity", "ACT_ARM", 33 );
	documentation->RegisterEnumValue( "reholster gun", "Activity", "ACT_DISARM", 34 );
	documentation->RegisterEnumValue( "monster chowing on a large food item (loop)", "Activity", "ACT_EAT", 35 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_DIESIMPLE", 36 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_DIEBACKWARD", 37 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_DIEFORWARD", 38 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_DIEVIOLENT", 39 );
	documentation->RegisterEnumValue( "barnacle tongue hits a monster", "Activity", "ACT_BARNACLE_HIT", 40 );
	documentation->RegisterEnumValue( "barnacle is lifting the monster ( loop )", "Activity", "ACT_BARNACLE_PULL", 41 );
	documentation->RegisterEnumValue( "barnacle latches on to the monster", "Activity", "ACT_BARNACLE_CHOMP", 42 );
	documentation->RegisterEnumValue( "barnacle is holding the monster in its mouth ( loop )", "Activity", "ACT_BARNACLE_CHEW", 43 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_SLEEP", 44 );
	documentation->RegisterEnumValue( "for active idles, look at something on or near the floor", "Activity", "ACT_INSPECT_FLOOR", 45 );
	documentation->RegisterEnumValue( "for active idles, look at something directly ahead of you ( doesn't HAVE to be a wall or on a wall )", "Activity", "ACT_INSPECT_WALL", 46 );
	documentation->RegisterEnumValue( "alternate idle animation in which the monster is clearly agitated. (loop)", "Activity", "ACT_IDLE_ANGRY", 47 );
	documentation->RegisterEnumValue( "limp  (loop)", "Activity", "ACT_WALK_HURT", 48 );
	documentation->RegisterEnumValue( "limp  (loop)", "Activity", "ACT_RUN_HURT", 49 );
	documentation->RegisterEnumValue( "Idle while in flight", "Activity", "ACT_HOVER", 50 );
	documentation->RegisterEnumValue( "Fly (don't flap)", "Activity", "ACT_GLIDE", 51 );
	documentation->RegisterEnumValue( "Turn left in flight", "Activity", "ACT_FLY_LEFT", 52 );
	documentation->RegisterEnumValue( "Turn right in flight", "Activity", "ACT_FLY_RIGHT", 53 );
	documentation->RegisterEnumValue( "this means the monster smells a scent carried by the air", "Activity", "ACT_DETECT_SCENT", 54 );
	documentation->RegisterEnumValue( "this is the act of actually sniffing an item in front of the monster", "Activity", "ACT_SNIFF", 55 );
	documentation->RegisterEnumValue( "some large monsters can eat small things in one bite. This plays one time, EAT loops.", "Activity", "ACT_BITE", 56 );
	documentation->RegisterEnumValue( "without attacking, monster demonstrates that it is angry. (Yell, stick out chest, etc )", "Activity", "ACT_THREAT_DISPLAY", 57 );
	documentation->RegisterEnumValue( "monster just saw something that it is afraid of", "Activity", "ACT_FEAR_DISPLAY", 58 );
	documentation->RegisterEnumValue( "for some reason, monster is excited. Sees something he really likes to eat, or whatever.", "Activity", "ACT_EXCITED", 59 );
	documentation->RegisterEnumValue( "very monster specific special attacks.", "Activity", "ACT_SPECIAL_ATTACK1", 60 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_SPECIAL_ATTACK2", 61 );
	documentation->RegisterEnumValue( "agitated idle.", "Activity", "ACT_COMBAT_IDLE", 62 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_WALK_SCARED", 63 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_RUN_SCARED", 64 );
	documentation->RegisterEnumValue( "killed a player, do a victory dance.", "Activity", "ACT_VICTORY_DANCE", 65 );
	documentation->RegisterEnumValue( "die, hit in head. ", "Activity", "ACT_DIE_HEADSHOT", 66 );
	documentation->RegisterEnumValue( "die, hit in chest", "Activity", "ACT_DIE_CHESTSHOT", 67 );
	documentation->RegisterEnumValue( "die, hit in gut", "Activity", "ACT_DIE_GUTSHOT", 68 );
	documentation->RegisterEnumValue( "die, hit in back", "Activity", "ACT_DIE_BACKSHOT", 69 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_FLINCH_HEAD", 70 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_FLINCH_CHEST", 71 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_FLINCH_STOMACH", 72 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_FLINCH_LEFTARM", 73 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_FLINCH_RIGHTARM", 74 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_FLINCH_LEFTLEG", 75 );
	documentation->RegisterEnumValue( "", "Activity", "ACT_FLINCH_RIGHTLEG", 76 );
	documentation->RegisterObjectType( "Activity map", "CActivityMap", 0, asOBJ_REF | asOBJ_NOCOUNT );
	documentation->RegisterObjectMethod( "Gets the name of the given activity", "CActivityMap", "string GetName(Activity activity) const", asFUNCTION( ActivityMap_GetName ), asCALL_CDECL_OBJLAST );
	documentation->RegisterGlobalProperty( "Activity map instance", "CActivityMap g_ActivityMap", activity_map );
}

void RegisterDefaultSchedules( CASDocumentation *documentation )
{
	documentation->SetDefaultNamespace( "Schedules" );
	documentation->RegisterGlobalProperty( "", "Schedule slIdleStand", slIdleStand );
	documentation->RegisterGlobalProperty( "", "Schedule slIdleTrigger", slIdleTrigger );
	documentation->RegisterGlobalProperty( "", "Schedule slIdleWalk", slIdleWalk );
	documentation->RegisterGlobalProperty( "", "Schedule slWakeAngry", slWakeAngry );
	documentation->RegisterGlobalProperty( "", "Schedule slAlertFace", slAlertFace );
	documentation->RegisterGlobalProperty( "", "Schedule slAlertStand", slAlertStand );
	documentation->RegisterGlobalProperty( "", "Schedule slCombatStand", slCombatStand );
	documentation->RegisterGlobalProperty( "", "Schedule slCombatFace", slCombatFace );
	documentation->RegisterGlobalProperty( "", "Schedule slReload", slReload );
	documentation->RegisterGlobalProperty( "", "Schedule slRangeAttack1", slRangeAttack1 );
	documentation->RegisterGlobalProperty( "", "Schedule slRangeAttack2", slRangeAttack2 );
	documentation->RegisterGlobalProperty( "", "Schedule slChaseEnemy", slChaseEnemy );
	documentation->RegisterGlobalProperty( "", "Schedule slSmallFlinch", slSmallFlinch );
	documentation->RegisterGlobalProperty( "", "Schedule slDie", slDie );
	documentation->RegisterGlobalProperty( "", "Schedule slError", slError );
	documentation->RegisterGlobalProperty( "", "Schedule slWalkToScript", slWalkToScript );
	documentation->RegisterGlobalProperty( "", "Schedule slRunToScript", slRunToScript );
	documentation->RegisterGlobalProperty( "", "Schedule slWaitScript", slWaitScript );
	documentation->RegisterGlobalProperty( "", "Schedule slTakeCoverFromBestSound", slTakeCoverFromBestSound );
	documentation->RegisterGlobalProperty( "", "Schedule slFail", slFail );
	documentation->SetDefaultNamespace( "" );
}

void RegisterSCScriptMonsterEntityDependencies( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "The state of a monster's script.", "SCRIPTSTATE", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "Playing the sequence", "SCRIPTSTATE", "SCRIPT_PLAYING", 0 );
	documentation->RegisterEnumValue( "Waiting on everyone in the script to be ready", "SCRIPTSTATE", "SCRIPT_WAIT", 1 );
	documentation->RegisterEnumValue( "Cancelling the script / cleaning up", "SCRIPTSTATE", "SCRIPT_CLEANUP", 2 );
	documentation->RegisterEnumValue( "Walk to mark", "SCRIPTSTATE", "SCRIPT_WALK_TO_MARK", 3 );
	documentation->RegisterEnumValue( "Run to mark", "SCRIPTSTATE", "SCRIPT_RUN_TO_MARK", 4 );
	RegisterSCScriptActivity( documentation );
	documentation->RegisterEnum( "Current state of a monster", "MONSTERSTATE", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "", "MONSTERSTATE", "MONSTERSTATE_NONE", 0 );
	documentation->RegisterEnumValue( "", "MONSTERSTATE", "MONSTERSTATE_IDLE", 1 );
	documentation->RegisterEnumValue( "", "MONSTERSTATE", "MONSTERSTATE_COMBAT", 2 );
	documentation->RegisterEnumValue( "", "MONSTERSTATE", "MONSTERSTATE_ALERT", 3 );
	documentation->RegisterEnumValue( "", "MONSTERSTATE", "MONSTERSTATE_HUNT", 4 );
	documentation->RegisterEnumValue( "", "MONSTERSTATE", "MONSTERSTATE_PRONE", 5 );
	documentation->RegisterEnumValue( "", "MONSTERSTATE", "MONSTERSTATE_SCRIPT", 6 );
	documentation->RegisterEnumValue( "", "MONSTERSTATE", "MONSTERSTATE_PLAYDEAD", 7 );
	documentation->RegisterEnumValue( "", "MONSTERSTATE", "MONSTERSTATE_DEAD", 8 );
	documentation->RegisterObjectType( "Task for monsters to execute.", "Task", sizeof( Task_t ), asOBJ_REF | asOBJ_NOCOUNT );
	RegisterSCScriptBaseTask<Task_t>( documentation, "Task" );
	documentation->RegisterEnum( "These go in the flData member of TASK_WALK_TO_TARGET, TASK_RUN_TO_TARGET", "TaskData", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "Move normally", "TaskData", "TARGET_MOVE_NORMAL", 0 );
	documentation->RegisterEnumValue( "Move scripted", "TaskData", "TARGET_MOVE_SCRIPTED", 1 );
	documentation->RegisterObjectType( "Schedule for monsters to follow.", "Schedule", sizeof( Schedule_t ), asOBJ_REF | asOBJ_NOCOUNT );
	RegisterSCScriptBaseSchedule<Schedule_t>( documentation, "Schedule" );
	documentation->RegisterObjectType( "Script task class", "ScriptTask", sizeof( Task_t ), asOBJ_VALUE | asOBJ_APP_CLASS_CD );
	RegisterSCScriptBaseTask<Task_t>( documentation, "ScriptTask" );
	documentation->RegisterObjectBehaviour( "Constructor", "ScriptTask", asBEHAVE_CONSTRUCT, "void ScriptTask()", asFUNCTION( DefaultConstructScriptTask ), asCALL_CDECL_OBJLAST, NULL );
	documentation->RegisterObjectBehaviour( "Constructor", "ScriptTask", asBEHAVE_CONSTRUCT, "void ScriptTask(int iTask, float flData = 0.0f)", asFUNCTION( ConstructScriptTask ), asCALL_CDECL_OBJLAST, NULL );
	documentation->RegisterObjectBehaviour( "Destructor", "ScriptTask", asBEHAVE_DESTRUCT, "void DestructScriptTask()", asFUNCTION( DestructScriptTask ), asCALL_CDECL_OBJLAST, NULL );
	documentation->RegisterObjectType( "Script schedule class", "ScriptSchedule", sizeof( Schedule_t ), asOBJ_REF | asOBJ_NOCOUNT );
	RegisterSCScriptBaseSchedule<Schedule_t>( documentation, "ScriptSchedule" );
	documentation->RegisterObjectBehaviour( "Constructor", "ScriptSchedule", asBEHAVE_FACTORY, "ScriptSchedule@ ScriptSchedule(int iInterruptMask, int iSoundMask, const string& in szName)", asFUNCTION( ConstructScriptSchedule ), asCALL_CDECL, NULL );
	documentation->RegisterObjectMethod( "Cast to Schedule", "ScriptSchedule", "Schedule@ opImplCast()", asFUNCTION( ScriptScheduleToScheduleCast ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "Adds a task to the list", "ScriptSchedule", "void AddTask(const ScriptTask& in task)", asFUNCTION( ScriptSchedule_AddTask ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectType( "Waypoint for monsters to follow.", "Waypoint", sizeof( WayPoint_t ), asOBJ_REF | asOBJ_NOCOUNT );
	documentation->RegisterObjectProperty( "Location of this waypoint.", "Waypoint", "Vector vecLocation", asOFFSET( WayPoint_t, vecLocation ) );
	documentation->RegisterObjectProperty( "Saved jump velocity.", "Waypoint", "Vector vecJumpVelocity", asOFFSET( WayPoint_t, vecJumpVelocity ) );
	documentation->RegisterObjectProperty( "Saved ladder velocity.", "Waypoint", "Vector vecLadderVelocity", asOFFSET( WayPoint_t, vecLadderVelocity ) );
	documentation->RegisterObjectProperty( "Handle to a door to use.", "Waypoint", "EHandle hDoor", asOFFSET( WayPoint_t, hDoor ) );
	documentation->RegisterObjectProperty( "How long to wait on this waypoint.", "Waypoint", "float flSavedMoveWait", asOFFSET( WayPoint_t, flSavedMoveWait ) );
	documentation->RegisterObjectProperty( "Type(s) of this waypoint.", "Waypoint", "int iType", asOFFSET( WayPoint_t, iType ) );
	documentation->RegisterEnum( "these MoveFlag values are assigned to a WayPoint's TYPE in order to demonstrate the\n"
	                             "type of movement the monster should use to get there.",
	                             "MoveFlag", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "local move to targetent.", "MoveFlag", "bits_MF_TO_TARGETENT", 1 );
	documentation->RegisterEnumValue( "local move to enemy", "MoveFlag", "bits_MF_TO_ENEMY", 2 );
	documentation->RegisterEnumValue( "local move to a hiding place", "MoveFlag", "bits_MF_TO_COVER", 4 );
	documentation->RegisterEnumValue( "local move to detour point.", "MoveFlag", "bits_MF_TO_DETOUR", 8 );
	documentation->RegisterEnumValue( "local move to a path corner", "MoveFlag", "bits_MF_TO_PATHCORNER", 16 );
	documentation->RegisterEnumValue( "local move to a node", "MoveFlag", "bits_MF_TO_NODE", 32 );
	documentation->RegisterEnumValue( "local move to an arbitrary point", "MoveFlag", "bits_MF_TO_LOCATION", 64 );
	documentation->RegisterEnumValue( "this waypoint is the goal of the whole move.", "MoveFlag", "bits_MF_IS_GOAL", 128 );
	documentation->RegisterEnumValue( "Don't let the route code simplify this waypoint", "MoveFlag", "bits_MF_DONT_SIMPLIFY", 256 );
	documentation->RegisterEnumValue( "Jumping...", "MoveFlag", "bits_MF_TO_JUMP", 512 );
	documentation->RegisterEnumValue( "Ladder...", "MoveFlag", "bits_MF_TO_LADDER", 1024 );
	documentation->RegisterEnumValue( "Elevator", "MoveFlag", "bits_MF_TO_ELEVATOR", 2048 );
	RegisterMonsterConditions( documentation );
	documentation->RegisterEnum( "Damage over time types.", "CDMG", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "", "CDMG", "itbd_Paralyze", 0 );
	documentation->RegisterEnumValue( "", "CDMG", "itbd_NerveGas", 1 );
	documentation->RegisterEnumValue( "", "CDMG", "itbd_Poison", 2 );
	documentation->RegisterEnumValue( "", "CDMG", "itbd_Radiation", 3 );
	documentation->RegisterEnumValue( "", "CDMG", "itbd_DrownRecover", 4 );
	documentation->RegisterEnumValue( "", "CDMG", "itbd_Acid", 5 );
	documentation->RegisterEnumValue( "", "CDMG", "itbd_SlowBurn", 6 );
	documentation->RegisterEnumValue( "", "CDMG", "itbd_SlowFreeze", 7 );
	documentation->RegisterEnumValue( "Number of damage over time types", "CDMG", "CDMG_TIMEBASED", 8 );
	documentation->RegisterEnum( "these bits represent the monster's memory", "MonsterMemory", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "No memory.", "MonsterMemory", "MEMORY_CLEAR", 0 );
	documentation->RegisterEnumValue( "right now only used for houndeyes.", "MonsterMemory", "bits_MEMORY_PROVOKED", 1 );
	documentation->RegisterEnumValue( "monster knows it is in a covered position.", "MonsterMemory", "bits_MEMORY_INCOVER", 2 );
	documentation->RegisterEnumValue( "Ally is suspicious of the player, and will move to provoked more easily", "MonsterMemory", "bits_MEMORY_SUSPICIOUS", 4 );
	documentation->RegisterEnumValue( "Finished monster path (just used by big momma for now)", "MonsterMemory", "bits_MEMORY_PATH_FINISHED", 8 );
	documentation->RegisterEnumValue( "Moving on a path", "MonsterMemory", "bits_MEMORY_ON_PATH", 16 );
	documentation->RegisterEnumValue( "Movement has already failed", "MonsterMemory", "bits_MEMORY_MOVE_FAILED", 32 );
	documentation->RegisterEnumValue( "Has already flinched", "MonsterMemory", "bits_MEMORY_FLINCHED", 64 );
	documentation->RegisterEnumValue( "HACKHACK -- remember that I've already called my Killed()", "MonsterMemory", "bits_MEMORY_KILLED", 128 );
	documentation->RegisterEnumValue( "Can only get there within radius", "MonsterMemory", "bits_MEMORY_MOVE_RADIUS_ONLY", 256 );
	documentation->RegisterEnumValue( "Monster-specific memory", "MonsterMemory", "bits_MEMORY_CUSTOM4", 0x10000000 );
	documentation->RegisterEnumValue( "Monster-specific memory", "MonsterMemory", "bits_MEMORY_CUSTOM3", 0x20000000 );
	documentation->RegisterEnumValue( "Monster-specific memory", "MonsterMemory", "bits_MEMORY_CUSTOM2", 0x40000000 );
	documentation->RegisterEnumValue( "Monster-specific memory", "MonsterMemory", "bits_MEMORY_CUSTOM1", 0x80000000 );
	documentation->RegisterEnum( "trigger conditions for scripted AI", "AITRIGGER", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "No Trigger", "AITRIGGER", "AITRIGGER_NONE", 0 );
	documentation->RegisterEnumValue( "See Player", "AITRIGGER", "AITRIGGER_SEEPLAYER_ANGRY_AT_PLAYER", 1 );
	documentation->RegisterEnumValue( "Take Damage", "AITRIGGER", "AITRIGGER_TAKEDAMAGE", 2 );
	documentation->RegisterEnumValue( "50% Health Remaining", "AITRIGGER", "AITRIGGER_HALFHEALTH", 3 );
	documentation->RegisterEnumValue( "Death", "AITRIGGER", "AITRIGGER_DEATH", 4 );
	documentation->RegisterEnumValue( "Squad Member Dead", "AITRIGGER", "AITRIGGER_SQUADMEMBERDIE", 5 );
	documentation->RegisterEnumValue( "Squad Leader Dead", "AITRIGGER", "AITRIGGER_SQUADLEADERDIE", 6 );
	documentation->RegisterEnumValue( "Hear World", "AITRIGGER", "AITRIGGER_HEARWORLD", 7 );
	documentation->RegisterEnumValue( "Hear Player", "AITRIGGER", "AITRIGGER_HEARPLAYER", 8 );
	documentation->RegisterEnumValue( "Hear Combat", "AITRIGGER", "AITRIGGER_HEARCOMBAT", 9 );
	documentation->RegisterEnumValue( "See Player Unconditional", "AITRIGGER", "AITRIGGER_SEEPLAYER_UNCONDITIONAL", 10 );
	documentation->RegisterEnumValue( "See Player, Not In Combat", "AITRIGGER", "AITRIGGER_SEEPLAYER_NOT_IN_COMBAT", 11 );
	RegisterMonsterScheduleTypes( documentation );
	RegisterMonsterTaskTypes( documentation );
	documentation->RegisterEnum( "Monster event types", "MonsterEventType", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "", "MonsterEventType", "EVENT_SPECIFIC", 0 );
	documentation->RegisterEnumValue( "", "MonsterEventType", "EVENT_SCRIPTED", 1000 );
	documentation->RegisterEnumValue( "", "MonsterEventType", "EVENT_SHARED", 2000 );
	documentation->RegisterEnumValue( "", "MonsterEventType", "EVENT_CLIENT", 5000 );
	documentation->RegisterEnumValue( "", "MonsterEventType", "MONSTER_EVENT_BODYDROP_LIGHT", 2001 );
	documentation->RegisterEnumValue( "", "MonsterEventType", "MONSTER_EVENT_BODYDROP_HEAVY", 2002 );
	documentation->RegisterEnumValue( "", "MonsterEventType", "MONSTER_EVENT_FOOTSTEP", 2003 );
	documentation->RegisterEnumValue( "", "MonsterEventType", "MONSTER_EVENT_SWISHSOUND", 2010 );
	documentation->RegisterObjectType( "Monster event type", "MonsterEvent", sizeof( MonsterEvent_t ), asOBJ_REF | asOBJ_NOCOUNT );
	documentation->RegisterObjectProperty( "Event type.\nSee MonsterEventType enum", "MonsterEvent", "int event", asOFFSET( MonsterEvent_t, event ) );
	documentation->RegisterObjectMethod( "Options. Usually a sound name.", "MonsterEvent", "string options() const", asFUNCTION( MonsterEvent_GetOptions ), asCALL_CDECL_OBJLAST );
	RegisterMonsterCapabilities( documentation );
	documentation->RegisterEnum( "Hit Group standards", "HITGROUP", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "", "HITGROUP", "HITGROUP_GENERIC", 0 );
	documentation->RegisterEnumValue( "", "HITGROUP", "HITGROUP_HEAD", 1 );
	documentation->RegisterEnumValue( "", "HITGROUP", "HITGROUP_CHEST", 2 );
	documentation->RegisterEnumValue( "", "HITGROUP", "HITGROUP_STOMACH", 3 );
	documentation->RegisterEnumValue( "", "HITGROUP", "HITGROUP_LEFTARM", 4 );
	documentation->RegisterEnumValue( "", "HITGROUP", "HITGROUP_RIGHTARM", 5 );
	documentation->RegisterEnumValue( "", "HITGROUP", "HITGROUP_LEFTLEG", 6 );
	documentation->RegisterEnumValue( "", "HITGROUP", "HITGROUP_RIGHTLEG", 7 );
	RegisterDefaultSchedules( documentation );
	documentation->RegisterGlobalProperty( "How many old enemies to remember", "const size_t MAX_OLD_ENEMIES", (void *) &AS_MAX_OLD_ENEMIES );
	documentation->RegisterGlobalProperty( "How many waypoints a monster can store at one time", "const size_t ROUTE_SIZE", (void *) &AS_ROUTE_SIZE );
	documentation->RegisterGlobalProperty( "Default hull minimum. Used with CEntityFuncs::SetSize", "const Vector VEC_HULL_MIN", (void *) &AS_VEC_HULL_MIN );
	documentation->RegisterGlobalProperty( "Default hull maximum. Used with CEntityFuncs::SetSize", "const Vector VEC_HULL_MAX", (void *) &AS_VEC_HULL_MAX );
	documentation->RegisterGlobalProperty( "Default human hull maximum. Used with CEntityFuncs::SetSize", "const Vector VEC_HUMAN_HULL_MIN", (void *) &AS_VEC_HUMAN_HULL_MIN );
	documentation->RegisterGlobalProperty( "Default human hull maximum while standing. Used with CEntityFuncs::SetSize", "const Vector VEC_HUMAN_HULL_MAX", (void *) &AS_VEC_HUMAN_HULL_MAX );
	documentation->RegisterGlobalProperty( "Default human hull maximum while ducking. Used with CEntityFuncs::SetSize", "const Vector VEC_HUMAN_HULL_DUCK", (void *) &AS_VEC_HUMAN_HULL_DUCK );
	documentation->RegisterGlobalProperty( "View offset.", "const Vector VEC_VIEW", (void *) &AS_VEC_VIEW );
	documentation->RegisterGlobalProperty( "Hull minimum while ducking. Used with CEntityFuncs::SetSize", "const Vector VEC_DUCK_HULL_MIN", (void *) &AS_VEC_DUCK_HULL_MIN );
	documentation->RegisterGlobalProperty( "Hull maximum while ducking. Used with CEntityFuncs::SetSize", "const Vector VEC_DUCK_HULL_MAX", (void *) &AS_VEC_DUCK_HULL_MAX );
	documentation->RegisterGlobalProperty( "Hull view offset while ducking.", "const Vector VEC_DUCK_VIEW", (void *) &AS_VEC_DUCK_VIEW );
}
