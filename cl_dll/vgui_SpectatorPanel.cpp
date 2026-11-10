// vgui_SpectatorPanel.cpp: implementation of the SpectatorPanel class.
//
//////////////////////////////////////////////////////////////////////

#include "hud.h"
#include "cl_util.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_SpectatorPanel.h"
#include "vgui_ScorePanel.h"

#include "Exports.h"

extern void CAM_ToFirstPerson(void);

/*
==========================
HUD_ChatInputPosition

Sets the location of the input for chat text
==========================
*/
void DLLEXPORT HUD_ChatInputPosition( int *x, int *y )
{
	gHUD.m_SayText.GetChatInputPosition(x, y);
		}

SpectatorPanel::SpectatorPanel(int x,int y,int wide,int tall) : Panel(x,y,wide,tall)
{
}

SpectatorPanel::~SpectatorPanel()
{

}

void SpectatorPanel::Initialize()
{
	int x,y,wide,tall;

	getBounds(x,y,wide,tall);
	gViewPort->GetSchemeManager()->getSchemeHandle( "Team Info Text" );

	m_TopBorder = new CTransparentPanel(64, 0, 0, ScreenWidth, YRES(32));
	m_TopBorder->setParent(this);

	m_BottomBorder = new CTransparentPanel(64, 0, ScreenHeight - YRES(32), ScreenWidth, YRES(32));
	m_BottomBorder->setParent(this);

	setPaintBackgroundEnabled(false);

	m_Health = new Label("Health: 100", XRES(15), YRES(6), XRES(72), YRES(20));
	m_Health->setParent( m_BottomBorder );
	m_Health->setPaintBackgroundEnabled(false);
	m_Health->setFgColor( Scheme::sc_primary1 );
	m_Health->setContentAlignment(Label::a_center );
	m_Health->setBorder( new LineBorder( Color( 59, 58, 34, 48 ) ) );

	m_Armor = new Label("Armor: 100", XRES(15) + m_Health->getWide(), YRES(6), XRES(72), YRES(20) );
	m_Armor->setParent(m_BottomBorder);
	m_Armor->setPaintBackgroundEnabled(false);
	m_Armor->setFgColor(Scheme::sc_primary1);
	m_Armor->setContentAlignment(Label::a_center);
	m_Armor->setBorder(new LineBorder(Color( 59, 58, 34, 48 )));

	float halfWidth = ScreenWidth / 2;
	float sideWidth = XRES(15) + m_Health->getWide() + m_Armor->getWide();
	float flLabelSize = (halfWidth - sideWidth) + (halfWidth - sideWidth);


	m_BottomMainLabel = new Label("Spectator Bottom",
		(ScreenWidth - flLabelSize) * 0.5f, YRES(6), flLabelSize, YRES(20));

	m_BottomMainLabel->setParent(m_BottomBorder);
	m_BottomMainLabel->setPaintBackgroundEnabled(false);
	m_BottomMainLabel->setFgColor( Scheme::sc_primary1 );
	m_BottomMainLabel->setContentAlignment(Label::a_center );
	m_BottomMainLabel->setBorder(new LineBorder(Color( 59, 58, 34, 48 )));

}

void SpectatorPanel::Update()
{
	if (!gViewPort->GetObserverMode())
	{
		setVisible(false);
		return;
	}

	CAM_ToFirstPerson();
	setVisible(true);

	const char *mode;

	switch (gViewPort->GetObserverMode())
	{
	case 1:
		mode = "#Spec_Mode2";
		break;
	case 2:
		mode = "#Spec_Mode3";
		break;
	case 3:
		mode = "#Spec_Mode1";
		break;
	case 4:
		mode = "#Spec_Mode4";
		break;
	default:
		mode = "";
		break;
	}

	int target = gViewPort->GetObserverTarget();
	char iTimeHeight[193] = {0};

	if (target < 1 || target > MAX_PLAYERS)
	{
		m_Health->setVisible(false);
		m_Armor->setVisible(false);
		UTIL_FormatString(iTimeHeight, "%s", mode);
		m_BottomMainLabel->setText(CHudTextMessage::BufferedLocaliseTextString(iTimeHeight));
		return;
	}

	m_Health->setVisible(true);

	if (g_PlayerExtraInfo[target].health > 0)
		UTIL_FormatString(iTimeHeight, "Health: %d", (int)g_PlayerExtraInfo[target].health);
	else
		safe_strcpy(iTimeHeight, "Dead", sizeof(iTimeHeight));

	m_Health->setText(CHudTextMessage::BufferedLocaliseTextString(iTimeHeight) );

	if (g_PlayerExtraInfo[target].health > 0)
	{
		UTIL_FormatString(iTimeHeight, "Armor: %d", (int)g_PlayerExtraInfo[target].armor);
		m_Armor->setVisible(true);
	}
	else
	{
		memset(iTimeHeight, 0, sizeof(iTimeHeight) );
		m_Armor->setVisible(false);
	}

	m_Armor->setText(CHudTextMessage::BufferedLocaliseTextString(iTimeHeight) );

	UTIL_FormatString(iTimeHeight, "%s : %s", mode, g_PlayerInfoList[target].name);
	m_BottomMainLabel->setText(CHudTextMessage::BufferedLocaliseTextString(iTimeHeight) );
}
