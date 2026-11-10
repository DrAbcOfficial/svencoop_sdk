#include "hud.h"
#include "cl_util.h"
#include "particle_engine.h"
#include "camera.h"
#include "com_model.h"
#include "r_studioint.h"
#include "event_api.h"
#include "triangleapi.h"
#include "pmtrace.h"
#include "pm_defs.h"
#include "IClient.h"

#include <SDL2/SDL_opengl.h>
#include <math.h>
#include <string.h>

#ifndef SURF_DRAWSKY
#define SURF_DRAWSKY	4
#define SURF_DRAWTURB	0x10
#endif

extern cvar_t *cl_particles_max;
extern engine_studio_api_t IEngineStudio;
bool IsThirdPerson(void);

CParticleUnit::CParticleUnit()
{
	m_vecUnknown0 = m_vecOrigin = m_vecAngles = m_vecUnknown36 = Vector(0, 0, 0);
	m_vecVelocity = m_vecExtraVelocity = Vector(0, 0, 0);
	m_flExtraVelocityUntil = m_flExtraVelocityTime = 0;
	m_flUnknown80 = m_flUnknown84 = m_flUnknown88 = 0;
	m_flEffectTime = m_flDieTime = m_flLifeTime = 0;
	m_flSize = m_flSizeSpeed = m_flSizeTarget = 0;
	m_vecColor = Vector(0, 0, 0);
	m_flAlpha = 0;
	m_iRenderMode = m_iFlags = 0;
	m_bActive = false;
	m_pszSprite = NULL;
	m_pSprite = NULL;
	m_flFrame = m_flFrameRate = 0;
	m_pNext = NULL;
}

CParticleUnit::CParticleUnit(const CParticleUnit &particle)
{
	*this = particle;
}

CParticleUnit &CParticleUnit::operator=(const CParticleUnit &particle)
{
	m_vecUnknown0 = particle.m_vecUnknown0;
	m_vecOrigin = particle.m_vecOrigin;
	m_vecAngles = particle.m_vecAngles;
	m_vecUnknown36 = particle.m_vecUnknown36;
	m_vecVelocity = particle.m_vecVelocity;
	m_vecExtraVelocity = particle.m_vecExtraVelocity;
	m_flExtraVelocityUntil = particle.m_flExtraVelocityUntil;
	m_flExtraVelocityTime = particle.m_flExtraVelocityTime;
	m_flUnknown80 = particle.m_flUnknown80;
	m_flUnknown84 = particle.m_flUnknown84;
	m_flUnknown88 = particle.m_flUnknown88;
	m_flEffectTime = particle.m_flEffectTime;
	m_flDieTime = particle.m_flDieTime;
	m_flLifeTime = particle.m_flLifeTime;
	m_flSize = particle.m_flSize;
	m_flSizeSpeed = particle.m_flSizeSpeed;
	m_flSizeTarget = particle.m_flSizeTarget;
	m_vecColor = particle.m_vecColor;
	m_flAlpha = particle.m_flAlpha;
	m_iRenderMode = particle.m_iRenderMode;
	m_iFlags = particle.m_iFlags;
	m_bActive = particle.m_bActive;
	m_pszSprite = particle.m_pszSprite;
	m_pSprite = particle.m_pSprite;
	m_flFrame = particle.m_flFrame;
	m_flFrameRate = particle.m_flFrameRate;
	m_pNext = particle.m_pNext;
	return *this;
}

CParticleSystem::CParticleSystem()
{
	m_vecOrigin = m_vecDirection = m_vecUnknown36 = Vector(0, 0, 0);
	m_pParticles = NULL;
	m_iParticleCount = 0;
	m_iMaxParticles = 0;
	m_iFlags = 0;
}

CParticleSystem::~CParticleSystem()
{
	SystemDie();
}

CParticleSparks::~CParticleSparks()
{
}

void CParticleBloodSplurt::SystemSpawn(void)
{
	m_iFlags = PSYS_GRAVITY;
	Precache();
	AddParticles();

	m_bActive = true;
	m_flDieTime = gEngfuncs.GetClientTime() + 10.0f;
}

void CParticleBloodSplurt::Precache(void)
{
	m_pDropSprite = (model_s *)g_pEngineClient->LoadClientModel("blooddrop.spr");
}

void CParticleBloodSplurt::AddParticles(void)
{
	m_pszSprite = "sprites/wep_smoke_01.spr";
	m_iMaxParticles = 30;
	m_flSpawnInterval = 0.01f;
	m_iFlags = PSYS_COLLIDE | PSYS_GRAVITY;

	ParticleAdd(DefaultParticle());
	m_flNextSpawn = gEngfuncs.GetClientTime() + m_flSpawnInterval;
}

CParticleUnit CParticleBloodSplurt::DefaultParticle(void)
{
	CParticleUnit particle;

	particle.m_vecUnknown0 = m_vecOrigin;
	particle.m_vecOrigin = particle.m_vecUnknown0;
	particle.m_iRenderMode = kRenderTransAlpha;
	particle.m_flSize = gEngfuncs.pfnRandomLong(0, 2);
	particle.m_pszSprite = m_pszSprite;
	particle.m_flFrame = 0;
	particle.m_flAlpha = 255;
	particle.m_vecColor = Vector(gEngfuncs.pfnRandomLong(120, 255), 0, 0);
	particle.m_vecVelocity = m_vecDirection * 100;
	particle.m_flDieTime = gEngfuncs.GetClientTime() + 5.0f;
	particle.m_flLifeTime = 5.0f;
	particle.m_flSizeTarget = 30.0f;
	particle.m_flSizeSpeed = 15.0f;
	return particle;
}

void CParticleBloodSplurt::UpdateOrientation(void)
{
	cl_entity_t *entity = gEngfuncs.GetEntityByIndex(m_iEntityIndex);

	if (entity)
		m_vecOrigin = entity->origin + m_vecUnknown36;
}

void CParticleBloodSplurt::SystemSpecific(CParticleUnit *particle)
{
	UpdateOrientation();
}

void CParticleSmoke::SystemSpawn(void)
{
	m_iFlags = 0;
	Precache();
	AddParticles();
}

void CParticleSmoke::Precache(void)
{
	m_pSmokeSprite = (model_s *)g_pEngineClient->LoadClientModel("poop.spr");
}

void CParticleSmoke::AddParticles(void)
{
}

void CParticleSmoke::SystemSpecific(CParticleUnit *particle)
{
}

void CParticleSystem::SystemThink(void)
{
	if (m_flDieTime > (float)gEngfuncs.GetClientTime() &&
		m_iParticleCount < m_iMaxParticles &&
		(float)gEngfuncs.GetClientTime() > m_flNextSpawn && m_flSpawnInterval != 0)
	{
		CParticleUnit particle = m_DefaultParticle;

		particle.m_vecOrigin = m_vecOrigin;
		particle.m_flExtraVelocityUntil = gEngfuncs.GetClientTime() + particle.m_flExtraVelocityTime;
		particle.m_flDieTime = gEngfuncs.GetClientTime() + particle.m_flLifeTime;
		m_flNextSpawn = gEngfuncs.GetClientTime() + m_flSpawnInterval;

		if (m_iType == PSYS_TINTED_SMOKE)
		{
			float y = m_vecDirection.y + gEngfuncs.pfnRandomFloat(-1, 1);
			float x = m_vecDirection.x + gEngfuncs.pfnRandomFloat(-1, 1);

			particle.m_vecVelocity = Vector(x * 64, y * 64, m_vecDirection.z * 64);
		}

		ParticleAdd(particle);
	}

	CParticleUnit *particle = m_pParticles;
	CParticleUnit *previous = NULL;

	while (particle)
	{
		if ((float)gEngfuncs.GetClientTime() > particle->m_flDieTime)
			particle->m_bActive = false;

		if (particle->m_bActive)
		{
			ParticleUpdate(particle);
			ParticleDraw(particle);
			previous = particle;
			particle = particle->m_pNext;
		}
		else
		{
			CParticleUnit *removed = particle;

			particle = particle->m_pNext;

			if (previous)
				previous->m_pNext = particle;
			else
				m_pParticles = particle;

			delete removed;
			m_iParticleCount--;
		}
	}

	if ((float)gEngfuncs.GetClientTime() > m_flDieTime && m_iParticleCount <= 0)
		m_bActive = false;
}

void CParticleSystem::SystemDie(void)
{
	CParticleUnit *particle = m_pParticles;

	while (particle)
	{
		CParticleUnit *removed = particle;

		particle = particle->m_pNext;
		delete removed;
	}

	m_pParticles = NULL;
}

void CParticleSystem::ParticleAdd(CParticleUnit particle)
{
	if (!m_pParticles)
		memcpy(&m_DefaultParticle, &particle, sizeof(particle));

	particle.m_bActive = true;

	CParticleUnit *added = new CParticleUnit;

	memcpy(added, &particle, sizeof(particle));
	added->m_pNext = m_pParticles;
	m_pParticles = added;

	if ((m_iType == PSYS_TINTED_SMOKE || m_iType == PSYS_GUNSHOT_SMOKE) && !(m_iParticleCount & 1))
		added->m_iFlags |= PARTICLE_ALTERNATE;

	m_iParticleCount++;
}

void CParticleSystem::ParticleUpdate(CParticleUnit *particle)
{
	float frameTime = ParticleEngine()->m_flFrameTime;

	if (frameTime <= 0)
		return;

	Vector oldOrigin = particle->m_vecOrigin;

	if ((m_iFlags & PSYS_GRAVITY) || (particle->m_iFlags & PARTICLE_GRAVITY))
		particle->m_vecVelocity.z -= frameTime * 800;

	if (particle->m_flExtraVelocityUntil <= (float)gEngfuncs.GetClientTime())
		particle->m_vecOrigin = oldOrigin + particle->m_vecVelocity * frameTime;
	else
	{
		float factor = particle->m_flExtraVelocityUntil / gEngfuncs.GetClientTime();

		particle->m_vecOrigin = oldOrigin +
			(particle->m_vecVelocity + particle->m_vecExtraVelocity) * factor * frameTime;
	}

	ParticleCollision(particle, oldOrigin);
}

void CParticleSystem::ParticleCollision(CParticleUnit *particle, Vector oldOrigin)
{
	gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/laserbeam.spr");

	if (!(m_iFlags & PSYS_COLLIDE))
		return;

	pmtrace_t trace = {};

	gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	gEngfuncs.pEventAPI->EV_PlayerTrace(oldOrigin, particle->m_vecOrigin, PM_STUDIO_BOX, -1, &trace);

	if (gEngfuncs.PM_PointContents(oldOrigin, NULL) != CONTENTS_SOLID && trace.fraction != 1)
	{
		float fraction = (float)(trace.fraction - 0.1) * ParticleEngine()->m_flFrameTime;

		particle->m_vecOrigin = oldOrigin + particle->m_vecVelocity * fraction;

		float dot = DotProduct(particle->m_vecVelocity, trace.plane.normal);
		float scale = gEngfuncs.pfnRandomFloat(2.0f, 1.5f) * -dot;

		particle->m_vecVelocity = (Vector(trace.plane.normal) * scale + particle->m_vecVelocity) * 0.5f;
	}
}

bool CParticleSystem::ParticleIsVisible(CParticleUnit *particle)
{
	Vector screen;

	return gEngfuncs.pTriAPI->WorldToScreen(particle->m_vecOrigin, screen) != 1;
}

void CParticleSystem::SystemSpecific(CParticleUnit *particle)
{
	float frameTime = ParticleEngine()->m_flFrameTime;

	switch (m_iType)
	{
	case PSYS_GUNSHOT_SMOKE:
	case PSYS_WEAPON_SMOKE:
	case PSYS_EXPLOSION:
	case PSYS_TINTED_SMOKE:
	{
		if (particle->m_iFlags & PARTICLE_ALTERNATE)
			particle->m_vecAngles.z -= frameTime * 64.0;
		else
			particle->m_vecAngles.z += frameTime * 64.0;

		if (particle->m_flSizeTarget <= 0)
			particle->m_flSize = fmaxf(particle->m_flSizeTarget,
				particle->m_flSize - particle->m_flSizeSpeed * frameTime);
		else
			particle->m_flSize = fminf(particle->m_flSizeTarget,
				particle->m_flSize + particle->m_flSizeSpeed * frameTime);

		particle->m_flFrame = fminf(100, particle->m_flFrame + frameTime * particle->m_flFrameRate);

		double alphaSpeed;

		if (m_iType == PSYS_TINTED_SMOKE)
			alphaSpeed = particle->m_flAlpha <= 100 ? 85.0 : 1350.0;
		else
			alphaSpeed = particle->m_flAlpha <= 100 ? 150.0 : 350.0;

		particle->m_flAlpha = fmax(0.0, particle->m_flAlpha - frameTime * alphaSpeed);

		if (particle->m_flAlpha == 0)
			particle->m_bActive = false;

		if ((float)gEngfuncs.GetClientTime() >= particle->m_flExtraVelocityUntil)
		{
			particle->m_vecVelocity.x += gEngfuncs.pfnRandomFloat(100, -100) * frameTime;
			particle->m_vecVelocity.y += gEngfuncs.pfnRandomFloat(100, -100) * frameTime;
			particle->m_vecVelocity.z += gEngfuncs.pfnRandomFloat(100, -100) * frameTime;
		}
		break;
	}
	case PSYS_GUNSHOT_DEBRIS:
		if (particle->m_vecVelocity.Length2D() > 32 && !(particle->m_iFlags & PARTICLE_NOSPIN))
			particle->m_vecAngles.z += frameTime * 2560.0f;
		break;

	case PSYS_WATER_SPLASH:
		if (particle->m_iFlags & PARTICLE_ALTERNATE)
		{
			particle->m_vecAngles.x = 90;
			particle->m_vecAngles.y = 0;
			particle->m_flSize += frameTime * 8.0;
			particle->m_flAlpha = fmax(0.0, particle->m_flAlpha - frameTime * 64.0);
		}
		else if (particle->m_iFlags & PARTICLE_DROPLET)
		{
			particle->m_vecAngles.z += frameTime * 64.0;

			if (particle->m_vecVelocity.z < 0)
				particle->m_flAlpha = fmax(0.0, particle->m_flAlpha - frameTime * 640.0);
		}
		else
		{
			if (particle->m_vecVelocity.z < 0 && (float)gEngfuncs.GetClientTime() > particle->m_flEffectTime)
				particle->m_flAlpha = fmax(0.0, particle->m_flAlpha - frameTime * 640.0);

			particle->m_vecAngles.x = 0;
			particle->m_vecAngles.z = 90;
			particle->m_flSize += frameTime * 28.0;
		}
		break;

	case PSYS_VIEW_SMOKE:
	{
		cl_entity_t *player = gEngfuncs.GetLocalPlayer();
		Vector forward, right, up;

		AngleVectors(player->curstate.angles, forward, right, up);
		m_vecOrigin = Vector(player->origin) + forward * particle->m_flEffectTime;
		particle->m_vecOrigin = m_vecOrigin;
		break;
	}
	}
}

void ParticleRenderMode(CParticleUnit *particle)
{
	cvar_t *mode = gEngfuncs.pfnGetCvarPointer("psys_rendermode");

	if (!strncmp(mode->string, "0", 1))
		gEngfuncs.pTriAPI->RenderMode(particle->m_iRenderMode);
	else
		gEngfuncs.pTriAPI->RenderMode(atoi(mode->string));
}

void ParticleQuad(Vector origin, Vector right, Vector up, float width, float height)
{
	Vector vertex;

	gEngfuncs.pTriAPI->TexCoord2f(0, 0);
	vertex = origin + up * height + right * -width;
	gEngfuncs.pTriAPI->Vertex3fv(vertex);
	gEngfuncs.pTriAPI->TexCoord2f(0, 1);
	vertex = origin + up * height + right * width;
	gEngfuncs.pTriAPI->Vertex3fv(vertex);
	gEngfuncs.pTriAPI->TexCoord2f(1, 1);
	vertex = origin + up * -height + right * width;
	gEngfuncs.pTriAPI->Vertex3fv(vertex);
	gEngfuncs.pTriAPI->TexCoord2f(1, 0);
	vertex = origin + up * -height + right * -width;
	gEngfuncs.pTriAPI->Vertex3fv(vertex);
}

bool CParticleSystem::SystemSpecificDraw(CParticleUnit *particle)
{
	if (m_iType == PSYS_STREAK)
	{
		Vector angles, forward, right, up;
		Vector origin = particle->m_vecOrigin;

		gEngfuncs.GetViewAngles(angles);
		particle->m_vecAngles.x = angles.x;
		particle->m_vecAngles.y = angles.y;
		SystemSpecific(particle);
		ParticleRenderMode(particle);
		gEngfuncs.pTriAPI->CullFace(TRI_FRONT);
		gEngfuncs.pTriAPI->Begin(TRI_TRIANGLE_FAN);
		gEngfuncs.pTriAPI->Color4ub((byte)(int)particle->m_vecColor.x,
			(byte)(int)particle->m_vecColor.y, (byte)(int)particle->m_vecColor.z,
			(byte)(int)(particle->m_flAlpha / 255.0f));
		gEngfuncs.pTriAPI->Brightness(255);
		gEngfuncs.GetLocalPlayer();

		Vector direction = particle->m_vecVelocity.Normalize();

		AngleVectors(direction, forward, right, up);

		Vector vertex;

		gEngfuncs.pTriAPI->TexCoord2f(0, 0);
		vertex = origin + up + forward * particle->m_vecVelocity.Length() - right * 4;
		gEngfuncs.pTriAPI->Vertex3fv(vertex);
		gEngfuncs.pTriAPI->TexCoord2f(0, 1);
		vertex = origin + up + forward * particle->m_vecVelocity.Length() + right * 4;
		gEngfuncs.pTriAPI->Vertex3fv(vertex);
		gEngfuncs.pTriAPI->TexCoord2f(1, 1);
		vertex = origin - up - forward * 10 + right * 4;
		gEngfuncs.pTriAPI->Vertex3fv(vertex);
		gEngfuncs.pTriAPI->TexCoord2f(1, 0);
		vertex = origin - up - forward * 10 - right * 4;
		gEngfuncs.pTriAPI->Vertex3fv(vertex);
		gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
		gEngfuncs.pTriAPI->End();
		return true;
	}

	if (m_iType != PSYS_WATER_SPLASH)
		return false;

	Vector angles, forward, right, up;
	Vector origin = particle->m_vecOrigin;

	gEngfuncs.GetViewAngles(angles);

	if (IsThirdPerson())
	{
		angles.x = cam_ofs[0];
		angles.y = cam_ofs[1];
	}

	particle->m_vecAngles.x = angles.x;
	particle->m_vecAngles.y = angles.y;
	SystemSpecific(particle);
	AngleVectors(particle->m_vecAngles, forward, right, up);

	if (!ParticleIsVisible(particle))
		return true;

	ParticleRenderMode(particle);
	gEngfuncs.pTriAPI->CullFace(TRI_FRONT);
	gEngfuncs.pTriAPI->Begin(TRI_TRIANGLE_FAN);
	gEngfuncs.pTriAPI->Color4f(particle->m_vecColor.x / 255.0f,
		particle->m_vecColor.y / 255.0f, particle->m_vecColor.z / 255.0f,
		particle->m_flAlpha / 255.0f);

	float height = particle->m_flSize;

	if (!(particle->m_iFlags & PARTICLE_ALTERNATE))
		height *= 0.5f;

	ParticleQuad(origin, right, up, particle->m_flSize, height);
	gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
	gEngfuncs.pTriAPI->End();
	return true;
}

color24 RecursiveLightPoint(mnode_t *node, Vector start, Vector end)
{
	model_t *world = IEngineStudio.GetModelByIndex(1);
	color24 color = {};

	if (node->contents < 0 || !node->plane)
		return color;

	float front = DotProduct(start, node->plane->normal) - node->plane->dist;
	float back = DotProduct(end, node->plane->normal) - node->plane->dist;
	int side = front < 0;

	if ((back < 0) == side)
		return RecursiveLightPoint(node->children[side], start, end);

	float fraction = front / (front - back);
	Vector mid = start + (end - start) * fraction;

	color = RecursiveLightPoint(node->children[side], start, mid);

	if (color.r || color.g || color.b)
		return color;

	msurface_t *surface = world->surfaces + node->firstsurface;

	for (int i = 0; i < node->numsurfaces; i++, surface++)
	{
		if (surface->flags & (SURF_DRAWSKY | SURF_DRAWTURB))
			continue;

		mtexinfo_t *tex = surface->texinfo;
		int s = (int)(DotProduct(mid, tex->vecs[0]) + tex->vecs[0][3]);
		int t = (int)(DotProduct(mid, tex->vecs[1]) + tex->vecs[1][3]);

		if (s < surface->texturemins[0] || t < surface->texturemins[1])
			continue;

		s -= surface->texturemins[0];
		t -= surface->texturemins[1];

		if (s > surface->extents[0] || t > surface->extents[1])
			continue;

		if (!surface->samples)
			return color;

		int width = (surface->extents[0] >> 4) + 1;
		int size = width * ((surface->extents[1] >> 4) + 1);
		color24 *sample = surface->samples + (s >> 4) + (t >> 4) * width;

		for (int style = 0; style < MAXLIGHTMAPS && surface->styles[style] != 255; style++)
		{
			color.r += sample->r;
			color.g += sample->g;
			color.b += sample->b;
			sample += size;
		}

		return color;
	}

	return RecursiveLightPoint(node->children[!side], mid, end);
}

color24 R_LightPoint(Vector point)
{
	model_t *world = IEngineStudio.GetModelByIndex(1);

	if (!world->lightdata)
	{
		color24 fullbright = {255, 255, 255};

		return fullbright;
	}

	return RecursiveLightPoint(world->nodes, point, point - Vector(0, 0, 2048));
}

void CParticleSystem::ParticleDraw(CParticleUnit *particle)
{
	if (cl_particles_max &&
		(float)(ParticleEngine()->m_iParticles + m_iParticleCount) > cl_particles_max->value)
	{
		return;
	}

	if (particle->m_pSprite)
	{
		if (!gEngfuncs.pTriAPI->SpriteTexture(particle->m_pSprite, (int)particle->m_flFrame))
			return;
	}
	else
	{
		cvar_t *sprite = gEngfuncs.pfnGetCvarPointer("psys_spritename");
		const char *name = !strncmp(sprite->string, "0", 1) ? particle->m_pszSprite : sprite->string;

		particle->m_pSprite = (model_t *)g_pEngineClient->LoadClientModel(name);

		if (!particle->m_pSprite || !gEngfuncs.pTriAPI->SpriteTexture(particle->m_pSprite, (int)particle->m_flFrame))
		{
			gEngfuncs.Con_Printf("Particle_Engine: Couldn't get sprite pointer for \"%s\"!\n", name);
			return;
		}
	}

	if (SystemSpecificDraw(particle))
		return;

	Vector angles, forward, right, up;
	Vector origin = particle->m_vecOrigin;

	gEngfuncs.GetViewAngles(angles);

	if (IsThirdPerson())
	{
		angles.x = cam_ofs[0];
		angles.y = cam_ofs[1];
	}

	particle->m_vecAngles.x = angles.x;
	particle->m_vecAngles.y = angles.y;
	SystemSpecific(particle);
	AngleVectors(particle->m_vecAngles, forward, right, up);

	if (!ParticleIsVisible(particle))
		return;

	if (particle->m_iFlags & PARTICLE_NODEPTH)
		glDisable(GL_DEPTH_TEST);

	ParticleRenderMode(particle);
	glBegin(GL_TRIANGLE_FAN);

	Vector color = particle->m_vecColor;

	if (particle->m_iFlags & PARTICLE_LIGHTING)
	{
		color24 light = R_LightPoint(particle->m_vecOrigin + Vector(0, 0, 2));

		color.x = (color.x / 255.0f) * light.r;
		color.y = (color.y / 255.0f) * light.g;
		color.z = (color.z / 255.0f) * light.b;
	}

	glColor4f(color.x / 255.0f, color.y / 255.0f, color.z / 255.0f,
		fminf(particle->m_flAlpha, 255) / 255.0f);
	ParticleQuad(origin, right, up, particle->m_flSize, particle->m_flSize);
	gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
	glEnd();

	if (particle->m_iFlags & PARTICLE_NODEPTH)
		glEnable(GL_DEPTH_TEST);
}
