#ifndef PIPEWRENCH_H
#define PIPEWRENCH_H

class CWrench : public CBasePlayerWeapon
{
public:
	CWrench();

	void OnCreate( void );
	void Spawn( void );
	void Precache( void );
	bool GetItemInfo( ItemInfo *info );
	bool AddToPlayer( CBasePlayer *player );
	int iItemSlot( void ) { return 1; }
	float GetRespawnTime( void ) { return 3.0f; }
	bool Deploy( void );
	bool CanHolster( void );
	void Holster( int skiplocal = 0 );
	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void WeaponIdle( void );
	void EXPORT SwingAgain( void );
	void EXPORT Smack( void );
	bool Swing( bool first );
	void BigSwing( void );

	int		m_iSwingMode;
	int		m_iSwing;
	float	m_flNextChargeTime;
	int		m_iChargeBonus;

	TraceResult m_trHit;
};

#endif // PIPEWRENCH_H
