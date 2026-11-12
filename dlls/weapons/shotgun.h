#ifndef SHOTGUN_H
#define SHOTGUN_H

#define SHOTGUN_AMMO1_DROP SHOTGUN_MAX_CLIP
#define SHOTGUN_SINGLE_PELLETS 8
#define SHOTGUN_DOUBLE_PELLETS 14
#define VECTOR_CONE_DM_SHOTGUN Vector( 0.08716, 0.04362, 0.00 )
#define VECTOR_CONE_DM_DOUBLESHOTGUN Vector( 0.17365, 0.04362, 0.00 )
#define WEAPONCUSTOM_SND_SCOCK1 19

enum shotgun_e
{
	SHOTGUN_IDLE = 0,
	SHOTGUN_FIRE,
	SHOTGUN_FIRE2,
	SHOTGUN_RELOAD,
	SHOTGUN_PUMP,
	SHOTGUN_START_RELOAD,
	SHOTGUN_DRAW,
	SHOTGUN_HOLSTER,
	SHOTGUN_IDLE4,
	SHOTGUN_IDLE_DEEP
};

class CShotgun : public CBasePlayerWeapon
{
public:
	int Save( CSave &save );
	int Restore( CRestore &restore );
	static TYPEDESCRIPTION m_SaveData[];

	void Spawn( void );
	void Precache( void );
	int iItemSlot( void ) { return 3; }
	bool GetItemInfo( ItemInfo *p );
	bool AddToPlayer( CBasePlayer *pPlayer );

	void ItemPostFrame( void );
	void PrimaryAttack( void );
	void SecondaryAttack( void );
	bool Deploy( void );
	void Holster( int skiplocal = 0 );
	void Reload( void );
	void WeaponIdle( void );
	void CustomDecrement( float flDecrement );

	void GetWeaponData( weapon_data_s *data );
	void SetWeaponData( weapon_data_s *data );

	bool UseDecrement( void ) { return true; }

	int            m_fInSpecialReload;
	float          m_flNextReload;
	int            m_iShell;
	float          m_flPumpTime;
	bool           m_fNeedPump;
	unsigned short m_usDoubleFire;
	unsigned short m_usSingleFire;
	bool           m_fHasDoubleAnim;
};

#endif
