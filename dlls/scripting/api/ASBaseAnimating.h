#ifndef ASBASEANIMATING_H
#define ASBASEANIMATING_H

#include "scripting/api/ASBaseEntity.h"

int AnimatingEntity_LookupSequence( const CString &label, CBaseAnimating *entity );
int AnimatingEntity_FindTransition( int endingSequence, int goalSequence, int &direction, CBaseAnimating *entity );
int AnimatingEntity_FindTransition( int endingSequence, int goalSequence, CBaseAnimating *entity );
bool AnimatingEntity_ExtractBbox( int sequence, Vector &mins, Vector &maxs, CBaseAnimating *entity );

template <class T>
void RegisterSCScriptBaseAnimatingEntity( CASDocumentation *documentation, const char *name )
{
	RegisterASScriptEntityHeaderCasts<CBaseAnimating, T>( documentation, "CBaseAnimating", name );
	RegisterSCScriptBaseDelayEntity<T>( documentation, name );

	documentation->RegisterObjectMethod(
	    "Accumulate animation frame time from last time called until now.", name,
	    "float StudioFrameAdvance(float flInterval = 0.0f)",
	    asMETHOD( T, StudioFrameAdvance ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Get sequence flags.", name,
	    "int GetSequenceFlags()",
	    asMETHOD( T, GetSequenceFlags ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns the index of the given activity, or -1 if no such activity exists.", name,
	    "int LookupActivity(int iActivity)",
	    asMETHOD( T, LookupActivity ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns the index of the given activity that has the heaviest weight, or -1 if no such activity exists.", name,
	    "int LookupActivityHeaviest(int iActivity)",
	    asMETHOD( T, LookupActivityHeaviest ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns the index of the given sequence, or -1 if no such sequence exists.", name,
	    "int LookupSequence(const string& in szLabel)",
	    asFUNCTION( AnimatingEntity_LookupSequence ), asCALL_CDECL_OBJLAST );

	documentation->RegisterObjectMethod(
	    "Resets sequence info.", name,
	    "void ResetSequenceInfo()",
	    asMETHOD( T, ResetSequenceInfo ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Resets gait sequence info.", name,
	    "void ResetGaitSequenceInfo()",
	    asMETHOD( T, ResetGaitSequenceInfo ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Handle events that have happend since last time called up until X seconds into the future.", name,
	    "void DispatchAnimEvents(float flFutureInterval = 0.1f)",
	    asMETHOD( T, DispatchAnimEvents ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets the specified bone controller to the given value.", name,
	    "float SetBoneController(int iController, float flValue)",
	    asMETHOD( T, SetBoneController ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Initializes bone controllers.", name,
	    "void InitBoneControllers()",
	    asMETHOD( T, InitBoneControllers ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets the given blending controller to the given value.", name,
	    "float SetBlending(int iBlender, float flValue)",
	    asMETHOD( T, SetBlending ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the position of the given bone.", name,
	    "void GetBonePosition(int iBone, Vector& out origin, Vector& out angles)",
	    asMETHOD( T, GetBonePosition ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets automatic movement.", name,
	    "void GetAutomovement(Vector& out origin, Vector& out angles, float flInterval = 0.1f)",
	    asMETHOD( T, GetAutomovement ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Find the transition between 2 sequences.", name,
	    "int FindTransition(int iEndingSequence, int iGoalSequence, int& out iDir)",
	    asFUNCTIONPR( AnimatingEntity_FindTransition, (int, int, int &, CBaseAnimating *), int ), asCALL_CDECL_OBJLAST );

	documentation->RegisterObjectMethod(
	    "Find the transition between 2 sequences.", name,
	    "int FindTransition(int iEndingSequence, int iGoalSequence)",
	    asFUNCTIONPR( AnimatingEntity_FindTransition, (int, int, CBaseAnimating *), int ), asCALL_CDECL_OBJLAST );

	documentation->RegisterObjectMethod(
	    "Gets the attachment position.", name,
	    "void GetAttachment(int iAttachment, Vector& out origin, Vector& out angles)",
	    asMETHOD( T, GetAttachment ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets the given body group to the given value.", name,
	    "void SetBodygroup(int iGroup, int iValue)",
	    asMETHOD( T, SetBodygroup ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the value of the given body group.", name,
	    "int GetBodygroup(int iGroup)",
	    asMETHOD( T, GetBodygroup ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Extracts the bounding box of the current model.", name,
	    "bool ExtractBbox(int iSequence, Vector& out mins, Vector& out maxs)",
	    asFUNCTION( AnimatingEntity_ExtractBbox ), asCALL_CDECL_OBJLAST );

	documentation->RegisterObjectMethod(
	    "Gets the number of attachments that the current model has.", name,
	    "int GetAttachmentCount()",
	    asMETHOD( T, GetAttachmentCount ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets the sequence box.", name,
	    "void SetSequenceBox()",
	    asMETHOD( T, SetSequenceBox ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets activity for the given sequence.", name,
	    "int GetSequenceActivityName(int iSequence)",
	    asMETHOD( T, GetSequenceActivityName ), asCALL_THISCALL );

	documentation->RegisterObjectProperty(
	    "Computed FPS for current sequence.", name,
	    "float m_flFrameRate", asOFFSET( T, m_flFrameRate ) );

	documentation->RegisterObjectProperty(
	    "Computed linear movement rate for current sequence.", name,
	    "float m_flGroundSpeed", asOFFSET( T, m_flGroundSpeed ) );

	documentation->RegisterObjectProperty(
	    "Last time the event list was checked.", name,
	    "float m_flLastEventCheck", asOFFSET( T, m_flLastEventCheck ) );

	documentation->RegisterObjectProperty(
	    "Last time the event list was checked.", name,
	    "float m_flLastGaitEventCheck", asOFFSET( T, m_flLastGaitEventCheck ) );

	documentation->RegisterObjectProperty(
	    "Flag set when StudioAdvanceFrame moves across a frame boundry.", name,
	    "bool m_fSequenceFinished", asOFFSET( T, m_fSequenceFinished ) );

	documentation->RegisterObjectProperty(
	    "True if the sequence loops.", name,
	    "bool m_fSequenceLoops", asOFFSET( T, m_fSequenceLoops ) );
}

#endif
