#include "extdll.h"
#include "scripting/core/CASCommonServerModuleValidation.h"
#include "scripting/core/CASServerManager.h"
#include "scripting/core/CASModuleBuilder.h"
#include "scripting/core/CASLog.h"
#include "server/server_support.h"
#include <angelscript.h>

asITypeInfo **CASCommonServerModuleValidation::m_pEntityObjectTypes;

void CASCommonServerModuleValidation::PrepareValidator(void)
{
	static const char *entityTypes[ENTITY_TYPE_COUNT] =
	{
		"CBaseEntity",
		"CBaseDelay",
		"CBaseAnimating",
		"CBaseToggle",
		"CBasePlayerItem",
		"CBasePlayerWeapon",
		"CBasePlayerAmmo",
		"CItemInventory",
		"CItem",
		"CGrenade",
		"CBaseMonster",
		"CCineMonster",
		"CBasePlayer",
		"CSprite",
		"CPathTrack",
		"CBeam",
		"CLaser",
		"CBaseTank",
		"CPathCondition",
		"CBaseButton",
		"CBaseDoor",
		"CGib"
	};

	m_pEntityObjectTypes = new asITypeInfo *[ENTITY_TYPE_COUNT];

	for (unsigned int index = 0; index < ENTITY_TYPE_COUNT; index++)
	{
		m_pEntityObjectTypes[index] = CASServerManager::GetInstance()->GetTypeInfoByName(entityTypes[index]);
		m_pEntityObjectTypes[index]->AddRef();
	}
}

void CASCommonServerModuleValidation::CleanupValidator(void)
{
	if (!m_pEntityObjectTypes)
		return;

	for (unsigned int index = 0; index < ENTITY_TYPE_COUNT; index++)
		m_pEntityObjectTypes[index]->Release();

	delete[] m_pEntityObjectTypes;
	m_pEntityObjectTypes = NULL;
}

bool CASCommonServerModuleValidation::CheckForEntityHandles(CASModuleBuilder *builder)
{
	asIScriptModule *module = builder->m_pModule;
	asIScriptEngine *engine = CASServerManager::GetInstance()->m_pEngine;
	unsigned int globalCount = module->GetGlobalVarCount();
	bool valid = true;

	for (unsigned int variable = 0; variable < globalCount; variable++)
	{
		const char *name;
		int typeId;

		if (module->GetGlobalVar(variable, &name, NULL, &typeId, NULL) < 0)
			continue;

		asITypeInfo *type = engine->GetTypeInfoById(typeId);

		if (!type)
			continue;

		for (unsigned int index = 0; index < ENTITY_TYPE_COUNT; index++)
		{
			if (type != m_pEntityObjectTypes[index])
				continue;

			char error[VALIDATION_ERROR_LENGTH];
			unsigned int length = (unsigned int) UTIL_FormatString(error, sizeof(error),
				"Can't have entity handles as global variables; variable '%s' not allowed", name);

			if (length >= sizeof(error))
			{
				CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
					"CASCommonServerModuleValidation::CheckForEntityHandles: failed to format error message!\n");
			}
			else
			{
				builder->ReportValidationError(error);
			}

			valid = false;
			break;
		}
	}

	return valid;
}
