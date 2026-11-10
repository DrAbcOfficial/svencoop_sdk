#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#ifndef DID_VEC3_T_DEFINE
#include "util_vector.h"
#endif

struct model_s;
struct particle_script_effect_s;

// CParticleSystem::SystemStartup types
enum
{
	PSYS_GUNSHOT_SMOKE = 0,
	PSYS_WEAPON_SMOKE,
	PSYS_UNUSED2,
	PSYS_GUNSHOT_DEBRIS,
	PSYS_STREAK,
	PSYS_UNUSED5,
	PSYS_WATER_SPLASH,
	PSYS_SCRIPT_WORLD,
	PSYS_SCRIPT_VIEWMODEL,
	PSYS_VIEW_SMOKE,
	PSYS_EXPLOSION,
	PSYS_TINTED_SMOKE
};

// CParticleSystem::m_iFlags
#define PSYS_COLLIDE				( 1<<4 )
#define PSYS_GRAVITY				( 1<<5 )

// CParticleUnit::m_iFlags
#define PARTICLE_ALTERNATE			( 1<<0 )	// reverse spin, or the flat ring of a water splash
#define PARTICLE_DROPLET			( 1<<3 )
#define PARTICLE_GRAVITY			( 1<<5 )
#define PARTICLE_NOSPIN				( 1<<6 )
#define PARTICLE_LIGHTING			( 1<<7 )	// tint by the world lightmap below the particle
#define PARTICLE_NODEPTH			( 1<<8 )

class CParticleUnit
{
public:
	CParticleUnit();
	CParticleUnit(const CParticleUnit &particle);
	CParticleUnit &operator=(const CParticleUnit &particle);

	Vector			m_vecUnknown0;
	Vector			m_vecOrigin;
	Vector			m_vecAngles;
	Vector			m_vecUnknown36;
	Vector			m_vecVelocity;
	Vector			m_vecExtraVelocity;
	float			m_flExtraVelocityUntil;
	float			m_flExtraVelocityTime;
	float			m_flUnknown80;
	float			m_flUnknown84;
	float			m_flUnknown88;
	float			m_flEffectTime;
	float			m_flDieTime;
	float			m_flLifeTime;
	float			m_flSize;
	float			m_flSizeSpeed;
	float			m_flSizeTarget;
	Vector			m_vecColor;
	float			m_flAlpha;
	int				m_iRenderMode;
	int				m_iFlags;
	bool			m_bActive;
	const char		*m_pszSprite;
	model_s			*m_pSprite;
	float			m_flFrame;
	float			m_flFrameRate;
	CParticleUnit	*m_pNext;
};

class CParticleSystem
{
public:
	CParticleSystem();
	~CParticleSystem();

	virtual void SystemStartup(int type, particle_script_effect_s *effect);
	virtual void SystemThink(void);
	virtual void SystemDie(void);
	virtual void SystemSpecific(CParticleUnit *particle);
	virtual bool SystemSpecificDraw(CParticleUnit *particle);
	virtual void ParticleAdd(CParticleUnit particle);
	virtual void ParticleUpdate(CParticleUnit *particle);
	virtual void ParticleDraw(CParticleUnit *particle);
	virtual bool ParticleIsVisible(CParticleUnit *particle);
	virtual void ParticleCollision(CParticleUnit *particle, Vector oldOrigin);

	bool			m_bActive;
	float			m_flDieTime;
	Vector			m_vecOrigin;
	Vector			m_vecDirection;
	Vector			m_vecUnknown36;
	float			m_flNextSpawn;
	float			m_flSpawnInterval;
	const char		*m_pszSprite;
	int				m_iParticleCount;
	int				m_iType;
	int				m_iFlags;
	CParticleSystem	*m_pNext;
	CParticleUnit	*m_pParticles;
	CParticleUnit	m_DefaultParticle;
	int				m_iMaxParticles;
};

class CParticleSparks : public CParticleSystem
{
public:
	~CParticleSparks();
};

class CParticleBloodSplurt : public CParticleSystem
{
public:
	void SystemSpawn(void);
	void Precache(void);
	void AddParticles(void);
	CParticleUnit DefaultParticle(void);
	void UpdateOrientation(void);
	virtual void SystemSpecific(CParticleUnit *particle);

	int		m_iEntityIndex;
	model_s	*m_pDropSprite;
};

class CParticleSmoke : public CParticleSystem
{
public:
	void SystemSpawn(void);
	void Precache(void);
	void AddParticles(void);
	virtual void SystemSpecific(CParticleUnit *particle);

	model_s	*m_pSmokeSprite;
};

#endif
