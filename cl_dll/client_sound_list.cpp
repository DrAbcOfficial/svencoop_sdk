#include "extdll.h"
#include "hud_iface.h"
#include "client_filesystem.h"
#include "client_sound.h"
#include "client_support.h"

void AlertMessage(ALERT_TYPE type, char *format, ...);

void CClient_SoundEngine::FlushCache(bool all)
{
	AlertMessage(at_console, "Flushing cached sound list files...\n");

	const char *address = GetIpAddr();
	char mapName[33];
	char search[260];
	FileFindHandle_t handle;

	StripMapFromPFN(gEngfuncs.pfnGetLevelName(), mapName);
	snprintf(search, sizeof(search), "%s/*.txt", "maps/soundcache");

	const char *fileName = g_pFileSystem->FindFirst(search, &handle, "GAME");

	if (!fileName)
		return;

	do
	{
		const char *directory = GetTopSubdir(((IFileSystem_SvEngine *)g_pFileSystem)->FindGetCurSearchPath(handle));
		char path[388];

		snprintf(path, sizeof(path), "%s/%s/%s", directory, "maps/soundcache", fileName);

		if (!all && !stricmp(gEngfuncs.pfnGetGameDirectory(), directory))
		{
			FileHandle_t file = g_pFileSystem->Open(path, "rb", "BASE");

			if (file)
			{
				char fileMap[32] = {};
				char fileAddress[128] = {};

				g_pFileSystem->ReadLine(fileMap, sizeof(fileMap), file);
				g_pFileSystem->ReadLine(fileAddress, sizeof(fileAddress), file);
				sscanf(fileMap, "%s\n", fileMap);
				sscanf(fileAddress, "%s\n", fileAddress);

				if ((!strlen(mapName) || !stricmp(mapName, fileMap)) && !stricmp(address, fileAddress))
				{
					AlertMessage(at_aiconsole, "Sound list file '%s' is currently in use, not flushing.\n", path);
					g_pFileSystem->Close(file);
					continue;
				}

				g_pFileSystem->Close(file);
			}
		}

		g_pFileSystem->GetLocalPath(path, path, sizeof(path));
		remove(path);
	}
	while ((fileName = g_pFileSystem->FindNext(handle)) != NULL);

	g_pFileSystem->FindClose(handle);
}

bool CClient_SoundEngine::LoadSoundList(void)
{
	if (!m_bInitialized || m_flMapTime > gEngfuncs.GetClientTime())
		return false;

	ClearSoundArrays();

	char mapName[128];
	char directory[384];
	char path[160];
	char command[202];

	StripMapFromPFN(gEngfuncs.pfnGetLevelName(), mapName);
	snprintf(directory, sizeof(directory), "%s/%s", gEngfuncs.pfnGetGameDirectory(), "maps/soundcache");
	snprintf(path, sizeof(path), "%s/%s.txt", "maps/soundcache", mapName);
	snprintf(command, sizeof(command), "dlfile %s\n", path);
	g_pFileSystem->CreateDirHierarchy("maps/soundcache", "GAME");
	gEngfuncs.pfnClientCmd(command);

	FileHandle_t file = g_pFileSystem->Open(path, "rb");

	if (!file)
	{
		AlertMessage(at_console, "Sound list file at '%s' not found.\n", path);
		gEngfuncs.pfnClientCmd(command);
		gEngfuncs.pfnClientCmd("retry");
		m_flMapTime = gEngfuncs.GetClientTime() + 3;
		return false;
	}

	char localPath[420];
	char line[260] = {};
	char currentMap[128] = {};
	char fileMap[128] = {};

	g_pFileSystem->GetLocalPath(path, localPath, sizeof(localPath));
	StripMapFromPFN(gEngfuncs.pfnGetLevelName(), currentMap);
	currentMap[sizeof(currentMap) - 1] = 0;
	g_pFileSystem->ReadLine(fileMap, sizeof(fileMap), file);
	sscanf(fileMap, "%s\n", fileMap);

	if (strcmp(fileMap, currentMap))
	{
		g_pFileSystem->Close(file);
		AlertMessage(at_console,
			"Deleting outdated sound list file '%s'. Scanned map name '%s' does not match current map '%s'.\n",
			path, fileMap, currentMap);
		remove(localPath);
		AlertMessage(at_console, "Reconnecting to server...", path);
		gEngfuncs.pfnClientCmd(command);
		gEngfuncs.pfnClientCmd("retry");
		m_flMapTime = gEngfuncs.GetClientTime() + 3;
		return false;
	}

	UTIL_CopyString(m_szMapName, currentMap, sizeof(m_szMapName));

	const char *address = GetIpAddr();
	char fileAddress[128] = {};

	g_pFileSystem->ReadLine(fileAddress, sizeof(fileAddress), file);
	sscanf(fileAddress, "%s\n", fileAddress);

	bool redundant = stricmp(address, fileAddress) != 0;

	while (g_pFileSystem->ReadLine(line, sizeof(line), file) && strncmp(line, "SOUNDLIST {", 11))
	{
	}

	int count = 0;

	while (g_pFileSystem->ReadLine(line, sizeof(line), file) && count < MAX_SOUNDLIST_SOUNDS && strncmp(line, "}", 1))
	{
		char *sample = TrimString(line);

		if (sample)
			UTIL_CopyString(m_szSoundNames[count++], sample, sizeof(m_szSoundNames[0]));
	}

	while (g_pFileSystem->ReadLine(line, sizeof(line), file) && strncmp(line, "SENTENCELIST {", 14))
	{
	}

	while (g_pFileSystem->ReadLine(line, sizeof(line), file) && m_iSentenceCount < MAX_SOUNDLIST_SENTENCES && strncmp(line, "}", 1))
	{
		line[strlen(line) - 1] = 0;
		ParseSentenceLine(line);
	}

	while (g_pFileSystem->ReadLine(line, sizeof(line), file) && strncmp(line, "CUSTOMMATERIALS {", 15))
	{
	}

	char materials[260] = {};

	while (g_pFileSystem->ReadLine(line, sizeof(line), file) && m_iSoundCount < MAX_SOUNDLIST_SOUNDS && strncmp(line, "}", 1))
	{
		unsigned int length = strlen(line);

		if (length && line[length - 1] == '\n')
			length--;

		if (length && line[length - 1] == '\r')
			length--;

		line[length] = 0;
		UTIL_CopyString(materials, line, sizeof(materials));
	}

	memset(line, 0, sizeof(line));
	g_pFileSystem->Close(file);
	m_Materials.LoadMaterialsList(materials);

	if (redundant)
	{
		AlertMessage(at_warning, "Deleting redundant sound list file '%s'.\n", path);
		remove(localPath);
	}

	int loaded = 0;
	int failed = 0;

	for (int i = 0; i < count; i++)
	{
		FMOD::Sound *sound = LoadSound(m_szSoundNames[i], false);

		if (sound)
		{
			loaded++;
			m_pSoundCache[i] = new sndCache_t;
			m_pSoundCache[i]->sound = sound;
			UTIL_CopyString(m_pSoundCache[i]->name, m_szSoundNames[i], sizeof(m_pSoundCache[i]->name));
		}
		else
		{
			failed++;
		}
	}

	AlertMessage(at_console, "[Sound Engine] Finished loading sound list: %d loaded, %d failed.\n", loaded, failed);
	m_bSoundListLoaded = true;
	FlushCache(false);
	return true;
}
