#ifndef GAUSS_H
#define GAUSS_H

#ifndef CLIENT_DLL
#define GAUSS_PRIMARY_AMMO 1
#define GAUSS_DAMAGE 20
#define GAUSS_FULL_CHARGE_TIME 3
#define GAUSS_SPIN_START_PITCH 110
#define GAUSS_MAX_PITCH_RISE 150
#define GAUSS_AMMO1_DROP AMMO_URANIUMBOX_GIVE

#define GAUSS_PRIMARY_CHARGE_VOLUME 256
#define GAUSS_PRIMARY_FIRE_VOLUME 450

#define WEAPONCUSTOM_SND_NULL 7
#define WEAPONCUSTOM_SND_ELECTRO4 8
#define WEAPONCUSTOM_SND_ELECTRO5 9
#define WEAPONCUSTOM_SND_ELECTRO6 10

enum gauss_e
{
	GAUSS_IDLE = 0,
	GAUSS_IDLE2,
	GAUSS_FIDGET,
	GAUSS_SPINUP,
	GAUSS_SPIN,
	GAUSS_FIRE,
	GAUSS_FIRE2,
	GAUSS_HOLSTER,
	GAUSS_DRAW
};
#endif

class CGauss : public CBasePlayerWeapon
{
public:
#ifndef CLIENT_DLL
	int Save( CSave &save );
	int Restore( CRestore &restore );
	static TYPEDESCRIPTION m_SaveData[];
#endif

	void Spawn( void );
	void Precache( void );
	int iItemSlot( void ) { return 4; }
	bool GetItemInfo( ItemInfo *p );
	bool AddToPlayer( CBasePlayer *pPlayer );

	bool Deploy( void );
	void Holster( int skiplocal = 0 );

	void ItemPostFrame( void );
	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void TertiaryAttack( void );
	void WeaponIdle( void );
	void StartFire( void );
	void Fire( Vector vecOrigSrc, Vector vecDir, float flDamage );
	float GetFullChargeTime( void );
	void CustomDecrement( float flDecrement );

	void GetWeaponData( weapon_data_s *data );
	void SetWeaponData( weapon_data_s *data );

	bool UseDecrement( void ) { return true; }

	int   m_fInAttack;
	float m_flStartCharge;
	bool  m_fStartChargeActive;
	float m_flPlayAftershock;
	bool  m_fAftershockActive;
	int   m_iBalls;
	int   m_iGlow;
	int   m_iBeam;
	int   m_iSoundState;
	float m_flNextAmmoBurn;
	bool  m_fAmmoBurnActive;
	bool  m_fPrimaryFire;

	unsigned short m_usGaussFire;
	unsigned short m_usGaussSpin;
	bool           m_fOvercharged;
};

#endif
