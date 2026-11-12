#ifndef CASADMINCONTROL_H
#define CASADMINCONTROL_H

#include "scripting/core/CASServerScriptGlobal.h"

class CASDocumentation;
class CBasePlayer;
class Vector;

class CASAdminControl : public CASServerScriptGlobal<CASAdminControl>
{
public:
	virtual ~CASAdminControl();

	unsigned int GetReservedSlots( void ) const;
	void SetReservedSlots( unsigned int slots );
	bool KillPlayer( CBasePlayer *invoker, CBasePlayer *target, float respawnTime = -1.0f );
	bool KillPlayer( CBasePlayer *target, float respawnTime = -1.0f );
	bool KickPlayer( CBasePlayer *invoker, CBasePlayer *target, float banTime = -1.0f );
	bool KickPlayer( CBasePlayer *target, float banTime = -1.0f );
	bool BanPlayer( CBasePlayer *invoker, CBasePlayer *target, float banTime = -1.0f );
	bool BanPlayer( CBasePlayer *target, float banTime = -1.0f );
	bool SlapPlayer( CBasePlayer *invoker, CBasePlayer *target, float damage, const Vector &direction, int damageType = 0 );
	bool SlapPlayer( CBasePlayer *target, float damage, const Vector &direction, int damageType = 0 );
	bool SlapPlayer( CBasePlayer *invoker, CBasePlayer *target, float damage, int damageType = 0 );
	bool SlapPlayer( CBasePlayer *target, float damage, int damageType = 0 );

private:
	friend class CASBaseScriptGlobal<IASServerScriptGlobal, CASAdminControl>;

	CASAdminControl();
};

void RegisterSCScriptAdmin( CASDocumentation *documentation );

#endif
