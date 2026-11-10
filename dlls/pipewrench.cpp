#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "gamerules.h"
#include "game.h"
#include "server_support.h"
#include "pipewrench.h"

#define WRENCH_SWING_NONE		0
#define WRENCH_SWING_WINDUP	1
#define WRENCH_SWING_HOLD		2

#define WRENCH_BODYHIT_VOLUME	128

LINK_ENTITY_TO_CLASS( weapon_pipewrench, CWrench );

enum
{
	WRENCH_IDLE1 = 0,
	WRENCH_IDLE2,
	WRENCH_IDLE3,
	WRENCH_DRAW,
	WRENCH_ATTACK1HIT = 5,
	WRENCH_ATTACK1MISS,
	WRENCH_ATTACK2HIT,
	WRENCH_ATTACK2MISS,
	WRENCH_ATTACK3HIT,
	WRENCH_ATTACK3MISS,
	WRENCH_BIGWINDUP,
	WRENCH_BIGHIT,
	WRENCH_BIGMISS,
	WRENCH_BIGHOLD
};

CWrench::CWrench()
{
	m_trHit.vecEndPos = Vector( 0, 0, 0 );
	m_trHit.vecPlaneNormal = Vector( 0, 0, 0 );
}

void CWrench::OnCreate( void )
{
	CBasePlayerWeapon::OnCreate();
	m_iBounceMaterial = 1;
}

void CWrench::Spawn( void )
{
	m_iId = WEAPON_PIPEWRENCH;
	Precache();
	SetModel_Intercept( edict(), GetW_Model( "models/w_pipe_wrench.mdl" ) );
	m_iClip = WEAPON_NOCLIP;
	m_flCustomDmg = pev->dmg;
	CBasePhysicsObject::Spawn();
}

void CWrench::Precache( void )
{
	PrecacheModel_Intercept( this, "models/v_pipe_wrench.mdl" );
	PrecacheModel_Intercept( this, "models/w_pipe_wrench.mdl" );
	PrecacheModel_Intercept( this, "models/p_pipe_wrench.mdl" );
	PrecacheCustomModels();
	PrecacheSound_Intercept( this, "weapons/pwrench_hit1.wav" );
	PrecacheSound_Intercept( this, "weapons/pwrench_hit2.wav" );
	PrecacheSound_Intercept( this, "weapons/pwrench_hitbod1.wav" );
	PrecacheSound_Intercept( this, "weapons/pwrench_hitbod2.wav" );
	PrecacheSound_Intercept( this, "weapons/pwrench_hitbod3.wav" );
	PrecacheSound_Intercept( this, "weapons/pwrench_miss1.wav" );
	PrecacheSound_Intercept( this, "weapons/pwrench_miss2.wav" );
	PrecacheSound_Intercept( this, "weapons/pwrench_big_miss.wav" );
}

bool CWrench::GetItemInfo( ItemInfo *info )
{
	info->pszName = STRING( pev->classname );
	info->pszAmmo1 = NULL;
	info->iMaxAmmo1 = -1;
	info->pszAmmo1Entity = NULL;
	info->iAmmo1Drop = -1;
	info->pszAmmo2 = NULL;
	info->iMaxAmmo2 = -1;
	info->pszAmmo2Entity = NULL;
	info->iAmmo2Drop = -1;
	info->iMaxClip = WEAPON_NOCLIP;
	info->iSlot = 0;
	info->iPosition = 1;
	info->iId = WEAPON_PIPEWRENCH;
	info->iFlags = ITEM_FLAG_ESSENTIAL;
	info->iWeight = 0;
	return TRUE;
}

bool CWrench::AddToPlayer( CBasePlayer *player )
{
	if ( !CBasePlayerWeapon::AddToPlayer( player ) )
		return FALSE;

	AddToPlayerMessage();
	return TRUE;
}

bool CWrench::Deploy( void )
{
	m_iChargeBonus = 0;
	m_iSwingMode = WRENCH_SWING_NONE;
	m_flTimeWeaponIdle = gpGlobals->time + 2.0f;
	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	m_hPlayer->pev->viewmodel = MAKE_STRING( GetV_Model( "models/v_pipe_wrench.mdl" ) );
	m_hPlayer->pev->weaponmodel = MAKE_STRING( GetP_Model( "models/p_pipe_wrench.mdl" ) );
	UTIL_CopyString( m_hPlayer->m_szAnimExtention, "crowbar", sizeof( m_hPlayer->m_szAnimExtention ) );
	SendWeaponAnim( WRENCH_DRAW, 0, 0 );
	m_hPlayer->SetAnimation( PLAYER_DEPLOY, 0 );
	return TRUE;
}

bool CWrench::CanHolster( void )
{
	return m_iSwingMode == WRENCH_SWING_NONE;
}

void CWrench::Holster( int skiplocal )
{
	m_fInReload = FALSE;
	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	m_hPlayer->pev->viewmodel = 0;
}

void CWrench::PrimaryAttack( void )
{
	if ( m_iSwingMode )
	{
		WeaponIdle();
		return;
	}

	if ( !Swing( true ) )
	{
		SetThink( &CWrench::SwingAgain );
		pev->nextthink = gpGlobals->time + 0.1;
	}
}

void CWrench::SwingAgain( void )
{
	m_iSwingMode = WRENCH_SWING_NONE;
	Swing( false );
}

void CWrench::SecondaryAttack( void )
{
	if ( m_iSwingMode == WRENCH_SWING_NONE )
	{
		SendWeaponAnim( WRENCH_BIGWINDUP, 0, 0 );
		m_iSwingMode = WRENCH_SWING_WINDUP;
		m_flTimeWeaponIdle = gpGlobals->time + 0.8;
		m_flNextSecondaryAttack = m_flTimeWeaponIdle;
		m_flNextPrimaryAttack = gpGlobals->time + 2.5;
		m_hPlayer->SetAnimation( PLAYER_ATTACK1, 1 );
	}
	else if ( m_iSwingMode == WRENCH_SWING_WINDUP )
	{
		if ( gpGlobals->time > m_flTimeWeaponIdle )
		{
			m_hPlayer->SetAnimation( PLAYER_ATTACK1, 2 );
			SendWeaponAnim( WRENCH_BIGHOLD, 0, 0 );
			m_iSwingMode = WRENCH_SWING_HOLD;
			m_flNextChargeTime = gpGlobals->time + 0.5;
			m_iChargeBonus = 10;
		}
	}
	else
	{
		m_hPlayer->SetAnimation( PLAYER_ATTACK1, 2 );

		if ( gpGlobals->time > m_flNextChargeTime )
		{
			if ( m_iChargeBonus < 85 )
				m_iChargeBonus += 15;

			m_flNextChargeTime = gpGlobals->time + 0.4;
		}
	}
}

void CWrench::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	if ( !m_iSwingMode )
	{
		float choice = RANDOM_FLOAT( 0, 10 );
		int animation;

		if ( choice <= 1.5f )
		{
			animation = WRENCH_IDLE1;
			m_flTimeWeaponIdle = gpGlobals->time + 2.0f;
		}
		else if ( choice <= 3.0f )
		{
			animation = WRENCH_IDLE2;
			m_flTimeWeaponIdle = gpGlobals->time + 3.0f;
		}
		else
		{
			animation = WRENCH_IDLE3;
			m_flTimeWeaponIdle = gpGlobals->time + 9.0f;
		}

		SendWeaponAnim( animation, 0, 0 );
	}

	if ( m_iSwingMode )
	{
		m_iSwingMode = WRENCH_SWING_NONE;
		BigSwing();
		m_flTimeWeaponIdle = gpGlobals->time + 2.0;
		m_flNextSecondaryAttack = gpGlobals->time + 1.0;
	}
}

void CWrench::Smack( void )
{
	DecalGunshot( &m_trHit, BULLET_PLAYER_CROWBAR );
}

bool CWrench::Swing( bool first )
{
	TraceResult trace;

	trace.vecEndPos = Vector( 0, 0, 0 );
	trace.vecPlaneNormal = Vector( 0, 0, 0 );
	UTIL_MakeVectors( m_hPlayer->pev->v_angle );

	Vector start = m_hPlayer->GetGunPosition();
	Vector end = start + gpGlobals->v_forward * 32.0f;

	UTIL_TraceLine( start, end, dont_ignore_monsters, m_hPlayer->edict(), &trace );

	if ( trace.flFraction >= 1.0f )
	{
		UTIL_TraceHull( start, end, dont_ignore_monsters, head_hull, m_hPlayer->edict(), &trace );

		if ( trace.flFraction < 1.0f )
		{
			CBaseEntity *target = CBaseEntity::Instance( trace.pHit );

			if ( !target || target->IsBSPModel() )
				UTIL_FindHullIntersection( start, trace, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX,
				                           m_hPlayer->edict(), 1000000.0f );

			end = trace.vecEndPos;
		}

		if ( trace.flFraction >= 1.0f )
		{
			if ( first )
			{
				int swing = m_iSwing;

				m_iSwing = (int) ( (unsigned int) m_iSwing + 1 );

				switch ( swing % 3 )
				{
					case 0:
						SendWeaponAnim( WRENCH_ATTACK1MISS, 0, 0 );
						break;
					case 1:
						SendWeaponAnim( WRENCH_ATTACK2MISS, 0, 0 );
						break;
					case 2:
						SendWeaponAnim( WRENCH_ATTACK3MISS, 0, 0 );
						break;
				}

				m_flNextPrimaryAttack = gpGlobals->time + 0.65;

				switch ( RANDOM_LONG( 0, 1 ) )
				{
					case 0:
						EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_WEAPON,
							"weapons/pwrench_miss1.wav", 1.0f, ATTN_NORM, 0, 94 + RANDOM_LONG( 0, 15 ) );
						break;
					case 1:
						EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_WEAPON,
							"weapons/pwrench_miss2.wav", 1.0f, ATTN_NORM, 0, 94 + RANDOM_LONG( 0, 15 ) );
						break;
				}

				m_hPlayer->SetAnimation( PLAYER_ATTACK1, 0 );
			}

			m_flNextSecondaryAttack = gpGlobals->time + 0.5;
			m_flTimeWeaponIdle = gpGlobals->time + 1.0;
			return false;
		}
	}

	CBaseEntity *target = CBaseEntity::Instance( trace.pHit );
	int swing = m_iSwing;

	m_iSwing = (int) ( (unsigned int) m_iSwing + 1 );

	switch ( swing % 2 + 1 )
	{
		case 0:
			SendWeaponAnim( WRENCH_ATTACK1HIT, 0, 0 );
			break;
		case 1:
			SendWeaponAnim( WRENCH_ATTACK2HIT, 0, 0 );
			break;
		case 2:
			SendWeaponAnim( WRENCH_ATTACK3HIT, 0, 0 );
			break;
	}

	switch ( RANDOM_LONG( 0, 1 ) )
	{
		case 0:
			EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_STATIC,
				"weapons/pwrench_miss1.wav", 1.0f, ATTN_NORM, 0, 94 + RANDOM_LONG( 0, 15 ) );
			break;
		case 1:
			EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_STATIC,
				"weapons/pwrench_miss2.wav", 1.0f, ATTN_NORM, 0, 94 + RANDOM_LONG( 0, 15 ) );
			break;
	}

	m_hPlayer->SetAnimation( PLAYER_ATTACK1, 0 );
	ClearMultiDamage();

	float damage = sk_plr_wrench.value;

	if ( m_flCustomDmg > 0 )
		damage = m_flCustomDmg;

	m_iChargeBonus = 1;

	if ( !( gpGlobals->time > m_flNextPrimaryAttack + 1.0f ) && !g_pGameRules->IsMultiplayer() )
		target->TraceAttack( m_hPlayer->pev, damage * 0.55, gpGlobals->v_forward, &trace, DMG_CLUB );
	else
		target->TraceAttack( m_hPlayer->pev, damage, gpGlobals->v_forward, &trace, DMG_CLUB );

	ApplyMultiDamage( m_hPlayer->pev, m_hPlayer->pev );
	m_iChargeBonus = 0;
	m_flNextPrimaryAttack = gpGlobals->time + 0.58;

	float volume = 1.0f;

	if ( target->Classify() == CLASS_NONE || target->Classify() == CLASS_MACHINE
		|| target->BloodColor() == DONT_BLEED )
	{
		float materialVolume = TEXTURETYPE_PlaySound( &trace, start, start + ( end - start ) * 2.0f, BULLET_PLAYER_CROWBAR );

		if ( g_pGameRules->IsMultiplayer() )
			materialVolume = 1.0f;

		switch ( RANDOM_LONG( 0, 1 ) )
		{
			case 0:
				EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_WEAPON,
					"weapons/pwrench_hit1.wav", materialVolume, ATTN_NORM, 0, 98 + RANDOM_LONG( 0, 3 ) );
				break;
			case 1:
				EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_WEAPON,
					"weapons/pwrench_hit2.wav", materialVolume, ATTN_NORM, 0, 98 + RANDOM_LONG( 0, 3 ) );
				break;
		}
	}
	else
	{
		if ( target->IsNetClient() )
		{
			Vector direction = ( pev->origin - target->pev->origin ).Normalize();

			target->pev->velocity = target->pev->velocity + direction * 120.0f;
		}

		switch ( RANDOM_LONG( 0, 2 ) )
		{
			case 0:
				EMIT_SOUND( m_hPlayer->edict(), CHAN_WEAPON, "weapons/pwrench_hitbod1.wav", 1.0f, ATTN_NORM );
				break;
			case 1:
				EMIT_SOUND( m_hPlayer->edict(), CHAN_WEAPON, "weapons/pwrench_hitbod2.wav", 1.0f, ATTN_NORM );
				break;
			case 2:
				EMIT_SOUND( m_hPlayer->edict(), CHAN_WEAPON, "weapons/pwrench_hitbod3.wav", 1.0f, ATTN_NORM );
				break;
		}

		m_hPlayer->m_iWeaponVolume = WRENCH_BODYHIT_VOLUME;

		if ( !target->IsAlive() )
			return true;

		volume = 0.1f;
	}

	m_trHit = trace;
	SetThink( &CWrench::Smack );
	pev->nextthink = gpGlobals->time + 0.2;
	m_hPlayer->m_iWeaponVolume = (int) ( volume * 512.0f );
	m_flNextSecondaryAttack = gpGlobals->time + 0.5;
	m_flTimeWeaponIdle = gpGlobals->time + 1.0;
	return true;
}

void CWrench::BigSwing( void )
{
	TraceResult trace;

	trace.vecEndPos = Vector( 0, 0, 0 );
	trace.vecPlaneNormal = Vector( 0, 0, 0 );
	UTIL_MakeVectors( m_hPlayer->pev->v_angle );

	Vector start = m_hPlayer->GetGunPosition();
	Vector end = start + gpGlobals->v_forward * 32.0f;

	UTIL_TraceLine( start, end, dont_ignore_monsters, m_hPlayer->edict(), &trace );

	if ( trace.flFraction >= 1.0f )
	{
		UTIL_TraceHull( start, end, dont_ignore_monsters, head_hull, m_hPlayer->edict(), &trace );

		if ( trace.flFraction < 1.0f )
		{
			CBaseEntity *target = CBaseEntity::Instance( trace.pHit );

			if ( !target || target->IsBSPModel() )
				UTIL_FindHullIntersection( start, trace, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX,
				                           m_hPlayer->edict(), 1000000.0f );

			end = trace.vecEndPos;
		}

		if ( trace.flFraction >= 1.0f )
		{
			SendWeaponAnim( WRENCH_BIGMISS, 0, 0 );
			m_flNextPrimaryAttack = gpGlobals->time + 1.0f;
			EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_STATIC,
				"weapons/pwrench_big_miss.wav", 1.0f, ATTN_NORM, 0, 94 + RANDOM_LONG( 0, 15 ) );
			m_hPlayer->SetAnimation( PLAYER_ATTACK1, 3 );
			m_iChargeBonus = 0;
			return;
		}
	}

	CBaseEntity *target = CBaseEntity::Instance( trace.pHit );

	SendWeaponAnim( WRENCH_BIGHIT, 0, 0 );
	EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_ITEM,
		"weapons/pwrench_big_miss.wav", 1.0f, ATTN_NORM, 0, 94 + RANDOM_LONG( 0, 15 ) );
	m_hPlayer->SetAnimation( PLAYER_ATTACK1, 3 );
	ClearMultiDamage();

	float damage = sk_plr_wrench.value;

	if ( m_flCustomDmg > 0 )
		damage = m_flCustomDmg;

	target->TraceAttack( m_hPlayer->pev, (float) m_iChargeBonus + damage, gpGlobals->v_forward, &trace, DMG_CLUB );
	ApplyMultiDamage( m_hPlayer->pev, m_hPlayer->pev );
	m_flNextPrimaryAttack = gpGlobals->time + 0.58;

	float volume = 1.0f;

	if ( target->Classify() == CLASS_NONE || target->Classify() == CLASS_MACHINE
		|| target->BloodColor() == DONT_BLEED )
	{
		float materialVolume = TEXTURETYPE_PlaySound( &trace, start, start + ( end - start ) * 2.0f, BULLET_PLAYER_CROWBAR );

		switch ( RANDOM_LONG( 0, 1 ) )
		{
			case 0:
				EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_WEAPON,
					"weapons/pwrench_hit1.wav", materialVolume, ATTN_NORM, 0, 98 + RANDOM_LONG( 0, 3 ) );
				break;
			case 1:
				EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_WEAPON,
					"weapons/pwrench_hit2.wav", materialVolume, ATTN_NORM, 0, 98 + RANDOM_LONG( 0, 3 ) );
				break;
		}
	}
	else
	{
		switch ( RANDOM_LONG( 0, 2 ) )
		{
			case 0:
				EMIT_SOUND( m_hPlayer->edict(), CHAN_WEAPON, "weapons/pwrench_hitbod1.wav", 1.0f, ATTN_NORM );
				break;
			case 1:
				EMIT_SOUND( m_hPlayer->edict(), CHAN_WEAPON, "weapons/pwrench_hitbod2.wav", 1.0f, ATTN_NORM );
				break;
			case 2:
				EMIT_SOUND( m_hPlayer->edict(), CHAN_WEAPON, "weapons/pwrench_hitbod3.wav", 1.0f, ATTN_NORM );
				break;
		}

		m_hPlayer->m_iWeaponVolume = WRENCH_BODYHIT_VOLUME;

		if ( target->IsAlive() )
			volume = 0.1f;
	}

	m_trHit = trace;
	SetThink( &CWrench::Smack );
	pev->nextthink = gpGlobals->time + 0.2;
	m_hPlayer->m_iWeaponVolume = (int) ( volume * 512.0f );
	m_iChargeBonus = 0;
}
