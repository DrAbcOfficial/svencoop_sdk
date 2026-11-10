#ifndef ASENTITYHEADERSUPPORT_H
#define ASENTITYHEADERSUPPORT_H

#include "CASDocumentation.h"
#include "CASLog.h"
#include "server_support.h"
#include <string.h>

class CBaseEntity;

template <class Base, class Derived>
Base *ASScriptEntityHeaderUpCast( Derived *entity )
{
	return static_cast<Base *>( entity );
}

template <class Base, class Derived>
Derived *ASScriptEntityHeaderDownCast( Base *entity )
{
	return dynamic_cast<Derived *>( entity );
}

template <class Base, class Derived>
void RegisterASScriptEntityHeaderCasts( CASDocumentation *documentation,
                                        const char *baseName, const char *derivedName )
{
	if ( !strcmp( baseName, derivedName ) )
		return;

	char declaration[256];
	int length = UTIL_FormatString( declaration, sizeof( declaration ), "%s@ opImplCast()", baseName );

	if ( (unsigned int) length >= sizeof( declaration ) )
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT,
		                              "Failed to format string for implicit cast for class '%s'!\n", derivedName );
	}
	else
	{
		documentation->RegisterObjectMethod( "Implicitly casts to base type.",
		                                     derivedName, documentation->AllocString( declaration ),
		                                     asFUNCTION( (ASScriptEntityHeaderUpCast<Base, Derived>) ), asCALL_CDECL_OBJLAST );
	}

	length = UTIL_FormatString( declaration, sizeof( declaration ), "%s@ opCast()", derivedName );

	if ( (unsigned int) length >= sizeof( declaration ) )
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT,
		                              "Failed to format string for explicit cast for class '%s'!\n", baseName );
	}
	else
	{
		documentation->RegisterObjectMethod( "Explicitly casts to sub type.",
		                                     baseName, documentation->AllocString( declaration ),
		                                     asFUNCTION( (ASScriptEntityHeaderDownCast<Base, Derived>) ), asCALL_CDECL_OBJLAST );
	}
}

void RegisterSCScriptGrenadeEntityDependencies( CASDocumentation *documentation );
void RegisterSCScriptEntities( CASDocumentation *documentation );

#endif
