#include "extdll.h"
#include "hud_iface.h"
#include "client_filesystem.h"
#include "client_sound.h"
#include "client_support.h"
#include <ctype.h>

void AlertMessage(ALERT_TYPE type, char *format, ...);

bool CClient_SoundEngine::AddSentence(sndSentence_t *sentence)
{
	if (m_iSentenceCount >= MAX_SOUNDLIST_SENTENCES)
		return false;

	m_pSentences[m_iSentenceCount++] = sentence;
	return true;
}

const char *CClient_SoundEngine::LookupSoundBySentenceIndex(int index)
{
	if ((unsigned int)index > 4095)
	{
		AlertMessage(at_error, "Tried to look up sound by sentence index out of bounds.\n");
		return NULL;
	}

	if (m_pSentences[index] && m_pSentences[index]->words)
		return m_pSentences[index]->words->sample;

	return NULL;
}

bool CClient_SoundEngine::ParseSentenceLine(char *line)
{
	char name[32] = {};
	int start = 0;

	while (line[start] && isspace((unsigned char)line[start]))
		start++;

	if (!line[start])
		return false;

	int end = start;

	while (line[end] && !isspace((unsigned char)line[end]))
		end++;

	int length = end - start;

	if (length >= 31)
	{
		AlertMessage(at_console, "Sentence name length too long! Greater than %d characters!\n", 31);
		return false;
	}

	char *source = &line[start];

	while (source[length] && isspace((unsigned char)source[length]) && length > 1)
		length--;

	strncpy(name, source, length + 1);
	name[length + 1] = 0;

	char *words = TrimString(&line[end]);

	if (!words)
		return false;

	if (strlen(words) >= 384)
	{
		AlertMessage(at_error, "Sentence length too long! Greater than %d characters!\n", 384);
		return false;
	}

	sndSentence_t *sentence = new sndSentence_t;

	UTIL_CopyString(sentence->name, name, sizeof(sentence->name));
	sentence->words = ParseSentenceWords(words);

	if (!AddSentence(sentence))
	{
		delete sentence;
		return false;
	}

	return true;
}

bool CClient_SoundEngine::ParseSentenceDirectory(char **words, char *directory, int size)
{
	int length = strlen(*words);

	while (length > 1 && (*words)[length - 1] != '/')
		length--;

	if (length <= 1)
		return false;

	strncpy(directory, *words, length < size ? length : size);
	*words += length;
	return true;
}

sndWord_t *CClient_SoundEngine::ParseSentenceWords(char *words)
{
	char directory[256] = {};

	if (!ParseSentenceDirectory(&words, directory, sizeof(directory)))
		strncpy(directory, "vox/", sizeof(directory));

	char *entries[32] = {};
	char *cursor = words;

	entries[0] = words;

	for (int i = 1; i < 32; i++)
	{
		while (*cursor && *cursor != ' ' && *cursor != ',' && *cursor != '.')
		{
			if (*cursor == '(')
			{
				char previous;

				do
				{
					previous = *cursor++;
				}
				while (previous != ')' && *cursor);

				break;
			}

			cursor++;
		}

		if (!*cursor)
			break;

		if (*cursor == ',' || *cursor == '.')
		{
			entries[i++] = *cursor == ',' ? "_comma" : "_period";

			if (i >= 32)
				break;
		}

		*cursor++ = 0;

		while (*cursor == ' ')
			cursor++;

		if (!*cursor)
			break;

		entries[i] = cursor;
	}

	sndWord_t *first = NULL;
	sndWord_t *last = NULL;

	for (int i = 0; entries[i]; i++)
	{
		sndWord_t *word = ParseWordEntry(entries[i], directory, i == 0);

		if (!word)
			continue;

		if (first)
			last->next = word;
		else
			first = word;

		last = word;
	}

	return first;
}

sndWord_t *CClient_SoundEngine::ParseWordEntry(char *entry, char *directory, bool resetDefaults)
{
	static sndWord_t DefaultParams;

	if (resetDefaults)
	{
		DefaultParams.volume = 100;
		DefaultParams.pitch = 100;
		DefaultParams.start = 0;
		DefaultParams.end = 100;
		DefaultParams.timeCompress = 100;
		DefaultParams.next = NULL;
	}

	sndWord_t *word = new sndWord_t(DefaultParams);
	char number[4] = {};
	char *cursor = entry;

	if (entry[strlen(entry) - 1] == ')')
	{
		while (*cursor)
		{
			while (*cursor && *cursor != '(')
			{
				if (*cursor == ')')
					goto parametersDone;

				cursor++;
			}

			*cursor++ = 0;

			while (*cursor && *cursor != 'v' && *cursor != 'p' && *cursor != 's' &&
				*cursor != 'e' && *cursor != 't')
				cursor++;

			char parameter = *cursor++;

			if (!isdigit((unsigned char)*cursor))
				break;

			for (unsigned int i = 0; *cursor && i < sizeof(number) && isdigit((unsigned char)*cursor); i++)
				number[i] = *cursor++;

			int value = atoi(number);

			switch (parameter)
			{
			case 'v':
				word->volume = value;
				break;

			case 'p':
				word->pitch = value;
				break;

			case 's':
				word->start = value < 0 ? 0 : value > 100 ? 100 : value;
				break;

			case 'e':
				word->end = value < 0 ? 0 : value > 100 ? 100 : value;
				break;

			case 't':
				word->timeCompress = value;
				break;
			}
		}

parametersDone:
		if (!strlen(entry))
		{
			DefaultParams.volume = word->volume;
			DefaultParams.pitch = word->pitch;
			DefaultParams.start = word->start;
			DefaultParams.end = word->end;
			DefaultParams.timeCompress = word->timeCompress;
			delete word;
			return NULL;
		}
	}

	char extension[6];

	if (findWordAudioFile(directory, entry, extension, sizeof(extension)))
		snprintf(word->sample, sizeof(word->sample), "%s%s%s", directory, entry, extension);

	return word;
}

bool CClient_SoundEngine::findWordAudioFile(char *directory, char *word, char *sample, unsigned int size)
{
	static const char *extensions[] = {
		".wav", ".fsb", ".flac", ".ogg", ".oga", ".mp3", ".mp2", ".wma", ".aac"
	};
	char path[260];

	gEngfuncs.pfnGetGameDirectory();

	for (int i = 0; i < 9; i++)
	{
		snprintf(sample, size, "%s", extensions[i]);
		snprintf(path, sizeof(path), "sound/%s%s%s", directory, word, sample);

		if (g_pFileSystem->FileExists(path))
			return true;
	}

	return false;
}
