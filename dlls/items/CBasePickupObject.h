#ifndef CBASEPICKUPOBJECT_H
#define CBASEPICKUPOBJECT_H

#include "items/CBasePhysicsObject.h"
#include "gamerules/CCustomRespawnDelay.h"
#include "items/PickupUtils.h"

class CASDocumentation;

class CBasePickupObject : public CBasePhysicsObject
{
	friend class CBaseMonster;
	friend class CBasePlayer;
	template <class T>
	friend void RegisterSCScriptBasePlayerItem( CASDocumentation *documentation, const char *name );

public:
	void OnCreate( void );
	void KeyValue( KeyValueData *data );
	int ObjectCaps( void );
	void Materialize( void );

	virtual bool CanCollect( CBaseEntity *other, COLLECT_TYPE collectType );
	virtual void Collect( CBaseEntity *other, COLLECT_TYPE collectType ) = 0;
	virtual void Collected( CBaseEntity *other, COLLECT_TYPE collectType );
	virtual void EXPORT DefaultUse( CBaseEntity *activator, CBaseEntity *caller,
	                                USE_TYPE useType, float value );
	void EXPORT CallDefaultUse( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value ) { DefaultUse( activator, caller, useType, value ); }
	virtual void Dropped( CBaseEntity *dropper );
	virtual float GetFadeDelay( void );

	void EXPORT FadeStart( void );
	void EXPORT FadeThink( void );
	void EXPORT DefaultTouch( CBaseEntity *other );

protected:
	float               m_flCollectTime;
	int                 m_iUnknown180;
	Vector              m_vecMinHullSize;
	Vector              m_vecMaxHullSize;

public:
	CCustomRespawnDelay m_RespawnDelay;
};

#endif
