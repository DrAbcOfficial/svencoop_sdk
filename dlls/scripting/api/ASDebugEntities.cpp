#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "scripting/api/ASDebugEntities.h"

std::string SCScriptEntityToString(void *object, int expandMembersLevel, CDebugger *debugger)
{
	entvars_t *pev = ((CBaseEntity *)object)->pev;
	const char *className = "";
	const char *targetName = "";

	if (pev)
	{
		targetName = STRING(pev->targetname);
		className = STRING(pev->classname);
	}

	return std::string("CBaseEntity: classname: ") + className + ", targetname: " + targetName;
}

std::string SCScriptPlayerEntityToString(void *object, int expandMembersLevel, CDebugger *debugger)
{
	entvars_t *pev = ((CBaseEntity *)object)->pev;
	const char *playerName = STRING(pev->netname);
	const char *targetName = STRING(pev->targetname);
	const char *className = STRING(pev->classname);

	return std::string("Entity: classname: ") + className + ", targetname: " + targetName +
		", player name: " + playerName;
}
