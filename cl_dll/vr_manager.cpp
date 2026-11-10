#include "extdll.h"
#include "hud_iface.h"
#include "cvardef.h"
#include "ref_params.h"
#include "portal_gl.h"
#include "vr_manager.h"

#define VR_SEPARATION_SCALE	0.01f
#define VR_MAX_SEPARATION	10.0f

void AlertMessage(ALERT_TYPE type, char *format, ...);

static VRManager vrManager;
static bool vrFBOWarningPrinted;

void VRManager::InitCVars(void)
{
	m_pVR = gEngfuncs.pfnRegisterVariable("cl_vr", "0", FCVAR_ARCHIVE);
	m_pInverted = gEngfuncs.pfnRegisterVariable("cl_vr_inverted", "0", FCVAR_ARCHIVE);
}

void VRManager::GenerateTexture(unsigned int *texture, int width, int height)
{
	if (*texture)
	{
		glDeleteTextures(1, texture);
		*texture = 0;
	}

	glGenTextures(1, texture);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool CancelVRDueToEngineFBOBeingSet(void)
{
	bool cancel = CancelPortalsDueToEngineFBOBeingSet();

	if (cancel && !vrFBOWarningPrinted)
	{
		AlertMessage(at_notice, "WARNING: 3D stereoscopic rendering unavailable due to FBO being set by engine. "
			"(To fix this, use borderless window instead of fullscreen mode, or restart the game using -nofbo "
			"as start parameter.)\n");
		vrFBOWarningPrinted = true;
	}

	return cancel;
}

// Each 3D frame renders the view twice: once per eye, offset along the view's right vector
void VRManager::VRRefdef(ref_params_s *params)
{
	if (!m_pVR)
		return;

	float separation = m_pVR->value * VR_SEPARATION_SCALE;

	if (separation < VR_SEPARATION_SCALE)
		return;

	if (CancelVRDueToEngineFBOBeingSet())
		return;

	separation = fminf(VR_MAX_SEPARATION, separation);

	if (m_pInverted && m_pInverted->value >= 1)
		separation = -separation;

	GLint activeTexture;

	glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);

	if (activeTexture != GL_TEXTURE0 && !SetActiveTexture(GL_TEXTURE0))
	{
		AlertMessage(at_console, "Invalid GL_ACTIVE_TEXTURE, unable to reset. 3D view not drawn.\n");
		return;
	}

	int width = params->viewport[2];
	int height = params->viewport[3];

	if (!m_uiLeftTexture || m_iWidth != width || m_iHeight != height)
		GenerateTexture(&m_uiLeftTexture, width, height);

	if (!m_uiRightTexture || m_iWidth != width || m_iHeight != height)
		GenerateTexture(&m_uiRightTexture, width, height);

	if (!m_uiLeftTexture || !m_uiRightTexture)
	{
		AlertMessage(at_console, "Couldn't create textures for VR/3D rendering. 3D view not drawn.\n");
		return;
	}

	m_iWidth = width;
	m_iHeight = height;

	if (params->nextView == 0)
	{
		params->nextView = 1;
	}
	else if (params->nextView == 1)
	{
		vec3_t forward, right, up;

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_uiLeftTexture);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_iWidth, m_iHeight);
		glBindTexture(GL_TEXTURE_2D, 0);

		gEngfuncs.pfnAngleVectors(params->viewangles, forward, right, up);

		for (int i = 0; i < 3; i++)
			params->vieworg[i] += right[i] * separation;

		params->nextView = 2;
	}
	else if (params->nextView == 2)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_uiRightTexture);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_iWidth, m_iHeight);
		glBindTexture(GL_TEXTURE_2D, 0);

		params->onlyClientDraw = 1;
		params->nextView = 0;
		m_bDraw = true;
	}

	if (activeTexture != GL_TEXTURE0)
		SetActiveTexture(activeTexture);
}

bool VRManager::DrawVR(void)
{
	if (!m_bDraw)
		return false;

	if (CancelVRDueToEngineFBOBeingSet())
		return false;

	GLint activeTexture;

	glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);

	if (activeTexture != GL_TEXTURE0 && !SetActiveTexture(GL_TEXTURE0))
	{
		AlertMessage(at_console, "Invalid GL_ACTIVE_TEXTURE, unable to reset. 3D view not drawn.\n");
		return false;
	}

	glColor4f(1, 1, 1, 1);
	glDisable(GL_BLEND);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 1, 1, 0, 0.1, 100);
	glDepthMask(GL_FALSE);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

	glBindTexture(GL_TEXTURE_2D, m_uiLeftTexture);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0);
	glVertex3f(0, 1, -1);
	glTexCoord2f(0, 1);
	glVertex3f(0, 0, -1);
	glTexCoord2f(1, 1);
	glVertex3f(0.5f, 0, -1);
	glTexCoord2f(1, 0);
	glVertex3f(0.5f, 1, -1);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, m_uiRightTexture);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0);
	glVertex3f(0.5f, 1, -1);
	glTexCoord2f(0, 1);
	glVertex3f(0.5f, 0, -1);
	glTexCoord2f(1, 1);
	glVertex3f(1, 0, -1);
	glTexCoord2f(1, 0);
	glVertex3f(1, 1, -1);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_STENCIL_TEST);
	glDisable(GL_ALPHA_TEST);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	if (activeTexture != GL_TEXTURE0)
		SetActiveTexture(activeTexture);

	m_bDraw = false;
	return true;
}

void P_VRInitCVars(void)
{
	vrManager.InitCVars();
}

void P_VRRefdef(ref_params_s *params)
{
	vrManager.VRRefdef(params);
}

bool P_DrawVR(void)
{
	return vrManager.DrawVR();
}
