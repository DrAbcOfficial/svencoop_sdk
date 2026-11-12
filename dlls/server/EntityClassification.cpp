#include "extdll.h"
#include "server/EntityClassification.h"

const char *UTIL_ClassifyToTag(int classification)
{
	switch (classification)
	{
	case CLASS_FORCE_NONE:
		return "none_forced";
	case CLASS_NONE:
		return "none";
	case CLASS_MACHINE:
		return "machine";
	case CLASS_PLAYER:
		return "player";
	case CLASS_HUMAN_PASSIVE:
		return "human_passive";
	case CLASS_HUMAN_MILITARY:
		return "human_military";
	case CLASS_ALIEN_MILITARY:
		return "alien_military";
	case CLASS_ALIEN_PASSIVE:
		return "alien_passive";
	case CLASS_ALIEN_MONSTER:
		return "alien_monster";
	case CLASS_ALIEN_PREY:
		return "alien_prey";
	case CLASS_ALIEN_PREDATOR:
		return "alien_predator";
	case CLASS_INSECT:
		return "insect";
	case CLASS_PLAYER_ALLY:
		return "player_ally";
	case CLASS_PLAYER_BIOWEAPON:
		return "player_bio_weapon";
	case CLASS_ALIEN_BIOWEAPON:
		return "alien_bio_weapon";
	case CLASS_XRACE_PITDRONE:
		return "x-race_pit_drone";
	case CLASS_XRACE_SHOCK:
		return "x-race_shock";
	case CLASS_TEAM1:
		return "team1";
	case CLASS_TEAM2:
		return "team2";
	case CLASS_TEAM3:
		return "team3";
	case CLASS_TEAM4:
		return "team4";
	case CLASS_BARNACLE:
		return "barnacle";
	default:
		return "unknown";
	}
}

const char *UTIL_ClassifyToName(int classification)
{
	switch (classification)
	{
	case CLASS_FORCE_NONE:
		return "None (forced)";
	case CLASS_NONE:
		return "None";
	case CLASS_MACHINE:
		return "Machine";
	case CLASS_PLAYER:
		return "Player";
	case CLASS_HUMAN_PASSIVE:
		return "Human passive";
	case CLASS_HUMAN_MILITARY:
		return "Human military";
	case CLASS_ALIEN_MILITARY:
		return "Alien military";
	case CLASS_ALIEN_PASSIVE:
		return "Alien passive";
	case CLASS_ALIEN_MONSTER:
		return "Alien monster";
	case CLASS_ALIEN_PREY:
		return "Alien prey";
	case CLASS_ALIEN_PREDATOR:
		return "Alien predator";
	case CLASS_INSECT:
		return "Insect";
	case CLASS_PLAYER_ALLY:
		return "Player ally";
	case CLASS_PLAYER_BIOWEAPON:
		return "Player bio weapon";
	case CLASS_ALIEN_BIOWEAPON:
		return "Alien bio weapon";
	case CLASS_XRACE_PITDRONE:
		return "X-race: Pit drone";
	case CLASS_XRACE_SHOCK:
		return "X-race: Shock";
	case CLASS_TEAM1:
		return "Team 1";
	case CLASS_TEAM2:
		return "Team 2";
	case CLASS_TEAM3:
		return "Team 3";
	case CLASS_TEAM4:
		return "Team 4";
	case CLASS_BARNACLE:
		return "Barnacle";
	default:
		return "Unknown";
	}
}
