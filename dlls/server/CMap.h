#ifndef CMAP_H
#define CMAP_H

class CCfgFile;
class CReplacementMap;
class CStringMap;

class CMap
{
public:
	enum
	{
		MAX_FORCED_PLAYER_MODELS = 16,
		MAX_FORCED_MODEL_NAME = 64,
		MAX_FORCED_MODEL_LIST = 512,
		MAX_CONFIG_PATH = 260
	};

	CMap();
	~CMap();

	static CMap *CreateInstance(void);
	static CMap *GetInstance(void);
	static void DestroyInstance(void);

	CStringMap *GetSoundReplacementMap(const char *name, bool reportErrors);
	bool HasForcedPlayerModels(void) const;
	void SetForcedPlayerModels(const char *models);
	bool KeyValue(const char *key, const char *value);
	const char *GetForcedPlayerModel(unsigned int index) const;
	void InitializeForcedPlayerModels(const char *models);
	void Initialize(void);
	void LoadMapScriptConfigs(void);

	CCfgFile *LoadGenericConfigFile(const char *fileName, bool skill);
	bool LoadSkillFile(const char *fileName);
	bool LoadGlobalSkillFile(void);
	bool LoadConfigFile(const char *fileName, bool keep);
	bool LoadGlobalConfigFile(void);
	bool LoadMapSkillFile(void);
	bool LoadMapConfigFile(void);

	static CMap			*m_pInstance;

	CReplacementMap		*m_pSoundReplacementMaps;
	string_t			m_iszForcedPlayerModels;
	unsigned int		m_uiForcedPlayerModels;
	char				m_szForcedPlayerModels[MAX_FORCED_PLAYER_MODELS][MAX_FORCED_MODEL_NAME];
	CCfgFile			*m_pMapConfigFile;
};

void SoundReplacementLoadedCallback(CStringMap &map);

#endif
