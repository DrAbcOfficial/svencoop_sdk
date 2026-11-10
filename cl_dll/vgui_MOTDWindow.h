#ifndef VGUI_MOTDWINDOW_H
#define VGUI_MOTDWINDOW_H

#include "vgui_TeamFortressViewport.h"

class CMessageWindowPanel : public CMenuPanel
{
public:
	CMessageWindowPanel(int iRemoveMe, int x, int y, int wide, int tall, int iTextID);
	void Initialize(const char *szText, const char *szTitle, bool bShadeFullscreen = true);

private:
	CTransparentPanel	*m_pBackgroundPanel;
	ScrollPanel			*m_pScrollPanel;
	Label				*m_pTitle;
	TextPanel			*m_pText;
};

#endif
