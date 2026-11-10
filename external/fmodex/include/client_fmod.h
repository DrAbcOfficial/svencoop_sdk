#ifndef CLIENT_FMOD_H
#define CLIENT_FMOD_H

#ifdef _WIN32
#define FMOD_CALL __stdcall
#define FMOD_IMPORT __declspec(dllimport)
#else
#define FMOD_CALL
#define FMOD_IMPORT
#endif

#define FMOD_VERSION	0x00044456

enum FMOD_RESULT
{
	FMOD_OK = 0,
	FMOD_ERR_FILE_BAD = 19,
	FMOD_ERR_FILE_NOTFOUND = 23,
	FMOD_ERR_FORMAT = 25,
	FMOD_ERR_INITIALIZATION = 31,
	FMOD_ERR_OUTPUT_CREATEBUFFER = 56
};

enum FMOD_OUTPUTTYPE { FMOD_OUTPUTTYPE_AUTODETECT = 0, FMOD_OUTPUTTYPE_NOSOUND = 2, FMOD_OUTPUTTYPE_DSOUND = 6 };
enum FMOD_SPEAKERMODE { FMOD_SPEAKERMODE_STEREO = 2 };
enum FMOD_DSP_TYPE { FMOD_DSP_TYPE_LOWPASS_SIMPLE = 18 };
enum FMOD_CHANNELINDEX { FMOD_CHANNEL_FREE = -1, FMOD_CHANNEL_REUSE = -2 };
enum FMOD_DELAYTYPE { FMOD_DELAYTYPE_DSPCLOCK_END = 2 };
enum FMOD_SOUND_TYPE { FMOD_SOUND_TYPE_UNKNOWN = 0, FMOD_SOUND_TYPE_OGGVORBIS = 13 };
enum FMOD_SOUND_FORMAT { FMOD_SOUND_FORMAT_NONE = 0, FMOD_SOUND_FORMAT_PCM16 = 2, FMOD_SOUND_FORMAT_PCMFLOAT = 5 };
enum FMOD_DSP_RESAMPLER { FMOD_DSP_RESAMPLER_LINEAR = 1 };
enum FMOD_SYSTEM_CALLBACKTYPE { FMOD_SYSTEM_CALLBACKTYPE_DEVICELISTCHANGED = 0 };

typedef unsigned int FMOD_MODE;
typedef unsigned int FMOD_TIMEUNIT;
typedef unsigned int FMOD_INITFLAGS;

const FMOD_MODE FMOD_LOOP_OFF = 0x00000001;
const FMOD_MODE FMOD_LOOP_NORMAL = 0x00000002;
const FMOD_MODE FMOD_2D = 0x00000008;
const FMOD_MODE FMOD_3D = 0x00000010;
const FMOD_MODE FMOD_SOFTWARE = 0x00000040;
const FMOD_MODE FMOD_CREATESAMPLE = 0x00000100;
const FMOD_MODE FMOD_CREATECOMPRESSEDSAMPLE = 0x00000200;
const FMOD_MODE FMOD_OPENMEMORY = 0x00000800;
const FMOD_MODE FMOD_OPENRAW = 0x00001000;
const FMOD_MODE FMOD_MPEGSEARCH = 0x00008000;
const FMOD_MODE FMOD_3D_WORLDRELATIVE = 0x00080000;
const FMOD_MODE FMOD_3D_LINEARROLLOFF = 0x00200000;
const FMOD_MODE FMOD_IGNORETAGS = 0x02000000;
const FMOD_TIMEUNIT FMOD_TIMEUNIT_MS = 1;
const FMOD_TIMEUNIT FMOD_TIMEUNIT_PCM = 2;
const FMOD_TIMEUNIT FMOD_TIMEUNIT_PCMBYTES = 4;
const FMOD_INITFLAGS FMOD_INIT_3D_RIGHTHANDED = 2;
const unsigned int FMOD_CAPS_HARDWARE_EMULATED = 0x00000002;
const unsigned int FMOD_MEMBITS_ALL = 0xffffffff;
const unsigned int FMOD_EVENT_MEMBITS_ALL = 0xffffffff;

struct FMOD_VECTOR
{
	float x;
	float y;
	float z;
};

struct FMOD_SYSTEM;
struct FMOD_SYNCPOINT;
struct FMOD_GUID;
struct FMOD_MEMORY_USAGE_DETAILS;
typedef FMOD_RESULT (FMOD_CALL *FMOD_SYSTEM_CALLBACK)(FMOD_SYSTEM *, FMOD_SYSTEM_CALLBACKTYPE, void *, void *);

struct FMOD_REVERB_PROPERTIES
{
	int				Instance;
	int				Environment;
	float			EnvDiffusion;
	int				Room;
	int				RoomHF;
	int				RoomLF;
	float			DecayTime;
	float			DecayHFRatio;
	float			DecayLFRatio;
	int				Reflections;
	float			ReflectionsDelay;
	int				Reverb;
	float			ReverbDelay;
	float			ModulationTime;
	float			ModulationDepth;
	float			HFReference;
	float			LFReference;
	float			Diffusion;
	float			Density;
	unsigned int	Flags;
};

struct FMOD_REVERB_CHANNELPROPERTIES
{
	int				Direct;
	int				Room;
	unsigned int	Flags;
	int				ConnectionPoint;
};

struct FMOD_CREATESOUNDEXINFO
{
	int					cbsize;
	unsigned int		length;
	unsigned int		fileoffset;
	int					numchannels;
	int					defaultfrequency;
	FMOD_SOUND_FORMAT	format;
	unsigned int		unknown24[28];
};

namespace FMOD
{
	class Sound;
	class Channel;
	class ChannelGroup;
	class DSP;
	class DSPConnection;

	class FMOD_IMPORT System
	{
	public:
		FMOD_RESULT FMOD_CALL release(void);
		FMOD_RESULT FMOD_CALL close(void);
		FMOD_RESULT FMOD_CALL update(void);
		FMOD_RESULT FMOD_CALL getVersion(unsigned int *version);
		FMOD_RESULT FMOD_CALL setCallback(FMOD_SYSTEM_CALLBACK callback);
		FMOD_RESULT FMOD_CALL getNumDrivers(int *count);
		FMOD_RESULT FMOD_CALL getDriverInfo(int id, char *name, int length, FMOD_GUID *guid);
		FMOD_RESULT FMOD_CALL getDriverCaps(int id, unsigned int *caps, int *frequency, FMOD_SPEAKERMODE *mode);
		FMOD_RESULT FMOD_CALL setSoftwareFormat(int frequency, FMOD_SOUND_FORMAT format,
			int channels, int maxinputchannels, FMOD_DSP_RESAMPLER resampler);
		FMOD_RESULT FMOD_CALL setDSPBufferSize(unsigned int length, int count);
		FMOD_RESULT FMOD_CALL setOutput(FMOD_OUTPUTTYPE output);
		FMOD_RESULT FMOD_CALL setSpeakerMode(FMOD_SPEAKERMODE mode);
		FMOD_RESULT FMOD_CALL init(int maxchannels, FMOD_INITFLAGS flags, void *extradata);
		FMOD_RESULT FMOD_CALL set3DSettings(float doppler, float distance, float rolloff);
		FMOD_RESULT FMOD_CALL set3DListenerAttributes(int listener, const FMOD_VECTOR *position,
			const FMOD_VECTOR *velocity, const FMOD_VECTOR *forward, const FMOD_VECTOR *up);
		FMOD_RESULT FMOD_CALL getMasterChannelGroup(ChannelGroup **group);
		FMOD_RESULT FMOD_CALL createChannelGroup(const char *name, ChannelGroup **group);
		FMOD_RESULT FMOD_CALL createDSPByType(FMOD_DSP_TYPE type, DSP **dsp);
		FMOD_RESULT FMOD_CALL createSound(const char *name, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO *extra, Sound **sound);
		FMOD_RESULT FMOD_CALL createStream(const char *name, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO *extra, Sound **sound);
		FMOD_RESULT FMOD_CALL playSound(FMOD_CHANNELINDEX index, Sound *sound, bool paused, Channel **channel);
		FMOD_RESULT FMOD_CALL getDSPClock(unsigned int *hi, unsigned int *lo);
		FMOD_RESULT FMOD_CALL setReverbProperties(const FMOD_REVERB_PROPERTIES *properties);
		FMOD_RESULT FMOD_CALL getMemoryInfo(unsigned int memorybits, unsigned int event_memorybits,
			unsigned int *memoryused, FMOD_MEMORY_USAGE_DETAILS *memoryused_details);
	};

	class FMOD_IMPORT Sound
	{
	public:
		FMOD_RESULT FMOD_CALL release(void);
		FMOD_RESULT FMOD_CALL lock(unsigned int offset, unsigned int length, void **ptr1, void **ptr2,
			unsigned int *len1, unsigned int *len2);
		FMOD_RESULT FMOD_CALL unlock(void *ptr1, void *ptr2, unsigned int len1, unsigned int len2);
		FMOD_RESULT FMOD_CALL getDefaults(float *frequency, float *volume, float *pan, int *priority);
		FMOD_RESULT FMOD_CALL getLength(unsigned int *length, FMOD_TIMEUNIT type);
		FMOD_RESULT FMOD_CALL getName(char *name, int length);
		FMOD_RESULT FMOD_CALL getFormat(FMOD_SOUND_TYPE *type, FMOD_SOUND_FORMAT *format, int *channels, int *bits);
		FMOD_RESULT FMOD_CALL getNumSyncPoints(int *count);
		FMOD_RESULT FMOD_CALL getSyncPoint(int index, FMOD_SYNCPOINT **point);
		FMOD_RESULT FMOD_CALL getSyncPointInfo(FMOD_SYNCPOINT *point, char *name, int namelen,
			unsigned int *offset, FMOD_TIMEUNIT type);
		FMOD_RESULT FMOD_CALL setMode(FMOD_MODE mode);
		FMOD_RESULT FMOD_CALL getMode(FMOD_MODE *mode);
		FMOD_RESULT FMOD_CALL setLoopPoints(unsigned int start, FMOD_TIMEUNIT starttype,
			unsigned int end, FMOD_TIMEUNIT endtype);
	};

	class FMOD_IMPORT Channel
	{
	public:
		FMOD_RESULT FMOD_CALL stop(void);
		FMOD_RESULT FMOD_CALL setPaused(bool paused);
		FMOD_RESULT FMOD_CALL setVolume(float volume);
		FMOD_RESULT FMOD_CALL setFrequency(float frequency);
		FMOD_RESULT FMOD_CALL setDelay(FMOD_DELAYTYPE type, unsigned int hi, unsigned int lo);
		FMOD_RESULT FMOD_CALL setPriority(int priority);
		FMOD_RESULT FMOD_CALL setPosition(unsigned int position, FMOD_TIMEUNIT type);
		FMOD_RESULT FMOD_CALL getPosition(unsigned int *position, FMOD_TIMEUNIT type);
		FMOD_RESULT FMOD_CALL setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES *properties);
		FMOD_RESULT FMOD_CALL getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES *properties);
		FMOD_RESULT FMOD_CALL setChannelGroup(ChannelGroup *group);
		FMOD_RESULT FMOD_CALL set3DAttributes(const FMOD_VECTOR *position, const FMOD_VECTOR *velocity);
		FMOD_RESULT FMOD_CALL set3DMinMaxDistance(float minimum, float maximum);
		FMOD_RESULT FMOD_CALL set3DPanLevel(float level);
		FMOD_RESULT FMOD_CALL isPlaying(bool *playing);
		FMOD_RESULT FMOD_CALL getWaveData(float *array, int count, int channeloffset);
		FMOD_RESULT FMOD_CALL setMode(FMOD_MODE mode);
		FMOD_RESULT FMOD_CALL getMode(FMOD_MODE *mode);
	};

	class FMOD_IMPORT ChannelGroup
	{
	public:
		FMOD_RESULT FMOD_CALL stop(void);
		FMOD_RESULT FMOD_CALL setVolume(float volume);
		FMOD_RESULT FMOD_CALL setMute(bool mute);
		FMOD_RESULT FMOD_CALL addGroup(ChannelGroup *group);
		FMOD_RESULT FMOD_CALL addDSP(DSP *dsp, DSPConnection **connection);
	};

	class FMOD_IMPORT DSP
	{
	public:
		FMOD_RESULT FMOD_CALL setBypass(bool bypass);
		FMOD_RESULT FMOD_CALL setParameter(int index, float value);
	};
}

extern "C" FMOD_IMPORT FMOD_RESULT FMOD_CALL FMOD_System_Create(FMOD::System **system);
const char *FMOD_ErrorString(FMOD_RESULT result);

#endif
