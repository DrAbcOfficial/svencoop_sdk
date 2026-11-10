#ifndef VGUI_MEDIASELECT_H
#define VGUI_MEDIASELECT_H

#include "vgui_Inventory.h"

struct MediaData
{
	unsigned int	index;
	char			path[1024];
};

void MediaButtonCallback(void *data);

class CMediaSelect : public CMenuPanel
{
public:
	CMediaSelect(int iTrans, int iRemoveMe, int x, int y, int wide, int tall);
	~CMediaSelect();

	virtual void Open(void);
	virtual bool SlotInput(int slot);
	void Rebuild(void);
	void Output(void);
	void Update(void);
	void UpdateTime(void);
	void SetTitleText(const char *text);
	void SetTime(const char *text);
	void SetFill(float fill);
	bool AddItem(CScrollButton *button);
	bool RemoveItem(CScrollButton *button);
	bool RemoveItem(unsigned int index);
	void RemoveAllItems(void);
	CScrollButton *GetActiveItem(void);
	unsigned int GetItemIndex(CScrollButton *button);
	void SetActiveItem(unsigned int index);
	void SetActiveItem(CScrollButton *button);

private:
	Dar<CScrollButton *>	m_Items;
	unsigned int		m_iItems;
	unsigned int		m_iActiveItem;
	bool				m_bDirty;
	char				m_szTime[512];
	float				m_flFill;

	CTransparentPanel	*m_pBackground;
	CTFScrollPanel		*m_pScrollPanel;
	Label				*m_pTitle;
	Label				*m_pSelection;
	Label				*m_pTime;
	Label				*m_pVolume;
	CImageLabel			*m_pRandom;
	Label				*m_pFill;
};

#endif
