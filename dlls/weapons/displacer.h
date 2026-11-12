#ifndef DISPLACER_H
#define DISPLACER_H

#include "weapons/weapons.h"

#include "utils/TypedEntityHandle.h"

// v_displacer.mdl
enum displacer_e
{
	DISPLACER_IDLE1 = 0,
	DISPLACER_IDLE2,
	DISPLACER_SPINUP,
	DISPLACER_SPIN,
	DISPLACER_FIRE,
	DISPLACER_DRAW,
	DISPLACER_HOLSTER1
};

// CDisplacer::m_iFireMode (CDisplacer::AttackMode_t)
#define DISPLACER_ATTACK_NONE		0
#define DISPLACER_ATTACK_PRIMARY	1
#define DISPLACER_ATTACK_SECONDARY	2
#define DISPLACER_ATTACK_TERTIARY	3

// CDisplacer::m_TertiaryMode
#define DISPLACER_TERTIARY_DISABLED	1
#define DISPLACER_TERTIARY_ENABLED	2

// defaults for the mapper settable keyvalues
#define DISPLACER_PORTAL_SPEED		500
#define DISPLACER_PRIMARY_AMMO		20
#define DISPLACER_SECONDARY_AMMO	60
#define DISPLACER_TERTIARY_AMMO		50

#define DISPLACER_VOLUME			450

class CDisplacer : public CBasePlayerWeapon
{
public:
	void	Spawn( void );
	void	Precache( void );
	void	KeyValue( KeyValueData *pkvd );
	int		iItemSlot( void ) { return 3; }
	bool	GetItemInfo( ItemInfo *p );
	bool	AddToPlayer( CBasePlayer *pPlayer );
	CBaseEntity *Respawn( void );

	void	PrimaryAttack( void );
	void	SecondaryAttack( void );
	void	TertiaryAttack( void );
	bool	Deploy( void );
	void	Holster( int skiplocal = 0 );
	void	WeaponIdle( void );
	bool	PlayEmptySound( void );

	void	ProjectileAttack( float flRecoil, int iAmmoNeeded, float flSpinTime, float flFireTime, int iMode, const char *pszSpinSound, float flThinkDelay, void ( CBaseEntity::*pfnThink )( void ) );
	void	FireProjectile( const Vector &vecDir, float flDamage, int iAmmo, float flRadius );
	void	EXPORT FirePrimary( void );
	void	EXPORT FireSecondary( void );
	void	EXPORT StartSpinTertiary( void );
	void	EXPORT EndSpinTertiary( void );
	void	FireTertiary( void );
	void	CancelFire( void );
	bool	AllowTertiaryFire( void ) const;

	void	GetWeaponData( weapon_data_s *data );
	void	SetWeaponData( weapon_data_s *data );

	bool	UseDecrement( void );

	unsigned short	m_usDisplacer;
	int				m_iFireMode;
	int				m_TertiaryMode;
	float			m_flPortalSpeed;
	float			m_flPortalRadius;
	string_t		m_iszTeleportDestination;
	float			m_flPrimaryAmmoNeeded;
	float			m_flSecondaryAmmoNeeded;
	float			m_flTertiaryAmmoNeeded;
};

class CBeam;

class CDisplacerPortal : public CBaseEntity
{
public:
	void Spawn( void );
	void Precache( void );
	void EXPORT BeamThink( void );
	void EXPORT ProjectileTouch( CBaseEntity *pOther );
	void EXPORT RadiusDmgThink( void );
	void ClearBeams( void );
	void Detonate( void );
	void DoRadiusDamage( float flRadius );

	static CDisplacerPortal *Create( const Vector &vecOrigin, const Vector &vecVelocity, edict_t *pOwner, float flDamage, float flRadius );

	int							m_iRingSprite;
	CEntityHandle<CBeam>			m_hBeams[ 8 ];
	CEntityHandle<CBaseEntity>	m_hTarget;
	float						m_flLastDamageTime;
	float						m_flLastBeamTime;
};

#endif
