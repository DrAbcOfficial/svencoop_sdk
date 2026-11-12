#ifndef WORLDMODELS_H
#define WORLDMODELS_H

enum WEAPONMDLIDX
{
	WEAPONMDL_PLAYER = 0,
	WEAPONMDL_WORLD,
	WEAPONMDL_VIEW,
};

struct StandardWeaponModels
{
	char view[64];
	char world[64];
	char player[64];
};

extern StandardWeaponModels g_pStandardWeaponModels[256];
extern const char *gszNoCustomMdl;

const char *GetWeaponStandardModel( WEAPONMDLIDX modelType, int weaponId, const char *defaultModel );

#endif
