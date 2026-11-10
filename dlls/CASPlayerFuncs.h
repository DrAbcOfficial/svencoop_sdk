#ifndef CASPLAYERFUNCS_H
#define CASPLAYERFUNCS_H

#include "extdll.h"
#include "AdminLevel.h"
#include "CString.h"
#include "CASServerScriptGlobal.h"
#include "player_hud.h"

class CBaseEntity;
class CBasePlayer;
class CBasePlayerItem;

typedef struct hudtextparms_s hudtextparms_t;

class CASPlayerFuncs : public CASServerScriptGlobal<CASPlayerFuncs>
{
public:
	virtual ~CASPlayerFuncs();

	void SayText( CBasePlayer *player, const CString &text );
	void SayTextAll( CBasePlayer *player, const CString &text );
	void ClientPrint( CBasePlayer *player, int destination, const CString &message, const CString &line2,
	                  const CString &line3, const CString &line4, const CString &line5 );
	void ClientPrintAll( int destination, const CString &message, const CString &line2,
	                     const CString &line3, const CString &line4, const CString &line5 );
	void CenterPrintAll( const CString &message, const CString &line2, const CString &line3,
	                     const CString &line4, const CString &line5 );
	void ShowMessage( CBasePlayer *player, const CString &message );
	void ShowMessageAll( const CString &message );
	void HudMessage( CBasePlayer *player, const hudtextparms_t &params, const CString &message );
	void HudMessageAll( const hudtextparms_t &params, const CString &message );
	void PrintKeyBindingString( CBasePlayer *player, const CString &text );
	void PrintKeyBindingStringAll( const CString &text );
	void HudToggleElement( CBasePlayer *player, unsigned char channel, bool visible );
	void HudCustomSprite( CBasePlayer *player, const hudspriteparams_t &params );
	void HudNumDisplay( CBasePlayer *player, const hudnumdisplayparams_t &params );
	void HudUpdateNum( CBasePlayer *player, unsigned char channel, float value );
	void HudTimeDisplay( CBasePlayer *player, const hudnumdisplayparams_t &params );
	void HudUpdateTime( CBasePlayer *player, unsigned char channel, float value );
	CBasePlayer *FindPlayerByIndex( int index );
	CBasePlayer *FindPlayerByName( const CString &name, bool caseSensitive );
	int GetNumPlayers() const;
	void ScreenShake( const Vector &center, float amplitude, float frequency, float duration, float radius );
	void ScreenShakeAll( const Vector &center, float amplitude, float frequency, float duration );
	void ScreenFade( CBaseEntity *entity, const Vector &color, float fadeTime, float fadeHold, int alpha, int flags );
	void ScreenFadeAll( const Vector &color, float fadeTime, float fadeHold, int alpha, int flags );
	void ConcussionEffect( CBaseEntity *entity, float amplitude, float frequency, float fadeTime );
	BOOL GetNextBestWeapon( CBasePlayer *player, CBasePlayerItem *weapon );
	bool CheatsAllowed( CBasePlayer *player, const CString &cheatName, bool mustBeAlive, bool noMessage ) const;
	AdminLevel_t AdminLevel( CBasePlayer *player ) const;
	CString AdminLevelToString( AdminLevel_t level ) const;
	AdminLevel_t StringToAdminLevel( const CString &text ) const;
	bool ReportBug( CBasePlayer *player, const CString &description, CString &fileName );
	bool ReportBug( CBasePlayer *player, const CString &description );
	void ApplyMapCfgToPlayer( CBasePlayer *player, bool reEquip );
	void RespawnPlayer( CBasePlayer *player, bool moveLiving, bool respawnDead );
	void RespawnAllPlayers( bool moveLiving, bool respawnDead );
	bool SpawnPointFilterPasses( CBaseEntity *point, CBaseEntity *player );
	bool IsSpawnPointValid( CBaseEntity *point, CBaseEntity *player );
	bool IsSpawnPointOccupied( CBaseEntity *point );
	int SharedRandomLong( unsigned int seed, int low, int high );
	float SharedRandomFloat( unsigned int seed, float low, float high );
	int GetAmmoIndex( const CString &name );
	CBasePlayer *CreateBot( const CString &name );
	void BotDisconnect( CBasePlayer *bot );

private:
	int HudGetChannelGroup();
	void ConvertHudTextChannel( hudtextparms_t &params );
};

#endif
