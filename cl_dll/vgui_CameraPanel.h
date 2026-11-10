#ifndef VGUI_CAMERAPANEL_H
#define VGUI_CAMERAPANEL_H

#include "vgui_TeamFortressViewport.h"
#include <VGUI_KeyCode.h>
#include <VGUI_MouseCode.h>

class CCamera_MouseInput : public InputSignal
{
public:
	CCamera_MouseInput(Panel *panel, int wide, int tall);

	virtual void cursorMoved(int x, int y, Panel *panel);
	virtual void cursorEntered(Panel *panel) {}
	virtual void cursorExited(Panel *panel) {}
	virtual void mousePressed(MouseCode code, Panel *panel);
	virtual void mouseDoublePressed(MouseCode code, Panel *panel);
	virtual void mouseReleased(MouseCode code, Panel *panel);
	virtual void mouseWheeled(int delta, Panel *panel);
	virtual void keyPressed(KeyCode code, Panel *panel);
	virtual void keyTyped(KeyCode code, Panel *panel) {}
	virtual void keyReleased(KeyCode code, Panel *panel);
	virtual void keyFocusTicked(Panel *panel) {}
	virtual void cursorMovedDelta(int x, int y, Panel *panel);

	int KeyInput(int down, int keynum, const char *binding);
	void MouseButtonsChanged(bool left, bool right, bool middle, Panel *panel);
	void GainedFocus(void);
	void LostFocus(void);
	void VidInit(void);
	void GetMousePos(int &x, int &y);
	void SendUpdate(int button, int event);

private:
	Panel	*m_pPanel;
	bool	m_bMouseButtons[MOUSE_LAST];
	bool	m_bKeys[KEY_LAST];
	int		m_iMouseX;
	int		m_iMouseY;
	float	m_vecMouse[3];
	float	m_flWidth;
	float	m_flHeight;
	float	m_flNextUpdate;
	bool	m_bFocused;
};

class CCameraPanel : public CMenuPanel
{
public:
	CCameraPanel(int iTrans, int iRemoveMe, int x, int y, int wide, int tall);

	virtual void Open(void);
	virtual void Close(void);
	int KeyInput(int down, int keynum, const char *binding);
	void MouseWheeled(int delta);
	void MouseButtonsChanged(bool left, bool right, bool middle);
	void MouseMovedDelta(int x, int y);
	bool IsOpen(void);
	void SetOverrideCursor(const char *sprite);
	bool HasOverrideCursor(void);
	void DrawOverrideCursor(float time);
	void VidInit(void);

private:
	CTransparentPanel	*m_pBackground;
	CCamera_MouseInput	*m_pMouseInput;
	HSPRITE				m_hCursor;
	bool				m_bOpen;
};

#endif
