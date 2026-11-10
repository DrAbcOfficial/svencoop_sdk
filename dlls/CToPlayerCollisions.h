#ifndef CTOPLAYERCOLLISIONS_H
#define CTOPLAYERCOLLISIONS_H

#include "CToEntityCollision.h"

class CBasePlayer;

class CToPlayerCollisions
{
public:
	CToPlayerCollisions();
	~CToPlayerCollisions();

	void MakePlayerIntouchable(CBasePlayer *player, float time);

	// Not implemented yet: the player lookup needs the full downcast and
	// connection interfaces
	void MakeAllPlayersIntouchable(float time);

	void MakeAllPlayersSolid(void);
	void EnableCollisionWithPlayer(CBasePlayer *player, bool checkIntersection);
	bool CanCollide(CBasePlayer *player);
	void Reset(void);

private:
	enum
	{
		PLAYER_COUNT = 32
	};

	CToEntityCollision	m_Collisions[PLAYER_COUNT];
};

#endif
