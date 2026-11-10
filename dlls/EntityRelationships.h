#ifndef ENTITYRELATIONSHIPS_H
#define ENTITYRELATIONSHIPS_H

namespace EntityRelationship
{
	enum Type
	{
		Ally = -2,
		None = 0,
		Dislike = 1,
		Hate = 2,
		Nemesis = 3,
		Fear = 4
	};

	enum { CLASSIFICATION_COUNT = 20 };
}

extern int iEnemy[EntityRelationship::CLASSIFICATION_COUNT][EntityRelationship::CLASSIFICATION_COUNT];

#endif
