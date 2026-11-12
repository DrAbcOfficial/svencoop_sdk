#ifndef CMULTIPLAYGAMEMGRHELPER_H
#define CMULTIPLAYGAMEMGRHELPER_H

#include "voice_gamemgr.h"
#include <map>

struct entvars_s;

class CMultiplayGameMgrHelper : public IVoiceGameMgrHelper
{
public:
	virtual bool CanPlayerHearPlayer( CBasePlayer *listener, CBasePlayer *talker );

	bool IsPlayerBanned( int listener, int talker );
	void SetPlayerBanned( int listener, int talker, bool banned );
	void ClearBansforListener( int listener );
	void ClearBansforTalker( int talker );
	void ClearBans( void );
	void PrintBanList( entvars_s *client );

private:
	typedef std::multimap<int, int> BanMap_t;

	BanMap_t m_Bans;
};

extern CMultiplayGameMgrHelper g_GameMgrHelper;

#endif
