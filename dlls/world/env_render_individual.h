#ifndef ENV_RENDER_INDIVIDUAL_H
#define ENV_RENDER_INDIVIDUAL_H

#include "utils/EntityHandle.h"

class CBasePlayer;

class CEnvRenderIndividual : public CBaseEntity
{
public:
	CEnvRenderIndividual() : m_bActive( false ) {}

	void Spawn() override;
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value ) override;

	void HandleEntityForPlayer( CBasePlayer *player, CBaseEntity *entity );
	void HandleEntity( CBaseEntity *activator, CBaseEntity *entity );
	void HandleEntities( CBaseEntity *activator );
	void EXPORT AutoApplyThink();

	int GetRenderMode( int original );
	int GetRenderAmt( int original );
	int GetRenderFx( int original );
	const Vector &GetRenderColor( const Vector &original );

private:
	bool m_bActive;

	EntityHandle<CBaseEntity> m_hCopyFrom;
};

#endif
