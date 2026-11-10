//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#if !defined ( EV_HLDMH )
#define EV_HLDMH

#include "weapon_animations.h"

#ifndef VECTOR_CONE_1DEGREES
#define VECTOR_CONE_1DEGREES Vector( 0.00873, 0.00873, 0.00873 )
#define VECTOR_CONE_2DEGREES Vector( 0.01745, 0.01745, 0.01745 )
#define VECTOR_CONE_3DEGREES Vector( 0.02618, 0.02618, 0.02618 )
#define VECTOR_CONE_4DEGREES Vector( 0.03490, 0.03490, 0.03490 )
#define VECTOR_CONE_5DEGREES Vector( 0.04362, 0.04362, 0.04362 )
#define VECTOR_CONE_6DEGREES Vector( 0.05234, 0.05234, 0.05234 )
#define VECTOR_CONE_7DEGREES Vector( 0.06105, 0.06105, 0.06105 )
#define VECTOR_CONE_8DEGREES Vector( 0.06976, 0.06976, 0.06976 )
#define VECTOR_CONE_9DEGREES Vector( 0.07846, 0.07846, 0.07846 )
#define VECTOR_CONE_10DEGREES Vector( 0.08716, 0.08716, 0.08716 )
#define VECTOR_CONE_15DEGREES Vector( 0.13053, 0.13053, 0.13053 )
#define VECTOR_CONE_20DEGREES Vector( 0.17365, 0.17365, 0.17365 )
#endif

// bullet types
typedef enum
{
	BULLET_NONE = 0,
	BULLET_PLAYER_9MM = 1,
	BULLET_PLAYER_MP5 = 2,
	BULLET_PLAYER_SAW = 3,
	BULLET_PLAYER_SNIPER = 4,
	BULLET_PLAYER_357 = 5,
	BULLET_PLAYER_EAGLE = 6,
	BULLET_PLAYER_BUCKSHOT = 7,
	BULLET_PLAYER_CROWBAR = 8,
	BULLET_PLAYER_CUSTOMDAMAGE = 9,
	BULLET_MONSTER_9MM = 10,
	BULLET_MONSTER_MP5 = 11,
	BULLET_MONSTER_SAW = 12,
	BULLET_MONSTER_12MM = 13,
	BULLET_MONSTER_EAGLE = 14,
	BULLET_MONSTER_SNIPER = 15,
	BULLET_MONSTER_BUCKSHOT = 16,
	DECAL_SCORCH_MARK = 17
} Bullet;

#ifndef BLOOD_COLOR_RED
#define BLOOD_COLOR_RED		247
#define BLOOD_COLOR_YELLOW	195
#endif

typedef enum
{
	GIBTYPE_HUMAN = 1,
	GIBTYPE_ALIEN = 2,
	GIBTYPE_GLASS = 5,
	GIBTYPE_STICKY = 6
} GIBTYPE;

#define HUMAN_GIB_BODIES	11	// models/hgibs.mdl, body 0 is the skull
#define ALIEN_GIB_BODIES	4	// models/agibs.mdl
#define GLASS_GIB_BODIES	4	// models/glassgibs.mdl has 8, only the first 4 are picked

// Gib message types. CGib::SpawnRandomGibs sends GIBMSG_HUMAN_RANDOM or GIBMSG_ALIEN_RANDOM
#define GIBMSG_HUMAN			0
#define GIBMSG_HUMAN_RANDOM		1
#define GIBMSG_ALIEN_RANDOM		2
#define GIBMSG_STICKY			4

enum glock_e {
	GLOCK_IDLE1 = 0,
	GLOCK_IDLE2,
	GLOCK_IDLE3,
	GLOCK_SHOOT,
	GLOCK_SHOOT_EMPTY,
	GLOCK_RELOAD,
	GLOCK_RELOAD_NOT_EMPTY,
	GLOCK_DRAW,
	GLOCK_HOLSTER,
	GLOCK_ADD_SILENCER
};

enum shotgun_e {
	SHOTGUN_IDLE = 0,
	SHOTGUN_FIRE,
	SHOTGUN_FIRE2,
	SHOTGUN_RELOAD,
	SHOTGUN_PUMP,
	SHOTGUN_START_RELOAD,
	SHOTGUN_DRAW,
	SHOTGUN_HOLSTER,
	SHOTGUN_IDLE4,
	SHOTGUN_IDLE_DEEP,
	SHOTGUN_FIRE_DOUBLE
};

enum mp5_e
{
	MP5_LONGIDLE = 0,
	MP5_IDLE1,
	MP5_LAUNCH,
	MP5_RELOAD,
	MP5_DEPLOY,
	MP5_FIRE1,
	MP5_FIRE2,
	MP5_FIRE3,
};

enum python_e {
	PYTHON_IDLE1 = 0,
	PYTHON_FIDGET,
	PYTHON_FIRE1,
	PYTHON_RELOAD,
	PYTHON_HOLSTER,
	PYTHON_DRAW,
	PYTHON_IDLE2,
	PYTHON_IDLE3
};

#define	GAUSS_PRIMARY_CHARGE_VOLUME	256// how loud gauss is while charging
#define GAUSS_PRIMARY_FIRE_VOLUME	450// how loud gauss is when discharged

enum gauss_e {
	GAUSS_IDLE = 0,
	GAUSS_IDLE2,
	GAUSS_FIDGET,
	GAUSS_SPINUP,
	GAUSS_SPIN,
	GAUSS_FIRE,
	GAUSS_FIRE2,
	GAUSS_HOLSTER,
	GAUSS_DRAW
};

enum sniperrifle_e {
	SNIPERRIFLE_DRAW = 0,
	SNIPERRIFLE_SLOWIDLE,
	SNIPERRIFLE_FIRE,
	SNIPERRIFLE_FIRELASTROUND,
	SNIPERRIFLE_RELOAD1,
	SNIPERRIFLE_RELOAD2,
	SNIPERRIFLE_RELOAD3,
	SNIPERRIFLE_SLOWIDLE2,
	SNIPERRIFLE_HOLSTER
};

enum m249_e {
	M249_SLOWIDLE = 0,
	M249_IDLE2,
	M249_RELOAD_START,
	M249_RELOAD_END,
	M249_HOLSTER,
	M249_DRAW,
	M249_SHOOT1,
	M249_SHOOT2,
	M249_SHOOT3
};

enum m16_e {
	M16_DRAW = 0,
	M16_HOLSTER,
	M16_IDLE,
	M16_FIDGET,
	M16_SHOOT_1,
	M16_SHOOT_2,
	M16_RELOAD_M16,
	M16_LAUNCH,
	M16_RELOAD_M203
};

enum eagle_e {
	EAGLE_IDLE1 = 0,
	EAGLE_IDLE2,
	EAGLE_IDLE3,
	EAGLE_IDLE4,
	EAGLE_IDLE5,
	EAGLE_SHOOT,
	EAGLE_SHOOT_EMPTY,
	EAGLE_RELOAD,
	EAGLE_RELOAD_NOSHOT,
	EAGLE_DRAW,
	EAGLE_HOLSTER
};

enum shockrifle_e {
	SHOCKRIFLE_IDLE1 = 0,
	SHOCKRIFLE_FIRE,
	SHOCKRIFLE_DRAW,
	SHOCKRIFLE_HOLSTER,
	SHOCKRIFLE_IDLE3
};

enum flamethrower_e {
	FLAMETHROWER_IDLE = 0,
	FLAMETHROWER_FIDGET,
	FLAMETHROWER_ALTFIRE1,
	FLAMETHROWER_ALTFIRE2,
	FLAMETHROWER_ALTFIRE3,
	FLAMETHROWER_FIRE1,
	FLAMETHROWER_FIRE2,
	FLAMETHROWER_FIRE3,
	FLAMETHROWER_FIRE4,
	FLAMETHROWER_DRAW,
	FLAMETHROWER_HOLSTER
};

struct tempent_s *EV_Gib(GIBTYPE type, int bloodColor, float *origin, float *velocity, float life, int body);
void EV_TE_GibCallback(struct tempent_s *ent, float frametime, float currenttime);
void EV_TE_GibHitCallback(struct tempent_s *ent, pmtrace_t *trace);
void EV_TE_StickyGibHitCallback(struct tempent_s *ent, pmtrace_t *trace);
int EV_Message_Gib(const char *pszName, int iSize, void *pbuf);
void EV_HLDM_DecalBlood(pmtrace_t *trace, int bloodColor);
char *EV_HLDM_DamageDecal(struct physent_s *entity);
int EV_IsAllyTeam(int iTeam1, int iTeam2);
float EV_HLDM_PlayTextureSound(int idx, pmtrace_t *trace, float *src, float *end, int bulletType);
void EV_HLDM_GunshotDecalTrace(pmtrace_t *trace, char *decalName);
void EV_HLDM_DecalGunshot(pmtrace_t *trace, int bulletType = 0);
int EV_HLDM_CheckTracer(int idx, float *src, float *end, float *forward, float *right,
	int bulletType, int tracerFrequency, int *count);
void EV_HLDM_WaterSplash(const Vector &origin, const Vector &direction, bool soundOnly);
void EV_HLDM_FireBullets(int idx, float *forward, float *right, float *up, int shots,
	float *src, float *aim, float *spread, float distance, int bulletType,
	int tracerFrequency, int *count);
void EV_StandardFireBullets(int idx, float *forward, float *right, float *up, int shots,
	float *src, float *aim, float *spread, float distance, int bulletType, int tracerFrequency);
void TE_FireBullet(Vector vecSrc, Vector vecDir, int iShots, Vector vecSpread, int iTracerFreq, int iEntity);
int __MsgFunc_TracerDecal(const char *pszName, int iSize, void *pbuf);

#endif // EV_HLDMH