#include "hud.h"
#include "cl_util.h"
#include "vgui_CameraPanel.h"
#include "triangleapi.h"

extern bool IsMouseActive(void);

CCamera_MouseInput::CCamera_MouseInput(Panel *panel, int wide, int tall)
{
	m_pPanel = panel;
	m_iMouseX = ScreenWidth / 2;
	m_iMouseY = ScreenHeight / 2;
	m_flWidth = wide;
	m_flHeight = tall;
	m_flNextUpdate = 0;
	m_bFocused = false;
	memset(m_vecMouse, 0, sizeof(m_vecMouse));
	memset(m_bMouseButtons, 0, sizeof(m_bMouseButtons));
	memset(m_bKeys, 0, sizeof(m_bKeys));
}

void CCamera_MouseInput::mousePressed(MouseCode code, Panel *panel)
{
	if (!IsMouseActive() || !m_pPanel->isVisible() || !m_bFocused)
		return;

	if ((unsigned int)code >= MOUSE_LAST || m_bMouseButtons[code])
		return;

	m_bMouseButtons[code] = true;
	SendUpdate(code, 1);
}

void CCamera_MouseInput::mouseDoublePressed(MouseCode code, Panel *panel)
{
	mousePressed(code, panel);
}

void CCamera_MouseInput::mouseReleased(MouseCode code, Panel *panel)
{
	if (!IsMouseActive() || !m_pPanel->isVisible() || !m_bFocused)
		return;

	if ((unsigned int)code >= MOUSE_LAST || !m_bMouseButtons[code])
		return;

	m_bMouseButtons[code] = false;
	SendUpdate(code, 0);
}

void CCamera_MouseInput::cursorMoved(int x, int y, Panel *panel)
{
	if (!IsMouseActive() || !m_pPanel->isVisible() || !m_bFocused)
		return;

	if (x > ScreenWidth)
		x = ScreenWidth;
	else if (x < 0)
		x = 0;

	if (y > ScreenHeight)
		y = ScreenHeight;
	else if (y < 0)
		y = 0;

	if (m_iMouseX == x && m_iMouseY == y)
		return;

	m_iMouseX = x;
	m_iMouseY = y;
	m_vecMouse[0] = x / (m_flWidth * 0.5f) - 1.0f;
	m_vecMouse[1] = -(y / (m_flHeight * 0.5f) - 1.0f);

	if ((float)gEngfuncs.GetClientTime() >= m_flNextUpdate)
	{
		if (m_bMouseButtons[MOUSE_LEFT])
			SendUpdate(MOUSE_LEFT, 3);
		else if (m_bMouseButtons[MOUSE_RIGHT])
			SendUpdate(MOUSE_RIGHT, 3);
		else if (m_bMouseButtons[MOUSE_MIDDLE])
			SendUpdate(MOUSE_MIDDLE, 3);
		else
			SendUpdate(0, 4);

		m_flNextUpdate = gEngfuncs.GetClientTime() + 0.05f;
	}
}

void CCamera_MouseInput::cursorMovedDelta(int x, int y, Panel *panel)
{
	cursorMoved(m_iMouseX + x, m_iMouseY + y, panel);
}

void CCamera_MouseInput::mouseWheeled(int delta, Panel *panel)
{
	if (!IsMouseActive() || !m_pPanel->isVisible() || !m_bFocused)
		return;

	SendUpdate(delta, 5);
}

void CCamera_MouseInput::keyPressed(KeyCode code, Panel *panel)
{
	if (!IsMouseActive() || !m_pPanel->isVisible() || !m_bFocused)
		return;

	if ((unsigned int)code >= KEY_LAST || m_bKeys[code])
		return;

	char command[128];

	m_bKeys[code] = true;
	UTIL_FormatString(command, "cam_key 1 %i", code);
	gEngfuncs.pfnClientCmd(command);
}

void CCamera_MouseInput::keyReleased(KeyCode code, Panel *panel)
{
	if (!IsMouseActive() || !m_pPanel->isVisible() || !m_bFocused)
		return;

	if ((unsigned int)code >= KEY_LAST)
		return;

	char command[128];

	m_bKeys[code] = false;
	UTIL_FormatString(command, "cam_key 0 %i", code);
	gEngfuncs.pfnClientCmd(command);
}

int CCamera_MouseInput::KeyInput(int down, int keynum, const char *binding)
{
	return IsMouseActive() && m_pPanel->isVisible();
}

void CCamera_MouseInput::MouseButtonsChanged(bool left, bool right, bool middle, Panel *panel)
{
	if (left)
		mousePressed(MOUSE_LEFT, panel);
	else
		mouseReleased(MOUSE_LEFT, panel);

	if (right)
		mousePressed(MOUSE_RIGHT, panel);
	else
		mouseReleased(MOUSE_RIGHT, panel);

	if (middle)
		mousePressed(MOUSE_MIDDLE, panel);
	else
		mouseReleased(MOUSE_MIDDLE, panel);
}

void CCamera_MouseInput::GainedFocus(void)
{
	for (int i = 0; i < MOUSE_LAST; i++)
		m_bMouseButtons[i] = App::getInstance()->isMouseDown((MouseCode)i, m_pPanel);

	m_flNextUpdate = 0;
	m_bFocused = true;
}

void CCamera_MouseInput::LostFocus(void)
{
	memset(m_bMouseButtons, 0, sizeof(m_bMouseButtons));
	memset(m_bKeys, 0, sizeof(m_bKeys));
	m_flNextUpdate = 0;
	m_bFocused = false;
}

void CCamera_MouseInput::VidInit(void)
{
	memset(m_bMouseButtons, 0, sizeof(m_bMouseButtons));
	memset(m_bKeys, 0, sizeof(m_bKeys));
	m_flNextUpdate = 0;
}

void CCamera_MouseInput::GetMousePos(int &x, int &y)
{
	x = m_iMouseX;
	y = m_iMouseY;
}

void CCamera_MouseInput::SendUpdate(int button, int event)
{
	if (!IsMouseActive() || !m_pPanel->isVisible() || !m_bFocused)
		return;

	float world[3];
	char command[128];

	gEngfuncs.pTriAPI->ScreenToWorld(m_vecMouse, world);
	UTIL_FormatString(command, "cam_mouse_%i_%i_%.2f_%.2f_%.2f_%.2f_%.2f",
		event, button, m_vecMouse[0], m_vecMouse[1], world[0], world[1], world[2]);
	gEngfuncs.pfnClientCmd(command);
}

CCameraPanel::CCameraPanel(int iTrans, int iRemoveMe, int x, int y, int wide, int tall)
	: CMenuPanel(iTrans, iRemoveMe, x, y, wide, tall)
{
	m_pBackground = new CTransparentPanel(255, x, y, wide, tall);
	m_pBackground->setParent(this);
	m_pBackground->setVisible(true);
	m_pMouseInput = new CCamera_MouseInput(m_pBackground, wide, tall);
	m_pBackground->addInputSignal(m_pMouseInput);
	m_hCursor = 0;
	m_bOpen = false;
}

void CCameraPanel::Open(void)
{
	CMenuPanel::Open();
	m_pBackground->requestFocus();
	m_pMouseInput->GainedFocus();
	m_bOpen = true;
	gViewPort->UpdateCursorState();
}

void CCameraPanel::Close(void)
{
	m_pMouseInput->LostFocus();
	CMenuPanel::Close();
	m_bOpen = false;
	gViewPort->UpdateCursorState();
}

int CCameraPanel::KeyInput(int down, int keynum, const char *binding)
{
	return m_pMouseInput->KeyInput(down, keynum, binding);
}

void CCameraPanel::MouseWheeled(int delta)
{
	m_pMouseInput->mouseWheeled(delta, m_pBackground);
}

void CCameraPanel::MouseButtonsChanged(bool left, bool right, bool middle)
{
	m_pMouseInput->MouseButtonsChanged(left, right, middle, m_pBackground);
}

void CCameraPanel::MouseMovedDelta(int x, int y)
{
	m_pMouseInput->cursorMovedDelta(x, y, m_pBackground);
}

bool CCameraPanel::IsOpen(void)
{
	return m_bOpen;
}

void CCameraPanel::SetOverrideCursor(const char *sprite)
{
	m_hCursor = 0;

	if (sprite)
	{
		m_hCursor = SPR_Load(sprite);
		if (!m_hCursor || !gEngfuncs.pTriAPI->SpriteTexture((model_s *)gEngfuncs.GetSpritePointer(m_hCursor), 0))
			m_hCursor = 0;
	}
}

bool CCameraPanel::HasOverrideCursor(void)
{
	return m_hCursor != 0;
}

void CCameraPanel::DrawOverrideCursor(float time)
{
	if (!IsMouseActive() || !m_pBackground->isVisible() || !IsOpen() || !isVisible() || !HasOverrideCursor())
		return;

	int x = ScreenWidth / 2, y = ScreenHeight / 2;

	m_pMouseInput->GetMousePos(x, y);

	int frame = (int)(time * 10.0f) % SPR_Frames(m_hCursor);
	wrect_t rect;

	rect.left = rect.top = 0;
	rect.right = SPR_Width(m_hCursor, frame);
	rect.bottom = SPR_Height(m_hCursor, frame);
	SPR_Set(m_hCursor, 255, 255, 255);
	SPR_DrawHoles(frame, x, y, &rect);
}

void CCameraPanel::VidInit(void)
{
	m_pMouseInput->VidInit();
}
