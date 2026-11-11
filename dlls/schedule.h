/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/
//=========================================================
// Scheduling 
//=========================================================

#ifndef	SCHEDULE_H
#define	SCHEDULE_H

#include "EntityHandle.h"

class CBaseEntity;

#define	TASKSTATUS_NEW				0			// Just started
#define TASKSTATUS_RUNNING			1			// Running task & movement
#define TASKSTATUS_RUNNING_MOVEMENT	2			// Just running movement
#define TASKSTATUS_RUNNING_TASK		3			// Just running task
#define TASKSTATUS_COMPLETE			4			// Completed, get next task


//=========================================================
// These are the schedule types
//=========================================================
typedef enum
{
	SCHED_NONE = 0,
	SCHED_IDLE_STAND = 1,
	SCHED_IDLE_WALK = 2,
	SCHED_WAKE_ANGRY = 3,
	SCHED_WAKE_CALLED = 4,
	SCHED_ALERT_FACE = 5,
	SCHED_ALERT_SMALL_FLINCH = 6,
	SCHED_ALERT_BIG_FLINCH = 7,
	SCHED_ALERT_STAND = 8,
	SCHED_INVESTIGATE_SOUND = 9,
	SCHED_INVESTIGATE_COMBAT = 10,
	SCHED_COMBAT_FACE = 11,
	SCHED_COMBAT_STAND = 12,
	SCHED_CHASE_ENEMY = 13,
	SCHED_CHASE_ENEMY_FAILED = 14,
	SCHED_VICTORY_DANCE = 15,
	SCHED_TARGET_FACE = 16,
	SCHED_TARGET_CHASE = 17,
	SCHED_SMALL_FLINCH = 18,
	SCHED_TAKE_COVER_FROM_ENEMY = 19,
	SCHED_TAKE_COVER_FROM_BEST_SOUND = 20,
	SCHED_TAKE_COVER_FROM_ORIGIN = 21,
	SCHED_COWER = 22,
	SCHED_MELEE_ATTACK1 = 23,
	SCHED_MELEE_ATTACK2 = 24,
	SCHED_RANGE_ATTACK1 = 25,
	SCHED_RANGE_ATTACK2 = 26,
	SCHED_SPECIAL_ATTACK1 = 27,
	SCHED_SPECIAL_ATTACK2 = 28,
	SCHED_STANDOFF = 29,
	SCHED_ARM_WEAPON = 30,
	SCHED_RELOAD = 31,
	SCHED_GUARD = 32,
	SCHED_AMBUSH = 33,
	SCHED_DIE = 34,
	SCHED_WAIT_TRIGGER = 35,
	SCHED_WAIT_TILL_SEEN = 36,
	SCHED_FOLLOW = 37,
	SCHED_SLEEP = 38,
	SCHED_WAKE = 39,
	SCHED_BARNACLE_VICTIM_GRAB = 40,
	SCHED_BARNACLE_VICTIM_CHOMP = 41,
	SCHED_AISCRIPT = 42,
	SCHED_FAIL = 43,
	SCHED_TARGET_PLAYERFACE = 44,
	SCHED_TARGET_PLAYERCHASE = 45,
	SCHED_TARGET_PLAYERCHASE_FORCE = 46,
	SCHED_MOVE_AWAY_PLAYERFOLLOW = 47,
	SCHED_MOVE_TO_TANK = 48,
	SCHED_WAIT_AT_TANK = 49,
	SCHED_FIND_ATTACK_POINT = 50,
	SCHED_LOSE_ENEMY = 51,
	SCHED_RANGE_ATTACK1_DEFAULT = 52,
	SCHED_SMALL_FLINCH_SPECIAL = 53,
	SCHED_TELEPORT_FALL = 54,
	SCHED_GUARD_POINT = 55,
	SCHED_PATH_WAYPOINT = 56,
	LAST_COMMON_SCHEDULE = 57,
} SCHEDULE_TYPE;

//=========================================================
// These are the shared tasks
//=========================================================
typedef enum
{
	TASK_INVALID = 0,
	TASK_WAIT = 1,
	TASK_WAIT_FACE_ENEMY = 2,
	TASK_WAIT_PVS = 3,
	TASK_SUGGEST_STATE = 4,
	TASK_WALK_TO_TARGET = 5,
	TASK_RUN_TO_TARGET = 6,
	TASK_MOVE_TO_TARGET_RANGE = 7,
	TASK_GET_PATH_TO_ENEMY = 8,
	TASK_GET_PATH_TO_ENEMY_LKP = 9,
	TASK_GET_PATH_TO_ENEMY_CORPSE = 10,
	TASK_GET_PATH_TO_LEADER = 11,
	TASK_GET_PATH_TO_SPOT = 12,
	TASK_GET_PATH_TO_TARGET = 13,
	TASK_GET_PATH_TO_HINTNODE = 14,
	TASK_GET_PATH_TO_LASTPOSITION = 15,
	TASK_GET_PATH_TO_BESTSOUND = 16,
	TASK_GET_PATH_TO_BESTSCENT = 17,
	TASK_RUN_PATH = 18,
	TASK_WALK_PATH = 19,
	TASK_STRAFE_PATH = 20,
	TASK_CLEAR_MOVE_WAIT = 21,
	TASK_STORE_LASTPOSITION = 22,
	TASK_CLEAR_LASTPOSITION = 23,
	TASK_PLAY_ACTIVE_IDLE = 24,
	TASK_FIND_HINTNODE = 25,
	TASK_CLEAR_HINTNODE = 26,
	TASK_SMALL_FLINCH = 27,
	TASK_FACE_IDEAL = 28,
	TASK_FACE_ROUTE = 29,
	TASK_FACE_ENEMY = 30,
	TASK_FACE_HINTNODE = 31,
	TASK_FACE_TARGET = 32,
	TASK_FACE_LASTPOSITION = 33,
	TASK_RANGE_ATTACK1 = 34,
	TASK_RANGE_ATTACK2 = 35,
	TASK_MELEE_ATTACK1 = 36,
	TASK_MELEE_ATTACK2 = 37,
	TASK_RELOAD = 38,
	TASK_RANGE_ATTACK1_NOTURN = 39,
	TASK_RANGE_ATTACK2_NOTURN = 40,
	TASK_MELEE_ATTACK1_NOTURN = 41,
	TASK_MELEE_ATTACK2_NOTURN = 42,
	TASK_RELOAD_NOTURN = 43,
	TASK_SPECIAL_ATTACK1 = 44,
	TASK_SPECIAL_ATTACK2 = 45,
	TASK_CROUCH = 46,
	TASK_STAND = 47,
	TASK_GUARD = 48,
	TASK_STEP_LEFT = 49,
	TASK_STEP_RIGHT = 50,
	TASK_STEP_FORWARD = 51,
	TASK_STEP_BACK = 52,
	TASK_DODGE_LEFT = 53,
	TASK_DODGE_RIGHT = 54,
	TASK_SOUND_ANGRY = 55,
	TASK_SOUND_DEATH = 56,
	TASK_SET_ACTIVITY = 57,
	TASK_SET_SCHEDULE = 58,
	TASK_SET_FAIL_SCHEDULE = 59,
	TASK_CLEAR_FAIL_SCHEDULE = 60,
	TASK_PLAY_SEQUENCE = 61,
	TASK_PLAY_SEQUENCE_FACE_ENEMY = 62,
	TASK_PLAY_SEQUENCE_FACE_TARGET = 63,
	TASK_SOUND_IDLE = 64,
	TASK_SOUND_WAKE = 65,
	TASK_SOUND_PAIN = 66,
	TASK_SOUND_DIE = 67,
	TASK_FIND_COVER_FROM_BEST_SOUND = 68,
	TASK_FIND_COVER_FROM_ENEMY = 69,
	TASK_FIND_LATERAL_COVER_FROM_ENEMY = 70,
	TASK_FIND_NODE_COVER_FROM_ENEMY = 71,
	TASK_FIND_NEAR_NODE_COVER_FROM_ENEMY = 72,
	TASK_FIND_FAR_NODE_COVER_FROM_ENEMY = 73,
	TASK_FIND_COVER_FROM_ORIGIN = 74,
	TASK_EAT = 75,
	TASK_DIE = 76,
	TASK_WAIT_FOR_SCRIPT = 77,
	TASK_PLAY_SCRIPT = 78,
	TASK_ENABLE_SCRIPT = 79,
	TASK_PLANT_ON_SCRIPT = 80,
	TASK_FACE_SCRIPT = 81,
	TASK_WAIT_RANDOM = 82,
	TASK_WAIT_INDEFINITE = 83,
	TASK_STOP_MOVING = 84,
	TASK_TURN_LEFT = 85,
	TASK_TURN_RIGHT = 86,
	TASK_REMEMBER = 87,
	TASK_FORGET = 88,
	TASK_WAIT_FOR_MOVEMENT = 89,
	TASK_WAIT_FOR_MOVEMENT_AP = 90,
	TASK_WAIT_FOR_MOVEMENT_ENEMY_OCCLUDED = 91,
	TASK_WALK_PLAYERPATH_FOR_UNITS = 92,
	TASK_MOVE_AWAY_PLAYERPATH = 93,
	TASK_FIND_TANK = 94,
	TASK_GET_PATH_TO_TANK = 95,
	TASK_WAIT_AT_TANK = 96,
	TASK_FIND_ATTACKPOINT_TO_ENEMY = 97,
	TASK_WAIT_FOR_MOVEMENT_GRENADE = 98,
	TASK_MOVE_TO_TARGET_RANGE_FORCE = 99,
	TASK_PLAY_SEQUENCE_COWER = 100,
	TASK_USE_TANK = 101,
	TASK_RUN_TO_TANK = 102,
	TASK_FIND_COVER_DISTANCE_FROM_ENEMY = 103,
	TASK_GET_PATH_TO_BESTCOMBATSOUND = 104,
	TASK_MOVE_TO_ENEMY = 105,
	TASK_PATH_WAYPOINT_INITIALIZE = 106,
	TASK_PATH_WAYPOINT_MOVEMENT = 107,
	TASK_PATH_WAYPOINT_ANGLES = 108,
	TASK_PATH_WAYPOINT_ARRIVAL = 109,
	TASK_PATH_WAYPOINT_WAIT = 110,
	TASK_PATH_WAYPOINT_DEPARTURE = 111,
	TASK_GLIDE_TO_GROUND = 112,
	LAST_COMMON_TASK = 113,
} SHARED_TASKS;


// These go in the flData member of the TASK_WALK_TO_TARGET, TASK_RUN_TO_TARGET
enum 
{
	TARGET_MOVE_NORMAL = 0,
	TARGET_MOVE_SCRIPTED = 1,
};


// A goal should be used for a task that requires several schedules to complete.  
// The goal index should indicate which schedule (ordinally) the monster is running.  
// That way, when tasks fail, the AI can make decisions based on the context of the 
// current goal and sequence rather than just the current schedule.
enum
{
	GOAL_ATTACK_ENEMY,
	GOAL_MOVE,
	GOAL_TAKE_COVER,
	GOAL_MOVE_TARGET,
	GOAL_EAT,
};

// an array of tasks is a task list
// an array of schedules is a schedule list
struct Task_t
{

	int		iTask;
	float	flData;
};

struct Schedule_t
{

	Task_t	*pTasklist;
	int		cTasks;	 
	int		iInterruptMask;// a bit mask of conditions that can interrupt this schedule 
	
	// a more specific mask that indicates which TYPES of sounds will interrupt the schedule in the 
	// event that the schedule is broken by COND_HEAR_SOUND
	int		iSoundMask;
	const	char *pName;
};

// an array of waypoints makes up the monster's route. 
// !!!LATER- this declaration doesn't belong in this file.
struct WayPoint_t
{
	WayPoint_t()
		: vecLocation( 0, 0, 0 ), vecJumpVelocity( 0, 0, 0 ), vecLadderVelocity( 0, 0, 0 ),
		  flSavedMoveWait( 0 ), iType( 0 )
	{
	}

	Vector						vecLocation;
	Vector						vecJumpVelocity;
	Vector						vecLadderVelocity;
	EntityHandle<CBaseEntity>	hDoor;
	float						flSavedMoveWait;
	int							iType;
};

// these MoveFlag values are assigned to a WayPoint's TYPE in order to demonstrate the 
// type of movement the monster should use to get there.
#define bits_MF_TO_TARGETENT		( 1 << 0 ) // local move to targetent.
#define bits_MF_TO_ENEMY			( 1 << 1 ) // local move to enemy
#define bits_MF_TO_COVER			( 1 << 2 ) // local move to a hiding place
#define bits_MF_TO_DETOUR			( 1 << 3 ) // local move to detour point.
#define bits_MF_TO_PATHCORNER		( 1 << 4 ) // local move to a path corner
#define bits_MF_TO_NODE				( 1 << 5 ) // local move to a node
#define bits_MF_TO_LOCATION			( 1 << 6 ) // local move to an arbitrary point
#define bits_MF_IS_GOAL				( 1 << 7 ) // this waypoint is the goal of the whole move.
#define bits_MF_DONT_SIMPLIFY		( 1 << 8 ) // Don't let the route code simplify this waypoint
#define bits_MF_TO_JUMP				( 1 << 9 ) // Jumping...
#define bits_MF_TO_LADDER			( 1 << 10 ) // Ladder...
#define bits_MF_TO_ELEVATOR			( 1 << 11 ) // Elevator

// If you define any flags that aren't _TO_ flags, add them here so we can mask
// them off when doing compares.
#define bits_MF_NOT_TO_MASK (bits_MF_IS_GOAL | bits_MF_DONT_SIMPLIFY)

#define MOVEGOAL_NONE				(0)
#define MOVEGOAL_TARGETENT			(bits_MF_TO_TARGETENT)
#define MOVEGOAL_ENEMY				(bits_MF_TO_ENEMY)
#define MOVEGOAL_PATHCORNER			(bits_MF_TO_PATHCORNER)
#define MOVEGOAL_LOCATION			(bits_MF_TO_LOCATION)
#define MOVEGOAL_NODE				(bits_MF_TO_NODE)
#define MOVEGOAL_COVER				(bits_MF_TO_COVER)	// a location, routed without triangulation extensions

// these bits represent conditions that may befall the monster, of which some are allowed 
// to interrupt certain schedules. 
#define bits_COND_NO_AMMO_LOADED		( 1 << 0 ) // weapon needs to be reloaded!
#define	bits_COND_SEE_HATE				( 1 << 1 ) // see something that you hate
#define bits_COND_SEE_FEAR				( 1 << 2 ) // see something that you are afraid of
#define bits_COND_SEE_DISLIKE			( 1 << 3 ) // see something that you dislike
#define bits_COND_SEE_ENEMY				( 1 << 4 ) // target entity is in full view.
#define bits_COND_ENEMY_OCCLUDED		( 1 << 5 ) // target entity occluded by the world
#define bits_COND_SMELL_FOOD			( 1 << 6 )
#define bits_COND_ENEMY_TOOFAR			( 1 << 7 )
#define bits_COND_LIGHT_DAMAGE			( 1 << 8 ) // hurt a little 
#define bits_COND_HEAVY_DAMAGE			( 1 << 9 ) // hurt a lot
#define bits_COND_CAN_RANGE_ATTACK1		( 1 << 10)
#define bits_COND_CAN_MELEE_ATTACK1		( 1 << 11)
#define bits_COND_CAN_RANGE_ATTACK2		( 1 << 12)
#define bits_COND_CAN_MELEE_ATTACK2		( 1 << 13)
#define bits_COND_CAN_USE_TANK			( 1 << 14) // can use a func_tank
// #define bits_COND_CAN_RANGE_ATTACK3		( 1 << 14)
#define bits_COND_PROVOKED				( 1 << 15)
#define bits_COND_NEW_ENEMY				( 1 << 16)
#define bits_COND_HEAR_SOUND			( 1 << 17) // there is an interesting sound
#define bits_COND_SMELL					( 1 << 18) // there is an interesting scent
#define bits_COND_ENEMY_FACING_ME		( 1 << 19) // enemy is facing me
#define bits_COND_ENEMY_DEAD			( 1 << 20) // enemy was killed. If you get this in combat, try to find another enemy. If you get it in alert, victory dance.
#define bits_COND_SEE_CLIENT			( 1 << 21) // see a client
#define bits_COND_SEE_NEMESIS			( 1 << 22) // see my nemesis
#define bits_COND_CAN_RANGE_ATTACK1_MOVING	( 1 << 23) // the same attacks, while moving
#define bits_COND_CAN_MELEE_ATTACK1_MOVING	( 1 << 24)
#define bits_COND_CAN_RANGE_ATTACK2_MOVING	( 1 << 25)
#define bits_COND_CAN_MELEE_ATTACK2_MOVING	( 1 << 26)

#define bits_COND_SPECIAL1				( 1 << 28) // Defined by individual monster
#define bits_COND_SPECIAL2				( 1 << 29) // Defined by individual monster

// Clients can push player allies out of their way
#define bits_COND_CLIENT_PUSH			( bits_COND_SPECIAL1 )

#define bits_COND_TASK_FAILED			( 1 << 30)
#define bits_COND_SCHEDULE_DONE			( 1 << 31)


#define bits_COND_ALL_SPECIAL			(bits_COND_SPECIAL1 | bits_COND_SPECIAL2)

#define bits_COND_CAN_ATTACK			(bits_COND_CAN_RANGE_ATTACK1 | bits_COND_CAN_MELEE_ATTACK1 | bits_COND_CAN_RANGE_ATTACK2 | bits_COND_CAN_MELEE_ATTACK2)
#define bits_COND_CAN_ATTACK_MOVING		(bits_COND_CAN_RANGE_ATTACK1_MOVING | bits_COND_CAN_MELEE_ATTACK1_MOVING | bits_COND_CAN_RANGE_ATTACK2_MOVING | bits_COND_CAN_MELEE_ATTACK2_MOVING)

#endif	// SCHEDULE_H
