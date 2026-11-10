#include "hud.h"
#include "cl_util.h"
#include "particle_engine.h"
#include "particle_effects.h"

static float flLastWaterSplash;

void MakeWaterSplash(float *origin, int type)
{
	float elapsed = gEngfuncs.GetClientTime() - flLastWaterSplash;

	if (elapsed > 0.25f)
	{
		CParticleSystem *system = ParticleEngine()->SystemCreate();

		system->m_flDieTime = gEngfuncs.GetClientTime() + 1.0f;
		system->m_vecOrigin = Vector(origin);
		system->m_vecDirection = Vector(0, 0, 0);
		system->SystemStartup(PSYS_WATER_SPLASH, NULL);
		flLastWaterSplash = gEngfuncs.GetClientTime();
	}
}
