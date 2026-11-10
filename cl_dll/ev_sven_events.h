#ifndef EV_SVEN_EVENTS_H
#define EV_SVEN_EVENTS_H

#include "event_args.h"

extern int tracerCount[32];
extern bool g_fEventRunned;

Vector BulletAccuracy(Vector moving, Vector standing, Vector ducked, event_args_t *args);
void CreateMuzzleFlash(Vector origin);

extern "C"
{
void EV_FireGlock1(event_args_t *args);
void EV_FireGlock2(event_args_t *args);
void EV_FireShotGunSingle(event_args_t *args);
void EV_FireShotGunDouble(event_args_t *args);
void EV_FireMP5(event_args_t *args);
void EV_FirePython(event_args_t *args);
void EV_FireDeagle(event_args_t *args);
void EV_FireGauss(event_args_t *args);
void EV_SpinGauss(event_args_t *args);
void EV_TrainPitchAdjust(event_args_t *args);
void EV_VehiclePitchAdjust(event_args_t *args);
void EV_Uzi(event_args_t *args);
void EV_UziAkimbo(event_args_t *args);
void EV_ClientsideCheckRun(event_args_t *args);
void EV_WeaponCustom(event_args_t *args);
void EV_Minigun(event_args_t *args);
void EV_MinigunSpin(event_args_t *args);
void EV_SniperRifle(event_args_t *args);
void EV_M249(event_args_t *args);
void EV_M249_LEGACY(event_args_t *args);
void EV_M16(event_args_t *args);
void EV_FireShockRifle(event_args_t *args);
void EV_FireShockRifleBeam(event_args_t *args);
void EV_DisplacerSpin(event_args_t *args);
}

#endif
