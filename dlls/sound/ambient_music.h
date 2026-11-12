#ifndef AMBIENT_MUSIC_H
#define AMBIENT_MUSIC_H

#include "utils/EntityHandle.h"
#include <unordered_map>

class CBasePlayer;

class CAmbientMusic : public CPointEntity
{
public:
	CAmbientMusic();

	void KeyValue( KeyValueData *pkvd ) override;
	void Precache() override;
	void Spawn() override;
	void Think() override;
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value ) override;
	void UpdateOnRemove() override;
	int ObjectCaps() override { return 0; }

	void Play( float offset = 0 );
	void Stop();
	void PlayForActivator( CBaseEntity *activator, float offset = 0 );
	void StopForActivator( CBaseEntity *activator );
	void NotifyClient( bool playing, int client, float offset );
	void UpdateClients( float offset = 0 );
	bool CheckClient( CBasePlayer *player );

private:
	float m_flStartTime;
	float m_flVolume;
	bool  m_bStartSilent;
	bool  m_bLooping;
	bool  m_bActivatorOnly;

	typedef std::unordered_map<EntityHandle<CBasePlayer>, float,
	                           EntityHandle<CBasePlayer>::Hash, EntityHandle<CBasePlayer>::Equal> ClientMap;
	ClientMap m_Clients;
};

#endif
