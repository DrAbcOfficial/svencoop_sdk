#include "ASEntityInterfaces.h"
#include "CASDocumentation.h"

void RegisterASScriptClassInterface( CASDocumentation *documentation )
{
	documentation->RegisterInterface( "Script class interface", "ScriptClassInterface" );
}
