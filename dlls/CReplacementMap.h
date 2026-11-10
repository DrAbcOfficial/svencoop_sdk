#ifndef CREPLACEMENTMAP_H
#define CREPLACEMENTMAP_H

#include <string>
#include <unordered_map>

class CStringMap;

struct ReplacementSettings
{
	const char	*m_pszName;
	const char	*m_pszDirectory;
	void		(*m_pfnLoaded)(CStringMap &map);
};

class CReplacementMap
{
public:
	enum
	{
		MAX_PATH_LENGTH = 261,
		MAX_TOKEN_LENGTH = 256,
		MAX_LINE_LENGTH = MAX_TOKEN_LENGTH * 2
	};

	CReplacementMap(const ReplacementSettings &settings);
	~CReplacementMap();

	unsigned int GetReplacementCount(void) const;
	CStringMap *GetOrLoadReplacementMap(const char *name, bool reportErrors);

	ReplacementSettings							m_Settings;
	std::unordered_map<std::string, CStringMap *>	m_Maps;
};

bool LoadReplacementMap(const char *name, bool reportErrors,
	const ReplacementSettings &settings, CStringMap &map);

#endif
