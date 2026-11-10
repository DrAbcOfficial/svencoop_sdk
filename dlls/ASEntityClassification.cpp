#include "extdll.h"
#include "ASEntityClassification.h"
#include "CASDocumentation.h"
#include "EntityClassification.h"

void RegisterASEntityClassification(CASDocumentation *documentation)
{
	documentation->RegisterEnum("Entity classification\nCan be set on entities with KeyValue using key 'classify'.",
		"CLASS");

	documentation->RegisterEnumValue("Forces class override to none", "CLASS", "CLASS_FORCE_NONE", CLASS_FORCE_NONE);
	documentation->RegisterEnumValue("No classification", "CLASS", "CLASS_NONE", CLASS_NONE);
	documentation->RegisterEnumValue("Is machine", "CLASS", "CLASS_MACHINE", CLASS_MACHINE);
	documentation->RegisterEnumValue("Is player", "CLASS", "CLASS_PLAYER", CLASS_PLAYER);
	documentation->RegisterEnumValue("Is passive human", "CLASS", "CLASS_HUMAN_PASSIVE", CLASS_HUMAN_PASSIVE);
	documentation->RegisterEnumValue("Is military human", "CLASS", "CLASS_HUMAN_MILITARY", CLASS_HUMAN_MILITARY);
	documentation->RegisterEnumValue("Is military alien", "CLASS", "CLASS_ALIEN_MILITARY", CLASS_ALIEN_MILITARY);
	documentation->RegisterEnumValue("Is passive alien", "CLASS", "CLASS_ALIEN_PASSIVE", CLASS_ALIEN_PASSIVE);
	documentation->RegisterEnumValue("Is monster alien", "CLASS", "CLASS_ALIEN_MONSTER", CLASS_ALIEN_MONSTER);
	documentation->RegisterEnumValue("Is prey alien", "CLASS", "CLASS_ALIEN_PREY", CLASS_ALIEN_PREY);
	documentation->RegisterEnumValue("Is predator alien", "CLASS", "CLASS_ALIEN_PREDATOR", CLASS_ALIEN_PREDATOR);
	documentation->RegisterEnumValue("Is insect", "CLASS", "CLASS_INSECT", CLASS_INSECT);
	documentation->RegisterEnumValue("is player ally", "CLASS", "CLASS_PLAYER_ALLY", CLASS_PLAYER_ALLY);
	documentation->RegisterEnumValue("Is player bioweapon", "CLASS", "CLASS_PLAYER_BIOWEAPON", CLASS_PLAYER_BIOWEAPON);
	documentation->RegisterEnumValue("Is alien bioweapon", "CLASS", "CLASS_ALIEN_BIOWEAPON", CLASS_ALIEN_BIOWEAPON);
	documentation->RegisterEnumValue("Is X-Race pitdrone", "CLASS", "CLASS_XRACE_PITDRONE", CLASS_XRACE_PITDRONE);
	documentation->RegisterEnumValue("Is X-Race shock trooper (also Voltigores)", "CLASS",
		"CLASS_XRACE_SHOCK", CLASS_XRACE_SHOCK);
	documentation->RegisterEnumValue("Is on Team 1", "CLASS", "CLASS_TEAM1", CLASS_TEAM1);
	documentation->RegisterEnumValue("Is on Team 2", "CLASS", "CLASS_TEAM2", CLASS_TEAM2);
	documentation->RegisterEnumValue("Is on Team 3", "CLASS", "CLASS_TEAM3", CLASS_TEAM3);
	documentation->RegisterEnumValue("Is on Team 4", "CLASS", "CLASS_TEAM4", CLASS_TEAM4);
	documentation->RegisterEnumValue("Is Barnacle", "CLASS", "CLASS_BARNACLE", CLASS_BARNACLE);
}
