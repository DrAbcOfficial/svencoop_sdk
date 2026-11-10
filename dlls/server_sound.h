#ifndef SERVER_SOUND_H
#define SERVER_SOUND_H

#include "materials.h"
#include "sound_message.h"

#define MAX_SENTENCE_NAME		32
#define MAX_SENTENCE_TEXT		384
#define MAX_SENTENCE_GROUPS		512
#define MAX_GROUP_SENTENCES		64

struct server_sentence_t
{
	char	name[MAX_SENTENCE_NAME];
	char	text[MAX_SENTENCE_TEXT];
};

struct server_sentencegroup_t
{
	char	name[MAX_SENTENCE_NAME];
	int		sentences[MAX_GROUP_SENTENCES];
	int		shuffledSentences[MAX_GROUP_SENTENCES];
	int		nextSentence;
	int		count;
};

class CServer_SoundEngine
{
public:
	CServer_SoundEngine();

	static CServer_SoundEngine *GetSoundEngine(void);
	void Initialize(void);
	void Precache(void);
	void PrecacheSound(const char *sample);
	int LookupSoundIndex(char *sample);
	int LookupSentenceIndex(char *name);
	char *LookupSentenceName(int index);
	int LookupSentenceGroupIndex(const char *name);
	void CategorizeSentence(const char *name, int index);
	void LoadSentenceList(void);
	void LoadMaterialsList(void);
	void WriteSoundList(bool force);

	void PlaySound(edict_t *entity, int channel, char *sample, float volume,
		float attenuation, int flags, int pitch, int targetPlayer,
		bool useOrigin, Vector origin, float offset);
	int PlaySentenceGroup(edict_t *entity, const char *name, float volume,
		float attenuation, int flags, int pitch, int channel);
	int PlaySentenceGroup(edict_t *entity, int index, float volume,
		float attenuation, int flags, int pitch, int channel);
	int PlaySentenceGroupSequential(edict_t *entity, const char *name,
		float volume, float attenuation, int flags, int pitch,
		int sentenceNumber, bool reset, int channel);

	CMaterialsList			m_Materials;
	char					m_szSentenceFile[260];
	char					m_szMaterialsFile[260];
	const char				*m_Sounds[MAX_SOUNDLIST_SOUNDS];
	server_sentence_t		m_Sentences[MAX_SOUNDLIST_SENTENCES];
	server_sentencegroup_t	m_SentenceGroups[MAX_SENTENCE_GROUPS];
	int						m_iLastSound;
	bool					m_bSoundListGenerated;
};

extern float g_flNextAmbientRefresh;

extern "C" void PM_PlaySoundFX_SERVER(int playerIndex, float *origin, int channel,
	char *sample, float volume, float attenuation, int flags, int pitch);
extern "C" char PM_FindMaterialType_SERVER(char *name);

#endif
