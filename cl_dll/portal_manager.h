#ifndef PORTAL_MANAGER_H
#define PORTAL_MANAGER_H

#include "portal_surface.h"
#include "pmtrace.h"
#include <vector>

struct tempent_s;

struct PortalTraceStep
{
	int		m_iStep;
	Vector	m_vecStart;
	Vector	m_vecEnd;
	Vector	m_vecDirection;
	Vector	m_vecPlaneNormal;
	float	m_flDistance;
	float	m_flFraction;
	float	m_flTotalFraction;
	float	m_flScale;
	int		m_iPortal;
};

struct PortalTraceResult : public pmtrace_t
{
	std::vector<PortalTraceStep *>	m_Steps;
	Vector							m_vecDirection;
	float							m_flScale;
};

class ClientPortalManager
{
public:
	ClientPortalManager();
	void ResetInit(void);
	void UpdateSettings(void);
	PortalSettings *GetSettings(PortalMode mode, bool unused = false);
	void SetPortalsEnabled(PortalMode mode, bool enabled);
	void EnablePortal(int index);
	void DisablePortal(int index);
	void KillPortal(int index);
	void UpdatePortal(int entityIndex, int sourceEntityIndex, const Vector &origin, const Vector &angles,
		PortalMode mode, PortalTextureMode textureMode, const char *textureName,
		const Vector &sourceOrigin, const Vector &sourceAngles, unsigned int width, unsigned int height,
		float zoom, int fps, bool halfFrame, bool noPlanes, bool shootThrough, int minDistance, int maxDistance);
	void RegisterSurfaceTexture(msurface_t *surface);
	mtexinfo_t *GetOriginalSurfaceTexture(msurface_t *surface);
	void IncrementSharedTexture(unsigned int texture);
	void DecrementSharedTexture(unsigned int texture);
	void GenerateInvisibleTexture(void);
	void GenerateVertexBuffers(void);
	bool AreShadersAvailable(void);
	void InitShader(void);
	void EnableShader(void);
	void DisableShader(void);
	void EnableClipPlane(int index, const Vector &angles, const Vector &origin, const PLANE &plane);
	void DisableClipPlanes(void);
	static void MakeQuality(PortalSettings *settings, unsigned int width, unsigned int height,
		unsigned int *outWidth, unsigned int *outHeight);
	PortalSource *CreatePortalSource(ClientPortal &portal, const Vector &viewOrigin,
		const Vector &viewAngles, int width, int height, msurface_t *surface);
	tempent_s *CreateTempEntityForIntelHDFix(cl_entity_t *player, float life, int modelIndex, const Vector &offset);
	bool IsEntityIntelHDFixTempEnt(cl_entity_t *entity);
	void PortalRender(ref_params_s *params);
	bool DrawPortals(void);
	void DrawPortalSurface(ClientPortal &portal, msurface_t *surface, unsigned int texture);
	void TracePortal(const Vector &start, const Vector &direction, float distance, int flags, int ignore,
		PortalTraceResult *result);

	PortalSettings								m_PortalSettings;
	PortalSettings								m_MirrorSettings;
	PortalSettings								m_MonitorSettings;
	bool										m_bDisableShaders;
	std::unordered_map<int, ClientPortal>		m_Portals;
	std::unordered_map<msurface_t *, mtexinfo_t *>	m_OriginalTextures;
	std::vector<PortalSource *>					m_PortalSources;
	std::unordered_map<unsigned int, unsigned int>	m_SharedTextures;
	Vector										m_vecForward;
	Vector										m_vecRight;
	Vector										m_vecUp;
	Vector										m_vecUnknown220;
	std::vector<tempent_s *>						m_IntelHDFixEntities;
	float										m_flNextIntelHDFix;
	int											m_iIntelHDFixModel;
	Vector										m_vecSavedViewOrigin;
	Vector										m_vecSavedViewAngles;
	Vector										m_vecSavedForward;
	Vector										m_vecSavedRight;
	Vector										m_vecSavedUp;
	float										m_SavedViewport[4];
	PLANE										m_Frustum[6];
	unsigned long								m_iFrame;
	int											m_iVisFrame;
	unsigned int								m_iInvisibleTexture;
	mtexinfo_t									m_InvisibleTextureInfo;
	bool										m_bShadersAvailable;
	unsigned int								m_iShader;
	bool										m_bSkipViewModel;
};

extern bool g_bRenderingPortals;

void P_PortalInitCVars(void);
void P_PortalInit(void);
void P_PortalRender(ref_params_s *params);
bool P_DrawPortals(void);
void P_EnablePortal(int index);
void P_DisablePortal(int index);
void P_UpdatePortal(int entityIndex, int sourceEntityIndex, const float *origin, const float *angles,
	unsigned char mode, unsigned char textureMode, const char *textureName,
	const float *sourceOrigin, const float *sourceAngles, unsigned int width, unsigned int height,
	float zoom, unsigned char fps, bool halfFrame, bool noPlanes, bool shootThrough, int minDistance, int maxDistance);
bool P_ShouldGameStudioModelRendererRenderPlayerModel(void);
bool P_IsEntityIntelHDFixTempEnt(cl_entity_t *entity);
void P_TracePortal(const Vector &start, const Vector &direction, float distance, int flags, int ignore,
	PortalTraceResult *result);

#endif
