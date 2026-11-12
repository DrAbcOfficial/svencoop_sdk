#ifndef SPORELAUNCHER_H
#define SPORELAUNCHER_H

#include "ai/actanimating.h"

enum sporelauncher_e
{
	SPLAUNCHER_IDLE = 0,
	SPLAUNCHER_FIDGET,
	SPLAUNCHER_RELOAD_REACH,
	SPLAUNCHER_RELOAD_LOAD,
	SPLAUNCHER_RELOAD_AIM,
	SPLAUNCHER_FIRE,
	SPLAUNCHER_HOLSTER1,
	SPLAUNCHER_DRAW1,
	SPLAUNCHER_IDLE2,
	SPLAUNCHER_RELOAD_AIMFAST
};

#define SPORE_RELOAD_NONE    0
#define SPORE_RELOAD_REACHED 1
#define SPORE_RELOAD_LOADED  2

class CSporeLauncher : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int iItemSlot( void ) { return 3; }
	bool GetItemInfo( ItemInfo *info );
	bool AddToPlayer( CBasePlayer *player );
	bool Deploy( void );
	bool UseDecrement( void ) { return true; }
	void OnCreate( void );
	void ItemPostFrame( void );
	void PrimaryAttack( void );
	void SecondaryAttack( void );
	bool CanFire( void );
	void Holster( int skiplocal = 0 );
	void Reload( void );
	void WeaponIdle( void );
	void GetWeaponData( weapon_data_s *data );
	void SetWeaponData( weapon_data_s *data );

	int            m_ReloadState;
	float          m_flNextReload;
	unsigned short m_usFireSpore;
};

class CSporeLauncherAmmo : public CActAnimating
{
public:
	void Spawn( void );
	void Precache( void );
	int Classify( void ) { return CLASS_NONE; }
	int ObjectCaps( void );
	int Save( CSave &save );
	int Restore( CRestore &restore );
	int TakeDamage( entvars_t *inflictor, entvars_t *attacker, float damage, int damageType );
	void EXPORT Think( void );
	void EXPORT Touch( CBaseEntity *other );
	void EXPORT Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );
	void TakeSpore( CBaseEntity *other );

	static TYPEDESCRIPTION m_SaveData[];

private:
	float m_flTimeRegeneration;
};

#endif
