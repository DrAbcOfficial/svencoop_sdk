#ifndef ASBASEMONSTER_H
#define ASBASEMONSTER_H

#include "scripting/api/ASBaseToggle.h"
#include "scripting/api/ASEntityHeaderSupport.h"
#include "CString.h"
#include "monsters/monsterevent.h"

template <class T>
void BaseMonsterEntity_SetupFriendly( T *entity )
{
	entity->T::SetupFriendly();
}

template <class T>
int BaseMonsterEntity_Classify( T *entity )
{
	return entity->T::Classify();
}

template <class T>
int BaseMonsterEntity_IgnoreConditions( T *entity )
{
	return entity->T::IgnoreConditions();
}

template <class T>
void BaseMonsterEntity_AlertSound( T *entity )
{
	entity->T::AlertSound();
}

template <class T>
void BaseMonsterEntity_PainSound( T *entity )
{
	entity->T::PainSound();
}

template <class T>
void BaseMonsterEntity_DeathSound( T *entity )
{
	entity->T::DeathSound();
}

template <class T>
int BaseMonsterEntity_ISoundMask( T *entity )
{
	return entity->T::ISoundMask();
}

template <class T>
bool BaseMonsterEntity_CheckRangeAttack1( float dot, float distance, T *entity )
{
	return entity->T::CheckRangeAttack1( dot, distance ) != FALSE;
}

template <class T>
bool BaseMonsterEntity_CheckRangeAttack1_Move( float dot, float distance, T *entity )
{
	return entity->T::CheckRangeAttack1_Move( dot, distance ) != FALSE;
}

template <class T>
bool BaseMonsterEntity_CheckRangeAttack2( float dot, float distance, T *entity )
{
	return entity->T::CheckRangeAttack2( dot, distance ) != FALSE;
}

template <class T>
bool BaseMonsterEntity_CheckRangeAttack2_Move( float dot, float distance, T *entity )
{
	return entity->T::CheckRangeAttack2_Move( dot, distance ) != FALSE;
}

template <class T>
bool BaseMonsterEntity_CheckMeleeAttack1( float dot, float distance, T *entity )
{
	return entity->T::CheckMeleeAttack1( dot, distance ) != FALSE;
}

template <class T>
bool BaseMonsterEntity_CheckMeleeAttack1_Move( float dot, float distance, T *entity )
{
	return entity->T::CheckMeleeAttack1_Move( dot, distance ) != FALSE;
}

template <class T>
bool BaseMonsterEntity_CheckMeleeAttack2( float dot, float distance, T *entity )
{
	return entity->T::CheckMeleeAttack2( dot, distance ) != FALSE;
}

template <class T>
bool BaseMonsterEntity_CheckMeleeAttack2_Move( float dot, float distance, T *entity )
{
	return entity->T::CheckMeleeAttack2_Move( dot, distance ) != FALSE;
}

template <class T>
void BaseMonsterEntity_CheckAmmo( T *entity )
{
	entity->T::CheckAmmo();
}

template <class T>
void BaseMonsterEntity_SetYawSpeed( T *entity )
{
	entity->T::SetYawSpeed();
}

template <class T>
void BaseMonsterEntity_StartTask( Task_t *task, T *entity )
{
	entity->T::StartTask( task );
}

template <class T>
void BaseMonsterEntity_RunTask( Task_t *task, T *entity )
{
	entity->T::RunTask( task );
}

template <class T>
void BaseMonsterEntity_HandleAnimEvent( MonsterEvent_t *event, T *entity )
{
	entity->T::HandleAnimEvent( event );
}

template <class T>
Schedule_t *BaseMonsterEntity_GetScheduleOfType( int type, T *entity )
{
	return entity->T::GetScheduleOfType( type );
}

template <class T>
Schedule_t *BaseMonsterEntity_GetSchedule( T *entity )
{
	return entity->T::GetSchedule();
}

template <class T>
Schedule_t *BaseMonsterEntity_ScheduleFromName( const CString &name, T *entity )
{
	return entity->T::ScheduleFromName( name.Data() );
}

template <class T>
void BaseMonsterEntity_RunAI( T *entity )
{
	entity->T::RunAI();
}

template <class T>
void BaseMonsterEntity_CallGibMonster( T *entity )
{
	entity->T::CallGibMonster();
}

template <class T>
void BaseMonsterEntity_GibMonster( T *entity )
{
	entity->T::GibMonster();
}

template <class T>
void RegisterSCScriptBaseBaseClassMonsterEntity( CASDocumentation *documentation, const char *description, const char *name )
{
	RegisterSCScriptBaseBaseClassEntity<T>( documentation, description, name );
	RegisterASScriptEntityHeaderCasts<CBaseMonster, T>( documentation, "BaseMonster", name );

	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of SetupFriendly", name,
	    "void SetupFriendly()",
	    asFUNCTION( BaseMonsterEntity_SetupFriendly<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of Classify", name,
	    "int Classify()",
	    asFUNCTION( BaseMonsterEntity_Classify<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of IgnoreConditions", name,
	    "int IgnoreConditions()",
	    asFUNCTION( BaseMonsterEntity_IgnoreConditions<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of AlertSound", name,
	    "void AlertSound()",
	    asFUNCTION( BaseMonsterEntity_AlertSound<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of PainSound", name,
	    "void PainSound()",
	    asFUNCTION( BaseMonsterEntity_PainSound<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of DeathSound", name,
	    "void DeathSound()",
	    asFUNCTION( BaseMonsterEntity_DeathSound<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of ISoundMask", name,
	    "int ISoundMask()",
	    asFUNCTION( BaseMonsterEntity_ISoundMask<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of CheckRangeAttack1", name,
	    "bool CheckRangeAttack1(float flDot, float flDist)",
	    asFUNCTION( BaseMonsterEntity_CheckRangeAttack1<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of CheckRangeAttack1_Move", name,
	    "bool CheckRangeAttack1_Move(float flDot, float flDist)",
	    asFUNCTION( BaseMonsterEntity_CheckRangeAttack1_Move<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of CheckRangeAttack2", name,
	    "bool CheckRangeAttack2(float flDot, float flDist)",
	    asFUNCTION( BaseMonsterEntity_CheckRangeAttack2<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of CheckRangeAttack2_Move", name,
	    "bool CheckRangeAttack2_Move(float flDot, float flDist)",
	    asFUNCTION( BaseMonsterEntity_CheckRangeAttack2_Move<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of CheckMeleeAttack1", name,
	    "bool CheckMeleeAttack1(float flDot, float flDist)",
	    asFUNCTION( BaseMonsterEntity_CheckMeleeAttack1<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of CheckMeleeAttack1_Move", name,
	    "bool CheckMeleeAttack1_Move(float flDot, float flDist)",
	    asFUNCTION( BaseMonsterEntity_CheckMeleeAttack1_Move<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of CheckMeleeAttack2", name,
	    "bool CheckMeleeAttack2(float flDot, float flDist)",
	    asFUNCTION( BaseMonsterEntity_CheckMeleeAttack2<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of CheckMeleeAttack2_Move", name,
	    "bool CheckMeleeAttack2_Move(float flDot, float flDist)",
	    asFUNCTION( BaseMonsterEntity_CheckMeleeAttack2_Move<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of CheckAmmo", name,
	    "void CheckAmmo()",
	    asFUNCTION( BaseMonsterEntity_CheckAmmo<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of SetYawSpeed", name,
	    "void SetYawSpeed()",
	    asFUNCTION( BaseMonsterEntity_SetYawSpeed<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of StartTask", name,
	    "void StartTask(Task@ pTask)",
	    asFUNCTION( BaseMonsterEntity_StartTask<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of RunTask", name,
	    "void RunTask(Task@ pTask)",
	    asFUNCTION( BaseMonsterEntity_RunTask<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of HandleAnimEvent", name,
	    "void HandleAnimEvent(MonsterEvent@ pEvent)",
	    asFUNCTION( BaseMonsterEntity_HandleAnimEvent<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of GetScheduleOfType", name,
	    "Schedule@ GetScheduleOfType(int iType)",
	    asFUNCTION( BaseMonsterEntity_GetScheduleOfType<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of GetSchedule", name,
	    "Schedule@ GetSchedule()",
	    asFUNCTION( BaseMonsterEntity_GetSchedule<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of ScheduleFromName", name,
	    "Schedule@ ScheduleFromName(const string& in szName)",
	    asFUNCTION( BaseMonsterEntity_ScheduleFromName<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of RunAI", name,
	    "void RunAI()",
	    asFUNCTION( BaseMonsterEntity_RunAI<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of CallGibMonster", name,
	    "void CallGibMonster()",
	    asFUNCTION( BaseMonsterEntity_CallGibMonster<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of GibMonster", name,
	    "void GibMonster()",
	    asFUNCTION( BaseMonsterEntity_GibMonster<T> ), asCALL_CDECL_OBJLAST );
}

class CScriptArray;

EHANDLE &MonsterEntity_Get_m_hOldEnemy( unsigned int index, CBaseMonster *entity );
Vector &MonsterEntity_Get_m_vecOldEnemy( unsigned int index, CBaseMonster *entity );
WayPoint_t &MonsterEntity_Get_m_Route( unsigned int index, CBaseMonster *entity );
signed char &MonsterEntity_Get_m_rgbTimeBasedDamage( unsigned int index, CBaseMonster *entity );
bool MonsterEntity_FInViewCone( const Vector &origin, CBaseMonster *entity );
Schedule_t *MonsterEntity_ScheduleInList( const CString &name, CScriptArray *array, CBaseMonster *entity );
Schedule_t *MonsterEntity_ScheduleFromName( const CString &name, CBaseMonster *entity );
void MonsterEntity_PlaySentence( const CString &sentence, float duration, float volume, float attenuation, CBaseMonster *entity );
void MonsterEntity_PlayScriptedSentence( const CString &sentence, float duration, float volume, float attenuation, bool concurrent, CBaseEntity *listener, CBaseMonster *entity );
void MonsterEntity_SetSequenceByName( const CString &sequence, CBaseMonster *entity );
CBaseEntity *MonsterEntity_DropItem( const CString &name, const Vector &position, const Vector &angles, CBaseMonster *entity );
CString MonsterEntity_Get_m_szTextureName( CBaseMonster *entity );

template <class T>
CInventoryItemNode *MonsterEntity_Get_m_pInventory( T *entity )
{
	return entity->m_pInventory;
}

template <class T>
void RegisterSCScriptBaseMonsterEntity( CASDocumentation *documentation, const char *name )
{
	RegisterASScriptEntityHeaderCasts<CBaseMonster, T>( documentation, "CBaseMonster", name );
	RegisterSCScriptBaseToggleEntity<T>( documentation, name );

	documentation->RegisterObjectProperty(
	    "the entity that the monster is fighting.", name,
	    "EHandle m_hEnemy", asOFFSET( T, m_hEnemy ) );
	documentation->RegisterObjectProperty(
	    "the entity that the monster is trying to reach.", name,
	    "EHandle m_hTargetEnt", asOFFSET( T, m_hTargetEnt ) );
	documentation->RegisterObjectProperty(
	    "Target tank to control.", name,
	    "EHandle m_hTargetTank", asOFFSET( T, m_hSelectedTank ) );
	documentation->RegisterObjectMethod(
	    "Target tank to control. Index must be between 0 and MAX_OLD_ENEMIES.", name,
	    "EHandle& m_hOldEnemy(size_t uiIndex)",
	    asFUNCTION( MonsterEntity_Get_m_hOldEnemy ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Target tank to control. Index must be between 0 and MAX_OLD_ENEMIES.", name,
	    "Vector& m_vecOldEnemy(size_t uiIndex)",
	    asFUNCTION( MonsterEntity_Get_m_vecOldEnemy ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectProperty(
	    "width of monster's field of view ( dot product ).", name,
	    "float m_flFieldOfView", asOFFSET( T, m_flFieldOfView ) );
	documentation->RegisterObjectProperty(
	    "if we're told to wait, this is the time that the wait will be over.", name,
	    "float m_flWaitFinished", asOFFSET( T, m_flWaitFinished ) );
	documentation->RegisterObjectProperty(
	    "if we're told to wait before moving, this is the time that the wait will be over.", name,
	    "float m_flMoveWaitFinished", asOFFSET( T, m_flMoveWaitFinished ) );
	documentation->RegisterObjectProperty(
	    "what the monster is doing (animation).", name,
	    "Activity m_Activity", asOFFSET( T, m_Activity ) );
	documentation->RegisterObjectProperty(
	    "monster should switch to this activity.", name,
	    "Activity m_IdealActivity", asOFFSET( T, m_IdealActivity ) );
	documentation->RegisterObjectProperty(
	    "gaitsequence.", name,
	    "Activity m_GaitActivity", asOFFSET( T, m_GaitActivity ) );
	documentation->RegisterObjectProperty(
	    "the last body region that took damage.", name,
	    "int m_LastHitGroup", asOFFSET( T, m_LastHitGroup ) );
	documentation->RegisterObjectProperty(
	    "monster's current state.", name,
	    "MONSTERSTATE m_MonsterState", asOFFSET( T, m_MonsterState ) );
	documentation->RegisterObjectProperty(
	    "monster should change to this state.", name,
	    "MONSTERSTATE m_IdealMonsterState", asOFFSET( T, m_IdealMonsterState ) );
	documentation->RegisterObjectProperty(
	    "Task status.", name,
	    "int m_iTaskStatus", asOFFSET( T, m_iTaskStatus ) );
	documentation->RegisterObjectProperty(
	    "Current schedule.", name,
	    "Schedule@ m_pSchedule", asOFFSET( T, m_pSchedule ) );
	documentation->RegisterObjectProperty(
	    "For land_on_ground schedules (remember last schedule and continue).", name,
	    "Schedule@ m_pScheduleSaved", asOFFSET( T, m_pScheduleSaved ) );
	documentation->RegisterObjectProperty(
	    "Schedule index.", name,
	    "int m_iScheduleIndex", asOFFSET( T, m_iScheduleIndex ) );
	documentation->RegisterObjectMethod(
	    "Positions of movement. Index must be between 0 and ROUTE_SIZE", name,
	    "Waypoint& m_Route(size_t uiIndex)",
	    asFUNCTION( MonsterEntity_Get_m_Route ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectProperty(
	    "Goal that defines route.", name,
	    "int m_movementGoal", asOFFSET( T, m_movementGoal ) );
	documentation->RegisterObjectProperty(
	    "Index into m_Route[].", name,
	    "int m_iRouteIndex", asOFFSET( T, m_iRouteIndex ) );
	documentation->RegisterObjectProperty(
	    "How long I should wait for something to move.", name,
	    "float m_moveWaitTime", asOFFSET( T, m_moveWaitTime ) );
	documentation->RegisterObjectProperty(
	    "Minimum radius.", name,
	    "float m_moveradius", asOFFSET( T, m_moveradius ) );
	documentation->RegisterObjectProperty(
	    "Kept around for node graph moves, so we know our ultimate goal.", name,
	    "Vector m_vecMoveGoal", asOFFSET( T, m_vecMoveGoal ) );
	documentation->RegisterObjectProperty(
	    "When moving, set this activity.", name,
	    "Activity m_movementActivity", asOFFSET( T, m_movementActivity ) );
	documentation->RegisterObjectProperty(
	    "first index of a linked list of sounds that the monster can hear.", name,
	    "int m_iAudibleList", asOFFSET( T, m_iAudibleList ) );
	documentation->RegisterObjectProperty(
	    "Sound types that can be heard.", name,
	    "int m_afSoundTypes", asOFFSET( T, m_afSoundTypes ) );
	documentation->RegisterObjectProperty(
	    "monster sometimes wants to return to where it started after an operation..", name,
	    "Vector m_vecLastPosition", asOFFSET( T, m_vecLastPosition ) );
	documentation->RegisterObjectProperty(
	    "this is the hint node that the monster is moving towards or performing active idle on..", name,
	    "int m_iHintNode", asOFFSET( T, m_iHintNode ) );
	documentation->RegisterObjectProperty(
	    "Monster memory.", name,
	    "int m_afMemory", asOFFSET( T, m_afMemory ) );
	documentation->RegisterObjectProperty(
	    "color of blood particles.", name,
	    "int m_bloodColor", asOFFSET( T, m_bloodColor ) );
	documentation->RegisterObjectProperty(
	    "keeps track of monster's maximum health value (for re-healing, etc).", name,
	    "int m_iMaxHealth", asOFFSET( T, m_iMaxHealth ) );
	documentation->RegisterObjectProperty(
	    "last known position of enemy. (enemy's origin).", name,
	    "Vector m_vecEnemyLKP", asOFFSET( T, m_vecEnemyLKP ) );
	documentation->RegisterObjectProperty(
	    "how much ammo is in the weapon (used to trigger reload anim sequences).", name,
	    "int m_cAmmoLoaded", asOFFSET( T, m_cAmmoLoaded ) );
	documentation->RegisterObjectProperty(
	    "tells us what a monster can/can't do.", name,
	    "int m_afCapability", asOFFSET( T, m_afCapability ) );
	documentation->RegisterObjectProperty(
	    "tells us what a monster can/can't do, while moving.", name,
	    "int m_afMoveShootCap", asOFFSET( T, m_afMoveShootCap ) );
	documentation->RegisterObjectProperty(
	    "cannot attack again until this time.", name,
	    "float m_flNextAttack", asOFFSET( T, m_flNextAttack ) );
	documentation->RegisterObjectProperty(
	    "what types of damage has monster (player) taken.", name,
	    "int m_bitsDamageType", asOFFSET( T, m_bitsDamageType ) );
	documentation->RegisterObjectMethod(
	    "time based damage counters, decr. 1 per 2 seconds. Index must be between 0 and CMG_TIMEBASED.", name,
	    "int8& m_rgbTimeBasedDamage(size_t uiIndex)",
	    asFUNCTION( MonsterEntity_Get_m_rgbTimeBasedDamage ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectProperty(
	    "how much damage did monster (player) last take.", name,
	    "float m_lastDamageAmount", asOFFSET( T, m_lastDamageAmount ) );
	documentation->RegisterObjectProperty(
	    "Time-based damage timer.", name,
	    "float m_tbdPrev", asOFFSET( T, m_tbdPrev ) );
	documentation->RegisterObjectProperty(
	    "Time based damage inflictor.", name,
	    "entvars_t@ pevTimeBasedInflictor", asOFFSET( T, m_pevPoisoner ) );
	documentation->RegisterObjectProperty(
	    "Schedule type to choose if current schedule fails.", name,
	    "int m_failSchedule", asOFFSET( T, m_failSchedule ) );
	documentation->RegisterObjectProperty(
	    "Time based damage inflictor.", name,
	    "float m_flHungryTime", asOFFSET( T, m_flHungryTime ) );
	documentation->RegisterObjectProperty(
	    "if enemy farther away than this, bits_COND_ENEMY_TOOFAR set in CheckEnemy.", name,
	    "float m_flDistTooFar", asOFFSET( T, m_flDistTooFar ) );
	documentation->RegisterObjectProperty(
	    "distance monster sees (Default 2048).", name,
	    "float m_flDistLook", asOFFSET( T, m_flDistLook ) );
	documentation->RegisterObjectProperty(
	    "for scripted AI, this is the condition that will cause the activation of the monster's TriggerTarget.", name,
	    "int m_iTriggerCondition", asOFFSET( T, m_iTriggerCondition ) );
	documentation->RegisterObjectProperty(
	    "Name of target that should be fired.", name,
	    "string_t m_iszTriggerTarget", asOFFSET( T, m_iszTriggerTarget ) );
	documentation->RegisterObjectProperty(
	    "HACK until we can query end of gun.", name,
	    "Vector m_HackedGunPos", asOFFSET( T, m_HackedGunPos ) );
	documentation->RegisterObjectProperty(
	    "internal cinematic state.", name,
	    "SCRIPTSTATE m_scriptState", asOFFSET( T, m_scriptState ) );
	documentation->RegisterObjectProperty(
	    "Cinematic entity.", name,
	    "EHandle m_hCine", asOFFSET( T, m_pCine ) );
	documentation->RegisterObjectProperty(
	    "Entity that is blocking cinematic execution.", name,
	    "EHandle m_hCineBlocker", asOFFSET( T, m_hCineBlocker ) );
	documentation->RegisterObjectMethod(
	    "Will make a monster angry at whomever activated it.", name,
	    "void MonsterUse(CBaseEntity@ pActivator, CBaseEntity@ pCaller, USE_TYPE useType, float flValue = 0.0f)",
	    asMETHOD( T, MonsterUse ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Look - Base class monster function to find enemies or\nfood by sight. iDistance is distance ( in units ) that "
	    "the\nmonster can see.\nSets the sight bits of the m_afConditions mask to indicate\nwhich types of entities were "
	    "sighted.\nFunction also sets the Looker's m_pLink\nto the head of a link list that contains all visible "
	    "ents.\n(linked via each ent's m_pLink field)",
	    name,
	    "void Look(int iDistance)",
	    asMETHOD( T, Look ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Runs this monster's AI.", name,
	    "void RunAI()",
	    asMETHOD( T, RunAI ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Listens for anything that is within audible distance.", name,
	    "void Listen()",
	    asMETHOD( T, Listen ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether this monster should fade on death.", name,
	    "bool ShouldFadeOnDeath()",
	    asMETHOD( T, ShouldFadeOnDeath ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "turns a monster towards its ideal_yaw", name,
	    "float ChangeYaw(int iSpeed)",
	    asMETHOD( T, ChangeYaw ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "turns a directional vector into a yaw value that points down that vector.", name,
	    "float VecToYaw(Vector vecDir)",
	    asMETHOD( T, VecToYaw ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "returns the difference ( in degrees ) between monster's current yaw and ideal_yaw.\nPositive result is left "
	    "turn, negative is right turn.",
	    name,
	    "float FlYawDiff()",
	    asMETHOD( T, FlYawDiff ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns the amount of force applied to this monster when the given amount of damage is done.", name,
	    "float DamageForce(float flDamage, int bitsDamageType)",
	    asMETHODPR( T, DamageForce, (float, int), float ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns the amount of force applied to this monster when the given amount of damage is done.", name,
	    "float DamageForce(float flDamage)",
	    asMETHODPR( T, DamageForce, (float), float ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "use this overridden to make some other friendly monsters", name,
	    "void SetupFriendly()",
	    asMETHOD( T, SetupFriendly ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Initializes the monster", name,
	    "void MonsterInit()",
	    asMETHOD( T, MonsterInit ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Call after animation/pose is set up", name,
	    "void MonsterInitDead()",
	    asMETHOD( T, MonsterInitDead ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Makes the monster become dead", name,
	    "void BecomeDead(float flSavedHealth)",
	    asMETHOD( T, BecomeDead ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Revives the monster", name,
	    "void Revive()",
	    asMETHOD( T, Revive ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Starts the monster", name,
	    "void StartMonster()",
	    asMETHOD( T, StartMonster ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Finds best visible enemy for attack", name,
	    "CBaseEntity@ BestVisibleEnemy()",
	    asMETHOD( T, BestVisibleEnemy ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "see if pEntity is in monster's view cone", name,
	    "bool FInViewCone(CBaseEntity@ pEntity)",
	    asMETHODPR( T, FInViewCone, (CBaseEntity *), BOOL ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "see if given location is in monster's view cone", name,
	    "bool FInViewCone(const Vector& in vecOrigin)",
	    asFUNCTION( MonsterEntity_FInViewCone ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "finds best visible enemy for attack", name,
	    "int CheckLocalMove(const Vector& in vecStart, const Vector& in vecEnd,CBaseEntity@ pTarget, float& out flDist, "
	    "Vector& out vecEndPosition, bool fOriginalCheck = false)",
	    asMETHOD( T, CheckLocalMove ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Starts the monster", name,
	    "void Move(float flInterval = 0.1f)",
	    asMETHOD( T, Move ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Starts the monster", name,
	    "void MoveExecute(CBaseEntity@ pTargetEnt, const Vector& in vecDir, float flInterval)",
	    asMETHOD( T, MoveExecute ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether this monster should advance its route", name,
	    "bool ShouldAdvanceRoute(float flWaypointDist)",
	    asMETHOD( T, ShouldAdvanceRoute ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the stopped activity", name,
	    "Activity GetStoppedActivity()",
	    asMETHOD( T, GetStoppedActivity ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Stops this monster moving", name,
	    "void Stop()",
	    asMETHOD( T, Stop ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "This will stop animation until you call ResetSequenceInfo() at some point in the future", name,
	    "void StopAnimation()",
	    asMETHOD( T, StopAnimation ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "this function will survey conditions and set appropriate conditions bits for attack types.", name,
	    "bool CheckRangeAttack1(float flDot, float flDist)",
	    asMETHOD( T, CheckRangeAttack1 ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "this function will survey conditions and set appropriate conditions bits for attack types.", name,
	    "bool CheckRangeAttack1_Move(float flDot, float flDist)",
	    asMETHOD( T, CheckRangeAttack1_Move ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "this function will survey conditions and set appropriate conditions bits for attack types.", name,
	    "bool CheckRangeAttack2(float flDot, float flDist)",
	    asMETHOD( T, CheckRangeAttack2 ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "this function will survey conditions and set appropriate conditions bits for attack types.", name,
	    "bool CheckRangeAttack2_Move(float flDot, float flDist)",
	    asMETHOD( T, CheckRangeAttack2_Move ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "this function will survey conditions and set appropriate conditions bits for attack types.", name,
	    "bool CheckMeleeAttack1(float flDot, float flDist)",
	    asMETHOD( T, CheckMeleeAttack1 ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "this function will survey conditions and set appropriate conditions bits for attack types.", name,
	    "bool CheckMeleeAttack1_Move(float flDot, float flDist)",
	    asMETHOD( T, CheckMeleeAttack1_Move ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "this function will survey conditions and set appropriate conditions bits for attack types.", name,
	    "bool CheckMeleeAttack2(float flDot, float flDist)",
	    asMETHOD( T, CheckMeleeAttack2 ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "this function will survey conditions and set appropriate conditions bits for attack types.", name,
	    "bool CheckMeleeAttack2_Move(float flDot, float flDist)",
	    asMETHOD( T, CheckMeleeAttack2_Move ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Checks whether the monster can use a func_tank. Return true if possible.", name,
	    "bool CheckTankUsage()",
	    asMETHOD( T, CheckTankUsage ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether this monster has a schedule.", name,
	    "bool FHaveSchedule()",
	    asMETHOD( T, FHaveSchedule ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether this monster's schedule is valid.", name,
	    "bool FScheduleValid()",
	    asMETHOD( T, FScheduleValid ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Clears this monster's schedule.", name,
	    "void ClearSchedule()",
	    asMETHOD( T, ClearSchedule ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether this monster's schedule is done.", name,
	    "bool FScheduleDone()",
	    asMETHOD( T, FScheduleDone ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Changes this monster's schedule.", name,
	    "void ChangeSchedule(Schedule@ pNewSchedule)",
	    asMETHOD( T, ChangeSchedule ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Moves to the next scheduled task.", name,
	    "bool NextScheduledTask()",
	    asMETHOD( T, NextScheduledTask ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns the schedule with name szName if it's in the list, NULL otherwise.", name,
	    "Schedule@ ScheduleInList(const string& in szName, array<Schedule@>@ pArray)",
	    asFUNCTION( MonsterEntity_ScheduleInList ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Returns the schedule with name szName if it exists, NULL otherwise.", name,
	    "Schedule@ ScheduleFromName(const string& in szName)",
	    asFUNCTION( MonsterEntity_ScheduleFromName ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Maintains the monster's schedule.", name,
	    "void MaintainSchedule()",
	    asMETHOD( T, MaintainSchedule ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Starts the given task.", name,
	    "void StartTask(Task@ pTask)",
	    asMETHOD( T, StartTask ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Runs the given task.", name,
	    "void RunTask(Task@ pTask)",
	    asMETHOD( T, RunTask ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets one of the available schedules of the given type.", name,
	    "Schedule@ GetScheduleOfType(int iType)",
	    asMETHOD( T, GetScheduleOfType ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the current schedule.", name,
	    "Schedule@ GetSchedule()",
	    asMETHOD( T, GetSchedule ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Called after the monster's schedule changes.", name,
	    "void ScheduleChange()",
	    asMETHOD( T, ScheduleChange ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether this monster can play its sequence.", name,
	    "bool CanPlaySequence(const bool bDisregardState, int iInterruptLevel)",
	    asMETHOD( T, CanPlaySequence ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether this monster can play its sentence.", name,
	    "bool CanPlaySentence(const bool bDisregardState)",
	    asMETHOD( T, CanPlaySentence ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Plays the given sentence.", name,
	    "void PlaySentence(const string& in szSentence, float duration, float volume, float attenuation)",
	    asFUNCTION( MonsterEntity_PlaySentence ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Plays the given scripted sentence.", name,
	    "void PlayScriptedSentence(const string& in szSentence, float duration, float volume, float attenuation, const "
	    "bool bConcurrent, CBaseEntity@ pListener)",
	    asFUNCTION( MonsterEntity_PlayScriptedSentence ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Stops playing its current sentence.", name,
	    "void SentenceStop()",
	    asMETHOD( T, SentenceStop ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the current task.", name,
	    "Task@ GetTask()",
	    asMETHOD( T, GetTask ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the ideal monster state.", name,
	    "MONSTERSTATE GetIdealState()",
	    asMETHOD( T, GetIdealState ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Sets the current activity.", name,
	    "void SetActivity(Activity newActivity)",
	    asMETHOD( T, SetActivity ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Sets the current gait activity.", name,
	    "void SetGaitActivity(Activity newActivity)",
	    asMETHOD( T, SetGaitActivity ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Sets the current sequence by name.", name,
	    "void SetSequenceByName(const string& in szSequence)",
	    asFUNCTION( MonsterEntity_SetSequenceByName ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Sets the current monster state.", name,
	    "void SetState(MONSTERSTATE state)",
	    asMETHOD( T, SetState ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Reports the AI state.", name,
	    "void ReportAIState()",
	    asMETHOD( T, ReportAIState ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Check attacks to the given enemy at the given distance.", name,
	    "void CheckAttacks(CBaseEntity@ pTarget, float flDist)",
	    asMETHOD( T, CheckAttacks ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Check attacks to the given enemy at the given distance while moving.", name,
	    "void CheckAttacks_Move(CBaseEntity@ pTarget, float flDist)",
	    asMETHOD( T, CheckAttacks_Move ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Check whether attacks are possible from the given position.", name,
	    "bool CheckAttacksFromPosition(CBaseEntity@ pTarget, float flDist, Vector vecPos)",
	    asMETHOD( T, CheckAttacksFromPosition ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "part of the Condition collection process, gets and stores data and conditions pertaining to a monster's "
	    "enemy.\nReturns true if Enemy LKP was updated.",
	    name,
	    "bool CheckEnemy(CBaseEntity@ pEnemy)",
	    asMETHOD( T, CheckEnemy ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Remember the last few enemies, always remember the player.", name,
	    "void PushEnemy(CBaseEntity@ pTarget, Vector& in vecLastKnownPos)",
	    asMETHOD( T, PushEnemy ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Try remembering the last few enemies.", name,
	    "bool PopEnemy()",
	    asMETHOD( T, PopEnemy ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "tries to build an entire node path from\nthe callers origin to the passed vector.\nIf this is  possible, "
	    "ROUTE_SIZE waypoints will be copied into the callers m_Route.\ntrue is returned if the operation  succeeds (path "
	    "is valid) or false if failed (no path  exists ).",
	    name,
	    "bool FGetNodeRoute(Vector vecDest)",
	    asMETHOD( T, FGetNodeRoute ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "TEST FGetNodeRoute.", name,
	    "bool TestFGetNodeRoute(Vector vecDest)",
	    asMETHOD( T, TestFGetNodeRoute ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Sets the task status to complete.", name,
	    "void TaskComplete()",
	    asMETHOD( T, TaskComplete ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Called when movement has completed.", name,
	    "void MovementComplete()",
	    asMETHOD( T, MovementComplete ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Sets the condition bit for a failed task.", name,
	    "void TaskFail()",
	    asMETHOD( T, TaskFail ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Sets the task status to running.", name,
	    "void TaskBegin()",
	    asMETHOD( T, TaskBegin ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether a task is running.", name,
	    "bool TaskIsRunning()",
	    asMETHOD( T, TaskIsRunning ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether the task is complete.", name,
	    "bool TaskIsComplete()",
	    asMETHOD( T, TaskIsComplete ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether movement is complete.", name,
	    "bool MovementIsComplete()",
	    asMETHOD( T, MovementIsComplete ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns an integer with all Conditions\nbits that are currently set and also set in the current\nschedule's "
	    "Interrupt mask.",
	    name,
	    "int IScheduleFlags()",
	    asMETHOD( T, IScheduleFlags ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "gets a yaw value for the caller that would\nface the supplied vector. Value is stuffed into the "
	    "monster's\nideal_yaw.",
	    name,
	    "void MakeIdealYaw(Vector vecTarget)",
	    asMETHOD( T, MakeIdealYaw ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Allows different yaw_speeds for each activity.", name,
	    "void SetYawSpeed()",
	    asMETHOD( T, SetYawSpeed ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns the MOVEGOAL for the given move flag.", name,
	    "int RouteClassify(int iMoveFlag)",
	    asMETHOD( T, RouteClassify ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Determine if we should ignore damage.", name,
	    "bool CheckAttacker(CBaseEntity@ pAttacker)",
	    asMETHOD( T, CheckAttacker ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Sets AI conditions.", name,
	    "void SetConditions(int iConditions)",
	    asMETHOD( T, SetConditions ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Clears AI conditions.", name,
	    "void ClearConditions(int iConditions)",
	    asMETHOD( T, ClearConditions ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether this monster has any of the given AI conditions.", name,
	    "bool HasConditions(int iConditions) const",
	    asMETHOD( T, HasConditions ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether this monster has all of the given AI conditions.", name,
	    "bool HasAllConditions(int iConditions) const",
	    asMETHOD( T, HasAllConditions ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns a pointer to the sound the monster should react to.", name,
	    "CSound@ PBestSound()",
	    asMETHOD( T, PBestSound ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether this monster should eat.", name,
	    "bool FShouldEat()",
	    asMETHOD( T, FShouldEat ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Make the monster 'full' for a while.", name,
	    "void Eat(float flFullDuration)",
	    asMETHOD( T, Eat ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Checks and, if necessary, fires the monster's trigger target.", name,
	    "bool FCheckAITrigger()",
	    asMETHOD( T, FCheckAITrigger ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether attacking at this time would result in friendly fire", name,
	    "bool NoFriendlyFire()",
	    asMETHODPR( T, NoFriendlyFire, (), bool ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether attacking at this time from the given location would result in friendly fire", name,
	    "bool NoFriendlyFire(Vector vecPos)",
	    asMETHODPR( T, NoFriendlyFire, ( Vector ), bool ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether attacking the given target at this time from the given location would result in friendly fire", name,
	    "bool NoFriendlyFire(Vector vecPos, CBaseEntity@ pTarget)",
	    asMETHODPR( T, NoFriendlyFire, (Vector, CBaseEntity *), bool ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether attacking the target location at this time would result in friendly fire", name,
	    "bool NoFriendlyFireToPos(Vector vecPos)",
	    asMETHOD( T, NoFriendlyFireToPos ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Tries to find the best suitable enemy for this monster.", name,
	    "bool GetEnemy()",
	    asMETHOD( T, GetEnemy ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gibs the monster.", name,
	    "void GibMonster()",
	    asMETHOD( T, GibMonster ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether the monster should be gibbed given the gib type.\nSee GIB enum.", name,
	    "bool ShouldGibMonster(int iGib)",
	    asMETHOD( T, ShouldGibMonster ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Determines whether to gib or fade this monster, and acts accordingly.", name,
	    "void CallGibMonster()",
	    asMETHOD( T, CallGibMonster ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether this monster has human gibs.", name,
	    "bool HasHumanGibs()",
	    asMETHOD( T, HasHumanGibs ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether this monster has alien gibs.", name,
	    "bool HasAlienGibs()",
	    asMETHOD( T, HasAlienGibs ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Fades this monster.\nNote: does not kill the monster.", name,
	    "void FadeMonster()",
	    asMETHOD( T, FadeMonster ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Shoots at the specified location.", name,
	    "Vector ShootAtEnemy(const Vector& in vecShootOrigin)",
	    asMETHOD( T, ShootAtEnemy ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "", name,
	    "Vector GetGunPosition()",
	    asMETHOD( T, GetGunPosition ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Plays this monster's death sound.", name,
	    "void DeathSound()",
	    asMETHOD( T, DeathSound ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Plays this monster's alert sound.", name,
	    "void AlertSound()",
	    asMETHOD( T, AlertSound ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Plays this monster's idle sound.", name,
	    "void IdleSound()",
	    asMETHOD( T, IdleSound ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Plays this monster's pain sound.", name,
	    "void PainSound()",
	    asMETHOD( T, PainSound ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Starts following its target.", name,
	    "void StartPlayerFollowing(CBaseEntity@ pLeader, bool bSilent = false)",
	    asMETHOD( T, StartPlayerFollowing ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Stops following its target.", name,
	    "void StopPlayerFollowing(const bool clearSchedule, bool bSilent = false)",
	    asMETHOD( T, StopPlayerFollowing ), asCALL_THISCALL );
	documentation->RegisterObjectProperty(
	    "Don't allow +USE until this time.", name,
	    "float m_useTime", asOFFSET( T, m_useTime ) );
	documentation->RegisterObjectMethod(
	    "Makes this monster remember the given memories.", name,
	    "void Remember(int iMemory)",
	    asMETHOD( T, Remember ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Makes this monster forget the given memories.", name,
	    "void Forget(int iMemory)",
	    asMETHOD( T, Forget ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether this monster remembers any of the given memories.", name,
	    "bool HasMemory(int iMemory) const",
	    asMETHOD( T, HasMemory ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether this monster remembers all of the given memories.", name,
	    "bool HasAllMemories(int iMemory) const",
	    asMETHOD( T, HasAllMemories ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Exits the scripted sequence this monster is currently in, if any.", name,
	    "bool ExitScriptedSequence()",
	    asMETHOD( T, ExitScriptedSequence ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Cleans up after a cinematic event.", name,
	    "bool CineCleanup()",
	    asMETHOD( T, CineCleanup ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Drops the given item from the given position at the given angle.", name,
	    "CBaseEntity@ DropItem(const string& in szItemName, const Vector& in vecPos, const Vector& in vecAng)",
	    asFUNCTION( MonsterEntity_DropItem ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Turns the shock glow effect on or off.", name,
	    "void ShockGlowEffect(bool fMode)",
	    asMETHOD( T, ShockGlowEffect ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Use method for following players (set only if friendly)", name,
	    "void FollowerPlayerUse(CBaseEntity@ pActivator, CBaseEntity@ pCaller, USE_TYPE useType, float flValue)",
	    asMETHOD( T, FollowerPlayerUse ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether this monster is following a player.", name,
	    "bool IsPlayerFollowing()",
	    asMETHOD( T, IsPlayerFollowing ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether this monster can follow a player.", name,
	    "bool CanPlayerFollow()",
	    asMETHOD( T, CanPlayerFollow ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Starts following a given player.", name,
	    "void StartPlayerFollowing(CBaseEntity@ pLeader)",
	    asMETHOD( T, StartPlayerFollowing ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Stops following a player.", name,
	    "void StopPlayerFollowing(const bool bClearSchedule)",
	    asMETHOD( T, StopPlayerFollowing ), asCALL_THISCALL );
	documentation->RegisterObjectProperty(
	    "The formatted name.\nFor better name outputs. E.g. \"Alien Slave\" rather than \"alien_slave\".", name,
	    "string_t m_FormattedName", asOFFSET( T, m_FormattedName ) );
	documentation->RegisterObjectMethod(
	    "Current texture name we're standing on.", name,
	    "string m_szTextureName()",
	    asFUNCTION( MonsterEntity_Get_m_szTextureName ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectProperty(
	    "Current texture type.\nSee TextureType enum.", name,
	    "int8 m_chTextureType", asOFFSET( T, m_chTextureType ) );
	documentation->RegisterObjectProperty(
	    "Whether this monster can fear creatures.", name,
	    "bool m_fCanFearCreatures", asOFFSET( T, m_fCheckCreatureDanger ) );
	documentation->RegisterObjectProperty(
	    "How long an npc will attempt to fire full auto.", name,
	    "float m_flAutomaticAttackTime", asOFFSET( T, m_flAutomaticAttackTime ) );
	documentation->RegisterObjectProperty(
	    "Current fall speed.", name,
	    "float m_flFallVelocity", asOFFSET( T, m_flFallVelocity ) );
	documentation->RegisterObjectMethod(
	    "Medic call sound.", name,
	    "void MedicCallSound(float flDistance = 0.0f)",
	    asMETHOD( T, MedicCallSound ), asCALL_THISCALL );
	documentation->RegisterObjectProperty(
	    "Monster will guard this entity and turn down follow requests.", name,
	    "EHandle m_hGuardEnt", asOFFSET( T, m_hGuardEnt ) );
	documentation->RegisterObjectProperty(
	    "Guard entity name.", name,
	    "string_t m_iszGuardEntName", asOFFSET( T, m_iszGuardEntName ) );
	documentation->RegisterObjectMethod(
	    "The inventory items this monster has", name,
	    "InventoryList@ get_m_pInventory() const",
	    asFUNCTION( MonsterEntity_Get_m_pInventory<T> ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Clears this monster's list of enemies", name,
	    "void ClearEnemyList()",
	    asMETHOD( T, ClearEnemyList ), asCALL_THISCALL );
	documentation->RegisterObjectProperty(
	    "Glow shell.", name,
	    "Vector m_vecEffectGlowColor", asOFFSET( T, m_vecEffectGlowColor ) );
	documentation->RegisterObjectProperty(
	    "Monster can't use weapons.", name,
	    "int m_iEffectBlockWeapons", asOFFSET( T, m_iEffectBlockWeapons ) );
	documentation->RegisterObjectProperty(
	    "is invulnerable (god mode)", name,
	    "int m_iEffectInvulnerable", asOFFSET( T, m_iEffectInvulnerable ) );
	documentation->RegisterObjectProperty(
	    "is invisible (render + non-targetable)", name,
	    "int m_iEffectInvisible", asOFFSET( T, m_iEffectInvisible ) );
	documentation->RegisterObjectProperty(
	    "is non-solid", name,
	    "int m_iEffectNonSolid", asOFFSET( T, m_iEffectNonSolid ) );
	documentation->RegisterObjectProperty(
	    "Extra/less breathing time underwater in seconds", name,
	    "float m_flEffectRespiration", asOFFSET( T, m_flEffectRespiration ) );
	documentation->RegisterObjectProperty(
	    "Gravity modifier (%)", name,
	    "float m_flEffectGravity", asOFFSET( T, m_flEffectGravity ) );
	documentation->RegisterObjectProperty(
	    "Movement friction modifier (%)", name,
	    "float m_flEffectFriction", asOFFSET( T, m_flEffectFriction ) );
	documentation->RegisterObjectProperty(
	    "Movement speed modifier (%)", name,
	    "float m_flEffectSpeed", asOFFSET( T, m_flEffectSpeed ) );
	documentation->RegisterObjectProperty(
	    "Damage modifier (%)", name,
	    "float m_flEffectDamage", asOFFSET( T, m_flEffectDamage ) );
	documentation->RegisterObjectMethod(
	    "Re-calculate compounded effect modifiers and apply to the monster", name,
	    "void ApplyEffects()",
	    asMETHOD( T, ApplyEffects ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Reset all effect modifiers", name,
	    "void ResetEffects()",
	    asMETHOD( T, ResetEffects ), asCALL_THISCALL );
}

#endif
