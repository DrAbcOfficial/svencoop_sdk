#ifndef PLAYERAMMO_H
#define PLAYERAMMO_H

#include "weapons/weapons.h"

class CPlayerAmmo : public CBasePlayerAmmo
{
public:
	CPlayerAmmo( int ammoGive, const char *ammoName, int maxCarry )
	    : m_iAmmoGive( ammoGive ), m_pszAmmoName( ammoName ), m_iMaxCarry( maxCarry ) {}

	void Spawn( void );
	void Precache( void );
	BOOL AddAmmo( CBaseEntity *other );
	virtual const char *GetDefaultModel( void ) = 0;
	virtual const char *GetPickupSound( void );

	int         m_iAmmoGive;
	const char *m_pszAmmoName;
	int         m_iMaxCarry;
};

#endif
