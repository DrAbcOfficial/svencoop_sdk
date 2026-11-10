#ifndef VR_MANAGER_H
#define VR_MANAGER_H

struct cvar_s;
struct ref_params_s;

class VRManager
{
public:
	void InitCVars(void);
	void GenerateTexture(unsigned int *texture, int width, int height);
	void VRRefdef(ref_params_s *params);
	bool DrawVR(void);

private:
	cvar_s			*m_pVR;
	cvar_s			*m_pInverted;
	int				m_iWidth;
	int				m_iHeight;
	unsigned int	m_uiLeftTexture;
	unsigned int	m_uiRightTexture;
	bool			m_bDraw;
};

bool CancelVRDueToEngineFBOBeingSet(void);
void P_VRInitCVars(void);
void P_VRRefdef(ref_params_s *params);
bool P_DrawVR(void);

#endif
