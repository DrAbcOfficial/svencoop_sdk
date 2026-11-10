#include "hud.h"
#include "cl_util.h"
#include "cl_entity.h"
#include "event_api.h"
#include "com_model.h"
#include "r_studioint.h"
#include "studio_util.h"
#include "client_filesystem.h"
#include "particle_engine.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern engine_studio_api_t IEngineStudio;

void ParseParticleOffset(const char *text, Vector &offset)
{
	char buffer[128] = {};

	strncpy(buffer, text, sizeof(buffer) - 1);

	char *value = buffer;
	int component;

	for (component = 0; component < 3; component++)
	{
		offset[component] = atof(value);

		while (*value && *value != ' ')
			value++;

		if (!*value)
			break;

		value++;
	}

	for (++component; component < 3; component++)
		offset[component] = 0;
}

void MuzzleBoneOrigin(particle_script_effect_s *effect, Vector &origin)
{
	float (*bones)[3][4] = (float (*)[3][4])IEngineStudio.StudioGetBoneTransform();
	float (*matrix)[4] = bones[effect->bone];

	if (effect->offset != Vector(0, 0, 0))
		VectorTransform(effect->offset, matrix, origin);
	else
		origin = Vector(matrix[0][3], matrix[1][3], matrix[2][3]);
}

void StartMuzzleSystem(cl_entity_t *entity, particle_script_effect_s *effect, const Vector &origin)
{
	CParticleSystem *system = ParticleEngine()->SystemCreate();

	system->m_flDieTime = gEngfuncs.GetClientTime() + 0.01f;
	system->m_vecOrigin = origin;
	system->m_vecDirection = Vector(0, 0, 0);
	system->m_pszSprite = effect->sprite;
	system->SystemStartup(entity == IEngineStudio.GetViewEntity() ? PSYS_SCRIPT_VIEWMODEL : PSYS_SCRIPT_WORLD, effect);
}

bool CParticleEngine::PlayMuzzleScriptByName(cl_entity_s *entity, const char *name)
{
	if (!entity || !name || !*name)
		return false;

	Vector origin(0, 0, 0);
	particle_script_effect_s *effect;

	for (effect = m_pMuzzleEffects; effect; effect = effect->next)
	{
		if (strcmp(effect->name, name))
			continue;

		if (effect->attachment >= 0)
		{
			Vector angles, forward, right, up;

			if (gEngfuncs.pEventAPI->EV_IsLocal(entity->index - 1))
				gEngfuncs.GetViewAngles(angles);
			else
				angles = entity->angles;

			AngleVectors(angles, forward, right, up);
			origin = entity->attachment[effect->attachment] + right * effect->offset.x
				+ forward * effect->offset.y + up * effect->offset.z;
		}
		else
		{
			MuzzleBoneOrigin(effect, origin);
		}

		StartMuzzleSystem(entity, effect, origin);
		return true;
	}

	char path[260];
	char line[260];
	char key[260] = {};
	char value[260] = {};

	snprintf(path, sizeof(path), "events/%s", name);
	path[sizeof(path) - 1] = 0;

	FileHandle_t file = OpenGameFile(path, "r");

	if (!file)
		return false;

	effect = new particle_script_effect_s;
	memset(effect, 0, sizeof(*effect));

	static const char *keys[] =
	{
		"attachment", "bone", "spritename", "scale", "colorR", "colorB", "colorG",
		"rendermode", "transparency", "offset", NULL
	};

	while (!g_pFileSystem->EndOfFile(file))
	{
		if (!g_pFileSystem->ReadLine(line, sizeof(line), file))
			continue;

		sscanf(line, "%s %[^\n\r]", key, value);

		for (int i = strlen(value) - 1; i >= 0 && isspace(value[i]); i--)
			value[i] = 0;

		int field;

		for (field = 0; keys[field]; field++)
		{
			if (!strcmp(keys[field], key))
				break;
		}

		switch (field)
		{
		case 0:
			effect->attachment = (signed char)atoi(value);
			break;
		case 1:
			effect->bone = atoi(value);
			break;
		case 2:
			strncpy(effect->sprite, value, sizeof(effect->sprite) - 1);
			break;
		case 3:
			effect->scale = (unsigned char)atoi(value);
			break;
		case 4:
			effect->colorR = (unsigned char)atoi(value);
			break;
		case 5:
			effect->colorB = (unsigned char)atoi(value);
			break;
		case 6:
			effect->colorG = (unsigned char)atoi(value);
			break;
		case 7:
			effect->rendermode = atoi(value);
			break;
		case 8:
			effect->transparency = atoi(value);
			break;
		case 9:
			ParseParticleOffset(value, effect->offset);
			break;
		}
	}

	strncpy(effect->name, name, sizeof(effect->name) - 1);
	AddMuzzleEffect(effect);

	if (effect->attachment >= 0)
	{
		origin = entity->attachment[effect->attachment] + effect->offset;
	}
	else if (effect->bone >= 0)
	{
		MuzzleBoneOrigin(effect, origin);
	}

	StartMuzzleSystem(entity, effect, origin);
	g_pFileSystem->Close(file);
	return true;
}
