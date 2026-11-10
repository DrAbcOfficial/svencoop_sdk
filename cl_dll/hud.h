/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//			
//  hud.h
//
// class CHud declaration
//
// CHud handles the message, calculation, and drawing the HUD
//


#define RGB_YELLOWISH 0x00FFA000 //255,160,0
#define RGB_REDISH 0x00FF1010 //255,160,0
#define RGB_GREENISH 0x0000A000 //0,160,0
#define RGB_BLUEISH 0x006482C8 //100,130,200
#define RGB_DARKREDISH 0x00602020 //96,32,32

#define HUD_SPRITE_RES	640 // the only resolution read from HUD sprite lists

#ifndef _WIN32
#define _cdecl 
#endif

#include "wrect.h"
#include "cl_dll.h"
#include "ammo.h"

#define DHN_DRAWZERO 1
#define DHN_2DIGITS  2
#define DHN_3DIGITS  4

// CHud::DrawNumber flags
#define DN_HIDEZERO		1	// a zero value has no digits
#define DN_LEADINGZEROS	2	// pad to the minimum digit count with zeros
#define DN_NEGATIVE		4
#define DN_PLUSSIGN		8

#define MIN_ALPHA	 100	

#define		HUDELEM_ACTIVE	1

typedef struct {
	int x, y;
} POSITION;

typedef struct {
	unsigned char r,g,b,a;
} RGBA;

typedef struct cvar_s cvar_t;
struct weapon_data_s;

enum SPR_MODE
{
	SPR_NORMAL = 1,
	SPR_TRANSPARENT = 2,
	SPR_ADDITIVE = 3
};


#define HUD_ACTIVE	1
#define HUD_INTERMISSION 2

#define MAX_PLAYER_NAME_LENGTH		32

#define	MAX_MOTD_LENGTH				4096

enum
{
	MAX_PLAYERS = 32,
	MAX_TEAMS = 64,
	MAX_TEAM_NAME = 64,
};

//
//-----------------------------------------------------
//
class CHudBase
{
public:
	POSITION  m_pos;
	int   m_type;
	int	  m_iFlags; // active, moving, 
	virtual		~CHudBase() {}
	virtual int Init( void ) {return 0;}
	virtual int VidInit( void ) {return 0;}
	virtual int Draw(float flTime) {return 0;}
	virtual void Think(void) {return;}
	virtual void Reset(void) {return;}
	virtual void InitHUDData( void ) {}		// called every time a server is connected to

};

struct HUDLIST {
	CHudBase	*p;
	HUDLIST		*pNext;
};



//
//-----------------------------------------------------
//
#include "voice_status.h" // base voice handling class


//
//-----------------------------------------------------
//
class CHudAmmo: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	void Think(void);
	void Reset(void);
	int DrawWList(float flTime);
	int GetDimAlpha(void);
	int GetBrightAlpha(void);
	int MsgFunc_CurWeapon(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_WeaponList(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_AmmoX(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_AmmoPickup( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_WeapPickup( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_ItemPickup( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_HideHUD( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_CustWeapon( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_WeaponSpr( const char *pszName, int iSize, void *pbuf );
	void UpdateWeaponList( weapon_data_s *weaponData );
	void CloseSelection( bool bSelected = false );

	void SlotInput( int iSlot );
	void _cdecl UserCmd_Slot1( void );
	void _cdecl UserCmd_Slot2( void );
	void _cdecl UserCmd_Slot3( void );
	void _cdecl UserCmd_Slot4( void );
	void _cdecl UserCmd_Slot5( void );
	void _cdecl UserCmd_Slot6( void );
	void _cdecl UserCmd_Slot7( void );
	void _cdecl UserCmd_Slot8( void );
	void _cdecl UserCmd_Slot9( void );
	void _cdecl UserCmd_Slot10( void );
	void _cdecl UserCmd_Close( void );
	void _cdecl UserCmd_NextWeapon( void );
	void _cdecl UserCmd_PrevWeapon( void );

private:
	float m_fFade;
	WEAPON *m_pWeapon;
	int	m_HUD_bucket0;
	int m_HUD_selection;
	int m_iBucketWidth;
	int m_iBucketHeight;
	int m_iBucketCount;
	float m_flSelectionTime;
	float m_flLastFastSwitch;
	cvar_t *m_pCvarSelectionTimeout;
	cvar_t *m_pCvarSelectionFadeout;

};

//
//-----------------------------------------------------
//
#include "health.h"
#include "crosshair.h"


#define FADE_TIME 100


//
//-----------------------------------------------------
//
class CHudGeiger: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_Geiger(const char *pszName, int iSize, void *pbuf);
	
private:
	int m_iGeigerRange;

};

//
//-----------------------------------------------------
//
class CHudTrain: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_Train(const char *pszName, int iSize, void *pbuf);

private:
	HSPRITE m_hSprite;
	int m_iPos;

};

//
//-----------------------------------------------------
//
struct extra_player_info_t
{
	float	frags;
	int		deaths;
	short	playerclass;
	short	teamnumber;
	char	teamname[MAX_TEAM_NAME];
	float	health;
	float	armor;
	short	extraslevel;
	bool	showextras;
	short	adminlevel;
};

struct team_info_t 
{
	char name[MAX_TEAM_NAME];
	short frags;
	short deaths;
	short ping;
	short packetloss;
	short ownteam;
	short players;
	int already_drawn;
	int scores_overriden;
	int teamnumber;
};

#include "player_info.h"

//
//-----------------------------------------------------
//
class CHudMenu : public CHudBase
{
public:
	int Init( void );
	void InitHUDData( void );
	int VidInit( void );
	void Reset( void );
	int Draw( float flTime );
	int MsgFunc_ShowMenu( const char *pszName, int iSize, void *pbuf );

	void SelectMenuItem( int menu_item );

	int m_fMenuDisplayed;
	int m_bitsValidSlots;
	float m_flShutoffTime;
	int m_fWaitingForMore;
	bool m_bIsASMenu;
};

//
//-----------------------------------------------------
//
#include "saytext.h"

class CHudBattery: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_Battery(const char *pszName,  int iSize, void *pbuf );
	
private:
	HSPRITE m_hSprite1;
	HSPRITE m_hSprite2;
	wrect_t *m_prc1;
	wrect_t *m_prc2;
	int	  m_iBat;	
	float m_fFade;
	int	  m_iHeight;		// width of the battery innards
};


//
//-----------------------------------------------------
//
class CHudFlashlight: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	void Reset( void );
	int MsgFunc_Flashlight(const char *pszName,  int iSize, void *pbuf );
	int MsgFunc_FlashBat(const char *pszName,  int iSize, void *pbuf );
	
private:
	HSPRITE m_hSprite1;
	HSPRITE m_hSprite2;
	HSPRITE m_hBeam;
	wrect_t *m_prc1;
	wrect_t *m_prc2;
	wrect_t *m_prcBeam;
	float m_flBat;	
	int	  m_iBat;	
	int	  m_fOn;
	float m_fFade;
	int	  m_iWidth;		// width of the battery innards
};

//
//-----------------------------------------------------
//
const int maxHUDMessages = 16;
struct message_parms_t
{
	client_textmessage_t	*pMessage;
	float	time;
	int x, y;
	int	totalWidth, totalHeight;
	int width;
	int lines;
	int lineLength;
	int length;
	int r, g, b;
	int a;
	int text;
	int fadeBlend;
	float charTime;
	float fadeTime;
};

//
//-----------------------------------------------------
//

class CHudTextMessage: public CHudBase
{
public:
	int Init( void );
	static char *LocaliseTextString( const char *msg, char *dst_buffer, int buffer_size );
	static char *BufferedLocaliseTextString( const char *msg );
	const char *LookupString( const char *msg_name, int *msg_dest = NULL );
	int MsgFunc_TextMsg(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_PrintKB(const char *pszName, int iSize, void *pbuf);
};

//
//-----------------------------------------------------
//

class CHudMessage: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_HudText(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_HudTextPro(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_GameTitle(const char *pszName, int iSize, void *pbuf);

	float FadeBlend( float fadein, float fadeout, float hold, float localTime );
	bool DrawGameTitle( float fTime );
	bool DrawHudMessages( float fTime );

	void MessageAdd( const char *pName, float time );
	void MessageAdd(client_textmessage_t * newMessage );
	void MessageDrawScan( client_textmessage_t *pMessage, float time );
	void MessageScanStart( void );
	void MessageScanNextChar( void );
	void Reset( void );

private:
	client_textmessage_t		*m_pMessages[maxHUDMessages];
	float						m_startTime[maxHUDMessages];
	message_parms_t				m_parms;
	float						m_gameTitleTime;
	client_textmessage_t		*m_pGameTitle;

	int m_HUD_title_life;
	int m_HUD_title_half;
};

//
//-----------------------------------------------------
//
#define MAX_SPRITE_NAME_LENGTH	24

#include "hud_inventory.h"
#include "hud_custom.h"
#include "client_crypto.h"

class CHud
{
private:
	HUDLIST			*m_pHudList;
	client_sprite_t	*m_pSpriteList;
	int				m_iSpriteCount;
	int				m_iSpriteCountAllRes;
	float			m_flMouseSensitivity;
	float			m_flConcussionScale;
	float			m_flConcussionFrequency;
	float			m_flConcussionFadeTime;
	int				m_iClientIndex;
	int				m_iClientUserID;
	char			m_szClientAuthID[44];

public:
	HSPRITE	m_hsprCursor;
	float	m_flTimeEnd;
	float	m_flTime;
	float	m_fOldTime;
	double	m_flTimeDelta;
	Vector	m_vecOrigin;
	Vector	m_vecAngles;
	int		m_iKeyBits;
	int		m_iHideHUDDisplay;
	int		m_iFOV;
	int		m_Teamplay;
	bool	m_bClassicMode;
	bool	m_bViewModelPos;
	Vector	m_vecViewModelPos;

	cvar_t	*m_pCvarDraw;
	cvar_t	*m_pCvarDebug;
	cvar_t	*m_pCvarHideCustom;
	cvar_t	*m_pCvarBorderSize;
	cvar_t	*m_pCvarDefaultAlpha;
	cvar_t	*m_pCvarMaxAlpha;
	cvar_t	*m_pCvarStealMouse;
	cvar_t	*m_pCvarBorderless;

	int m_iFontHeight;
	HSPRITE LoadSprite(const char *name, wrect_t &rect);
	int CalcPosition(float position, int size, int screenSize, bool absolute, bool centered, bool ignoreBorder);
	int CalcXPosition(float position, int width, bool absolute, bool centered, bool ignoreBorder);
	int CalcYPosition(float position, int height, bool absolute, bool centered, bool ignoreBorder);
	int CalcXPositionLegacy(float position, int width);
	int CalcYPositionLegacy(float position, int height);
	RGBA GetEffectColor(int effect, const RGBA &first, const RGBA &second, float period, float time);
	void CheckGetSpriteSize(HSPRITE sprite, int frame, wrect_t &rect);
	int DrawConsoleString(int x, int y, const char *string);
	int DrawHudNumber(int x, int y, int iFlags, int iNumber, int r, int g, int b );
	int DrawString(int x, int y, int maxX, char *string, int r, int g, int b, int font);
	int DrawStringReverse(int x, int y, int minX, char *string, int r, int g, int b, int font);
	int DrawNumberString(int x, int y, int minX, int number, int r, int g, int b);
	int GetNumWidth(int iNumber, int iFlags);
	int DrawSprite( int x, int y, HSPRITE sprite, const wrect_t *rect, int r, int g, int b, int frame = 0, SPR_MODE mode = SPR_ADDITIVE );
	int DrawNumber( int x, int y, int number, int r, int g, int b, int minDigits, int maxDigits, int flags );
	int GetDigitWidth( void );
	int GetNumDigits( int number, int flags );
	bool ShowSign( int number, int flags );
	int GetSpaceWidthForDigits(int digits, int minDigits, int maxDigits, bool sign);
	int GetWidthForDigits(int digits, int minDigits, int maxDigits, bool sign);
	int GetNumberWidth( int number, int minDigits, int maxDigits, int flags );
	int GetSpaceWidth( void );
	int GetBorderSize( void );
	int GetWidth( void );
	int GetHeight( void );
	int GetSeparatorWidth( void );
	int DrawSeparator( int x, int y, int r, int g, int b, int a );
	int GetLineSeparatorHeight(void);
	int GetLineY( int line );
	int GetDefaultAlpha( void );
	int GetMaxAlpha( void );
	void UpdateFade( float &fade );
	byte GetFadeAlpha( float fade );

private:
	// the memory for these arrays are allocated in the first call to CHud::VidInit(), when the hud.txt and associated sprites are loaded.
	// freed in ~CHud()
	HSPRITE *m_rghSprites;	/*[HUD_SPRITE_COUNT]*/			// the sprites loaded from hud.txt
	wrect_t *m_rgrcRects;	/*[HUD_SPRITE_COUNT]*/
	char *m_rgszSpriteNames; /*[HUD_SPRITE_COUNT][MAX_SPRITE_NAME_LENGTH]*/

	cvar_t *default_fov;
public:
	HSPRITE GetSprite( int index ) 
	{
		return (index < 0) ? 0 : m_rghSprites[index];
	}

	wrect_t& GetSpriteRect( int index )
	{
		return m_rgrcRects[index];
	}

	
	int GetSpriteIndex( const char *SpriteName );	// gets a sprite index, for use in the m_rghSprites[] array

	CHudHealth			m_Health;
	CHudBattery			m_Battery;
	CHudAmmo			m_Ammo;
	CHudTrain			m_Train;
	CHudFlashlight		m_Flash;
	CHudCrosshair		m_Crosshair;
	CHudMessage			m_Message;
	CHudSayText			m_SayText;
	CHudNotifyText		m_NotifyText;
	CHudMenu			m_Menu;
	CHudTextMessage		m_TextMessage;
	CHudInventoryIcon	m_Inventory;
	CHudGeiger			m_Geiger;
	CHudCustom			m_Custom;

	void Init( void );
	void VidInit( void );
	void Shutdown(void);
	void Think(void);
	void DrawElements(void);
	int Redraw( float flTime, int intermission );
	int UpdateClientData( client_data_t *cdata, float time );

	CHud() : m_iSpriteCount(0), m_pHudList(NULL)
	{
		m_pCryptKeys[0] = m_pCryptKeys[1] = NULL;
	}
	~CHud();			// destructor, frees allocated memory

	unsigned char *GetCryptKey(unsigned int index, bool force = false);
	void ReleaseCryptKey(unsigned int index);

	// user messages
	int _cdecl MsgFunc_Damage(const char *pszName, int iSize, void *pbuf );
	int _cdecl MsgFunc_ClServerInfo(const char *pszName, int iSize, void *pbuf);
	int _cdecl MsgFunc_ClExtrasInfo(const char *pszName, int iSize, void *pbuf);
	int _cdecl MsgFunc_GameMode(const char *pszName, int iSize, void *pbuf );
	int _cdecl MsgFunc_ResetHUD(const char *pszName,  int iSize, void *pbuf);
	void _cdecl MsgFunc_InitHUD( const char *pszName, int iSize, void *pbuf );
	void _cdecl MsgFunc_PrtlUpdt(const char *pszName, int iSize, void *pbuf);
	void MsgFunc_CdAudio(const char *pszName, int iSize, void *pbuf);
	void _cdecl MsgFunc_ViewMode( const char *pszName, int iSize, void *pbuf );
	int _cdecl MsgFunc_SetFOV(const char *pszName,  int iSize, void *pbuf);
	int _cdecl MsgFunc_VModelPos(const char *pszName, int iSize, void *pbuf);
	int _cdecl MsgFunc_ClassicMode(const char *pszName, int iSize, void *pbuf);
	int _cdecl MsgFunc_Fog(const char *pszName, int iSize, void *pbuf);
	int  _cdecl MsgFunc_Concuss( const char *pszName, int iSize, void *pbuf );

	// Screen information
	SCREENINFO	m_scrinfo;

	int m_iIntermission;

	// sprite indexes
	int m_HUD_number_0;
	int m_HUD_minus;
	int m_HUD_plus;

	int		m_iWeaponBits;
	char	m_szServerVersion[33];
	char	m_szServerBuild[129];

	CClientCrypto	m_Crypto;
	unsigned char	*m_pCryptKeys[2];

	void AddHudElem(CHudBase *p);

	float GetSensitivity();

};

extern CHud gHUD;

// green debug markers, drawn when hud_debug is set
void DRAW_DEBUG_LINE_V( int x, int y );
void DRAW_DEBUG_LINE_H( int x, int y );
void DRAW_DEBUG_CROSS( int x, int y );
void DRAW_DEBUG_RECT( int x, int y, int width, int height );

extern int g_iPlayerClass;
extern int g_iTeamNumber;
extern int g_iUser1;
extern int g_iUser2;
extern int g_iUser3;

