#ifndef SHOCKRIFLE_H
#define SHOCKRIFLE_H

class CShockRifle : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int iItemSlot( void ) { return 4; }
	bool GetItemInfo( ItemInfo *info );
	bool AddToPlayer( CBasePlayer *player );
	const char *GetPickupSound( void );
	CBasePlayerItem *DropItem( void );
	bool IsUseable( void ) { return true; }
	int ObjectCaps( void ) { return FCAP_IMPULSE_USE; }
	void Materialize( void );
	void ItemPostFrame( void );
	void InactiveItemPostFrame( void );
	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void TertiaryAttack( void );
	bool Deploy( void );
	void Holster( int skiplocal = 0 );
	void WeaponIdle( void );
	void CustomDecrement( float decrement );
	void GetWeaponData( weapon_data_s *data );
	void SetWeaponData( weapon_data_s *data );
	bool UseDecrement( void ) { return TRUE; }

private:
	void BeamFire( const Vector &source, const Vector &angles );
	void BeamSplash( const Vector &source, edict_t *ignored, float damage );
	void CreateBeamEffect( void );
	void EndBeamAttack( void );
	void AmmoDecay( void );
	void UseAmmo( int count );
	void Prime( void );
	void SparkEffect( void );
	void DetonateInHand( void );

	float          m_flRechargeTime;
	float          m_flAmmoDecayTime;
	bool           m_fPrimed;
	unsigned short m_usShockRifle;
	unsigned short m_usShockRifleBeam;
	void DestroyBeamEffect( void );

	EHANDLE        m_hBeamSprites[4];
	int            m_iBeam;
	int            m_fInAttack;
	float          m_flNextBeamAmmoTime;
	float          m_flArmorRecharge;
};

#endif
