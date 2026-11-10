#include "vgui_int.h"
#include "VGUI_Font.h"

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_Inventory.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

class CMenuHandler_InventorySelect : public ActionSignal
{
public:
	CMenuHandler_InventorySelect(clientinventory_s *item, CommandButton *button)
	{
		m_pItem = item;
		m_pButton = button;
	}

	virtual void actionPerformed(Panel *panel)
	{
		if (gViewPort && gViewPort->GetInventoryPanel())
			gViewPort->GetInventoryPanel()->SetActiveItem(m_pItem, m_pButton);
	}

private:
	clientinventory_s	*m_pItem;
	CommandButton		*m_pButton;
};

class CMenuHandler_InventoryActivate : public ActionSignal
{
public:
	virtual void actionPerformed(Panel *panel)
	{
		if (gViewPort && gViewPort->GetInventoryPanel())
			gViewPort->GetInventoryPanel()->FireAction("activate");
	}
};

class CMenuHandler_InventoryDrop : public ActionSignal
{
public:
	virtual void actionPerformed(Panel *panel)
	{
		if (gViewPort && gViewPort->GetInventoryPanel())
			gViewPort->GetInventoryPanel()->FireAction("drop");
	}
};

class CMenuHandler_InventoryDropAll : public ActionSignal
{
public:
	virtual void actionPerformed(Panel *panel)
	{
		if (gViewPort && gViewPort->GetInventoryPanel())
			gViewPort->GetInventoryPanel()->FireAction("dropall");
	}
};

CScrollButton::CScrollButton(const char *text, int x, int y, int wide, int tall, bool noHighlight)
	: CommandButton(text, x, y, wide, tall, noHighlight)
{
	m_pszText = NULL;
	m_pUserData = NULL;
	m_bKeepArmed = false;
	m_Unknown340 = 0;
	m_pActionSignal = NULL;
	setText(text);
}

CScrollButton::~CScrollButton()
{
	operator delete(m_pUserData);
	m_pUserData = NULL;
}

void CScrollButton::setText(const char *text)
{
	operator delete(m_pszText);

	if (text)
	{
		m_pszText = (char *)operator new[](strlen(text) + 2);
		strcpy(m_pszText, text);
	}

	CommandButton::setText(text);
}

void CScrollButton::cursorExited(Panel *panel)
{
	if (!m_bKeepArmed)
		CommandButton::cursorExited();
}

CInventoryPanel::CInventoryPanel(int iTrans, int iRemoveMe, int x, int y, int wide, int tall)
	: CMenuPanel(iTrans, iRemoveMe, x, y, wide, tall)
{
	CSchemeManager *schemes = gViewPort->GetSchemeManager();
	SchemeHandle_t titleScheme = schemes->getSchemeHandle("Title Font");
	float sx = (float)ScreenWidth / 640.0f;
	float sy = (float)ScreenHeight / 480.0f;
	int r, g, b, a;

	schemes->getSchemeHandle("Briefing Text");
	m_bReady = true;
	m_iItems = 0;
	m_pItems = NULL;
	m_pActiveItem = NULL;
	m_pActiveButton = NULL;
	m_pButtons = NULL;

	m_pTitle = new Label(CHudTextMessage::BufferedLocaliseTextString("#Invmenu_Topcaption"),
		(int)(sx * 25), (int)(sy * 18), (int)(sx * 579), (int)(sx * 35));
	m_pTitle->setParent(this);
	m_pTitle->setFont(schemes->getFont(titleScheme));
	schemes->getFgColor(titleScheme, r, g, b, a);
	m_pTitle->setFgColor(r, g, b, a);
	schemes->getBgColor(titleScheme, r, g, b, a);
	m_pTitle->setBgColor(r, g, b, a);
	m_pTitle->setBorder(new LineBorder(Color(115, 150, 180, 0)));
	m_pTitle->setContentAlignment(Label::a_northwest);

	m_pClose = new CommandButton("X", (int)(sx * 560), (int)sy, (int)(sx * 16), (int)(sx * 16));
	m_pClose->setParent(m_pTitle);
	m_pClose->addActionSignal(new CMenuHandler_TextWindow(HIDE_TEXTWINDOW));
	m_pClose->setFgColor(255, 0, 0, 0);
	m_pClose->setBorder(new LineBorder(Color(115, 150, 180, 0)));
	m_pClose->setTextAlignment(Label::a_center);
	m_pClose->setContentAlignment(Label::a_center);
	m_pClose->setNoKeyPrefix(true);
	m_pClose->setVisible(true);

	m_pScrollPanel = new CTFScrollPanel((int)(sx * 25), (int)(sy * 69), (int)(sx * 579), (int)(sy * 285));
	m_pScrollPanel->setParent(this);
	m_pScrollPanel->setBorder(new LineBorder(Color(115, 150, 180, 0)));
	m_pScrollPanel->setBgColor(r, g, b, a);
	m_pScrollPanel->setScrollBarAutoVisible(false, true);
	m_pScrollPanel->setScrollBarVisible(false, true);
	m_pScrollPanel->setVisible(true);

	m_pEmptyLabel = new Label(CHudTextMessage::BufferedLocaliseTextString("#Invmenu_NoInventory"),
		(int)sx, (int)(sy * 2), (int)(sx * 579 - sx * 2), (int)(sy * 285 - sy * 2));
	m_pEmptyLabel->setParent(m_pScrollPanel);
	m_pEmptyLabel->setBorder(NULL);
	m_pEmptyLabel->setVisible(true);
	m_pEmptyLabel->setBgColor(0, 0, 0, 255);
	m_pEmptyLabel->setFgColor(255, 63, 63, 40);
	m_pEmptyLabel->setTextAlignment(Label::a_center);

	m_pDetails = new CTransparentPanel(200, (int)(sx * 25), (int)(sy * 375), (int)(sx * 579), (int)(sy * 64));
	m_pDetails->setParent(this);
	m_pDetails->setBorder(new LineBorder(Color(115, 150, 180, 0)));
	m_pDetails->setBgColor(r, g, b, a);
	m_pDetails->setVisible(true);

	m_pName = new Label(CHudTextMessage::BufferedLocaliseTextString("#Invmenu_NoItemSelected"),
		(int)(sx * 2), (int)(sy * 2), (int)(sx * 575 - sx * 4), (int)(sy * 13));
	m_pName->setParent(m_pDetails);
	m_pName->setBorder(NULL);
	m_pName->setBgColor(0, 0, 0, 255);
	m_pName->setFgColor(255, 63, 63, 80);
	m_pName->setTextAlignment(Label::a_center);
	m_pName->setVisible(true);

	m_pDescription = new Label("", (int)(sx * 2), (int)(sy * 13 + sy * 2 + sy * 2),
		(int)(sx * 575 - sx * 4), (int)(sy * 13 + sy * 13));
	m_pDescription->setParent(m_pDetails);
	m_pDescription->setBorder(NULL);
	m_pDescription->setBgColor(0, 0, 0, 255);
	m_pDescription->setFgColor(223, 31, 31, 80);
	m_pDescription->setTextAlignment(Label::a_center);
	m_pDescription->setVisible(true);

	float buttonY = sy * 13 + sy * 2 + sy * 2 + (sy * 13 + sy * 13) + sy * 2;

	m_pActivate = new CommandButton(CHudTextMessage::BufferedLocaliseTextString("#Invmenu_Activate"),
		(int)(sx * 579 * 0.5f - sx * 80 * 0.5f), (int)buttonY, (int)(sx * 80), (int)(sy * 16));
	m_pActivate->setParent(m_pDetails);
	m_pActivate->addActionSignal(new CMenuHandler_InventoryActivate());
	m_pActivate->setBorder(new LineBorder(Color(115, 150, 180, 0)));
	m_pActivate->setFgColor(159, 159, 159, 80);
	m_pActivate->setTextAlignment(Label::a_center);
	m_pActivate->setContentAlignment(Label::a_center);
	m_pActivate->setNoKeyPrefix(true);
	m_pActivate->setEnabled(false);
	m_pActivate->setVisible(true);

	m_pDrop = new CommandButton(CHudTextMessage::BufferedLocaliseTextString("#Invmenu_Drop"),
		(int)(sx * 2), (int)buttonY, (int)(sx * 80), (int)(sy * 16));
	m_pDrop->setParent(m_pDetails);
	m_pDrop->addActionSignal(new CMenuHandler_InventoryDrop());
	m_pDrop->setBorder(new LineBorder(Color(115, 150, 180, 0)));
	m_pDrop->setFgColor(159, 159, 159, 80);
	m_pDrop->setTextAlignment(Label::a_center);
	m_pDrop->setContentAlignment(Label::a_center);
	m_pDrop->setNoKeyPrefix(true);
	m_pDrop->setEnabled(false);
	m_pDrop->setVisible(true);

	m_pDropAll = new CommandButton(CHudTextMessage::BufferedLocaliseTextString("#Invmenu_DropAll"),
		(int)(sx * 579 - sx * 80 * 2.4 - sx * 2 + sx * 80 + sx * 2),
		(int)(sy * 13 + sy * 13 + sy * 2 + sy * 2 + sy * 13 + sy * 2),
		(int)(sx * 80 * 1.33), (int)(sy * 16));
	m_pDropAll->setParent(m_pDetails);
	m_pDropAll->addActionSignal(new CMenuHandler_InventoryDropAll());
	m_pDropAll->setBorder(new LineBorder(Color(115, 150, 180, 0)));
	m_pDropAll->setFgColor(255, 63, 63, 80);
	m_pDropAll->setTextAlignment(Label::a_center);
	m_pDropAll->setContentAlignment(Label::a_center);
	m_pDropAll->setNoKeyPrefix(true);
	m_pDropAll->setEnabled(false);
	m_pDropAll->setVisible(true);
}

void FreeInventoryItem(clientinventory_s *item)
{
	delete[] item->name;
	delete[] item->displayName;
	delete[] item->description;
	delete[] item->target;
	delete[] item->icon;
	delete item;
}

CInventoryPanel::~CInventoryPanel()
{
	while (m_pItems)
	{
		clientinventory_s *item = m_pItems;

		m_pItems = item->next;
		FreeInventoryItem(item);
	}

	for (int i = 0; i < m_iItems; i++)
	{
		m_pButtons[i]->setParent(NULL);
		delete m_pButtons[i];
	}

	free(m_pButtons);
}

bool CInventoryPanel::HasItem(const clientinventory_s *item) const
{
	for (clientinventory_s *current = m_pItems; current; current = current->next)
	{
		if (current == item)
			return true;
	}

	return false;
}

void CInventoryPanel::Open(void)
{
	RebuildGrid();
	Update();
	setVisible(true);
}

void CInventoryPanel::Reset(void)
{
	m_pActiveItem = NULL;
	m_pActiveButton = NULL;
	RebuildGrid();
}

void CInventoryPanel::SetActiveItem(clientinventory_s *item, CommandButton *button)
{
	if (item && !HasItem(item))
	{
		item = NULL;
		button = NULL;
	}

	m_pActiveItem = item;
	m_pActiveButton = button;
	Update();
}

void CInventoryPanel::Update(void)
{
	for (int i = 0; i < m_iItems; i++)
	{
		CommandButton *button = m_pButtons[i];

		if (button && button != m_pActiveButton)
		{
			button->setBorder(new LineBorder(Color(0, 0, 128, 0)));
			button->setBgColor(159, 159, 159, 120);
			button->setFgColor(0, 0, 255, 0);
			button->repaint();
		}
	}

	m_pDrop->setBorder(new LineBorder(Color(115, 150, 180, 0)));

	if (m_pActiveButton && m_pActiveButton->isVisible())
	{
		m_pActiveButton->setBorder(new LineBorder(Color(255, 255, 0, 0)));
		m_pActiveButton->setBgColor(255, 255, 63, 31);
		m_pActiveButton->setFgColor(255, 0, 0, 0);
		m_pActiveButton->repaint();

		if (m_pActiveItem->displayName && *m_pActiveItem->displayName)
		{
			m_pName->setText("%s", m_pActiveItem->displayName);
			m_pName->setFgColor(159, 223, 223, 80);
		}
		else if (m_pActiveItem->name && *m_pActiveItem->name)
		{
			m_pName->setText("%s", m_pActiveItem->name);
			m_pName->setFgColor(127, 159, 159, 80);
		}
		else
		{
			m_pName->setText("%s", CHudTextMessage::BufferedLocaliseTextString("#Invmenu_NoItemName"));
			m_pName->setFgColor(255, 63, 63, 80);
		}

		if (m_pActiveItem->description && *m_pActiveItem->description)
		{
			m_pDescription->setText("%s", m_pActiveItem->description);
			m_pDescription->setFgColor(95, 127, 127, 80);
		}
		else
		{
			m_pDescription->setText("%s", CHudTextMessage::BufferedLocaliseTextString("#Invmenu_NoItemDescription"));
			m_pDescription->setFgColor(223, 31, 31, 80);
		}

		m_pActivate->setEnabled(true);
		m_pDrop->setEnabled(true);

		if (!m_pActiveItem->canActivate)
			m_pActivate->setBorder(new LineBorder(Color(255, 0, 0, 0)));

		if (!m_pActiveItem->canDrop)
			m_pDrop->setBorder(new LineBorder(Color(255, 0, 0, 0)));
	}
	else
	{
		m_pName->setText("%s", CHudTextMessage::BufferedLocaliseTextString("#Invmenu_NoItemSelected"));
		m_pName->setFgColor(255, 63, 63, 80);
		m_pDescription->setText("");
		m_pDescription->setFgColor(223, 31, 31, 80);
		m_pActivate->setEnabled(false);
		m_pDrop->setEnabled(false);
	}

	m_pScrollPanel->setScrollValue(0, 0);
	m_pScrollPanel->validate();
	repaintAll();
	gHUD.m_Inventory.Visible(m_iItems > 0);
	gHUD.m_Inventory.SetCount(m_iItems);
}

void CInventoryPanel::RebuildGrid(void)
{
	if (!m_bReady)
		return;

	m_bReady = false;

	for (int i = 0; i < m_iItems; i++)
	{
		m_pButtons[i]->setParent(NULL);
		delete m_pButtons[i];
	}

	free(m_pButtons);
	m_pButtons = NULL;
	m_iItems = 0;

	for (clientinventory_s *item = m_pItems; item; item = item->next)
		m_iItems++;

	const char *icon = NULL;

	if (m_iItems > 0)
	{
		CSchemeManager *schemes = gViewPort->GetSchemeManager();
		SchemeHandle_t textScheme = schemes->getSchemeHandle("Briefing Text");
		float sx = (float)ScreenWidth / 640.0f;
		float sy = (float)ScreenHeight / 480.0f;
		float size = V_min(sx * 64.0f, sy * 64.0f);
		float gap = V_min(sx * 4.0f, sy * 4.0f);
		int i = 0;

		icon = m_pItems->icon;
		m_pButtons = (CScrollButton **)malloc(sizeof(*m_pButtons) * m_iItems);

		for (clientinventory_s *item = m_pItems; item && i < m_iItems; item = item->next, ++i)
		{
			int x = (int)(gap * 0.5f);
			int y = x;

			if (i > 0)
			{
				int wide, tall;

				m_pButtons[i - 1]->getPos(x, y);
				x = (int)((size + size) + gap + (float)x);
				m_pScrollPanel->getSize(wide, tall);

				if ((size + size) + (float)x + gap * 0.5f > (float)wide)
				{
					x = (int)(gap * 0.5f);
					y = (int)(size * 0.5f + gap + (float)y);
				}
			}

			CScrollButton *button = new CScrollButton("Unknown item", x, y, (int)(size + size), (int)(size * 0.5f));

			m_pButtons[i] = button;
			button->setParent(m_pScrollPanel->getClient());
			button->addActionSignal(new CMenuHandler_InventorySelect(item, button));
			button->setBorder(new LineBorder(Color(0, 0, 128, 0)));
			button->setFont(schemes->getFont(textScheme));
			button->setBgColor(159, 159, 159, 120);
			button->setFgColor(0, 0, 255, 0);
			button->setTextAlignment(Label::a_center);
			button->setContentAlignment(Label::a_center);
			button->setNoKeyPrefix(true);
			button->setVisible(true);

			char text[32] = { 0 };

			if (item->displayName && *item->displayName)
				strncpy(text, item->displayName, sizeof(text));
			else if (item->name && *item->name)
				strncpy(text, item->name, sizeof(text));
			else
				UTIL_FormatString(text, "%d", item->id);

			text[sizeof(text) - 1] = 0;

			if (icon && *icon && stricmp(m_pItems->icon, item->icon))
				icon = NULL;

			button->setText(text);
			button->repaint();

			if (m_pActiveItem == item)
				m_pActiveButton = button;

			button->repaint();
		}

		m_pEmptyLabel->setVisible(false);
		m_pDropAll->setEnabled(true);
	}
	else
	{
		m_pEmptyLabel->setVisible(true);
		m_pDropAll->setEnabled(false);
		m_pActiveItem = NULL;
		m_pActiveButton = NULL;
	}

	gHUD.m_Inventory.SetIcon(icon);
	gHUD.m_Inventory.Visible(m_iItems > 0);
	m_bReady = true;
}

void CInventoryPanel::FireAction(const char *action)
{
	char command[256] = { 0 };

	Update();

	if (!stricmp(action, "activate") || !stricmp(action, "drop"))
	{
		if (!m_pActiveItem || !m_pActiveItem->id)
		{
			gViewPort->HideTopMenu();
			return;
		}

		UTIL_FormatString(command, "%sitem \"#%d\"", action, m_pActiveItem->id);
	}
	else if (!stricmp(action, "dropall"))
	{
		strcpy(command, "dropitem \"*\"");
	}

	Reset();

	if (*command)
	{
		gEngfuncs.pfnClientCmd(command);
		gEngfuncs.pfnPlaySoundByName("misc/talk.wav", 1);
	}

	gViewPort->HideTopMenu();
}

int CInventoryPanel::MsgFunc_InvAdd(const char *pszName, int iSize, void *pbuf)
{
	if (!m_bReady)
		return 0;

	clientinventory_s *item = new clientinventory_s;

	memset(item, 0, sizeof(*item));
	BEGIN_READ(pbuf, iSize);
	item->id = READ_LONG();
	item->canActivate = READ_BYTE() != 0;
	item->canDrop = READ_BYTE() != 0;
	item->keepOnClear = READ_BYTE() != 0;
	item->weight = READ_FLOAT();
	item->name = READ_ALLOC_STRING();
	item->displayName = READ_ALLOC_STRING();
	item->description = READ_ALLOC_STRING();
	item->target = READ_ALLOC_STRING();
	item->icon = READ_ALLOC_STRING();

	clientinventory_s **tail = &m_pItems;

	while (*tail)
		tail = &(*tail)->next;

	*tail = item;
	RebuildGrid();
	Update();
	return 1;
}

int CInventoryPanel::MsgFunc_InvRemove(const char *pszName, int iSize, void *pbuf)
{
	if (!m_bReady)
		return 1;

	BEGIN_READ(pbuf, iSize);

	int id = READ_LONG();
	int preserve = READ_BYTE();
	clientinventory_s **link = &m_pItems;

	while (*link)
	{
		clientinventory_s *item = *link;

		if ((id && id != item->id) || (item->keepOnClear && preserve))
		{
			link = &item->next;
			continue;
		}

		if (m_pActiveItem == item)
		{
			m_pActiveItem = NULL;
			m_pActiveButton = NULL;
		}

		*link = item->next;
		FreeInventoryItem(item);
	}

	RebuildGrid();
	Update();
	return 1;
}
