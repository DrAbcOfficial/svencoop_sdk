#include "extdll.h"
#include "hud_iface.h"
#include "parsemsg.h"
#include "client_sound.h"

int __MsgFunc_StartSound(const char *name, int size, void *buffer)
{
	BEGIN_READ(buffer, size);

	int flags = READ_SHORT();
	int entityIndex = (flags & SOUNDMSG_FOLLOW_ENTITY) ? READ_SHORT() : -1;
	float volume = (flags & SOUNDMSG_VOLUME) ? (float)READ_BYTE() / 255.0f : 1.0f;
	int pitch = (flags & SOUNDMSG_PITCH) ? READ_BYTE() : PITCH_NORM;
	float attenuation = (flags & SOUNDMSG_ATTENUATION) ? (float)READ_BYTE() : ATTN_NORM;
	float origin[3] = { 0, 0, 0 };
	float offset = 0;

	if (!(flags & SOUNDMSG_RADIUS) && (flags & SOUNDMSG_ATTENUATION))
		attenuation /= 64.0f;

	if (flags & SOUNDMSG_ORIGIN)
	{
		origin[0] = (float)(READ_LONG() * 0.125);
		origin[1] = (float)(READ_LONG() * 0.125);
		origin[2] = (float)(READ_LONG() * 0.125);
	}

	if (flags & SOUNDMSG_OFFSET)
		offset = READ_FLOAT();

	int channelIndex = READ_BYTE();
	int index = READ_SHORT();

	if (flags & SOUNDMSG_SENTENCE)
	{
		if (index < MAX_SOUNDLIST_SENTENCES)
		{
			CClient_SoundEngine *sound = CClient_SoundEngine::GetSoundEngine();

			sound->PlayFMODSound(flags, entityIndex, origin, channelIndex, NULL,
				volume, attenuation, 0, pitch, index, 0);
		}
	}
	else
	{
		CClient_SoundEngine *sound = CClient_SoundEngine::GetSoundEngine();

		sound->PlayFMODSound(flags, entityIndex, origin, channelIndex,
			sound->LookupSoundByIndex(index), volume, attenuation, 0, pitch, -1, offset);
	}

	return 1;
}

int __MsgFunc_Speaksent(const char *name, int size, void *buffer)
{
	BEGIN_READ(buffer, size);

	char command[1024] = { 0 };

	_snprintf(command, sizeof(command), "speak %s\n", READ_STRING());
	command[sizeof(command) - 1] = 0;
	gEngfuncs.pfnClientCmd(command);
	return 1;
}
