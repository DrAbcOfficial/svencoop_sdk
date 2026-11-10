#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "CToPlayerCollisions.h"

CToPlayerCollisions::CToPlayerCollisions()
{
}

CToPlayerCollisions::~CToPlayerCollisions()
{
}

void CToPlayerCollisions::MakePlayerIntouchable(CBasePlayer *player, float time)
{
	if (!player)
		return;

	edict_t *edict = player->pev ? ENT(player->pev) : NULL;
	CToEntityCollision &collision = m_Collisions[ENTINDEX(edict) - 1];

	collision.SetEnt(player);
	collision.SetNotSolidUntil(time);
}

void CToPlayerCollisions::MakeAllPlayersSolid(void)
{
	for (unsigned int index = 0; index < PLAYER_COUNT; index++)
		m_Collisions[index].SetNotSolidUntil(0);
}

void CToPlayerCollisions::EnableCollisionWithPlayer(CBasePlayer *player, bool checkIntersection)
{
	if (!player)
		return;

	if (checkIntersection)
	{
		edict_t *edict = player->pev ? ENT(player->pev) : NULL;
		CToEntityCollision &collision = m_Collisions[ENTINDEX(edict) - 1];

		if (collision.DoIntersect(player))
			return;
	}

	edict_t *edict = player->pev ? ENT(player->pev) : NULL;

	m_Collisions[ENTINDEX(edict) - 1].SetNotSolidUntil(0);
}

bool CToPlayerCollisions::CanCollide(CBasePlayer *player)
{
	edict_t *edict = player->pev ? ENT(player->pev) : NULL;

	return m_Collisions[ENTINDEX(edict) - 1].CanCollide();
}

void CToPlayerCollisions::Reset(void)
{
	for (unsigned int index = 0; index < PLAYER_COUNT; index++)
		m_Collisions[index].Reset();
}
