#ifndef PARTICLE_ENGINE_H
#define PARTICLE_ENGINE_H

#include "particle_system.h"

struct cl_entity_s;

struct particle_script_effect_s
{
	char						name[260];
	signed char					attachment;
	int							bone;
	char						sprite[260];
	unsigned char				scale;
	unsigned char				colorR;
	unsigned char				colorB;
	unsigned char				colorG;
	int							rendermode;
	int							transparency;
	Vector						offset;
	particle_script_effect_s	*next;
};

class CParticleEngine
{
public:
	CParticleEngine();
	~CParticleEngine();

	void EngineThink(void);
	CParticleSystem *SystemCreate(void);
	void AddSystem(CParticleSystem *system);
	void AddMuzzleEffect(particle_script_effect_s *effect);
	void ClearMuzzleEffectList(void);
	void ClearAllSystems(void);
	bool PlayMuzzleScriptByName(cl_entity_s *entity, const char *name);
	void UpdateFrametime(void);

	int							m_iParticles;
	float						m_flFrameTime;
	float						m_flLastTime;
	bool						m_bInitialized;
	CParticleSystem				*m_pSystems;
	particle_script_effect_s	*m_pMuzzleEffects;
};

CParticleEngine *ParticleEngine(void);

#endif
