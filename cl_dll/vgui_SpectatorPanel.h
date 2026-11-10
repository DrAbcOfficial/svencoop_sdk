// vgui_SpectatorPanel.h: interface for the SpectatorPanel class.
//
//////////////////////////////////////////////////////////////////////

#ifndef SPECTATORPANEL_H
#define SPECTATORPANEL_H

#include <VGUI_Panel.h>
#include <VGUI_Label.h>

using namespace vgui;

class CTransparentPanel;

class SpectatorPanel : public Panel
{
public:
	SpectatorPanel(int x, int y, int wide, int tall);
	virtual ~SpectatorPanel();

	void Initialize();
	void Update();

private:
	CTransparentPanel	*m_TopBorder;
	CTransparentPanel	*m_BottomBorder;
	Label				*m_Health;
	Label				*m_Armor;
	Label				*m_BottomMainLabel;
};

#endif
