#ifndef CASSCRIPTBASECLASSES_H
#define CASSCRIPTBASECLASSES_H

struct edict_s;

class CASScriptBaseClasses
{
public:
	struct ScriptBaseClass_t
	{
		const char	*m_pszName;
		const char	*m_pszSource;
	};

	static const ScriptBaseClass_t *GetBaseClasses(void);
	static unsigned int GetBaseClassesCount(void);
	static void OutputToConsole(edict_s *player);

private:
	enum
	{
		FIRST_ENTITY_HEADER = 1,
		CONSOLE_BUFFER_SIZE = 4096
	};

	static const char	*m_pszBaseEntity;
	static const char	*m_pszBaseAnimating;
	static const char	*m_pszBaseMonsterEntity;
	static const char	*m_pszBaseTankEntity;
	static const char	*m_pszBasePlayerItemEntity;
	static const char	*m_pszBasePlayerWeaponEntity;
	static const char	*m_pszBaseItemEntity;
	static const char	*m_pszBasePlayerAmmoEntity;

	static ScriptBaseClass_t	m_ScriptBaseClasses[];
};

#endif
