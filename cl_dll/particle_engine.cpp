#include "hud.h"
#include "cl_util.h"
#include "particle_engine.h"

#include <SDL2/SDL_opengl.h>

cvar_t *cl_particles_max;
cvar_t *cl_texture_lod;

static CParticleEngine *g_pParticleEngine;

CParticleEngine *ParticleEngine(void)
{
	if (!g_pParticleEngine)
		g_pParticleEngine = new CParticleEngine;

	return g_pParticleEngine;
}

CParticleEngine::CParticleEngine()
{
	m_iParticles = 0;
	m_flFrameTime = 0;
	m_flLastTime = 0;
	m_bInitialized = false;
	m_pSystems = NULL;
	m_pMuzzleEffects = NULL;
}

CParticleEngine::~CParticleEngine()
{
	ClearAllSystems();
}

void CParticleEngine::UpdateFrametime(void)
{
	m_flFrameTime = gEngfuncs.GetClientTime() - m_flLastTime;
	m_flLastTime = gEngfuncs.GetClientTime();
}

void CParticleEngine::EngineThink(void)
{
	glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, cl_texture_lod->value);

	if (!m_bInitialized)
	{
		ClearAllSystems();
		m_bInitialized = true;
		return;
	}

	UpdateFrametime();
	m_iParticles = 0;

	CParticleSystem *system = m_pSystems;
	CParticleSystem *previous = NULL;

	while (system)
	{
		if (system->m_bActive)
		{
			system->SystemThink();
			m_iParticles += system->m_iParticleCount;
			previous = system;
			system = system->m_pNext;
		}
		else
		{
			CParticleSystem *removed = system;

			system = system->m_pNext;

			if (previous)
				previous->m_pNext = system;
			else
				m_pSystems = system;

			removed->SystemDie();
			delete removed;
		}
	}
}

CParticleSystem *CParticleEngine::SystemCreate(void)
{
	CParticleSystem *system = new CParticleSystem;

	AddSystem(system);
	return system;
}

void CParticleEngine::AddSystem(CParticleSystem *system)
{
	if (!system)
		return;

	system->m_pNext = m_pSystems;
	m_pSystems = system;
}

void CParticleEngine::AddMuzzleEffect(particle_script_effect_s *effect)
{
	if (!effect)
		return;

	effect->next = m_pMuzzleEffects;
	m_pMuzzleEffects = effect;
}

void CParticleEngine::ClearMuzzleEffectList(void)
{
	particle_script_effect_s *effect = m_pMuzzleEffects;

	while (effect)
	{
		particle_script_effect_s *removed = effect;

		effect = effect->next;
		delete removed;
	}

	m_pMuzzleEffects = NULL;
}

void CParticleEngine::ClearAllSystems(void)
{
	CParticleSystem *system = m_pSystems;

	while (system)
	{
		CParticleSystem *removed = system;

		system = system->m_pNext;
		removed->SystemDie();
		delete removed;
	}

	m_pSystems = NULL;
	ClearMuzzleEffectList();
}
