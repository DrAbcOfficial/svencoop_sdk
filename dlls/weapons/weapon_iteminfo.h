#ifndef WEAPON_ITEMINFO_H
#define WEAPON_ITEMINFO_H

typedef struct
{
	int			iSlot;
	int			iPosition;
	const char	*pszAmmo1;			// ammo 1 type
	int			iMaxAmmo1;			// max ammo 1
	const char	*pszAmmo1Entity;	// entity a dropped weapon leaves behind for ammo 1
	int			iAmmo1Drop;			// amount of dropped primary ammo
	const char	*pszAmmo2;			// ammo 2 type
	int			iMaxAmmo2;			// max ammo 2
	const char	*pszAmmo2Entity;	// entity a dropped weapon leaves behind for ammo 2
	int			iAmmo2Drop;			// amount of dropped secondary ammo
	const char	*pszName;
	int			iMaxClip;
	int			iId;
	int			iFlags;
	int			iWeight;			// importance in weapon autoselection
} ItemInfo;

typedef struct
{
	const char	*pszName;
	int			iId;
	int			iMaxCarry;
	const char	*pszWeapon;	// weapon granted with exhaustible ammo
} AmmoInfo;

#endif // WEAPON_ITEMINFO_H
