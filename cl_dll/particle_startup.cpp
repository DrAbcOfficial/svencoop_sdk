#include "hud.h"
#include "cl_util.h"
#include "cl_entity.h"
#include "particle_engine.h"

void CParticleSystem::SystemStartup(int type, particle_script_effect_s *effect)
{
	CParticleUnit particle;

	m_iType = type;

	switch (type)
	{
	case PSYS_GUNSHOT_SMOKE:
	case PSYS_WEAPON_SMOKE:
		m_pszSprite = type == PSYS_WEAPON_SMOKE ? "sprites/wep_smoke_01.spr" : "sprites/puff1.spr";
		m_iMaxParticles = 3;
		m_flSpawnInterval = 0;
		particle.m_vecUnknown0 = m_vecOrigin;
		particle.m_vecOrigin = m_vecOrigin;
		particle.m_iFlags = PARTICLE_LIGHTING;
		particle.m_iRenderMode = kRenderTransAlpha;
		particle.m_pszSprite = m_pszSprite;
		particle.m_flFrame = 0;
		particle.m_flSize = type == PSYS_WEAPON_SMOKE ? 0.1f : 1.0f;
		particle.m_flAlpha = type == PSYS_WEAPON_SMOKE ? 80.0f : 500.0f;
		particle.m_vecColor.x = gEngfuncs.pfnRandomLong(150, 255);
		particle.m_vecColor.y = particle.m_vecColor.z = particle.m_vecColor.x;
		particle.m_vecVelocity = m_vecDirection * 10.0f;
		particle.m_flExtraVelocityUntil = gEngfuncs.GetClientTime() + 0.1f;
		particle.m_flExtraVelocityTime = 0.05f;
		particle.m_flDieTime = gEngfuncs.GetClientTime() + 5.0f;
		particle.m_flLifeTime = 5;
		particle.m_flSizeTarget = 30;
		particle.m_flSizeSpeed = 16;
		particle.m_vecExtraVelocity = m_vecDirection * 100.0f;
		ParticleAdd(particle);

		particle.m_flSizeSpeed = 28;
		particle.m_vecExtraVelocity = m_vecDirection * 200.0f;
		ParticleAdd(particle);
		break;

	case PSYS_GUNSHOT_DEBRIS:
		m_iMaxParticles = 1;
		m_flSpawnInterval = 0;
		m_iFlags = PSYS_COLLIDE | PSYS_GRAVITY;
		particle.m_vecUnknown0 = m_vecOrigin;
		particle.m_vecOrigin = m_vecOrigin;
		particle.m_iFlags |= PARTICLE_LIGHTING;
		particle.m_vecVelocity = m_vecDirection * 120.0f;
		particle.m_iRenderMode = kRenderTransAlpha;
		particle.m_flSize = 1;
		particle.m_pszSprite = m_pszSprite;
		particle.m_flAlpha = 255;
		particle.m_vecColor = Vector(0, 0, 0);
		particle.m_flDieTime = gEngfuncs.GetClientTime() + 4.0f;
		particle.m_flLifeTime = 4;
		break;

	case PSYS_STREAK:
		m_iMaxParticles = 1;
		m_flSpawnInterval = 0;
		m_iFlags = PSYS_COLLIDE | PSYS_GRAVITY;
		particle.m_vecUnknown0 = m_vecOrigin;
		particle.m_vecOrigin = m_vecOrigin;
		particle.m_vecVelocity = m_vecDirection * 30.0f;
		particle.m_iRenderMode = kRenderTransAlpha;
		particle.m_flSize = 4;
		particle.m_pszSprite = m_pszSprite;
		particle.m_flAlpha = 255;
		particle.m_vecColor = Vector(255, 255, 255);
		particle.m_flDieTime = gEngfuncs.GetClientTime() + 10.0f;
		particle.m_flLifeTime = 10;
		break;

	case PSYS_WATER_SPLASH:
		m_pszSprite = "sprites/particles/rain_splash.spr";
		m_iMaxParticles = 9;
		m_flSpawnInterval = 0;
		m_iFlags = 0;
		particle.m_vecUnknown0 = m_vecOrigin;
		particle.m_vecOrigin = m_vecOrigin;
		particle.m_iRenderMode = kRenderTransAdd;
		particle.m_flSize = 20;
		particle.m_pszSprite = m_pszSprite;
		particle.m_flAlpha = 200;
		particle.m_vecColor = Vector(255, 255, 255);
		particle.m_vecVelocity = Vector(0, 0, 0);
		particle.m_iFlags = PARTICLE_ALTERNATE;
		particle.m_flDieTime = gEngfuncs.GetClientTime() + 4.0f;
		ParticleAdd(particle);

		particle.m_vecUnknown0 = m_vecOrigin;
		particle.m_vecOrigin = m_vecOrigin;
		particle.m_flSize = 8;
		particle.m_flAlpha = 255;
		particle.m_vecVelocity = Vector(0, 0, 160);
		particle.m_iFlags = PARTICLE_GRAVITY;
		particle.m_flDieTime = gEngfuncs.GetClientTime() + 1.0f;
		ParticleAdd(particle);

		particle.m_vecUnknown0 = m_vecOrigin;
		particle.m_vecOrigin = m_vecOrigin;
		particle.m_flSize = 12;
		particle.m_vecVelocity = Vector(0, 0, 96);
		particle.m_vecExtraVelocity = Vector(0, 0, 192);
		particle.m_flExtraVelocityUntil = gEngfuncs.GetClientTime() + 0.1f;
		particle.m_flExtraVelocityTime = 0.1f;
		particle.m_flEffectTime = gEngfuncs.GetClientTime() + 0.05f;
		particle.m_flDieTime = gEngfuncs.GetClientTime() + 1.0f;
		ParticleAdd(particle);

		particle.m_vecUnknown0 = m_vecOrigin;
		particle.m_vecOrigin = m_vecOrigin;
		particle.m_flSize = 20;
		particle.m_vecVelocity = Vector(0, 0, 120);
		particle.m_vecExtraVelocity = Vector(0, 0, 360);
		particle.m_flExtraVelocityUntil = gEngfuncs.GetClientTime() + 0.1f;
		particle.m_flExtraVelocityTime = 0.1f;
		particle.m_flEffectTime = gEngfuncs.GetClientTime() + 0.1f;
		particle.m_flDieTime = gEngfuncs.GetClientTime() + 1.0f;
		ParticleAdd(particle);

		for (int i = 0; i < 5; i++)
		{
			particle.m_vecUnknown0 = m_vecOrigin + Vector(0, 0, gEngfuncs.pfnRandomLong(0, 12));
			particle.m_vecOrigin = particle.m_vecUnknown0;
			particle.m_flSize = 3;
			particle.m_flSizeTarget = 3;
			particle.m_flAlpha = 200;

			float z = gEngfuncs.pfnRandomFloat(100, 192);
			float y = gEngfuncs.pfnRandomFloat(-96, 96);
			float x = gEngfuncs.pfnRandomFloat(-96, 96);

			particle.m_vecVelocity = Vector(x, y, z);
			particle.m_vecExtraVelocity = Vector(0, 0, 128);
			particle.m_flExtraVelocityUntil = gEngfuncs.GetClientTime() + 0.1f;
			particle.m_flExtraVelocityTime = 0.1f;
			particle.m_iFlags |= PARTICLE_DROPLET;
			particle.m_flDieTime = gEngfuncs.GetClientTime() + 0.5f;
			ParticleAdd(particle);
		}
		break;

	case PSYS_SCRIPT_WORLD:
	case PSYS_SCRIPT_VIEWMODEL:
		if (effect)
		{
			particle.m_flAlpha = effect->transparency;
			particle.m_vecColor = Vector(effect->colorR, effect->colorB, effect->colorG);
			particle.m_iRenderMode = effect->rendermode;
			particle.m_pszSprite = effect->sprite;
			particle.m_flSize = effect->scale;
		}

		m_iMaxParticles = 1;
		particle.m_vecOrigin = m_vecOrigin;
		particle.m_iFlags = type == PSYS_SCRIPT_VIEWMODEL ? PARTICLE_NODEPTH : 0;
		particle.m_vecAngles.z = gEngfuncs.pfnRandomLong(-16, 16);
		particle.m_vecVelocity = Vector(0, 0, 0);
		particle.m_flDieTime = gEngfuncs.GetClientTime() + 0.01f;
		break;

	case PSYS_VIEW_SMOKE:
	{
		cl_entity_t *player = gEngfuncs.GetLocalPlayer();
		Vector angles = player->angles;

		m_vecOrigin = player->origin;
		VectorNormalize(angles);
		m_pszSprite = "sprites/wep_smoke_01.spr";
		m_iMaxParticles = 10;
		m_flSpawnInterval = 0.2f;

		for (int i = 0; i < 1152; i += 128)
		{
			particle.m_flUnknown80 = particle.m_flUnknown84 = particle.m_flUnknown88 = 0;
			particle.m_vecOrigin = Vector(0, 0, 0);
			particle.m_iFlags = 0;
			particle.m_iRenderMode = kRenderTransAlpha;
			particle.m_flSize = 2048;
			particle.m_pszSprite = m_pszSprite;
			particle.m_flEffectTime = i;
			particle.m_flFrame = 0;
			particle.m_flAlpha = 255;
			particle.m_vecColor = Vector(255, 255, 255);
			particle.m_vecVelocity = Vector(0, 0, 0);
			particle.m_vecExtraVelocity = Vector(0, 0, 0);
			particle.m_flExtraVelocityUntil = gEngfuncs.GetClientTime();
			particle.m_flExtraVelocityTime = 0.2f;
			particle.m_flDieTime = gEngfuncs.GetClientTime() + 10.0f;
			particle.m_flLifeTime = 10;
			ParticleAdd(particle);
		}
		break;
	}

	case PSYS_EXPLOSION:
		m_pszSprite = "sprites/eexplo.spr";
		m_iMaxParticles = 3;
		m_flSpawnInterval = 0.2f;
		particle.m_vecUnknown0 = m_vecOrigin;
		particle.m_vecOrigin = m_vecOrigin;
		particle.m_iFlags = 0;
		particle.m_iRenderMode = kRenderTransAdd;
		particle.m_flSize = 20;
		particle.m_pszSprite = m_pszSprite;
		particle.m_flFrame = 0;
		particle.m_flFrameRate = 30;
		particle.m_flAlpha = 25500;
		particle.m_vecColor = Vector(255, 255, 255);
		particle.m_vecVelocity = m_vecDirection;
		particle.m_flExtraVelocityUntil = gEngfuncs.GetClientTime() + 0.1;
		particle.m_flExtraVelocityTime = 0;
		particle.m_flDieTime = gEngfuncs.GetClientTime() + 10.0f;
		particle.m_flLifeTime = 10;
		particle.m_flSizeTarget = 50;
		particle.m_flSizeSpeed = 16;
		particle.m_vecExtraVelocity = m_vecDirection;
		ParticleAdd(particle);

		particle.m_flSizeSpeed = 18;
		particle.m_vecExtraVelocity = m_vecDirection * 32.0f;
		ParticleAdd(particle);

		particle.m_flSizeSpeed = 20;
		particle.m_vecExtraVelocity = m_vecDirection * 64.0f;
		ParticleAdd(particle);

		particle.m_vecUnknown0 = m_vecOrigin;
		particle.m_vecOrigin = m_vecOrigin;
		particle.m_iFlags = 0;
		particle.m_iRenderMode = kRenderTransAlpha;
		particle.m_flSize = 30;
		particle.m_pszSprite = "sprites/puff1.spr";
		particle.m_flFrame = 0;
		particle.m_flAlpha = 2550;
		particle.m_vecColor.x = gEngfuncs.pfnRandomLong(100, 200);
		particle.m_vecColor.y = particle.m_vecColor.z = particle.m_vecColor.x;
		particle.m_vecVelocity = m_vecDirection * 6.0f;
		particle.m_flExtraVelocityUntil = gEngfuncs.GetClientTime() + 0.1;
		particle.m_flExtraVelocityTime = 0;
		particle.m_flDieTime = gEngfuncs.GetClientTime() + 10.0f;
		particle.m_flLifeTime = 10;
		particle.m_flSizeTarget = 90;
		particle.m_flSizeSpeed = 8;
		particle.m_vecExtraVelocity = m_vecDirection;
		ParticleAdd(particle);

		particle.m_flSizeSpeed = 9;
		particle.m_vecExtraVelocity = m_vecDirection * 12.0f;
		ParticleAdd(particle);

		particle.m_flSizeSpeed = 10;
		particle.m_vecExtraVelocity = m_vecDirection * 24.0f;
		ParticleAdd(particle);
		break;

	case PSYS_TINTED_SMOKE:
	{
		m_pszSprite = "sprites/puff1.spr";
		m_flSpawnInterval = 0.1f;
		particle.m_vecUnknown0 = m_vecOrigin + Vector(0, 0, 12);
		particle.m_vecOrigin = particle.m_vecUnknown0;
		particle.m_iFlags = PARTICLE_LIGHTING;
		particle.m_iRenderMode = kRenderTransAlpha;
		particle.m_flSize = 1;
		particle.m_pszSprite = m_pszSprite;
		particle.m_flFrame = 0;
		particle.m_flAlpha = 3500;
		particle.m_vecColor.x = gEngfuncs.pfnRandomLong(150, 255);
		particle.m_vecColor.y = 0;
		particle.m_vecColor.z = particle.m_vecColor.x;

		float y = gEngfuncs.pfnRandomFloat(-1, 1) + m_vecDirection.y;
		float x = gEngfuncs.pfnRandomFloat(-1, 1) + m_vecDirection.x;

		particle.m_vecVelocity = Vector(x, y, m_vecDirection.z) * 64.0f;
		particle.m_flExtraVelocityUntil = gEngfuncs.GetClientTime() + 0.1f;
		particle.m_flExtraVelocityTime = 0.05f;
		particle.m_flDieTime = gEngfuncs.GetClientTime() + 5.0f;
		particle.m_flLifeTime = 8;
		particle.m_flSizeTarget = 256;
		particle.m_flSizeSpeed = 270;
		particle.m_vecExtraVelocity = m_vecDirection * 100.0f;
		ParticleAdd(particle);

		particle.m_flSizeSpeed = 256;
		particle.m_vecExtraVelocity = m_vecDirection * 200.0f;
		ParticleAdd(particle);
		break;
	}
	}

	ParticleAdd(particle);
	m_bActive = true;
	m_flNextSpawn = gEngfuncs.GetClientTime() + m_flSpawnInterval;
}
