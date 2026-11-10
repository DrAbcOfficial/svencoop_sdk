#ifndef GAME_PERSISTENT_INVENTORY_H
#define GAME_PERSISTENT_INVENTORY_H

class CBasePlayer;

class CGamePersistentInventory
{
public:
	CGamePersistentInventory( const char *mapName );
	~CGamePersistentInventory();
	void CapturePlayers();
	bool Load( CBasePlayer *player );

	char m_szMapName[32 + 1];

private:
	struct Snapshot;

	Snapshot *Save( CBasePlayer *player );

	Snapshot *m_pInventories;
};

extern CGamePersistentInventory *g_pGamePersistentInventory;

#endif
