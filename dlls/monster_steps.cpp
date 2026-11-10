#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "pm_materials.h"
#include "server_sound.h"
#include "server_support.h"

enum
{
	STEP_CONCRETE,
	STEP_METAL,
	STEP_DIRT,
	STEP_VENT,
	STEP_GRATE,
	STEP_TILE,
	STEP_SLOSH,
	STEP_WADE,
	STEP_LADDER,
	STEP_WOOD,
	STEP_ORGANIC,
	STEP_SNOW
};

void CBaseMonster::HandleStep( void )
{
	if ( !( pev->flags & FL_ONGROUND ) || !( m_flGroundSpeed >= 25.0f ) )
		return;

	bool walking = m_flGroundSpeed < 150.0f;
	int step = STEP_CONCRETE;
	float volume = walking ? 0.2f : 0.5f;
	Vector origin( ( pev->absmin.x + pev->absmax.x ) * 0.5f,
	               ( pev->absmax.y + pev->absmin.y ) * 0.5f, pev->absmin.z );

	if ( UTIL_PointContents( origin + Vector( 0, 0, 2 ) ) == CONTENTS_WATER || pev->waterlevel == 1 )
		step = STEP_SLOSH;
	else
	{
		Vector start( origin.x, origin.y, pev->absmin.z + 4.0 );
		Vector end( origin.x, origin.y, pev->absmin.z - 24.0 );
		const char *texture = TRACE_TEXTURE( pev->groundentity, start, end );

		if ( texture )
		{
			if ( *texture == '-' )
				texture += 2;

			if ( *texture == '{' || *texture == '!' )
				texture++;

			if ( strnicmp( texture, m_szTextureName, 15 ) != 0 )
			{
				char textureName[16];

				UTIL_CopyString( textureName, texture, sizeof( textureName ) );
				textureName[15] = '\0';
				UTIL_CopyString( m_szTextureName, textureName, sizeof( m_szTextureName ) );
				m_chTextureType = PM_FindMaterialType_SERVER( m_szTextureName );
			}
		}

		switch ( m_chTextureType )
		{
		case CHAR_TEX_METAL:
			step = STEP_METAL;
			break;
		case CHAR_TEX_DIRT:
			step = STEP_DIRT;
			volume = walking ? 0.25f : 0.55f;
			break;
		case CHAR_TEX_VENT:
			step = STEP_VENT;
			volume = walking ? 0.4f : 0.7f;
			break;
		case CHAR_TEX_GRATE:
			step = STEP_GRATE;
			break;
		case CHAR_TEX_TILE:
			step = STEP_TILE;
			break;
		case CHAR_TEX_SLOSH:
			step = STEP_SLOSH;
			break;
		case CHAR_TEX_WOOD:
			step = STEP_WOOD;
			break;
		case CHAR_TEX_FLESH:
			step = STEP_ORGANIC;
			break;
		case CHAR_TEX_SNOW:
			step = STEP_SNOW;
			break;
		}
	}

	PlayStepSound( step, volume );
}

void CBaseMonster::PlayStepSound( int step, float volume )
{
	static int skipStep;
	static const char *const sounds[12][8] =
	{
		{ "player/pl_step1.wav", "player/pl_step3.wav", "player/pl_step2.wav", "player/pl_step4.wav",
		  "player/pl_step5.wav", "player/pl_step6.wav", "player/pl_step7.wav", "player/pl_step8.wav" },
		{ "player/pl_metal1.wav", "player/pl_metal3.wav", "player/pl_metal2.wav", "player/pl_metal4.wav" },
		{ "player/pl_dirt1.wav", "player/pl_dirt3.wav", "player/pl_dirt2.wav", "player/pl_dirt4.wav" },
		{ "player/pl_duct1.wav", "player/pl_duct3.wav", "player/pl_duct2.wav", "player/pl_duct4.wav" },
		{ "player/pl_grate1.wav", "player/pl_grate3.wav", "player/pl_grate2.wav", "player/pl_grate4.wav" },
		{ "player/pl_tile1.wav", "player/pl_tile3.wav", "player/pl_tile2.wav", "player/pl_tile4.wav", "player/pl_tile5.wav" },
		{ "player/pl_slosh1.wav", "player/pl_slosh3.wav", "player/pl_slosh2.wav", "player/pl_slosh4.wav" },
		{ "player/pl_wade1.wav", "player/pl_wade2.wav", "player/pl_wade3.wav", "player/pl_wade4.wav" },
		{ "player/pl_ladder1.wav", "player/pl_ladder3.wav", "player/pl_ladder2.wav", "player/pl_ladder4.wav" },
		{ "player/pl_wood1.wav", "player/pl_wood3.wav", "player/pl_wood2.wav", "player/pl_wood4.wav" },
		{ "player/pl_organic1.wav", "player/pl_organic3.wav", "player/pl_organic2.wav", "player/pl_organic4.wav" },
		{ "player/pl_snow1.wav", "player/pl_snow3.wav", "player/pl_snow2.wav", "player/pl_snow4.wav",
		  "player/pl_snow5.wav", "player/pl_snow6.wav", "player/pl_snow7.wav", "player/pl_snow8.wav" }
	};
	int previousFoot = m_iStepLeft;
	int soundIndex = RANDOM_LONG( 0, 1 ) + previousFoot * 2;

	m_iStepLeft = !previousFoot;

	if ( (unsigned int) step > STEP_SNOW )
		step = STEP_CONCRETE;

	unsigned int soundCount = 4;

	if ( step == STEP_CONCRETE || step == STEP_SNOW )
	{
		soundIndex += RANDOM_LONG( 0, 1 ) ? 4 : 0;
		soundCount = 8;
	}
	else if ( step == STEP_TILE )
	{
		if ( !RANDOM_LONG( 0, 4 ) )
			soundIndex = 4;

		soundCount = 5;
	}
	else if ( step == STEP_WADE )
	{
		if ( !skipStep )
		{
			skipStep = 1;
			return;
		}

		if ( skipStep++ == 3 )
			skipStep = 0;
	}

	if ( (unsigned int) soundIndex >= soundCount )
		return;

	EMIT_SOUND_DYN( pev ? pev->pContainingEntity : NULL, CHAN_BODY, sounds[step][soundIndex],
	                volume, ATTN_NORM, 0, PITCH_NORM, 0, false, g_vecZero );
}
