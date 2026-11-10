#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "EntityRelationships.h"

namespace
{
	const float FEAR_HEALTH_FRACTION = 0.2f;
}

int iEnemy[EntityRelationship::CLASSIFICATION_COUNT][EntityRelationship::CLASSIFICATION_COUNT] =
{
	{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
	{ 0,  0,  1,  1,  0,  1,  1,  1,  1,  1,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1},
	{ 0,  1, -2, -2,  1,  1,  1,  1,  1,  1,  0, -2,  1,  1,  1,  1,  1,  1,  1,  1},
	{ 0,  4, -2, -2,  2,  1,  1,  2,  1,  1,  0, -2,  0,  0,  4,  4,  1,  1,  1,  1},
	{ 0,  0,  1,  1, -2,  2,  1,  1,  1,  1,  0,  1,  0,  0,  2,  2,  1,  1,  1,  1},
	{ 0,  1,  1,  1,  2, -2,  0,  0,  0,  0,  0,  1,  0,  0,  1,  2,  1,  1,  1,  1},
	{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1},
	{ 0,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  1,  1,  1,  1},
	{ 0,  0,  1,  1,  1,  0,  0,  0,  0,  4,  0,  1,  0,  0,  4,  0,  1,  1,  1,  1},
	{ 0,  0,  1,  1,  1,  0,  0,  0,  2,  0,  0,  1,  0,  0,  1,  1,  1,  1,  1,  1},
	{ 4,  4,  4,  4,  4,  0,  4,  4,  4,  4,  0,  4,  0,  0,  0,  0,  4,  4,  4,  4},
	{ 0,  1, -2, -2,  1,  1,  1,  1,  1,  1,  0, -2,  0,  0,  1,  1,  1,  1,  1,  1},
	{ 0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1},
	{ 0,  0,  1,  1,  1, -2,  0,  1,  1,  0,  0,  1,  1,  0,  1,  1,  1,  1,  1,  1},
	{ 0,  1,  1,  1,  1,  1,  1,  0,  1,  1,  0,  1,  0,  0, -2, -2,  1,  1,  1,  1},
	{ 0,  1,  2,  1,  2,  2,  1,  0,  0,  1,  0,  1,  0,  0, -2, -2,  1,  1,  1,  1},
	{ 0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  1,  0,  0,  1,  1, -2,  1,  1,  1},
	{ 0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  1,  0,  0,  1,  1,  1, -2,  1,  1},
	{ 0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  1,  0,  0,  1,  1,  1,  1, -2,  1},
	{ 0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  1,  0,  0,  1,  1,  1,  1,  1, -2},
};

int CBaseEntity::IRelationshipByClass(int classification)
{
	if ((unsigned int)classification >= EntityRelationship::CLASSIFICATION_COUNT)
		return EntityRelationship::None;

	unsigned int sourceClass = (unsigned int)Classify();

	if (sourceClass >= EntityRelationship::CLASSIFICATION_COUNT)
		return EntityRelationship::None;

	int relationship = iEnemy[sourceClass][classification];

	if (relationship > EntityRelationship::None && pev->max_health * FEAR_HEALTH_FRACTION > pev->health)
		return EntityRelationship::Fear;

	return relationship;
}
