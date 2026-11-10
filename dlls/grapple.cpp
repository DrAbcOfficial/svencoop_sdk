#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "grapple.h"
#include "customentity.h"
#include "player.h"
#include "effects.h"
#include "game.h"

enum bgrapple_e
{
	BGRAPPLE_BREATHE = 0,
	BGRAPPLE_LONGIDLE,
	BGRAPPLE_SHORTIDLE,
	BGRAPPLE_COUGH,
	BGRAPPLE_DOWN,
	BGRAPPLE_UP,
	BGRAPPLE_FIRE,
	BGRAPPLE_FIREWAITING,
	BGRAPPLE_FIREREACHED,
	BGRAPPLE_FIRETRAVEL,
	BGRAPPLE_FIRERELEASE
};

LINK_ENTITY_TO_CLASS( weapon_grapple, CBarnacleGrapple );
LINK_ENTITY_TO_CLASS( grappletongue, CBarnacleGrappleTongue );

void CBarnacleGrapple::OnCreate( void )
{
	CBasePlayerWeapon::OnCreate();
	m_iBounceMaterial = 2;
}

void CBarnacleGrapple::Spawn( void )
{
	m_iId = WEAPON_GRAPPLE;
	Precache();
	SetModel_Intercept( edict(), GetW_Model( "models/w_bgrap.mdl" ) );
	m_iClip = WEAPON_NOCLIP;
	m_flCustomDmg = pev->dmg;
	m_fFire = false;

	CBasePhysicsObject::Spawn();
}

void CBarnacleGrapple::Precache( void )
{
	PrecacheCustomModels();
	PrecacheModel_Intercept( this, "models/v_bgrap.mdl" );
	PrecacheModel_Intercept( this, "models/w_bgrap.mdl" );
	PrecacheModel_Intercept( this, "models/p_bgrap.mdl" );
	UTIL_PrecacheOther( "grappletongue" );
	PrecacheSound_Intercept( this, "weapons/bgrapple_cough.wav" );
	PrecacheSound_Intercept( this, "weapons/bgrapple_fire.wav" );
	PrecacheSound_Intercept( this, "weapons/bgrapple_impact.wav" );
	PrecacheSound_Intercept( this, "weapons/bgrapple_pull.wav" );
	PrecacheSound_Intercept( this, "weapons/bgrapple_release.wav" );
	PrecacheSound_Intercept( this, "weapons/alienweap_draw.wav" );
	PrecacheSound_Intercept( this, "weapons/bullet_hit2.wav" );
	PrecacheSound_Intercept( this, "barnacle/bcl_chew1.wav" );
	PrecacheSound_Intercept( this, "barnacle/bcl_chew2.wav" );
}

bool CBarnacleGrapple::GetItemInfo( ItemInfo *p )
{
	p->pszName = STRING( pev->classname );
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo1Entity = NULL;
	p->iAmmo1Drop = AMMO_DROP_NONE;
	p->pszAmmo2Entity = NULL;
	p->iAmmo2Drop = AMMO_DROP_NONE;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 0;
	p->iPosition = 3;
	p->iId = WEAPON_GRAPPLE;
	p->iFlags = ITEM_FLAG_ESSENTIAL;
	p->iWeight = 0;

	return true;
}

bool CBarnacleGrapple::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( !CBasePlayerWeapon::AddToPlayer( pPlayer ) )
		return false;

	AddToPlayerMessage();
	return true;
}

bool CBarnacleGrapple::Deploy( void )
{
	m_flTimeWeaponIdle = gpGlobals->time + 1.5f;
	m_hPlayer->pev->viewmodel = MAKE_STRING( GetV_Model( "models/v_bgrap.mdl" ) );
	m_hPlayer->pev->weaponmodel = MAKE_STRING( GetP_Model( "models/p_bgrap.mdl" ) );
	strncpy( m_hPlayer->m_szAnimExtention, "hive", sizeof( m_hPlayer->m_szAnimExtention ) );
	SendWeaponAnim( BGRAPPLE_UP, 0 );
	m_hPlayer->SetAnimation( PLAYER_DEPLOY );
	m_hPlayer->m_fGrappleFired = false;
	m_hPlayer->m_fGrappleActive = false;

	return true;
}

bool CBarnacleGrapple::CanHolster( void )
{
	return !( m_hPlayer->m_afPhysicsFlags & PFLAG_ONGRAPPLE );
}

void CBarnacleGrapple::Holster( int skiplocal )
{
	m_fInReload = false;
	m_fFire = false;
	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5f;
	m_hPlayer->m_fGrappleFired = false;
	m_hPlayer->pev->viewmodel = 0;
}

void CBarnacleGrapple::ItemPostFrame( void )
{
	if ( m_hPlayer->m_fGrappleActive )
	{
		m_flTimeWeaponIdle = gpGlobals->time;
		WeaponIdle();
	}

	CBasePlayerWeapon::ItemPostFrame();
}

void CBarnacleGrapple::PrimaryAttack( void )
{
	m_fFire = true;

	bool fHit = Swing();

	if ( fHit )
	{
		SendWeaponAnim( BGRAPPLE_FIRE, 0, 0 );
		m_flNextPrimaryAttack = gpGlobals->time + 0.5f;
	}

	if ( m_hPlayer->m_afPhysicsFlags & PFLAG_ONGRAPPLE )
	{
		if ( gpGlobals->time > m_flNextPullSound )
		{
			EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_WEAPON, "weapons/bgrapple_pull.wav", 1, ATTN_NORM, 0, 115 );
			SendWeaponAnim( BGRAPPLE_FIRETRAVEL, 0, 0 );
			m_flNextPullSound = gpGlobals->time + 0.7f;
			m_flNextPrimaryAttack = gpGlobals->time + 0.5f;
		}

		return;
	}

	if ( m_hPlayer->m_fGrappleFired )
	{
		if ( gpGlobals->time > m_flNextFireSound )
		{
			EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_WEAPON, "weapons/bgrapple_pull.wav", 1, ATTN_NORM, 0, 115 );
			SendWeaponAnim( BGRAPPLE_FIREWAITING, 0, 0 );
			m_flNextFireSound = gpGlobals->time + 0.7f;
		}

		return;
	}

	if ( fHit )
		return;

	m_hPlayer->m_fGrappleFired = true;
	UTIL_MakeVectors( m_hPlayer->pev->v_angle );

	Vector vecSrc = m_hPlayer->GetGunPosition();
	Vector vecOrigin = m_hPlayer->pev->origin + gpGlobals->v_forward * 24 + gpGlobals->v_up * 16;
	TraceResult tr;

	UTIL_TraceLine( vecSrc, vecSrc + gpGlobals->v_forward * 2048, dont_ignore_monsters, dont_ignore_glass, m_hPlayer->edict(), &tr );

	Vector vecAngles = UTIL_VecToAngles( ( tr.vecEndPos - vecOrigin ).Normalize() );

	CBarnacleGrappleTongue::Shoot( m_hPlayer->pev, vecOrigin, vecAngles, m_hPlayer );
	SendWeaponAnim( BGRAPPLE_FIRE, 0, 0 );
	EMIT_SOUND_DYN( edict(), CHAN_WEAPON, "weapons/bgrapple_fire.wav", 1, ATTN_NORM, 0, 115 + RANDOM_LONG( 0, 15 ) );
	m_flTimeWeaponIdle = gpGlobals->time + 0.5f;
	m_flNextPrimaryAttack = gpGlobals->time + 0.5f;
}

void CBarnacleGrapple::SecondaryAttack( void )
{
	m_hPlayer->m_fGrappleRappelMode = !m_hPlayer->m_fGrappleRappelMode;
	ClientPrint( m_hPlayer->pev, HUD_PRINTCENTER, m_hPlayer->m_fGrappleRappelMode ? "Rappelling Mode\n" : "Pull Mode\n" );
	m_flNextSecondaryAttack = gpGlobals->time + 0.5f;
}

void CBarnacleGrapple::WeaponIdle( void )
{
	if ( m_hPlayer->m_fGrappleActive || m_fFire )
	{
		SendWeaponAnim( BGRAPPLE_FIRERELEASE, 0, 0 );
		EMIT_SOUND_DYN( edict(), CHAN_WEAPON, "common/null.wav", 0, 0, SND_STOP, 100 );
		m_hPlayer->m_fGrappleFired = false;
		m_hPlayer->m_fGrappleActive = false;
		m_flTimeWeaponIdle = gpGlobals->time + 0.95f;
		m_fFire = false;
		return;
	}

	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	int iAnim;
	float flChoice = RANDOM_FLOAT( 0, 10 );

	if ( flChoice > 1.5f )
	{
		if ( flChoice > 3 )
		{
			iAnim = BGRAPPLE_LONGIDLE;
			m_flTimeWeaponIdle = gpGlobals->time + 10;
		}
		else if ( RANDOM_LONG( 0, 1 ) > 0 )
		{
			iAnim = BGRAPPLE_SHORTIDLE;
			m_flTimeWeaponIdle = gpGlobals->time + 1.5f;
		}
		else
		{
			iAnim = BGRAPPLE_COUGH;
			m_flTimeWeaponIdle = gpGlobals->time + 5;
			EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_WEAPON, "weapons/bgrapple_cough.wav", 1, ATTN_NORM, 0, 100 + RANDOM_LONG( 0, 15 ) );
		}
	}
	else
	{
		iAnim = BGRAPPLE_BREATHE;
		m_flTimeWeaponIdle = gpGlobals->time + 3;
	}

	SendWeaponAnim( iAnim, 0 );
}

bool CBarnacleGrapple::Swing( void )
{
	UTIL_MakeVectors( m_hPlayer->pev->v_angle );

	Vector vecSrc = m_hPlayer->GetGunPosition();
	Vector vecEnd = vecSrc + gpGlobals->v_forward * 24;
	TraceResult tr;

	UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, m_hPlayer->edict(), &tr );

	if ( tr.flFraction == 1 )
	{
		UTIL_TraceHull( vecSrc, vecEnd, dont_ignore_monsters, head_hull, m_hPlayer->edict(), &tr );

		if ( tr.flFraction >= 1 )
			return false;

		CBaseEntity *pHit = CBaseEntity::Instance( tr.pHit );

		if ( !pHit || pHit->IsBSPModel() )
			UTIL_FindHullIntersection( vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, m_hPlayer->edict(), 1000000.0f );

		vecEnd = tr.vecEndPos;
	}

	if ( tr.flFraction >= 1 )
		return false;

	CBaseEntity *pEntity = CBaseEntity::Instance( tr.pHit );

	if ( !pEntity || pEntity->IsBSPModel() || pEntity->Classify() == CLASS_NONE || pEntity->Classify() == CLASS_MACHINE )
		return false;

	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:
		EMIT_SOUND( edict(), CHAN_ITEM, "barnacle/bcl_chew1.wav", 1, ATTN_NORM );
		break;
	case 1:
		EMIT_SOUND( edict(), CHAN_ITEM, "barnacle/bcl_chew2.wav", 1, ATTN_NORM );
		break;
	}

	m_hPlayer->SetAnimation( PLAYER_ATTACK1 );
	ClearMultiDamage();
	pEntity->TraceAttack( m_hPlayer->pev, m_flCustomDmg > 0 ? m_flCustomDmg : sk_plr_grapple.value, gpGlobals->v_forward, &tr, DMG_CLUB );
	ApplyMultiDamage( m_hPlayer->pev, m_hPlayer->pev );
	m_flNextPrimaryAttack = gpGlobals->time + 0.58f;
	m_hPlayer->m_iWeaponVolume = 128;

	if ( !pEntity->IsAlive() && FBitSet( pEntity->pev->flags, FL_MONSTER )
		&& pEntity->MyMonsterPointer()->m_MonsterState != MONSTERSTATE_SCRIPT && pEntity->BloodColor() != DONT_BLEED )
	{
		pEntity->Killed( m_hPlayer->pev, GIB_ALWAYS );
	}

	return true;
}

void CBarnacleGrappleTongue::Spawn( void )
{
	Precache();
	pev->movetype = MOVETYPE_FLY;
	pev->classname = MAKE_STRING( "grappletongue" );
	pev->solid = SOLID_BBOX;
	pev->rendermode = kRenderTransAlpha;
	pev->renderamt = 0;
	pev->frame = 0;
	pev->scale = 1;
	SetModel_Intercept( edict(), "sprites/tongue.spr" );
	UTIL_SetSize( pev, Vector( -2, -2, -2 ), Vector( 2, 2, 2 ) );
	m_iGrappleType = 0;
	m_flPullDelay = gpGlobals->time + 3;
}

void CBarnacleGrappleTongue::Precache( void )
{
	PrecacheModel_Intercept( this, "sprites/tongue.spr" );
}

void CBarnacleGrappleTongue::Shoot( entvars_t *pevOwner, Vector vecOrigin, Vector vecAngles, CBasePlayer *pPlayer )
{
	CBarnacleGrappleTongue *pTongue = GetClassPtr( (CBarnacleGrappleTongue *) NULL );

	if ( !pTongue )
		return;

	UTIL_MakeAimVectors( vecAngles );
	pTongue->Spawn();
	pTongue->pev->owner = ENT( pevOwner );
	pTongue->m_hPlayer = pPlayer;
	pTongue->pev->angles = vecAngles;
	pTongue->pev->velocity = gpGlobals->v_forward * 1400;
	pTongue->m_hTarget = NULL;
	UTIL_SetOrigin( pTongue->pev, vecOrigin );
	pTongue->m_hBeam = CBeam::BeamCreate( "sprites/tongue.spr", 15 );
	pTongue->m_hBeam->EntsInit( pTongue->entindex(), pPlayer->entindex() );
	pTongue->m_hBeam->SetStartAttachment( 1 );
	pTongue->m_hBeam->SetEndAttachment( 2 );
	pTongue->m_hBeam->SetBrightness( 125 );
	pTongue->m_hBeam->SetFlags( BEAM_FSOLID );
	pTongue->m_hBeam->SetNoise( 1 );
	pTongue->SetThink( &CBarnacleGrappleTongue::FlyThink );
	pTongue->pev->nextthink = gpGlobals->time;
}

void CBarnacleGrappleTongue::FlyThink( void )
{
	if ( m_hPlayer && m_hPlayer->IsAlive() && m_hPlayer->m_fGrappleFired
		&& ( m_hPlayer->pev->button & IN_ATTACK )
		&& ( m_hPlayer->pev->origin - pev->origin ).Length() < 8192 && !m_fTerminate )
	{
		pev->nextthink = gpGlobals->time + 0.1f;
		CheckMovement();
		return;
	}

	if ( m_hPlayer && m_hPlayer->pev->movetype == MOVETYPE_NONE )
		m_hPlayer->pev->movetype = MOVETYPE_WALK;

	Killed( pev, GIB_NORMAL );
}

void CBarnacleGrappleTongue::Touch( CBaseEntity *pOther )
{
	if ( !m_hPlayer )
	{
		m_fTerminate = true;
		return;
	}

	if ( pOther == m_hPlayer )
		return;

	SetTouch( NULL );
	EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_WEAPON, "weapons/bgrapple_impact.wav", 1, ATTN_NORM, 0, 100 + RANDOM_LONG( 0, 7 ) );
	pev->angles = UTIL_VecToAngles( pev->velocity.Normalize() );
	pev->velocity = g_vecZero;
	pev->solid = SOLID_NOT;
	UTIL_SetOrigin( pev, pev->origin );
	m_iGrappleType = 0;

	if ( !CheckType( pOther ) )
		m_fTerminate = true;
}

void CBarnacleGrappleTongue::Killed( entvars_t *pevAttacker, int iGib )
{
	if ( m_hBeam )
		UTIL_Remove( m_hBeam );

	m_hBeam = NULL;

	if ( m_hPlayer )
	{
		EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_WEAPON, "weapons/bgrapple_release.wav", 1, ATTN_NORM, 0, 110 + RANDOM_LONG( 0, 7 ) );
		m_hPlayer->m_afPhysicsFlags &= ~PFLAG_ONGRAPPLE;
		ClearBits( m_hPlayer->pev->flags, FL_FLY );

		if ( m_hPlayer->pev->movetype != MOVETYPE_NOCLIP )
			m_hPlayer->pev->movetype = MOVETYPE_WALK;

		m_hPlayer->m_fGrappleActive = true;
	}

	SUB_Remove();
}

int CBarnacleGrappleTongue::GetMonsterType( CBaseEntity *pEntity )
{
	if ( FClassnameIs( pEntity->pev, "monster_headcrab" ) || FClassnameIs( pEntity->pev, "monster_shockroach" )
		|| FClassnameIs( pEntity->pev, "monster_leech" ) || FClassnameIs( pEntity->pev, "monster_snark" )
		|| FClassnameIs( pEntity->pev, "monster_cockroach" ) || FClassnameIs( pEntity->pev, "monster_babycrab" )
		|| FClassnameIs( pEntity->pev, "monster_stukabat" ) || FClassnameIs( pEntity->pev, "monster_chumtoad" )
		|| FClassnameIs( pEntity->pev, "monster_rat" ) )
	{
		return 1;
	}

	return FClassnameIs( pEntity->pev, "monster_tentacle" ) ? 4 : 2;
}

bool CBarnacleGrappleTongue::CheckType( CBaseEntity *pEntity )
{
	if ( pEntity->IsBSPModel() )
	{
		char szTexture[64] = {};
		m_hTarget = NULL;

		edict_t *pHit = INDEXENT( 0 );
		UTIL_MakeVectors( m_hPlayer->pev->v_angle );

		Vector vecSrc = pev->origin;
		Vector vecEnd = vecSrc + gpGlobals->v_forward * 24;
		TraceResult tr;

		UTIL_TraceLine( vecSrc, vecEnd, ignore_monsters, edict(), &tr );

		if ( tr.pHit )
			pHit = tr.pHit;

		const char *pszTexture = TRACE_TEXTURE( pHit, vecSrc, vecEnd );

		if ( !pszTexture )
			return false;

		if ( *pszTexture == '-' || *pszTexture == '+' )
			pszTexture += 2;

		if ( *pszTexture == '{' || *pszTexture == '!' || *pszTexture == '~' || *pszTexture == ' ' )
			pszTexture++;

		strncpy( szTexture, pszTexture, sizeof( szTexture ) );
		szTexture[15] = 0;

		if ( strnicmp( szTexture, "xeno_grapple", 12 ) != 0 )
			return false;

		m_iGrappleType = 1;
		return true;
	}

	if ( FClassnameIs( pEntity->pev, "ammo_spore" ) )
	{
		m_hTarget = pEntity;
		m_iGrappleType = 2;
		return true;
	}

	int iMode = (int) CVAR_GET_FLOAT( "mp_grapple_mode" );

	if ( iMode == 0 )
	{
		if ( GetMonsterType( pEntity ) == 1 )
		{
			m_hTarget = pEntity;
			m_iGrappleType = 3;
			return true;
		}

		if ( !FBitSet( pEntity->pev->flags, FL_CLIENT ) || CVAR_GET_FLOAT( "mp_disable_player_rappel" ) != 0 )
			return false;
	}
	else
	{
		if ( iMode != 1 && iMode != 2 )
			return false;

		if ( !FBitSet( pEntity->pev->flags, FL_CLIENT ) || CVAR_GET_FLOAT( "mp_disable_player_rappel" ) != 0 )
		{
			if ( !FBitSet( pEntity->pev->flags, FL_MONSTER ) )
				return false;

			m_hTarget = pEntity;
			m_iGrappleType = 3;
			return true;
		}
	}

	m_hTarget = pEntity;
	m_iGrappleType = 4;
	return true;
}

void CBarnacleGrappleTongue::PullRappelEntity( void )
{
	if ( !m_hPlayer || !m_hTarget )
	{
		m_fTerminate = true;
		return;
	}

	if ( m_hTarget->IsPlayer() )
	{
		if ( !m_hTarget->IsAlive() || m_hTarget->pev->solid == SOLID_NOT )
		{
			m_fTerminate = true;
			return;
		}

		if ( m_hTarget->pev->groundentity == m_hPlayer->edict() )
		{
			m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 4;
			m_fTerminate = true;
			return;
		}
	}

	if ( GetMonsterType( m_hTarget ) == 4 )
	{
		PullRappel( this );
	}
	else
	{
		pev->origin = m_hTarget->Center();
		PullRappel( m_hTarget );
	}
}

void CBarnacleGrappleTongue::CheckMovement( void )
{
	if ( !m_hPlayer )
		return;

	switch ( m_iGrappleType )
	{
	case 3:
	{
		m_hPlayer->m_afPhysicsFlags |= PFLAG_ONGRAPPLE;

		if ( !m_hTarget || ( FClassnameIs( m_hTarget->pev, "monster_stukabat" ) && m_hTarget->pev->movetype == MOVETYPE_TOSS ) )
		{
			Killed( pev, GIB_NORMAL );
			return;
		}

		int iMode = (int) CVAR_GET_FLOAT( "mp_grapple_mode" );

		if ( iMode != 0 )
		{
			if ( iMode != 1 && iMode != 2 )
				return;

			if ( GetMonsterType( m_hTarget ) != 1 )
			{
				PullRappelEntity();
				return;
			}
		}

		pev->origin = m_hTarget->Center();

		if ( ( m_hPlayer->pev->origin - pev->origin ).Length2D() >= 16
			|| fabs( pev->origin.z - m_hPlayer->pev->origin.z ) >= 16 )
		{
			m_hTarget->pev->velocity = ( m_hPlayer->pev->origin - m_hTarget->pev->origin ).Normalize() * 500;
		}
		break;
	}
	case 4:
		if ( m_hTarget && m_hTarget->IsPlayer() )
			PullRappelEntity();
		break;
	case 2:
		if ( m_hTarget && FClassnameIs( m_hTarget->pev, "ammo_spore" ) )
			PullRappelEntity();
		break;
	case 1:
		PullRappel( this );
		break;
	}
}

void CBarnacleGrappleTongue::PullRappel( CBaseEntity *pEntity )
{
	CBasePlayer *pPlayer = m_hPlayer;

	pPlayer->m_afPhysicsFlags |= PFLAG_ONGRAPPLE;
	pev->movetype = MOVETYPE_FLY;

	if ( pPlayer->m_fGrappleRappelMode )
	{
		ClearBits( pPlayer->pev->flags, FL_FLY );

		if ( pEntity->pev->origin.z > pPlayer->pev->origin.z )
		{
			Vector vecDelta = pEntity->pev->origin - pPlayer->pev->origin;
			vecDelta.z = 0;

			if ( !FBitSet( pPlayer->pev->flags, FL_ONGROUND ) )
				pPlayer->pev->velocity = pPlayer->pev->velocity + vecDelta;

			if ( pPlayer->pev->velocity.Length() > 600 )
				pPlayer->pev->velocity = pPlayer->pev->velocity.Normalize() * 600;

			if ( pPlayer->pev->velocity.z < -250 )
				pPlayer->pev->velocity.z = -250;
		}

		return;
	}

	if ( m_vecPullVelocity == g_vecZero )
		m_vecPullVelocity = pPlayer->pev->velocity;

	if ( m_vecPullVelocity.Length() > 200 )
		m_vecPullVelocity = m_vecPullVelocity.Normalize() * 200;

	m_vecPullVelocity = m_vecPullVelocity + ( pEntity->pev->origin - pPlayer->pev->origin ).Normalize() * 200;

	if ( pPlayer->pev->origin.z < pEntity->pev->origin.z )
	{
		SetBits( pPlayer->pev->flags, FL_FLY );
		ClearBits( pPlayer->pev->flags, FL_ONGROUND );

		float flDistance = ( pEntity->pev->origin - pPlayer->pev->origin ).Length();

		if ( flDistance > 128 || m_flPullDelay > gpGlobals->time )
			pPlayer->pev->velocity = pPlayer->pev->velocity + m_vecPullVelocity;
		else
			pPlayer->pev->velocity = pPlayer->pev->velocity * ( flDistance / 128 ) + m_vecPullVelocity;

		if ( pPlayer->pev->velocity.Length() > 600 )
			pPlayer->pev->velocity = pPlayer->pev->velocity.Normalize() * 600;

		return;
	}

	ClearBits( pPlayer->pev->flags, FL_FLY );
	ClearBits( pPlayer->pev->flags, FL_ONGROUND );

	float flVerticalVelocity = pPlayer->pev->velocity.z;
	pPlayer->pev->velocity = pPlayer->pev->velocity + m_vecPullVelocity;

	if ( pPlayer->pev->velocity.Length() > 400 )
		pPlayer->pev->velocity = pPlayer->pev->velocity.Normalize() * 400;

	pPlayer->pev->velocity.z = flVerticalVelocity;

	if ( pPlayer->pev->velocity.z > 0 )
		pPlayer->pev->velocity.z *= 0.5f;
}
