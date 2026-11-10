#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "schedule.h"
#include "weapons.h"
#include "player.h"
#include "gamerules.h"
#include "effects.h"
#include "shockroach.h"
#include "shockrifle.h"
#include "customentity.h"
#include "user_messages.h"
#include "event_flags.h"
#include "weaponinfo.h"

#define SHOCKRIFLE_PRIMARY_VOLUME 450

extern cvar_t sk_plr_shockrifle_beam;
extern cvar_t sk_plr_shockrifle_recharge;
extern cvar_t sk_shockroach_dmg_xpl_touch;
extern cvar_t sk_shockroach_dmg_xpl_splash;

static const int shockRifleDamageBits = DMG_SHOCK | DMG_SHOCK_GLOW;
static const char *shockRifleSparkSounds[] =
{
	"buttons/spark1.wav",
	"buttons/spark2.wav",
	"buttons/spark3.wav",
	"buttons/spark4.wav",
	"buttons/spark5.wav",
	"buttons/spark6.wav"
};

static const Vector shockRifleBeamDirections[] =
{
	Vector( 1.0f, 0.0f, 0.0f ),
	Vector( 0.995f, -0.0995f, 0.0f ),
	Vector( 0.995f, 0.0995f, 0.0f ),
	Vector( 0.995f, 0.0f, 0.0995f )
};

LINK_ENTITY_TO_CLASS( weapon_shockrifle, CShockRifle );

bool CShockRifle::GetItemInfo( ItemInfo *info )
{
	info->pszName = STRING( pev->classname );
	info->pszAmmo1 = "shock charges";
	info->iMaxAmmo1 = SHOCK_MAX_CARRY;
	info->pszAmmo1Entity = NULL;
	info->iAmmo1Drop = -1;
	info->pszAmmo2 = NULL;
	info->iMaxAmmo2 = -1;
	info->pszAmmo2Entity = NULL;
	info->iAmmo2Drop = -1;
	info->iMaxClip = SHOCKRIFLE_MAX_CLIP;
	info->iSlot = 9;
	info->iPosition = 0;
	info->iId = m_iId = WEAPON_SHOCKRIFLE;
	info->iFlags = ITEM_FLAG_NOAUTORELOAD | ITEM_FLAG_NOAUTOSWITCHEMPTY;
	info->iWeight = SHOCKRIFLE_WEIGHT;

	return TRUE;
}

bool CShockRifle::AddToPlayer( CBasePlayer *player )
{
	if ( !CBasePlayerWeapon::AddToPlayer( player ) )
		return FALSE;

	if ( IsMultiplayer() && !( pev->spawnflags & SF_NODEFAULTAMMO ) )
		player->m_rgAmmo[PrimaryAmmoIndex()] = SHOCK_MAX_CARRY;

	AddToPlayerMessage();
	return TRUE;
}

void CShockRifle::Materialize( void )
{
	CBasePickupObject::Materialize();
}

void CShockRifle::CustomDecrement( float decrement )
{
	m_flRechargeTime = V_max( m_flRechargeTime - decrement, -0.01 );
	m_flAmmoDecayTime = V_max( m_flAmmoDecayTime - decrement, -0.01 );
}

void CShockRifle::Precache( void )
{
	PrecacheModel_Intercept( this, "models/v_shock.mdl" );
	PrecacheModel_Intercept( this, "models/w_shock_rifle.mdl" );
	PrecacheModel_Intercept( this, "models/p_shock.mdl" );
	m_iBeam = PrecacheModel_Intercept( this, "sprites/lgtning.spr" );
	PrecacheModel_Intercept( this, "sprites/blueflare1.spr" );

	PrecacheSound_Intercept( this, "weapons/shock_discharge2.wav" );
	PrecacheSound_Intercept( this, "weapons/shock_draw.wav" );
	PrecacheSound_Intercept( this, "weapons/shock_fire.wav" );
	PrecacheSound_Intercept( this, "weapons/shock_impact.wav" );
	PrecacheSound_Intercept( this, "weapons/shock_recharge.wav" );
	PrecacheSound_Intercept( this, "debris/beamstart15.wav" );
	PrecacheSound_Intercept( this, "weapons/lhit.wav" );
	PrecacheSound_Intercept( this, "weapons/lhit_end.wav" );

	for ( int i = 0; i < ARRAYSIZE( shockRifleSparkSounds ); i++ )
		PrecacheSound_Intercept( this, (char *) shockRifleSparkSounds[i] );

	PrecacheCustomModels();
	UTIL_PrecacheOther( "monster_shockroach" );
	UTIL_PrecacheOther( "shock_beam" );

	m_usShockRifle = PRECACHE_EVENT( 1, "events/shockrifle.sc" );
	m_usShockRifleBeam = PRECACHE_EVENT( 1, "events/shockriflebeam.sc" );
}

void CShockRifle::Spawn( void )
{
	m_bExclusiveHold = true;
	m_iId = WEAPON_SHOCKRIFLE;
	pev->classname = MAKE_STRING( "weapon_shockrifle" );

	Precache();
	SetModel_Intercept( edict(), GetW_Model( "models/w_shock_rifle.mdl" ) );

	m_iDefaultAmmo = SHOCKRIFLE_DEFAULT_GIVE;
	m_flCustomDmg = pev->dmg;
	m_flArmorRecharge = sk_plr_shockrifle_recharge.value;

	if ( sk_plr_shockrifle_recharge.value < 0.0f )
		m_flArmorRecharge = 0.0f;

	if ( sk_plr_shockrifle_recharge.value > 20.0f )
		m_flArmorRecharge = 20.0f;

	CBasePhysicsObject::Spawn();
}

const char *CShockRifle::GetPickupSound( void )
{
	return "weapons/shock_draw.wav";
}

bool CShockRifle::Deploy( void )
{
	if ( m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgSRPrimed, NULL, m_hPlayer->pev );
		WRITE_BYTE( m_hPlayer->entindex() );
		WRITE_BYTE( 0 );
		WRITE_BYTE( 0 );
		WRITE_BYTE( 0 );
		WRITE_BYTE( 0 );
		MESSAGE_END();
	}

	m_hPlayer->m_fGrappleActive = false;
	return DefaultDeploy( GetV_Model( "models/v_shock.mdl" ), GetP_Model( "models/p_shock.mdl" ),
		2, "bow", 0, 0 );
}

void CShockRifle::Holster( int skiplocal )
{
	if ( m_fInAttack )
		EndBeamAttack();

	m_hPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;

	MESSAGE_BEGIN( MSG_ONE, gmsgSRPrimedOff, NULL, m_hPlayer->pev );
	WRITE_BYTE( m_hPlayer->entindex() );
	MESSAGE_END();

	m_hPlayer->pev->viewmodel = 0;
}

CBasePlayerItem *CShockRifle::DropItem( void )
{
	CShockRoach *roach = GetClassPtr( (CShockRoach *) NULL );

	if ( roach )
	{
		roach->pev->origin = m_hPlayer->pev->origin;
		roach->pev->owner = m_hPlayer->edict();
		roach->pev->angles.y = m_hPlayer->pev->v_angle.y;
		roach->pev->spawnflags |= SF_MONSTER_FALL_TO_GROUND;
		roach->Spawn();

		g_engfuncs.pfnAngleVectors( m_hPlayer->pev->v_angle, roach->pev->velocity, NULL, NULL );
		roach->pev->velocity = roach->pev->velocity * 400.0f;
		roach->SetTouch( NULL );
		roach->SetActivity( ACT_LEAP );

		if ( m_fPrimed )
		{
			roach->m_fExplosiveLeap = true;

			MESSAGE_BEGIN( MSG_ONE, gmsgSRPrimedOff, NULL, m_hPlayer->pev );
			WRITE_BYTE( m_hPlayer->entindex() );
			MESSAGE_END();
		}
	}

	DestroyItem();
	return NULL;
}

void CShockRifle::PrimaryAttack( void )
{
	if ( m_fInAttack )
		EndBeamAttack();

	if ( m_hPlayer->pev->waterlevel == 3 )
	{
		UseAmmo( 20 );
		SecondaryAttack();
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		return;
	}

	if ( m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		return;

	m_hPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
	m_hPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;
	UTIL_MakeVectors( m_hPlayer->pev->v_angle );

	Vector source = m_hPlayer->GetGunPosition() + gpGlobals->v_forward * 16.0f +
		gpGlobals->v_right * 8.0f - gpGlobals->v_up * 12.0f;

	PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), m_usShockRifle, 0.0, (float *) &source, (float *) &g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );

	Vector angles = m_hPlayer->pev->v_angle + m_hPlayer->pev->punchangle;

	angles.x = -angles.x;
	CBaseEntity *bolt = CBaseEntity::Create( "shock_beam", source, angles, false, m_hPlayer->edict() );
	bolt->pev->velocity = gpGlobals->v_forward * 2000.0f;
	m_hPlayer->SetAnimation( PLAYER_ATTACK1 );

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_hPlayer->random_seed, 10, 15 );
	UseAmmo( 2 );
}

void CShockRifle::SecondaryAttack( void )
{
	Vector source = m_hPlayer->GetGunPosition();
	Vector angles = m_hPlayer->pev->v_angle + m_hPlayer->pev->punchangle;

	UTIL_MakeVectors( angles );

	if ( m_hPlayer->pev->flags & FL_INWATER )
		UseAmmo( 5 );

	if ( m_fInAttack )
	{
		if ( m_fInAttack == 1 )
		{
			BeamFire( source, angles );
			m_hPlayer->m_iWeaponVolume = SHOCKRIFLE_PRIMARY_VOLUME;

			if ( m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
			{
				EndBeamAttack();
				m_fInAttack = 0;
			}
		}

		return;
	}

	if ( m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
	{
		PlayEmptySound();
		return;
	}

	m_flNextBeamAmmoTime = gpGlobals->time;
	m_hPlayer->m_iWeaponVolume = SHOCKRIFLE_PRIMARY_VOLUME;
	EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_WEAPON, "debris/beamstart15.wav", 1.0, ATTN_NORM, 0, PITCH_NORM );
	EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_STATIC, "weapons/lhit.wav", 1.0, ATTN_NORM, 0, PITCH_NORM );

	pev->dmgtime = gpGlobals->time + 0.2;
	m_fInAttack = 1;
	CreateBeamEffect();
}

void CShockRifle::TertiaryAttack( void )
{
	WeaponIdle();
}

void CShockRifle::BeamFire( const Vector &source, const Vector &angles )
{
	Vector directions[4];
	TraceResult traces[4];
	int damageCounts[4];
	bool splash[4];
	bool monster[4];
	int selected = -1;
	float selectedDistance = 1024.0f;

	for ( int i = 0; i < 4; i++ )
	{
		const Vector &beam = shockRifleBeamDirections[i];

		directions[i] = gpGlobals->v_forward * beam.x + gpGlobals->v_right * beam.y + gpGlobals->v_up * beam.z;
		damageCounts[i] = 0;
		splash[i] = true;
		monster[i] = false;
		UTIL_TraceLine( source, source + directions[i] * 1024.0f, dont_ignore_monsters, m_hPlayer->edict(), &traces[i] );

		if ( traces[i].fAllSolid || traces[i].flFraction == 1.0f )
			continue;

		CBaseEntity *target = CBaseEntity::Instance( traces[i].pHit );

		if ( !target )
			continue;

		damageCounts[i] = 1;
		monster[i] = target->pev->movetype == MOVETYPE_STEP || target->pev->movetype == MOVETYPE_FLY;

		if ( selected != -1 && monster[selected] && !monster[i] )
			continue;

		float distance = 1024.0f - ( 1.0f - traces[i].flFraction ) * 1024.0f;

		if ( ( ( selected == -1 || !monster[selected] ) && monster[i] ) || !( selectedDistance <= distance ) )
		{
			selected = i;
			selectedDistance = distance;
		}
	}

	if ( selected != -1 && gpGlobals->time > pev->dmgtime )
	{
		for ( int i = 0; i < 4; i++ )
		{
			if ( !monster[i] )
			{
				damageCounts[i] = 0;
				damageCounts[selected]++;
			}
		}

		for ( int i = 0; i < 4; i++ )
		{
			if ( !damageCounts[i] )
				continue;

			CBaseEntity *target = CBaseEntity::Instance( traces[i].pHit );

			if ( target->pev->takedamage == DAMAGE_NO )
				continue;

			float damage = damageCounts[i] * sk_plr_shockrifle_beam.value;

			if ( m_flCustomDmg > 0.0f )
				damage = ( m_flCustomDmg / 10.0f ) * damageCounts[i];

			ClearMultiDamage();

			if ( target->IsMonster() && target->IsAlive() && m_hPlayer->IRelationship( target ) < R_NO )
			{
				if ( target->pev->armortype > target->pev->armorvalue )
				{
					target->pev->armorvalue += m_flArmorRecharge;

					if ( target->pev->armorvalue > target->pev->armortype )
						target->pev->armorvalue = target->pev->armortype;
				}
			}
			else
			{
				target->TraceAttack( m_hPlayer->pev, damage, directions[i], &traces[i], shockRifleDamageBits );
				ApplyMultiDamage( m_hPlayer->pev, m_hPlayer->pev );
			}

			if ( splash[i] )
			{
				int count = 1;

				for ( int j = 0; j < 4; j++ )
				{
					if ( i != j && traces[i].pHit == traces[j].pHit )
					{
						splash[j] = false;
						count++;
					}
				}

				BeamSplash( target->Center(), target->edict(), count * damage );
			}
		}

		pev->dmgtime = gpGlobals->time + 0.2;
	}

	if ( gpGlobals->time >= m_flNextBeamAmmoTime )
	{
		UseAmmo( 1 );
		m_flNextBeamAmmoTime = gpGlobals->time + 0.08;
	}

	PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_hPlayer->edict(), m_usShockRifleBeam, 0.0, (float *) &g_vecZero, (float *) &g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );
}

void CShockRifle::BeamSplash( const Vector &source, edict_t *ignored, float damage )
{
	CBaseEntity *target = NULL;

	while ( ( target = UTIL_FindEntityInSphere( target, source, 256.0f ) ) != NULL )
	{
		if ( target->pev->takedamage == DAMAGE_NO || target == m_hPlayer ||
			target->pev->deadflag == DEAD_DEAD )
			continue;

		TraceResult trace;
		Vector end = target->BodyTarget( source );

		UTIL_TraceLine( source, end, dont_ignore_monsters, ignored, &trace );

		if ( trace.flFraction != 1.0f && trace.pHit != target->edict() )
			continue;

		if ( trace.fStartSolid )
		{
			trace.vecEndPos = source;
			trace.flFraction = 0;
		}

		if ( trace.flFraction == 1.0f )
		{
			target->TakeDamage( m_hPlayer->pev, m_hPlayer->pev, damage / 1.5, shockRifleDamageBits );
		}
		else
		{
			ClearMultiDamage();
			target->TraceAttack( m_hPlayer->pev, damage / 1.5,
				( trace.vecEndPos - source ).Normalize(), &trace, shockRifleDamageBits );
			ApplyMultiDamage( m_hPlayer->pev, m_hPlayer->pev );
		}

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMPOINTS );
		WRITE_COORD( source.x );
		WRITE_COORD( source.y );
		WRITE_COORD( source.z );
		WRITE_COORD( trace.vecEndPos.x );
		WRITE_COORD( trace.vecEndPos.y );
		WRITE_COORD( trace.vecEndPos.z );
		WRITE_SHORT( m_iBeam );
		WRITE_BYTE( 0 );
		WRITE_BYTE( 0 );
		WRITE_BYTE( 1 );
		WRITE_BYTE( 20 );
		WRITE_BYTE( 100 );
		WRITE_BYTE( 96 );
		WRITE_BYTE( 180 );
		WRITE_BYTE( 255 );
		WRITE_BYTE( 255 );
		WRITE_BYTE( 50 );
		MESSAGE_END();
	}
}

void CShockRifle::CreateBeamEffect( void )
{
	for ( int i = 0; i < ARRAYSIZE( m_hBeamSprites ); i++ )
	{
		edict_t *edict = m_hBeamSprites[i].Get();

		if ( edict && !edict->free && GET_PRIVATE( edict ) )
			continue;

		CSprite *sprite = CSprite::SpriteCreate( "sprites/blueflare1.spr", g_vecZero, FALSE );

		m_hBeamSprites[i] = sprite;
		sprite->SetScale( 0.5f );
		sprite->SetTransparency( kRenderGlow, 96, 180, 255, 255, kRenderFxNoDissipation );
		sprite->pev->spawnflags |= SF_SPRITE_TEMPORARY;
		sprite->SetAttachment( m_hPlayer->edict(), i + 1 );
	}
}

void CShockRifle::DestroyBeamEffect( void )
{
	for ( int i = 0; i < ARRAYSIZE( m_hBeamSprites ); i++ )
	{
		edict_t *edict = m_hBeamSprites[i].Get();

		if ( edict && !edict->free && GET_PRIVATE( edict ) )
		{
			UTIL_Remove( (CBaseEntity *) GET_PRIVATE( edict ) );
			m_hBeamSprites[i] = NULL;
		}
	}
}

void CShockRifle::EndBeamAttack( void )
{
	EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_STATIC, "weapons/lhit_end.wav", 1.0, ATTN_NORM, 0, PITCH_NORM );

	DestroyBeamEffect();

	m_fInAttack = 0;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0;
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
}

void CShockRifle::AmmoDecay( void )
{
	if ( m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 30 )
		return;

	if ( !( UTIL_WeaponTimeBase() > m_flAmmoDecayTime ) )
		return;

	UseAmmo( 1 );
	m_flAmmoDecayTime = ( m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] / 60.0f ) + ( UTIL_WeaponTimeBase() + 0.05 );
}

void CShockRifle::UseAmmo( int count )
{
	if ( m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		return;

	m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= count;

	if ( m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
	{
		m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] = 0;
		DetonateInHand();
	}
}

void CShockRifle::Prime( void )
{
	m_fPrimed = true;

	unsigned char time[sizeof( float )];

	memcpy( time, &gpGlobals->time, sizeof( time ) );
	MESSAGE_BEGIN( MSG_ONE, gmsgSRPrimed, NULL, m_hPlayer->pev );
	WRITE_BYTE( m_hPlayer->entindex() );
	WRITE_BYTE( time[0] );
	WRITE_BYTE( time[1] );
	WRITE_BYTE( time[2] );
	WRITE_BYTE( time[3] );
	MESSAGE_END();
}

void CShockRifle::SparkEffect( void )
{
	float probability = V_min( 1.0f, ( 30 - m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] ) / 5.0f );

	if ( UTIL_SharedRandomFloat( m_hPlayer->random_seed, 0, 1 ) >= probability )
		return;

	int attachment = RANDOM_LONG( 1, 4 );
	float minWidth = (int) ( probability * 20.0f + 5.0f );
	float width = UTIL_SharedRandomFloat( m_hPlayer->random_seed, minWidth, minWidth + probability * 30.0f );
	Vector angles = m_hPlayer->pev->angles;

	angles.x *= 9.0f;
	UTIL_MakeVectors( angles );

	Vector end = m_hPlayer->GetGunPosition() + gpGlobals->v_forward * 24.0f + gpGlobals->v_right * 4.0f;
	float extent = probability * 16.0f + 8.0f;
	float z = UTIL_SharedRandomFloat( m_hPlayer->random_seed, -extent, extent );
	float y = UTIL_SharedRandomFloat( m_hPlayer->random_seed, -extent, extent );
	float x = UTIL_SharedRandomFloat( m_hPlayer->random_seed, -extent, extent );

	end = end + Vector( x, y, z );

	CBeam *beam = CBeam::BeamCreate( "sprites/lgtning.spr", (int) width );

	if ( beam )
	{
		beam->SetType( 1 );
		beam->SetEndEntity( m_hPlayer->entindex() );
		beam->SetEndAttachment( attachment );
		beam->SetStartPos( end );
		beam->RelinkBeam();

		beam->pev->spawnflags = SF_BEAM_TEMPORARY;
		beam->SetFlags( BEAM_FSHADEIN );
		beam->SetScrollRate( 50 );
		beam->SetNoise( 100 );
		beam->SetColor( 96, 180, 255 );
		beam->SetThink( &CBaseEntity::SUB_Remove );
		beam->pev->nextthink = gpGlobals->time + 0.05;
	}

	if ( UTIL_SharedRandomFloat( m_hPlayer->random_seed, 0, 1 ) > 0.7 )
	{
		int sound = RANDOM_LONG( 0, ARRAYSIZE( shockRifleSparkSounds ) - 1 );

		EMIT_SOUND_DYN( m_hPlayer->edict(), CHAN_AUTO, shockRifleSparkSounds[sound],
			1.0, ATTN_NORM, 0, (int) ( probability * 20.0f + 100.0f ) );
	}
}

void CShockRifle::DetonateInHand( void )
{
	Vector source = m_hPlayer->GetGunPosition() + gpGlobals->v_forward * 16.0f +
		gpGlobals->v_right * 8.0f - gpGlobals->v_up * 12.0f;

	MESSAGE_BEGIN( MSG_PVS, gmsgSRDetonate, source );
	WRITE_COORD( source.x );
	WRITE_COORD( source.y );
	WRITE_COORD( source.z );
	WRITE_BYTE( 64 );
	MESSAGE_END();

	if ( m_hPlayer->pev->takedamage != DAMAGE_NO )
		m_hPlayer->TakeDamage( pev, m_hPlayer->pev, sk_shockroach_dmg_xpl_touch.value,
			shockRifleDamageBits | DMG_NEVERGIB );

	::RadiusDamage( pev->origin, pev, m_hPlayer->pev, sk_shockroach_dmg_xpl_splash.value,
		sk_shockroach_dmg_xpl_splash.value * 2.5, CLASS_NONE, shockRifleDamageBits | DMG_NEVERGIB );
	g_pGameRules->GetNextBestWeapon( m_hPlayer, this );
	m_hPlayer->RemovePlayerItem( this );
	SetThink( &CBaseEntity::SUB_Remove );
	pev->nextthink = gpGlobals->time;

	MESSAGE_BEGIN( MSG_ONE, gmsgSRPrimedOff, NULL, m_hPlayer->pev );
	WRITE_BYTE( m_hPlayer->entindex() );
	MESSAGE_END();
}

void CShockRifle::ItemPostFrame( void )
{
	if ( m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 30 )
	{
		if ( !m_fPrimed )
			Prime();

		if ( m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
			DetonateInHand();
	}

	if ( m_fPrimed )
		SparkEffect();

	CBasePlayerWeapon::ItemPostFrame();
}

void CShockRifle::InactiveItemPostFrame( void )
{
	AmmoDecay();
}

void CShockRifle::WeaponIdle( void )
{
	if ( m_fInAttack )
		EndBeamAttack();

	AmmoDecay();

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	int animation;
	float random = UTIL_SharedRandomFloat( m_hPlayer->random_seed, 0, 1 );

	if ( random <= 0.75 )
	{
		animation = 4;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.5;
	}
	else
	{
		animation = 0;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.75;
	}

	SendWeaponAnim( animation, 0, 0 );
}

void CShockRifle::GetWeaponData( weapon_data_s *data )
{
	data->iuser1 = m_iPrimaryAmmoType;
	data->iuser2 = m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType];
	data->iuser3 = m_iSecondaryAmmoType;
	data->iuser4 = m_hPlayer->m_rgAmmo[m_iSecondaryAmmoType];
	data->fuser1 = m_flRechargeTime;
}

void CShockRifle::SetWeaponData( weapon_data_s *data )
{
	m_iPrimaryAmmoType = data->iuser1;
	m_hPlayer->m_rgAmmo[m_iPrimaryAmmoType] = data->iuser2;
	m_flRechargeTime = data->fuser1;
}
