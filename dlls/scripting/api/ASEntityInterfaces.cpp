#include "scripting/api/ASEntityInterfaces.h"
#include "scripting/core/CASDocumentation.h"

void RegisterASScriptClassInterface( CASDocumentation *documentation )
{
	documentation->RegisterInterface( "Script class interface", "ScriptClassInterface" );
}
