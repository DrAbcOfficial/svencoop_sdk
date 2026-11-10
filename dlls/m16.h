#ifndef M16_H
#define M16_H

#include "weapons.h"

#ifndef M16_WEIGHT
#define M16_WEIGHT 15
#define M16_MAX_CLIP 30
#define M16_DEFAULT_GIVE 60
#define AMMO_556CLIP_GIVE 30
#define M16_AMMO1_DROP 30
#define M16_AMMO2_DROP 1
#endif

#ifndef WEAPONCUSTOM_SND_GLAUNCHER
#define WEAPONCUSTOM_SND_GLAUNCHER 2
#define WEAPONCUSTOM_SND_GLAUNCHER2 3
#endif

#ifndef M16_BURST_ROUNDS
#define M16_BURST_ROUNDS 3
#endif

#ifndef CLIENT_DLL
enum m16_e
{
	M16_DRAW = 0,
	M16_HOLSTER,
	M16_IDLE,
	M16_FIDGET,
	M16_SHOOT_1,
	M16_SHOOT_2,
	M16_RELOAD_M16,
	M16_LAUNCH,
	M16_RELOAD_M203
};
#endif

class CM16 : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int iItemSlot( void ) { return 3; }
	bool GetItemInfo( ItemInfo *info );
	bool AddToPlayer( CBasePlayer *player );

	void ItemPostFrame( void );
	void BurstSupplement( void );
	void PrimaryAttack( void );
	void SecondaryAttack( void );
	int SecondaryAmmoIndex( void );
	bool Deploy( void );
	void Holster( int skiplocal = 0 );
	void Reload( void );
	void WeaponIdle( void );
	void CustomDecrement( float decrement ) {}

	void GetWeaponData( weapon_data_s *data );
	void SetWeaponData( weapon_data_s *data );
	bool UseDecrement( void ) { return true; }

#ifdef CLIENT_DLL
	int m_iGrenadeReloadAnim;
#endif
	int m_iShell;
#ifdef CLIENT_DLL
	int m_iSecondaryAmmoCarried;
#endif
};

#endif
