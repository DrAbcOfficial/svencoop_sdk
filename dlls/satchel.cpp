/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
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
#if !defined( OEM_BUILD ) && !defined( HLDEMO_BUILD )

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "weaponinfo.h"
#include "CClassicMode.h"
#include "game.h"
#include "soundent.h"
#include "server_sound.h"
#include "event_flags.h"

#undef SET_MODEL
#define SET_MODEL( entity, model ) SetModel_Intercept( entity, model )
#undef PRECACHE_MODEL
#define PRECACHE_MODEL( path ) UTIL_PrecacheModel( this, path )
#undef PRECACHE_SOUND
#define PRECACHE_SOUND( path ) PrecacheSound_Intercept( this, path )

bool SatchelClassicMode( void )
{
	return CClassicMode::GetInstance()->IsEnabled();
}

enum satchel_e {
	SATCHEL_IDLE1 = 0,
	SATCHEL_FIDGET1,
	SATCHEL_DRAW,
	SATCHEL_DROP
};

enum satchel_radio_e {
	SATCHEL_RADIO_IDLE1 = 0,
	SATCHEL_RADIO_FIDGET1,
	SATCHEL_RADIO_DRAW,
	SATCHEL_RADIO_FIRE,
	SATCHEL_RADIO_HOLSTER
};



class CSatchelCharge : public CGrenade
{
	void Spawn( void );
	void Precache( void );
	void BounceSound( void );

	void EXPORT SatchelSlide( CBaseEntity *pOther );
	void EXPORT SatchelThink( void );
	void EXPORT PreDetonateUse( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );
	bool PlayerUse( CBaseEntity *other );

	float m_flSlideTime;

public:
	int ObjectCaps( void ) { return FCAP_IMPULSE_USE; }
	bool CriticalRemove( void );
	void Deactivate( void );
};
LINK_ENTITY_TO_CLASS( monster_satchel, CSatchelCharge );

bool CSatchelCharge::CriticalRemove( void )
{
	SetThink( &CSatchelCharge::CallDetonate );
	pev->nextthink = gpGlobals->time;

	return true;
}

//=========================================================
// Deactivate - do whatever it is we do to an orphaned 
// satchel when we don't want it in the world anymore.
//=========================================================
void CSatchelCharge::Deactivate( void )
{
	pev->solid = SOLID_NOT;
	UTIL_Remove( this );
}

void CSatchelCharge :: Spawn( void )
{
	Precache( );
	// motor
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;

	if ( pev->iuser2 )
		SET_MODEL( edict(), STRING( pev->iuser2 ) );
	else
		SET_MODEL( edict(), SatchelClassicMode() ? "models/hlclassic/w_satchel.mdl" : "models/w_satchel.mdl");
	//UTIL_SetSize(pev, Vector( -16, -16, -4), Vector(16, 16, 32));	// Old box -- size of headcrab monsters/players get blocked by this
	UTIL_SetSize(pev, Vector( -4, -4, -4), Vector(4, 4, 4));	// Uses point-sized, and can be stepped over
	UTIL_SetOrigin( pev, pev->origin );

	SetTouch( &CSatchelCharge::SatchelSlide );
	SetUse( &CSatchelCharge::PreDetonateUse );
	SetThink( &CSatchelCharge::SatchelThink );
	pev->nextthink = gpGlobals->time + 0.1;
	m_flSlideTime = gpGlobals->time;
	pev->gravity = 0.5;
	pev->friction = 0.8;

	if ( pev->dmg <= 0.0f )
		pev->dmg = sk_plr_satchel.value;
	// ResetSequenceInfo( );
	pev->sequence = 1;
}

void CSatchelCharge::SatchelSlide( CBaseEntity *pOther )
{
	if ( pOther->IsPlayer() && pOther->IsAlive() )
	{
		if ( m_flSlideTime <= gpGlobals->time )
		{
			pev->velocity = pev->velocity + pOther->pev->velocity * 0.7;

			if ( pev->velocity.Length() > 500.0f )
				pev->velocity = pev->velocity.Normalize() * 500.0f;

			pev->avelocity = pev->avelocity * RANDOM_FLOAT( 1.1, 2.0 );
			BounceSound();
			m_flSlideTime = gpGlobals->time + 0.005;
		}

		return;
	}

	pev->gravity = 1.0;

	// don't hit the guy that launched this grenade
	if ( pOther->IsBSPModel() )
	{
		if ( gpGlobals->time >= m_flSlideTime )
		{
			AlignToGroundStatic( this, 60.0f );

			if ( pev->velocity.Length() < 5.0f && ( pev->flags & FL_ONGROUND ) )
				pev->origin.z += 1.0;

	// pev->avelocity = Vector (300, 300, 300);
	pev->solid = SOLID_TRIGGER;
			UTIL_SetOrigin( pev, pev->origin );
			CSoundEnt::InsertSound( bits_SOUND_DANGER, pev->origin, 512, 1.0, this );
		}

		if ( pev->velocity.Length2D() <= 2.0f )
		{
			pev->velocity.x = 0;
			pev->velocity.y = 0;
		}
		else
		{
		// add a bit of static friction
		pev->velocity = pev->velocity * 0.95;
		}

		pev->avelocity = pev->avelocity * 0.9;
		m_flSlideTime = gpGlobals->time + 0.005;
		// play sliding sound, volume based on velocity
	}
	if ( !(pev->flags & FL_ONGROUND) && pev->velocity.Length2D() > 10.0f )
		BounceSound();

	StudioFrameAdvance( );
}

void CSatchelCharge :: SatchelThink( void )
{
	StudioFrameAdvance( );
	pev->nextthink = gpGlobals->time + 0.1;

	if (!IsInWorld())
	{
		UTIL_Remove( this );
		return;
	}

	if (pev->waterlevel == 3)
	{
		pev->flags &= ~FL_ONGROUND;
		pev->movetype = MOVETYPE_FLY;
		pev->velocity = pev->velocity * 0.8;
		pev->avelocity = pev->avelocity * 0.9;
		pev->velocity.z += 8;
	}
	else if (pev->waterlevel == 0)
	{
		pev->movetype = MOVETYPE_BOUNCE;
	}
}

void CSatchelCharge :: Precache( void )
{
	PRECACHE_MODEL("models/grenade.mdl");
	PRECACHE_SOUND("weapons/g_bounce1.wav");
	PRECACHE_SOUND("weapons/g_bounce2.wav");
	PRECACHE_SOUND("weapons/g_bounce3.wav");
}

void CSatchelCharge :: BounceSound( void )
{
	if ( !( m_flLastBounceSound <= gpGlobals->time ) )
		return;

	const char *sounds[] = { "weapons/g_bounce1.wav", "weapons/g_bounce2.wav", "weapons/g_bounce3.wav" };
	int sound = RANDOM_LONG( 0, 2 );

	if ( sound >= 0 && sound < ARRAYSIZE( sounds ) )
		EMIT_SOUND_DYN( edict(), CHAN_VOICE, sounds[sound], 1.0, ATTN_NORM, 0, PITCH_NORM, 0, false, g_vecZero );

	m_flLastBounceSound = gpGlobals->time + 0.1;
}

bool CSatchelCharge::PlayerUse( CBaseEntity *other )
{
	if ( !other || !other->IsPlayer() || other->edict() != pev->owner )
		return false;

	CBasePlayer *player = dynamic_cast<CBasePlayer *>( other );

	if ( !player )
		return false;

	if ( !g_pGameRules->CanHaveAmmo( player, CBasePlayer::GetAmmoIndex( "Satchel Charge" ), SATCHEL_MAX_CARRY, NULL ) )
		return false;

	CBaseEntity *item = Create( "weapon_satchel", pev->origin, pev->angles, true );

	if ( !item )
		return false;

	item->pev->spawnflags |= SF_CREATEDWEAPON | SF_GIVENITEM;
	item->pev->owner = player->edict();
	player->GiveItem( item );
	pev->movetype = MOVETYPE_FOLLOW;
	pev->solid = SOLID_NOT;
	pev->aiment = player->edict();
	pev->effects = EF_NODRAW;
	pev->modelindex = 0;
	pev->model = 0;
	pev->owner = player->edict();
	pev->nextthink = gpGlobals->time + 0.1;
	SetThink( NULL );
	SetTouch( NULL );
	SetUse( NULL );
	EMIT_SOUND( player->edict(), CHAN_ITEM, "items/gunpickup2.wav", 1.0, ATTN_NORM );
	pev->solid = SOLID_NOT;
	UTIL_Remove( this );
	return true;
}

void CSatchelCharge::PreDetonateUse( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value )
{
	if ( useType == USE_SET )
		PlayerUse( activator );
	else
		DetonateUse( activator, caller, useType, value );
}

LINK_ENTITY_TO_CLASS( weapon_satchel, CSatchel );

bool WasDeployed( CBasePlayerItem *item, CBasePlayer *player )
{
	return ( item->pev->spawnflags & ( SF_CREATEDWEAPON | SF_GIVENITEM ) ) == ( SF_CREATEDWEAPON | SF_GIVENITEM ) &&
	       item->pev->owner == player->edict();
}

//=========================================================
// CALLED THROUGH the newly-touched weapon's instance. The existing player weapon is pOriginal
//=========================================================
bool CSatchel::AddDuplicate( CBasePlayerItem *pOriginal )
{
	if ( !g_pGameRules->IsMultiplayer() || !pOriginal || pOriginal->m_iId != WEAPON_SATCHEL )
		return CBasePlayerWeapon::AddDuplicate( pOriginal );

	CSatchel *satchel = dynamic_cast<CSatchel *>( pOriginal );

		if ( !satchel )
		return CBasePlayerWeapon::AddDuplicate( pOriginal );

	bool ownPickup = WasDeployed( this, pOriginal->m_hPlayer );

	if ( satchel->m_chargeReady != 0 && !ownPickup )
		return FALSE;

	if ( ownPickup && satchel->m_hPlayer )
	{
		CBasePlayer *player = satchel->m_hPlayer;
		CBaseEntity *charge = NULL;
		int count = 0;

		while ( ( charge = UTIL_FindEntityInSphere( charge, player->pev->origin, 4096 ) ) != NULL )
		{
			if ( FClassnameIs( charge->pev, "monster_satchel" ) && charge->pev->owner == player->edict() )
				++count;
		}

		if ( count <= 1 )
		{
			satchel->m_chargeReady = 0;

			if ( player->m_pActiveItem && player->m_pActiveItem->m_iId == WEAPON_SATCHEL )
			{
				bool classic = SatchelClassicMode();

				satchel->SetV_Model( satchel->GetV_Model( classic ? "models/hlclassic/v_satchel.mdl" : "models/v_satchel.mdl" ) );
				satchel->SetP_Model( satchel->GetP_Model( classic ? "models/hlclassic/p_satchel.mdl" : "models/p_satchel.mdl" ) );
				satchel->SendWeaponAnim( SATCHEL_DRAW );
				satchel->m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
				satchel->m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
			}
		}
	}

	return CBasePlayerWeapon::AddDuplicate ( pOriginal );
}

//=========================================================
//=========================================================
bool CSatchel::AddToPlayer( CBasePlayer *pPlayer )
{
	m_chargeReady = 0;
	return CBasePlayerWeapon::AddToPlayer( pPlayer );
}

void CSatchel::Spawn( void )
{
	Precache( );
	m_iId = WEAPON_SATCHEL;
	m_iDefaultAmmo = SATCHEL_DEFAULT_GIVE;
	SET_MODEL( edict(), GetW_Model( SatchelClassicMode() ? "models/hlclassic/w_satchel.mdl" : "models/w_satchel.mdl" ) );
	m_flCustomDmg = pev->dmg;
	CBasePhysicsObject::Spawn();// get ready to fall down.
}


void CSatchel::Precache( void )
{
	PrecacheCustomModels();

	bool classic = SatchelClassicMode();

	PRECACHE_MODEL( classic ? "models/hlclassic/v_satchel.mdl" : "models/v_satchel.mdl");
	PRECACHE_MODEL( classic ? "models/hlclassic/v_satchel_radio.mdl" : "models/v_satchel_radio.mdl");
	PRECACHE_MODEL( classic ? "models/hlclassic/w_satchel.mdl" : "models/w_satchel.mdl");
	PRECACHE_MODEL( classic ? "models/hlclassic/p_satchel.mdl" : "models/p_satchel.mdl");
	PRECACHE_MODEL( classic ? "models/hlclassic/p_satchel_radio.mdl" : "models/p_satchel_radio.mdl");

	UTIL_PrecacheOther( "monster_satchel" );
}

bool CSatchel::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Satchel Charge";
	p->iMaxAmmo1 = SATCHEL_MAX_CARRY;
	p->pszAmmo1Entity = "weapon_satchel";
	p->iAmmo1Drop = 1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo2Entity = NULL;
	p->iAmmo2Drop = AMMO_DROP_NONE;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 4;
	p->iPosition = 1;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY | ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;
	p->iId = m_iId = WEAPON_SATCHEL;
	p->iWeight = SATCHEL_WEIGHT;

	return true;
}

//=========================================================
//=========================================================
bool CSatchel::IsUseable( void )
{
	return m_hPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] > 0 || m_chargeReady != 0;
}

bool CSatchel::CanDeploy( void )
{
	return m_hPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] > 0 || m_chargeReady != 0;
}

bool CSatchel::Deploy( void )
{
	if ( m_chargeReady == 2 )
		m_chargeReady = 0;

	bool classic = SatchelClassicMode();

	if ( m_chargeReady == 0 )
	{
		SetV_Model( GetV_Model( classic ? "models/hlclassic/v_satchel.mdl" : "models/v_satchel.mdl" ) );
		SetP_Model( GetP_Model( classic ? "models/hlclassic/p_satchel.mdl" : "models/p_satchel.mdl" ) );
		SendWeaponAnim( SATCHEL_DRAW );
		strncpy( m_hPlayer->m_szAnimExtention, "trip", sizeof( m_hPlayer->m_szAnimExtention ) );
	}
	else
	{
		SetV_Model( GetV_Model( classic ? "models/hlclassic/v_satchel_radio.mdl" : "models/v_satchel_radio.mdl" ) );
		SetP_Model( GetP_Model( classic ? "models/hlclassic/p_satchel_radio.mdl" : "models/p_satchel_radio.mdl" ) );
		SendWeaponAnim( SATCHEL_RADIO_DRAW );
		strncpy( m_hPlayer->m_szAnimExtention, "hive", sizeof( m_hPlayer->m_szAnimExtention ) );
	}

	m_hPlayer->SetAnimation( PLAYER_DEPLOY );
	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );
	return TRUE;
}

void CSatchel::Holster( int skiplocal /* = 0 */ )
{
	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), g_evWeaponCustom, 0.0, (float *) &g_vecZero, (float *) &g_vecZero,
	                     7.0, 1.0, WEAPONCUSTOM_SOUND, CHAN_WEAPON, 0, 0 );

	if ( !m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] && ( m_chargeReady == 0 || m_chargeReady == 2 ) )
	{
		SetThink( &CSatchel::DestroyItem );
		pev->nextthink = UTIL_WeaponTimeBase() + 0.1;
	}

	m_hPlayer->pev->viewmodel = 0;
}

void CSatchel::PrimaryAttack( void )
{
	if ( m_chargeReady == 0 )
	{
		Throw( );
		return;
	}

	if ( m_chargeReady != 1 )
		return;

	SendWeaponAnim( SATCHEL_RADIO_FIRE );

		CBaseEntity *pSatchel = NULL;

		while ((pSatchel = UTIL_FindEntityInSphere( pSatchel, m_hPlayer->pev->origin, 4096 )) != NULL)
		{
			if (FClassnameIs( pSatchel->pev, "monster_satchel") && pSatchel->pev->owner == m_hPlayer->edict() )
				{
					pSatchel->Use( m_hPlayer, m_hPlayer, USE_ON, 0 );
					m_chargeReady = 2;
				}
			}

	m_chargeReady = 2;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
}

void CSatchel::SecondaryAttack( void )
{
	if ( m_chargeReady != 2 )
	{
		Throw( );
	}
}


void CSatchel::Throw( void )
{
	if ( m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		return;

		Vector vecThrow = gpGlobals->v_forward * 274 + m_hPlayer->pev->velocity;
	CBaseEntity *pSatchel = Create( "monster_satchel", m_hPlayer->pev->origin, g_vecZero, true, m_hPlayer->edict() );

	pSatchel->pev->iuser2 = pev->iuser2;
	pSatchel->Spawn();
		pSatchel->pev->velocity = vecThrow;
		pSatchel->pev->avelocity.y = 400;
	pSatchel->pev->dmg = m_flCustomDmg;

	if ( m_flCustomDmg <= 0.0f )
		pSatchel->pev->dmg = sk_plr_satchel.value;

	bool classic = SatchelClassicMode();

	SetV_Model( GetV_Model( classic ? "models/hlclassic/v_satchel_radio.mdl" : "models/v_satchel_radio.mdl" ) );
	SetP_Model( GetP_Model( classic ? "models/hlclassic/p_satchel_radio.mdl" : "models/p_satchel_radio.mdl" ) );
	SendWeaponAnim( SATCHEL_RADIO_DRAW );

		// player "shoot" animation
		m_hPlayer->SetAnimation( PLAYER_ATTACK1 );

		m_chargeReady = 1;
	--m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType];

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
	}


void CSatchel::WeaponIdle( void )
{
	if ( !( m_flTimeWeaponIdle <= UTIL_WeaponTimeBase() ) )
		return;

	switch( m_chargeReady )
	{
	case 0:
		SendWeaponAnim( SATCHEL_FIDGET1 );
		strncpy( m_hPlayer->m_szAnimExtention, "trip", sizeof( m_hPlayer->m_szAnimExtention ) );
		break;
	case 1:
		SendWeaponAnim( SATCHEL_RADIO_FIDGET1 );
		strncpy( m_hPlayer->m_szAnimExtention, "hive", sizeof( m_hPlayer->m_szAnimExtention ) );
		break;
	case 2:
	{
		if ( !m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] )
		{
			m_chargeReady = 0;
			RetireWeapon();
			return;
		}

		bool classic = SatchelClassicMode();

		SetV_Model( GetV_Model( classic ? "models/hlclassic/v_satchel.mdl" : "models/v_satchel.mdl" ) );
		SetP_Model( GetP_Model( classic ? "models/hlclassic/p_satchel.mdl" : "models/p_satchel.mdl" ) );
		SendWeaponAnim( SATCHEL_DRAW );
		strncpy( m_hPlayer->m_szAnimExtention, "trip", sizeof( m_hPlayer->m_szAnimExtention ) );

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_chargeReady = 0;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
		break;
	}
	}
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );// how long till we do this again.
}

void CSatchel::GetWeaponData( weapon_data_s *data )
{
	data->iuser1 = m_iPrimaryAmmoType;
	data->iuser2 = m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType];
	data->iuser3 = m_iSecondaryAmmoType;
	data->iuser4 = m_hPlayer->m_rgAmmo[m_iSecondaryAmmoType];
	data->fuser1 = m_chargeReady;
}

void CSatchel::SetWeaponData( weapon_data_s *data )
{
	m_iPrimaryAmmoType = data->iuser1;
	m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] = data->iuser2;
	m_iSecondaryAmmoType = data->iuser3;
	m_hPlayer->m_rgAmmo[m_iSecondaryAmmoType] = data->iuser4;
	m_chargeReady = (int) data->fuser1;
}

//=========================================================
// DeactivateSatchels - removes all satchels owned by
// the provided player. Should only be used upon death.
//
// Made this global on purpose.
//=========================================================
void DeactivateSatchels( CBasePlayer *pOwner )
{
	edict_t *pFind; 

	pFind = FIND_ENTITY_BY_CLASSNAME( NULL, "monster_satchel" );

	while ( !FNullEnt( pFind ) )
	{
		CBaseEntity *pEnt = CBaseEntity::Instance( pFind );
		CSatchelCharge *pSatchel = (CSatchelCharge *)pEnt;

		if ( pSatchel )
		{
			if ( pSatchel->pev->owner == pOwner->edict() )
			{
				pSatchel->Deactivate();
			}
		}

		pFind = FIND_ENTITY_BY_CLASSNAME( pFind, "monster_satchel" );
	}
}

#endif
