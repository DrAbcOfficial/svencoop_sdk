#ifndef ASBASETOGGLE_H
#define ASBASETOGGLE_H

#include "ASBaseAnimating.h"

template <class T>
CInventoryRules *ToggleEntity_Get_m_pInventoryRules( T *entity )
{
	return entity->m_pInventoryRules;
}

template <class T>
void RegisterSCScriptBaseToggleEntity( CASDocumentation *documentation, const char *name )
{
	RegisterASScriptEntityHeaderCasts<CBaseToggle, T>( documentation, "CBaseToggle", name );
	RegisterSCScriptBaseAnimatingEntity<T>( documentation, name );

	documentation->RegisterObjectMethod(
	    "Performs linear movement.", name,
	    "void LinearMove(Vector vecDest, float flSpeed)",
	    asMETHOD( T, LinearMove ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Performs angular movement.", name,
	    "void AngularMove(Vector vecDestAngle, float flSpeed)",
	    asMETHOD( T, AngularMove ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this entity is locked by its master.\nSeparate from IsLockedByMaster due to limitations.", name,
	    "bool IsToggleLockedByMaster()",
	    asMETHOD( T, IsLockedByMaster ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "The inventory rules.", name,
	    "InventoryRules@ get_m_pInventoryRules() const",
	    asFUNCTION( ToggleEntity_Get_m_pInventoryRules<T> ), asCALL_CDECL_OBJLAST );

	documentation->RegisterObjectProperty(
	    "Current toggle state.", name,
	    "TOGGLE_STATE m_toggle_state", asOFFSET( T, m_toggle_state ) );

	documentation->RegisterObjectProperty(
	    "How far a door should slide or rotate.", name,
	    "float m_flMoveDistance", asOFFSET( T, m_flMoveDistance ) );

	documentation->RegisterObjectProperty(
	    "How long to wait before resetting.", name,
	    "float m_flWait", asOFFSET( T, m_flWait ) );

	documentation->RegisterObjectProperty(
	    "How much to stick out of a wall. Will recede further into walls if negative.", name,
	    "float m_flLip", asOFFSET( T, m_flLip ) );

	documentation->RegisterObjectProperty(
	    "For plats.", name,
	    "float m_flTWidth", asOFFSET( T, m_flTWidth ) );

	documentation->RegisterObjectProperty(
	    "For plats.", name,
	    "float m_flTLength", asOFFSET( T, m_flTLength ) );

	documentation->RegisterObjectProperty(
	    "Trigger_counter only: # of activations remaining.", name,
	    "int m_cTriggersLeft", asOFFSET( T, m_cTriggersLeft ) );

	documentation->RegisterObjectProperty(
	    "Height.", name,
	    "float m_flHeight", asOFFSET( T, m_flHeight ) );

	documentation->RegisterObjectProperty(
	    "Handle to the activator.", name,
	    "EHandle m_hActivator", asOFFSET( T, m_hActivator ) );

	documentation->RegisterObjectProperty(
	    "Closed position.", name,
	    "Vector m_vecPosition1", asOFFSET( T, m_vecPosition1 ) );

	documentation->RegisterObjectProperty(
	    "Open position.", name,
	    "Vector m_vecPosition2", asOFFSET( T, m_vecPosition2 ) );

	documentation->RegisterObjectProperty(
	    "Closed angle.", name,
	    "Vector m_vecAngle1", asOFFSET( T, m_vecAngle1 ) );

	documentation->RegisterObjectProperty(
	    "Open angle.", name,
	    "Vector m_vecAngle2", asOFFSET( T, m_vecAngle2 ) );

	documentation->RegisterObjectProperty(
	    "Final destination.", name,
	    "Vector m_vecFinalDest", asOFFSET( T, m_vecFinalDest ) );

	documentation->RegisterObjectProperty(
	    "Final angle.", name,
	    "Vector m_vecFinalAngle", asOFFSET( T, m_vecFinalAngle ) );

	documentation->RegisterObjectProperty(
	    "DMG_ damage type that the door or trigger does.", name,
	    "int m_bitsDamageInflict", asOFFSET( T, m_bitsDamageInflict ) );

	documentation->RegisterObjectProperty(
	    "This entity's master, if any.", name,
	    "string_t m_sMaster", asOFFSET( T, m_sMaster ) );
}

#endif
