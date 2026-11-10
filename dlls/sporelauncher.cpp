/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "soundent.h"
#include "weaponinfo.h"
#include "event_flags.h"
#include "sporelauncher.h"

#ifndef CLIENT_DLL
#include "playerammo.h"
#include "sporegrenade.h"

#define SF_ITEM_TOUCH_ONLY		128
#define SF_ITEM_USE_WITHOUT_LOS	512

#undef PRECACHE_MODEL
#define PRECACHE_MODEL( path ) UTIL_PrecacheModel( this, path )
#undef PRECACHE_SOUND
#define PRECACHE_SOUND( path ) PrecacheSound_Intercept( this, path )
#endif

LINK_ENTITY_TO_CLASS( weapon_sporelauncher, CSporeLauncher );

void CSporeLauncher::Spawn( void )
{
	Precache();
	SET_MODEL( ENT( pev ), GetW_Model( "models/w_spore_launcher.mdl" ) );
	pev->classname = MAKE_STRING( "weapon_sporelauncher" );

	m_iDefaultAmmo = SPORELAUNCHER_DEFAULT_GIVE;
	m_iId = WEAPON_SPORELAUNCHER;
	m_iSpecialState = 0;
	m_flCustomDmg = pev->dmg;

#ifdef CLIENT_DLL
	FallInit();// get ready to fall down.
#else
	CBasePhysicsObject::Spawn();
#endif
}

void CSporeLauncher::Precache( void )
{
	UTIL_PrecacheOther( "sporegrenade" );

	PrecacheCustomModels();

	PRECACHE_MODEL( "models/v_spore_launcher.mdl" );
	PRECACHE_MODEL( "models/w_spore_launcher.mdl" );
	PRECACHE_MODEL( "models/p_spore_launcher.mdl" );
	PRECACHE_MODEL( "models/spore.mdl" );
	PRECACHE_MODEL( "models/spore_ammo.mdl" );

	PRECACHE_SOUND( "weapons/splauncher_altfire.wav" );
	PRECACHE_SOUND( "weapons/splauncher_bounce.wav" );
	PRECACHE_SOUND( "weapons/splauncher_fire.wav" );
	PRECACHE_SOUND( "weapons/splauncher_impact.wav" );
	PRECACHE_SOUND( "weapons/splauncher_pet.wav" );
	PRECACHE_SOUND( "weapons/splauncher_reload.wav" );

	m_usFireSpore = PRECACHE_EVENT( 1, "events/sporelauncher.sc" );
}

bool CSporeLauncher::GetItemInfo( ItemInfo *p )
{
	p->pszName = STRING( pev->classname );
	p->pszAmmo1 = "sporeclip";
	p->iMaxAmmo1 = SPORE_MAX_CARRY;
	p->pszAmmo1Entity = "ammo_sporeclip";
	p->iAmmo1Drop = SPORELAUNCHER_AMMO1_DROP;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo2Entity = NULL;
	p->iAmmo2Drop = AMMO_DROP_NONE;
	p->iMaxClip = SPORELAUNCHER_MAX_CLIP;
	p->iSlot = 5;
	p->iPosition = 2;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_SPORELAUNCHER;
	p->iWeight = SPORELAUNCHER_WEIGHT;

	return true;
}

bool CSporeLauncher::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		AddToPlayerMessage();
		return true;
	}

	return false;
}

void CSporeLauncher::OnCreate( void )
{
	CBasePlayerWeapon::OnCreate();

	m_iBounceMaterial = BOUNCE_MATERIAL_FLESH;
}

void CSporeLauncher::ItemPostFrame( void )
{
	CBasePlayerWeapon::ItemPostFrame();
}

void CSporeLauncher::Holster( int skiplocal /* = 0 */ )
{
	m_ReloadState = SPORE_RELOAD_NONE;

	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	m_hPlayer->pev->viewmodel = 0;
}

void CSporeLauncher::PrimaryAttack( void )
{
	if ( m_iClip <= 0 )
	{
		Reload();
		return;
	}

	if ( !CanFire() )
		return;

	m_hPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_hPlayer->m_iWeaponFlash = DIM_GUN_FLASH;
	m_hPlayer->m_iExtraSoundTypes = bits_SOUND_DANGER;

	m_iClip--;

	// player "shoot" animation
	m_hPlayer->SetAnimation( PLAYER_ATTACK1 );

	SendWeaponAnim( SPLAUNCHER_FIRE );

	PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), g_evWeaponCustom, 0.0, (float *)&g_vecZero, (float *)&g_vecZero,
		WEAPONCUSTOM_SND_SPLAUNCHER_FIRE, 1.0, WEAPONCUSTOM_SOUND, CHAN_WEAPON, 0, 0 );

#ifndef CLIENT_DLL
	UTIL_MakeVectors( m_hPlayer->pev->v_angle + m_hPlayer->pev->punchangle );

	Vector source = m_hPlayer->pev->origin;

	if ( !( m_hPlayer->pev->button & IN_DUCK ) )
		source = source + m_hPlayer->pev->view_ofs * 0.5;

	source = source + gpGlobals->v_right * 8 + gpGlobals->v_up * 4;
	CSporeGrenade::ShootLob( m_hPlayer->pev, source, gpGlobals->v_forward * 700, false, m_flCustomDmg );
#endif

	if ( !m_iClip && m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] <= 0 )
	{
		// HEV suit - indicate out of ammo condition
		m_hPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );
	}

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.6;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.6;
	m_ReloadState = SPORE_RELOAD_NONE;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.0;
}

void CSporeLauncher::SecondaryAttack( void )
{
	if ( m_iClip <= 0 )
	{
		Reload();
		return;
	}

	if ( !CanFire() )
		return;

	m_hPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_hPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;
	m_hPlayer->m_iExtraSoundTypes = bits_SOUND_DANGER;

	m_iClip--;

	// player "shoot" animation
	m_hPlayer->SetAnimation( PLAYER_ATTACK1 );

	SendWeaponAnim( SPLAUNCHER_FIRE );

	PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), g_evWeaponCustom, 0.0, (float *)&g_vecZero, (float *)&g_vecZero,
		WEAPONCUSTOM_SND_SPLAUNCHER_ALTFIRE, 1.0, WEAPONCUSTOM_SOUND, CHAN_WEAPON, 0, 0 );

#ifndef CLIENT_DLL
	UTIL_MakeVectors( m_hPlayer->pev->v_angle + m_hPlayer->pev->punchangle );

	Vector source = m_hPlayer->pev->origin;

	if ( !( m_hPlayer->pev->button & IN_DUCK ) )
		source = source + m_hPlayer->pev->view_ofs * 0.5;

	source = source + gpGlobals->v_right * 8 + gpGlobals->v_up * 4;
	CSporeGrenade::ShootLob( m_hPlayer->pev, source, gpGlobals->v_forward * 1200, false, m_flCustomDmg );
#endif

	if ( !m_iClip && m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] <= 0 )
	{
		// HEV suit - indicate out of ammo condition
		m_hPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );
	}

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.6;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.6;
	m_ReloadState = SPORE_RELOAD_NONE;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.0;
}

// Aborts a reload in progress before firing
bool CSporeLauncher::CanFire( void )
{
	if ( m_ReloadState > SPORE_RELOAD_NONE )
	{
		SendWeaponAnim( SPLAUNCHER_RELOAD_AIMFAST );
		m_ReloadState = SPORE_RELOAD_NONE;

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.3;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 4.0;
		return false;
	}

	if ( m_iClip > 0 )
		return true;

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.15;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 4.0;

	Reload();
	return false;
}

bool CSporeLauncher::Deploy( void )
{
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5;

	return DefaultDeploy( GetV_Model( "models/v_spore_launcher.mdl" ), GetP_Model( "models/p_spore_launcher.mdl" ), SPLAUNCHER_DRAW1, "rpg" );
}

// Loads one spore at a time, like the shotgun
void CSporeLauncher::Reload( void )
{
	if ( m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] <= 0 || m_iClip == SPORELAUNCHER_MAX_CLIP )
		return;

	// don't reload until recoil is done
	if ( m_flNextReload > UTIL_WeaponTimeBase() )
		return;

	if ( m_ReloadState == SPORE_RELOAD_NONE )
	{
		SendWeaponAnim( SPLAUNCHER_RELOAD_REACH );

		m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase();
		m_ReloadState = SPORE_RELOAD_REACHED;
		m_flNextReload = UTIL_WeaponTimeBase();

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.7;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.7;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.7;
		return;
	}

	if ( m_ReloadState == SPORE_RELOAD_REACHED )
	{
		if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
			return;

		if ( m_iClip >= SPORELAUNCHER_MAX_CLIP )
		{
			m_iClip = SPORELAUNCHER_MAX_CLIP;

			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.7;
			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.7;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.7;

			SendWeaponAnim( SPLAUNCHER_RELOAD_AIM );
			m_ReloadState = SPORE_RELOAD_NONE;
			return;
		}

		SendWeaponAnim( SPLAUNCHER_RELOAD_LOAD );

		EMIT_SOUND_DYN( ENT( m_hPlayer->pev ), CHAN_ITEM, "weapons/splauncher_reload.wav", 1.0, ATTN_NORM, 0, 85 + RANDOM_LONG( 0, 31 ) );

		m_flNextReload = UTIL_WeaponTimeBase();

		float flNextAttack = UTIL_WeaponTimeBase() + ( m_iClip < 1 ? 0.7 : 0.1 );

		m_flNextSecondaryAttack = flNextAttack;
		m_flNextPrimaryAttack = flNextAttack;
		m_ReloadState = SPORE_RELOAD_LOADED;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.8;

#ifndef CLIENT_DLL
		CBasePlayerWeapon::Reload();
#endif

		return;
	}

	if ( m_ReloadState == SPORE_RELOAD_LOADED )
	{
		// Add them to the clip
		m_iClip++;
		m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ]--;
		m_ReloadState = SPORE_RELOAD_REACHED;
		return;
	}

#ifndef CLIENT_DLL
	CBasePlayerWeapon::Reload();
#endif
}

void CSporeLauncher::WeaponIdle( void )
{
	ResetEmptySound();

	m_hPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	if ( m_iClip == 0 && m_ReloadState == SPORE_RELOAD_NONE && m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] )
	{
		Reload();
		return;
	}

	if ( m_ReloadState != SPORE_RELOAD_NONE )
	{
		if ( m_iClip < SPORELAUNCHER_MAX_CLIP && m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] )
		{
			Reload();
			return;
		}

		// reload debounce has timed out
		SendWeaponAnim( SPLAUNCHER_RELOAD_AIM );
		m_ReloadState = SPORE_RELOAD_NONE;

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0;
		return;
	}

	int iAnim = SPLAUNCHER_IDLE;
	float flRand = RANDOM_FLOAT( 0, 10 );

	if ( flRand > 6 )
	{
		if ( flRand > 8 )
		{
			iAnim = SPLAUNCHER_FIDGET;
			EMIT_SOUND_DYN( ENT( pev ), CHAN_ITEM, "weapons/splauncher_pet.wav", 1.0, ATTN_NORM, 0, PITCH_NORM );
		}
		else
		{
			iAnim = SPLAUNCHER_IDLE2;
		}

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 4.0;
	}
	else
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0;
	}

	SendWeaponAnim( iAnim, 0 );
}

void CSporeLauncher::GetWeaponData( weapon_data_t *data )
{
	data->iuser1 = m_iPrimaryAmmoType;
	data->iuser2 = m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ];
	data->iuser3 = m_ReloadState;
}

void CSporeLauncher::SetWeaponData( weapon_data_t *data )
{
	m_iPrimaryAmmoType = data->iuser1;
	m_hPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] = data->iuser2;
	m_iSpecialState = (int)data->fuser1;
	m_ReloadState = data->iuser3;
}

class CSporeLauncherAmmoClip : public CPlayerAmmo
{
public:
	CSporeLauncherAmmoClip() : CPlayerAmmo( AMMO_SPORE_GIVE, "sporeclip", SPORE_MAX_CARRY ) { }

	void OnCreate( void )
	{
		CPlayerAmmo::OnCreate();

		m_iBounceMaterial = BOUNCE_MATERIAL_FLESH;
	}

	const char *GetDefaultModel( void ) { return "models/spore.mdl"; }
	const char *GetPickupSound( void ) { return "weapons/spore_ammo.wav"; }
};

LINK_ENTITY_TO_CLASS( ammo_sporeclip, CSporeLauncherAmmoClip );

#ifndef CLIENT_DLL
LINK_ENTITY_TO_CLASS( ammo_spore, CSporeLauncherAmmo );

TYPEDESCRIPTION CSporeLauncherAmmo::m_SaveData[] =
{
	DEFINE_FIELD( CSporeLauncherAmmo, m_flTimeRegeneration, FIELD_FLOAT ),
};

IMPLEMENT_SAVERESTORE( CSporeLauncherAmmo, CActAnimating );

void CSporeLauncherAmmo::Spawn( void )
{
	Precache();
	SET_MODEL( ENT( pev ), "models/spore_ammo.mdl" );
	pev->angles.x -= 90;
	UTIL_SetSize( pev, Vector( -16, -16, -16 ), Vector( 16, 16, 16 ) );
	UTIL_SetOrigin( pev, pev->origin );

	if ( pev->movetype != MOVETYPE_FLY && pev->movetype != MOVETYPE_NOCLIP )
	{
		UTIL_MakeVectors( pev->angles );

		if ( pev->angles.x == 0 || pev->angles.x == 360 )
			UTIL_SetOrigin( pev, pev->origin - Vector( 0, 0, 15.5 ) );
		else if ( pev->angles.x == -180 || pev->angles.x == 180 )
			UTIL_SetOrigin( pev, pev->origin + Vector( 0, 0, 15.5 ) );
		else
			UTIL_SetOrigin( pev, pev->origin - gpGlobals->v_up * 0.5 );

		pev->movetype = MOVETYPE_FLY;
	}

	pev->solid = SOLID_SLIDEBOX;
	pev->takedamage = DAMAGE_YES;
	pev->effects = EF_INVLIGHT;
	pev->frame = 0;
	pev->body = 0;
	pev->sequence = 0;
	pev->framerate = 1;
	SetThink( NULL );
	SetTouch( NULL );
	m_flTimeRegeneration = gpGlobals->time + 0.1;
	pev->nextthink = gpGlobals->time + 0.1;
	StudioFrameAdvance( 0 );
}

void CSporeLauncherAmmo::Precache( void )
{
	CActAnimating::Precache();

	if ( !pev->model )
		UTIL_PrecacheModel( this, "models/spore_ammo.mdl" );

	PrecacheSound_Intercept( this, "weapons/spore_ammo.wav" );
}

int CSporeLauncherAmmo::ObjectCaps( void )
{
	if ( pev->effects & EF_NODRAW )
		return 0;

	return ( pev->spawnflags & SF_ITEM_TOUCH_ONLY ) ? 0 : FCAP_IMPULSE_USE;
}

void CSporeLauncherAmmo::Think( void )
{
	float interval = StudioFrameAdvance( 0 );

	DispatchAnimEvents( interval );

	switch ( pev->sequence )
	{
	case 0:
		if ( pev->body == 1 )
			return;

		if ( m_flTimeRegeneration <= gpGlobals->time )
		{
			pev->sequence = RANDOM_LONG( 0, 1 ) ? 3 : 1;
			pev->nextthink = gpGlobals->time + 4.0;
			pev->body = 1;
			break;
		}

		pev->nextthink = m_flTimeRegeneration;
		return;

	case 1:
	case 3:
		pev->sequence = pev->sequence == 1 ? 6 : 5;
		pev->nextthink = gpGlobals->time + 2.0;
		break;

	case 2:
	case 4:
		m_flTimeRegeneration = gpGlobals->time + 10.0;
		pev->nextthink = gpGlobals->time + ( pev->sequence == 2 ? 11.33 : 10.67 );
		pev->sequence = 0;
		break;

	case 5:
	case 6:
		pev->nextthink = gpGlobals->time + 2.0;
		break;

	default:
		pev->nextthink = gpGlobals->time + 0.1;
		break;
	}

	pev->rendermode = m_iOriginalRenderMode;
	pev->renderfx = m_iOriginalRenderFX;
	pev->renderamt = m_flOriginalRenderAmount;
	pev->rendercolor = m_vecOriginalRenderColor;
}

int CSporeLauncherAmmo::TakeDamage( entvars_t *inflictor, entvars_t *attacker, float damage, int damageType )
{
	if ( pev->body == 0 )
		return 0;

	pev->body = 0;
	EMIT_SOUND_DYN( ENT( pev ), CHAN_ITEM, "weapons/splauncher_altfire.wav", 1.0, ATTN_NORM, 0, 85 + RANDOM_LONG( 0, 31 ) );
	pev->sequence = pev->sequence == 5 ? 2 : 4;
	StudioFrameAdvance( 0 );
	pev->nextthink = gpGlobals->time + 2.0;
	UTIL_MakeVectors( pev->angles );
	CSporeGrenade::ShootLob( pev, pev->origin + gpGlobals->v_up * -8, gpGlobals->v_up * 1200, true, 0 );
	return 1;
}

void CSporeLauncherAmmo::TakeSpore( CBaseEntity *other )
{
	if ( !other || !other->IsPlayer() )
		return;

	CBasePlayer *player = dynamic_cast<CBasePlayer *>( other );

	if ( !player || pev->body == 0 )
		return;

	int ammoIndex = CBasePlayer::GetAmmoIndex( "sporeclip" );

	if ( player->AmmoInventory( ammoIndex ) >= SPORE_MAX_CARRY )
		return;

	if ( player->GiveAmmo( 1, "sporeclip", SPORE_MAX_CARRY, true, NULL ) == -1 )
		return;

	pev->body = 0;
	EMIT_SOUND( ENT( pev ), CHAN_ITEM, "weapons/spore_ammo.wav", 1.0, ATTN_NORM );
	pev->sequence = pev->sequence == 5 ? 2 : 4;
	StudioFrameAdvance( 0 );
	pev->nextthink = gpGlobals->time + 2.0;
}

void CSporeLauncherAmmo::Touch( CBaseEntity *other )
{
	if ( !( pev->spawnflags & SF_ITEM_USE_ONLY ) )
		TakeSpore( other );
}

void CSporeLauncherAmmo::Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value )
{
	if ( !activator )
		return;

	if ( useType == USE_SET && ( pev->spawnflags & SF_ITEM_TOUCH_ONLY ) )
		return;

	if ( !( pev->spawnflags & SF_ITEM_USE_WITHOUT_LOS ) && activator == caller && !UTIL_HasLineOfSight( activator, this, true, false ) )
		return;

	switch ( useType )
	{
	case USE_ON:
	case USE_TOGGLE:
	case USE_SET:
		TakeSpore( activator );
		break;

	case USE_KILL:
		UTIL_Remove( this );
		break;

	default:
		break;
	}
}
#endif
