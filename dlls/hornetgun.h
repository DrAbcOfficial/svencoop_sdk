#ifndef HORNETGUN_H
#define HORNETGUN_H

#ifndef CLIENT_DLL
#define HORNETGUN_MAX_CLIP WEAPON_NOCLIP
#define WEAPONCUSTOM_SND_AG_FIRE1 11

enum hgun_e
{
	HGUN_IDLE1 = 0,
	HGUN_FIDGETSWAY,
	HGUN_FIDGETSHAKE,
	HGUN_DOWN,
	HGUN_UP,
	HGUN_SHOOT
};
#endif

class CHgun : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int iItemSlot( void ) { return 4; }
	bool GetItemInfo( ItemInfo *p );
	bool AddToPlayer( CBasePlayer *pPlayer );
	void OnCreate( void );
	bool IsUseable( void ) { return true; }

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	bool Deploy( void );
	void Holster( int skiplocal = 0 );
	void Reload( void );
	void WeaponIdle( void );
	void InactiveItemPostFrame( void );
	void CustomDecrement( float flDecrement );

	void GetWeaponData( weapon_data_s *data );
	void SetWeaponData( weapon_data_s *data );

	bool UseDecrement( void ) { return true; }

	float m_flNextAnimTime;
	float m_flRechargeTime;
	int   m_iFirePhase;
};

#endif
