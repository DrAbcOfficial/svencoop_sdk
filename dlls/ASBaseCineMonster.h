#ifndef ASBASECINEMONSTER_H
#define ASBASECINEMONSTER_H

#include "ASBaseMonster.h"
#include "scripted.h"

bool CineMonsterEntity_StartSequence( CBaseMonster *target, const CString &sequence, bool completeOnEmpty, CCineMonster *cine );

template <class T>
void RegisterSCScriptBaseCineMonsterEntity( CASDocumentation *documentation, const char *name )
{
	RegisterASScriptEntityHeaderCasts<CCineMonster, T>( documentation, "CCineMonster", name );
	RegisterSCScriptBaseMonsterEntity<T>( documentation, name );

	documentation->RegisterObjectMethod(
	    "Find all the cinematic entities with my targetname and tell them to wait before starting.", name,
	    "void DelayStart(int iState)",
	    asMETHOD( T, DelayStart ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Make the entity enter a scripted sequence.", name,
	    "void PossessEntity()",
	    asMETHOD( T, PossessEntity ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Find all the cinematic entities with my targetname and stop them from playing.", name,
	    "void CancelScript()",
	    asMETHOD( T, CancelScript ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Make the entity enter a scripted sequence.", name,
	    "bool StartSequence(CBaseMonster@ pTarget, const string& in szSequence, const bool fCompleteOnEmpty)",
	    asFUNCTION( CineMonsterEntity_StartSequence ), asCALL_CDECL_OBJLAST );

	documentation->RegisterObjectMethod(
	    "If this returns false, scripted sequences cannot possess entities regardless of state.", name,
	    "bool FCanOverrideState() const",
	    asMETHOD( T, FCanOverrideState ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether this script can be interrupted.", name,
	    "bool CanInterrupt()",
	    asMETHOD( T, CanInterrupt ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the ignore conditions.", name,
	    "int IgnoreConditions()",
	    asMETHOD( T, IgnoreConditions ), asCALL_THISCALL );

	documentation->RegisterObjectProperty(
	    "Te idle animation name.", name,
	    "string_t m_iszIdle", asOFFSET( T, m_iszIdle ) );

	documentation->RegisterObjectProperty(
	    "The play animation name.", name,
	    "string_t m_iszPlay", asOFFSET( T, m_iszPlay ) );

	documentation->RegisterObjectProperty(
	    "The target entity name.", name,
	    "string_t m_iszEntity", asOFFSET( T, m_iszEntity ) );

	documentation->RegisterObjectProperty(
	    "The radius to search in.", name,
	    "float m_flRadius", asOFFSET( T, m_flRadius ) );

	documentation->RegisterObjectProperty(
	    "How much time has to pass before the scripted sequence can repeat.", name,
	    "float m_flRepeat", asOFFSET( T, m_flRepeat ) );

	documentation->RegisterObjectProperty(
	    "Delay before execution.", name,
	    "int m_iDelay", asOFFSET( T, m_iDelay ) );

	documentation->RegisterObjectProperty(
	    "When to start the script.", name,
	    "float m_startTime", asOFFSET( T, m_startTime ) );

	documentation->RegisterObjectProperty(
	    "Whether this scripted sequence can be interrupted.", name,
	    "bool m_interruptable", asOFFSET( T, m_interruptable ) );

	documentation->RegisterObjectProperty(
	    "When in radius, script is done.", name,
	    "float m_flMoveToRadius", asOFFSET( T, m_flMoveToRadius ) );
}

#endif
