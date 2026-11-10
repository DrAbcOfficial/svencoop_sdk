#include "hud.h"
#include "cl_util.h"
#include "portal_manager.h"
#include "portal_gl.h"
#include "IClient.h"
#include "ref_params.h"
#include "pm_movevars.h"
#include "event_api.h"
#include "r_efx.h"
#include "triangleapi.h"
#include "eiface.h"

#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

extern "C" int CL_IsThirdPerson(void);
void AlertMessage(ALERT_TYPE type, char *format, ...);

static ClientPortalManager *g_pPortalManager;

static cvar_t *cl_portal_quality;
static cvar_t *cl_portal_halfframes;
static cvar_t *cl_portal_fps;
static cvar_t *cl_portal_disabled;
static cvar_t *cl_mirror_quality;
static cvar_t *cl_mirror_halfframes;
static cvar_t *cl_mirror_fps;
static cvar_t *cl_mirror_disabled;
static cvar_t *cl_monitor_quality;
static cvar_t *cl_monitor_halfframes;
static cvar_t *cl_monitor_fps;
static cvar_t *cl_monitor_disabled;
static cvar_t *cl_portal_disable_shaders;

static unsigned int mirror_vertexbuffer;
static unsigned int portal_vertexbuffer;
static bool fboWarningPrinted;

void PortalWarning(const char *message)
{
	if (CVAR_GET_FLOAT("developer") >= 1)
		gEngfuncs.Con_Printf("%s", message);
}

void P_PortalInitCVars(void)
{
	cl_portal_quality = CVAR_CREATE("cl_portal_quality", "100", FCVAR_ARCHIVE);
	cl_portal_halfframes = CVAR_CREATE("cl_portal_halfframes", "0", FCVAR_ARCHIVE);
	cl_portal_fps = CVAR_CREATE("cl_portal_fps", "0", FCVAR_ARCHIVE);
	cl_portal_disabled = CVAR_CREATE("cl_portal_disabled", "0", FCVAR_ARCHIVE);
	cl_mirror_quality = CVAR_CREATE("cl_mirror_quality", "100", FCVAR_ARCHIVE);
	cl_mirror_halfframes = CVAR_CREATE("cl_mirror_halfframes", "0", FCVAR_ARCHIVE);
	cl_mirror_fps = CVAR_CREATE("cl_mirror_fps", "0", FCVAR_ARCHIVE);
	cl_mirror_disabled = CVAR_CREATE("cl_mirror_disabled", "0", FCVAR_ARCHIVE);
	cl_monitor_quality = CVAR_CREATE("cl_monitor_quality", "100", FCVAR_ARCHIVE);
	cl_monitor_halfframes = CVAR_CREATE("cl_monitor_halfframes", "0", FCVAR_ARCHIVE);
	cl_monitor_fps = CVAR_CREATE("cl_monitor_fps", "0", FCVAR_ARCHIVE);
	cl_monitor_disabled = CVAR_CREATE("cl_monitor_disabled", "0", FCVAR_ARCHIVE);
	cl_portal_disable_shaders = CVAR_CREATE("cl_portal_disable_shaders", "0", FCVAR_ARCHIVE);
}

void P_PortalInit(void)
{
	InitializePortalGL();

	if (!g_pPortalManager)
		g_pPortalManager = new ClientPortalManager;

	g_pPortalManager->ResetInit();
}

ClientPortalManager *GetPortalManager(void)
{
	if (!g_pPortalManager)
		P_PortalInit();

	return g_pPortalManager;
}

void P_PortalRender(ref_params_s *params)
{
	GetPortalManager()->PortalRender(params);
}

bool CancelPortalsDueToEngineFBOBeingSet(void)
{
	GLint framebuffer = 0;
	GLint drawFramebuffer = 0;
	GLint readFramebuffer = 0;

	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebuffer);
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFramebuffer);
	glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readFramebuffer);

	if (!framebuffer && !drawFramebuffer && !readFramebuffer)
		return false;

	if (!fboWarningPrinted)
	{
		AlertMessage(at_notice, "WARNING: Cannot render portals, mirrors or monitors due to FBO being set by engine. "
			"(To fix this, use borderless window instead of fullscreen mode, or restart the game using -nofbo "
			"as start parameter.)\n");
		fboWarningPrinted = true;
	}

	return true;
}

bool P_DrawPortals(void)
{
	return GetPortalManager()->DrawPortals();
}

void P_EnablePortal(int index)
{
	GetPortalManager()->EnablePortal(index);
}

void P_DisablePortal(int index)
{
	GetPortalManager()->DisablePortal(index);
}

void P_UpdatePortal(int entityIndex, int sourceEntityIndex, const float *origin, const float *angles,
	unsigned char mode, unsigned char textureMode, const char *textureName,
	const float *sourceOrigin, const float *sourceAngles, unsigned int width, unsigned int height,
	float zoom, unsigned char fps, bool halfFrame, bool noPlanes, bool shootThrough, int minDistance, int maxDistance)
{
	GetPortalManager()->UpdatePortal(entityIndex, sourceEntityIndex,
		Vector(origin[0], origin[1], origin[2]), Vector(angles[0], angles[1], angles[2]),
		mode <= PORTAL_PORTAL ? (PortalMode)mode : PORTAL_MIRROR,
		textureMode <= PORTAL_TEXTURE_NAMED ? (PortalTextureMode)textureMode : PORTAL_TEXTURE_TRANSPARENT,
		textureName, Vector(sourceOrigin[0], sourceOrigin[1], sourceOrigin[2]),
		Vector(sourceAngles[0], sourceAngles[1], sourceAngles[2]),
		width, height, zoom, fps, halfFrame, noPlanes, shootThrough, minDistance, maxDistance);
}

bool P_ShouldGameStudioModelRendererRenderPlayerModel(void)
{
	ClientPortalManager *manager = GetPortalManager();
	bool skip = manager->m_bSkipViewModel;

	manager->m_bSkipViewModel = false;
	return skip;
}

bool P_IsEntityIntelHDFixTempEnt(cl_entity_t *entity)
{
	return GetPortalManager()->IsEntityIntelHDFixTempEnt(entity);
}

void P_TracePortal(const Vector &start, const Vector &direction, float distance, int flags, int ignore,
	PortalTraceResult *result)
{
	GetPortalManager()->TracePortal(start, direction, distance, flags, ignore, result);
}

ClientPortalManager::ClientPortalManager() : m_PortalSettings(), m_MirrorSettings(), m_MonitorSettings()
{
	m_bDisableShaders = false;
	m_iInvisibleTexture = 0;
	m_bShadersAvailable = false;
	m_iShader = 0;
	ResetInit();
}

void ClientPortalManager::ResetInit(void)
{
	m_iFrame = 0;
	m_iVisFrame = 0;
	m_IntelHDFixEntities.clear();
	m_flNextIntelHDFix = 0;
	m_iIntelHDFixModel = 0;
	m_bSkipViewModel = false;
	m_Portals.clear();
	m_OriginalTextures.clear();

	for (size_t i = 0; i < m_PortalSources.size(); i++)
		delete m_PortalSources[i];

	m_PortalSources.clear();

	for (auto &texture : m_SharedTextures)
	{
		glDeleteTextures(1, &texture.second);
	}

	m_SharedTextures.clear();
	m_vecForward = m_vecRight = m_vecUp = m_vecUnknown220 = Vector(0, 0, 0);
	m_vecSavedViewOrigin = m_vecSavedViewAngles = Vector(0, 0, 0);
	m_vecSavedForward = m_vecSavedRight = m_vecSavedUp = Vector(0, 0, 0);
	memset(m_SavedViewport, 0, sizeof(m_SavedViewport));

	for (int i = 0; i < 6; i++)
	{
		m_Frustum[i].normal.LoadZero();
		m_Frustum[i].intercept = 0;
	}

	GenerateInvisibleTexture();
	GenerateVertexBuffers();

	if (m_iShader && portalDeleteProgram)
	{
		portalDeleteProgram(m_iShader);
		m_iShader = 0;
	}

	m_bShadersAvailable = false;
	UpdateSettings();
}

void UpdatePortalSettings(PortalSettings &settings, cvar_t *quality, cvar_t *halfFrames,
	cvar_t *fps, cvar_t *disabled, int maxTextureSize)
{
	settings.m_iMaxTextureSize = maxTextureSize;
	settings.m_iMinTextureSize = 32;

	if (quality)
		settings.m_flQuality = fminf(1.0f, fmaxf(0.0f, quality->value * 0.01f));

	if (halfFrames)
		settings.m_bHalfFrame = halfFrames->value >= 1;

	if (disabled)
		settings.m_bDisabled = disabled->value >= 1;

	if (fps)
	{
		settings.m_iFPS = (int)fmaxf(0.0f, fps->value);
		settings.m_flFrameInterval = settings.m_iFPS > 0 ? 1.0f / settings.m_iFPS : 0;
	}
}

void ClientPortalManager::UpdateSettings(void)
{
	GLint maxTextureSize = 0;

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
	if (maxTextureSize < 32)
		maxTextureSize = 32;

	UpdatePortalSettings(m_PortalSettings, cl_portal_quality, cl_portal_halfframes,
		cl_portal_fps, cl_portal_disabled, maxTextureSize);
	UpdatePortalSettings(m_MirrorSettings, cl_mirror_quality, cl_mirror_halfframes,
		cl_mirror_fps, cl_mirror_disabled, maxTextureSize);
	UpdatePortalSettings(m_MonitorSettings, cl_monitor_quality, cl_monitor_halfframes,
		cl_monitor_fps, cl_monitor_disabled, maxTextureSize);

	if (cl_portal_disable_shaders)
		m_bDisableShaders = cl_portal_disable_shaders->value >= 1;

	if (m_PortalSettings.m_bWasDisabled != m_PortalSettings.m_bDisabled)
		SetPortalsEnabled(PORTAL_PORTAL, !m_PortalSettings.m_bDisabled);

	if (m_MirrorSettings.m_bWasDisabled != m_MirrorSettings.m_bDisabled)
		SetPortalsEnabled(PORTAL_MIRROR, !m_MirrorSettings.m_bDisabled);

	if (m_MonitorSettings.m_bWasDisabled != m_MonitorSettings.m_bDisabled)
		SetPortalsEnabled(PORTAL_MONITOR, !m_MonitorSettings.m_bDisabled);

	m_PortalSettings.m_bWasDisabled = m_PortalSettings.m_bDisabled;
	m_MirrorSettings.m_bWasDisabled = m_MirrorSettings.m_bDisabled;
	m_MonitorSettings.m_bWasDisabled = m_MonitorSettings.m_bDisabled;
}

PortalSettings *ClientPortalManager::GetSettings(PortalMode mode, bool unused)
{
	switch (mode)
	{
	case PORTAL_MIRROR:
		return &m_MirrorSettings;
	case PORTAL_MONITOR:
		return &m_MonitorSettings;
	case PORTAL_PORTAL:
		return &m_PortalSettings;
	default:
		if (CVAR_GET_FLOAT("developer") >= 1)
			gEngfuncs.Con_Printf("Warning: ClientPortalManager::GetSettings() called with unknown Portal Mode: %i\n", mode);

		return &m_MirrorSettings;
	}
}

void ClientPortalManager::SetPortalsEnabled(PortalMode mode, bool enabled)
{
	for (auto &entry : m_Portals)
	{
		if (entry.second.GetPortalMode() == mode)
			entry.second.SetActive(this, enabled, true);
	}
}

void ClientPortalManager::EnablePortal(int index)
{
	auto portal = m_Portals.find(index);

	if (portal != m_Portals.end())
		portal->second.SetActive(this, true);
}

void ClientPortalManager::DisablePortal(int index)
{
	auto portal = m_Portals.find(index);

	if (portal != m_Portals.end())
		portal->second.SetActive(this, false);
}

void ClientPortalManager::KillPortal(int index)
{
	m_Portals.erase(index);
}

void ClientPortalManager::UpdatePortal(int entityIndex, int sourceEntityIndex, const Vector &origin, const Vector &angles,
	PortalMode mode, PortalTextureMode textureMode, const char *textureName,
	const Vector &sourceOrigin, const Vector &sourceAngles, unsigned int width, unsigned int height,
	float zoom, int fps, bool halfFrame, bool noPlanes, bool shootThrough, int minDistance, int maxDistance)
{
	m_Portals[entityIndex].Update(this, entityIndex, sourceEntityIndex, origin, angles, mode, textureMode,
		textureName, sourceOrigin, sourceAngles, width, height, zoom, fps, halfFrame,
		noPlanes, shootThrough, minDistance, maxDistance);
}

void ClientPortalManager::RegisterSurfaceTexture(msurface_t *surface)
{
	if (surface->texinfo && surface->texinfo->texture && surface->texinfo != &m_InvisibleTextureInfo)
	{
		m_OriginalTextures[surface] = surface->texinfo;
		surface->texinfo = &m_InvisibleTextureInfo;
	}
}

mtexinfo_t *ClientPortalManager::GetOriginalSurfaceTexture(msurface_t *surface)
{
	auto texture = m_OriginalTextures.find(surface);

	return texture == m_OriginalTextures.end() ? NULL : texture->second;
}

void ClientPortalManager::IncrementSharedTexture(unsigned int texture)
{
	m_SharedTextures[texture]++;
}

void ClientPortalManager::DecrementSharedTexture(unsigned int texture)
{
	int count = m_SharedTextures[texture] - 1;

	if (count > 0)
	{
		m_SharedTextures[texture] = count;
	}
	else
	{
		glDeleteTextures(1, &texture);
		m_SharedTextures[texture] = 0;
		m_SharedTextures.erase(texture);
	}
}

void ClientPortalManager::GenerateInvisibleTexture(void)
{
	GLint activeTexture;

	if (m_iInvisibleTexture)
	{
		glDeleteTextures(1, &m_iInvisibleTexture);
		m_iInvisibleTexture = 0;
	}

	glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);
	if (activeTexture == GL_TEXTURE0 || SetActiveTexture(GL_TEXTURE0))
	{
		unsigned int pixel = 0;

		glGenTextures(1, &m_iInvisibleTexture);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_iInvisibleTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
		glBindTexture(GL_TEXTURE_2D, 0);

		memset(m_InvisibleTextureInfo.vecs, 0, sizeof(m_InvisibleTextureInfo.vecs));
		m_InvisibleTextureInfo.vecs[0][0] = 1;
		m_InvisibleTextureInfo.vecs[1][1] = 1;
		m_InvisibleTextureInfo.mipadjust = 0;
		m_InvisibleTextureInfo.flags = 0;
		m_InvisibleTextureInfo.texture = new texture_t;

		texture_t *texture = m_InvisibleTextureInfo.texture;

		strcpy(texture->name, "{");
		texture->width = texture->height = 1;
		texture->gl_texturenum = m_iInvisibleTexture;
		texture->anim_total = texture->anim_min = texture->anim_max = 0;
		texture->anim_next = texture->alternate_anims = NULL;
		memset(texture->offsets, 0, sizeof(texture->offsets));
		texture->paloffset = 0;
	}
	else
	{
		PortalWarning("Invalid GL_ACTIVE_TEXTURE, unable to reset. Couldn't create invisible texture for portals.\n");
	}

	if (activeTexture != GL_TEXTURE0)
		SetActiveTexture(activeTexture);
}

void ClientPortalManager::GenerateVertexBuffers(void)
{
	static const float mirrorBuffer[] =
	{
		0, 1, -1, 0, 0, -1, 1, 0, -1, 1, 1, -1,
		1, 0, 1, 1, 0, 1, 0, 0
	};
	static const float portalBuffer[] =
	{
		0, 1, -1, 0, 0, -1, 1, 0, -1, 1, 1, -1,
		0, 0, 0, 1, 1, 1, 1, 0
	};

	if (!portalGenBuffers || !portalDeleteBuffers || !portalBindBuffer || !portalBufferData)
		return;

	if (mirror_vertexbuffer)
		portalDeleteBuffers(1, &mirror_vertexbuffer);

	mirror_vertexbuffer = 0;

	if (portal_vertexbuffer)
		portalDeleteBuffers(1, &portal_vertexbuffer);

	portal_vertexbuffer = 0;
	portalGenBuffers(1, &mirror_vertexbuffer);
	portalBindBuffer(GL_ARRAY_BUFFER, mirror_vertexbuffer);
	portalBufferData(GL_ARRAY_BUFFER, sizeof(mirrorBuffer), mirrorBuffer, GL_STATIC_DRAW);
	portalBindBuffer(GL_ARRAY_BUFFER, 0);
	portalGenBuffers(1, &portal_vertexbuffer);
	portalBindBuffer(GL_ARRAY_BUFFER, portal_vertexbuffer);
	portalBufferData(GL_ARRAY_BUFFER, sizeof(portalBuffer), portalBuffer, GL_STATIC_DRAW);
	portalBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool ClientPortalManager::AreShadersAvailable(void)
{
	GLboolean compiler = GL_FALSE;

	if (m_bDisableShaders)
		return false;

	glGetBooleanv(GL_SHADER_COMPILER, &compiler);
	return compiler == GL_TRUE && portalCreateShader && portalShaderSource && portalCompileShader
		&& portalCreateProgram && portalAttachShader && portalLinkProgram && portalUseProgram
		&& portalDeleteProgram && portalDeleteShader;
}

void ClientPortalManager::InitShader(void)
{
	static const char *vertexSource =
		"varying vec2 vTexCoord;"
		"void main(void) {"
		"vTexCoord = gl_MultiTexCoord0.xy;"
		"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
		"}";
	static const char *fragmentSource =
		"sampler2D myTexture;"
		"varying vec2 vTexCoord;"
		"void main(void) {"
		"vec4 color = texture2D(myTexture, vTexCoord);"
		"if (color.rgb == vec3(0.0, 0.0, 1.0)) discard;"
		"gl_FragColor = color;"
		"}";

	m_bShadersAvailable = AreShadersAvailable();
	if (m_bShadersAvailable)
	{
		if (!m_iShader)
		{
			GLuint vertex = portalCreateShader(GL_VERTEX_SHADER);
			portalShaderSource(vertex, 1, &vertexSource, NULL);
			portalCompileShader(vertex);

			GLuint fragment = portalCreateShader(GL_FRAGMENT_SHADER);

			portalShaderSource(fragment, 1, &fragmentSource, NULL);
			portalCompileShader(fragment);
			m_iShader = portalCreateProgram();

			if (m_iShader)
			{
				portalAttachShader(m_iShader, vertex);
				portalAttachShader(m_iShader, fragment);
				portalLinkProgram(m_iShader);
			}
			else
			{
				m_bShadersAvailable = false;
			}

			portalDeleteShader(vertex);
			portalDeleteShader(fragment);
		}
	}
	else if (m_iShader && portalDeleteProgram)
	{
		portalDeleteProgram(m_iShader);
		m_iShader = 0;
	}
}

void ClientPortalManager::EnableShader(void)
{
	InitShader();
	if (m_bShadersAvailable && portalUseProgram)
		portalUseProgram(m_iShader);
}

void ClientPortalManager::DisableShader(void)
{
	InitShader();
	if (m_bShadersAvailable && portalUseProgram)
		portalUseProgram(0);
}

void ClientPortalManager::EnableClipPlane(int index, const Vector &angles, const Vector &origin, const PLANE &plane)
{
	VECTOR3D forward, right, up;
	Vector normal(plane.normal);
	GLdouble equation[4];

	_AngleVectors(VECTOR3D(angles), &forward, &right, &up);
	equation[0] = -DotProduct(right, normal);
	equation[1] = -DotProduct(up, normal);
	equation[2] = DotProduct(forward, normal);
	equation[3] = DotProduct(normal * plane.intercept - origin
		+ normal * 4, normal);
	glLoadIdentity();
	glClipPlane(GL_CLIP_PLANE0 + index, equation);
	glEnable(GL_CLIP_PLANE0 + index);
}

void ClientPortalManager::DisableClipPlanes(void)
{
	for (int i = 0; i < 6; i++)
		glDisable(GL_CLIP_PLANE0 + i);
}

void ClientPortalManager::MakeQuality(PortalSettings *settings, unsigned int width, unsigned int height,
	unsigned int *outWidth, unsigned int *outHeight)
{
	*outWidth = (int)width % 2 == 1 ? width + 1 : width;
	*outHeight = (int)height % 2 == 1 ? height + 1 : height;
}

PortalSource *ClientPortalManager::CreatePortalSource(ClientPortal &portal, const Vector &viewOrigin,
	const Vector &viewAngles, int width, int height, msurface_t *surface)
{
	if (portal.GetPortalMode() == PORTAL_MONITOR)
	{
		return new PortalSource(&m_MonitorSettings, portal.GetSourceOrigin(), portal.GetSourceAngles(),
			portal.GetSourceOrigin(), false, portal.GetMonitorWidth(), portal.GetMonitorHeight(), true);
	}

	Vector point = Vector(surface->polys->verts[0]) + portal.GetOrigin();
	Vector normal = surface->plane->normal;

	RotateVector(point, portal.GetAngles(), portal.GetOrigin());
	RotateVector(normal, portal.GetAngles(), Vector(0, 0, 0));

	if (portal.GetPortalMode() != PORTAL_MIRROR)
	{
		Vector origin = (viewOrigin - portal.GetOrigin()) * portal.GetZoom();
		Vector rotation = portal.GetSourceAngles() - portal.GetAngles();
		Vector angles = viewAngles;

		RotateVectorReverse(origin, rotation, Vector(0, 0, 0));
		origin = origin + portal.GetSourceOrigin();
		RotateAngles(angles, rotation);
		return new PortalSource(&m_PortalSettings, origin, angles, portal.GetSourceOrigin(),
			false, width, height, portal.IsNoPlanes());
	}

	Vector origin, angles;
	Vector mins, maxs;

	GetMirroredPosition(point, normal, viewOrigin, origin);
	GetMirroredAngles(normal, viewAngles, angles);
	GetSurfaceBounds(portal.GetAngles(), surface, mins, maxs);
	point = (mins + maxs) * 0.5f + portal.GetOrigin();
	return new PortalSource(&m_MirrorSettings, origin, angles, point, true, width, height, false);
}

tempent_s *ClientPortalManager::CreateTempEntityForIntelHDFix(cl_entity_t *player, float life,
	int modelIndex, const Vector &offset)
{
	Vector zero(0, 0, 0);
	TEMPENTITY *entity = gEngfuncs.pEfxAPI->R_TempModel(player->origin, zero, zero, life, modelIndex, 0);

	entity->entity.curstate.rendermode = kRenderNormal;
	entity->entity.curstate.renderamt = 255;
	entity->entity.curstate.scale = 0.001f;
	entity->flags = FTENT_PLYRATTACHMENT;
	entity->clientIndex = (short)player->index;
	entity->tentOffset = offset;
	return entity;
}

bool ClientPortalManager::IsEntityIntelHDFixTempEnt(cl_entity_t *entity)
{
	for (size_t i = 0; i < m_IntelHDFixEntities.size(); i++)
	{
		if (&m_IntelHDFixEntities[i]->entity == entity)
			return true;
	}

	return false;
}

void ClientPortalManager::PortalRender(ref_params_s *params)
{
	for (size_t i = 0; i < m_PortalSources.size(); i++)
		delete m_PortalSources[i];

	m_PortalSources.clear();
	UpdateSettings();

	if (m_PortalSettings.m_bDisabled && m_MirrorSettings.m_bDisabled && m_MonitorSettings.m_bDisabled)
		return;

	AngleVectors(params->viewangles, m_vecForward, m_vecRight, m_vecUp);
	m_iFrame++;
	m_iVisFrame = gEngfuncs.GetLocalPlayer()->curstate.messagenum;
	m_vecSavedViewOrigin = params->vieworg;
	m_vecSavedViewAngles = params->viewangles;
	m_vecSavedForward = params->forward;
	m_vecSavedRight = params->right;
	m_vecSavedUp = params->up;

	for (int i = 0; i < 4; i++)
		m_SavedViewport[i] = params->viewport[i];

	CalculateFrustumFromRefParams(params, m_Frustum);

	for (auto &entry : m_Portals)
	{
		ClientPortal &portal = entry.second;

		portal.CheckModelChanged(this, false);
		if (!portal.IsVisible(this, m_iFrame, m_iVisFrame, m_vecSavedViewOrigin, m_Frustum)
			|| !portal.ShouldRender(this, m_iFrame))
			continue;

		portal.PrepareForNewFrame(this);

		for (auto surface : portal.m_PortalSurfaces)
		{
			if (!portal.IsSurfaceVisible(this, m_iFrame, m_iVisFrame,
				m_vecSavedViewOrigin, m_vecSavedForward, m_Frustum, surface))
				continue;

			PortalSource *source = CreatePortalSource(portal, m_vecSavedViewOrigin, m_vecSavedViewAngles,
				(int)m_SavedViewport[2], (int)m_SavedViewport[3], surface);
			bool shared = false;

			for (size_t i = 0; i < m_PortalSources.size(); i++)
			{
				PortalSource *other = m_PortalSources[i];

				if (((source->m_bMirrored && other->m_bMirrored) || (source->m_bNoPlanes && other->m_bNoPlanes))
					&& source->m_vecAngles == other->m_vecAngles && source->m_vecOrigin == other->m_vecOrigin
					&& source->m_vecPVSOrigin == other->m_vecPVSOrigin)
				{
					delete source;
					source = other;
					shared = true;
					break;
				}
			}

			source->CalculateClipPlane(portal.GetPortalMode(), m_vecSavedViewOrigin, m_vecSavedViewAngles,
				portal.GetOrigin(), portal.GetAngles(), portal.GetSourceOrigin(), portal.GetSourceAngles(),
				portal.GetZoom(), surface);

			if (!source->m_iTexture)
				source->CreateTexture();

			portal.AddTextureForQuickLookup(surface, source->m_iTexture);
			IncrementSharedTexture(source->m_iTexture);

			if (!shared)
				m_PortalSources.push_back(source);
		}
	}

	if (m_PortalSources.empty())
		return;

	g_pEngineClient->PushView(NULL, false, false);
	g_bRenderingPortals = true;

	for (size_t i = 0; i < m_PortalSources.size(); i++)
	{
		PortalSource *source = m_PortalSources[i];

		source->SetupRendering(this, params);
		glClearColor(0, 0, 1, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		cl_entity_t *player = gEngfuncs.GetLocalPlayer();
		float time = gEngfuncs.GetClientTime();

		if (time >= m_flNextIntelHDFix)
		{
			if (!m_iIntelHDFixModel)
				m_iIntelHDFixModel = gEngfuncs.pEventAPI->EV_FindModelIndex("models/shell.mdl");

			for (size_t j = 0; j < m_IntelHDFixEntities.size(); j++)
				m_IntelHDFixEntities[j]->flags |= 0x40000;

			m_IntelHDFixEntities.clear();
			m_IntelHDFixEntities.push_back(CreateTempEntityForIntelHDFix(player, 1, m_iIntelHDFixModel, Vector(0, -10, 20)));
			m_IntelHDFixEntities.push_back(CreateTempEntityForIntelHDFix(player, 1, m_iIntelHDFixModel, Vector(0, 10, 20)));
			m_IntelHDFixEntities.push_back(CreateTempEntityForIntelHDFix(player, 1, m_iIntelHDFixModel, Vector(-10, 0, 20)));
			m_IntelHDFixEntities.push_back(CreateTempEntityForIntelHDFix(player, 1, m_iIntelHDFixModel, Vector(10, 0, 20)));
			m_flNextIntelHDFix = time + 0.9f;
		}

		if (!CL_IsThirdPerson())
			m_bSkipViewModel = true;

		g_pEngineClient->RenderView(params, false, false, 1);

		GLint activeTexture;

		glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);
		if (activeTexture == GL_TEXTURE0 || SetActiveTexture(GL_TEXTURE0))
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, source->m_iTexture);
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0,
				(int)(m_SavedViewport[3] - source->m_iHeight), source->m_iWidth, source->m_iHeight);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else
		{
			PortalWarning("Invalid GL_ACTIVE_TEXTURE, unable to reset. Portal not drawn.\n");
		}

		if (activeTexture != GL_TEXTURE0)
			SetActiveTexture(activeTexture);

		m_vecSavedViewOrigin.CopyToArray(params->vieworg);
		m_vecSavedViewAngles.CopyToArray(params->viewangles);
		m_vecSavedForward.CopyToArray(params->forward);
		m_vecSavedRight.CopyToArray(params->right);
		m_vecSavedUp.CopyToArray(params->up);

		for (int j = 0; j < 4; j++)
			params->viewport[j] = (int)m_SavedViewport[j];
	}

	DisableClipPlanes();
	glClearColor(0, 0, 0, 0);
	g_bRenderingPortals = false;
	g_pEngineClient->PopView();
}

bool ClientPortalManager::DrawPortals(void)
{
	if ((m_PortalSettings.m_bDisabled && m_MirrorSettings.m_bDisabled && m_MonitorSettings.m_bDisabled)
		|| g_bRenderingPortals || m_Portals.empty())
		return false;

	GLint activeTexture;

	glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);
	if (activeTexture != GL_TEXTURE0 && !SetActiveTexture(GL_TEXTURE0))
	{
		PortalWarning("Invalid GL_ACTIVE_TEXTURE, unable to reset. Portals will not be drawn.\n");
		return false;
	}

	gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
	glColor4f(1, 1, 1, 1);
	glDisable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);
	EnableShader();

	for (auto &entry : m_Portals)
	{
		ClientPortal &portal = entry.second;

		if (!portal.IsVisible(this, m_iFrame, m_iVisFrame, m_vecSavedViewOrigin, m_Frustum))
			continue;

		for (auto surface : portal.m_PortalSurfaces)
		{
			if (portal.IsSurfaceVisible(this, m_iFrame, m_iVisFrame,
				m_vecSavedForward, m_vecSavedViewOrigin, m_Frustum, surface))
				DrawPortalSurface(portal, surface, portal.LookupTexture(surface));
		}
	}

	DisableShader();

	if (activeTexture != GL_TEXTURE0)
		SetActiveTexture(activeTexture);

	g_pEngineClient->PopView();
	return true;
}

void DrawPortalPolygon(ClientPortal &portal, msurface_t *surface, int textureCoordinates)
{
	glpoly_s *poly = surface->polys;

	glBegin(GL_POLYGON);

	for (int i = 0; i < poly->numverts; i++)
	{
		Vector vertex(poly->verts[i]);

		if (textureCoordinates)
			glTexCoord2f(poly->verts[i][3], textureCoordinates < 0 ? -poly->verts[i][4] : poly->verts[i][4]);

		RotateVector(vertex, portal.GetAngles(), Vector(0, 0, 0));
		vertex = vertex + portal.GetOrigin();
		glVertex3fv(vertex);
	}

	glEnd();
}

void DrawMonitor(ClientPortal &portal, msurface_t *surface, unsigned int texture)
{
	glDisable(GL_ALPHA_TEST);
	glBindTexture(GL_TEXTURE_2D, texture);
	DrawPortalPolygon(portal, surface, -1);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void DrawStencil(ClientPortal &portal, msurface_t *surface, unsigned int texture)
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 3, 255);
	glStencilOp(GL_ZERO, GL_ZERO, GL_REPLACE);
	glClear(GL_STENCIL_BUFFER_BIT);
	glColor4f(0, 0, 0, 0);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	DrawPortalPolygon(portal, surface, 0);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_TEXTURE_2D);
	glDepthMask(GL_TRUE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

void DrawPortalOrMirrorImageWhereStencilIsOne(ClientPortal &portal, msurface_t *surface, unsigned int texture)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 1, 1, 0, 0.1, 100);
	glDepthMask(GL_FALSE);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5f);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_LEQUAL, 1, 255);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_POLYGON);

	float left = portal.GetPortalMode() == PORTAL_MIRROR ? 1 : 0;
	float right = 1 - left;

	glTexCoord2f(left, 0);
	glVertex3f(0, 1, -1);
	glTexCoord2f(left, 1);
	glVertex3f(0, 0, -1);
	glTexCoord2f(right, 1);
	glVertex3f(1, 0, -1);
	glTexCoord2f(right, 0);
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
}

void DrawDepth(ClientPortal &portal, msurface_t *surface, unsigned int texture)
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_EQUAL, 1, 255);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	DrawPortalPolygon(portal, surface, 0);
	glDisable(GL_STENCIL_TEST);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glEnable(GL_TEXTURE_2D);
}

void DrawOverlay(ClientPortal &portal, msurface_t *surface, unsigned int texture)
{
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5f);
	glBindTexture(GL_TEXTURE_2D, texture);
	DrawPortalPolygon(portal, surface, 1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_ALPHA_TEST);
}

void ClientPortalManager::DrawPortalSurface(ClientPortal &portal, msurface_t *surface, unsigned int texture)
{
	if (portal.GetPortalMode() == PORTAL_MONITOR)
	{
		DrawMonitor(portal, surface, texture);
	}
	else
	{
		DrawStencil(portal, surface, texture);
		DrawPortalOrMirrorImageWhereStencilIsOne(portal, surface, texture);
		DrawDepth(portal, surface, texture);
	}

	mtexinfo_t *original = GetOriginalSurfaceTexture(surface);

	if (original && original->texture && original->texture->name[0] == '{')
		DrawOverlay(portal, surface, original->texture->gl_texturenum);
}
