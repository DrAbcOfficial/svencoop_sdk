#ifndef MEDKIT_H
#define MEDKIT_H

class CMedkit : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	bool GetItemInfo( ItemInfo *info );
	int iItemSlot( void ) { return 1; }
	bool AddToPlayer( CBasePlayer *player );
	bool Deploy( void );
	void Holster( int skiplocal = 0 );
	void WeaponIdle( void );
	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void ItemPostFrame( void );
	void InactiveItemPostFrame( void );

private:
	void UpdateAmmo( void );
	int UseAmmo( int amount );
	CBaseEntity *FindTargetToRevive( bool *clearAim );
	void ReviveEntity( void );

	float m_flRechargeProgress;
	int   m_iReviveState;
	float m_flReviveTime;
};

#endif
