#ifndef ASBASEPATHTRACK_H
#define ASBASEPATHTRACK_H

#include "ASBaseEntity.h"
#include "trains.h"

void PathTrackEntity_Project( CPathTrack *start, CPathTrack *end, Vector &origin, float distance, CPathTrack *track );
CPathTrack *PathTrackEntity_LookAhead( Vector &origin, Vector &resultOrigin, float distance, bool move, CPathTrack *track );

template <class T>
void RegisterSCScriptBasePathTrackEntity( CASDocumentation *documentation, const char *name )
{
	RegisterASScriptEntityHeaderCasts<CPathTrack, T>( documentation, "CPathTrack", name );
	RegisterSCScriptBaseEntity<T>( documentation, name, true );

	documentation->RegisterObjectMethod(
	    "Sets the previous track in this path", name,
	    "void SetPrevious(CPathTrack@ pPrevious)",
	    asMETHOD( T, SetPrevious ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Links the tracks together", name,
	    "void Link()",
	    asMETHOD( T, Link ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns pPath if enabled, null otherwise.", name,
	    "CPathTrack@ ValidPath(CPathTrack@ pPath, const bool testFlag)",
	    asMETHOD( T, ValidPath ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "", name,
	    "void Project(CPathTrack@ pStart, CPathTrack@ pEnd, Vector& out vecOrigin, float flDist)",
	    asFUNCTION( PathTrackEntity_Project ), asCALL_CDECL_OBJLAST );

	documentation->RegisterObjectMethod(
	    "", name,
	    "CPathTrack@ LookAhead(Vector& in vecOrigin, Vector& out vecResultOrigin, float flDist, const bool bMove)",
	    asFUNCTION( PathTrackEntity_LookAhead ), asCALL_CDECL_OBJLAST );

	documentation->RegisterObjectMethod(
	    "", name,
	    "CPathTrack@ Nearest(Vector vecOrigin)",
	    asMETHOD( T, Nearest ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "", name,
	    "CPathTrack@ GetNext()",
	    asMETHOD( T, GetNext ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "", name,
	    "CPathTrack@ GetPrevious()",
	    asMETHOD( T, GetPrevious ), asCALL_THISCALL );

	documentation->RegisterObjectProperty(
	    "", name,
	    "float m_length", asOFFSET( T, m_length ) );

	documentation->RegisterObjectProperty(
	    "", name,
	    "string_t m_altName", asOFFSET( T, m_altName ) );

	documentation->RegisterObjectProperty(
	    "", name,
	    "EHandle m_hNext", asOFFSET( T, m_hNext ) );

	documentation->RegisterObjectProperty(
	    "", name,
	    "EHandle m_hPrevious", asOFFSET( T, m_hPrevious ) );

	documentation->RegisterObjectProperty(
	    "", name,
	    "EHandle m_hAltpath", asOFFSET( T, m_hAltpath ) );

	documentation->RegisterObjectProperty(
	    "New maximum speed", name,
	    "float m_flMaxSpeed", asOFFSET( T, m_flMaxSpeed ) );

	documentation->RegisterObjectProperty(
	    "New train speed", name,
	    "float m_flNewSpeed", asOFFSET( T, m_flNewSpeed ) );
}

#endif
