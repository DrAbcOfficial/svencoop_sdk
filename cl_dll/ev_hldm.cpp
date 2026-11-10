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
#include "hud.h"
#include "cl_util.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "pm_defs.h"
#include "eventscripts.h"
#include "ev_hldm.h"
#include "ev_sven_events.h"
#include "r_efx.h"
#include "event_api.h"
#include "event_args.h"
#include "dlight.h"
#include "client_sound.h"
#include "parsemsg.h"

#include <stdio.h>
#include <string.h>

#define SND_STOP (1 << 5)
#define SND_CHANGE_PITCH (1 << 7)

int tracerCount[32];
bool g_fEventRunned;

void V_PunchAxis(int axis, float punch);
extern cvar_t *g_pDeveloper;

void EV_FireGlock1( event_args_t *args )
{
	int idx = args->entindex;
	Vector origin( args->origin), angles( args->angles), velocity( args->velocity);
	Vector forward, right, up, shellVelocity, shellOrigin, source;
	AngleVectors( angles, forward, right, up );

	int shell = gEngfuncs.pEventAPI->EV_FindModelIndex (gHUD.m_bClassicMode ?
		"models/hlclassic/shell.mdl" : "models/shell.mdl");// brass shell

	if ( EV_IsLocal( idx ) )
	{
		if (args->bparam2 != 1)
			EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation(args->bparam1 ? GLOCK_SHOOT_EMPTY : GLOCK_SHOOT, args->bparam2);
	}

	EV_GetDefaultShellInfo( args, origin, velocity, shellVelocity, shellOrigin, forward, right, up, 18, -6, 8);

	EV_EjectBrass (shellOrigin, shellVelocity, angles.y, shell, TE_BOUNCE_SHELL );

	const char *sample;
	float volume = 1;
	int pitch;

	if (gHUD.m_bClassicMode)
	{
		sample = "hlclassic/weapons/pl_gun3.wav";
		pitch = gEngfuncs.pfnRandomLong( 0, 3 ) + 98;
	}
	else if (args->bparam2 != 1)
	{
		sample = "weapons/pl_gun3.wav";
		pitch = gEngfuncs.pfnRandomLong(0, 3) + 98;
	}
	else
	{
		if ( gEngfuncs.pfnRandomLong(0, 1) )
			sample = "weapons/pl_gun2.wav";
		else
			sample = "weapons/pl_gun1.wav";

		pitch = gEngfuncs.pfnRandomLong(0, 3) + 98;
		volume = gEngfuncs.pfnRandomFloat(0.42, 0.5);
	}

	CClient_SoundEngine::GetSoundEngine()->PlayFMODSound(0, idx, origin,
		CHAN_AUTO, sample, volume, ATTN_NORM, 0, pitch, -1, 0);

	EV_GetGunPosition( args, source, origin );

	Vector spread(0.01, 0.01, 0.01);

	EV_StandardFireBullets( idx, forward, right, up, 1, source, forward, spread, 8192, BULLET_PLAYER_9MM, 0);

	if (EV_IsLocal(idx))
		V_PunchAxis(0, -2);

	if (args->bparam2 != 1)
		CreateMuzzleFlash(origin - up * 18 + forward * 60);
}

void EV_FireGlock2( event_args_t *args )
{
	int idx = args->entindex;
	Vector origin( args->origin), angles( args->angles), velocity( args->velocity);
	Vector forward, right, up, shellVelocity, shellOrigin, source;

	AngleVectors( angles, forward, right, up );

	int shell = gEngfuncs.pEventAPI->EV_FindModelIndex (gHUD.m_bClassicMode ?
		"models/hlclassic/shell.mdl" : "models/shell.mdl");// brass shell

	if ( EV_IsLocal( idx ) )
	{
		// Add muzzle flash to current weapon model
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation(args->bparam1 ? GLOCK_SHOOT_EMPTY : GLOCK_SHOOT, 2);
	}

	EV_GetDefaultShellInfo( args, origin, velocity, shellVelocity, shellOrigin, forward, right, up, 18, -6, 8);

	EV_EjectBrass (shellOrigin, shellVelocity, angles.y, shell, TE_BOUNCE_SHELL );

	const char *sample;

	if ( gHUD.m_bClassicMode )
		sample = "hlclassic/weapons/pl_gun3.wav";
	else
		sample = "weapons/pl_gun3.wav";

	int pitch = gEngfuncs.pfnRandomLong( 0, 3 ) + 98;

	CClient_SoundEngine::GetSoundEngine()->PlayFMODSound(0, idx, origin,
		CHAN_AUTO, sample, 1, ATTN_NORM, 0, pitch, -1, 0);

	EV_GetGunPosition( args, source, origin );

	Vector spread(0.1, 0.1, 0.1);

	EV_StandardFireBullets( idx, forward, right, up, 1, source, forward, spread, 8192, BULLET_PLAYER_9MM, 0);

	if (EV_IsLocal(idx))
		V_PunchAxis(0, -2);

	CreateMuzzleFlash(origin - up * 18 + forward * 60);

}

void EV_FireShotGunSingle( event_args_t *args )
{
	int idx = args->entindex;
	Vector origin( args->origin), angles( args->angles);
	Vector forward, right, up, source;

	AngleVectors( angles, forward, right, up);// brass shell

	if ( EV_IsLocal( idx ) )
	{
		// Add muzzle flash to current weapon model
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation( SHOTGUN_FIRE, 2 );
	}

	const char *sample;

	if ( gHUD.m_bClassicMode )
		sample = "hlclassic/weapons/sbarrel1.wav";
	else
		sample = "weapons/sbarrel1.wav";

	int pitch = gEngfuncs.pfnRandomLong( 0, 31) + 93;

	float volume;

	if ( gHUD.m_bClassicMode )
		volume = gEngfuncs.pfnRandomFloat(0.95, 1);
	else
		volume = 1;

	CClient_SoundEngine::GetSoundEngine()->PlayFMODSound(0, idx, args->origin,
		CHAN_AUTO, sample, volume, ATTN_NORM, 0, pitch, -1, 0);

	EV_GetGunPosition( args, source, origin );

	Vector spread;

	if ( gEngfuncs.GetMaxClients() > 1 )
		spread = Vector(0.08716, 0.04362, 0.0);
	else
		spread = VECTOR_CONE_10DEGREES;

	EV_HLDM_FireBullets( idx, forward, right, up, 8, source, forward, spread, 2048, BULLET_PLAYER_BUCKSHOT, 0, &tracerCount[idx-1]);

	if (EV_IsLocal(idx))
		V_PunchAxis(0, -5);

	CreateMuzzleFlash(origin - up * 18 + forward * 60);
	}

void EV_FireShotGunDouble( event_args_t *args )
{
	int idx = args->entindex;
	Vector origin( args->origin), angles( args->angles), velocity( args->velocity);
	Vector forward, right, up, shellVelocity, shellOrigin, source;

	AngleVectors( angles, forward, right, up );

	int shell = gEngfuncs.pEventAPI->EV_FindModelIndex (gHUD.m_bClassicMode ?
		"models/hlclassic/shotgunshell.mdl" : "models/shotgunshell.mdl");// brass shell

	if ( EV_IsLocal( idx ) )
	{
		// Add muzzle flash to current weapon model
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation(args->bparam1 == 1 ? SHOTGUN_FIRE_DOUBLE : SHOTGUN_FIRE2, 2 );
	}

	EV_GetDefaultShellInfo( args, origin, velocity, shellVelocity, shellOrigin, forward, right, up, 16, -25, 6 );

		EV_EjectBrass (shellOrigin, shellVelocity, angles.y, shell, TE_BOUNCE_SHOTSHELL );

	const char *sample;
	float volume = 1;
	int pitch;

	if (gHUD.m_bClassicMode)
	{
		sample = "hlclassic/weapons/dbarrel1.wav";
		pitch = gEngfuncs.pfnRandomLong(0, 31) + 85;
		volume = gEngfuncs.pfnRandomFloat(0.98, 1);
	}
	else if (args->bparam1 == 1)
	{
		sample = "weapons/dbarrel2.wav";
		pitch = gEngfuncs.pfnRandomLong( 0, 31) + 83;
	}
	else
	{
		sample = "weapons/sbarrel1.wav";
		pitch = gEngfuncs.pfnRandomLong(0, 31) + 93;
	}

	CClient_SoundEngine::GetSoundEngine()->PlayFMODSound(0, idx, args->origin,
		CHAN_AUTO, sample, volume, ATTN_NORM, 0, pitch, -1, 0);

	EV_GetGunPosition( args, source, origin );

	Vector spread = VECTOR_CONE_10DEGREES;
	int shots = 8;

	if ( gEngfuncs.GetMaxClients() > 1 )
	{
		spread = Vector(0.17365, 0.04362, 0.0);

		if ( args->bparam1 == 1 )
			shots = 12;
		else
			shots = 6;
	}

	EV_HLDM_FireBullets( idx, forward, right, up, shots, source, forward, spread, 2048, BULLET_PLAYER_BUCKSHOT, 0, &tracerCount[idx-1]);

	if (EV_IsLocal(idx))
		V_PunchAxis(0, -10);

	Vector muzzle = origin - up * 18 + forward * 60;
	dlight_t *light = gEngfuncs.pEfxAPI->CL_AllocDlight(0);

	VectorCopy(muzzle, light->origin);
	light->color.r = 255;
	light->color.g = 200;
	light->color.b = 80;
	light->radius = 64;
	light->decay = 0;
	light->die = gEngfuncs.GetClientTime() + 0.1;
	light = gEngfuncs.pEfxAPI->CL_AllocElight(0);
	VectorCopy(muzzle, light->origin);
	light->color.r = 255;
	light->color.g = 200;
	light->color.b = 80;
	light->radius = 32;
	light->decay = 0;
	light->minlight = 100;
	light->die = gEngfuncs.GetClientTime() + 0.1;
}

void EV_FireMP5( event_args_t *args )
{
	int idx = args->entindex;
	Vector origin( args->origin), angles( args->angles), velocity( args->velocity);
	Vector forward, right, up, shellVelocity, shellOrigin, source;

	AngleVectors( angles, forward, right, up );

	int shell = gEngfuncs.pEventAPI->EV_FindModelIndex (gHUD.m_bClassicMode ?
		"models/hlclassic/shell.mdl" : "models/shell.mdl");// brass shell

	if ( EV_IsLocal( idx ) )
	{
		// Add muzzle flash to current weapon model
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation( MP5_FIRE1 + gEngfuncs.pfnRandomLong(0,2), 2 );
	}

	EV_GetDefaultShellInfo(args, origin, velocity, shellVelocity, shellOrigin, forward, right, up, 20, -16, 10);
	EV_EjectBrass(shellOrigin, shellVelocity, angles.y, shell, TE_BOUNCE_SHELL);

	static const char *sounds[2][3] =
	{
		{ "weapons/hks1.wav", "weapons/hks2.wav", "weapons/hks3.wav" },
		{ "hlclassic/weapons/hks1.wav", "hlclassic/weapons/hks2.wav", "hlclassic/weapons/hks3.wav" }
	};
	const char *sample = sounds[gHUD.m_bClassicMode][gEngfuncs.pfnRandomLong(0, 2)];

	CClient_SoundEngine::GetSoundEngine()->PlayFMODSound(0, idx, args->origin,
		CHAN_AUTO, sample, 1, ATTN_NORM, 0, PITCH_NORM, -1, 0);
	EV_GetGunPosition(args, source, origin);

	Vector spread;

	if ( args->bparam1 == 1 )
		spread = BulletAccuracy(VECTOR_CONE_8DEGREES, VECTOR_CONE_4DEGREES, VECTOR_CONE_3DEGREES, args);
	else
		spread = BulletAccuracy(VECTOR_CONE_8DEGREES, VECTOR_CONE_6DEGREES, VECTOR_CONE_5DEGREES, args);

	EV_StandardFireBullets(idx, forward, right, up, 1, source, forward, spread, 8192, BULLET_PLAYER_MP5, 2);

	if (EV_IsLocal(idx))
		V_PunchAxis( 0, args->bparam1 == 1 ? gEngfuncs.pfnRandomFloat(-1, 1) : gEngfuncs.pfnRandomFloat( -2, 2 ) );

	CreateMuzzleFlash(origin - up * 18 + forward * 60);
	}

void EV_FirePython( event_args_t *args )
{
	int idx = args->entindex;
	Vector origin( args->origin), angles( args->angles);
	Vector forward, right, up, source;

	AngleVectors( angles, forward, right, up );

	if ( EV_IsLocal( idx ) )
	{
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation( PYTHON_FIRE1, 0 );
	}

	const char *sample;
	int pitch = PITCH_NORM;

	if (gHUD.m_bClassicMode)
		sample = gEngfuncs.pfnRandomLong( 0, 1 ) ? "hlclassic/weapons/357_shot2.wav" : "hlclassic/weapons/357_shot1.wav";
	else
	{
		sample = "weapons/357_shot1.wav";
		pitch = gEngfuncs.pfnRandomLong(0, 3) + 98;
	}

	float volume = gEngfuncs.pfnRandomFloat(0.8, 0.9);

	CClient_SoundEngine::GetSoundEngine()->PlayFMODSound(0, idx, origin, CHAN_WEAPON, sample, volume, ATTN_NORM, 0, pitch, -1, 0);
	EV_GetGunPosition( args, source, origin );

	Vector spread = BulletAccuracy(VECTOR_CONE_4DEGREES, VECTOR_CONE_3DEGREES, VECTOR_CONE_2DEGREES, args);

	EV_StandardFireBullets( idx, forward, right, up, 1, source, forward, spread, 8192, BULLET_PLAYER_357, 0);

	if (EV_IsLocal(idx))
		V_PunchAxis(0, -12);

	CreateMuzzleFlash(origin - up * 18 + forward * 60);
}

void EV_SpinGauss( event_args_t *args )
{
	CClient_SoundEngine::GetSoundEngine()->PlayFMODSound(args->bparam1 ? SOUNDMSG_CHANGE_PITCH : 0,
		args->entindex, args->origin, CHAN_WEAPON, "ambience/pulsemachine.wav",
		1, ATTN_NORM, 0, args->iparam1, -1, 0);
}

void EV_TrainPitchAdjust( event_args_t *args )
{
	unsigned short us_params = (unsigned short)args->iparam1;
	int sound = (us_params >> 12) & 0x0007;
	static const char *sounds[] =
	{
		"plats/ttrain1.wav", "plats/ttrain2.wav", "plats/ttrain3.wav",
		"plats/ttrain4.wav", "plats/ttrain6.wav", "plats/ttrain7.wav"
	};

	if (sound < 1 || sound > (int)ARRAYSIZE(sounds))
		return;

	float m_flVolume = (us_params & 0x003f)/40.0;
	int pitch = ( ( us_params >> 6 ) & 0x003f ) * 10;

	CClient_SoundEngine::GetSoundEngine()->PlayFMODSound(0, args->entindex, args->origin, CHAN_STATIC, sounds[sound - 1], m_flVolume, ATTN_NORM,
		args->bparam1 ? SND_STOP : SND_CHANGE_PITCH, pitch, -1, 0);
	}

void EV_VehiclePitchAdjust(event_args_t *args)
{
	unsigned short packed = (unsigned short)args->iparam1;
	int sound = (packed >> 12) & 0x0007;
	static const char *sounds[] =
	{
		"plats/vehicle1.wav", "plats/vehicle2.wav", "plats/vehicle3.wav",
		"plats/vehicle4.wav", "plats/vehicle6.wav", "plats/vehicle7.wav"
	};

	if (sound < 1 || sound > (int)ARRAYSIZE(sounds))
		return;

	if (args->bparam1)
		gEngfuncs.pEventAPI->EV_StopSound(args->entindex, CHAN_STATIC, sounds[sound - 1]);
	else
	{
		float volume = (packed & 0x003f) / 40.0;
		int pitch = ((packed >> 6) & 0x003f) * 10;

		gEngfuncs.pEventAPI->EV_PlaySound(args->entindex, args->origin,
			CHAN_STATIC, sounds[sound - 1], volume, ATTN_NORM, SND_CHANGE_PITCH, pitch);
	}
}

void EV_ClientsideCheckRun(event_args_t *args)
{
	g_fEventRunned = true;
}

void EV_WeaponCustom(event_args_t *args)
{
	if (args->iparam1 == 1)
	{
		int pitch = gEngfuncs.pfnRandomLong(0, 15) + 94;

		CClient_SoundEngine::GetSoundEngine()->PlayFMODSound(0, args->entindex, args->origin,
			args->iparam2, "weapons/357_cock1.wav", 1, ATTN_NORM, 0, pitch, -1, 0);
		return;
	}

	if (args->iparam1 != 2)
	{
		if (g_pDeveloper->value >= 1)
			gEngfuncs.Con_Printf("Unknown custom weapon event %d\n", args->iparam1);

		return;
	}

	static const char *sounds[] =
	{
		"weapons/357_cock1.wav", "weapons/rocketfire1.wav", "weapons/glauncher.wav",
		"weapons/glauncher2.wav", "weapons/357_reload1.wav", "weapons/xbow_fire1.wav",
		"weapons/xbow_reload1.wav", "common/null.wav", "weapons/electro4.wav",
		"weapons/electro5.wav", "weapons/electro6.wav", "agrunt/ag_fire1.wav",
		"agrunt/ag_fire2.wav", "agrunt/ag_fire3.wav", "weapons/dbarrel1.wav",
		"weapons/sbarrel1.wav", "weapons/reload1.wav", "weapons/reload3.wav",
		"misc/bbcover.wav", "weapons/scock1.wav", "weapons/sniper_zoom.wav",
		"weapons/desert_eagle_sight.wav", "weapons/desert_eagle_sight2.wav",
		"weapons/splauncher_fire.wav", "weapons/splauncher_altfire.wav"
	};
	int sound = (int)args->fparam1;

	if (sound < 1 || sound >= (int)ARRAYSIZE(sounds))
		sound = 0;

	const char *sample = sounds[sound];
	bool classic = sound <= 6 || (sound >= 8 && sound <= 10) ||
		(sound >= 14 && sound <= 17) || sound == 19;
	char buffer[260];

	if (sound == 19)
	{
		Vector origin(args->origin), angles(args->angles), velocity(args->velocity);
		Vector forward, right, up, shellVelocity, shellOrigin;

		AngleVectors(angles, forward, right, up);

		if (!args->bparam2)
		{
			int shell = gEngfuncs.pEventAPI->EV_FindModelIndex(gHUD.m_bClassicMode ?
				"models/hlclassic/shotgunshell.mdl" : "models/shotgunshell.mdl");

			EV_GetDefaultShellInfo(args, origin, velocity, shellVelocity, shellOrigin, forward, right, up, 16, -25, 6);
			EV_EjectBrass(shellOrigin, shellVelocity, angles.y, shell, TE_BOUNCE_SHOTSHELL);
		}
	}

	if (gHUD.m_bClassicMode && classic)
	{
		snprintf(buffer, sizeof(buffer), "hlclassic/%s", sample);
		sample = buffer;
	}

	int pitch = gEngfuncs.pfnRandomLong(0, 15) + 94;

	CClient_SoundEngine::GetSoundEngine()->PlayFMODSound(0, args->entindex, args->origin,
		args->iparam2, sample, args->fparam2, ATTN_NORM, 0, pitch, -1, 0);
}

void EV_FireShockRifle(event_args_t *args)
{
	if (EV_IsLocal(args->entindex))
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation(SHOCKRIFLE_FIRE, 0);

		for (int attachment = 0x2000; attachment < 0x5000; attachment += 0x1000)
		{
			int model = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/lgtning.spr");

			gEngfuncs.pEfxAPI->R_BeamEnts(args->entindex | attachment, args->entindex | 0x1000,
				model, 0.1, 1, 0.5, 1, 0, 0, 0, 96.0f / 255, 180.0f / 255, 1);
		}
	}

	dlight_t *light = gEngfuncs.pEfxAPI->CL_AllocElight(0);

	VectorCopy(args->origin, light->origin);
	light->color.r = 85;
	light->color.g = 200;
	light->color.b = 255;
	light->radius = 24;
	light->decay = 24;
	light->die = gEngfuncs.GetClientTime() + 0.2;
	CClient_SoundEngine::GetSoundEngine()->PlayFMODSound(0, args->entindex, args->origin,
		CHAN_WEAPON, "weapons/shock_fire.wav", 1, ATTN_NORM, 0, PITCH_NORM, -1, 0);
}

void EV_DisplacerSpin(event_args_t *args)
{
	if (!EV_IsLocal(args->entindex))
		return;

	for (int attachment = 0x2000; attachment < 0x4000; attachment += 0x1000)
	{
		int model = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/lgtning.spr");

		gEngfuncs.pEfxAPI->R_BeamEnts(args->entindex | attachment, args->entindex | (attachment + 0x1000),
			model, args->fparam1, 1, 0.5, 1, 0, 0, 0, 96.0f / 255, 128.0f / 255, 16.0f / 255);
	}
}

int __MsgFunc_Flamethwr(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int iEntity = READ_BYTE();
	Vector vecOrigin, vecVelocity;

	vecOrigin.x = READ_COORD();
	vecOrigin.y = READ_COORD();
	vecOrigin.z = READ_COORD();
	vecVelocity.x = READ_COORD();
	vecVelocity.y = READ_COORD();
	vecVelocity.z = READ_COORD();

	if (EV_IsLocal(iEntity))
		gEngfuncs.pEventAPI->EV_WeaponAnimation(FLAMETHROWER_FIRE1, 0);

	CClient_SoundEngine::GetSoundEngine()->PlayFMODSound(0, iEntity, vecOrigin, CHAN_WEAPON,
		"tfc/weapons/flmfire2.wav", 1, ATTN_NORM, 0, PITCH_NORM, -1, 0);

	TEMPENTITY *pFlame = gEngfuncs.pEfxAPI->R_TempSprite(vecOrigin, vecVelocity, 1,
		gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/flamethrower/fthrow.spr"), kRenderNormal,
		kRenderFxNone, 1, 0, 0);

	if (pFlame)
	{
		pFlame->flags |= FTENT_SPRANIMATE;
		pFlame->entity.curstate.framerate = 20;
	}

	return 1;
}
