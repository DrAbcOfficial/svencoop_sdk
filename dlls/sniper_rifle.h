#ifndef SNIPER_RIFLE_H
#define SNIPER_RIFLE_H

#include "weapons.h"

struct weapon_data_s;

enum sniperrifle_e
{
	SNIPERRIFLE_DRAW = 0,
	SNIPERRIFLE_SLOWIDLE,
	SNIPERRIFLE_FIRE,
	SNIPERRIFLE_FIRELASTROUND,
	SNIPERRIFLE_RELOAD1,
	SNIPERRIFLE_RELOAD2,
	SNIPERRIFLE_RELOAD3,
	SNIPERRIFLE_SLOWIDLE2,
	SNIPERRIFLE_HOLSTER
};

#define SNIPERRIFLE_ZOOM_FOV 20

class CSniperRifle : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int iItemSlot( void ) { return 3; }
	bool GetItemInfo( ItemInfo *p );
	bool AddToPlayer( CBasePlayer *pPlayer );

	void ItemPostFrame( void );
	void PrimaryAttack( void );
	void SecondaryAttack( void );
	int SecondaryAmmoIndex( void );
	bool Deploy( void );
	void Holster( int skiplocal = 0 );
	void Reload( void );
	bool SpecialReload( int iClipSize, int iAnim, float fDelay, int fTwoStage );
	void WeaponIdle( void );

	void GetWeaponData( weapon_data_s *data );
	void SetWeaponData( weapon_data_s *data );

	bool UseDecrement( void ) { return true; }

	int				m_iReloadEndAnim; // second reload stage, played by WeaponIdle
	int				m_iShell;
	unsigned short	m_usSniper;
};

#endif
