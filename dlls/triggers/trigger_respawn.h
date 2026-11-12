#ifndef TRIGGER_RESPAWN_H
#define TRIGGER_RESPAWN_H

class CTriggerRespawn : public CPointEntity
{
public:
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );

private:
	static void RespawnTarget( CBaseEntity *entity, bool allowLiving, bool respawnObserver, int respawnDelay );
};

#endif
