#ifndef EGON_H
#define EGON_H

#include <vector>

struct PortalTraceResult;

class CEgon : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int iItemSlot( void ) { return 4; }
	bool GetItemInfo( ItemInfo *p );
	bool AddToPlayer( CBasePlayer *player );
	bool Deploy( void );
	void Holster( int skiplocal = 0 );
	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void TertiaryAttack( void );
	void WeaponIdle( void );

	void Attack( void );
	void Fire( const Vector &source, const Vector &direction );
	void UpdateEffect( PortalTraceResult &trace );
	void CreateBeams( unsigned int count );
	void DestroyEffect( unsigned int count = 0 );
	void EndAttack( void );
	float GetTimeDist( void );
	float GetPulseInterval( void );
	float GetDischargeInterval( void );
	BOOL HasAmmo( void );
	void UseAmmo( int count );

private:
	float m_flAmmoUseTime;
	float m_shootTime;

	std::vector<CEntityHandle<CBeam>> m_Beams;
	std::vector<CEntityHandle<CBeam>> m_Noise;
	CEntityHandle<CSprite>           m_Sprite;

	int   m_fireState;
	float m_shakeTime;
	bool  m_deployed;
};

#endif
