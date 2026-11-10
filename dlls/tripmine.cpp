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
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "effects.h"
#include "gamerules.h"
#include "CClassicMode.h"
#include "game.h"
#include "server_sound.h"
#include "customentity.h"

#undef SET_MODEL
#define SET_MODEL( entity, model ) SetModel_Intercept( entity, model )
#undef PRECACHE_MODEL
#define PRECACHE_MODEL( path ) UTIL_PrecacheModel( this, path )
#undef PRECACHE_SOUND
#define PRECACHE_SOUND( path ) PrecacheSound_Intercept( this, path )

bool TripmineClassicMode( void )
{
	return CClassicMode::GetInstance()->IsEnabled();
}

#define	TRIPMINE_PRIMARY_VOLUME		450



enum tripmine_e {
	TRIPMINE_IDLE1 = 0,
	TRIPMINE_IDLE2,
	TRIPMINE_ARM1,
	TRIPMINE_ARM2,
	TRIPMINE_FIDGET,
	TRIPMINE_HOLSTER,
	TRIPMINE_DRAW,
	TRIPMINE_WORLD,
	TRIPMINE_GROUND,
};


#ifndef CLIENT_DLL

class CTripmineGrenade : public CGrenade
{
	friend class CTripmine;

	void Spawn( void );
	void Spawn( const char *model );
	void Precache( void );
	int Classify( void );
	int ShowHUDInfo( void ) { return 0; }

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );

	static	TYPEDESCRIPTION m_SaveData[];

	int TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );
	
	void EXPORT WarningThink( void );
	void EXPORT PowerupThink( void );
	void EXPORT BeamBreakThink( void );
	void EXPORT DelayDeathThink( void );
	void Killed( entvars_t *pevAttacker, int iGib );
	bool CheckAttacker( CBaseEntity *pAttacker );
	bool CriticalRemove( void );
	void UpdateOnRemove( void );
	void Detonate( void );

	void MakeBeam( void );
	void KillBeam( void );

	float		m_flPowerUp;
	Vector		m_vecDir;
	Vector		m_vecEnd;
	float		m_flBeamLength;

	EHANDLE		m_hOwner;
	EHANDLE		m_hBeam;
	Vector		m_posOwner;
	Vector		m_angleOwner;
	EHANDLE		m_hRealOwner;
	bool		m_fPlayerPlaced;
	bool m_fSolid;
	bool m_fPlayerTrip;

	void PlayerTrip( void ) { m_fPlayerTrip = true; }
	float m_flBeamLength2;
};

LINK_ENTITY_TO_CLASS( monster_tripmine, CTripmineGrenade );

TYPEDESCRIPTION	CTripmineGrenade::m_SaveData[] = 
{
	DEFINE_FIELD( CTripmineGrenade, m_flPowerUp, FIELD_TIME ),
	DEFINE_FIELD( CTripmineGrenade, m_vecDir, FIELD_VECTOR ),
	DEFINE_FIELD( CTripmineGrenade, m_vecEnd, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( CTripmineGrenade, m_flBeamLength, FIELD_FLOAT ),
	DEFINE_FIELD( CTripmineGrenade, m_hOwner, FIELD_EHANDLE ),
	DEFINE_FIELD( CTripmineGrenade, m_hBeam, FIELD_EHANDLE ),
	DEFINE_FIELD( CTripmineGrenade, m_posOwner, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( CTripmineGrenade, m_angleOwner, FIELD_VECTOR ),
	DEFINE_FIELD( CTripmineGrenade, m_hRealOwner, FIELD_EHANDLE ),
};

IMPLEMENT_SAVERESTORE(CTripmineGrenade,CGrenade);

void CTripmineGrenade :: Spawn( void )
{
	Spawn( TripmineClassicMode() ? "models/hlclassic/w_tripmine.mdl" : "models/w_tripmine.mdl" );
}

void CTripmineGrenade::Spawn( const char *model )
{
	Precache();

	if ( !SetupModel() )
		SET_MODEL( edict(), model );

	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_NOT;
	pev->takedamage = DAMAGE_YES;

	if ( pev->dmg <= 0.0f )
		pev->dmg = sk_plr_tripmine.value;

	pev->health = 1;
	pev->sequence = 1;
	UTIL_SetSize( pev, Vector( -8, -8, -8 ), Vector( 8, 8, 8 ) );
	UTIL_SetOrigin( pev, pev->origin );
	m_fSolid = false;
	m_flPowerUp = gpGlobals->time + ( ( pev->spawnflags & 1 ) ? 1.0 : 2.5 );

	if ( pev->owner )
	{
		EMIT_SOUND_DYN( edict(), CHAN_VOICE, "weapons/mine_deploy.wav", 1.0, ATTN_NORM, 0, PITCH_NORM, 0, false, g_vecZero );
		EMIT_SOUND_DYN( edict(), CHAN_BODY, "weapons/mine_charge.wav", 0.2, ATTN_NORM, 0, PITCH_NORM, 0, false, g_vecZero );
		m_hRealOwner = CBaseEntity::Instance( pev->owner );
	}

	UTIL_MakeAimVectors( pev->angles );
	m_vecDir = gpGlobals->v_forward;
	m_vecEnd = pev->origin + m_vecDir * 2048;
	SetThink( &CTripmineGrenade::PowerupThink );
	pev->nextthink = gpGlobals->time + 0.2;
}

void CTripmineGrenade :: Precache( void )
{
	CBaseEntity::Precache();

	if ( !pev->model )
		PRECACHE_MODEL( TripmineClassicMode() ? "models/hlclassic/w_tripmine.mdl" : "models/w_tripmine.mdl" );
	PRECACHE_SOUND("weapons/mine_deploy.wav");
	PRECACHE_SOUND("weapons/mine_activate.wav");
	PRECACHE_SOUND("weapons/mine_charge.wav");
}


void CTripmineGrenade :: WarningThink( void  )
{
	// play warning sound
	// EMIT_SOUND( ENT(pev), CHAN_VOICE, "buttons/Blip2.wav", 1.0, ATTN_NORM );

	// set to power up
	SetThink( &CTripmineGrenade::PowerupThink );
	pev->nextthink = gpGlobals->time + 1.0;
}

void CTripmineGrenade :: PowerupThink( void  )
{
	TraceResult tr;

	if (m_hOwner == NULL)
	{
		// find an owner
		edict_t *oldowner = pev->owner;
		pev->owner = NULL;
		UTIL_TraceLine( pev->origin + m_vecDir * 8, pev->origin - m_vecDir * 32, dont_ignore_monsters, edict(), &tr );
		if (tr.fStartSolid || (oldowner && tr.pHit == oldowner))
		{
			pev->owner = oldowner;
			m_flPowerUp += 0.1;
			pev->nextthink = gpGlobals->time + 0.1;
			return;
		}
		if (tr.flFraction < 1.0)
		{
			pev->owner = tr.pHit;
			m_hOwner = CBaseEntity::Instance( pev->owner );
			m_posOwner = m_hOwner->pev->origin;
			m_angleOwner = m_hOwner->pev->angles;
		}
		else
		{
			STOP_SOUND( edict(), CHAN_VOICE, "weapons/mine_deploy.wav" );
			STOP_SOUND( edict(), CHAN_BODY, "weapons/mine_charge.wav" );
			SetThink(&CTripmineGrenade::SUB_Remove );
			pev->nextthink = gpGlobals->time + 0.1;
			ALERT( at_console, "WARNING:Tripmine at %.0f, %.0f, %.0f removed\n", pev->origin.x, pev->origin.y, pev->origin.z );
			KillBeam();
			return;
		}
	}
	else if (m_posOwner != m_hOwner->pev->origin || m_angleOwner != m_hOwner->pev->angles)
	{
		EMIT_SOUND_DYN( edict(), CHAN_VOICE, "weapons/mine_deploy.wav", 0, 0, SND_STOP, PITCH_NORM, 0, false, g_vecZero );
		EMIT_SOUND_DYN( edict(), CHAN_BODY, "weapons/mine_charge.wav", 0, 0, SND_STOP, PITCH_NORM, 0, false, g_vecZero );
		CBaseEntity *pMine = Create( "weapon_tripmine", pev->origin + m_vecDir * 24, pev->angles, false );
		pMine->pev->spawnflags |= SF_NORESPAWN;

		SetThink( &CTripmineGrenade::SUB_Remove );
		KillBeam();
		pev->nextthink = gpGlobals->time + 0.1;
		return;
	}
	// ALERT( at_console, "%d %.0f %.0f %0.f\n", pev->owner, m_pOwner->pev->origin.x, m_pOwner->pev->origin.y, m_pOwner->pev->origin.z );

	if ( !( gpGlobals->time <= m_flPowerUp ) )
	{
		UTIL_SetOrigin( pev, pev->origin );
		pev->solid = SOLID_BBOX;
		MakeBeam( );

		// play enabled sound
        EMIT_SOUND_DYN( edict(), CHAN_VOICE, "weapons/mine_activate.wav", 0.5, ATTN_NORM, 1, 75, 0, false, g_vecZero );

		CBaseEntity *entities[16];

		if ( UTIL_EntitiesInBox( entities, ARRAYSIZE( entities ), pev->origin + pev->mins, pev->origin + pev->maxs, FL_CLIENT | FL_MONSTER ) > 0 )
		{
			Detonate();
			return;
		}
	}
	pev->nextthink = gpGlobals->time + 0.1;
}


void CTripmineGrenade :: KillBeam( void )
{
	CBaseEntity *beam = m_hBeam;

	if ( beam )
	{
		UTIL_Remove( beam );
		m_hBeam = NULL;
	}
}

bool CTripmineGrenade::CheckAttacker( CBaseEntity *pAttacker )
{
	return false;
}

int CTripmineGrenade::Classify( void )
{
	return m_fPlayerPlaced ? CLASS_PLAYER_ALLY : CLASS_ALIEN_BIOWEAPON;
}

bool CTripmineGrenade::CriticalRemove( void )
{
	TakeDamage( VARS( INDEXENT( 0 ) ), VARS( INDEXENT( 0 ) ), 10.0f, DMG_GENERIC );
	return true;
}

void CTripmineGrenade::UpdateOnRemove( void )
{
	CGrenade::UpdateOnRemove();
	KillBeam();
}

void CTripmineGrenade::Detonate( void )
{
	CBaseEntity *owner = m_hRealOwner;

	pev->owner = owner ? owner->edict() : NULL;
	pev->health = 0;
	Killed( VARS( pev->owner ), GIB_NORMAL );
}

void CTripmineGrenade :: MakeBeam( void )
{
	TraceResult tr;
	TraceResult trOffset;
	edict_t *oldOwner = pev->owner;

	pev->owner = NULL;
	UTIL_TraceLine( pev->origin, m_vecEnd, dont_ignore_monsters, edict(), &tr );

	m_flBeamLength = tr.flFraction;
	UTIL_TraceLine( pev->origin + m_vecDir * 8, m_vecEnd, dont_ignore_monsters, edict(), &trOffset );
	m_flBeamLength2 = trOffset.flFraction;

	// set to follow laser spot
	SetThink( &CTripmineGrenade::BeamBreakThink );
	pev->nextthink = gpGlobals->time + 0.1;

	Vector vecTmpEnd = pev->origin + m_vecDir * 2048 * m_flBeamLength;
	CBeam *beam = CBeam::BeamCreate( g_pModelNameLaser, 10 );

	m_hBeam = beam;
	beam->PointEntInit( vecTmpEnd, entindex() );
	beam->SetColor( 0, 214, 198 );
	beam->SetScrollRate( 255 );
	beam->SetBrightness( 64 );
	pev->owner = oldOwner;

	if ( tr.flFraction >= 1.0f )
		beam->SetFlags( beam->GetFlags() | BEAM_FSHADEIN );
}

void CTripmineGrenade :: BeamBreakThink( void  )
{
	if ( !m_fSolid )
	{
		CBaseEntity *entities[2];
		int count = UTIL_EntitiesInBox( entities, ARRAYSIZE( entities ), pev->origin - Vector( 12, 12, 12 ),
		                                pev->origin + Vector( 12, 12, 12 ), FL_MONSTER );
		bool occupied = false;

		for ( int i = 0; i < count; ++i )
		{
			if ( entities[i] != this )
				occupied = true;
		}

		if ( !occupied )
		{
			pev->solid = SOLID_BBOX;
			UTIL_SetSize( pev, Vector( -8, -8, -8 ), Vector( 8, 8, 8 ) );
			m_fSolid = true;
		}
	}

	TraceResult tr;
	edict_t *oldOwner = pev->owner;

	pev->owner = NULL;

	// HACKHACK Set simple box using this really nice global!
	gpGlobals->trace_flags = FTRACE_SIMPLEBOX;
	UTIL_TraceLine( pev->origin + m_vecDir * 8, m_vecEnd, dont_ignore_monsters, edict(), &tr );
	pev->owner = oldOwner;

	if ( !m_hBeam )
	{
		MakeBeam( );
		if ( tr.pHit )
			m_hOwner = CBaseEntity::Instance( tr.pHit );	// reset owner too
	}

	if (fabs( m_flBeamLength2 - tr.flFraction ) > 0.001 || !m_hOwner ||
	     m_posOwner != m_hOwner->pev->origin || m_angleOwner != m_hOwner->pev->angles)
	{
		Detonate();
		return;
	}

	pev->nextthink = gpGlobals->time + 0.1;
}

int CTripmineGrenade :: TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	if ( m_flPowerUp <= gpGlobals->time || pev->health <= flDamage )
		return CBaseMonster::TakeDamage( pevInflictor, pevAttacker, flDamage, bitsDamageType );

	SetThink( &CTripmineGrenade::SUB_Remove );
		pev->nextthink = gpGlobals->time + 0.1;
		KillBeam();
		return FALSE;
	}

void CTripmineGrenade::Killed( entvars_t *pevAttacker, int iGib )
{
	pev->takedamage = DAMAGE_NO;

	if ( pevAttacker && ( pevAttacker->flags & FL_CLIENT ) )
		m_fPlayerPlaced = true;

	SetThink( &CTripmineGrenade::DelayDeathThink );
	pev->nextthink = gpGlobals->time + RANDOM_FLOAT( 0.1, 0.3 );
	EMIT_SOUND_DYN( edict(), CHAN_BODY, "common/null.wav", 0.5, ATTN_NORM, 0, PITCH_NORM, 0, false, g_vecZero ); // shut off chargeup
}


void CTripmineGrenade::DelayDeathThink( void )
{
	KillBeam();
	TraceResult tr;
	UTIL_TraceLine ( pev->origin + m_vecDir * 8, pev->origin - m_vecDir * 64,  dont_ignore_monsters, edict(), & tr);

	CBaseEntity *owner = m_hRealOwner;

	if ( owner )
		pev->owner = owner->edict();
	else
		pev->owner = NULL;

	Explode( &tr, DMG_BLAST );
}
#endif

LINK_ENTITY_TO_CLASS( weapon_tripmine, CTripmine );

void CTripmine::Spawn( void )
{
	Precache( );
	SET_MODEL( edict(), GetW_Model( TripmineClassicMode() ? "models/hlclassic/w_tripmine.mdl" : "models/w_tripmine.mdl" ) );
	m_iId = WEAPON_TRIPMINE;// get ready to fall down

	m_iDefaultAmmo = TRIPMINE_DEFAULT_GIVE;
	m_flCustomDmg = pev->dmg;
	CBasePhysicsObject::Spawn();

	if ( !g_pGameRules->IsDeathmatch() )
	{
		m_vecMinHullSize = Vector(-16, -16, 0);
		m_vecMaxHullSize = Vector(16, 16, 28);
	}
}

void CTripmine::Precache( void )
{
	PrecacheCustomModels();

	bool classic = TripmineClassicMode();

	PRECACHE_MODEL( classic ? "models/hlclassic/v_tripmine.mdl" : "models/v_tripmine.mdl");
	PRECACHE_MODEL ( classic ? "models/hlclassic/w_tripmine.mdl" : "models/w_tripmine.mdl" );
	PRECACHE_MODEL( classic ? "models/hlclassic/p_tripmine.mdl" : "models/p_tripmine.mdl");
	UTIL_PrecacheOther( "monster_tripmine" );
}

bool CTripmine::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Trip Mine";
	p->iMaxAmmo1 = TRIPMINE_MAX_CARRY;
	p->pszAmmo1Entity = "weapon_tripmine";
	p->iAmmo1Drop = 1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo2Entity = NULL;
	p->iAmmo2Drop = AMMO_DROP_NONE;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 4;
	p->iPosition = 2;
	p->iId = m_iId = WEAPON_TRIPMINE;
	p->iWeight = TRIPMINE_WEIGHT;
	p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;

	return true;
}

bool CTripmine::Deploy( void )
{
	pev->body = 0;

	if ( TripmineClassicMode() )
		return DefaultDeploy( GetV_Model( "models/hlclassic/v_tripmine.mdl" ), GetP_Model( "models/hlclassic/p_tripmine.mdl" ), TRIPMINE_DRAW, "trip" );

	return DefaultDeploy( GetV_Model( "models/v_tripmine.mdl" ), GetP_Model( "models/p_tripmine.mdl" ), TRIPMINE_DRAW, "trip" );
}

void CTripmine::Holster( int skiplocal /* = 0 */ )
{
	m_hPlayer->m_flNextAttack = 0.5;
	m_fInReload = false;

	if (!m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType])
	{
		SetThink( &CTripmine::DestroyItem );
		pev->nextthink = gpGlobals->time + 0.1;
	}

	EMIT_SOUND_DYN(m_hPlayer->edict(), CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM, 0, PITCH_NORM, 0, false, g_vecZero );
	m_hPlayer->pev->viewmodel = 0;
}

void CTripmine::PrimaryAttack( void )
{
	if (m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		return;

	UTIL_MakeVectors( m_hPlayer->pev->v_angle + m_hPlayer->pev->punchangle );
	Vector vecSrc	 = m_hPlayer->GetGunPosition( );
	TraceResult tr;

	UTIL_TraceLine( vecSrc, vecSrc + gpGlobals->v_forward * 128, dont_ignore_monsters, m_hPlayer->edict(), &tr );

	if (tr.flFraction < 1.0)
	{
		CBaseEntity *pEntity = CBaseEntity::Instance( tr.pHit );
		if ( pEntity && !(pEntity->pev->flags & FL_CONVEYOR) )
		{
			Vector angles = UTIL_VecToAngles( tr.vecPlaneNormal );
			CTripmineGrenade *pEnt = (CTripmineGrenade *) Create( "monster_tripmine", tr.vecEndPos + tr.vecPlaneNormal * 8, angles, true, m_hPlayer->edict() );

			pEnt->pev->sequence = 1;
			pEnt->pev->dmg = m_flCustomDmg;

			if ( m_flCustomDmg <= 0.0f )
				pEnt->pev->dmg = sk_plr_tripmine.value;

			pEnt->Spawn( GetW_Model( TripmineClassicMode() ? "models/hlclassic/w_tripmine.mdl" : "models/w_tripmine.mdl" ) );
			pEnt->m_fPlayerPlaced = true;
			--m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType];

			// player "shoot" animation
			m_hPlayer->SetAnimation( PLAYER_ATTACK1 );

			if ( m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
			{
				// no more mines!
				RetireWeapon();
				return;
			}

			SendWeaponAnim( TRIPMINE_DRAW );
		}
	}

	m_flNextPrimaryAttack = gpGlobals->time + 0.3;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT( 10, 15 );
}

void CTripmine::WeaponIdle( void )
{
	if ( !( m_flTimeWeaponIdle <= gpGlobals->time ) )
		return;

	if ( m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0 )
	{
		SendWeaponAnim( TRIPMINE_DRAW );
	}
	else
	{
		RetireWeapon(); 
		return;
	}

	int iAnim;
	float flRand = RANDOM_FLOAT( 0, 1 );
	if ( !( flRand > 0.25f ) )
	{
		iAnim = TRIPMINE_IDLE1;
		m_flTimeWeaponIdle = gpGlobals->time + 90.0 / 30.0;
	}
	else if ( !( flRand > 0.75f ) )
	{
		iAnim = TRIPMINE_IDLE2;
		m_flTimeWeaponIdle = gpGlobals->time + 60.0 / 30.0;
	}
	else
	{
		iAnim = TRIPMINE_FIDGET;
		m_flTimeWeaponIdle = gpGlobals->time + 100.0 / 30.0;
	}

	SendWeaponAnim( iAnim );
}




