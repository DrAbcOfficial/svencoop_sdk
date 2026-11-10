#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "playerammo.h"

class CMinigunChainammo : public CPlayerAmmo
{
public:
	CMinigunChainammo() : CPlayerAmmo( 100, "556", 600 ) {}
	const char *GetDefaultModel( void );
};

class CSniperRifleAmmoClip : public CPlayerAmmo
{
public:
	CSniperRifleAmmoClip() : CPlayerAmmo( 5, "m40a1", 15 ) {}
	const char *GetDefaultModel( void );
};

LINK_ENTITY_TO_CLASS( ammo_556, CMinigunChainammo );
LINK_ENTITY_TO_CLASS( ammo_762, CSniperRifleAmmoClip );

void CPlayerAmmo::Spawn( void )
{
	Precache();

	if ( SetupModel() )
		UTIL_SetModel( this, STRING( pev->model ) );
	else
		UTIL_SetModel( this, GetDefaultModel() );

	CBasePlayerAmmo::Spawn();
}

void CPlayerAmmo::Precache( void )
{
	CBaseEntity::Precache();
	UTIL_PrecacheModel( this, pev->model ? STRING( pev->model ) : GetDefaultModel() );
	PrecacheSound_Intercept( this, GetPickupSound() );
}

BOOL CPlayerAmmo::AddAmmo( CBaseEntity *other )
{
	if ( other->GiveAmmo( m_iAmmoGive, m_pszAmmoName, m_iMaxCarry, false, NULL ) == -1 )
		return FALSE;

	EMIT_SOUND_DYN( edict(), CHAN_ITEM, GetPickupSound(), 1.0, ATTN_NORM, 0, PITCH_NORM,
		0, false, g_vecZero );
	return TRUE;
}

const char *CPlayerAmmo::GetPickupSound( void )
{
	return "items/9mmclip1.wav";
}

const char *CMinigunChainammo::GetDefaultModel( void )
{
	return "models/w_saw_clip.mdl";
}

const char *CSniperRifleAmmoClip::GetDefaultModel( void )
{
	return "models/w_m40a1clip.mdl";
}
