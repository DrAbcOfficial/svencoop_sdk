#ifndef M249_H
#define M249_H

#include "weapons/weapons.h"

struct weapon_data_s;

enum m249_e
{
	M249_SLOWIDLE = 0,
	M249_IDLE2,
	M249_RELOAD_START,
	M249_RELOAD_END,
	M249_HOLSTER,
	M249_DRAW,
	M249_SHOOT1,
	M249_SHOOT2,
	M249_SHOOT3
};

#define M249_BELT_ROUNDS 15
#define M249_LEGACY_BELT_ROUNDS 7

class CM249 : public CBasePlayerWeapon
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
	bool SpecialReload( int iClipSize, int iAnim, float fDelay, int body );
	void WeaponIdle( void );

	int GetBodyFromClip( void );
	bool CheckForLegacyModel( void );

	void GetWeaponData( weapon_data_s *data );
	void SetWeaponData( weapon_data_s *data );

	bool UseDecrement( void ) { return true; }

	int				m_iReloadEndAnim; // second reload stage, played by WeaponIdle
	int				m_iShell;
	bool			m_fLegacyModel;
	unsigned short	m_usFireM249Legacy;
	unsigned short	m_usFireM249;
};

#endif
