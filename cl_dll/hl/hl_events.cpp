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
#include "../hud.h"
#include "../cl_util.h"
#include "../ev_sven_events.h"
#include "event_api.h"

void Game_HookEvents( void )
{
	gEngfuncs.pfnHookEvent( "events/glock1.sc",					EV_FireGlock1 );
	gEngfuncs.pfnHookEvent( "events/glock2.sc",					EV_FireGlock2 );
	gEngfuncs.pfnHookEvent( "events/shotgun1.sc",				EV_FireShotGunSingle );
	gEngfuncs.pfnHookEvent( "events/shotgun2.sc",				EV_FireShotGunDouble );
	gEngfuncs.pfnHookEvent( "events/mp5.sc",					EV_FireMP5 );
	gEngfuncs.pfnHookEvent("events/python.sc", EV_FirePython);
	gEngfuncs.pfnHookEvent("events/deagle.sc", EV_FireDeagle);
	gEngfuncs.pfnHookEvent( "events/gauss.sc",					EV_FireGauss );
	gEngfuncs.pfnHookEvent( "events/gaussspin.sc",				EV_SpinGauss );
	gEngfuncs.pfnHookEvent( "events/train.sc",					EV_TrainPitchAdjust );
	gEngfuncs.pfnHookEvent("events/vehicle.sc", EV_VehiclePitchAdjust);
	gEngfuncs.pfnHookEvent("events/uzi.sc", EV_Uzi);
	gEngfuncs.pfnHookEvent("events/uziakimbo.sc", EV_UziAkimbo);
	gEngfuncs.pfnHookEvent("events/clcheck.sc", EV_ClientsideCheckRun);
	gEngfuncs.pfnHookEvent("events/weapon_custom.sc", EV_WeaponCustom);
	gEngfuncs.pfnHookEvent("events/minigun.sc", EV_Minigun);
	gEngfuncs.pfnHookEvent("events/minigun_spin.sc", EV_MinigunSpin);
	gEngfuncs.pfnHookEvent("events/sniperrifle.sc", EV_SniperRifle);
	gEngfuncs.pfnHookEvent("events/m249.sc", EV_M249);
	gEngfuncs.pfnHookEvent("events/m249_LEGACY.sc", EV_M249_LEGACY);
	gEngfuncs.pfnHookEvent("events/m16a2.sc", EV_M16);
	gEngfuncs.pfnHookEvent("events/shockrifle.sc", EV_FireShockRifle);
	gEngfuncs.pfnHookEvent("events/shockriflebeam.sc", EV_FireShockRifleBeam);
	gEngfuncs.pfnHookEvent("events/displacer.sc", EV_DisplacerSpin);
}
