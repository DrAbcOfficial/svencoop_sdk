#ifndef CSERVER_H
#define CSERVER_H

#include "donid.h"

class CBasePlayer;
struct edict_s;
class CServerState;
class CVoteManager;

class CServer
{
public:
	static CServer *GetInstance();
	static void DestroyInstance();

	void Think();
	void MapInit();
	void MapEnd();
	CVoteManager &GetVoteManager();
	unsigned int GetPlayerCount() const;
	unsigned int GetReservedSlots() const;
	void SetReservedSlots( unsigned int slots );
	bool ClientConnected( edict_s *client, const char *name, const char *address, char *rejectReason );
	bool ClientCommand( CBasePlayer *player, const char *command );
	void KillPlayer( CBasePlayer *player, float respawnDelay = -1.0f );
	bool KickPlayer( CBasePlayer *player, const char *authId = NULL, float minutes = -1.0f );
	void ClientPutInServer( CBasePlayer *player );
	void ClientDisconnected( edict_s *client );
	int UpdatePlayerExtras( CBasePlayer *player, bool force = false );
	const extrasinfo_s *GetPlayerExtras( const CBasePlayer *player ) const;
	void UpdateClientData( CBasePlayer *player, bool initializeHud = false );
	bool BanPlayer( CBasePlayer *player, const char *authId, float minutes );

private:
	bool KickBanPlayer( CBasePlayer *player, const char *authId, float minutes );
	friend class CClassicMode;

	CServer();
	~CServer();
	CServer( const CServer & );
	CServer &operator=( const CServer & );

	CServerState *m_pState;
	float         m_flRestartTime;
};

#endif
