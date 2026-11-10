#include "extdll.h"
#include "util.h"
#include "server_support.h"
#include "server_sound.h"
#include "CStringMap.h"
#include "CReplacementMap.h"
#include "CCfgFile.h"
#include "CMap.h"

CMap *CMap::m_pInstance;

CMap::CMap()
{
	ReplacementSettings settings = { "Sound", "sound", SoundReplacementLoadedCallback };

	m_pSoundReplacementMaps = new CReplacementMap(settings);
	m_iszForcedPlayerModels = 0;
	m_uiForcedPlayerModels = 0;
	m_pMapConfigFile = NULL;
	memset(m_szForcedPlayerModels, 0, sizeof(m_szForcedPlayerModels));
}

CMap::~CMap()
{
	delete m_pSoundReplacementMaps;
	delete m_pMapConfigFile;
}

CMap *CMap::CreateInstance(void)
{
	if (m_pInstance)
	{
		ALERT(at_console, "CMap::CreateInstance: Instance already exists!\n");
		return m_pInstance;
	}

	m_pInstance = new CMap;
	return m_pInstance;
}

CMap *CMap::GetInstance(void)
{
	return m_pInstance;
}

void CMap::DestroyInstance(void)
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
	else
	{
		ALERT(at_console, "CMap::DestroyInstance: Instance not created or already destroyed!\n");
	}
}

CStringMap *CMap::GetSoundReplacementMap(const char *name, bool reportErrors)
{
	return m_pSoundReplacementMaps->GetOrLoadReplacementMap(name, reportErrors);
}

bool CMap::HasForcedPlayerModels(void) const
{
	return m_iszForcedPlayerModels && STRING(m_iszForcedPlayerModels)[0];
}

void CMap::SetForcedPlayerModels(const char *models)
{
	m_iszForcedPlayerModels = AllocStringLocal(models);
}

bool CMap::KeyValue(const char *key, const char *value)
{
	if (strcmp(key, "forcepmodels"))
		return false;

	SetForcedPlayerModels(value);
	return true;
}

const char *CMap::GetForcedPlayerModel(unsigned int index) const
{
	return m_szForcedPlayerModels[index];
}

void CMap::InitializeForcedPlayerModels(const char *models)
{
	m_uiForcedPlayerModels = 0;
	memset(m_szForcedPlayerModels, 0, sizeof(m_szForcedPlayerModels));

	char list[MAX_FORCED_MODEL_LIST];
	char *context = NULL;

	UTIL_CopyString(list, models, sizeof(list));

#ifdef _WIN32
	char *model = strtok_s(list, ";", &context);
#else
	char *model = strtok_r(list, ";", &context);
#endif

	while (model && *model)
	{
		if (m_uiForcedPlayerModels == MAX_FORCED_PLAYER_MODELS)
		{
			ALERT(at_warning, "Forced player models list exceeds maximum number of models(%u)!\n",
				MAX_FORCED_PLAYER_MODELS);
			return;
		}

		UTIL_CopyString(m_szForcedPlayerModels[m_uiForcedPlayerModels++], model, MAX_FORCED_MODEL_NAME);

#ifdef _WIN32
		model = strtok_s(NULL, ";", &context);
#else
		model = strtok_r(NULL, ";", &context);
#endif
	}
}

void CMap::Initialize(void)
{
	if (m_iszForcedPlayerModels)
		InitializeForcedPlayerModels(STRING(m_iszForcedPlayerModels));
}

CCfgFile *CMap::LoadGenericConfigFile( const char *fileName, bool skill )
{
	if ( !fileName || !*fileName )
		return NULL;

	CCfgFile *config = new CCfgFile( fileName );

	if ( config && config->m_bValid )
	{
		if ( skill )
			config->ApplyCfgOnSkill();
		else
			config->ApplyCfgOnMap();
	}

	return config;
}

bool CMap::LoadConfigFile( const char *fileName, bool keep )
{
	CCfgFile *config = LoadGenericConfigFile( fileName, false );

	if ( config )
	{
		if ( keep )
			m_pMapConfigFile = config;
		else
			delete config;
	}

	return false;
}

bool CMap::LoadGlobalConfigFile( void )
{
	return LoadConfigFile( "default_map_settings.cfg", false );
}

bool CMap::LoadMapConfigFile( void )
{
	const char *mapName = STRING( gpGlobals->mapname );

	if ( !mapName || !*mapName )
		return false;

	char path[MAX_CONFIG_PATH];

	snprintf( path, sizeof( path ), "maps/%s.cfg", mapName );
	return LoadConfigFile( path, true );
}

bool CMap::LoadSkillFile(const char *fileName)
{
	if (!fileName || !*fileName)
		return false;

	CCfgFile *config = new CCfgFile(fileName);

	if (!config)
		return false;

	if (config->m_bValid)
		config->ApplyCfgOnSkill();

	delete config;
	return true;
}

bool CMap::LoadGlobalSkillFile(void)
{
	const char *fileName = CVAR_GET_STRING("skillcfgfile");

	if (!fileName || !*fileName)
		return false;

	char path[MAX_CONFIG_PATH];

	UTIL_CopyString(path, fileName, sizeof(path));
	return LoadSkillFile(path);
}

bool CMap::LoadMapSkillFile(void)
{
	const char *mapName = STRING(gpGlobals->mapname);

	if (!mapName || !*mapName)
		return false;

	char path[MAX_CONFIG_PATH] = {};

	snprintf(path, sizeof(path), "maps/%s_skl.cfg", mapName);
	return LoadSkillFile(path);
}

void SoundReplacementLoadedCallback(CStringMap &map)
{
	for (unsigned int i = 0; i < map.m_uiCount; i++)
		CServer_SoundEngine::GetSoundEngine()->PrecacheSound(map[i]);
}

void CMap::LoadMapScriptConfigs( void )
{
	{
		CCfgFile globalConfig( "default_map_settings.cfg" );

		if ( globalConfig.m_bValid )
			globalConfig.LoadMapScripts();
	}

	const char *mapName = STRING( gpGlobals->mapname );

	if ( !mapName || !*mapName )
		return;

	char path[MAX_CONFIG_PATH];

	snprintf( path, sizeof( path ), "maps/%s.cfg", mapName );
	m_pMapConfigFile = new CCfgFile( path );

	if ( m_pMapConfigFile->m_bValid )
		m_pMapConfigFile->LoadMapScripts();
}
