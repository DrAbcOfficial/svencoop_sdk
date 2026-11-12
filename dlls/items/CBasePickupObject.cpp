#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "items/CBasePickupObject.h"
#include "scripting/core/CASHook.h"
#include "gamerules/game.h"

#define SF_ITEM_TOUCH_ONLY		128
#define SF_ITEM_USE_WITHOUT_LOS	512

void CBasePickupObject::OnCreate( void )
{
	CBasePhysicsObject::OnCreate();

	m_flCollectTime = 0;
	m_iUnknown180 = 0;
	m_vecMinHullSize = g_vecZero;
	m_RespawnDelay.OnCreate( "mp_weapon_respawndelay" );
}

void CBasePickupObject::KeyValue( KeyValueData *data )
{
	if ( !m_RespawnDelay.KeyValue( this, data ) )
		CBasePhysicsObject::KeyValue( data );
}

int CBasePickupObject::ObjectCaps( void )
{
	if ( ( pev->effects & EF_NODRAW ) || ( pev->spawnflags & SF_ITEM_TOUCH_ONLY ) )
		return FCAP_ACROSS_TRANSITION;

	return FCAP_ACROSS_TRANSITION | FCAP_IMPULSE_USE;
}

bool CBasePickupObject::CanCollect( CBaseEntity *other, COLLECT_TYPE collectType )
{
	if ( collectType == COLLECT_TOUCH )
	{
		if ( ( pev->spawnflags & SF_ITEM_USE_ONLY ) && !( pev->spawnflags & SF_GIVENITEM ) )
			return false;
	}
	else if ( collectType == COLLECT_USE_DIRECT )
	{
		if ( pev->spawnflags & SF_ITEM_TOUCH_ONLY )
			return false;

		if ( !( pev->spawnflags & SF_ITEM_USE_WITHOUT_LOS ) && !UTIL_HasLineOfSight( other, this, false, false ) )
			return false;
	}

	if ( ( pev->effects & EF_NODRAW ) && !( pev->spawnflags & SF_GIVENITEM ) )
		return false;

	if ( m_flCollectTime < 0 || m_flCollectTime > gpGlobals->time )
		return false;

	if ( !other || !other->IsPlayer() || !other->IsAlive() )
		return false;

	bool canCollect = true;

	g_CanCollectHook.Call( 0, this, other, &canCollect );
	return canCollect;
}

void CBasePickupObject::Collected( CBaseEntity *other, COLLECT_TYPE collectType )
{
	pev->renderamt = m_flOriginalRenderAmount;
	pev->rendermode = m_iOriginalRenderMode;
	g_CollectedHook.Call( 0, this, other );
	SUB_UseTargets( other, USE_TOGGLE, 0 );
}

void CBasePickupObject::DefaultTouch( CBaseEntity *other )
{
	CBasePhysicsObject::DefaultTouch( other );

	if ( !other->IsBSPModel() && CanCollect( other, COLLECT_TOUCH ) )
		Collect( other, COLLECT_TOUCH );
}

void CBasePickupObject::DefaultUse( CBaseEntity *activator, CBaseEntity *caller,
                                  USE_TYPE useType, float value )
{
	if ( !activator )
		return;

	if ( useType == USE_ON || useType == USE_SET || useType == USE_TOGGLE )
	{
		COLLECT_TYPE collectType = GetUseCollectType( activator, caller, useType );

		if ( CanCollect( activator, collectType ) )
			Collect( activator, collectType );
	}
	else if ( useType == USE_KILL )
		Kill();
}

void CBasePickupObject::Materialize( void )
{
	g_MaterializeHook.Call( 0, this );

	if ( pev->effects & EF_NODRAW )
	{
		EMIT_SOUND_DYN( edict(), CHAN_WEAPON, "items/suitchargeok1.wav", 1, ATTN_NORM, 0, 150 );
		pev->effects &= ~EF_NODRAW;
		pev->effects |= EF_MUZZLEFLASH;
	}

	UTIL_SetOrigin( pev, pev->origin );
	Item_SetDefaultSize( pev, m_vecMinHullSize, m_vecMaxHullSize );
	SetTouch( &CBasePickupObject::DefaultTouch );
	SetUse( &CBasePickupObject::CallDefaultUse );

	if ( ( pev->spawnflags & ( SF_DELAYREMOVE | SF_CREATEDWEAPON ) ) == ( SF_DELAYREMOVE | SF_CREATEDWEAPON ) )
	{
		SetThink( &CBasePickupObject::FadeStart );
		pev->nextthink = gpGlobals->time + GetFadeDelay();
	}
	else if ( pev->spawnflags & SF_GIVENITEM )
	{
		pev->effects |= EF_NODRAW;
		SetThink( &CBasePhysicsObject::CallKill );
		pev->nextthink = gpGlobals->time + 0.1;
	}
	else
		SetThink( NULL );
}

void CBasePickupObject::Dropped( CBaseEntity *dropper )
{
	pev->flags &= ~FL_ONGROUND;
	WorldInit();

	pev->spawnflags |= SF_DELAYREMOVE | SF_CREATEDWEAPON;
	m_flCollectTime = gpGlobals->time + 0.7f;
	m_fTouchedWorld = false;
}

float CBasePickupObject::GetFadeDelay( void )
{
	float delay = mp_weaponfadedelay.value;

	return delay < 0 ? 60.0f : delay;
}

void CBasePickupObject::FadeStart( void )
{
	if ( pev->rendermode == kRenderNormal )
	{
		pev->renderamt = 255;
		pev->rendermode = kRenderTransTexture;
	}

	SetThink( &CBasePickupObject::FadeThink );
	pev->nextthink = gpGlobals->time + 0.1;
}

void CBasePickupObject::FadeThink( void )
{
	if ( pev->renderamt > 10 )
		pev->renderamt -= 10;
	else
	{
		pev->renderamt = 0;
		SetThink( &CBasePhysicsObject::CallKill );
	}

	pev->nextthink = gpGlobals->time + 0.05;
}
