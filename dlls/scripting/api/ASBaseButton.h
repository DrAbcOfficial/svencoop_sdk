#ifndef ASBASEBUTTON_H
#define ASBASEBUTTON_H

#include "scripting/api/ASBaseToggle.h"

template <class T>
void RegisterSCScriptBaseButtonEntity( CASDocumentation *documentation, const char *name )
{
	RegisterASScriptEntityHeaderCasts<CBaseButton, T>( documentation, "CBaseButton", name );
	RegisterSCScriptBaseToggleEntity<T>( documentation, name );

	documentation->RegisterObjectMethod(
	    "Activates the button.", name,
	    "void ButtonActivate()",
	    asMETHOD( T, ButtonActivate ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Button Touch function.", name,
	    "void ButtonTouch(CBaseEntity@ pOther)",
	    asMETHOD( T, ButtonTouch ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Makes the button start sparking. The button will spark randomly after this.", name,
	    "void ButtonSpark()",
	    asMETHOD( T, ButtonSpark ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Button has reached the \"in/up\" position.  Activate its \"targets\", and pause before \"popping out\".", name,
	    "void TriggerAndWait()",
	    asMETHOD( T, TriggerAndWait ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Starts the button moving \"out/down\"", name,
	    "void ButtonReturn()",
	    asMETHOD( T, ButtonReturn ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Button has returned to start state.  Quiesce it.", name,
	    "void ButtonBackHome()",
	    asMETHOD( T, ButtonBackHome ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Button Use function.", name,
	    "void ButtonUse(CBaseEntity@ pActivator, CBaseEntity@ pCaller, USE_TYPE useType, float flValue)",
	    asMETHOD( T, ButtonUse ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the button response to a touch event.", name,
	    "ButtonCode ButtonResponseToTouch()",
	    asMETHOD( T, ButtonResponseToTouch ), asCALL_THISCALL );

	documentation->RegisterObjectProperty(
	    "button stays pushed in until touched again?", name,
	    "bool m_fStayPushed", asOFFSET( T, m_fStayPushed ) );

	documentation->RegisterObjectProperty(
	    "a rotating button?  default is a sliding button.", name,
	    "bool m_fRotating", asOFFSET( T, m_fRotating ) );
}

#endif
