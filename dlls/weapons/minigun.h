#ifndef MINIGUN_H
#define MINIGUN_H

#define MINIGUN_SPEED_NONE		0
#define MINIGUN_SPEED_FIRE		1
#define MINIGUN_SPEED_IDLE		2
#define MINIGUN_SPEED_SPINUP	3

#define MINIGUN_SPIN_LOOP		0
#define MINIGUN_SPIN_START		1
#define MINIGUN_SPIN_STOP		2

class CMinigun : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int iItemSlot( void ) { return 4; }
	bool GetItemInfo( ItemInfo *info );
	bool AddToPlayer( CBasePlayer *player );
	float GetRespawnTime( void ) { return 180; }
	CBasePlayerItem *DropItem( void );

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void TertiaryAttack( void );
	int SecondaryAmmoIndex( void );
	bool Deploy( void );
	void Holster( int skiplocal = 0 );
	void Reload( void );
	void WeaponIdle( void );
	void CustomDecrement( float decrement );

	int ObjectCaps( void );
	void OnCreate( void );
	void Materialize( void );
	void TouchedWorld( bool firstTouch );
	void Dropped( CBaseEntity *dropper );
	bool CanCollect( CBaseEntity *other, COLLECT_TYPE collectType );

	void SetPModel( bool spinning );
	void SetPlayerSpeed( int state );
	void GetWeaponData( weapon_data_s *data );
	void SetWeaponData( weapon_data_s *data );

	bool UseDecrement( void ) { return true; }

	int				m_iShell;
	bool			m_fSpinning;
	bool			m_fSpunUp;
	bool			m_fSpinModel;
	float			m_flDeployTime;
	int				m_iSpeedState;
	float			m_flSpeedReduction;
	unsigned short	m_usMinigunFire;
	unsigned short	m_usMinigunSpin;
};

#endif
