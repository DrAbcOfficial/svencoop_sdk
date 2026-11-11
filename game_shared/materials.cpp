#include "extdll.h"
#include "util.h"
#include "server_filesystem.h"
#include "server_support.h"
#define MATERIAL_ALERT ALERT
#include "materials.h"
#include "pm_materials.h"
#include <ctype.h>

CMaterialsList::CMaterialsList()
{
	Reset();
}

CMaterialsList::~CMaterialsList()
{
}

void CMaterialsList::Reset(void)
{
	memset(m_Materials, 0, sizeof(m_Materials));
	m_iMaterialCount = 0;
}

int CMaterialsList::LinearMaterialSearch(const char *name) const
{
	if (!name)
		return -1;

	for (int i = 0; i < MAX_MATERIALS && strlen(m_Materials[i].name); i++)
	{
		if (!stricmp(name, m_Materials[i].name))
			return i;
	}

	return -1;
}

int CMaterialsList::LookupMaterialIndex(const char *name) const
{
	if (!name || !m_iMaterialCount)
		return -1;

	int first = 0;
	int last = m_iMaterialCount - 1;

	while (first <= last)
	{
		int index = (first + last) / 2;
		int result = strnicmp(name, m_Materials[index].name, 15);

		if (!result)
			return index;

		if (result > 0)
		{
			first = index + 1;
		}
		else
		{
			if (!index)
				return -1;

			last = index - 1;
		}
	}

	return -1;
}

char CMaterialsList::FindMaterialType(const char *name) const
{
	int index = LookupMaterialIndex(name);

	return index == -1 ? CHAR_TEX_CONCRETE : m_Materials[index].type;
}

void CMaterialsList::SortMaterials(void)
{
	for (int i = 0; i < m_iMaterialCount; i++)
	{
		for (int j = i + 1; j < m_iMaterialCount; j++)
		{
			if (stricmp(m_Materials[i].name, m_Materials[j].name) > 0)
			{
				materialentry_t entry = m_Materials[i];

				m_Materials[i] = m_Materials[j];
				m_Materials[j] = entry;
			}
		}
	}
}

void CMaterialsList::LoadMaterialsList(const char *customFile)
{
	char paths[2][260] = {};

	snprintf(paths[0], sizeof(paths[0]), "sound/materials.txt");

	if (customFile && *customFile)
		snprintf(paths[1], sizeof(paths[1]), "sound/%s/%s", UTIL_GetMapName(), customFile);

	for (int fileIndex = 0; fileIndex < 2 && paths[fileIndex][0]; fileIndex++)
	{
		MATERIAL_ALERT(at_console, "Loading materials list \"%s\"...\n", paths[fileIndex]);

		FileHandle_t file = OpenGameFile(paths[fileIndex], "r");

		if (!file)
		{
			MATERIAL_ALERT(at_error, "[Sound Engine] - Failed to open %s materials file \"%s\"\n",
				fileIndex ? "custom" : "core", paths[fileIndex]);
			continue;
		}

		char line[512];
		int lineNumber = 0;

		while (!g_pFileSystem->EndOfFile(file))
		{
			if (m_iMaterialCount > MAX_MATERIALS)
				break;

			if (m_iMaterialCount >= MAX_MATERIALS)
			{
				MATERIAL_ALERT(at_warning, "Maximum materials loaded (%d), aborting.\n", m_iMaterialCount);
				m_iMaterialCount--;
				break;
			}

			if (!g_pFileSystem->ReadLine(line, sizeof(line), file))
				continue;

			lineNumber++;

			if (!line[0] || !strlen(line) || (line[0] == '/' && line[1] == '/') ||
				line[0] == '#' || !isalpha((unsigned char)line[0]))
				continue;

			unsigned int length = strlen(line);

			if (line[length - 1] == '\n')
				length--;

			if (length && line[length - 1] == '\r')
				length--;

			line[length] = 0;

			int start = 0;

			while (line[start] && isspace((unsigned char)line[start]))
				start++;

			char type = toupper(line[start]);

			switch (type)
			{
			case CHAR_TEX_CONCRETE:
			case CHAR_TEX_DIRT:
			case CHAR_TEX_FLESH:
			case CHAR_TEX_GRATE:
			case CHAR_TEX_METAL:
			case CHAR_TEX_SNOW:
			case CHAR_TEX_COMPUTER:
			case CHAR_TEX_SLOSH:
			case CHAR_TEX_TILE:
			case CHAR_TEX_VENT:
			case CHAR_TEX_WOOD:
			case CHAR_TEX_GLASS:
				break;

			default:
				MATERIAL_ALERT(at_error, "[Sound Engine] - Unknown material type %c! (Line #%d)\n",
					line[start], lineNumber);
				continue;
			}

			start++;

			while (line[start] && isspace((unsigned char)line[start]))
				start++;

			int end = start;

			while (line[end] && !isspace((unsigned char)line[end]))
				end++;

			int nameLength = end - start;

			if (nameLength >= MAX_MATERIAL_NAME)
			{
				MATERIAL_ALERT(at_warning, "[Sound Engine] - Material name length too long! Greater than or equal to %d characters, material name will be chopped down. (Line #%d)\n",
					MAX_MATERIAL_NAME, lineNumber);

				do
					nameLength--;
				while (nameLength >= MAX_MATERIAL_NAME);
			}

			char *name = &line[start];

			while (name[nameLength] && isspace((unsigned char)name[nameLength]) && nameLength > 1)
				nameLength--;

			if (!nameLength)
			{
				MATERIAL_ALERT(at_error, "[Sound Engine] - Empty material name! (Line #%d)\n", lineNumber);
				continue;
			}

			char materialName[MAX_MATERIAL_NAME] = {};

			UTIL_CopyString(materialName, name, nameLength + 1);

			int index = LinearMaterialSearch(materialName);

			if (index == -1)
			{
				materialentry_t &entry = m_Materials[m_iMaterialCount];

				entry.type = type;
				UTIL_CopyString(entry.name, name, nameLength + 1);
				MATERIAL_ALERT(at_aiconsole, "[Sound Engine] - Material #%d loaded: \"%s\" as type '%c'\n",
					m_iMaterialCount, entry.name, entry.type);
				m_iMaterialCount++;
			}
			else
			{
				materialentry_t &entry = m_Materials[index];

				entry.type = type;
				UTIL_CopyString(entry.name, name, nameLength + 1);
				MATERIAL_ALERT(at_aiconsole, "[Sound Engine] - Material #%d replaced: \"%s\" as type '%c'\n",
					index, entry.name, entry.type);
			}
		}

		g_pFileSystem->Close(file);
		MATERIAL_ALERT(at_aiconsole, "[Sound Engine] - %d material(s) loaded.\n",
			m_Materials[0].type ? m_iMaterialCount : 0);
	}

	SortMaterials();
}
