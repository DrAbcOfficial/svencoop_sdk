#ifndef DESERT_EAGLE_H
#define DESERT_EAGLE_H

#include "weapons.h"

#ifndef EAGLE_WEIGHT
#define EAGLE_WEIGHT 15
#define EAGLE_MAX_CLIP 7
#define EAGLE_DEFAULT_GIVE 7
#define EAGLE_AMMO1_DROP 6
#endif

#ifndef WEAPONCUSTOM_SND_DESERT_EAGLE_SIGHT
#define WEAPONCUSTOM_SND_DESERT_EAGLE_SIGHT 21
#define WEAPONCUSTOM_SND_DESERT_EAGLE_SIGHT2 22
#endif

#ifndef CLIENT_DLL
enum eagle_e
{
	EAGLE_IDLE1 = 0,
	EAGLE_IDLE2,
	EAGLE_IDLE3,
	EAGLE_IDLE4,
	EAGLE_IDLE5,
	EAGLE_SHOOT,
	EAGLE_SHOOT_EMPTY,
	EAGLE_RELOAD,
	EAGLE_RELOAD_NOSHOT,
	EAGLE_DRAW,
	EAGLE_HOLSTER
};

class CDESpot : public CBaseEntity
{
public:
	void Spawn( void );
	void Precache( void );
	int ObjectCaps( void ) { return FCAP_DONT_SAVE; }
	void Suspend( float delay );
	void EXPORT Revive( void );
	static CDESpot *CreateDESpot( void );
};
#endif

class CDesertEagle : public CBasePlayerWeapon
{
public:
#ifndef CLIENT_DLL
	CDesertEagle();
#endif
	void Spawn( void );
	void Precache( void );
	int iItemSlot( void ) { return 2; }
	bool GetItemInfo( ItemInfo *info );
	bool AddToPlayer( CBasePlayer *player );

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	bool Deploy( void );
	void Holster( int skiplocal = 0 );
	void Reload( void );
	void WeaponIdle( void );
	bool ShouldWeaponIdle( void ) { return true; }
	void UpdateSpot( void );

	void GetWeaponData( weapon_data_s *data );
	void SetWeaponData( weapon_data_s *data );
	bool UseDecrement( void ) { return true; }

	bool					m_fSpotActive;
#ifndef CLIENT_DLL
	CEntityHandle<CDESpot>	m_hSpot;
#endif
	int						m_fLaserOn;
	unsigned short			m_usFireEagle;
};

#endif
