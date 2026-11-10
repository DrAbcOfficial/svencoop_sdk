#include "hud.h"
#include "cl_util.h"
#include "cl_entity.h"
#include "pm_defs.h"
#include "event_api.h"
#include "event_args.h"
#include "in_defs.h"
#include "eventscripts.h"
#include "ev_hldm.h"
#include "ev_sven_events.h"
#include "r_efx.h"
#include "dlight.h"
#include "client_sound.h"

void V_PunchAxis(int axis, float punch);

Vector BulletAccuracy(Vector moving, Vector standing, Vector ducked, event_args_t *args)
{
	if (args->velocity[0] != 0 || args->velocity[1] != 0 || args->velocity[2] != 0)
		return moving;

	return args->ducking == 1 ? ducked : standing;
}

void CreateMuzzleFlash(Vector origin)
{
	dlight_t *light = gEngfuncs.pEfxAPI->CL_AllocDlight(0);

	VectorCopy(origin, light->origin);
	light->color.r = 255;
	light->color.g = 200;
	light->color.b = 180;
	light->radius = 96;
	light->decay = 0;
	light->die = gEngfuncs.GetClientTime() + 0.1;
}

void PlayWeaponSound(int index, float *origin, int channel, const char *sample,
	float volume, float attenuation, int pitch)
{
	CClient_SoundEngine::GetSoundEngine()->PlayFMODSound(0, index, origin, channel,
		sample, volume, attenuation, 0, pitch, -1, 0);
}

void EV_Uzi(event_args_t *args)
{
	int index = args->entindex;
	Vector origin(args->origin), angles(args->angles), velocity(args->velocity);
	Vector forward, right, up, source, shellOrigin, shellVelocity;

	AngleVectors(angles, forward, right, up);

	int shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/shell.mdl");

	if (EV_IsLocal(index))
	{
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation(UZI_SHOOT, 0);
	}

	EV_GetDefaultShellInfo(args, origin, velocity, shellVelocity, shellOrigin,
		forward, right, up, 18, -12, 7);
	EV_EjectBrass(shellOrigin, shellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL);

	const char *samples[] = { "weapons/uzi/shoot1.wav", "weapons/uzi/shoot2.wav", "weapons/uzi/shoot3.wav" };
	const char *sample = samples[gEngfuncs.pfnRandomLong(0, 2)];
	int pitch = 94 + gEngfuncs.pfnRandomLong(0, 15);

	PlayWeaponSound(index, origin, CHAN_WEAPON, sample, 1, ATTN_NORM, pitch);
	EV_GetGunPosition(args, source, origin);

	Vector spread = BulletAccuracy(VECTOR_CONE_6DEGREES, VECTOR_CONE_4DEGREES, VECTOR_CONE_3DEGREES, args);

	EV_HLDM_FireBullets(index, forward, right, up, 1, source, forward, spread,
		8192, BULLET_PLAYER_MP5, 0, &tracerCount[index - 1]);

	if (EV_IsLocal(index))
		V_PunchAxis(PITCH, gEngfuncs.pfnRandomFloat(-2, 2));

	CreateMuzzleFlash(origin + Vector(0, 0, 0) - up * 18 + forward * 60);
}

void EV_UziAkimbo(event_args_t *args)
{
	int index = args->entindex;
	Vector origin(args->origin), angles(args->angles), velocity(args->velocity);
	Vector forward, right, up, source, shellOrigin, shellVelocity;
	Vector viewOffset(0, 0, 0);

	AngleVectors(angles, forward, right, up);

	int shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/shell.mdl");

	if (EV_IsLocal(index))
	{
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation(args->iparam1, 2);
	}

	EV_GetGunPosition(args, source, origin);

	if (args->iparam1 >= UZI_AKIMBO_SHOOT_BOTH)
	{
		const char *sample = gEngfuncs.pfnRandomLong(0, 1) ?
			"weapons/uzi/fire_both2.wav" : "weapons/uzi/fire_both1.wav";
		int pitch = 94 + gEngfuncs.pfnRandomLong(0, 15);

		PlayWeaponSound(index, origin, CHAN_WEAPON, sample, 1, ATTN_NORM, pitch);

		Vector spread = VECTOR_CONE_9DEGREES;

		EV_HLDM_FireBullets(index, forward, right, up, 2, source, forward, spread,
			8192, BULLET_PLAYER_MP5, 0, &tracerCount[index - 1]);
		EV_GetDefaultShellInfo(args, origin, velocity, shellVelocity, shellOrigin,
			forward, right, up, 18, -12, 7);
		EV_EjectBrass(shellOrigin, shellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL);

		if (EV_IsLocal(index))
			gEngfuncs.pEventAPI->EV_LocalPlayerViewheight(viewOffset);
		else if (args->ducking == 1)
			viewOffset.z = 12;

		float rightSpeed = gEngfuncs.pfnRandomFloat(30, 50);
		float upSpeed = gEngfuncs.pfnRandomFloat(90, 120);

		shellVelocity = velocity + right * rightSpeed + up * upSpeed + forward * 5;
		shellOrigin = origin + viewOffset - up * 5 + forward * 18 - right * 8;
		EV_EjectBrass(shellOrigin, shellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL);
	}
	else if (args->iparam1 == UZI_AKIMBO_SHOOT_RIGHT)
	{
		Vector spread = VECTOR_CONE_8DEGREES;

		EV_HLDM_FireBullets(index, forward, right, up, 1, source, forward, spread,
			8192, BULLET_PLAYER_MP5, 0, &tracerCount[index - 1]);

		const char *sample = gEngfuncs.pfnRandomLong(0, 1) ?
			"weapons/uzi/shoot1.wav" : "weapons/uzi/shoot2.wav";
		int pitch = 94 + gEngfuncs.pfnRandomLong(0, 15);

		PlayWeaponSound(index, origin, CHAN_WEAPON, sample, 1, ATTN_NORM, pitch);
		EV_GetDefaultShellInfo(args, origin, velocity, shellVelocity, shellOrigin,
			forward, right, up, 18, -12, 7);
		EV_EjectBrass(shellOrigin, shellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL);
	}
	else
	{
		const char *sample = gEngfuncs.pfnRandomLong(0, 1) ?
			"weapons/uzi/shoot2.wav" : "weapons/uzi/shoot3.wav";
		int pitch = 94 + gEngfuncs.pfnRandomLong(0, 15);

		PlayWeaponSound(index, origin, CHAN_WEAPON, sample, 1, ATTN_NORM, pitch);

		Vector spread = BulletAccuracy(VECTOR_CONE_10DEGREES, VECTOR_CONE_8DEGREES, VECTOR_CONE_7DEGREES, args);

		EV_HLDM_FireBullets(index, forward, right, up, 1, source, forward, spread,
			8192, BULLET_PLAYER_MP5, 2, &tracerCount[index - 1]);
		EV_GetDefaultShellInfo(args, origin, velocity, shellVelocity, shellOrigin,
			forward, right, up, 18, -7, -8);
		EV_EjectBrass(shellOrigin, shellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL);
	}

	if (EV_IsLocal(index))
		V_PunchAxis(PITCH, gEngfuncs.pfnRandomFloat(-2, 2));

	CreateMuzzleFlash(origin + viewOffset + forward * 48);
}

void EV_Minigun(event_args_t *args)
{
	int index = args->entindex;
	Vector origin(args->origin), angles(args->angles), velocity(args->velocity);
	Vector forward, right, up, source, shellOrigin, shellVelocity;
	Vector viewOffset(0, 0, 28);

	AngleVectors(angles, forward, right, up);

	if (EV_IsPlayer(index))
	{
		if (EV_IsLocal(index))
			gEngfuncs.pEventAPI->EV_LocalPlayerViewheight(viewOffset);
		else if (args->ducking == 1)
			viewOffset.z = 12;
	}

	EV_GetGunPosition(args, source, origin);
	source.z -= 6;

	float rightSpeed = gEngfuncs.pfnRandomFloat(50, 70);
	float upSpeed = gEngfuncs.pfnRandomFloat(80, 120);

	shellVelocity = velocity + right * rightSpeed + up * upSpeed + forward * 25;
	shellOrigin = origin + viewOffset - up * 18 + forward * 20 + right * 8;

	int shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/shell.mdl");

	EV_EjectBrass(shellOrigin, shellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL);

	int pitch = 98 + gEngfuncs.pfnRandomLong(0, 3);
	float volume = gEngfuncs.pfnRandomFloat(0.92f, 1);

	PlayWeaponSound(index, origin, CHAN_ITEM, "minigun/hw_shoot1.wav", volume, 0.6f, pitch);

	Vector spread = BulletAccuracy(VECTOR_CONE_10DEGREES, VECTOR_CONE_7DEGREES, VECTOR_CONE_5DEGREES, args);
	int count = 2;

	EV_HLDM_FireBullets(index, forward, right, up, 1, source, forward, spread,
		2048, BULLET_MONSTER_MP5, 1, &count);

	if (EV_IsLocal(index))
	{
		Vector viewAngles(0, 0, 0);

		gEngfuncs.GetViewAngles(viewAngles);

		if (args->ducking)
		{
			viewAngles[PITCH] += gEngfuncs.pfnRandomFloat(-0.5f, 0.5f);
			viewAngles[YAW] += gEngfuncs.pfnRandomFloat(-0.25f, 0.25f);
		}
		else
		{
			viewAngles[PITCH] += gEngfuncs.pfnRandomFloat(-1.5f, 1.5f);
			viewAngles[YAW] += gEngfuncs.pfnRandomFloat(-1, 1);
		}

		gEngfuncs.SetViewAngles(viewAngles);
		V_PunchAxis(PITCH, gEngfuncs.pfnRandomFloat(-1, 1));
	}

	Vector muzzleOrigin = origin + viewOffset - up * 18 + forward * 60;
	dlight_t *light = gEngfuncs.pEfxAPI->CL_AllocDlight(0);

	VectorCopy(muzzleOrigin, light->origin);
	light->color.r = 255;
	light->color.g = 200;
	light->color.b = 80;
	light->radius = 64;
	light->decay = 0;
	light->die = gEngfuncs.GetClientTime() + 0.2;
}

void EV_MinigunSpin(event_args_t *args)
{
	static float nextSpinTime = 0;
	int index = args->entindex;
	Vector origin(args->origin);

	gEngfuncs.pEventAPI->EV_KillEvents(index, "events/minigun.sc");

	const char *sample;

	switch (args->iparam1)
	{
	case 0:
		if (!(gEngfuncs.GetClientTime() >= nextSpinTime))
			return;

		sample = "hassault/hw_spin.wav";
		break;

	case 1:
		sample = "hassault/hw_spinup.wav";
		break;

	case 2:
		sample = "hassault/hw_spindown.wav";
		break;

	default:
		return;
	}

	int pitch = 98 + gEngfuncs.pfnRandomLong(0, 3);
	float volume = gEngfuncs.pfnRandomFloat(0.92f, 1);

	PlayWeaponSound(index, origin, CHAN_ITEM, sample, volume, 0.6f, pitch);

	if (!args->iparam1)
	{
		nextSpinTime = gEngfuncs.GetClientTime() + 0.4;
	}
	else if (EV_IsLocal(index))
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation(args->iparam1 == 1 ? MINIGUN_SPINUP : MINIGUN_SPINDOWN, 0);
	}
}

void EV_SniperRifle(event_args_t *args)
{
	int index = args->entindex;
	Vector origin(args->origin), angles(args->angles);
	Vector forward, right, up, source;

	AngleVectors(angles, forward, right, up);

	if (EV_IsLocal(index))
	{
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation(args->iparam1 == 1 ? SNIPERRIFLE_FIRELASTROUND : SNIPERRIFLE_FIRE, 0);
	}

	int pitch = 98 + gEngfuncs.pfnRandomLong(0, 3);

	PlayWeaponSound(index, args->origin, CHAN_WEAPON, "weapons/sniper_fire.wav", 1, ATTN_NORM, pitch);
	EV_GetGunPosition(args, source, origin);

	Vector spread;

	if (args->bparam2 == 1)
	{
		spread = BulletAccuracy(VECTOR_CONE_1DEGREES,
			Vector(0, 0, 0), Vector(0, 0, 0), args);
	}
	else
	{
		spread = BulletAccuracy(VECTOR_CONE_9DEGREES, VECTOR_CONE_6DEGREES, VECTOR_CONE_3DEGREES, args);
	}

	EV_HLDM_FireBullets(index, forward, right, up, 1, source, forward, spread,
		8192, BULLET_MONSTER_SNIPER, 0, &tracerCount[index - 1]);

	if (EV_IsLocal(index))
	{
		if (args->bparam2 == 1)
			V_PunchAxis(PITCH, gEngfuncs.pfnRandomFloat(-2, 2));
		else
			V_PunchAxis(PITCH, -10);
	}
}

void FireM249(event_args_t *args, bool legacy)
{
	int index = args->entindex;
	Vector origin(args->origin), angles(args->angles), velocity(args->velocity);
	Vector forward, right, up, source, shellOrigin, shellVelocity;

	AngleVectors(angles, forward, right, up);

	int shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/saw_shell.mdl");

	if (EV_IsLocal(index))
	{
		EV_MuzzleFlash();

		int body = 0;

		if (legacy)
		{
			if (args->iparam1 == 0 || args->iparam1 == 1)
				body = 8;
			else if (args->iparam1 >= 2 && args->iparam1 <= 8)
				body = 9 - args->iparam1;
		}
		else if ((unsigned int)args->iparam1 <= 15)
		{
			body = 15 - args->iparam1;
		}

		gEngfuncs.pEventAPI->EV_WeaponAnimation(M249_SHOOT1 + gEngfuncs.pfnRandomLong(0, 2), body);
	}

	EV_GetDefaultShellInfo(args, origin, velocity, shellVelocity, shellOrigin,
		forward, right, up, 24, -18, 12);
	EV_EjectBrass(shellOrigin, shellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL);

	int pitch = 94 + gEngfuncs.pfnRandomLong(0, 15);

	PlayWeaponSound(index, origin, CHAN_WEAPON, "weapons/saw_fire1.wav", 1, ATTN_NORM, pitch);
	EV_GetGunPosition(args, source, origin);

	Vector spread = BulletAccuracy(VECTOR_CONE_7DEGREES, VECTOR_CONE_4DEGREES, VECTOR_CONE_3DEGREES, args);

	EV_HLDM_FireBullets(index, forward, right, up, 1, source, forward, spread,
		8192, BULLET_PLAYER_SAW, 1, &tracerCount[index - 1]);
	EV_IsLocal(index);
	CreateMuzzleFlash(origin + Vector(0, 0, 0) - up * 18 + forward * 60);
}

void EV_M249(event_args_t *args)
{
	FireM249(args, false);
}

void EV_M249_LEGACY(event_args_t *args)
{
	FireM249(args, true);
}

void EV_M16(event_args_t *args)
{
	int index = args->entindex;
	Vector origin(args->origin), angles(args->angles), velocity(args->velocity);
	Vector forward, right, up, source, shellOrigin, shellVelocity;

	AngleVectors(angles, forward, right, up);

	int shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/shell.mdl");

	if (EV_IsLocal(index))
	{
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation(M16_SHOOT_1 + gEngfuncs.pfnRandomLong(0, 1), 0);
	}

	EV_GetDefaultShellInfo(args, origin, velocity, shellVelocity, shellOrigin,
		forward, right, up, 18, -16, 10);
	EV_EjectBrass(shellOrigin, shellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL);

	if (args->bparam1)
	{
		int pitch = 103 + gEngfuncs.pfnRandomLong(0, 5);

		PlayWeaponSound(index, args->origin, CHAN_AUTO, "weapons/m16_3round.wav", 1, ATTN_NORM, pitch);
	}

	EV_GetGunPosition(args, source, origin);

	Vector spread = BulletAccuracy(VECTOR_CONE_6DEGREES, VECTOR_CONE_4DEGREES, VECTOR_CONE_3DEGREES, args);

	EV_HLDM_FireBullets(index, forward, right, up, 1, source, forward, spread,
		8192, BULLET_PLAYER_MP5, 2, &tracerCount[index - 1]);

	if (EV_IsLocal(index))
		V_PunchAxis(PITCH, gEngfuncs.pfnRandomFloat(-2, 2));

	CreateMuzzleFlash(origin + Vector(0, 0, 0) - up * 18 + forward * 60);
}

void EV_FireDeagle(event_args_t *args)
{
	int index = args->entindex;
	Vector origin(args->origin), angles(args->angles), velocity(args->velocity);
	Vector forward, right, up, source, shellOrigin, shellVelocity;

	AngleVectors(angles, forward, right, up);

	if (EV_IsLocal(index))
	{
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation(args->bparam1 ? EAGLE_SHOOT_EMPTY : EAGLE_SHOOT, 2);
	}

	int shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/shell.mdl");

	EV_GetDefaultShellInfo(args, origin, velocity, shellVelocity, shellOrigin,
		forward, right, up, 18, -6, 8);
	EV_EjectBrass(shellOrigin, shellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL);

	int pitch = 98 + gEngfuncs.pfnRandomLong(0, 3);
	float volume = gEngfuncs.pfnRandomFloat(0.8f, 0.9f);

	PlayWeaponSound(index, origin, CHAN_WEAPON, "weapons/de_shot1.wav", volume, ATTN_NORM, pitch);
	EV_GetGunPosition(args, source, origin);

	Vector spread;

	if (args->bparam2 == 1)
	{
		spread = BulletAccuracy(VECTOR_CONE_5DEGREES, VECTOR_CONE_4DEGREES, VECTOR_CONE_3DEGREES, args);
	}
	else
	{
		spread = BulletAccuracy(VECTOR_CONE_3DEGREES, VECTOR_CONE_2DEGREES, VECTOR_CONE_1DEGREES, args);
	}

	EV_HLDM_FireBullets(index, forward, right, up, 1, source, forward, spread,
		8192, BULLET_PLAYER_EAGLE, 0, &tracerCount[index - 1]);

	if (EV_IsLocal(index))
		V_PunchAxis(PITCH, args->bparam2 == 1 ? -10 : -6);

	CreateMuzzleFlash(origin + Vector(0, 0, 0) - up * 18 + forward * 60);
}
