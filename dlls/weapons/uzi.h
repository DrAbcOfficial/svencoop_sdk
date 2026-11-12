#ifndef UZI_H
#define UZI_H

// CUzi::Fire modes. The right uzi fires from m_iClip, the left one from m_iClip2
#define UZI_FIRE_NONE		0
#define UZI_FIRE_SINGLE		1
#define UZI_FIRE_RIGHT		2
#define UZI_FIRE_LEFT		3
#define UZI_FIRE_BOTH		4

// CanHaveXtra extras
#define XTRA_ELECTRIC_CROWBAR	1
#define XTRA_GOLDEN_UZI		2
#define XTRA_CHUMTOAD		3
#define XTRA_CHUMTOAD_HEALTH	4
#define XTRA_SUBMIT_ID		5

#define UZI_DAMAGE			5
#define UZI_GOLDEN_DAMAGE	8

// v_uzi.mdl skins
#define UZI_SKIN_DEFAULT	0
#define UZI_SKIN_GILDED		1

class CUzi : public CBasePlayerWeapon
{
public:
	void	Spawn( void );
	void	Precache( void );
	int		iItemSlot( void ) { return 2; }
	bool	GetItemInfo( ItemInfo *p );
	bool	AddToPlayer( CBasePlayer *pPlayer );
	bool	AddDuplicate( CBasePlayerItem *pOriginal );
	bool	CanHaveDuplicates( void ) { return !m_fAkimboAvailable; }
	CBaseEntity *Respawn( void );
	CBasePlayerItem *DropItem( void );

	void	PrimaryAttack( void );
	void	SecondaryAttack( void );
	void	Fire( int iMode, float &flNextAttack );
	bool	CanDeploy( void );
	bool	Deploy( void );
	bool	Deploy( int iAnim, float flDelay );
	void	AkimboDeploy( bool bAkimbo );
	void	Holster( int skiplocal = 0 );
	void	Reload( void );
	bool	ReloadAkimbo( int iClipSize, int iAnim, float fDelay, int body );
	void	FinishReload( void );
	bool	ShouldReload( void );
	void	WeaponIdle( void );
	void	InactiveItemPostFrame( void );

	void	CheckGilded( int fAllowGilded );
	const char	*GetHUDSpriteName( void );
	void	SetAkimbo( bool bAkimbo, bool bForce );
	const char	*GetPModelName( void );
	void	CheckAkimboMagazines( void );
	void	SetRightClip( int iClip );
	void	SetLeftClip( int iClip );

	void	GetWeaponData( weapon_data_s *data );
	void	SetWeaponData( weapon_data_s *data );


	bool	UseDecrement( void ) { return true; }

	int				m_iNextReloadAnim;		// side still to reload after a double reload, -1 if none
	int				m_iReloadSide;			// side being reloaded right now
	unsigned short	m_usUzi;
	unsigned short	m_usUziAkimbo;
	bool			m_fAkimboAvailable;		// the player carries a second uzi
	int				m_iShell;
	int				m_iAkimboReloadAnim;	// requested akimbo reload animation
	bool			m_fSwitchingAkimbo;
};

#endif
