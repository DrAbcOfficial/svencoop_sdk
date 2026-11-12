#ifndef ASBASEDOOR_H
#define ASBASEDOOR_H

#include "scripting/api/ASBaseToggle.h"
#include "world/doors.h"

template <class T>
void RegisterSCScriptBaseDoorEntity( CASDocumentation *documentation, const char *name )
{
	RegisterASScriptEntityHeaderCasts<CBaseDoor, T>( documentation, "CBaseDoor", name );
	RegisterSCScriptBaseToggleEntity<T>( documentation, name );

	documentation->RegisterObjectMethod(
	    "Door touch function.", name,
	    "void DoorTouch(CBaseEntity@ pOther)",
	    asMETHOD( T, DoorTouch ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Makes the door go up.", name,
	    "void DoorGoUp()",
	    asMETHOD( T, DoorGoUp ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Makes the door go down.", name,
	    "void DoorGoDown()",
	    asMETHOD( T, DoorGoDown ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "If the door is moving, changes the movement direction. Returns true if the door was moving.", name,
	    "bool DoorChangeDirection()",
	    asMETHOD( T, DoorChangeDirection ), asCALL_THISCALL );
}

#endif
