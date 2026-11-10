#include "extdll.h"
#include "util.h"
#include "server_filesystem.h"
#include "CStringMap.h"
#include "CReplacementMap.h"

CReplacementMap::CReplacementMap(const ReplacementSettings &settings)
	: m_Settings(settings)
{
}

CReplacementMap::~CReplacementMap()
{
	for (std::unordered_map<std::string, CStringMap *>::iterator it = m_Maps.begin(); it != m_Maps.end(); it++)
		delete it->second;
}

unsigned int CReplacementMap::GetReplacementCount(void) const
{
	return m_Maps.size();
}

CStringMap *CReplacementMap::GetOrLoadReplacementMap(const char *name, bool reportErrors)
{
	if (!name || !*name)
		return NULL;

	char directory[MAX_PATH_LENGTH];
	char path[MAX_PATH_LENGTH];

	snprintf(directory, sizeof(directory), "%s/%s", m_Settings.m_pszDirectory, STRING(gpGlobals->mapname));
	snprintf(path, sizeof(path), "%s/%s", directory, name);

	std::unordered_map<std::string, CStringMap *>::iterator it = m_Maps.find(path);

	if (it != m_Maps.end())
		return it->second;

	ALERT(at_console, "Loading replacement map \"%s\"...\n", name);

	CStringMap *map = new CStringMap;

	if (!m_Maps.insert(std::make_pair(path, map)).second)
	{
		ALERT(at_error, "Failed to insert %s replacement file into map!\n", m_Settings.m_pszName);
		delete map;
		return NULL;
	}

	return LoadReplacementMap(name, reportErrors, m_Settings, *map) ? map : NULL;
}

bool LoadReplacementMap(const char *name, bool reportErrors,
	const ReplacementSettings &settings, CStringMap &map)
{
	if (!name || !*name)
		return false;

	char directory[CReplacementMap::MAX_PATH_LENGTH];
	char path[CReplacementMap::MAX_PATH_LENGTH];

	snprintf(directory, sizeof(directory), "%s/%s", settings.m_pszDirectory, STRING(gpGlobals->mapname));
	snprintf(path, sizeof(path), "%s/%s", directory, name);

	if (g_pFileSystem->IsDirectory(path))
	{
		ALERT(at_error, "%s Replacement File '%s' is a directory!\n", settings.m_pszName, path);
		return false;
	}

	FileHandle_t file = OpenGameFile(path, "r");

	if (!file)
	{
		if (reportErrors)
			ALERT(at_error, "%s Replacement file not found: %s\n", settings.m_pszName, path);

		return false;
	}

	char source[CReplacementMap::MAX_TOKEN_LENGTH];
	char replacement[CReplacementMap::MAX_TOKEN_LENGTH];
	char line[CReplacementMap::MAX_LINE_LENGTH];

	source[sizeof(source) - 1] = 0;
	replacement[sizeof(replacement) - 1] = 0;

	while (!g_pFileSystem->EndOfFile(file))
	{
		if (!g_pFileSystem->ReadLine(line, sizeof(line), file) || !strlen(line) ||
			(line[0] == '/' ? line[1] == '/' : line[0] == '#'))
			continue;

		char *sourceContext = NULL;
		char *replacementContext = NULL;

		if (sscanf(line, "%s %s\n", source, replacement) > 0)
		{
#ifdef _WIN32
			char *first = strtok_s(source, "\"", &sourceContext);
			char *second = strtok_s(replacement, "\"", &replacementContext);
#else
			char *first = strtok_r(source, "\"", &sourceContext);
			char *second = strtok_r(replacement, "\"", &replacementContext);
#endif

			map.AddEntry(first, second);
			line[0] = 0;
		}
	}

	g_pFileSystem->Close(file);

	if (settings.m_pfnLoaded)
		settings.m_pfnLoaded(map);

	if (reportErrors)
		ALERT(at_console, "Finished precaching custom %ss.\n", settings.m_pszName);

	return true;
}
