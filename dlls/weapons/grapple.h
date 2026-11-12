#ifndef GRAPPLE_H
#define GRAPPLE_H

#include "weapons/weapons.h"

class CBeam;

class CBarnacleGrappleTongue : public CBaseEntity
{
public:
	void Spawn( void );
	void Precache( void );
	void Touch( CBaseEntity *pOther );
	void Killed( entvars_t *pevAttacker, int iGib );
	void EXPORT FlyThink( void );
	void CheckMovement( void );
	bool CheckType( CBaseEntity *pEntity );
	int GetMonsterType( CBaseEntity *pEntity );
	void PullRappel( CBaseEntity *pEntity );
	void PullRappelEntity( void );

	static void Shoot( entvars_t *pevOwner, Vector vecOrigin, Vector vecAngles, CBasePlayer *pPlayer );

	CEntityHandle<CBeam>			m_hBeam;
	CEntityHandle<CBasePlayer>		m_hPlayer;
	EHANDLE							m_hTarget;
	int								m_iGrappleType;
	Vector							m_vecPullVelocity;
	float							m_flPullDelay;
	bool							m_fTerminate;
};

class CBarnacleGrapple : public CBasePlayerWeapon
{
public:
	void OnCreate( void );
	void Spawn( void );
	void Precache( void );
	bool GetItemInfo( ItemInfo *p );
	bool AddToPlayer( CBasePlayer *pPlayer );
	int iItemSlot( void ) { return 1; }
	float GetRespawnTime( void ) { return 3.0f; }
	bool Deploy( void );
	bool CanHolster( void );
	void Holster( int skiplocal = 0 );
	void ItemPostFrame( void );
	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void WeaponIdle( void );
	bool Swing( void );

	float	m_flNextFireSound;
	float	m_flNextPullSound;
	bool	m_fFire;
};

#endif
