#include "extdll.h"
#include "scripting/core/CASSoundEngine.h"
#include "cbase.h"
#include "sound/server_sound.h"

CASSoundEngine::~CASSoundEngine()
{
}

void CASSoundEngine::PrecacheSound( const CString &sample )
{
	if ( !sample.IsEmpty() )
	{
		CServer_SoundEngine *soundEngine = CServer_SoundEngine::GetSoundEngine();
		const char *pooledSample = STRING( AllocStringLocal( sample.Data() ) );

		soundEngine->PrecacheSound( pooledSample );
	}
}

void CASSoundEngine::PrecacheSoundEnt( CBaseEntity *entity, const CString &sample )
{
	if ( !sample.IsEmpty() )
		PrecacheSound_Intercept( entity, STRING( AllocStringLocal( sample.Data() ) ) );
}

CString CASSoundEngine::FindSoundReplacementSample( CBaseEntity *entity, const CString &sample ) const
{
	return CString( ::FindSoundReplacementSample( entity, sample.Data() ) );
}

void CASSoundEngine::PlaySound( edict_t *entity, int channel, const CString &sample, float volume,
                                float attenuation, int flags, int pitch, int targetPlayer, bool useOrigin, const Vector &origin )
{
	CServer_SoundEngine::GetSoundEngine()->PlaySound( entity, channel, (char *) sample.Data(),
	                                                  volume, attenuation, flags, pitch, targetPlayer, useOrigin, origin, 0 );
}

void CASSoundEngine::StopSound( edict_t *entity, int channel, const CString &sample, bool useSoundReplacement )
{
	if ( useSoundReplacement )
	{
		EMIT_SOUND_DYN( entity, channel, sample.Data(), 0, 0, SOUNDMSG_STOP, PITCH_NORM, 0, false, g_vecZero );
	}
	else
	{
		CServer_SoundEngine::GetSoundEngine()->PlaySound( entity, channel, (char *) sample.Data(),
		                                                  1, 1, SOUNDMSG_STOP, PITCH_NORM, 0, false, g_vecZero, 0 );
	}
}

void CASSoundEngine::EmitSoundDyn( edict_t *entity, int channel, const CString &sample, float volume,
                                   float attenuation, int flags, int pitch, int targetPlayer )
{
	EMIT_SOUND_DYN( entity, channel, sample.Data(), volume, attenuation, flags, pitch, targetPlayer, false, g_vecZero );
}

void CASSoundEngine::EmitSound( edict_t *entity, int channel, const CString &sample, float volume, float attenuation )
{
	EMIT_SOUND_DYN( entity, channel, sample.Data(), volume, attenuation, 0, PITCH_NORM, 0, false, g_vecZero );
}

void CASSoundEngine::EmitSoundSuit( edict_t *entity, const CString &sample )
{
	EMIT_SOUND_SUIT( entity, sample.Data() );
}

void CASSoundEngine::EmitGroupIdSuit( edict_t *entity, int group )
{
	EMIT_GROUPID_SUIT( entity, group );
}

void CASSoundEngine::EmitGroupNameSuit( edict_t *entity, const CString &sample )
{
	EMIT_GROUPNAME_SUIT( entity, sample.Data() );
}

int CASSoundEngine::LookupSentenceIndex( const CString &name )
{
	return CServer_SoundEngine::GetSoundEngine()->LookupSentenceIndex( (char *) name.Data() );
}

int CASSoundEngine::LookupSentenceGroupIndex( const CString &name )
{
	return CServer_SoundEngine::GetSoundEngine()->LookupSentenceGroupIndex( name.Data() );
}

int CASSoundEngine::PlaySentenceGroup( edict_t *entity, const CString &name, float volume,
                                       float attenuation, int flags, int pitch )
{
	return CServer_SoundEngine::GetSoundEngine()->PlaySentenceGroup( entity, name.Data(), volume, attenuation, flags, pitch, -1 );
}

int CASSoundEngine::PlaySentenceGroup( edict_t *entity, int group, float volume, float attenuation, int flags, int pitch )
{
	return CServer_SoundEngine::GetSoundEngine()->PlaySentenceGroup( entity, group, volume, attenuation, flags, pitch, -1 );
}

int CASSoundEngine::PlaySentenceGroupSequential( edict_t *entity, const CString &name, float volume,
                                                 float attenuation, int flags, int pitch, int sentence, bool reset )
{
	return CServer_SoundEngine::GetSoundEngine()->PlaySentenceGroupSequential( entity, name.Data(),
	                                                                           volume, attenuation, flags, pitch, sentence, reset, -1 );
}

void CASSoundEngine::EmitAmbientSound( edict_t *entity, const Vector &origin, const CString &sample,
                                       float volume, float attenuation, int flags, int pitch )
{
	UTIL_EmitAmbientSound( entity, origin, sample.Data(), volume, attenuation, flags, pitch );
}

float CASSoundEngine::PlayHitSound( TraceResult &trace, const Vector &source, const Vector &end, int bulletType )
{
	return TEXTURETYPE_PlaySound( &trace, source, end, bulletType );
}

char CASSoundEngine::FindMaterialType( const CString &material ) const
{
	return CServer_SoundEngine::GetSoundEngine()->m_Materials.FindMaterialType( material.Data() );
}
