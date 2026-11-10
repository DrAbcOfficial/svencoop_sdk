#include "hud.h"
#include "cl_util.h"
#include "portal_surface.h"
#include "portal_manager.h"
#include "portal_gl.h"
#include "ref_params.h"
#include "pm_movevars.h"

#include <SDL2/SDL_opengl.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

void RotateVectorX(Vector &value, float radians)
{
	if (radians == 0)
		return;

	float cosine = cos((double)radians);
	float sine = sin((double)radians);
	float x = value.x;
	float z = value.z;

	value.x = z * sine + x * cosine;
	value.z = z * cosine - x * sine;
}

void RotateVectorY(Vector &value, float radians)
{
	if (radians == 0)
		return;

	float cosine = cos((double)radians);
	float sine = sin((double)radians);
	float x = value.x;
	float y = value.y;

	value.x = x * cosine - y * sine;
	value.y = x * sine + y * cosine;
}

void RotateVectorZ(Vector &value, float radians)
{
	if (radians == 0)
		return;

	float cosine = cos((double)radians);
	float sine = sin((double)radians);
	float y = value.y;
	float z = value.z;

	value.y = y * cosine - z * sine;
	value.z = z * cosine + y * sine;
}

void RotateVector(Vector &value, const Vector &angles, const Vector &origin, bool reverse)
{
	if (value.Length() == 0)
		return;

	value = value - origin;

	if (reverse)
	{
		RotateVectorY(value, (double)angles.y / 180.0 * M_PI);
		RotateVectorX(value, (double)angles.x / 180.0 * M_PI);
		RotateVectorZ(value, (double)angles.z / 180.0 * M_PI);
	}
	else
	{
		RotateVectorZ(value, (double)angles.z / 180.0 * M_PI);
		RotateVectorX(value, (double)angles.x / 180.0 * M_PI);
		RotateVectorY(value, (double)angles.y / 180.0 * M_PI);
	}

	value = value + origin;
}

void RotateVectorReverse(Vector &value, const Vector &angles, const Vector &origin)
{
	RotateVector(value, angles, origin, true);
}

void _NormalizeAngles(Vector &angles)
{
	for (int i = 0; i < 3; i++)
	{
		while (angles[i] >= 360.0f)
			angles[i] -= 360.0f;

		while (angles[i] < 0)
			angles[i] += 360.0f;
	}
}

void GetAnglesFromVectors(const Vector &forward, const Vector &right, const Vector &up, Vector &angles)
{
	float yaw = atan2((double)forward.y, (double)forward.x);
	float cosYaw = cos((double)yaw);
	float sinYaw = sin((double)yaw);
	float roll = atan2((double)-right.z, (double)up.z);
	float cosRoll = cos((double)roll);
	float sinRoll = sin((double)roll);
	float cosPitch;

	if (fabs(cosYaw) > 0.01f)
		cosPitch = forward.x / cosYaw;
	else if (fabs(sinYaw) > 0.01f)
		cosPitch = forward.y / sinYaw;
	else if (fabs(sinRoll) > 0.01f)
		cosPitch = -right.z / sinRoll;
	else if (fabs(cosRoll) > 0.01f)
		cosPitch = up.z / cosRoll;
	else
		cosPitch = cos((double)(float)asin((double)-forward.z));

	float pitch = atan2((double)-forward.z, (double)cosPitch);

	angles.x = pitch / (M_PI / 180);
	angles.y = yaw / (M_PI / 180);
	angles.z = roll / (M_PI / 180);
	_NormalizeAngles(angles);
}

void RotateAngles(Vector &angles, const Vector &rotation)
{
	Vector forward, right, up;

	AngleVectors(angles, forward, right, up);
	RotateVectorReverse(forward, rotation, Vector(0, 0, 0));
	RotateVectorReverse(right, rotation, Vector(0, 0, 0));
	RotateVectorReverse(up, rotation, Vector(0, 0, 0));
	GetAnglesFromVectors(forward, right, up, angles);
	_NormalizeAngles(angles);
}

void GetMirroredPosition(const Vector &planePoint, const Vector &normal, const Vector &position, Vector &mirrored)
{
	float distance = DotProduct(normal, position) - DotProduct(planePoint, normal);

	mirrored = position - normal * (2 * distance);
}

void GetMirroredAngles(const Vector &normal, const Vector &angles, Vector &mirrored)
{
	Vector forward, right, up;
	Vector mirroredForward, mirroredRight, mirroredUp;
	Vector origin(0, 0, 0);

	AngleVectors(angles, forward, right, up);
	GetMirroredPosition(origin, normal, forward, mirroredForward);
	GetMirroredPosition(origin, normal, right, mirroredRight);
	GetMirroredPosition(origin, normal, up, mirroredUp);
	GetAnglesFromVectors(mirroredForward, -mirroredRight, mirroredUp, mirrored);
}

void GetSurfaceBounds(const Vector &angles, msurface_s *surface, Vector &mins, Vector &maxs)
{
	mins = Vector(FLT_MAX, FLT_MAX, FLT_MAX);
	maxs = Vector(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (int i = 0; i < surface->polys->numverts; i++)
	{
		Vector point(surface->polys->verts[i]);

		RotateVector(point, angles, Vector(0, 0, 0));

		for (int j = 0; j < 3; j++)
		{
			mins[j] = fminf(mins[j], point[j]);
			maxs[j] = fmaxf(maxs[j], point[j]);
		}
	}
}

Vector GetSurfaceNormal(msurface_s *surface)
{
	Vector first(surface->polys->verts[0]);
	Vector second(surface->polys->verts[1]);

	for (int i = 2; i < surface->polys->numverts; i++)
	{
		Vector point(surface->polys->verts[i]);
		Vector normal = CrossProduct(point - second, point - first);
		float length = normal.Length();

		if (length > 0.01f)
			return normal * (float)(1.0 / length);
	}

	return Vector(0, 0, 0);
}

void V_CalcFov(float fov, float width, float height, float *fovX, float *fovY)
{
	if (fov < 1 || fov > 170)
		fov = 90;

	*fovX = fov;

	float x = width / tan(fov * (float)(M_PI / 180) * 0.5f);

	*fovY = atan(height / x) * (float)(180 / M_PI) * 2;
}

void CalculateFrustumFromRefParams(const ref_params_s *params, PLANE *frustum)
{
	float fovX, fovY;

	V_CalcFov(gHUD.m_iFOV, params->viewport[2], params->viewport[3], &fovX, &fovY);

	float farClip = fmaxf(PORTAL_FRUSTUM_MIN_FAR, params->movevars->zmax);
	VECTOR3D forward(params->forward);
	VECTOR3D right(params->right);
	VECTOR3D up(params->up);
	VECTOR3D origin(params->vieworg);

	forward.Normalize();

	frustum[0].normal = forward;
	frustum[0].CalculateIntercept(forward * PORTAL_FRUSTUM_NEAR + origin);
	frustum[1].normal = -forward;
	frustum[1].CalculateIntercept(forward * farClip + origin);

	frustum[2].normal = forward.GetRotatedAxis(fovX - 90, up);
	frustum[2].CalculateIntercept(origin);
	frustum[3].normal = forward.GetRotatedAxis(90 - fovX, up);
	frustum[3].CalculateIntercept(origin);
	frustum[4].normal = forward.GetRotatedAxis(fovY - 90, right);
	frustum[4].CalculateIntercept(origin);
	frustum[5].normal = forward.GetRotatedAxis(90 - fovY, right);
	frustum[5].CalculateIntercept(origin);
}

PortalSource::PortalSource(PortalSettings *settings, const Vector &origin, const Vector &angles,
	const Vector &pvsOrigin, bool mirrored, unsigned int width, unsigned int height, bool noPlanes)
{
	m_vecOrigin = origin;
	m_vecAngles = angles;
	m_vecPVSOrigin = pvsOrigin;
	m_iClipPlanes = 0;
	m_bMirrored = mirrored;
	m_bNoPlanes = noPlanes;
	m_iTexture = 0;
	ClientPortalManager::MakeQuality(settings, width, height, &m_iWidth, &m_iHeight);
}

PortalSource::~PortalSource()
{
}

void PortalSource::CreateTexture(void)
{
	if (m_iTexture)
		return;

	GLint activeTexture;

	glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);

	if (activeTexture == GL_TEXTURE0 || SetActiveTexture(GL_TEXTURE0))
	{
		glGenTextures(1, &m_iTexture);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_iTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_iWidth, m_iHeight, 0, GL_RGBA, GL_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		SetActiveTexture(activeTexture);
	}
	else if (gEngfuncs.pfnGetCvarFloat("developer") >= 1)
	{
		gEngfuncs.Con_Printf("Invalid GL_ACTIVE_TEXTURE, unable to reset. Couldn't create texture for PortalSource.\n");
	}

	if (activeTexture != GL_TEXTURE0)
		SetActiveTexture(activeTexture);
}

void PortalSource::SetupRendering(ClientPortalManager *manager, ref_params_s *params)
{
	VectorCopy(m_vecOrigin, params->vieworg);
	VectorCopy(m_vecAngles, params->viewangles);
	VectorCopy(m_vecPVSOrigin, params->pvsOrigin);
	params->pvsOverride = 1;
	AngleVectors(params->viewangles, params->forward, params->right, params->up);
	params->viewport[0] = params->viewport[1] = 0;
	params->viewport[2] = m_iWidth;
	params->viewport[3] = m_iHeight;
	manager->DisableClipPlanes();

	for (unsigned int i = 0; i < m_iClipPlanes; i++)
		manager->EnableClipPlane(i, m_vecAngles, m_vecOrigin, m_ClipPlanes[i]);
}

void PortalSource::CalculateClipPlane(PortalMode mode, const Vector &viewOrigin, const Vector &viewAngles,
	const Vector &origin, const Vector &angles, const Vector &sourceOrigin,
	const Vector &sourceAngles, float zoom, msurface_s *surface)
{
	if (mode == PORTAL_MONITOR || m_bNoPlanes
		|| m_Surfaces.find(surface) != m_Surfaces.end()
		|| m_Planes.find(surface->plane) != m_Planes.end())
		return;

	if (m_iClipPlanes == 6)
	{
		if (gEngfuncs.pfnGetCvarFloat("developer") >= 1)
			gEngfuncs.Con_Printf("Error: Too many clip planes on portal! Maximum: 6 (Too many surfaces on brush?)\n");

		return;
	}

	Vector point = origin + Vector(surface->polys->verts[0]);
	Vector normal = GetSurfaceNormal(surface);

	RotateVector(point, angles, origin);
	RotateVector(normal, angles, Vector(0, 0, 0));

	if (mode != PORTAL_MIRROR)
	{
		point = (point - origin) * zoom;

		Vector rotation = sourceAngles - angles;

		RotateVectorReverse(point, rotation, Vector(0, 0, 0));
		point = point + sourceOrigin;
		RotateVectorReverse(normal, rotation, Vector(0, 0, 0));
	}
	else
	{
		normal = -normal;
	}

	m_Surfaces.insert(surface);
	m_Planes.insert(surface->plane);
	m_ClipPlanes[m_iClipPlanes].normal = VECTOR3D(normal);
	m_ClipPlanes[m_iClipPlanes].intercept = -DotProduct(point, normal);
	m_ClipPlanes[m_iClipPlanes].Normalize();
	m_ClipPlanes[m_iClipPlanes].intercept = -m_ClipPlanes[m_iClipPlanes].intercept;
	m_iClipPlanes++;
}

ClientPortal::ClientPortal()
{
	m_vecOrigin = m_vecAngles = m_vecSourceOrigin = m_vecSourceAngles = Vector(0, 0, 0);
	m_pEntity = m_pSourceEntity = NULL;
	m_pModel = NULL;
	m_Mode = PORTAL_MIRROR;
	m_TextureMode = PORTAL_TEXTURE_TRANSPARENT;
	memset(m_szTextureName, 0, sizeof(m_szTextureName));
	m_iVisibleFrame = 0;
	m_bActive = true;
	m_bVisible = false;
	m_iFrameParity = rand() % 2;
	m_flLastRenderTime = 0;
	m_bNoPlanes = false;
	m_iFPS = 0;
	m_flFrameInterval = 0;
	m_bHalfFrame = false;
	m_bShootThrough = false;
	m_bOutOfRange = false;
	m_bTexturesReplaced = false;
}

ClientPortal::~ClientPortal()
{
}

PortalMode ClientPortal::GetPortalMode(void)
{
	return m_Mode;
}

const Vector &ClientPortal::GetOrigin(void)
{
	return m_vecOrigin;
}

const Vector &ClientPortal::GetAngles(void)
{
	return m_vecAngles;
}

const Vector &ClientPortal::GetSourceOrigin(void)
{
	return m_vecSourceOrigin;
}

const Vector &ClientPortal::GetSourceAngles(void)
{
	return m_vecSourceAngles;
}

unsigned int ClientPortal::GetMonitorWidth(void)
{
	return m_iMonitorWidth;
}

unsigned int ClientPortal::GetMonitorHeight(void)
{
	return m_iMonitorHeight;
}

float ClientPortal::GetZoom(void)
{
	return m_flZoom;
}

bool ClientPortal::IsNoPlanes(void)
{
	return m_bNoPlanes;
}

bool ClientPortal::IsActive(void)
{
	return m_bActive;
}

bool ClientPortal::IsShootThroughEnabled(void)
{
	return m_bShootThrough;
}

bool ClientPortal::IsOutOfRange(void)
{
	return m_bOutOfRange;
}

bool ClientPortal::IsPortalTexture(ClientPortalManager *manager, mtexinfo_t *texture)
{
	if (m_TextureMode == PORTAL_TEXTURE_ALL)
		return true;

	if (!texture || !texture->texture)
		return false;

	if (texture == &manager->m_InvisibleTextureInfo)
		return true;

	if (m_TextureMode == PORTAL_TEXTURE_TRANSPARENT)
		return texture->texture->name[0] == '{';

	if (m_TextureMode == PORTAL_TEXTURE_NAMED)
		return stricmp(texture->texture->name, m_szTextureName) == 0;

	return false;
}

void ClientPortal::Update(ClientPortalManager *manager, int entityIndex, int sourceEntityIndex,
	const Vector &origin, const Vector &angles, PortalMode mode, PortalTextureMode textureMode,
	const char *textureName, const Vector &sourceOrigin, const Vector &sourceAngles,
	unsigned int width, unsigned int height, float zoom, int fps, bool halfFrame,
	bool noPlanes, bool shootThrough, int minDistance, int maxDistance)
{
	PortalTextureMode oldMode = m_TextureMode;
	bool nameChanged = stricmp(m_szTextureName, textureName) != 0;

	if (nameChanged)
	{
		memset(m_szTextureName, 0, sizeof(m_szTextureName));

		if (textureName && *textureName)
			strncpy(m_szTextureName, textureName, sizeof(m_szTextureName) - 1);
	}

	m_vecOrigin = origin;
	m_vecAngles = angles;
	m_Mode = mode;
	m_TextureMode = textureMode;
	m_vecSourceOrigin = sourceOrigin;
	m_vecSourceAngles = sourceAngles;
	m_iMonitorWidth = width;
	m_iMonitorHeight = height;
	m_iMinDistance = minDistance;
	m_iMaxDistance = maxDistance;
	m_bNoPlanes = noPlanes;
	m_bShootThrough = shootThrough;
	m_flZoom = zoom;
	m_iFPS = fps;
	m_flFrameInterval = fps > 0 ? 1.0f / fps : 0;
	m_bHalfFrame = halfFrame;

	if (!m_pEntity || m_pEntity->index != entityIndex)
		m_pEntity = gEngfuncs.GetEntityByIndex(entityIndex);

	if (!sourceEntityIndex)
	{
		m_pSourceEntity = NULL;
	}
	else if (!m_pSourceEntity || m_pSourceEntity->index != sourceEntityIndex)
	{
		m_pSourceEntity = gEngfuncs.GetEntityByIndex(sourceEntityIndex);

		if (!m_pSourceEntity || m_pSourceEntity->index != sourceEntityIndex)
			m_pSourceEntity = NULL;
	}

	CheckModelChanged(manager, oldMode != m_TextureMode || nameChanged);
}

void ClientPortal::ReplaceModelTextures(ClientPortalManager *manager)
{
	if (manager->GetSettings(m_Mode)->m_bDisabled || !m_pModel || m_bTexturesReplaced)
		return;

	m_PortalSurfaces.clear();

	for (int i = 0; i < m_pModel->nummodelsurfaces; i++)
	{
		msurface_t *surface = m_pModel->surfaces + m_pModel->firstmodelsurface + i;

		if (IsPortalTexture(manager, surface->texinfo))
		{
			manager->RegisterSurfaceTexture(surface);
			surface->texinfo = &manager->m_InvisibleTextureInfo;
			m_PortalSurfaces.insert(surface);
		}
	}

	m_bTexturesReplaced = true;
}

void ClientPortal::RestoreModelTextures(ClientPortalManager *manager, bool force)
{
	if (!force && manager->GetSettings(m_Mode)->m_bDisabled)
		return;

	if (!m_pModel || !m_bTexturesReplaced)
		return;

	for (int i = 0; i < m_pModel->nummodelsurfaces; i++)
	{
		msurface_t *surface = m_pModel->surfaces + m_pModel->firstmodelsurface + i;
		mtexinfo_t *texture = manager->GetOriginalSurfaceTexture(surface);

		if (texture && texture->texture)
			surface->texinfo = texture;
	}

	m_bTexturesReplaced = false;
}

void ClientPortal::CheckModelChanged(ClientPortalManager *manager, bool force)
{
	model_t *model = m_pEntity ? m_pEntity->model : NULL;

	if (model != m_pModel || force)
	{
		RestoreModelTextures(manager, force);
		m_pModel = model;
		ReplaceModelTextures(manager);
	}
}

void ClientPortal::PrepareForNewFrame(ClientPortalManager *manager)
{
	if (m_pSourceEntity)
	{
		m_vecSourceOrigin = m_pSourceEntity->origin;
		m_vecSourceAngles = m_pSourceEntity->angles;
	}

	for (std::unordered_map<msurface_s *, unsigned int>::iterator it = m_SurfaceTextures.begin();
		it != m_SurfaceTextures.end(); it++)
		manager->DecrementSharedTexture(it->second);

	m_SurfaceTextures.clear();
	m_SurfaceVisibility.clear();
}

void ClientPortal::SetActive(ClientPortalManager *manager, bool active, bool force)
{
	m_bActive = active;

	if (active)
		ReplaceModelTextures(manager);
	else
		RestoreModelTextures(manager, force);
}

void ClientPortal::CheckRenderDistance(ClientPortalManager *manager, const Vector &viewOrigin)
{
	if (!m_bActive)
		return;

	bool inRange = m_iMinDistance <= 0 && m_iMaxDistance <= 0;

	if (!inRange)
	{
		float distance = (viewOrigin - m_vecOrigin).Length();

		inRange = (m_iMinDistance <= 0 || distance >= m_iMinDistance)
			&& (m_iMaxDistance <= 0 || distance <= m_iMaxDistance);
	}

	m_bOutOfRange = !inRange;

	if (inRange)
		ReplaceModelTextures(manager);
	else
		RestoreModelTextures(manager, false);
}

bool ClientPortal::ShouldRender(ClientPortalManager *manager, unsigned long frame)
{
	if (!m_bActive)
		return false;

	PortalSettings *settings = manager->GetSettings(m_Mode);

	if (settings->m_bDisabled)
		return false;

	if ((m_bHalfFrame || settings->m_bHalfFrame) && (frame & 1) == m_iFrameParity)
		return false;

	float time = gEngfuncs.GetClientTime();
	float elapsed = time - m_flLastRenderTime;

	if (!(m_flFrameInterval <= elapsed) || !(settings->m_flFrameInterval <= elapsed))
		return false;

	m_flLastRenderTime = time;
	return true;
}

bool BoundsInFrustum(const Vector &origin, const Vector &mins, const Vector &maxs, PLANE *frustum)
{
	for (int i = 0; i < 6; i++)
	{
		VECTOR3D point;

		for (int j = 0; j < 3; j++)
			point[j] = origin[j] + (frustum[i].normal[j] <= 0 ? mins[j] : maxs[j]);

		if (frustum[i].ClassifyPoint(point) == POINT_BEHIND_PLANE)
			return false;
	}

	return true;
}

bool ClientPortal::IsVisible(ClientPortalManager *manager, unsigned long frame, int visFrame,
	const Vector &viewOrigin, PLANE *frustum)
{
	if (!m_bActive || manager->GetSettings(m_Mode)->m_bDisabled)
		return false;

	if (m_iVisibleFrame != frame)
	{
		CheckRenderDistance(manager, viewOrigin);
		m_iVisibleFrame = frame;
		m_bVisible = !m_bOutOfRange && m_pEntity && m_pModel && m_pModel->name[0] == '*'
			&& (m_pEntity->curstate.rendermode == 0 || m_pEntity->curstate.renderamt > 0)
			&& m_pEntity->curstate.messagenum == visFrame
			&& BoundsInFrustum(m_pEntity->origin, m_pModel->mins, m_pModel->maxs, frustum);
	}

	return m_bVisible;
}

bool ClientPortal::IsSurfaceVisible(ClientPortalManager *manager, unsigned long frame, int visFrame,
	const Vector &viewOrigin, const Vector &viewForward, PLANE *frustum, msurface_s *surface)
{
	if (!IsVisible(manager, frame, visFrame, viewOrigin, frustum))
		return false;

	std::unordered_map<msurface_s *, bool>::iterator cached = m_SurfaceVisibility.find(surface);

	if (cached != m_SurfaceVisibility.end())
		return cached->second;

	bool visible = IsPortalTexture(manager, surface->texinfo);

	if (visible)
	{
		Vector normal = GetSurfaceNormal(surface);
		Vector point(surface->polys->verts[0]);

		RotateVector(normal, m_pEntity->angles, Vector(0, 0, 0));
		RotateVector(point, m_pEntity->angles, Vector(0, 0, 0));
		visible = DotProduct(viewOrigin - (point + m_pEntity->origin), normal) > -0.01f;

		if (visible)
		{
			Vector mins, maxs;

			GetSurfaceBounds(m_pEntity->angles, surface, mins, maxs);
			visible = BoundsInFrustum(m_pEntity->origin, mins, maxs, frustum);
		}
	}

	m_SurfaceVisibility[surface] = visible;
	return visible;
}

void ClientPortal::AddTextureForQuickLookup(msurface_s *surface, unsigned int texture)
{
	m_SurfaceTextures[surface] = texture;
}

unsigned int ClientPortal::LookupTexture(msurface_s *surface)
{
	return m_SurfaceTextures[surface];
}
