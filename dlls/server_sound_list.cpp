#include "extdll.h"
#include "util.h"
#include "server_sound.h"
#include "server_filesystem.h"
#include "server_support.h"
#include <errno.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

void CServer_SoundEngine::Precache(void)
{
	char gameDir[MAX_PATH];
	char directory[384];
	char relativePath[160];
	char fileName[sizeof( gameDir ) + sizeof( relativePath )];
	const char *mapName = STRING(gpGlobals->mapname);

	GET_GAME_DIR(gameDir);
	snprintf(directory, sizeof(directory), "%s/%s", gameDir, "maps/soundcache");
	snprintf(relativePath, sizeof(relativePath), "%s/%s.txt", "maps/soundcache", mapName);
	snprintf(fileName, sizeof(fileName), "%s/%s", gameDir, relativePath);

#ifdef _WIN32
	int result = _mkdir(directory);
#else
	int result = mkdir(directory, 0755);
#endif

	if (result && errno != EEXIST)
		ALERT(at_error, "Failed to create folder '%s', sound list files can't be written! %d\n", directory, result);

	FILE *file = fopen(fileName, "w");

	if (file)
	{
		fprintf(file, "maps/%s.bsp\n", mapName);
		fprintf(file, "startup\n");
		fclose(file);
	}

	PRECACHE_GENERIC((char *)STRING(AllocStringLocal(relativePath)));
}

void CServer_SoundEngine::PrecacheSound(const char *sample)
{
	if (!sample)
		return;

	if (*sample == '*')
		sample++;

	int index = 0;

	while (index < MAX_SOUNDLIST_SOUNDS && m_Sounds[index])
	{
		if (!strcmp(m_Sounds[index], sample))
			return;

		index++;
	}

	if (index >= MAX_SOUNDLIST_SOUNDS)
	{
		ALERT(at_error, "[Sound Engine] - Can't precache %s, %i sounds already precached!\n", sample, MAX_SOUNDLIST_SOUNDS);
		return;
	}

	if (m_bSoundListGenerated)
	{
		ALERT(at_console, "[Sound Engine] - Can't precache %s, sound list already generated.\n", sample);
		return;
	}

	m_Sounds[index] = sample;
	m_iLastSound = index;

	char path[MAX_PATH];

	snprintf(path, sizeof(path), "sound/%s", sample);

	if (((IFileSystem_SvEngine *)g_pFileSystem)->FileExistsEx(path, "GAME"))
		PRECACHE_GENERIC((char *)STRING(AllocStringLocal(path)));
}

void CServer_SoundEngine::LoadMaterialsList(void)
{
	m_Materials.LoadMaterialsList(m_szMaterialsFile);

	if (m_szMaterialsFile[0])
	{
		char path[MAX_PATH];

		snprintf(path, sizeof(path), "sound/%s/%s", STRING(gpGlobals->mapname), m_szMaterialsFile);
		PRECACHE_GENERIC((char *)STRING(AllocStringLocal(path)));
	}
}

void CServer_SoundEngine::WriteSoundList(bool force)
{
	if (m_bSoundListGenerated && !force)
		return;

	if (!m_Sentences[0].name[0] || force)
		LoadSentenceList();

	if (!m_Materials.m_Materials[0].type || m_Materials.m_iMaterialCount < 1 || force)
		LoadMaterialsList();

	char gameDir[MAX_PATH];
	char fileName[MAX_PATH + 128];
	const char *mapName = STRING(gpGlobals->mapname);

	GET_GAME_DIR(gameDir);
	snprintf(fileName, sizeof(fileName), "%s/%s/%s.txt", gameDir, "maps/soundcache", mapName);

	FILE *file = fopen(fileName, "w");

	if (file)
	{
		fprintf(file, "%s\n", mapName);
		fprintf(file, "%s\n", GetIpAddr());
		fprintf(file, "SOUNDLIST {\n");

		for (int i = 0; i < MAX_SOUNDLIST_SOUNDS && m_Sounds[i]; i++)
			fprintf(file, "%s\n", m_Sounds[i]);

		fprintf(file, "}\n");
		fprintf(file, "SENTENCELIST {\n");

		for (int i = 0; i < MAX_SOUNDLIST_SENTENCES && strlen(m_Sentences[i].name); i++)
			fprintf(file, "%s %s\n", m_Sentences[i].name, m_Sentences[i].text);

		fprintf(file, "}\n");
		fprintf(file, "CUSTOMMATERIALS {\n");

		if (m_szMaterialsFile[0])
			fprintf(file, "%s\n", m_szMaterialsFile);

		fprintf(file, "}\n");
		fclose(file);
	}
	else
	{
		ALERT(at_logged, "[Sound Engine] - Failed to write sound cache \"%s\" - Error #%d\n", fileName, errno);
	}

	m_bSoundListGenerated = true;
}
