#ifndef ENTITYCLASSIFICATION_H
#define ENTITYCLASSIFICATION_H

#define CLASS_FORCE_NONE			-1
#define CLASS_NONE				0
#define CLASS_MACHINE			1
#define CLASS_PLAYER			2
#define CLASS_HUMAN_PASSIVE		3
#define CLASS_HUMAN_MILITARY		4
#define CLASS_ALIEN_MILITARY		5
#define CLASS_ALIEN_PASSIVE		6
#define CLASS_ALIEN_MONSTER		7
#define CLASS_ALIEN_PREY			8
#define CLASS_ALIEN_PREDATOR		9
#define CLASS_INSECT				10
#define CLASS_PLAYER_ALLY		11
#define CLASS_PLAYER_BIOWEAPON	12
#define CLASS_ALIEN_BIOWEAPON		13
#define CLASS_XRACE_PITDRONE		14
#define CLASS_XRACE_SHOCK			15
#define CLASS_TEAM1				16
#define CLASS_TEAM2				17
#define CLASS_TEAM3				18
#define CLASS_TEAM4				19
#define CLASS_BARNACLE			99

const char *UTIL_ClassifyToTag(int classification);
const char *UTIL_ClassifyToName(int classification);

#endif
