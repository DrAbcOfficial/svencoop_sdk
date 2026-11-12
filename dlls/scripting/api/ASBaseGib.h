#ifndef ASBASEGIB_H
#define ASBASEGIB_H

#include "scripting/api/ASBaseEntity.h"
#include "monsters/monsters.h"
#include "server/server_support.h"

inline void CGib_Spawn( CGib *gib, const CString &modelName )
{
	gib->Spawn( STRING( AllocStringLocal( modelName.Data() ) ) );
}

template <class T>
void RegisterSCScriptBaseCGib( CASDocumentation *documentation, const char *name )
{
	RegisterASScriptEntityHeaderCasts<CGib, T>( documentation, "CGib", name );
	RegisterSCScriptBaseEntity<T>( documentation, name, true );

	documentation->RegisterObjectMethod(
	    "Spawns the gib with the given model. Note: this sets several of CGib's variables. Set variables after calling this.",
	    name, "void Spawn(const string& in szModelName)",
	    asFUNCTION( CGib_Spawn ), asCALL_CDECL_OBJFIRST );

	documentation->RegisterObjectMethod(
	    "Limits the velocity of the gib", name,
	    "void LimitVelocity()",
	    asMETHOD( T, LimitVelocity ), asCALL_THISCALL );

	documentation->RegisterObjectProperty(
	    "The color of the blood that is sprayed onto walls when the gib touches it.", name,
	    "int m_bloodColor", asOFFSET( T, m_bloodColor ) );

	documentation->RegisterObjectProperty(
	    "Maximum number of blood decals this gib can spray.", name,
	    "int m_cBloodDecals", asOFFSET( T, m_cBloodDecals ) );

	documentation->RegisterObjectProperty(
	    "Gib material type. One of the Materials enum values should be used here.", name,
	    "int m_material", asOFFSET( T, m_material ) );

	documentation->RegisterObjectProperty(
	    "Lifetime of this gib, in seconds", name,
	    "float m_lifeTime", asOFFSET( T, m_lifeTime ) );
}

#endif
