#include "extdll.h"
#include "CASSoundEngine.h"
#include "cbase.h"
#include "ASSound.h"
#include "CASDocumentation.h"
#include "server_sound.h"

static const float AS_ATTN_NONE = ATTN_NONE;
static const float AS_ATTN_NORM = ATTN_NORM;
static const float AS_ATTN_IDLE = ATTN_IDLE;
static const float AS_ATTN_STATIC = ATTN_STATIC;
static const float AS_VOL_NORM = VOL_NORM;

void RegisterSCScriptSound( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "Sound flags", "SoundFlag" );
	documentation->RegisterEnumValue( "Volume flag", "SoundFlag", "SND_VOLUME", SOUNDMSG_VOLUME );
	documentation->RegisterEnumValue( "Pitch flag", "SoundFlag", "SND_PITCH", SOUNDMSG_PITCH );
	documentation->RegisterEnumValue( "Attenuation flag", "SoundFlag", "SND_ATTENUATION", SOUNDMSG_ATTENUATION );
	documentation->RegisterEnumValue( "Origin flag", "SoundFlag", "SND_ORIGIN", SOUNDMSG_ORIGIN );
	documentation->RegisterEnumValue( "Entity flag", "SoundFlag", "SND_ENT", SOUNDMSG_FOLLOW_ENTITY );
	documentation->RegisterEnumValue( "Stop flag", "SoundFlag", "SND_STOP", SOUNDMSG_STOP );
	documentation->RegisterEnumValue( "Change volume flag", "SoundFlag", "SND_CHANGE_VOL", SOUNDMSG_CHANGE_VOLUME );
	documentation->RegisterEnumValue( "Change pitch flag", "SoundFlag", "SND_CHANGE_PITCH", SOUNDMSG_CHANGE_PITCH );
	documentation->RegisterEnumValue( "Sentence flag", "SoundFlag", "SND_SENTENCE", SOUNDMSG_SENTENCE );
	documentation->RegisterEnumValue( "Refresh flag", "SoundFlag", "SND_REFRESH", SOUNDMSG_SPAWNING );
	documentation->RegisterEnumValue( "Force single flag", "SoundFlag", "SND_FORCE_SINGLE", SOUNDMSG_FORCE_ONCE );
	documentation->RegisterEnumValue( "Force loop flag", "SoundFlag", "SND_FORCE_LOOP", SOUNDMSG_FORCE_LOOP );
	documentation->RegisterEnumValue( "Linear flag", "SoundFlag", "SND_LINEAR", SOUNDMSG_RADIUS );
	documentation->RegisterEnumValue( "Skip Origin and use entity flag", "SoundFlag", "SND_SKIP_ORIGIN_USE_ENT", SOUNDMSG_SKIP_ORIGIN_USE_ENT );

	documentation->RegisterEnum( "Audio channels", "SOUND_CHANNEL" );
	documentation->RegisterEnumValue(
	    "Automatically selects a channel for use. You cannot stop sounds while using this channel with StopSound, "
	    "or in conjunction with the SND_STOP flag.",
	    "SOUND_CHANNEL", "CHAN_AUTO", CHAN_AUTO );
	documentation->RegisterEnumValue( "", "SOUND_CHANNEL", "CHAN_WEAPON", CHAN_WEAPON );
	documentation->RegisterEnumValue( "", "SOUND_CHANNEL", "CHAN_VOICE", CHAN_VOICE );
	documentation->RegisterEnumValue( "", "SOUND_CHANNEL", "CHAN_ITEM", CHAN_ITEM );
	documentation->RegisterEnumValue( "", "SOUND_CHANNEL", "CHAN_BODY", CHAN_BODY );
	documentation->RegisterEnumValue( "allocate stream channel from the static or dynamic area", "SOUND_CHANNEL", "CHAN_STREAM", CHAN_STREAM );
	documentation->RegisterEnumValue( "allocate channel from the static area", "SOUND_CHANNEL", "CHAN_STATIC", CHAN_STATIC );
	documentation->RegisterEnumValue( "", "SOUND_CHANNEL", "CHAN_MUSIC", CHAN_MUSIC );

	documentation->RegisterGlobalProperty( "", "const float ATTN_NONE", (void *) &AS_ATTN_NONE );
	documentation->RegisterGlobalProperty( "", "const float ATTN_NORM", (void *) &AS_ATTN_NORM );
	documentation->RegisterGlobalProperty( "", "const float ATTN_IDLE", (void *) &AS_ATTN_IDLE );
	documentation->RegisterGlobalProperty( "", "const float ATTN_STATIC", (void *) &AS_ATTN_STATIC );
	documentation->RegisterEnum( "Pitch values. Other values are possible. Range: 0 - 255, where 255 is very high.", "PITCH" );
	documentation->RegisterEnumValue( "non-pitch shifted", "PITCH", "PITCH_NORM", PITCH_NORM );
	documentation->RegisterEnumValue( "low pitch", "PITCH", "PITCH_LOW", PITCH_LOW );
	documentation->RegisterEnumValue( "high pitch", "PITCH", "PITCH_HIGH", PITCH_HIGH );
	documentation->RegisterGlobalProperty( "Volume value normal", "const float VOL_NORM", (void *) &AS_VOL_NORM );
	documentation->RegisterObjectType(
	    "Sound engine.\nCan either play sounds given to it directly, or use sound replacement to look up a replacement sample.",
	    "CSoundEngine", 0, asOBJ_REF | asOBJ_NOCOUNT );

	documentation->RegisterObjectMethod(
	    "Precaches the given sound.\nMust be called in MapInit",
	    "CSoundEngine", "void PrecacheSound(const string& in szFilename)",
	    asMETHOD( CASSoundEngine, PrecacheSound ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Precaches the given sound for the given entity. Supports entity sound replacement.\nMust be called in MapInit",
	    "CSoundEngine", "void PrecacheSound(CBaseEntity@ pEntity, const string& in szFilename)",
	    asMETHOD( CASSoundEngine, PrecacheSoundEnt ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Finds the sound replacement sample for the given entity and sample.If the sample is replaced, "
	    "it will return the sample used instead. Otherwise, szSample is returned.",
	    "CSoundEngine", "string FindSoundReplacementSample(CBaseEntity@ pEntity, const string& in szSample) const",
	    asMETHOD( CASSoundEngine, FindSoundReplacementSample ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Plays the given sound with the given parameters. Sound replacement is not used here.",
	    "CSoundEngine", "void PlaySound(edict_t@ entity, SOUND_CHANNEL channel, const string& in sample,"
	                    "float volume, float attenuation, int flags = 0, int pitch = PITCH_NORM,"
	                    "int target_ent_unreliable = 0, bool setOrigin = false, const Vector& in vecOrigin = g_vecZero)",
	    asMETHOD( CASSoundEngine, PlaySound ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Stops the given sound on the given entity on the given channel. If the sound was started using PlaySound, "
	    "set fUseSoundReplacement to false.",
	    "CSoundEngine", "void StopSound(edict_t@ entity, SOUND_CHANNEL channel, const string& in szSample, "
	                    "const bool fUseSoundReplacement = true)",
	    asMETHOD( CASSoundEngine, StopSound ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Plays the given sound with the given parameters. Sound replacement will affect this.",
	    "CSoundEngine", "void EmitSoundDyn(edict_t@ entity, SOUND_CHANNEL channel, const string& in szSample,"
	                    "float flVolume, float flAttenuation, int iFlags = 0, int iPitch = PITCH_NORM, int target_ent_unreliable = 0)",
	    asMETHOD( CASSoundEngine, EmitSoundDyn ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Plays the given sound with the given parameters. Sound replacement will affect this.",
	    "CSoundEngine", "void EmitSound(edict_t@ entity, SOUND_CHANNEL channel, const string& in szSample, float flVolume, float flAttenuation)",
	    asMETHOD( CASSoundEngine, EmitSound ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Play a specific sentence over the HEV suit speaker - just pass player entity, and !sentencename.",
	    "CSoundEngine", "void EmitSoundSuit(edict_t@ entity, const string& in szSample)",
	    asMETHOD( CASSoundEngine, EmitSoundSuit ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Play a sentence, randomly selected from the passed in group id, over the HEV suit speaker.",
	    "CSoundEngine", "void EmitGroupIdSuit(edict_t@ entity, int isentencereg)",
	    asMETHOD( CASSoundEngine, EmitGroupIdSuit ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Play a sentence, randomly selected from the passed in groupname.",
	    "CSoundEngine", "void EmitGroupNameSuit(edict_t@ entity, const string& in szSample)",
	    asMETHOD( CASSoundEngine, EmitGroupNameSuit ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Looks up the sentence index of the given sentence",
	    "CSoundEngine", "int LookupSentenceIndex(const string& in sentenceName)",
	    asMETHOD( CASSoundEngine, LookupSentenceIndex ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Looks up the sentence group index of the given sentence",
	    "CSoundEngine", "int LookupSentenceGroupIndex(const string& in szGroup)",
	    asMETHOD( CASSoundEngine, LookupSentenceGroupIndex ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Plays a sentence group",
	    "CSoundEngine", "int PlaySentenceGroup(edict_t@ entity, const string& in szGroupName, float volume, float attenuation, int flags, int pitch)",
	    asMETHODPR( CASSoundEngine, PlaySentenceGroup, (edict_t *, const CString &, float, float, int, int), int ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Plays a sentence group",
	    "CSoundEngine", "int PlaySentenceGroup(edict_t@ entity, int iGroupIndex, float volume, float attenuation, int flags, int pitch)",
	    asMETHODPR( CASSoundEngine, PlaySentenceGroup, (edict_t *, int, float, float, int, int), int ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Plays a sentence group sequentially",
	    "CSoundEngine", "int PlaySentenceGroupSequential(edict_t@ entity, const string& in szGroupName,"
	                    "float volume, float attenuation, int flags, int pitch, int ipick, const bool bReset)",
	    asMETHOD( CASSoundEngine, PlaySentenceGroupSequential ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Plays a sentence group sequentially",
	    "CSoundEngine", "void EmitAmbientSound(edict_t@ entity, const Vector& in vecOrigin, const string& in szSample,"
	                    "float flVolume, float flAttenuation, int fFlags, int iPitch)",
	    asMETHOD( CASSoundEngine, EmitAmbientSound ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Plays a hit sound based on the trace result's hit target. Returns the volume at which the hit is being played.",
	    "CSoundEngine", "float PlayHitSound( TraceResult& in tr, const Vector& in vecSrc, const Vector& in vecEnd, int iBulletType )",
	    asMETHOD( CASSoundEngine, PlayHitSound ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Maps the texture name to a material type. See the TextureType enum.",
	    "CSoundEngine", "char FindMaterialType(const string& in szMaterial) const",
	    asMETHOD( CASSoundEngine, FindMaterialType ), asCALL_THISCALL );

	documentation->RegisterGlobalProperty( "Sound engine instance", "CSoundEngine g_SoundSystem", CASSoundEngine::GetInstance() );
}
