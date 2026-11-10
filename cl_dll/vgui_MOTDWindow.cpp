//=========== (C) Copyright 1996-2002 Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================

#include "VGUI_Font.h"
#include "VGUI_TextImage.h"

#include "hud.h"
#include "cl_util.h"
#include "vgui_MOTDWindow.h"

#define MOTD_TITLE_X				XRES(16)
#define MOTD_TITLE_Y				YRES(16)

#define MOTD_WINDOW_X			XRES(112)
#define MOTD_WINDOW_Y			YRES(80)
#define MOTD_WINDOW_SIZE_X		XRES(424)
#define MOTD_WINDOW_SIZE_Y		YRES(312)

//-----------------------------------------------------------------------------
// Purpose: Constructs a message panel
//-----------------------------------------------------------------------------
CMessageWindowPanel::CMessageWindowPanel(int iRemoveMe, int x, int y, int wide, int tall, int iTextID) : CMenuPanel(100, iRemoveMe, x, y, wide, tall )
{
	// Get the scheme used for the Titles
	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();

	// schemes
	SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle( "Title Font" );
	SchemeHandle_t hMOTDText = pSchemes->getSchemeHandle( "Briefing Text" );

	// color schemes
	int r, g, b, a;

	// Create the window
	m_pBackgroundPanel = new CTransparentPanel(255, MOTD_WINDOW_X, MOTD_WINDOW_Y, MOTD_WINDOW_SIZE_X, MOTD_WINDOW_SIZE_Y );
	m_pBackgroundPanel->setParent( this );
	m_pBackgroundPanel->setBorder( new LineBorder( Color(115, 150, 180,0)) );
	m_pBackgroundPanel->setVisible( true );

	int iXSize,iYSize,iXPos,iYPos;
	m_pBackgroundPanel->getPos( iXPos,iYPos );
	m_pBackgroundPanel->getSize( iXSize,iYSize );

	m_pTitle = new Label( "", iXPos + MOTD_TITLE_X, iYPos + MOTD_TITLE_Y );
	m_pTitle->setParent( this );
	m_pTitle->setFont( pSchemes->getFont(hTitleScheme) );
	m_pTitle->setFont( Scheme::sf_primary1 );

	pSchemes->getFgColor( hTitleScheme, r, g, b, a );
	m_pTitle->setFgColor( r, g, b, a );
	m_pTitle->setFgColor( Scheme::sc_primary1 );
	pSchemes->getBgColor( hTitleScheme, r, g, b, a );
	m_pTitle->setBgColor( r, g, b, a );
	m_pTitle->setContentAlignment(Label::a_west );

	m_pScrollPanel = new CTFScrollPanel( iXPos + XRES(16), iYPos + MOTD_TITLE_Y*2 + YRES(16), iXSize - XRES(32), iYSize - (YRES(48) + BUTTON_SIZE_Y*2) );
	m_pScrollPanel->setParent(this);
	m_pScrollPanel->setScrollBarAutoVisible(false, false);
	m_pScrollPanel->setScrollBarVisible(true, true);
	m_pScrollPanel->validate();

	m_pText = new TextPanel( "", 0,0, 64,64);
	m_pText->setParent(m_pScrollPanel->getClient() );
	m_pText->setFont( pSchemes->getFont(hMOTDText) );
	pSchemes->getFgColor( hMOTDText, r, g, b, a );
	m_pText->setFgColor( r, g, b, a );
	pSchemes->getBgColor( hMOTDText, r, g, b, a );
	m_pText->setBgColor( r, g, b, a);

	CommandButton *pButton = new CommandButton( CHudTextMessage::BufferedLocaliseTextString( "#Menu_OK" ), iXPos + XRES(16), iYPos + iYSize - YRES(16) - BUTTON_SIZE_Y, CMENU_SIZE_X, BUTTON_SIZE_Y);
	pButton->addActionSignal(new CMenuHandler_TextWindow(HIDE_TEXTWINDOW));
	pButton->setParent(this);

	if (iTextID == 1)
	{
		pButton = new CommandButton(CHudTextMessage::BufferedLocaliseTextString("#Textwindow_Button_Mission_Briefing"),
			iXPos + XRES(16) + CMENU_SIZE_X, iYPos + iYSize - YRES(16) - BUTTON_SIZE_Y, CMENU_SIZE_X, BUTTON_SIZE_Y);
		pButton->addActionSignal(new CMenuHandler_TextWindow(MENU_SWITCH_TO_MAPBRIEFING));
		pButton->setParent(this);
	}
	else if (iTextID == 0)
	{
		pButton = new CommandButton(CHudTextMessage::BufferedLocaliseTextString("#Textwindow_Button_MOTD"),
			iXPos + XRES(16) + CMENU_SIZE_X, iYPos + iYSize - YRES(16) - BUTTON_SIZE_Y, CMENU_SIZE_X, BUTTON_SIZE_Y);
		pButton->addActionSignal(new CMenuHandler_TextWindow(MENU_SWITCH_TO_INTRO));
		pButton->setParent(this);
	}
}

void CMessageWindowPanel::Initialize(const char *szText, const char *szTitle, bool bShadeFullscreen)
{
	m_BackgroundColor = Color(0, 0, 0, bShadeFullscreen ? 155 : 0);
	m_pBackgroundPanel->m_BackgroundColor = Color(0, 0, 0, bShadeFullscreen ? 0 : 155);
	m_pTitle->setText(szTitle);
	m_pText->setText(szText);
	m_pText->setSize(m_pScrollPanel->getClientClip()->getWide() - 2, 5000);

	int iScrollSizeX, iScrollSizeY;

	m_pText->getTextImage()->getTextSizeWrapped(iScrollSizeX, iScrollSizeY);
	m_pText->setSize(iScrollSizeX, iScrollSizeY);
	m_pScrollPanel->setScrollBarAutoVisible(true, true);
	m_pScrollPanel->setScrollBarVisible(false, false);
	m_pScrollPanel->validate();
}
