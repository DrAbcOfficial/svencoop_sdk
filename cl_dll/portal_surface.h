#ifndef PORTAL_SURFACE_H
#define PORTAL_SURFACE_H

#ifndef DID_VEC3_T_DEFINE
#include "util_vector.h"
#endif

#include <unordered_map>
#include <unordered_set>
#include "com_model.h"
#include "sven_math.h"

#define PORTAL_FRUSTUM_NEAR		4.0f
#define PORTAL_FRUSTUM_MIN_FAR	256.0f

struct cl_entity_s;
struct model_s;
struct msurface_s;
struct ref_params_s;
class ClientPortalManager;

struct glpoly_s
{
	glpoly_s	*next;
	glpoly_s	*chain;
	int			numverts;
	int			flags;
	float		verts[4][7];
};

enum PortalMode
{
	PORTAL_MIRROR = 0,
	PORTAL_MONITOR,
	PORTAL_PORTAL
};

enum PortalTextureMode
{
	PORTAL_TEXTURE_TRANSPARENT = 0,
	PORTAL_TEXTURE_ALL,
	PORTAL_TEXTURE_NAMED
};

struct PortalSettings
{
	bool			m_bDisabled;
	bool			m_bWasDisabled;
	bool			m_bHalfFrame;
	float			m_flQuality;
	int				m_iFPS;
	float			m_flFrameInterval;
	unsigned int	m_iMaxTextureSize;
	unsigned int	m_iMinTextureSize;
};

void RotateVector(Vector &value, const Vector &angles, const Vector &origin, bool reverse = false);
void RotateVectorReverse(Vector &value, const Vector &angles, const Vector &origin);
void RotateAngles(Vector &angles, const Vector &rotation);
void GetAnglesFromVectors(const Vector &forward, const Vector &right, const Vector &up, Vector &angles);
void GetMirroredPosition(const Vector &planePoint, const Vector &normal, const Vector &position, Vector &mirrored);
void GetMirroredAngles(const Vector &normal, const Vector &angles, Vector &mirrored);
void GetSurfaceBounds(const Vector &angles, msurface_s *surface, Vector &mins, Vector &maxs);
Vector GetSurfaceNormal(msurface_s *surface);
void V_CalcFov(float fov, float width, float height, float *fovX, float *fovY);
void CalculateFrustumFromRefParams(const ref_params_s *params, PLANE *frustum);

class PortalSource
{
public:
	PortalSource(PortalSettings *settings, const Vector &origin, const Vector &angles,
		const Vector &pvsOrigin, bool mirrored, unsigned int width, unsigned int height, bool noPlanes);
	~PortalSource();

	void CreateTexture(void);
	void SetupRendering(ClientPortalManager *manager, ref_params_s *params);
	void CalculateClipPlane(PortalMode mode, const Vector &viewOrigin, const Vector &viewAngles,
		const Vector &origin, const Vector &angles, const Vector &sourceOrigin,
		const Vector &sourceAngles, float zoom, msurface_s *surface);

	Vector								m_vecOrigin;
	Vector								m_vecAngles;
	Vector								m_vecPVSOrigin;
	unsigned int						m_iClipPlanes;
	PLANE								m_ClipPlanes[6];
	std::unordered_set<msurface_s *>	m_Surfaces;
	std::unordered_set<mplane_t *>		m_Planes;
	bool								m_bMirrored;
	bool								m_bNoPlanes;
	unsigned int						m_iTexture;
	unsigned int						m_iWidth;
	unsigned int						m_iHeight;
};

class ClientPortal
{
public:
	ClientPortal();
	~ClientPortal();

	PortalMode GetPortalMode(void);
	const Vector &GetOrigin(void);
	const Vector &GetAngles(void);
	const Vector &GetSourceOrigin(void);
	const Vector &GetSourceAngles(void);
	unsigned int GetMonitorWidth(void);
	unsigned int GetMonitorHeight(void);
	float GetZoom(void);
	bool IsNoPlanes(void);
	bool IsActive(void);
	bool IsShootThroughEnabled(void);
	bool IsOutOfRange(void);
	bool IsPortalTexture(ClientPortalManager *manager, mtexinfo_t *texture);
	void Update(ClientPortalManager *manager, int entityIndex, int sourceEntityIndex,
		const Vector &origin, const Vector &angles, PortalMode mode, PortalTextureMode textureMode,
		const char *textureName, const Vector &sourceOrigin, const Vector &sourceAngles,
		unsigned int width, unsigned int height, float zoom, int fps, bool halfFrame,
		bool noPlanes, bool shootThrough, int minDistance, int maxDistance);
	void ReplaceModelTextures(ClientPortalManager *manager);
	void RestoreModelTextures(ClientPortalManager *manager, bool force);
	void CheckModelChanged(ClientPortalManager *manager, bool force);
	void PrepareForNewFrame(ClientPortalManager *manager);
	void SetActive(ClientPortalManager *manager, bool active, bool force = false);
	void CheckRenderDistance(ClientPortalManager *manager, const Vector &viewOrigin);
	bool ShouldRender(ClientPortalManager *manager, unsigned long frame);
	bool IsVisible(ClientPortalManager *manager, unsigned long frame, int visFrame,
		const Vector &viewOrigin, PLANE *frustum);
	bool IsSurfaceVisible(ClientPortalManager *manager, unsigned long frame, int visFrame,
		const Vector &viewOrigin, const Vector &viewForward, PLANE *frustum, msurface_s *surface);
	void AddTextureForQuickLookup(msurface_s *surface, unsigned int texture);
	unsigned int LookupTexture(msurface_s *surface);

	Vector	m_vecOrigin;
	Vector	m_vecAngles;
	Vector	m_vecSourceOrigin;
	Vector	m_vecSourceAngles;

	unsigned int	m_iMonitorWidth;
	unsigned int	m_iMonitorHeight;
	float			m_flZoom;
	bool			m_bNoPlanes;

	PortalMode			m_Mode;
	PortalTextureMode	m_TextureMode;
	char				m_szTextureName[33];

	int		m_iFrameParity;
	float	m_flLastRenderTime;
	int		m_iFPS;
	float	m_flFrameInterval;
	bool	m_bHalfFrame;
	int		m_iMinDistance;
	int		m_iMaxDistance;

	cl_entity_s	*m_pEntity;
	cl_entity_s	*m_pSourceEntity;
	model_s		*m_pModel;

	bool			m_bTexturesReplaced;
	unsigned long	m_iVisibleFrame;
	bool			m_bActive;
	bool			m_bVisible;
	bool			m_bShootThrough;

	std::unordered_map<msurface_s *, unsigned int>	m_SurfaceTextures;
	std::unordered_map<msurface_s *, bool>			m_SurfaceVisibility;
	bool											m_bOutOfRange;
	std::unordered_set<msurface_s *>				m_PortalSurfaces;
};

#endif
