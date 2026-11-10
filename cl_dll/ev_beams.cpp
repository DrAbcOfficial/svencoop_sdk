#include "hud.h"
#include "cl_util.h"
#include "cl_entity.h"
#include "pm_defs.h"
#include "eventscripts.h"
#include "ev_hldm.h"
#include "ev_sven_events.h"
#include "r_efx.h"
#include "event_api.h"
#include "client_sound.h"

void V_PunchAxis(int axis, float punch);

void EV_FireGauss( event_args_t *args )
{
	int idx = args->entindex;
	float flDamage = args->fparam1;
	bool primaryfire = args->bparam1 != 0;
	Vector origin = args->origin;
	Vector angles = args->angles;
	Vector vecSrc, dest, forward, right, up;
	pmtrace_t tr, exitTrace;
	bool fFirstBeam = true;
	bool fHasPunched = false;
	int nMaxHits = 10;

	gEngfuncs.pEventAPI->EV_KillEvents( idx, "events/gaussspin.sc" );
	EV_GetGunPosition( args, vecSrc, origin );

	int beamModel = gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/smoke.spr" );
	int glowModel = gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/hotglow.spr" );

	AngleVectors( angles, forward, right, up );
	dest = vecSrc + forward * 8192.0f;

	if ( EV_IsLocal( idx ) )
	{
		V_PunchAxis( 0, -2.0 );
		gEngfuncs.pEventAPI->EV_WeaponAnimation( GAUSS_FIRE2, 2 );
	}

	CClient_SoundEngine::GetSoundEngine()->PlayFMODSound( SOUNDMSG_STOP, idx, NULL, CHAN_WEAPON, "common/null.wav", 0, 0, 0, 0, -1, 0 );

	float volume = flDamage * ( 1.0 / 400.0 ) + 0.5;
	const char *sample = gHUD.m_bClassicMode ? "hlclassic/weapons/gauss2.wav" : "weapons/gauss2.wav";

	CClient_SoundEngine::GetSoundEngine()->PlayFMODSound( 0, idx, origin, CHAN_WEAPON,
	                                                      sample, volume, ATTN_NORM, 0, 85 + gEngfuncs.pfnRandomLong( 0, 31 ), -1, 0 );

	while ( flDamage > 10.0f && nMaxHits > 0 )
	{
		nMaxHits--;

		gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );

		// Store off the old count
		gEngfuncs.pEventAPI->EV_PushPMStates();

		// Now add in all of the players.
		gEngfuncs.pEventAPI->EV_SetSolidPlayers( idx - 1 );

		gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
		gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc, dest, PM_STUDIO_BOX, -1, &tr );

		gEngfuncs.pEventAPI->EV_PopPMStates();

		if ( tr.allsolid )
			return;

		float width = primaryfire ? 1.0f : 2.5f;
		float brightness = primaryfire ? 128.0f : flDamage;
		float green = primaryfire ? 128.0f : 255.0f;
		float blue = primaryfire ? 0.0f : 255.0f;

		if ( fFirstBeam )
		{
			if ( EV_IsLocal( idx ) )
				EV_MuzzleFlash();

			fFirstBeam = false;

			gEngfuncs.pEfxAPI->R_BeamEntPoint(
			    idx | 0x1000,
			    tr.endpos,
			    beamModel,
			    0.1, width, 0.1f, brightness,
			    0,
			    0,
			    0, 255, green, blue );
		}
		else
		{
			gEngfuncs.pEfxAPI->R_BeamPoints( vecSrc,
			                                 tr.endpos,
			                                 beamModel,
			                                 0.1, width, 0.1f, brightness,
			                                 0,
			                                 0,
			                                 0, 255, green, blue );
		}

		physent_t *pEntity = gEngfuncs.pEventAPI->EV_GetPhysent( tr.ent );

		if ( !pEntity )
			return;

		if ( pEntity->solid != SOLID_BSP )
		{
			vecSrc = tr.endpos + forward;
			continue;
		}

		float incidence = -DotProduct( tr.plane.normal, forward );

		if ( incidence < 0.5 ) // 60 degrees
		{
			forward = forward + tr.plane.normal * ( incidence + incidence );
			vecSrc = tr.endpos + forward * 8.0f;
			dest = vecSrc + forward * 8192.0f;

			float reflectedDamage = incidence * flDamage;
			Vector fwd = tr.endpos + tr.plane.normal;

			gEngfuncs.pEfxAPI->R_TempSprite( tr.endpos, vec3_origin, 0.2, glowModel, kRenderGlow, kRenderFxNoDissipation, reflectedDamage / 255.0,
			                                 reflectedDamage * 0.5 * 0.1, FTENT_FADEOUT );

			gEngfuncs.pEfxAPI->R_Sprite_Trail( TE_SPRITETRAIL, tr.endpos, fwd,
			                                   glowModel, (int) ( reflectedDamage * 0.3 ), 0.1, gEngfuncs.pfnRandomFloat( 10, 20 ) / 100.0, 100,
			                                   255, 200 );

			// lose energy
			if ( incidence == 0 )
				incidence = 0.1;

			flDamage *= 1.0f - incidence;
			continue;
		}

		EV_HLDM_DecalGunshot( &tr, 0 );

		gEngfuncs.pEfxAPI->R_TempSprite( tr.endpos, vec3_origin, 1.0, glowModel, kRenderGlow, kRenderFxNoDissipation, flDamage / 255.0, 6.0, FTENT_FADEOUT );

		// limit it to one hole punch
		if ( fHasPunched )
			return;

		fHasPunched = true;

		// try punching through wall if secondary attack (primary is incapable of breaking through)
		if ( primaryfire )
		{
			Vector fwd = tr.endpos + tr.plane.normal;

			gEngfuncs.pEfxAPI->R_TempSprite( tr.endpos, vec3_origin, 0.2f, glowModel,
			                                 kRenderGlow, kRenderFxNoDissipation, 200.0 / 255.0, 0.3f, FTENT_FADEOUT );
			gEngfuncs.pEfxAPI->R_Sprite_Trail( TE_SPRITETRAIL, tr.endpos, fwd,
			                                   glowModel, 8, 0.6f, gEngfuncs.pfnRandomFloat( 10, 20 ) / 100.0, 100, 255, 200 );
			return;
		}

		Vector start = tr.endpos + forward * 8.0f;

		// Store off the old count
		gEngfuncs.pEventAPI->EV_PushPMStates();

		// Now add in all of the players.
		gEngfuncs.pEventAPI->EV_SetSolidPlayers( idx - 1 );

		gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
		gEngfuncs.pEventAPI->EV_PlayerTrace( start, dest, PM_STUDIO_BOX, -1, &exitTrace );

		if ( exitTrace.allsolid )
		{
			flDamage = 0;
		}
		else
		{
			gEngfuncs.pEventAPI->EV_PlayerTrace( exitTrace.endpos, tr.endpos, PM_STUDIO_BOX, -1, &exitTrace );

			Vector delta = exitTrace.endpos - tr.endpos;
			float n = Length( delta );

			if ( flDamage > n )
			{
				if ( n == 0 )
					n = 1;

				flDamage -= n;

				Vector fwd = tr.endpos - forward;
				gEngfuncs.pEfxAPI->R_Sprite_Trail( TE_SPRITETRAIL, tr.endpos, fwd,
				                                   glowModel, 3, 0.1, gEngfuncs.pfnRandomFloat( 10, 20 ) / 100.0, 100,
				                                   255, 100 );
				EV_HLDM_DecalGunshot( &exitTrace, 0 );

				gEngfuncs.pEfxAPI->R_TempSprite( exitTrace.endpos, vec3_origin, 0.1, glowModel, kRenderGlow, kRenderFxNoDissipation, flDamage / 255.0, 6.0, FTENT_FADEOUT );

				fwd = exitTrace.endpos - forward;
				gEngfuncs.pEfxAPI->R_Sprite_Trail( TE_SPRITETRAIL, exitTrace.endpos, fwd,
				                                   glowModel, (int) ( flDamage * 0.3 ), 0.1, gEngfuncs.pfnRandomFloat( 10, 20 ) / 100.0, 200,
				                                   255, 40 );
				vecSrc = exitTrace.endpos + forward;
			}
		}

		gEngfuncs.pEventAPI->EV_PopPMStates();
	}
}

static const Vector ShockRifleBeamOffsets[4] =
{
	Vector(1, 0, 0),
	Vector(0.995f, -0.0995f, 0.0f),
	Vector(0.995f, 0.0995f, 0.0f),
	Vector(0.995f, 0.0f, 0.0995f)
};

void EV_FireAdjustedShockRifleBeam(const Vector &origin, const Vector &direction, float distance,
	int idx, pmtrace_t *trace, Vector *points)
{
	Vector start = origin;
	Vector end = origin + direction * distance;

	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetSolidPlayers(idx - 1);
	gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	gEngfuncs.pEventAPI->EV_PlayerTrace(start, end, PM_STUDIO_BOX, -1, trace);
	gEngfuncs.pEventAPI->EV_PopPMStates();

	points[0] = start;
	points[1] = trace->endpos;
}

void EV_DrawShockRifleBeam(pmtrace_t *trace, Vector *points, int attachment)
{
	physent_t *physent = gEngfuncs.pEventAPI->EV_GetPhysent(trace->ent);

	if (physent && (physent->solid == SOLID_BSP || physent->movetype == MOVETYPE_PUSHSTEP))
	{
		gEngfuncs.pEfxAPI->R_SparkEffect(trace->endpos, 10, -100, 100);

		cl_entity_t *entity = gEngfuncs.GetEntityByIndex(physent->info);

		if ((!entity || !(entity->curstate.effects & EF_NODECALS)) && gEngfuncs.pfnRandomLong(0, 6) == 1)
		{
			static char decalName[32];

			snprintf(decalName, sizeof(decalName), "{shot%i", gEngfuncs.pfnRandomLong(1, 5));
			gEngfuncs.pEfxAPI->R_DecalShoot(gEngfuncs.pEfxAPI->Draw_DecalIndex(
				gEngfuncs.pEfxAPI->Draw_DecalIndexFromName(decalName)), trace->ent, 0, trace->endpos, 0);
		}
	}

	int model = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/lgtning.spr");

	gEngfuncs.pEfxAPI->R_BeamEntPoint(attachment, points[1], model,
		0.05f, 5, 0.25f, 1, 5, 0, 0, 96.0f / 255.0f, 180.0f / 255.0f, 1);
}

void EV_FireShockRifleBeam(event_args_t *args)
{
	pmtrace_t traces[4];
	Vector points[4][2];
	bool priority[4];
	Vector origin, forward, right, up;
	Vector angles = args->angles;
	int selected = -1;
	float closestDistance = 1024.0f;

	EV_GetGunPosition(args, origin, args->origin);
	AngleVectors(angles, forward, right, up);

	for (int i = 0; i < 4; i++)
	{
		priority[i] = false;
		Vector direction = right * ShockRifleBeamOffsets[i].y + forward * ShockRifleBeamOffsets[i].x
			+ up * ShockRifleBeamOffsets[i].z;

		EV_FireAdjustedShockRifleBeam(origin, direction, 1024.0f, args->entindex, &traces[i], points[i]);

		physent_t *entity = gEngfuncs.pEventAPI->EV_GetPhysent(traces[i].ent);

		if (traces[i].allsolid || traces[i].fraction == 1.0f || !entity)
			continue;

		float distance = 1024.0f - (1.0f - traces[i].fraction) * 1024.0f;

		priority[i] = entity->movetype == MOVETYPE_STEP || entity->movetype == MOVETYPE_FLY;

		if (selected >= 0 && priority[selected] && !priority[i])
			continue;

		if ((priority[i] && (selected < 0 || !priority[selected])) || closestDistance > distance)
		{
			closestDistance = distance;
			selected = i;
		}
	}

	if (selected >= 0)
	{
		for (int i = 0; i < 4; i++)
		{
			int hit = priority[i] ? i : selected;

			EV_DrawShockRifleBeam(&traces[hit], points[hit], args->entindex | ((i + 1) << 12));
		}
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			float x = gEngfuncs.pfnRandomFloat(-0.1f, 0.1f) + ShockRifleBeamOffsets[i].x;
			float y = gEngfuncs.pfnRandomFloat(-0.1f, 0.1f) + ShockRifleBeamOffsets[i].y;
			float z = gEngfuncs.pfnRandomFloat(-0.1f, 0.1f) + ShockRifleBeamOffsets[i].z;
			Vector direction = forward * x + right * y + up * z;

			EV_FireAdjustedShockRifleBeam(origin, direction, 1024.0f, args->entindex, &traces[i], points[i]);
			EV_DrawShockRifleBeam(&traces[i], points[i], args->entindex | ((i + 1) << 12));
		}
	}
}
