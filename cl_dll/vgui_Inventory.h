#ifndef VGUI_INVENTORY_H
#define VGUI_INVENTORY_H

#include "vgui_TeamFortressViewport.h"

struct clientinventory_s
{
	int					id;
	bool				canActivate;
	bool				canDrop;
	bool				keepOnClear;
	float				weight;
	char				*name;
	char				*displayName;
	char				*description;
	char				*target;
	char				*icon;
	clientinventory_s	*next;
};

class CScrollButton : public CommandButton
{
public:
	CScrollButton(const char *text, int x, int y, int wide, int tall, bool noHighlight = false);
	~CScrollButton();

	virtual void setText(const char *text);
	virtual void cursorExited(Panel *panel);

private:
	friend class CMediaSelect;

	char			*m_pszText;
	void			*m_pUserData;
	bool			m_bKeepArmed;
	bool			m_bInUse;
	unsigned int	m_Unknown340;
	ActionSignal	*m_pActionSignal;
};

class CInventoryPanel : public CMenuPanel
{
public:
	CInventoryPanel(int iTrans, int iRemoveMe, int x, int y, int wide, int tall);
	~CInventoryPanel();

	virtual void Open(void);
	virtual void Reset(void);
	void Update(void);
	void RebuildGrid(void);
	bool HasItem(const clientinventory_s *item) const;
	void SetActiveItem(clientinventory_s *item, CommandButton *button);
	void FireAction(const char *action);
	int MsgFunc_InvAdd(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_InvRemove(const char *pszName, int iSize, void *pbuf);

private:
	bool				m_bReady;
	int					m_iItems;
	clientinventory_s	*m_pItems;
	clientinventory_s	*m_pActiveItem;
	CommandButton		*m_pActiveButton;
	Label				*m_pTitle;
	CommandButton		*m_pClose;
	ScrollPanel			*m_pScrollPanel;
	Label				*m_pEmptyLabel;
	CScrollButton		**m_pButtons;
	CTransparentPanel	*m_pDetails;
	Label				*m_pName;
	Label				*m_pDescription;
	CommandButton		*m_pActivate;
	CommandButton		*m_pDrop;
	CommandButton		*m_pDropAll;
};

#endif
