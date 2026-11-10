/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#ifndef ITEMS_H
#define ITEMS_H

#include "CCustomRespawnDelay.h"
#include "PickupUtils.h"

class CItem : public CBaseEntity
{
public:
	void OnCreate( void );
	void KeyValue( KeyValueData *data );
	int ObjectCaps( void );
	void	Spawn( void );
	CBaseEntity*	Respawn( void );
	void	EXPORT ItemTouch( CBaseEntity *pOther );
	virtual BOOL MyTouch( CBasePlayer *pPlayer ) { return FALSE; };
	virtual void EXPORT Materialize( void );
	void EXPORT CallMaterialize( void ) { Materialize(); }
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );
	bool CanCollect( CBaseEntity *other, COLLECT_TYPE collectType );
	void Collect( CBaseEntity *other, COLLECT_TYPE collectType );

	string_t            m_iszSequence;
	CCustomRespawnDelay m_RespawnDelay;
};

#endif // ITEMS_H
