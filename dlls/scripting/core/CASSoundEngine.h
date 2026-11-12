#ifndef CASSOUNDENGINE_H
#define CASSOUNDENGINE_H

#include "util.h"
#include "scripting/core/CASServerScriptGlobal.h"
#include "CString.h"

class CASSoundEngine : public CASServerScriptGlobal<CASSoundEngine>
{
public:
	virtual ~CASSoundEngine();

	void PrecacheSound( const CString &sample );
	void PrecacheSoundEnt( CBaseEntity *entity, const CString &sample );
	CString FindSoundReplacementSample( CBaseEntity *entity, const CString &sample ) const;
	void PlaySound( edict_t *entity, int channel, const CString &sample, float volume,
	                float attenuation, int flags, int pitch, int targetPlayer, bool useOrigin, const Vector &origin );
	void StopSound( edict_t *entity, int channel, const CString &sample, bool useSoundReplacement );
	void EmitSoundDyn( edict_t *entity, int channel, const CString &sample, float volume,
	                   float attenuation, int flags, int pitch, int targetPlayer );
	void EmitSound( edict_t *entity, int channel, const CString &sample, float volume, float attenuation );
	void EmitSoundSuit( edict_t *entity, const CString &sample );
	void EmitGroupIdSuit( edict_t *entity, int group );
	void EmitGroupNameSuit( edict_t *entity, const CString &sample );
	int LookupSentenceIndex( const CString &name );
	int LookupSentenceGroupIndex( const CString &name );
	int PlaySentenceGroup( edict_t *entity, const CString &name, float volume, float attenuation, int flags, int pitch );
	int PlaySentenceGroup( edict_t *entity, int group, float volume, float attenuation, int flags, int pitch );
	int PlaySentenceGroupSequential( edict_t *entity, const CString &name, float volume,
	                                 float attenuation, int flags, int pitch, int sentence, bool reset );
	void EmitAmbientSound( edict_t *entity, const Vector &origin, const CString &sample,
	                       float volume, float attenuation, int flags, int pitch );
	float PlayHitSound( TraceResult &trace, const Vector &source, const Vector &end, int bulletType );
	char FindMaterialType( const CString &material ) const;
};

#endif
