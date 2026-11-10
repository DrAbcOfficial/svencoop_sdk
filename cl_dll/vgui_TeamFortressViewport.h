
#ifndef TEAMFORTRESSVIEWPORT_H
#define TEAMFORTRESSVIEWPORT_H

#include<VGUI_Panel.h>
#include<VGUI_Frame.h>
#include<VGUI_TextPanel.h>
#include<VGUI_Label.h>
#include<VGUI_Button.h>
#include<VGUI_ActionSignal.h>
#include<VGUI_InputSignal.h>
#include<VGUI_Scheme.h>
#include<VGUI_Image.h>
#include<VGUI_FileInputStream.h>
#include<VGUI_BitmapTGA.h>
#include<VGUI_DesktopIcon.h>
#include<VGUI_App.h>
#include<VGUI_MiniApp.h>
#include<VGUI_LineBorder.h>
#include<VGUI_String.h>
#include<VGUI_ScrollPanel.h>
#include<VGUI_ScrollBar.h>
#include<VGUI_Slider.h>

// custom scheme handling
#include "vgui_SchemeManager.h"

#define TF_DEFS_ONLY
#ifdef _TFC
#include "../tfc/tf_defs.h"
#else
#define PC_LASTCLASS 10
#define PC_UNDEFINED 0
#define MENU_DEFAULT				1
#define MENU_TEAM 					2
#define MENU_CLASS 					3
#define MENU_MAPBRIEFING			4
#define MENU_INTRO 					5
#define MENU_CLASSHELP				6
#define MENU_CLASSHELP2 			7
#define MENU_REPEATHELP 			8
#define MENU_SPECHELP				9
#endif

// Game menus
#define MENU_SWITCH_TO_MAPBRIEFING	9	// replaces the open text window
#define MENU_SWITCH_TO_INTRO		10
#define MENU_VOTE					22
#define MENU_MAPVOTE				23
#define MENU_VOTEPOPUP				24
#define MENU_CAMERA					25
#define MENU_INVENTORY				26
#define MENU_MEDIASELECT			30
using namespace vgui;

class Cursor;
class ScorePanel;
class SpectatorPanel;
class CCommandMenu;
class CommandLabel;
class CommandButton;
class CMenuPanel;
class ServerBrowser;
class DragNDropPanel;
class CTransparentPanel;
class CMessageWindowPanel;
class CMediaSelect;
class CVotePanel;
class CVotePopup;
class CMapVotePanel;
class CCameraPanel;
class CInventoryPanel;
class TeamFortressViewport;

char* GetVGUITGAName(const char *pszName);
BitmapTGA *LoadTGAForRes(const char* pImageName);
void ScaleColors( int &r, int &g, int &b, int a );
extern const char *sTFClassSelection[];
extern int sTFValidClassInts[];
extern const char *sLocalisedClasses[];
extern int iTeamColors[6][3];
extern int iNumberOfTeamColors;
extern TeamFortressViewport *gViewPort;

#define MAX_SERVERNAME_LENGTH	128
#define MAX_TEAMNAME_SIZE 64

// Command Menu positions 
#define MAX_MENUS				40
#define MAX_BUTTONS				100

#define BUTTON_SIZE_Y			YRES(30)
#define CMENU_SIZE_X			XRES(160)

#define SUBMENU_SIZE_X			(CMENU_SIZE_X / 8)
#define SUBMENU_SIZE_Y			(BUTTON_SIZE_Y / 6)

#define CMENU_TOP				(BUTTON_SIZE_Y * 4)

//#define MAX_TEAMNAME_SIZE		64
#define MAX_BUTTON_SIZE			32

// Map Briefing Window
#define MAPBRIEF_INDENT			30

// Team Menu
#define TMENU_INDENT_X			(30 * ((float)ScreenHeight / 640))
#define TMENU_HEADER			100
#define TMENU_SIZE_X			(ScreenWidth - (TMENU_INDENT_X * 2))
#define TMENU_SIZE_Y			(TMENU_HEADER + BUTTON_SIZE_Y * 7)
#define TMENU_PLAYER_INDENT		(((float)TMENU_SIZE_X / 3) * 2)
#define TMENU_INDENT_Y			(((float)ScreenHeight - TMENU_SIZE_Y) / 2)

// Class Menu
#define CLMENU_INDENT_X			(30 * ((float)ScreenHeight / 640))
#define CLMENU_HEADER			100
#define CLMENU_SIZE_X			(ScreenWidth - (CLMENU_INDENT_X * 2))
#define CLMENU_SIZE_Y			(CLMENU_HEADER + BUTTON_SIZE_Y * 11)
#define CLMENU_PLAYER_INDENT	(((float)CLMENU_SIZE_X / 3) * 2)
#define CLMENU_INDENT_Y			(((float)ScreenHeight - CLMENU_SIZE_Y) / 2)

// Arrows
enum
{
	ARROW_UP,
	ARROW_DOWN,
	ARROW_LEFT,
	ARROW_RIGHT,
};

//==============================================================================
// VIEWPORT PIECES
//============================================================
// Wrapper for an Image Label without a background
class CImageLabel : public Label
{
public:
	BitmapTGA	*m_pTGA;

public:
	void LoadImage(const char * pImageName);
	CImageLabel( const char* pImageName,int x,int y );
	CImageLabel( const char* pImageName,int x,int y,int wide,int tall );

	virtual int getImageTall();
	virtual int getImageWide();

	virtual void paintBackground()
	{
		// Do nothing, so the background's left transparent.
	}
};

// Command Label
// Overridden label so we can darken it when submenus open
class CommandLabel : public Label
{
private:
	int		m_iState;

public:
	CommandLabel(const char* text,int x,int y,int wide,int tall) : Label(text,x,y,wide,tall)
	{
		m_iState = false;
	}

	void PushUp()
	{
		m_iState = false;
		repaint();
	}

	void PushDown()
	{
		m_iState = true;
		repaint();
	}
};

//============================================================
// Command Buttons
class CommandButton : public Button
{
private:
	friend class CVotePanel;
	friend class CMapVotePanel;

	// Submenus under this button
	CCommandMenu *m_pSubMenu;
	CCommandMenu *m_pParentMenu;
	CommandLabel *m_pSubLabel;

	char	m_sMainText[64];
	char	m_cBoundKey;
	bool	m_bNoKeyPrefix;

	SchemeHandle_t m_hTextScheme;

	void RecalculateText( void );

public:
	bool	m_bNoHighlight;
	bool	m_bFlat;
	bool	m_bNoBorder;

public:
	CommandButton(const char* text,int x,int y,int wide,int tall, bool bNoHighlight, bool bFlat);
	// Constructors
	CommandButton( const char* text,int x,int y,int wide,int tall, bool bNoHighlight = false);
	CommandButton( int iPlayerClass, const char* text,int x,int y,int wide,int tall, bool bFlat );

	void Init( void );

	// Menu Handling
	void AddSubMenu( CCommandMenu *pNewMenu );
	void AddSubLabel( CommandLabel *pSubLabel )
	{
		m_pSubLabel = pSubLabel;
	}

	virtual int IsNotValid( void )
	{
		return false;
	}

	void UpdateSubMenus( int iAdjustment );
	CCommandMenu *GetSubMenu() { return m_pSubMenu; };
	
	CCommandMenu *getParentMenu( void );
	void setParentMenu( CCommandMenu *pParentMenu );

	// Overloaded vgui functions
	virtual void paint();
	virtual void setText( const char *text );
	virtual void paintBackground();

	void cursorEntered( void );
	void cursorExited( void );

	void setBoundKey( char boundKey );
	char getBoundKey( void );
	void setNoKeyPrefix(bool noPrefix);
};

class ColorButton : public CommandButton
{
private:

	Color *ArmedColor;
	Color *UnArmedColor;

	Color *ArmedBorderColor;
	Color *UnArmedBorderColor;

public:
	ColorButton( const char* text,int x,int y,int wide,int tall, bool bNoHighlight, bool bFlat ) : 
	  CommandButton( text, x, y, wide, tall, bNoHighlight, bFlat  ) 
	  {
		  ArmedColor = NULL;
		  UnArmedColor = NULL;
		  ArmedBorderColor = NULL;
		  UnArmedBorderColor = NULL;
	  }
	

	virtual void paintBackground()
	{
		int r, g, b, a;
		Color bgcolor;

		if ( isArmed() )
		{
			// Highlight background
		/*	getBgColor(bgcolor);
			bgcolor.getColor(r, g, b, a);
			drawSetColor( r,g,b,a );
			drawFilledRect(0,0,_size[0],_size[1]);*/

			if ( ArmedBorderColor )
			{
				ArmedBorderColor->getColor( r, g, b, a);
				drawSetColor( r, g, b, a );
				drawOutlinedRect(0,0,_size[0],_size[1]);
			}
		}
		else
		{
			if ( UnArmedBorderColor )
			{
				UnArmedBorderColor->getColor( r, g, b, a);
				drawSetColor( r, g, b, a );
				drawOutlinedRect(0,0,_size[0],_size[1]);
			}
		}
	}
	void paint()
	{
		int r, g, b, a;
		if ( isArmed() )
		{
			if (ArmedColor)
			{
				ArmedColor->getColor(r, g, b, a);
				setFgColor(r, g, b, a);
			}
			else
				setFgColor( Scheme::sc_secondary2 );
		}
		else
		{
			if (UnArmedColor)
			{
				UnArmedColor->getColor(r, g, b, a);
				setFgColor(r, g, b, a);
			}
			else
				setFgColor( Scheme::sc_primary1 );
		}
		
		Button::paint();
	}
	
	void setArmedColor ( int r, int g, int b, int a )
	{
		ArmedColor = new Color( r, g, b, a );
	}

	void setUnArmedColor ( int r, int g, int b, int a )
	{
		UnArmedColor = new Color( r, g, b, a );
	}

	void setArmedBorderColor ( int r, int g, int b, int a )
	{
		ArmedBorderColor = new Color( r, g, b, a );
	}

	void setUnArmedBorderColor ( int r, int g, int b, int a )
	{
		UnArmedBorderColor = new Color( r, g, b, a );
	}
};

class SpectButton : public CommandButton
{
private:
	
public:
	SpectButton( int iPlayerClass, const char* text,int x,int y,int wide,int tall ) : 
	  CommandButton( text, x, y, wide, tall, false)
	  {
		  	Init();
			
			setText( text );
	  }
		
	virtual void paintBackground()
	{
		if ( isArmed())
		{
			drawSetColor( 143,143, 54, 125 ); 
			drawFilledRect( 5, 0,_size[0] - 5,_size[1]);
		}
	}

	virtual void paint()
	{
	
		if ( isArmed() )
		{
			setFgColor( 194, 202, 54, 0 );
		}
		else
		{
			setFgColor( 143, 143, 54, 15 );
		}

		Button::paint();
	}
};
//============================================================
// Command Menus
class CCommandMenu : public Panel
{
private:
	CCommandMenu *m_pParentMenu;
	int			  m_iXOffset;
	int			  m_iYOffset;

	// Buttons in this menu
	CommandButton *m_aButtons[ MAX_BUTTONS ];
	int			  m_iButtons;

public:
	CCommandMenu( CCommandMenu *pParentMenu, int x,int y,int wide,int tall ) : Panel(x,y,wide,tall)
	{
		m_pParentMenu = pParentMenu;
		m_iXOffset = x;
		m_iYOffset = y;
		m_iButtons = 0;
	}

	void		AddButton( CommandButton *pButton );
	bool		RecalculateVisibles( int iNewYPos, bool bHideAll );
	void		RecalculatePositions( int iYOffset );
	void		MakeVisible( CCommandMenu *pChildMenu );

	CCommandMenu *GetParentMenu() { return m_pParentMenu; };
	int			GetXOffset() { return m_iXOffset; };
	int			GetYOffset() { return m_iYOffset; };
	int			GetNumButtons() { return m_iButtons; };
	CommandButton *FindButtonWithSubmenu( CCommandMenu *pSubMenu );

	void		ClearButtonsOfArmedState( void );

	void		RemoveAllButtons(void);


	bool		KeyInput( int keyNum );

	virtual void paintBackground();
};

//==============================================================================
// Command menu root button (drop down box style)

class DropDownButton : public ColorButton
{
private:
	CImageLabel *m_pOpenButton;

public:

		DropDownButton( const char* text,int x,int y,int wide,int tall, bool bNoHighlight, bool bFlat ) : 
						ColorButton( text, x, y, wide, tall, bNoHighlight, bFlat  ) 
	  {
			// Put a > to show it's a submenu
			m_pOpenButton = new CImageLabel( "arrowup", XRES( CMENU_SIZE_X-2 ) , YRES( BUTTON_SIZE_Y-2 ) );
			m_pOpenButton->setParent(this);
	
			int textwide, texttall;
			getSize( textwide, texttall);
		
			// Reposition
			m_pOpenButton->setPos( textwide-(m_pOpenButton->getImageWide()+6), -2 /*(tall - m_pOpenButton->getImageTall()*2) / 2*/ );
			m_pOpenButton->setVisible(true);

	  }

	virtual void   setVisible(bool state)
	{
		m_pOpenButton->setVisible(state);
		ColorButton::setVisible(state);
	}
	

};

//==============================================================================
// Button with image instead of text

class CImageButton : public ColorButton
{
private:
	CImageLabel *m_pOpenButton;

public:

		CImageButton( const char* text,int x,int y,int wide,int tall, bool bNoHighlight, bool bFlat ) : 
						ColorButton( " ", x, y, wide, tall, bNoHighlight, bFlat  ) 
	  {
			m_pOpenButton = new CImageLabel( text,1,1,wide-2 , tall-2 );
			m_pOpenButton->setParent(this);
	
			// Reposition
		//	m_pOpenButton->setPos( x+1,y+1 );
		//	m_pOpenButton->setSize(wide-2,tall-2);

			m_pOpenButton->setVisible(true);
	  }

	virtual void   setVisible(bool state)
	{
		m_pOpenButton->setVisible(state);
		ColorButton::setVisible(state);
	}
	

};

//==============================================================================
class TeamFortressViewport : public Panel
{
private:
	vgui::Cursor	*_cursorNone;
	vgui::Cursor	*_cursorArrow;
	bool			m_iInitialized;

	CCommandMenu	*m_pCommandMenus[MAX_MENUS];
	CCommandMenu	*m_pCurrentCommandMenu;
	float			m_flMenuOpenTime;
	float			m_flScoreBoardLastUpdated;
	float			m_flSpectatorPanelLastUpdated;
	int				m_iNumMenus;
	int				m_iCurrentTeamNumber;
	int				m_iUser1;
	int				m_iUser2;
	unsigned char	m_Unknown392[8];

	CSchemeManager	m_SchemeManager;
	bool			m_iGotAllMOTD;
	char			m_szMOTD[MAX_MOTD_LENGTH];

	CommandButton	*m_pTeamButtons[6];
	bool			m_iAllowSpectators;
	int				m_iValidClasses[5];
	int				m_iNumberOfTeams;
	int				m_iTeamColors[5][3];
	char			m_sTeamNames[5][MAX_TEAMNAME_SIZE];
	char			m_sDetpackStrings[3][MAX_TEAMNAME_SIZE];

	CMessageWindowPanel	*m_pTextWindows[2];

	void CreateSpectatorMenu( void );
	void CreateTeamMenu( void );
	void CreateCameraMenu( void );
	void CreateMediaSelectMenu( void );
	void CreateVoteMenu( void );
	void CreateInventoryMenu( void );

public:
	TeamFortressViewport(int x,int y,int wide,int tall);
	void Initialize( void );
	void VidInit( void );

	void CreateCommandMenu( void );
	void CreateScoreBoard( void );
	CommandButton *CreateCustomButton( char *pButtonText, char *pButtonName );
	CCommandMenu *CreateSubMenu( CommandButton *pButton, CCommandMenu *pParentMenu );

	void UpdateCursorState( void );
	void UpdateCommandMenu( void );
	void UpdateOnPlayerInfo( void );
	void UpdateHighlights( void );
	void UpdateSpectatorMenu( void );

	int KeyInput( int down, int keynum, const char *pszCurrentBinding );
	void GetAllPlayersInfo( void );
	void DeathMsg( int killer, int victim );

	void ShowCommandMenu( void );
	void InputSignalHideCommandMenu( void );
	void HideCommandMenu( void );
	void SetCurrentCommandMenu( CCommandMenu *pNewMenu );
	void SetCurrentMenu( CMenuPanel *pMenu );

	void ShowScoreBoard( void );
	void HideScoreBoard( void );
	bool IsScoreBoardVisible( void );
	bool AllowedToPrintText( void );

	void ShowVGUIMenu( int iMenu );
	CMenuPanel *ShowTeamMenu( void );
	CMenuPanel *ShowVoteMenu( void );
	CMenuPanel *ShowMapVoteMenu( void );
	CMenuPanel *ShowVotePopup( void );
	CMenuPanel *ShowCameraMenu( void );
	CMenuPanel *ShowInventoryMenu( void );
	CMenuPanel *ShowMediaSelectMenu( void );
	void SetEndOfTime( float flTime );
	void HideVGUIMenu( void );
	void HideTopMenu( void );
	void RemoveMenu( CMenuPanel *pMenu );
	CMenuPanel *CreateTextWindow( int iTextToShow );

	int GetValidClasses(int iTeam) { return m_iValidClasses[iTeam]; }
	int GetNumberOfTeams() { return m_iNumberOfTeams; }
	char *GetTeamName(int iTeam)
	{
		return iTeam == 5 ? m_sDetpackStrings[0] : m_sTeamNames[iTeam];
	}
	int GetAllowSpectators() { return m_iAllowSpectators; }
	int GetObserverMode() { return m_iUser1; }
	int GetObserverTarget() { return m_iUser2; }

	int MsgFunc_ValClass( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_TeamNames( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_VGUIMenu( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_MOTD( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_ServerName( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_ScoreInfo( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_TeamScore( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_TeamInfo( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_Spectator( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_AllowSpec( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_NextMap( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_VoteMenu( const char *pszName, int iSize, void *pbuf );
	int MsgFuncs_EndVote( const char *pszName, int iSize, void *pbuf );

	bool SlotInput( int iSlot );
	virtual void paintBackground();

	CSchemeManager *GetSchemeManager( void ) { return &m_SchemeManager; }
	ScorePanel *GetScoreBoard( void ) { return m_pScoreBoard; }
	CCameraPanel *GetCameraPanel( void ) { return m_pCameraPanel; }
	CInventoryPanel *GetInventoryPanel( void ) { return m_pInventory; }

	void *operator new( size_t stAllocateBlock );

public:
	CMenuPanel		*m_pCurrentMenu;
	unsigned char	m_Unknown5152[12];
	ScorePanel		*m_pScoreBoard;
	SpectatorPanel	*m_pSpectatorPanel;
	char			m_szServerName[MAX_SERVERNAME_LENGTH];
	char			m_szServerHostname[MAX_SERVERNAME_LENGTH];
	unsigned char	m_Unknown5300[160];
	char			m_sMapName[64];
	char			m_sNextMapName[64];

	CMediaSelect	*m_pMediaSelect;
	CVotePanel		*m_pVotePanel;
	CVotePopup		*m_pVotePopup;
	CMapVotePanel	*m_pMapVotePanel;
	CCameraPanel	*m_pCameraPanel;
	CInventoryPanel	*m_pInventory;
};
//============================================================
// Command Menu Button Handlers
#define MAX_COMMAND_SIZE	256

class CMenuHandler_StringCommand : public ActionSignal
{
protected:
	char	m_pszCommand[MAX_COMMAND_SIZE];
	int		m_iCloseVGUIMenu;
public:
	CMenuHandler_StringCommand( const char *pszCommand )
	{
		strncpy( m_pszCommand, pszCommand, MAX_COMMAND_SIZE);
		m_pszCommand[MAX_COMMAND_SIZE-1] = '\0';
		m_iCloseVGUIMenu = false;
	}

	CMenuHandler_StringCommand( const char *pszCommand, int iClose )
	{
		strncpy( m_pszCommand, pszCommand, MAX_COMMAND_SIZE);
		m_pszCommand[MAX_COMMAND_SIZE-1] = '\0';
		m_iCloseVGUIMenu = true;
	}

	virtual void actionPerformed(Panel* panel)
	{
		gEngfuncs.pfnClientCmd(m_pszCommand);

		if (m_iCloseVGUIMenu)
			gViewPort->HideTopMenu();
		else
			gViewPort->HideCommandMenu();
	}
};

// This works the same as CMenuHandler_StringCommand, except it watches the string command 
// for specific commands, and modifies client vars based upon them.
class CMenuHandler_StringCommandWatch : public CMenuHandler_StringCommand
{
private:
public:
	CMenuHandler_StringCommandWatch( const char *pszCommand ) : CMenuHandler_StringCommand( pszCommand )
	{
	}

	CMenuHandler_StringCommandWatch( const char *pszCommand, int iClose ) : CMenuHandler_StringCommand( pszCommand, iClose )
	{
	}

	virtual void actionPerformed(Panel* panel)
	{
		CMenuHandler_StringCommand::actionPerformed( panel );

		// Try to guess the player's new team (it'll be corrected if it's wrong)
		if ( !strcmp( m_pszCommand, "leaveteam" ) )
			g_iTeamNumber = 0;
		else if ( !strcmp( m_pszCommand, "jointeam 1" ) )
			g_iTeamNumber = 1;
		else if ( !strcmp( m_pszCommand, "jointeam 2" ) )
			g_iTeamNumber = 2;
		else if ( !strcmp( m_pszCommand, "jointeam 3" ) )
			g_iTeamNumber = 3;
		else if ( !strcmp( m_pszCommand, "jointeam 4" ) )
			g_iTeamNumber = 4;
	}
};

// Used instead of CMenuHandler_StringCommand for Class Selection buttons.
// Checks the state of hud_classautokill and kills the player if set
class CMenuHandler_StringCommandClassSelect : public CMenuHandler_StringCommand
{
private:
public:
	CMenuHandler_StringCommandClassSelect( const char *pszCommand ) : CMenuHandler_StringCommand( pszCommand )
	{
	}

	CMenuHandler_StringCommandClassSelect( const char *pszCommand, int iClose ) : CMenuHandler_StringCommand( pszCommand, iClose )
	{
	}

	virtual void actionPerformed(Panel* panel);
};

class CMenuHandler_PopupSubMenuInput : public InputSignal
{
private:
	CCommandMenu *m_pSubMenu;
	Button		 *m_pButton;
public:
	CMenuHandler_PopupSubMenuInput( Button *pButton, CCommandMenu *pSubMenu )
	{
		m_pSubMenu = pSubMenu;
		m_pButton = pButton;
	}

	virtual void cursorMoved(int x,int y,Panel* panel)
	{
		//gViewPort->SetCurrentCommandMenu( m_pSubMenu );
	}

	virtual void cursorEntered(Panel* panel) 
	{
		gViewPort->SetCurrentCommandMenu( m_pSubMenu );

		if (m_pButton)
			m_pButton->setArmed(true);
	};
	virtual void cursorExited(Panel* Panel) {};
	virtual void mousePressed(MouseCode code,Panel* panel)  {};
	virtual void mouseDoublePressed(MouseCode code,Panel* panel)  {};
	virtual void mouseReleased(MouseCode code,Panel* panel) {};
	virtual void mouseWheeled(int delta,Panel* panel) {};
	virtual void keyPressed(KeyCode code,Panel* panel) {};
	virtual void keyTyped(KeyCode code,Panel* panel) {};
	virtual void keyReleased(KeyCode code,Panel* panel) {};
	virtual void keyFocusTicked(Panel* panel) {};
};

class CMenuHandler_LabelInput : public InputSignal
{
private:
	ActionSignal *m_pActionSignal;
public:
	CMenuHandler_LabelInput( ActionSignal *pSignal )
	{
		m_pActionSignal = pSignal;
	}

	virtual void mousePressed(MouseCode code,Panel* panel)
	{
		m_pActionSignal->actionPerformed( panel );
	}

	virtual void mouseReleased(MouseCode code,Panel* panel) {};
	virtual void cursorEntered(Panel* panel) {};
	virtual void cursorExited(Panel* Panel) {};
	virtual void cursorMoved(int x,int y,Panel* panel) {};
	virtual void mouseDoublePressed(MouseCode code,Panel* panel)  {};
	virtual void mouseWheeled(int delta,Panel* panel) {};
	virtual void keyPressed(KeyCode code,Panel* panel) {};
	virtual void keyTyped(KeyCode code,Panel* panel) {};
	virtual void keyReleased(KeyCode code,Panel* panel) {};
	virtual void keyFocusTicked(Panel* panel) {};
};

#define HIDE_TEXTWINDOW		0
#define SHOW_MAPBRIEFING	1
#define SHOW_CLASSDESC		2
#define SHOW_MOTD			3
#define SHOW_SPECHELP		4

class CMenuHandler_TextWindow : public ActionSignal
{
private:
	int		m_iState;
	bool	m_bCloseCurrent;
public:
	CMenuHandler_TextWindow( int iState, bool bCloseCurrent = false )
	{
		m_iState = iState;
		m_bCloseCurrent = bCloseCurrent;
	}

	virtual void actionPerformed(Panel* panel)
	{
		if (m_iState == HIDE_TEXTWINDOW)
		{
			gViewPort->HideTopMenu();
		}
		else 
		{
			if (m_bCloseCurrent)
				gViewPort->HideTopMenu();

			gViewPort->HideCommandMenu();
			gViewPort->ShowVGUIMenu( m_iState );
		}
	}
};

class CMenuHandler_ToggleCvar : public ActionSignal
{
private:
	cvar_t* m_cvar;

public:
	CMenuHandler_ToggleCvar( char * cvarname )
	{
		m_cvar = gEngfuncs.pfnGetCvarPointer( cvarname );
	}

	virtual void actionPerformed(Panel* panel)
	{
		if ( m_cvar->value )
			m_cvar->value = 0.0f;
		else
			m_cvar->value = 1.0f;

		// hide the menu 
		gViewPort->HideCommandMenu();

		gViewPort->UpdateSpectatorMenu();
	}

	
};

class CDragNDropHandler : public InputSignal
{
private:
	DragNDropPanel*	m_pPanel;
	bool			m_bDragging;
	int				m_iaDragOrgPos[2];
	int				m_iaDragStart[2];

public:
	CDragNDropHandler(DragNDropPanel* pPanel)
	{
		m_pPanel = pPanel;
		m_bDragging = false;
	}

	void cursorMoved(int x,int y,Panel* panel);
	void mousePressed(MouseCode code,Panel* panel);
	void mouseReleased(MouseCode code,Panel* panel);

	void mouseDoublePressed(MouseCode code,Panel* panel) {};
	void cursorEntered(Panel* panel) {};
	void cursorExited(Panel* panel) {};
	void mouseWheeled(int delta,Panel* panel) {};
	void keyPressed(KeyCode code,Panel* panel) {};
	void keyTyped(KeyCode code,Panel* panel) {};
	void keyReleased(KeyCode code,Panel* panel) {};
	void keyFocusTicked(Panel* panel) {};
};

class CHandler_MenuButtonOver : public InputSignal
{
private:
	int			m_iButton;
	CMenuPanel	*m_pMenuPanel;
public:
	CHandler_MenuButtonOver( CMenuPanel *pPanel, int iButton )
	{
		m_iButton = iButton;
		m_pMenuPanel = pPanel;
	}
		
	void cursorEntered(Panel *panel);

	void cursorMoved(int x,int y,Panel* panel) {};
	void mousePressed(MouseCode code,Panel* panel) {};
	void mouseReleased(MouseCode code,Panel* panel) {};
	void mouseDoublePressed(MouseCode code,Panel* panel) {};
	void cursorExited(Panel* panel) {};
	void mouseWheeled(int delta,Panel* panel) {};
	void keyPressed(KeyCode code,Panel* panel) {};
	void keyTyped(KeyCode code,Panel* panel) {};
	void keyReleased(KeyCode code,Panel* panel) {};
	void keyFocusTicked(Panel* panel) {};
};

class CHandler_ButtonHighlight : public InputSignal
{
private:
	Button *m_pButton;
public:
	CHandler_ButtonHighlight( Button *pButton )
	{
		m_pButton = pButton;
	}

	virtual void cursorEntered(Panel* panel) 
	{ 
		m_pButton->setArmed(true);
	};
	virtual void cursorExited(Panel* Panel) 
	{
		m_pButton->setArmed(false);
	};
	virtual void mousePressed(MouseCode code,Panel* panel) {};
	virtual void mouseReleased(MouseCode code,Panel* panel) {};
	virtual void cursorMoved(int x,int y,Panel* panel) {};
	virtual void mouseDoublePressed(MouseCode code,Panel* panel)  {};
	virtual void mouseWheeled(int delta,Panel* panel) {};
	virtual void keyPressed(KeyCode code,Panel* panel) {};
	virtual void keyTyped(KeyCode code,Panel* panel) {};
	virtual void keyReleased(KeyCode code,Panel* panel) {};
	virtual void keyFocusTicked(Panel* panel) {};
};

//-----------------------------------------------------------------------------
// Purpose: Special handler for highlighting of command menu buttons
//-----------------------------------------------------------------------------
class CHandler_CommandButtonHighlight : public CHandler_ButtonHighlight
{
private:
	CommandButton *m_pCommandButton;
public:
	CHandler_CommandButtonHighlight( CommandButton *pButton ) : CHandler_ButtonHighlight( pButton )
	{
		m_pCommandButton = pButton;
	}

	virtual void cursorEntered( Panel *panel )
	{
		m_pCommandButton->cursorEntered();
	}

	virtual void cursorExited( Panel *panel )
	{
		m_pCommandButton->cursorExited();
	}
};


//================================================================
// Overidden Command Buttons for special visibilities

class TeamButton : public CommandButton
{
private:
	int	m_iTeamNumber;
public:
	TeamButton( int iTeam, const char* text,int x,int y,int wide,int tall ) : CommandButton( text,x,y,wide,tall)
	{
		m_iTeamNumber = iTeam;
	}

	virtual int IsNotValid()
	{
		int iTeams = gViewPort->GetNumberOfTeams();
		// Never valid if there's only 1 team
		if (iTeams == 1)
			return true;

		// Auto Team's always visible
		if (m_iTeamNumber == 5)
			return false;

		if (iTeams >= m_iTeamNumber && m_iTeamNumber != g_iTeamNumber)
			return false;

		return true;
	}
};


class SpectateButton : public CommandButton
{
public:
	SpectateButton( const char* text,int x,int y,int wide,int tall, bool bNoHighlight ) : CommandButton( text,x,y,wide,tall, bNoHighlight)
	{
	}

	virtual int IsNotValid()
	{
		// Only visible if the server allows it
		if ( gViewPort->GetAllowSpectators() != 0 )
			return false;

		return true;
	}
};

#define		DISGUISE_TEAM1		(1<<0)
#define		DISGUISE_TEAM2		(1<<1)
#define		DISGUISE_TEAM3		(1<<2)
#define		DISGUISE_TEAM4		(1<<3)

extern int iBuildingCosts[];
#define BUILDSTATE_HASBUILDING		(1<<0)		// Data is building ID (1 = Dispenser, 2 = Sentry, 3 = Entry Teleporter, 4 = Exit Teleporter)
#define BUILDSTATE_BUILDING			(1<<1)
#define BUILDSTATE_BASE				(1<<2)
#define BUILDSTATE_CANBUILD			(1<<3)		// Data is building ID (1 = Dispenser, 2 = Sentry, 3 = Entry Teleporter, 4 = Exit Teleporter)


#define MAX_MAPNAME 256

class MapButton : public CommandButton
{
private:
	char m_szMapName[ MAX_MAPNAME ];

public:
	MapButton( const char *pMapName, const char* text,int x,int y,int wide,int tall ) : CommandButton( text,x,y,wide,tall)
	{
		sprintf( m_szMapName, "maps/%s.bsp", pMapName );
	}

	virtual int IsNotValid()
	{
		const char *level = gEngfuncs.pfnGetLevelName();
		if (!level)
			return true;

		// Does it match the current map name?
		if ( strcmp(m_szMapName, level) )
			return true;

		return false;
	}
};

//-----------------------------------------------------------------------------
// Purpose: CommandButton which is only displayed if the player is on team X
//-----------------------------------------------------------------------------
class TeamOnlyCommandButton : public CommandButton
{
private:
	int m_iTeamNum;

public:
	TeamOnlyCommandButton( int iTeamNum, const char* text,int x,int y,int wide,int tall, bool flat ) : 
	  CommandButton( text, x, y, wide, tall, false, flat ), m_iTeamNum(iTeamNum) {}

	virtual int IsNotValid()
	{
		if ( g_iTeamNumber != m_iTeamNum )
			return true;

		return CommandButton::IsNotValid();
	}
};

//-----------------------------------------------------------------------------
// Purpose: CommandButton which is only displayed if the player is on team X
//-----------------------------------------------------------------------------
class ToggleCommandButton : public CommandButton, public InputSignal
{
private:
	cvar_t* m_cvar;
	CImageLabel*	pLabelOn;
	CImageLabel*	pLabelOff;
	

public:
	ToggleCommandButton( const char* cvarname, const char* text,int x,int y,int wide,int tall, bool flat ) : 
	  CommandButton( text, x, y, wide, tall, false, flat )
	 {
		m_cvar = gEngfuncs.pfnGetCvarPointer( cvarname );

			// Put a > to show it's a submenu
		pLabelOn = new CImageLabel( "checked", 0, 0 );
		pLabelOn->setParent(this);
		pLabelOn->addInputSignal(this);
				
		pLabelOff = new CImageLabel( "unchecked", 0, 0 );
		pLabelOff->setParent(this);
		pLabelOff->setEnabled(true);
		pLabelOff->addInputSignal(this);

		int textwide, texttall;
		getTextSize( textwide, texttall);
	
		// Reposition
		pLabelOn->setPos( textwide, (tall - pLabelOn->getTall()) / 2 );

		pLabelOff->setPos( textwide, (tall - pLabelOff->getTall()) / 2 );
		
		// Set text color to orange
		setFgColor(Scheme::sc_primary1);
	}

	virtual void cursorEntered(Panel* panel)
	{
		CommandButton::cursorEntered();
	}

	virtual void cursorExited(Panel* panel)
	{
		CommandButton::cursorExited();
	}

	virtual void mousePressed(MouseCode code,Panel* panel)
	{
		doClick();
	};

	virtual void cursorMoved(int x,int y,Panel* panel) {};
	
	virtual void mouseDoublePressed(MouseCode code,Panel* panel)  {};
	virtual void mouseReleased(MouseCode code,Panel* panel) {};
	virtual void mouseWheeled(int delta,Panel* panel) {};
	virtual void keyPressed(KeyCode code,Panel* panel) {};
	virtual void keyTyped(KeyCode code,Panel* panel) {};
	virtual void keyReleased(KeyCode code,Panel* panel) {};
	virtual void keyFocusTicked(Panel* panel) {};

	virtual void paint( void )
	{
		if ( !m_cvar )
		{
			pLabelOff->setVisible(false);
			pLabelOn->setVisible(false);
		} 
		else if ( m_cvar->value )
		{
			pLabelOff->setVisible(false);
			pLabelOn->setVisible(true);
		}
		else
		{
			pLabelOff->setVisible(true);
			pLabelOn->setVisible(false);
		}

		CommandButton::paint();

	} 
};
class SpectToggleButton : public CommandButton, public InputSignal
{
private:
	cvar_t* m_cvar;
	CImageLabel*	pLabelOn; 

public:
	SpectToggleButton( const char* cvarname, const char* text,int x,int y,int wide,int tall, bool flat ) : 
	  CommandButton( text, x, y, wide, tall, false, flat )
	 {
		m_cvar = gEngfuncs.pfnGetCvarPointer( cvarname );

		// Put a > to show it's a submenu
		pLabelOn = new CImageLabel( "checked", 0, 0 );
		pLabelOn->setParent(this);
		pLabelOn->addInputSignal(this);
		

		int textwide, texttall;
		getTextSize( textwide, texttall);
	
		// Reposition
		pLabelOn->setPos( textwide, (tall - pLabelOn->getTall()) / 2 );
	  } 

	virtual void cursorEntered(Panel* panel)
	{
		CommandButton::cursorEntered();
	}

	virtual void cursorExited(Panel* panel)
	{
		CommandButton::cursorExited();
	}

	virtual void mousePressed(MouseCode code,Panel* panel)
	{
		doClick();
	};

	virtual void cursorMoved(int x,int y,Panel* panel) {};
	
	virtual void mouseDoublePressed(MouseCode code,Panel* panel)  {};
	virtual void mouseReleased(MouseCode code,Panel* panel) {};
	virtual void mouseWheeled(int delta,Panel* panel) {};
	virtual void keyPressed(KeyCode code,Panel* panel) {};
	virtual void keyTyped(KeyCode code,Panel* panel) {};
	virtual void keyReleased(KeyCode code,Panel* panel) {};
	virtual void keyFocusTicked(Panel* panel) {};

	virtual void paintBackground()
	{
		if ( isArmed() )
		{
			drawSetColor( 143,143, 54, 125 ); 
			drawFilledRect( 5, 0,_size[0] - 5,_size[1]);
		}
	}
	
	virtual void paint( void )
	{
		if ( isArmed() )
		{ 
			setFgColor( 194, 202, 54, 0 );
		}
		else
		{
			setFgColor( 143, 143, 54, 15 );
		}

		if ( !m_cvar )
		{
			pLabelOn->setVisible(false);
		} 
		else if ( m_cvar->value )
		{
			pLabelOn->setVisible(true);
		}
		else
		{
			pLabelOn->setVisible(false);
		}

		Button::paint();
	} 
};

/*
class SpectToggleButton : public ToggleCommandButton
{
private:
	struct cvar_s * m_cvar;
	CImageLabel *	pLabelOn;
	CImageLabel *	pLabelOff;
	
public:

	SpectToggleButton( const char* cvarname, const char* text,int x,int y,int wide,int tall, bool flat ) : 
	  ToggleCommandButton( cvarname, text, x, y, wide, tall, flat, TRUE )
	 {
		m_cvar = gEngfuncs.pfnGetCvarPointer( cvarname );

			// Put a > to show it's a submenu
		pLabelOn = new CImageLabel( "checked", 0, 0 );
		pLabelOn->setParent(this);
		pLabelOn->addInputSignal(this);
				
		pLabelOff = new CImageLabel( "unchecked", 0, 0 );
		pLabelOff->setParent(this);
		pLabelOff->setEnabled(true);
		pLabelOff->addInputSignal(this);

		int textwide, texttall;
		getTextSize( textwide, texttall);
	
		// Reposition
		pLabelOn->setPos( textwide, (tall - pLabelOn->getTall()) / 2 );

		pLabelOff->setPos( textwide, (tall - pLabelOff->getTall()) / 2 );
		
		// Set text color to orange
		setFgColor(Scheme::sc_primary1);
	}
		
	virtual void paintBackground()
	{
		if ( isArmed())
		{
			drawSetColor( 143,143, 54, 125 ); 
			drawFilledRect( 5, 0,_size[0] - 5,_size[1]);
		}
	}

	virtual void paint()
	{
	
		if ( isArmed() )
		{ 
			setFgColor( 194, 202, 54, 0 );
		}
		else
		{
			setFgColor( 143, 143, 54, 15 );
		}

			if ( !m_cvar )
		{
			pLabelOff->setVisible(false);
			pLabelOn->setVisible(false);
		} 
		else if ( m_cvar->value )
		{
			pLabelOff->setVisible(false);
			pLabelOn->setVisible(true);
		}
		else
		{
			pLabelOff->setVisible(true);
			pLabelOn->setVisible(false);
		}

		Button::paint();
	}
};
*/
//============================================================
// Panel that can be dragged around
class DragNDropPanel : public Panel
{
private:
	bool		m_bBeingDragged;
	LineBorder	*m_pBorder;
public:
	DragNDropPanel(int x,int y,int wide,int tall) : Panel(x,y,wide,tall)
	{
		m_bBeingDragged = false;

		// Create the Drag Handler
		addInputSignal( new CDragNDropHandler(this) );

		// Create the border (for dragging)
		m_pBorder = new LineBorder();
	}

	virtual void setDragged( bool bState )
	{
		m_bBeingDragged = bState;

		if (m_bBeingDragged)
			setBorder(m_pBorder);
		else
			setBorder(NULL);
	}
};

//================================================================
// Panel that draws itself with a transparent black background
class CTransparentPanel : public Panel
{
	friend class CMessageWindowPanel;

private:
	Color	m_BackgroundColor;
public:
	CTransparentPanel(int iTrans, int x,int y,int wide,int tall) : Panel(x,y,wide,tall)
	{
		m_BackgroundColor = Color(0, 0, 0, 255 - iTrans);
	}

	virtual void paintBackground()
	{
		drawSetColor(m_BackgroundColor[0], m_BackgroundColor[1], m_BackgroundColor[2], 255 - m_BackgroundColor[3]);
		drawFilledRect(0, 0, _size[0], _size[1]);
	}
};

//================================================================
// Menu Panel that supports buffering of menus
class CMenuPanel : public CTransparentPanel
{
	friend class TeamFortressViewport;

private:
	CMenuPanel *m_pNextMenu;
	int			m_iMenuID;
	int			m_iRemoveMe;
	bool		m_iIsActive;
	float		m_flOpenTime;
public:
	CMenuPanel(int iRemoveMe, int x,int y,int wide,int tall) : CTransparentPanel(100, x,y,wide,tall)
	{
		Reset();
		m_iRemoveMe = iRemoveMe;
	}

	CMenuPanel(int iTrans, int iRemoveMe, int x,int y,int wide,int tall) : CTransparentPanel(iTrans, x,y,wide,tall)
	{
		Reset();
		m_iRemoveMe = iRemoveMe;
	}

	virtual void Reset( void )
	{
		m_pNextMenu = NULL;
		m_iIsActive = false;
		m_flOpenTime = 0;
	}

	void SetNextMenu( CMenuPanel *pNextPanel )
	{
		CMenuPanel *pMenu = this;

		while (pMenu != pNextPanel)
		{
			if (!pMenu->m_pNextMenu)
			{
				pMenu->m_pNextMenu = pNextPanel;
				return;
			}

			pMenu = pMenu->m_pNextMenu;
		}
	}

	void SetMenuID( int iID )
	{
		m_iMenuID = iID;
	}

	void SetActive( int iState )
	{
		m_iIsActive = iState;
	}

	virtual void Open( void )
	{
		setVisible( true );

		// Note the open time, so we can delay input for a bit
		m_flOpenTime = gHUD.m_flTime;
	}

	virtual void Close( void )
	{
		setVisible( false );
		m_iIsActive = false;

		if ( m_iRemoveMe )
			gViewPort->removeChild( this );

		// This MenuPanel has now been deleted. Don't append code here.
	}

	int			ShouldBeRemoved() { return m_iRemoveMe; };
	CMenuPanel* GetNextMenu() { return m_pNextMenu; };
	int			GetMenuID() { return m_iMenuID; };
	int			IsActive() { return m_iIsActive; };
	float		GetOpenTime() { return m_flOpenTime; };

	// Numeric input
	virtual bool SlotInput( int iSlot ) { return false; };
	virtual void SetActiveInfo( int iInput ) {};
};

//================================================================
// Custom drawn scroll bars
class CTFScrollButton : public CommandButton
{
private:
	BitmapTGA	*m_pTGA;

public:
	CTFScrollButton(int iArrow, const char* text,int x,int y,int wide,int tall);

	virtual void paint( void );
	virtual void paintBackground( void );
};

// Custom drawn slider bar
class CTFSlider : public Slider
{
public:
	CTFSlider(int x,int y,int wide,int tall,bool vertical) : Slider(x,y,wide,tall,vertical)
	{
	};

	virtual void paintBackground( void );
};

// Custom drawn scrollpanel
class CTFScrollPanel : public ScrollPanel
{
public:
	CTFScrollPanel(int x,int y,int wide,int tall);
};

//================================================================
// Menu Panels that take key input
//============================================================


//=========================================================
// Specific Menus to handle old HUD sections
class CHealthPanel : public DragNDropPanel
{
private:
	BitmapTGA	*m_pHealthTGA;
	Label		*m_pHealthLabel;
public:
	CHealthPanel(int x,int y,int wide,int tall) : DragNDropPanel(x,y,wide,tall)
	{
		// Load the Health icon
		FileInputStream* fis = new FileInputStream( GetVGUITGAName("%d_hud_health"), false);
		m_pHealthTGA = new BitmapTGA(fis,true);
		fis->close();

		// Create the Health Label
		int iXSize,iYSize;
		m_pHealthTGA->getSize(iXSize,iYSize);
		m_pHealthLabel = new Label("",0,0,iXSize,iYSize);
		m_pHealthLabel->setImage(m_pHealthTGA);
		m_pHealthLabel->setParent(this);

		// Set panel dimension
		// Shouldn't be needed once Billy's fized setImage not recalculating the size
		//setSize( iXSize + 100, gHUD.m_iFontHeight + 10 );
		//m_pHealthLabel->setPos( 10, (getTall() - iYSize) / 2 );
	}

	virtual void paintBackground()
	{
	}

	void paint()
	{
		// Get the paint color
		int r,g,b,a;
		// Has health changed? Flash the health #
		if (gHUD.m_Health.m_fFade)
		{
			gHUD.m_Health.m_fFade -= (gHUD.m_flTimeDelta * 20);
			if (gHUD.m_Health.m_fFade <= 0)
			{
				a = MIN_ALPHA;
				gHUD.m_Health.m_fFade = 0;
			}

			// Fade the health number back to dim
			a = MIN_ALPHA +  (gHUD.m_Health.m_fFade/FADE_TIME) * 128;
		}
		else
			a = MIN_ALPHA;

		gHUD.m_Health.GetPainColor( r, g, b );
		ScaleColors(r, g, b, a );

		// If health is getting low, make it bright red
		if (gHUD.m_Health.m_iHealth <= 15)
			a = 255;

		int iXSize,iYSize, iXPos, iYPos;
		m_pHealthTGA->getSize(iXSize,iYSize);
		m_pHealthTGA->getPos(iXPos, iYPos);

		// Paint the player's health
		int x = gHUD.DrawHudNumber( iXPos + iXSize + 5, iYPos + 5, DHN_3DIGITS | DHN_DRAWZERO, gHUD.m_Health.m_iHealth, r, g, b);

		// Draw the vertical line
		int HealthWidth = gHUD.GetSpriteRect(gHUD.m_HUD_number_0).right - gHUD.GetSpriteRect(gHUD.m_HUD_number_0).left;
		x += HealthWidth / 2;
		FillRGBA(x, iYPos + 5, HealthWidth / 10, gHUD.m_iFontHeight, 255, 160, 0, a);
	}
};

#endif
