#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_VotePanel.h"
#include "client_sound.h"

#include <stdio.h>

extern hud_player_info_t g_PlayerInfoList[MAX_PLAYERS + 1];

class CMenuHandler_VoteSelect : public ActionSignal
{
public:
	CMenuHandler_VoteSelect(int menu, CommandButton *button)
	{
		m_iMenu = menu;
		m_pButton = button;
	}

	virtual void actionPerformed(Panel *panel)
	{
		if (!gViewPort)
			return;

		if (m_iMenu == MENU_VOTE)
			gViewPort->m_pVotePanel->SetActiveButton(m_pButton);
		else if (m_iMenu == MENU_MAPVOTE)
			gViewPort->m_pMapVotePanel->SetActiveButton(m_pButton);
	}

private:
	int				m_iMenu;
	CommandButton	*m_pButton;
};

class CMenuHandler_VoteAction : public ActionSignal
{
public:
	CMenuHandler_VoteAction(const char *action, int menu)
	{
		m_pszAction = action;
		m_iMenu = menu;
	}

	virtual void actionPerformed(Panel *panel)
	{
		if (!gViewPort)
			return;

		if (m_iMenu == MENU_MAPVOTE)
			gViewPort->m_pMapVotePanel->FireAction(m_pszAction);
		else
			gViewPort->m_pVotePanel->FireAction(m_pszAction);
	}

private:
	const char	*m_pszAction;
	int			m_iMenu;
};

CVotePanel::CVotePanel(int iTrans, int iRemoveMe, int x, int y, int wide, int tall)
	: CMenuPanel(iTrans, iRemoveMe, x, y, wide, tall)
{
	float sx = (float)ScreenWidth / 640.0f;
	float sy = (float)ScreenHeight / 480.0f;

	m_pActiveButton = NULL;
	m_pActions = new CTransparentPanel(200, (int)(sx * 469), (int)(sy * 69), (int)(sx * 134), (int)(sy * 385));
	m_pActions->setParent(this);
	m_pActions->setBorder(new LineBorder(Color(115, 150, 180, 0)));
	m_pActions->setVisible(true);

	m_pClose = new CommandButton(CHudTextMessage::BufferedLocaliseTextString("#Menu_Cancel"),
		(int)(sx * 10), (int)(sy * 339), (int)(sx * 114), (int)(sy * 35));
	m_pClose->setParent(m_pActions);
	m_pClose->addActionSignal(new CMenuHandler_TextWindow(HIDE_TEXTWINDOW));
	m_pClose->setBorder(new LineBorder(Color(115, 150, 180, 0)));
	m_pClose->setVisible(true);

	m_pKick = new CommandButton(CHudTextMessage::BufferedLocaliseTextString("#Button_Votekick"),
		(int)(sx * 10), (int)(sy * 10), (int)(sx * 114), (int)(sy * 35));
	m_pKick->setParent(m_pActions);
	m_pKick->addActionSignal(new CMenuHandler_VoteAction("votekick", MENU_VOTE));
	m_pKick->setVisible(true);
	m_pKick->setBorder(new LineBorder(Color(115, 150, 180, 0)));

	m_pBan = new CommandButton(CHudTextMessage::BufferedLocaliseTextString("#Button_Voteban"),
		(int)(sx * 10), (int)(sy * 130), (int)(sx * 114), (int)(sy * 35));
	m_pBan->setParent(m_pActions);
	m_pBan->addActionSignal(new CMenuHandler_VoteAction("voteban", MENU_VOTE));
	m_pBan->setVisible(true);
	m_pBan->setBorder(new LineBorder(Color(115, 150, 180, 0)));

	m_pKill = new CommandButton(CHudTextMessage::BufferedLocaliseTextString("#Button_Votekill"),
		(int)(sx * 10), (int)(sy * 70), (int)(sx * 114), (int)(sy * 35));
	m_pKill->setParent(m_pActions);
	m_pKill->addActionSignal(new CMenuHandler_VoteAction("votekill", MENU_VOTE));
	m_pKill->setVisible(true);
	m_pKill->setBorder(new LineBorder(Color(115, 150, 180, 0)));

	m_pMap = new CommandButton(CHudTextMessage::BufferedLocaliseTextString("#Button_Votemap"),
		(int)(sx * 10), (int)(sy * 190), (int)(sx * 114), (int)(sy * 35));
	m_pMap->setParent(m_pActions);
	m_pMap->addActionSignal(new CMenuHandler_TextWindow(MENU_MAPVOTE, true));
	m_pMap->setVisible(true);
	m_pMap->setBorder(new LineBorder(Color(115, 150, 180, 0)));

	m_pTitle = new Label(CHudTextMessage::BufferedLocaliseTextString("#Votemenu_Topcaption"),
		(int)(sx * 25), (int)(sy * 18), (int)(sx * 579), (int)(sx * 35));
	m_pTitle->setBgColor(160, 160, 160, 80);
	m_pTitle->setFgColor(0, 0, 0, 0);
	m_pTitle->setBorder(new LineBorder(Color(115, 150, 180, 0)));
	m_pTitle->setContentAlignment(Label::a_west);
	m_pTitle->setParent(this);

	m_pSelection = new Label(CHudTextMessage::BufferedLocaliseTextString("#Votemenu_Topnoplayer"),
		(int)(sx * 25), (int)(sy * 18), (int)(sx * 579 - sx * 5), (int)(sx * 35));
	m_pSelection->setBgColor(0, 0, 0, 255);
	m_pSelection->setFgColor(0, 0, 75, 0);
	m_pSelection->setContentAlignment(Label::a_east);
	m_pSelection->setParent(this);

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		float buttonX = sx * 25;
		float buttonWide = sx * 212;
		float buttonTall = sy * 24;
		float buttonY = sy * 70 + (i / 2) * buttonTall;
		char text[128];

		if (i & 1)
			buttonX += buttonWide;

		snprintf(text, sizeof(text), "Player %d", i);
		m_pPlayers[i] = new CommandButton(text, (int)buttonX, (int)buttonY, (int)buttonWide, (int)buttonTall);
		m_pPlayers[i]->setParent(this);
		m_pPlayers[i]->addActionSignal(new CMenuHandler_VoteSelect(MENU_VOTE, m_pPlayers[i]));
		m_pPlayers[i]->setVisible(true);
		m_pPlayers[i]->setBorder(new LineBorder(Color(115, 150, 180, 0)));
		m_pPlayers[i]->setBgColor(160, 160, 160, 120);
		m_pPlayers[i]->setFgColor(0, 0, 255, 0);
	}
}

void CVotePanel::Open(void)
{
	Update();
}

void CVotePanel::UpdatePlayers(void)
{
	gViewPort->GetAllPlayersInfo();

	for (int i = 0; i < MAX_PLAYERS; i++)
		m_pPlayers[i]->setVisible(false);

	int button = 0;

	for (int player = 1; player <= MAX_PLAYERS; player++)
	{
		const char *name = g_PlayerInfoList[player].name;

		if (name && *name)
		{
			m_pPlayers[button]->setText(name);
			m_pPlayers[button]->setVisible(true);
			button++;
		}
	}
}

void CVotePanel::Update(void)
{
	UpdatePlayers();

	if (m_pActiveButton && m_pActiveButton->isVisible())
		m_pSelection->setText(m_pActiveButton->m_sMainText);
	else
		m_pSelection->setText(CHudTextMessage::BufferedLocaliseTextString("#Votemenu_Topnoplayer"));
}

void CVotePanel::SetActiveButton(CommandButton *button)
{
	m_pActiveButton = button;
	Update();
}

void CVotePanel::FireAction(const char *action)
{
	Update();

	if (!m_pActiveButton || !m_pActiveButton->isVisible() || !action)
		return;

	char command[128];

	snprintf(command, sizeof(command), "%s \"%s\"", action, m_pActiveButton->m_sMainText);
	gEngfuncs.pfnClientCmd(command);
	CClient_SoundEngine::GetSoundEngine()->Play2DSound("misc/talk.wav", 1);
	gViewPort->HideTopMenu();
}

CMapVotePanel::CMapVotePanel(int iTrans, int iRemoveMe, int x, int y, int wide, int tall)
	: CMenuPanel(iTrans, iRemoveMe, x, y, wide, tall)
{
	float sx = (float)ScreenWidth / 640.0f;
	float sy = (float)ScreenHeight / 480.0f;

	m_pActiveButton = NULL;
	m_bReceivingMaps = true;
	m_pActions = new CTransparentPanel(200, (int)(sx * 469), (int)(sy * 69), (int)(sx * 134), (int)(sy * 385));
	m_pActions->setParent(this);
	m_pActions->setBorder(new LineBorder(Color(115, 150, 180, 0)));
	m_pActions->setVisible(true);

	m_pClose = new CScrollButton(CHudTextMessage::BufferedLocaliseTextString("#Menu_Cancel"),
		(int)(sx * 10), (int)(sy * 339), (int)(sx * 114), (int)(sy * 35));
	m_pClose->setParent(m_pActions);
	m_pClose->addActionSignal(new CMenuHandler_TextWindow(HIDE_TEXTWINDOW));
	m_pClose->setBorder(new LineBorder(Color(115, 150, 180, 0)));
	m_pClose->setVisible(true);

	m_pKick = new CScrollButton(CHudTextMessage::BufferedLocaliseTextString("#Button_Votekick"),
		(int)(sx * 10), (int)(sy * 10), (int)(sx * 114), (int)(sy * 35));
	m_pKick->setParent(m_pActions);
	m_pKick->addActionSignal(new CMenuHandler_TextWindow(MENU_VOTE, true));
	m_pKick->setVisible(true);
	m_pKick->setBorder(new LineBorder(Color(115, 150, 180, 0)));

	m_pBan = new CScrollButton(CHudTextMessage::BufferedLocaliseTextString("#Button_Voteban"),
		(int)(sx * 10), (int)(sy * 130), (int)(sx * 114), (int)(sy * 35));
	m_pBan->setParent(m_pActions);
	m_pBan->addActionSignal(new CMenuHandler_TextWindow(MENU_VOTE, true));
	m_pBan->setVisible(true);
	m_pBan->setBorder(new LineBorder(Color(115, 150, 180, 0)));

	m_pKill = new CScrollButton(CHudTextMessage::BufferedLocaliseTextString("#Button_Votekill"),
		(int)(sx * 10), (int)(sy * 70), (int)(sx * 114), (int)(sy * 35));
	m_pKill->setParent(m_pActions);
	m_pKill->addActionSignal(new CMenuHandler_TextWindow(MENU_VOTE, true));
	m_pKill->setVisible(true);
	m_pKill->setBorder(new LineBorder(Color(115, 150, 180, 0)));

	m_pMap = new CScrollButton(CHudTextMessage::BufferedLocaliseTextString("#Button_Votemap"),
		(int)(sx * 10), (int)(sy * 190), (int)(sx * 114), (int)(sy * 35));
	m_pMap->setParent(m_pActions);
	m_pMap->addActionSignal(new CMenuHandler_VoteAction("votemap", MENU_MAPVOTE));
	m_pMap->setVisible(true);
	m_pMap->setBorder(new LineBorder(Color(115, 150, 180, 0)));

	CSchemeManager *schemes = gViewPort->GetSchemeManager();
	SchemeHandle_t titleScheme = schemes->getSchemeHandle("Title Font");
	int r, g, b, a;

	m_pTitle = new Label("", (int)(sx * 25), (int)(sy * 18));
	m_pTitle->setParent(this);
	m_pTitle->setFont(schemes->getFont(titleScheme));
	schemes->getFgColor(titleScheme, r, g, b, a);
	m_pTitle->setFgColor(r, g, b, a);
	schemes->getBgColor(titleScheme, r, g, b, a);
	m_pTitle->setBgColor(r, g, b, a);
	m_pTitle->setContentAlignment(Label::a_west);
	m_pTitle->setText(CHudTextMessage::BufferedLocaliseTextString("#Votemenu_Topcaption"));

	m_pSelection = new Label(CHudTextMessage::BufferedLocaliseTextString("#Votemenu_Topnoplayer"),
		(int)(sx * 25), (int)(sy * 18), (int)(sx * 579 - sx * 5), (int)(sx * 35));
	m_pSelection->setBgColor(0, 0, 0, 255);
	m_pSelection->setFgColor(0, 0, 75, 0);
	m_pSelection->setContentAlignment(Label::a_east);
	m_pSelection->setParent(this);

	m_pScrollPanel = new CTFScrollPanel((int)(sx * 25), (int)(sy * 69),
		(int)(sx * 212 + sx * 212), (int)(sy * 24 * 16));
	m_pScrollPanel->setBgColor(100, 50, 0, 100);
	m_pScrollPanel->setFgColor(0, 120, 166, 0);
	m_pScrollPanel->setBorder(new LineBorder(Color(115, 150, 180, 0)));
	m_pScrollPanel->setParent(this);
	m_pScrollPanel->setScrollBarAutoVisible(true, false);
	m_pScrollPanel->setScrollBarVisible(true, true);
	m_pScrollPanel->validate();
	m_iMaps = 0;
	m_pMaps = NULL;
	Update();
}

void CMapVotePanel::Reset(void)
{
	m_pActiveButton = NULL;
	m_bReceivingMaps = true;
}

void CMapVotePanel::Open(void)
{
	Update();
}

void CMapVotePanel::Update(void)
{
	if (m_pActiveButton && m_pActiveButton->isVisible())
		m_pSelection->setText(m_pActiveButton->m_sMainText);
	else
		m_pSelection->setText(CHudTextMessage::BufferedLocaliseTextString("#Votemenu_Topnomap"));

	m_pScrollPanel->setScrollValue(0, 0);
	m_pScrollPanel->validate();
}

void CMapVotePanel::SetActiveButton(CommandButton *button)
{
	m_pActiveButton = button;
	Update();
}

void CMapVotePanel::FireAction(const char *action)
{
	Update();

	if (!m_pActiveButton || !m_pActiveButton->isVisible() || !action)
		return;

	char command[128];

	snprintf(command, sizeof(command), "%s \"%s\"", action, m_pActiveButton->m_sMainText);
	gEngfuncs.pfnClientCmd(command);
	CClient_SoundEngine::GetSoundEngine()->Play2DSound("misc/talk.wav", 1);
	gViewPort->HideTopMenu();
}

int CMapVotePanel::MsgFunc_MapList(const char *pszName, int iSize, void *pbuf)
{
	if (!m_bReceivingMaps)
		return 1;

	BEGIN_READ(pbuf, iSize);

	int operation = READ_BYTE();
	int first = 0;
	int end;

	if (operation == MAPLIST_END)
	{
		m_bReceivingMaps = false;
		return 1;
	}

	if (operation != MAPLIST_RESET)
	{
		first = READ_SHORT();
		end = READ_SHORT();
	}
	else
	{
		for (int i = 0; i < m_iMaps; i++)
		{
			m_pMaps[i]->setParent(NULL);
			delete m_pMaps[i];
		}

		delete[] m_pMaps;
		m_pMaps = NULL;
		end = READ_SHORT();
		m_iMaps = end;
		m_pActiveButton = NULL;
	}

	if (m_iMaps < 0)
		m_iMaps = 0;

	if (operation == MAPLIST_RESET)
		m_pMaps = new CScrollButton *[m_iMaps];

	float sx = (float)ScreenWidth / 640.0f;
	float sy = (float)ScreenHeight / 480.0f;

	for (int i = first; i < end; i++)
	{
		float buttonWide = sx * 200 - m_pScrollPanel->getVerticalScrollBar()->getWide() / 2;
		float buttonTall = sy * 24;
		float buttonX = (i & 1) ? buttonWide : 0;
		float buttonY = (i / 2) * buttonTall;
		const char *name = READ_STRING();

		m_pMaps[i] = new CScrollButton(name, (int)buttonX, (int)buttonY, (int)buttonWide, (int)buttonTall);
		m_pMaps[i]->m_bNoBorder = true;
		m_pMaps[i]->setParent(m_pScrollPanel->getClient());
		m_pMaps[i]->addActionSignal(new CMenuHandler_VoteSelect(MENU_MAPVOTE, m_pMaps[i]));
		m_pMaps[i]->setVisible(true);
		m_pMaps[i]->setBgColor(160, 160, 160, 120);
		m_pMaps[i]->setFgColor(0, 0, 255, 0);
	}

	Update();
	return 1;
}

CVotePopup::CVotePopup(int iTrans, int iRemoveMe, int x, int y, int wide, int tall)
	: CMenuPanel(iTrans, iRemoveMe, x, y, wide, tall)
{
	CSchemeManager *schemes = gViewPort->GetSchemeManager();
	SchemeHandle_t titleScheme = schemes->getSchemeHandle("Title Font");
	SchemeHandle_t textScheme = schemes->getSchemeHandle("Briefing Text");
	float sx = (float)ScreenWidth / 640.0f;
	float sy = (float)ScreenHeight / 480.0f;
	int r, g, b, a;

	m_pBackground = new CTransparentPanel(200, (int)(sx * 128), (int)(sy * 256), (int)(sx * 384), (int)(sy * 120));
	m_pBackground->setBorder(new LineBorder(1));
	m_pBackground->setParent(this);

	m_pYes = new CommandButton(CHudTextMessage::BufferedLocaliseTextString("#Menu_Yes"),
		(int)(sx * 11), (int)(sy * 60), (int)(sx * 114), (int)(sy * 35));
	m_pYes->setParent(m_pBackground);
	m_pYes->addActionSignal(new CMenuHandler_StringCommand("voteyes"));
	m_pYes->addActionSignal(new CMenuHandler_TextWindow(HIDE_TEXTWINDOW));
	m_pYes->setBorder(new LineBorder(Color(115, 150, 180, 0)));
	m_pYes->setBoundKey('1');
	m_pYes->setVisible(true);

	m_pNo = new CommandButton(CHudTextMessage::BufferedLocaliseTextString("#Menu_No"),
		(int)(sx * 11 + sx * 114 + sx * 10), (int)(sy * 60), (int)(sx * 114), (int)(sy * 35));
	m_pNo->setParent(m_pBackground);
	m_pNo->addActionSignal(new CMenuHandler_StringCommand("voteno"));
	m_pNo->addActionSignal(new CMenuHandler_TextWindow(HIDE_TEXTWINDOW));
	m_pNo->setBoundKey('2');
	m_pNo->setBorder(new LineBorder(Color(115, 150, 180, 0)));
	m_pNo->setVisible(true);

	m_pTitle = new Label(CHudTextMessage::BufferedLocaliseTextString("#VotePopup_Title"), (int)(sx * 11), (int)(sy * 35));
	m_pTitle->setParent(m_pBackground);
	m_pTitle->setBgColor(160, 160, 160, 255);
	schemes->getFgColor(titleScheme, r, g, b, a);
	m_pTitle->setFgColor(r, g, b, a);
	m_pTitle->setContentAlignment(Label::a_west);
	m_pTitle->setVisible(true);
	m_pTitle->setFont(schemes->getFont(titleScheme));
	m_pTitle->setFont(Scheme::sf_primary2);

	m_pText = new Label("Text here ", (int)(sx * 11), (int)(sy * 25));
	m_pText->setParent(m_pBackground);
	m_pText->setBgColor(160, 160, 160, 255);
	schemes->getFgColor(textScheme, r, g, b, a);
	m_pText->setFgColor(r, g, b, a);
	m_pText->setContentAlignment(Label::a_west);
	m_pText->setVisible(true);
	m_pText->setFont(schemes->getFont(textScheme));
	m_pText->setFont(Scheme::sf_primary2);
}

void CVotePopup::Open(void)
{
	Update();
}

void CVotePopup::Update(void)
{
}

bool CVotePopup::SlotInput(int slot)
{
	gEngfuncs.pfnClientCmd(slot == 0 ? "voteyes" : "voteno");
	gViewPort->HideTopMenu();
	return true;
}

int CVotePopup::MsgFunc_VoteMenu(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	m_iVoteType = READ_BYTE();
	m_pText->setText(READ_STRING());

	const char *yes = READ_STRING();

	m_pYes->setText(*yes ? yes : CHudTextMessage::BufferedLocaliseTextString("#Menu_Yes"));

	const char *no = READ_STRING();

	m_pNo->setText(*no ? no : CHudTextMessage::BufferedLocaliseTextString("#Menu_No"));
	Update();
	return 1;
}
