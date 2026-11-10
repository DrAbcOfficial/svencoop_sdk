#ifndef ASBASETANK_H
#define ASBASETANK_H

#include "ASBaseEntity.h"
#include "func_tank.h"

inline void RegisterSCScriptTankEntityDependencies( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "func_tank bullet types", "TANKBULLET" );
	documentation->RegisterEnumValue( "No bullet. Nothing is fired when this is set", "TANKBULLET",
	                                  "TANK_BULLET_NONE", TANK_BULLET_NONE );
	documentation->RegisterEnumValue( "9mm rounds", "TANKBULLET", "TANK_BULLET_9MM", TANK_BULLET_9MM );
	documentation->RegisterEnumValue( "MP5 rounds", "TANKBULLET", "TANK_BULLET_MP5", TANK_BULLET_MP5 );
	documentation->RegisterEnumValue( "12 mm rounds", "TANKBULLET", "TANK_BULLET_12MM", TANK_BULLET_12MM );

	documentation->RegisterEnum( "func_tank spawn flags", "TankFlags" );
	documentation->RegisterEnumValue( "Whether the tank is active or not", "TankFlags",
	                                  "SF_TANK_ACTIVE", SF_TANK_ACTIVE );
	documentation->RegisterEnumValue(
	    "Whether to only attack if the tank has a direct line of sight of its target or continue firing.",
	    "TankFlags", "SF_TANK_LINEOFSIGHT", SF_TANK_LINEOFSIGHT );
	documentation->RegisterEnumValue( "Whether this func_tank is controllable or not", "TankFlags",
	                                  "SF_TANK_CANCONTROL", SF_TANK_CANCONTROL );
	documentation->RegisterEnumValue(
	    "If set, uses the relationship specified when dealing with player targets. Otherwise, defaults to always "
	    "attacking monsters.",
	    "TankFlags", "SF_TANK_USERELATIONS", SF_TANK_USERELATIONS );
	documentation->RegisterEnumValue( "If set, controlling player can't fire this tank.", "TankFlags",
	                                  "SF_TANK_PLAYERCANTFIRE", SF_TANK_PLAYERCANTFIRE );
}

template <class T>
void BaseTankEntity_Fire( const Vector &barrelEnd, const Vector &forward, entvars_t *attacker, T *tank )
{
	tank->T::Fire( barrelEnd, forward, attacker );
}

template <class T>
Vector BaseTankEntity_UpdateTargetPosition( CBaseEntity *target, T *tank )
{
	return tank->T::UpdateTargetPosition( target );
}

template <class T>
int BaseTankEntity_BulletCount( T *tank )
{
	return tank->T::BulletCount();
}

inline void TankEntity_TankTrace( const Vector &start, const Vector &forward, const Vector &spread,
                                  TraceResult &trace, CFuncTank *tank )
{
	UTIL_MakeAimVectors( tank->pev->angles );
	tank->TankTrace( start, forward, spread, trace );
}

inline Vector TankEntity_AdjustAnglesForBarrel( const Vector &angles, float distance, CFuncTank *tank )
{
	Vector result = angles;

	tank->AdjustAnglesForBarrel( result, distance );
	return result;
}

template <class T>
void RegisterSCScriptBaseBaseClassTankEntity( CASDocumentation *documentation,
                                              const char *description, const char *name )
{
	RegisterSCScriptBaseBaseClassEntity<T>( documentation, description, name );
	RegisterASScriptEntityHeaderCasts<CFuncTank, T>( documentation, "BaseTank", name );

	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of Fire", name,
	    "void Fire(const Vector& in vecBarrelEnd, const Vector& in vecForward, entvars_t@ pevAttacker)",
	    asFUNCTION( BaseTankEntity_Fire<T> ), asCALL_CDECL_OBJLAST );

	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of UpdateTargetPosition", name,
	    "Vector UpdateTargetPosition(CBaseEntity@ pTargetEntity)",
	    asFUNCTION( BaseTankEntity_UpdateTargetPosition<T> ), asCALL_CDECL_OBJLAST );

	documentation->RegisterObjectMethod(
	    "Calls the base class implementation of BulletCount", name,
	    "int BulletCount()", asFUNCTION( BaseTankEntity_BulletCount<T> ), asCALL_CDECL_OBJLAST );
}

template <class T>
void RegisterSCScriptBaseTankEntity( CASDocumentation *documentation, const char *name )
{
	RegisterASScriptEntityHeaderCasts<CFuncTank, T>( documentation, "CBaseTank", name );
	RegisterSCScriptBaseEntity<T>( documentation, name, true );

	documentation->RegisterObjectMethod(
	    "Updates the tank's angles to match either its controlling player, or its current target.Accounts for "
	    "rotation speed. Will fire at the target if it's automated.",
	    name,
	    "void TrackTarget()",
	    asMETHOD( T, TrackTarget ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Points the tank at the target location.", name,
	    "bool PointAt(const Vector& in vecDestination)",
	    asMETHOD( T, PointAt ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Fires the tank. Barrel end position, aiming direction and attacker entvars_t must be given.", name,
	    "void Fire(const Vector& in vecBarrelEnd, const Vector& in vecForward, entvars_t@ pevAttacker)",
	    asMETHODPR( T, Fire, (const Vector &, const Vector &, entvars_t *), void ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Fires the tank. Uses this tank's settings to fire, does not determine if it is targeting anything.", name,
	    "void Fire()",
	    asMETHODPR( T, Fire, (), void ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the updated position of the given target. Target must be valid.", name,
	    "Vector UpdateTargetPosition(CBaseEntity@ pTargetEntity)",
	    asMETHOD( T, UpdateTargetPosition ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Start playing the rotating sound, if defined.", name,
	    "void StartRotSound()",
	    asMETHOD( T, StartRotSound ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Stop playing the rotating sound.", name,
	    "void StopRotSound()",
	    asMETHOD( T, StopRotSound ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether the tank is active. Active tanks are AI controlled.", name,
	    "bool IsActive() const",
	    asMETHOD( T, IsActive ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Activates the tank. Active tanks are AI controlled.", name,
	    "void TankActivate()",
	    asMETHOD( T, TankActivate ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Deactivates the tank. Active tanks are AI controlled.", name,
	    "void TankDeactivate()",
	    asMETHOD( T, TankDeactivate ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this tank is able to fire.", name,
	    "bool CanFire() const",
	    asMETHOD( T, CanFire ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether the given range is within the minimum and maximum range of this tank.", name,
	    "bool InRange(float flRange)",
	    asMETHOD( T, InRange ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Finds a target to attack, if any.", name,
	    "CBaseEntity@ FindTarget()",
	    asMETHOD( T, FindTarget ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Makes the given monster begin controlling this tank, if no other monster had control before.", name,
	    "void MonsterStartup(CBaseMonster@ pMonster)",
	    asMETHOD( T, MonsterStartup ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Performs a traceline using a random spread based on the given spread amount.", name,
	    "void TankTrace(const Vector& in vecStart, const Vector& in vecForward, const Vector& in vecSpread, "
	    "TraceResult& out tr)",
	    asFUNCTION( TankEntity_TankTrace ), asCALL_CDECL_OBJLAST );

	documentation->RegisterObjectMethod(
	    "Returns the barrel position.", name,
	    "Vector BarrelPosition()",
	    asMETHOD( T, BarrelPosition ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the bullet count at this time.", name,
	    "int BulletCount()",
	    asMETHOD( T, BulletCount ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Adjusts the given angles to point at the target.", name,
	    "Vector AdjustAnglesForBarrel(const Vector& in vecInAngles, float flDistance)",
	    asFUNCTION( TankEntity_AdjustAnglesForBarrel ), asCALL_CDECL_OBJLAST );

	documentation->RegisterObjectMethod(
	    "Makes the given player start controlling this tank.", name,
	    "bool StartControl(CBasePlayer@ pController)",
	    asMETHOD( T, StartControl ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "If a player is controlling this tank, makes the player stop controlling the tank.", name,
	    "void StopControl()",
	    asMETHOD( T, StopControl ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "If a monster is controlling this tank, makes the monster stop controlling the tank.", name,
	    "void MonsterStopControl()",
	    asMETHOD( T, MonsterStopControl ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Looks around itself in a box that is iDistance * 2 in width and height for possible targets and stores "
	    "the list in m_pLink.\nThe target stores the next target in the list in its m_pLink, and so on.",
	    name,
	    "void Look(int iDistance)",
	    asMETHOD( T, Look ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the current player controlling this tank, if any.", name,
	    "const CBasePlayer@ GetController()",
	    asMETHOD( T, GetController ), asCALL_THISCALL );

	documentation->RegisterObjectProperty(
	    "Start origin of the player that is currently controlling this tank.\nUsed to determine when a player has "
	    "moved too far to continue controlling this tank.",
	    name,
	    "Vector m_vecControllerUsePos", asOFFSET( T, m_vecControllerUsePos ) );

	documentation->RegisterObjectProperty(
	    "Next attack time", name,
	    "float m_flNextAttack", asOFFSET( T, m_flNextAttack ) );

	documentation->RegisterObjectProperty(
	    "\"Center\" yaw", name,
	    "float m_yawCenter", asOFFSET( T, m_yawCenter ) );

	documentation->RegisterObjectProperty(
	    "Max turn rate to track targets", name,
	    "float m_yawRate", asOFFSET( T, m_yawRate ) );

	documentation->RegisterObjectProperty(
	    "Range of turning motion (one-sided: 30 is +/- 30 degress from center)\nZero is full rotation", name,
	    "float m_yawRange", asOFFSET( T, m_yawRange ) );

	documentation->RegisterObjectProperty(
	    "Tolerance angle", name,
	    "float m_yawTolerance", asOFFSET( T, m_yawTolerance ) );

	documentation->RegisterObjectProperty(
	    "\"Center\" pitch", name,
	    "float m_pitchCenter", asOFFSET( T, m_pitchCenter ) );

	documentation->RegisterObjectProperty(
	    "Max turn rate on pitch", name,
	    "float m_pitchRate", asOFFSET( T, m_pitchRate ) );

	documentation->RegisterObjectProperty(
	    "Range of pitch motion as above", name,
	    "float m_pitchRange", asOFFSET( T, m_pitchRange ) );

	documentation->RegisterObjectProperty(
	    "Tolerance angle", name,
	    "float m_pitchTolerance", asOFFSET( T, m_pitchTolerance ) );

	documentation->RegisterObjectProperty(
	    "Last time I fired", name,
	    "float m_fireLast", asOFFSET( T, m_fireLast ) );

	documentation->RegisterObjectProperty(
	    "How many rounds/second", name,
	    "float m_fireRate", asOFFSET( T, m_fireRate ) );

	documentation->RegisterObjectProperty(
	    "Last time I saw target", name,
	    "float m_lastSightTime", asOFFSET( T, m_lastSightTime ) );

	documentation->RegisterObjectProperty(
	    "Persistence of firing (how long do I shoot when I can't see)", name,
	    "float m_persist", asOFFSET( T, m_persist ) );

	documentation->RegisterObjectProperty(
	    "Minimum range to aim/track", name,
	    "float m_minRange", asOFFSET( T, m_minRange ) );

	documentation->RegisterObjectProperty(
	    "Max range to aim/track", name,
	    "float m_maxRange", asOFFSET( T, m_maxRange ) );

	documentation->RegisterObjectProperty(
	    "Length of the freakin barrel", name,
	    "Vector m_barrelPos", asOFFSET( T, m_barrelPos ) );

	documentation->RegisterObjectProperty(
	    "Scale of any sprites we shoot", name,
	    "float m_spriteScale", asOFFSET( T, m_spriteScale ) );

	documentation->RegisterObjectProperty(
	    "Bullet type", name,
	    "TANKBULLET m_bulletType", asOFFSET( T, m_bulletType ) );

	documentation->RegisterObjectProperty(
	    "0 means use Bullet type's default damage", name,
	    "int m_iBulletDamage", asOFFSET( T, m_iBulletDamage ) );

	documentation->RegisterObjectProperty(
	    "Last sight of target", name,
	    "Vector m_sightOrigin", asOFFSET( T, m_sightOrigin ) );

	documentation->RegisterObjectProperty(
	    "firing spread", name,
	    "int m_spread", asOFFSET( T, m_spread ) );

	documentation->RegisterObjectProperty(
	    "Master entity (game_team_master or multisource)", name,
	    "string_t m_iszMaster", asOFFSET( T, m_iszMaster ) );
}

#endif
